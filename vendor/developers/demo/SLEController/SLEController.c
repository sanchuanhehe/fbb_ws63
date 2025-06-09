/**
 * 项目描述：
 * SLEController 是一款基于 Hispark ws63 开发板的多任务低延迟无线控制器，
 * 支持摇杆输入采集、OLED 实时显示、SLE 协议无线通信，适用于远程设备/无人机等场景。
 *
 * Project Description:
 * SLEController is a multi-tasking, low-latency wireless controller
 * based on the Hispark ws63 development board.
 * It supports joystick input acquisition, real-time OLED display,
 * and SLE protocol wireless communication, suitable for remote device/drone control scenarios.
 *
 * Copyright (c) lamonce
 *
 * History:
 * 2025-06-09, Edit file.
 */

#include "soc_osal.h"
#include "pinctrl.h"
#include "adc.h"
#include "adc_porting.h"
#include "common_def.h"
#include "app_init.h"
#include "tcxo.h"
#include "string.h"
#include "osal_wait.h"
#include "gpio.h"
#include "uart.h"         // 串口控制
#include "SSD1306_OLED.h" // OLED 驱动
#include "securec.h"      // 安全函数库，用于替代标准 C 库中不安全的函数

#include "sle_low_latency.h"                     // 低延迟通信框架
#include "sle_uart_server/sle_uart_server.h"     // Server 端头文件
#include "sle_uart_server/sle_uart_server_adv.h" // Server 端广播相关头文件
#include "sle_device_discovery.h"                // 设备发现相关头文件
#include "sle_errcode.h"                         // 错误码定义

#define TASK_STACK_SIZE 0x4000  // Task 堆栈大小
#define TASK_PRIO 24            // Task 优先级
#define OLED_I2C_ADDR 0x3C      // OLED I2C 地址
#define I2C_SET_BAUDRATE 400000 // I2C 波特率 400KHz
#define I2C_MASTER_ADDR 0x0     // I2C 高速模式主机地址
#define I2C_SCL_PIN 16          // I2C SCL 引脚
#define I2C_SDA_PIN 15          // I2C SDA 引脚
#define I2C_BUS_ID I2C_BUS_1    // I2C 总线ID
#define I2C_TRANSFER_LEN 8      // I2C 传输长度
#define ADC_DELAY_TIME 100      // 延时

#define QUEUE_SIZE 12                                                                  // 定义队列大小
#define JOYSTICK_QUEUE_NODE_SIZE (sizeof(JoystickData) / sizeof(uint8_t))              // 定义摇杆数据队列节点大小，单位字节
#define JOYSTICK_PERCENT_QUEUE_NODE_SIZE (sizeof(SLESendDataStruct) / sizeof(uint8_t)) // 定义百分比队列节点大小，单位字节
#define BAT_QUEUE_NODE_SIZE (sizeof(uint32_t) / sizeof(uint8_t))                       // 定义电池电压队列节点大小，单位字节
#define KEY3_PIN 3                                                                     // 定义按钮引脚号
#define KEY6_PIN 6

// ADC通道
#define ADC_CHANNEL0 0 // GPIO7
#define ADC_CHANNEL1 1 // GPIO8
#define ADC_CHANNEL2 2 // GPIO9
#define ADC_CHANNEL3 3 // GPIO10
// #define ADC_CHANNEL5 4 // GPIO11
#define ADC_CHANNEL4 5 // GPIO12
// 注意电池的 GPIO 不能是 11，否则会因为启动时 GPIO11 上拉导致启动失败
#define DATA_CALIBRATION 5 // 数据校准百分比

// SLE Server 端相关宏定义
#define SLE_UART_SERVER_DELAY_COUNT 5          // Server 端延时计数
#define SLE_UART_TASK_STACK_SIZE 0x1200        // 任务栈大小
#define SLE_ADV_HANDLE_DEFAULT 1               // 广播句柄
#define SLE_UART_SERVER_MSG_QUEUE_LEN 5        // 消息队列长度
#define SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE 32  // 消息节点大小
#define SLE_UART_SERVER_QUEUE_DELAY 0xFFFFFFFF // 消息队列延时，此处设置为最大值
#define SLE_UART_SERVER_BUFF_MAX_SIZE 800      // SLE UART Server 端缓冲区最大大小
#define SLE_UART_TASK_PRIO 28                  // SLE 任务优先级
#define SLE_UART_TASK_DURATION_MS 2000         // SLE 任务休眠时间
#define NAME_MAX_LENGTH 16                     // SLE 广播名称最大长度

typedef struct
{
    uint16_t adc_ch0; // 摇杆1 Y轴
    uint16_t adc_ch1; // 摇杆1 X轴
    uint16_t adc_ch2; // 摇杆2 Y轴
    uint16_t adc_ch3; // 摇杆2 X轴
} JoystickData;

typedef struct
{
    uint32_t adc_ch0_percent; // 摇杆1 Y轴百分比
    uint32_t adc_ch1_percent; // 摇杆1 X轴百分比
    uint32_t adc_ch2_percent; // 摇杆2 Y轴百分比
    uint32_t adc_ch3_percent; // 摇杆2 X轴百分比
    uint8_t is_test_mode;     // 是否为测试模式
} SLESendDataStruct;

unsigned long g_sle_uart_server_msgqueue_id;                 // 消息队列句柄
#define SLE_UART_SERVER_LOG "[sle uart server]"              // 日志前缀
static uint8_t sle_local_name[NAME_MAX_LENGTH] = "NearLink"; // 设备本地名称，用于 SLE 广播和连接

static unsigned long joystick_data_queueID = 0;   // 摇杆数据队列ID
static unsigned long SLE_Transfer_QueueID = 0;    // SLE 传输队列ID
static unsigned long battery_voltage_queueID = 0; // 电池电压队列ID

static osal_mutex g_mux_id;            // 互斥锁，用于保证 OLED 初始化和 ADC 初始化的顺序
static uint8_t OLED_display_index = 0; // OLED 显示索引
static bool oled_init_flag = false;    // OLED 初始化标志
static uint8_t test_flag = 0;          // 无人机测试模式

static bool joystick_ready_index = false;        // 摇杆标志
static uint16_t joystick_adc_default_value0 = 0; // ADC 最大值保存
static uint16_t joystick_adc_default_value1 = 0;
static uint16_t joystick_adc_default_value2 = 0;
static uint16_t joystick_adc_default_value3 = 0;

/**
 * @brief Button interrupt service routine for KEY3.
 *        When the joystick is in calibration state, pressing KEY3 sets the calibration flag to true.
 *        Afterwards, pressing KEY3 toggles the OLED display content.
 *
 * @brief 按钮 KEY3 的中断服务函数。
 *        当摇杆处于校准状态时，按下 KEY3 会将校准标志设为 true。
 *        之后再按 KEY3 会切换 OLED 显示内容。
 */
static void ButtonISR_KEY3(void)
{
    if (joystick_ready_index == false) // 摇杆校准标记
    {
        joystick_ready_index = true;
        osal_printk("Button pressed, joystick ready!\r\n");
    }
    else // 切换 OLED 显示
    {
        OLED_display_index++;
        OLED_display_index %= 3; // 切换 OLED 显示索引，最大为 （3 -1）
        oled_init_flag = false;
    }
}

static void ButtonISR_KEY6(void)
{
    osal_printk("Button KEY6 pressed!\r\n");
    test_flag++;
    test_flag %= 3; // 切换测试模式标志，最大为 （3 -1）
}

/**
 * @brief GPIO pin initialization function.
 *
 * @brief GPIO 引脚初始化函数。
 */
static void pin_init(void)
{
    uapi_pin_set_mode(I2C_SCL_PIN, PIN_MODE_2);
    uapi_pin_set_mode(I2C_SDA_PIN, PIN_MODE_2);
    uapi_pin_set_mode(KEY3_PIN, PIN_MODE_0);
    uapi_gpio_set_dir(KEY3_PIN, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(KEY3_PIN, GPIO_INTERRUPT_RISING_EDGE, (gpio_callback_t)ButtonISR_KEY3); // 注册按钮中断服务函数
    uapi_pin_set_mode(KEY6_PIN, PIN_MODE_0);
    uapi_gpio_set_dir(KEY6_PIN, GPIO_DIRECTION_INPUT);
    if (ret != 0)
    {
        uapi_gpio_unregister_isr_func(KEY3_PIN);
    }
    else
    {
        osal_printk("Button ISR registered successfully!\r\n");
    }
}

/**
 * @brief Read request callback for SLE UART server.
 *
 * @param server_id SLE Server ID
 * @param conn_id Connection ID
 * @param read_cb_para Read request callback parameter
 * @param status Status
 *
 * @brief SLE UART Server 的读请求回调函数。
 *
 * @param server_id SLE Server ID
 * @param conn_id 连接 ID
 * @param read_cb_para 读请求回调参数
 * @param status 状态
 */
static void ssaps_server_read_request_callback(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                               errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_UART_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}

/**
 * @brief Write request callback for SLE UART server.
 *
 * @param server_id SLE Server ID
 * @param conn_id Connection ID
 * @param write_cb_para Write request callback parameter
 * @param status Status
 *
 * @brief SLE UART Server 的写请求回调函数。
 *
 * @param server_id SLE Server ID
 * @param conn_id 连接 ID
 * @param write_cb_para 写请求回调参数
 * @param status 状态
 */
static void ssaps_server_write_request_callback(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                                errcode_t status)
{
    osal_printk("%s ssaps write request callback cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_UART_SERVER_LOG, server_id, conn_id, write_cb_para->handle, status);

    // 判断写入数据的长度和内容
    if ((write_cb_para->length > 0) && write_cb_para->value)
    {
        // 打印接收的数据
        osal_printk("\n sle uart received data : %s\r\n", write_cb_para->value);
    }
}

/**
 * @brief Create SLE UART server message queue.
 *
 * @brief 创建 SLE UART Server 端队列。
 */
static void sle_uart_server_create_msgqueue(void)
{
    if (osal_msg_queue_create("sle_uart_server_msgqueue",                          // 队列名称，保留
                              SLE_UART_SERVER_MSG_QUEUE_LEN,                       // 队列长度
                              (unsigned long *)&g_sle_uart_server_msgqueue_id,     // 成功创建的队列控制结构的 ID
                              0,                                                   // 队列模式，保留
                              SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE) != OSAL_SUCCESS) // 消息节点大小
    {
        osal_printk("^%s sle_uart_server_create_msgqueue message queue create failed!\n", SLE_UART_SERVER_LOG);
    }
}

/**
 * @brief Delete SLE UART server message queue.
 *
 * @brief 删除 SLE UART Server 端队列。
 */
static void sle_uart_server_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_sle_uart_server_msgqueue_id);
}

/**
 * @brief Write data to SLE UART server message queue.
 *
 * @param buffer_addr Data buffer address
 * @param buffer_size Data buffer size
 *
 * @brief 写入数据到 SLE UART Server 端队列。
 *
 * @param buffer_addr 数据缓冲区地址
 * @param buffer_size 数据缓冲区大小
 */
static void sle_uart_server_write_msgqueue(uint8_t *buffer_addr, uint16_t buffer_size)
{
    osal_msg_queue_write_copy(g_sle_uart_server_msgqueue_id, (void *)buffer_addr,
                              (uint32_t)buffer_size, 0);
}

/**
 * @brief Receive message from SLE UART server message queue.
 *
 * @param buffer_addr Data buffer address
 * @param buffer_size Data buffer size
 * @return int32_t
 *
 * @brief 从 SLE UART Server 端队列接收消息。
 *
 * @param buffer_addr 数据缓冲区地址
 * @param buffer_size 数据缓冲区大小
 * @return int32_t
 */
static int32_t sle_uart_server_receive_msgqueue(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    return osal_msg_queue_read_copy(g_sle_uart_server_msgqueue_id, (void *)buffer_addr,
                                    buffer_size, SLE_UART_SERVER_QUEUE_DELAY);
}

/**
 * @brief Initialize receive buffer.
 *
 * @param buffer_addr Data buffer address
 * @param buffer_size Data buffer size
 *
 * @details Actually clears the buffer (sets all to 0).
 *
 * @brief 初始化接收缓冲区。
 *
 * @param buffer_addr 数据缓冲区地址
 * @param buffer_size 数据缓冲区大小
 *
 * @details 实际操作为清空缓冲区（全部置为 0）。
 */
static void sle_uart_server_rx_buf_init(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    *buffer_size = SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE;
    (void)memset_s(buffer_addr, *buffer_size, 0, *buffer_size);
}

/**
 * @brief OLED display task.
 *        This task initializes the OLED display and updates its content in real time based on joystick input and battery voltage.
 *        Task flow:
 *        1. Acquire mutex to ensure OLED and ADC initialization order
 *        2. Configure and initialize I2C bus
 *        3. Initialize OLED display
 *        4. Show initialization info and wait for other tasks to be ready
 *        5. Enter main loop, get joystick and battery data from ADC task
 *        6. Switch display content based on OLED display index
 *
 * @brief OLED 显示任务。
 *        该任务负责初始化 OLED 显示屏，并根据摇杆输入和电池电压实时更新显示内容。
 *        任务流程如下：
 *        1. 获取互斥锁，确保 OLED 和 ADC 初始化顺序
 *        2. 配置并初始化 I2C 总线
 *        3. 初始化 OLED 显示屏
 *        4. 显示初始化信息，等待其它任务准备就绪
 *        5. 进入主循环，获取 ADC 任务发送的摇杆数据和电池电压数据
 *        6. 根据 OLED 显示索引切换显示内容
 */
static void *OLEDDisplayTask(void)
{
    osal_printk("OLED_Display Task Start\r\n");

    uint32_t mutex_ret = 0;
    mutex_ret = osal_mutex_lock(&g_mux_id); // 获取互斥锁
    if (mutex_ret == OSAL_SUCCESS)
    {
        osal_printk("Mutex locked successfully, starting OLED initialization!\r\n");
    }
    else
    {
        osal_printk("Failed to lock mutex, error code: %d\r\n", mutex_ret);
        return NULL;
    }

    // 配置并初始化 I2C 总线
    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
    uint16_t dev_addr = OLED_I2C_ADDR;
    if (uapi_i2c_master_init(I2C_BUS_ID, baudrate, hscode) == ERRCODE_SUCC)
    {
        osal_printk("I2C master init success!\r\n");
    }
    else
    {
        osal_printk("I2C master init failed!\r\n");
        return NULL;
    }

    // 配置 I2C 引脚和缓冲区
    i2c_data_t data = {0};
    uint8_t tx_buff[I2C_TRANSFER_LEN] = {0};
    uint8_t rx_buff[I2C_TRANSFER_LEN] = {0};
    data.send_buf = tx_buff;
    data.send_len = I2C_TRANSFER_LEN;
    data.receive_buf = rx_buff;
    data.receive_len = I2C_TRANSFER_LEN;

    SSD1306_OLED_Init(I2C_BUS_ID, dev_addr, &data); // 初始化OLED

    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "System"); // 在第 1 行第 1 列显示 "System"
    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 2, 1, "initializing..."); // 在第 2 行第 1 列显示 "initializing..."
    osal_msleep(2000);            // 等待 2000 ms，等待其他任务准备就绪
    osal_mutex_unlock(&g_mux_id); // 释放互斥锁
    osal_printk("Mutex unlocked, OLED initialization complete!\r\n");

    uint32_t ret = 0;
    uint32_t ret1 = 0;
    JoystickData joystick_data = {0}; // 摇杆数据
    uint32_t battery_voltage = 0;     // 电池电压
    uint32_t joystick_msgSize = sizeof(joystick_data);
    uint32_t battery_msgSize = sizeof(battery_voltage);
    JoystickData joystick_rcv_data = {0};

    SSD1306_OLED_Clear(I2C_BUS_ID, dev_addr, &data);                             // 清屏
    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "Joystick Test"); // 在第 1 行第 1 列显示 "Joystick Test"
    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 2, 1, "X1:     X2:    "); // 在第 2 行第 1 列显示 "X1:     X2:    "
    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 3, 1, "Y1:     Y2:    "); // 在第 3 行第 1 列显示 "Y1:     Y2:    "
    SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 4, 1, "KEY3 TO ENTER"); // 在第 4 行第 1 列显示 "KEY3 TO ENTER"

    while (!joystick_ready_index)
    {
        ret = osal_msg_queue_read_copy(joystick_data_queueID, &joystick_rcv_data, &joystick_msgSize, OSAL_WAIT_FOREVER); // 注意这里的 msgSize 一定要是指针
        ret1 = osal_msg_queue_read_copy(battery_voltage_queueID, &battery_voltage, &battery_msgSize, OSAL_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS)
        {
            osal_printk("recv message failed, ret = %d\n", ret);
        }
        

        SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 3, 4, joystick_rcv_data.adc_ch0, 4, false); // 在第 3 行第 4 列显示摇杆1 Y轴数据，长度为 4
        SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 4, joystick_rcv_data.adc_ch1, 4, false); // 在第 2 行第 4 列显示摇杆1 X轴数据，长度为 4
        SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 3, 12, joystick_rcv_data.adc_ch2, 4, false); // 在第 3 行第 12 列显示摇杆2 Y轴数据，长度为 4
        SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 12, joystick_rcv_data.adc_ch3, 4, false); // 在第 2 行第 12 列显示摇杆2 X轴数据，长度为 4

        // 更新最大 ADC 值
        if (joystick_rcv_data.adc_ch0 > joystick_adc_default_value0)
        {
            joystick_adc_default_value0 = joystick_rcv_data.adc_ch0;
        }
        if (joystick_rcv_data.adc_ch1 > joystick_adc_default_value1)
        {
            joystick_adc_default_value1 = joystick_rcv_data.adc_ch1;
        }
        if (joystick_rcv_data.adc_ch2 > joystick_adc_default_value2)
        {
            joystick_adc_default_value2 = joystick_rcv_data.adc_ch2;
        }
        if (joystick_rcv_data.adc_ch3 > joystick_adc_default_value3)
        {
            joystick_adc_default_value3 = joystick_rcv_data.adc_ch3;
        }
    }

    errcode_t ret_key6 = uapi_gpio_register_isr_func(KEY6_PIN, GPIO_INTERRUPT_RISING_EDGE, (gpio_callback_t)ButtonISR_KEY6);
    if (ret_key6 != 0)
    {
        uapi_gpio_unregister_isr_func(KEY6_PIN);
        osal_printk("Button KEY6 ISR register failed!\r\n");
    }
    // else
    // {
    //     osal_printk("Button KEY6 ISR register success!\r\n");
    // }

    while (true)
    {
        ret = osal_msg_queue_read_copy(joystick_data_queueID, &joystick_rcv_data, &joystick_msgSize, OSAL_WAIT_FOREVER); // 注意这里的 msgSize 一定要是指针
        ret1 = osal_msg_queue_read_copy(battery_voltage_queueID, &battery_voltage, &battery_msgSize, OSAL_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS || ret1 != OSAL_SUCCESS)
        {
            osal_printk("recv message failed, ret = %d\n, ret1 = %d", ret, ret1);
        }

        if (OLED_display_index == 0) // OLED_display_index == 0 时显示摇杆数据
        {
            if (!oled_init_flag) // 如果固定显示的内容没有初始化过
            {
                SSD1306_OLED_Clear(I2C_BUS_ID, dev_addr, &data);                        // 清屏
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "ADC Data"); // 在第 1 行第 1 列显示 "ADC Data"
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 2, 1, "X1:   % X2:   %"); // 在第 2 行第 1 列显示 "X1:   % X2:   %"
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 3, 1, "Y1:   % Y2:   %"); // 在第 3 行第 1 列显示 "Y1:   % Y2:   %"
                oled_init_flag = true;
            }

            double voltage_percent0 = (double)joystick_rcv_data.adc_ch0 / joystick_adc_default_value0 * 100; // 计算电压百分比
            double voltage_percent1 = (double)joystick_rcv_data.adc_ch1 / joystick_adc_default_value1 * 100;
            double voltage_percent2 = (double)joystick_rcv_data.adc_ch2 / joystick_adc_default_value2 * 100;
            double voltage_percent3 = (double)joystick_rcv_data.adc_ch3 / joystick_adc_default_value3 * 100;
            if (voltage_percent0 < (50 + DATA_CALIBRATION) && voltage_percent0 > (50 - DATA_CALIBRATION)) // 如果电压百分比在 50% ± DATA_CALIBRATION 范围内，则设置为 50%
            {
                voltage_percent0 = 50.0; // 设置为 50%
            }
            if (voltage_percent1 < (50 + DATA_CALIBRATION) && voltage_percent1 > (50 - DATA_CALIBRATION)) // 如果电压百分比在 50% ± DATA_CALIBRATION 范围内，则设置为 50%
            {
                voltage_percent1 = 50.0; // 设置为 50%
            }
            if (voltage_percent2 < (50 + DATA_CALIBRATION) && voltage_percent2 > (50 - DATA_CALIBRATION)) // 如果电压百分比在 50% ± DATA_CALIBRATION 范围内，则设置为 50%
            {
                voltage_percent2 = 50.0; // 设置为 50%
            }
            if (voltage_percent3 < (50 + DATA_CALIBRATION) && voltage_percent3 > (50 - DATA_CALIBRATION)) // 如果电压百分比在 50% ± DATA_CALIBRATION 范围内，则设置为 50%
            {
                voltage_percent3 = 50.0; // 设置为 50%
            }

            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 3, 4, (int)voltage_percent0, 3, false); // 在第 3 行第 4 列显示电压百分比
            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 4, (int)voltage_percent1, 3, false); // 在第 2 行第 4 列显示电压百分比
            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 3, 12, (int)voltage_percent2, 3, false); // 在第 3 行第 12 列显示电压百分比
            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 12, (int)voltage_percent3, 3, false); // 在第 2 行第 12 列显示电压百分比
        }

        else if (OLED_display_index == 1) // OLED_display_index == 1 时显示 SLE 相关信息、电池电压等
        {
            if (!oled_init_flag)
            {
                SSD1306_OLED_Clear(I2C_BUS_ID, dev_addr, &data);
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 3, 1, "SLE Name: "); // 在第 3 行第 1 列显示 "SLE Name: "
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 4, 1, (char *)sle_local_name); // 在第 4 行第 1 列显示 SLE 本地名称
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 2, 1, "Battery:     mv"); // 在第 2 行第 1 列显示 "Battery:     mv"
                oled_init_flag = true;
            }

            // 判断 SLE 连接状态
            if (sle_uart_client_is_connected())
            {
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "SLE Connected   "); // 在第 1 行第 1 列显示 "SLE Connected   "
            }
            else
            {
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "SLE Disconnected"); // 在第 1 行第 1 列显示 "SLE Disconnected"
            }

            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 9, (int)battery_voltage, 4, false); // 在第 2 行第 9 列显示电池电压，长度为 4
        }
        else if (OLED_display_index == 2) // 当 OLED_display_index == 2 时显示测试模式标志
        {
            if (!oled_init_flag)
            {
                SSD1306_OLED_Clear(I2C_BUS_ID, dev_addr, &data);
                SSD1306_OLED_ShowString(I2C_BUS_ID, dev_addr, &data, 1, 1, "Drone Test Mode"); // 在第 1 行第 1 列显示 "Drone Test Mode"
                oled_init_flag = true;
            }

            SSD1306_OLED_ShowIntNum(I2C_BUS_ID, dev_addr, &data, 2, 1, test_flag, 1, false); // 在第 2 行第 1 列显示测试模式标志
        }
    }
    return NULL;
}

/**
 * @brief ADC input task.
 *        This task reads joystick and battery voltage data from ADC ports and sends them to message queues.
 *        Task flow:
 *        1. Initialize ADC and set ADC clock
 *        2. Acquire mutex to ensure ADC and OLED initialization order
 *        3. Enter main loop, periodically read ADC data
 *        4. Write joystick and battery data to message queues
 *        5. Calculate joystick data percentage and write to SLE transfer queue
 *
 * @brief ADC 输入任务。
 *        该任务负责从 ADC 端口读取摇杆数据和电池电压数据，并将其发送到消息队列中。
 *        任务流程如下：
 *        1. 初始化 ADC，设置 ADC 时钟
 *        2. 获取互斥锁，确保 ADC 和 OLED 初始化顺序
 *        3. 进入主循环，定期读取 ADC 数据
 *        4. 将读取到的摇杆数据和电池电压数据写入消息队列
 *        5. 计算摇杆数据的百分比，并将其写入 SLE 传输消息队列
 */
static void *JoystickInputTask(void)
{
    osal_printk("JoystickInputTask start\r\n");
    uapi_adc_init(ADC_CLOCK_NONE); // 初始化ADC

    uint8_t adc_channel0 = ADC_CHANNEL0; // ADC通道
    uint8_t adc_channel1 = ADC_CHANNEL1;
    uint8_t adc_channel2 = ADC_CHANNEL2;
    uint8_t adc_channel3 = ADC_CHANNEL3;
    uint8_t adc_channel4 = ADC_CHANNEL4; // 电池电压通道
    uint32_t ret1 = 0;
    uint32_t ret2 = 0;
    uint32_t ret3 = 0;
    uint32_t mutex_ret = 0;
    JoystickData joystick_data = {0};      // 摇杆数据
    SLESendDataStruct sle_send_data = {0}; // 摇杆百分比数据
    uint16_t battery_voltage = 0;          // 电池电压
    osal_msleep(1000);                                                 // 延时 1000 ms，等待 ADC 初始化完成
    mutex_ret = osal_mutex_lock_timeout(&g_mux_id, OSAL_WAIT_FOREVER); // 获取互斥锁
    if (mutex_ret != OSAL_SUCCESS)
    {

        osal_printk("Failed to lock mutex, error code: %d\r\n", mutex_ret);
    }
    else
    {

        osal_printk("ADC_task get mutex successfully!\r\n");
    }

    osal_printk("ADC start\r\n");
    while (true)
    {

        adc_port_read(adc_channel0, &joystick_data.adc_ch0);
        adc_port_read(adc_channel1, &joystick_data.adc_ch1);
        adc_port_read(adc_channel2, &joystick_data.adc_ch2);
        adc_port_read(adc_channel3, &joystick_data.adc_ch3);
        adc_port_read(adc_channel4, &battery_voltage); // 读取电池电压

        ret1 = osal_msg_queue_write_copy(joystick_data_queueID, &joystick_data, sizeof(joystick_data), 0xff); // 写入显示消息队列

        if (joystick_ready_index == true)
        {

            sle_send_data.adc_ch0_percent = (joystick_data.adc_ch0 * 100) / joystick_adc_default_value0; // 计算百分比（乘以 100 后除以最大值）
            sle_send_data.adc_ch1_percent = (joystick_data.adc_ch1 * 100) / joystick_adc_default_value1; // 计算百分比（乘以 100 后除以最大值）
            sle_send_data.adc_ch2_percent = (joystick_data.adc_ch2 * 100) / joystick_adc_default_value2; // 计算百分比（乘以 100 后除以最大值）
            sle_send_data.adc_ch3_percent = (joystick_data.adc_ch3 * 100) / joystick_adc_default_value3; // 计算百分比（乘以 100 后除以最大值）

            // 受限于 ADC 精度和摇杆的实际情况，需要对百分比进行校准，防止摇杆在中间位置时 ADC 读数不稳定导致百分比偏离 50%
            if (sle_send_data.adc_ch0_percent < (50 + DATA_CALIBRATION) && sle_send_data.adc_ch0_percent > (50 - DATA_CALIBRATION)) // 如果百分比在 50% ± DATA_CALIBRATION 范围内
            {

                sle_send_data.adc_ch0_percent = 50.0; // 设置为 50%
            }
            if (sle_send_data.adc_ch1_percent < (50 + DATA_CALIBRATION) && sle_send_data.adc_ch1_percent > (50 - DATA_CALIBRATION)) // 如果百分比在 50% ± DATA_CALIBRATION 范围内
            {

                sle_send_data.adc_ch1_percent = 50.0; // 设置为 50%
            }
            if (sle_send_data.adc_ch2_percent < (50 + DATA_CALIBRATION) && sle_send_data.adc_ch2_percent > (50 - DATA_CALIBRATION)) // 如果百分比在 50% ± DATA_CALIBRATION 范围内
            {

                sle_send_data.adc_ch2_percent = 50.0; // 设置为 50%
            }
            if (sle_send_data.adc_ch3_percent < (50 + DATA_CALIBRATION) && sle_send_data.adc_ch3_percent > (50 - DATA_CALIBRATION)) // 如果百分比在 50% ± DATA_CALIBRATION 范围内
            {

                sle_send_data.adc_ch3_percent = 50.0; // 设置为 50%
            }
            sle_send_data.is_test_mode = test_flag;                                                              // 设置测试模式标志
            ret2 = osal_msg_queue_write_copy(SLE_Transfer_QueueID, &sle_send_data, sizeof(sle_send_data), 0xff); // 写入 SLE 传输消息队列
        }

        uint32_t BAT_Vol = battery_voltage * 1000 * 3 / 2 / 1000; // ADC 读取的数据为三分之二分压的电池电压

        ret3 = osal_msg_queue_write_copy(battery_voltage_queueID, &BAT_Vol, sizeof(BAT_Vol), 0xff); // 写入电池电压消息队列
        if (ret1 != OSAL_SUCCESS || ret2 != OSAL_SUCCESS || ret3 != OSAL_SUCCESS)
        {

            osal_printk("send message failed, ret1 = %d, ret2 = %d\n, ret3 = %d\n", ret1, ret2, ret3);
        }

        osal_msleep(ADC_DELAY_TIME);
    }

    uapi_adc_deinit();
    return NULL;
}

/**
 * @brief SLE UART server task.
 *        This task initializes the SLE UART server and handles received and sent messages.
 *        Task flow:
 *        1. Initialize receive buffer and connection state
 *        2. Create SLE server message queue
 *        3. Register message queue and callback functions
 *        4. Initialize SLE server
 *        5. Enter main loop, start advertising if not connected
 *        6. Periodically check connection state, continue advertising if not connected
 *
 * @brief SLE UART Server 端任务。
 *        该任务负责初始化 SLE UART Server 端，并处理接收和发送的消息。
 *        任务流程如下：
 *        1. 初始化接收缓冲区和连接状态
 *        2. 创建 SLE Server 端消息队列
 *        3. 注册消息队列和回调函数
 *        4. 初始化 SLE Server 端
 *        5. 进入主循环，若接收到的消息为未连接状态，则开始广播
 *        6. 定期检查连接状态，若未连接则继续广播
 */
static void *sle_uart_server_task(const char *arg)
{
    unused(arg);
    osal_printk("%s sle_uart_server_task start\r\n", SLE_UART_SERVER_LOG);

    int set_name_ret = set_SLE_local_name(sle_local_name); // 设置 SLE 本地名称
    if (set_name_ret != ERRCODE_SLE_SUCCESS)
    {
        osal_printk("%s set SLE local name failed, ret = %d\r\n", SLE_UART_SERVER_LOG, set_name_ret);
        return NULL;
    }
    else
    {
        osal_printk("%s set SLE local name success, name = %s\r\n", SLE_UART_SERVER_LOG, sle_local_name);
    }

    uint8_t rx_buf[SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE] = {0}; // 定义接收缓冲区
    uint32_t rx_length = SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE;  // 接收缓冲区长度
    uint8_t sle_connect_state[] = "sle_dis_connect";          // 连接状态

    sle_uart_server_create_msgqueue();                                                             // 创建消息队列
    sle_uart_server_register_msg(sle_uart_server_write_msgqueue);                                  // 注册消息队列
    sle_uart_server_init(ssaps_server_read_request_callback, ssaps_server_write_request_callback); // 初始化 Server 端
    // 这是一个高度抽象的函数，将 SLE Server 的多种回调函数的注册、启动广播等操作进行集成

    osal_printk("%s sle_uart_server_task init success\r\n", SLE_UART_SERVER_LOG);

    errcode_t ret = 0;

    while (1)
    {

        // 清空接收缓冲区
        sle_uart_server_rx_buf_init(rx_buf, &rx_length);

        // 接收数据
        sle_uart_server_receive_msgqueue(rx_buf, &rx_length);

        // 检查接收数据是否为定义的未连接状态，若未连接，则开始广播
        if (strncmp((const char *)rx_buf, (const char *)sle_connect_state, sizeof(sle_connect_state)) == 0)
        {
            ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
            if (ret != ERRCODE_SLE_SUCCESS)
            {
                osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce fail :%02x\r\n",
                            SLE_UART_SERVER_LOG, ret);
            }
        }
        osal_msleep(SLE_UART_TASK_DURATION_MS); // 休眠一段时间，然后再次检查连接状态
    }
    // 删除消息队列
    sle_uart_server_delete_msgqueue();
    return NULL;
}

/**
 * @brief SLE send task.
 *        This task reads joystick percentage data from the SLE transfer queue and sends it to the client.
 *        Task flow:
 *        1. Initialize joystick percentage data and message size
 *        2. Enter loop, read data from SLE transfer queue
 *        3. Check if client is connected, send data if connected
 *        4. Print log if not connected
 *
 * @brief SLE 发送任务。
 *        该任务负责从 SLE 传输队列中读取摇杆百分比数据，并将其发送到客户端。
 *        任务流程如下：
 *        1. 初始化摇杆百分比数据和消息大小
 *        2. 进入循环，读取 SLE 传输队列中的数据
 *        3. 检查是否有客户端连接，若有则发送数据
 *        4. 若没有客户端连接，则打印日志提示
 */
static void *SELSendTask(void)
{
    osal_printk("SLE_Send Task Start\r\n");
    uint32_t msg_ret = 0;
    SLESendDataStruct sle_send_data = {0}; // 摇杆百分比数据
    uint32_t msgSize = sizeof(sle_send_data);
    char data_string[800] = {0};
    while (true)
    {
        msg_ret = osal_msg_queue_read_copy(SLE_Transfer_QueueID, &sle_send_data, &msgSize, OSAL_WAIT_FOREVER);
        if (msg_ret == OSAL_SUCCESS)
        {
            sprintf_s(data_string, sizeof(data_string),
                      "X1:%d,Y1:%d,X2:%d,Y2:%d,test_mode:%d\n",
                      sle_send_data.adc_ch1_percent,
                      sle_send_data.adc_ch0_percent,
                      sle_send_data.adc_ch3_percent,
                      sle_send_data.adc_ch2_percent,
                      sle_send_data.is_test_mode);
            osal_printk("%s send joystick data to client: %s\n", SLE_UART_SERVER_LOG, data_string);

            if (sle_uart_client_is_connected()) // 检查是否有客户端连接
            {
                // 发送数据到 Client 端
                // osal_printk("%s send joystick data to client: %s\n", SLE_UART_SERVER_LOG, data_string);
                sle_uart_server_send_report_by_handle((uint8_t *)data_string, strlen(data_string));
            }
            else
            {
                osal_printk("%s sle client is not connected! \r\n", SLE_UART_SERVER_LOG);
            }
        }
        else
        {
            osal_printk("%s recv message failed, ret = %d\n", SLE_UART_SERVER_LOG, msg_ret);
        }
    }
    return NULL;
}

/**
 * @brief System initialization function.
 *        This function performs system initialization, including pin initialization, message queue creation, mutex initialization, and task creation.
 *        Task flow:
 *        1. Initialize GPIO pins
 *        2. Create joystick, SLE transfer, and battery voltage queues
 *        3. Initialize mutex
 *        4. Create joystick input, OLED display, SLE transfer, and SLE server tasks
 *        5. Set task priorities
 *
 * @brief 系统初始化函数。
 *        该函数负责系统的初始化工作，包括引脚初始化、消息队列创建、互斥锁初始化和任务创建等。
 *        任务流程如下：
 *        1. 初始化 GPIO 引脚
 *        2. 创建摇杆数据队列、SLE 传输队列和电池电压队列
 *        3. 初始化互斥锁
 *        4. 创建摇杆输入任务、OLED 显示任务、SLE 传输任务和 SLE Server 端任务
 *        5. 设置任务优先级
 */
static void sysInit(void)
{
    pin_init(); // 初始化 GPIO 引脚

    uint32_t ret1 = 0;
    uint32_t ret2 = 0;
    uint32_t ret3 = 0;

    ret1 = osal_msg_queue_create("Joystick_Data_Queue", QUEUE_SIZE, &joystick_data_queueID, 0, JOYSTICK_QUEUE_NODE_SIZE); // 创建摇杆数据队列
    ret2 = osal_msg_queue_create("SLE_Transmit_Queue", QUEUE_SIZE, &SLE_Transfer_QueueID, 0, JOYSTICK_PERCENT_QUEUE_NODE_SIZE);
    ret3 = osal_msg_queue_create("Battery_Voltage_Queue", QUEUE_SIZE, &battery_voltage_queueID, 0, BAT_QUEUE_NODE_SIZE);
    if (ret1 == OSAL_SUCCESS && ret2 == OSAL_SUCCESS && ret3 == OSAL_SUCCESS)
    {
        osal_printk("Create message queue success.\r\n");
    }
    else
    {
        osal_printk("Create message queue failed.\r\n");
    }

    osal_mutex_init(&g_mux_id); // 初始化互斥锁

    osal_task *JoyStickInput_TaskHandle = NULL;
    osal_task *OLEDDisplay_TaskHandle = NULL;
    osal_task *SLE_Transmit_TaskHandle = NULL;
    osal_task *SLE_Send_TaskHandle = NULL;

    osal_kthread_lock();
    JoyStickInput_TaskHandle = osal_kthread_create((osal_kthread_handler)JoystickInputTask, NULL, "AdcTask", TASK_STACK_SIZE);
    OLEDDisplay_TaskHandle = osal_kthread_create((osal_kthread_handler)OLEDDisplayTask, NULL, "I2C_OLED_Task", TASK_STACK_SIZE);
    SLE_Send_TaskHandle = osal_kthread_create((osal_kthread_handler)SELSendTask, NULL, "SLE_Send_Task", TASK_STACK_SIZE);
    SLE_Transmit_TaskHandle = osal_kthread_create((osal_kthread_handler)sle_uart_server_task, 0, "SLEUartServerTask", SLE_UART_TASK_STACK_SIZE);
    if (JoyStickInput_TaskHandle != NULL && OLEDDisplay_TaskHandle != NULL && SLE_Transmit_TaskHandle != NULL && SLE_Send_TaskHandle != NULL)
    {
        osal_printk("Create task success.\r\n");
        osal_kthread_set_priority(JoyStickInput_TaskHandle, TASK_PRIO);
        osal_kthread_set_priority(OLEDDisplay_TaskHandle, TASK_PRIO);
        osal_kthread_set_priority(SLE_Transmit_TaskHandle, SLE_UART_TASK_PRIO); // 设置任务优先级
        osal_kthread_set_priority(SLE_Send_TaskHandle, TASK_PRIO);
    }
    else
    {
        osal_printk("Task Created Failed.\r\n");
        osal_printk("JoyStickInput_TaskHandle = %d, OLEDDisplay_TaskHandle = %d, SLE_Transmit_TaskHandle = %d, SLE_Send_TaskHandle = %d\r\n",
                    (int)JoyStickInput_TaskHandle, (int)OLEDDisplay_TaskHandle, (int)SLE_Transmit_TaskHandle, (int)SLE_Send_TaskHandle);
    }

    osal_kthread_unlock();
}

// 程序入口函数
app_run(sysInit);
