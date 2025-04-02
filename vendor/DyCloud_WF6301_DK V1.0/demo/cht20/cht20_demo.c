/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: I2C Sample Source. \n
 *
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "i2c.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BAUDRATE 100000
#define I2C_TASK_DURATION_MS 1000
#define I2C_READ_DELAY_MS 20 /* I2C 读操作后等待时间(ms) */
#define I2C_TRANSFER_LEN 1
#define CHT20_DATE_LEN 6
#define CHT20_DATA_SCALE 1048576.0f /* 数据量程 */
#define HUMIDITY_PERCENTAGE 100.0f  /* 湿度缩放系数 */

#define TEMP_SCALING_FACTOR 200.0f /* 温度缩放系数 */
#define TEMP_OFFSET 50.0f          /* 温度偏移量 */

#define I2C_TASK_PRIO 24
#define I2C_TASK_STACK_SIZE 0x1000

bool Parse_temperature_humidity(uint8_t *data, uint32_t receive_len, float *temperature, float *humidity)
{
    // 检查数据长度是否足够
    if (receive_len < CHT20_DATE_LEN) {
        return false; // 数据长度不足，无法解析
    }

    // 检查状态字节的Bit[7]是否为0
    if ((data[0] & 0x80) != 0) {
        return false; // 转换未完成，无法解析
    }

    // 提取湿度的20个bit
    uint32_t humidity_data = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
    // 解析湿度
    *humidity = ((float)((humidity_data)*HUMIDITY_PERCENTAGE / CHT20_DATA_SCALE)); /*  */

    // 提取温度的20个bit
    uint32_t temperature_data = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];
    // 解析温度
    *temperature = ((float)((temperature_data)*TEMP_SCALING_FACTOR / CHT20_DATA_SCALE - TEMP_OFFSET));

    return true; // 解析成功
}
/**
 * @brief 执行 CH20 传感器测量并读取结果
 * @param bus_id I2C 总线标识符
 * @param dev_addr 设备地址
 * @param data I2C 传输数据结构体指针
 * @return bool 操作结果 (true: 成功, false: 失败)
 */
bool CH20_MeasureAndRead(uint8_t bus_id, uint8_t dev_addr, i2c_data_t *data)
{
    // 发送测量指令
    if (uapi_i2c_master_write(bus_id, dev_addr, data) != ERRCODE_SUCC) {
        osal_printk("CH20 write command failed\r\n");
        return false;
    }

    osal_msleep(200); // 200:延时200ms 等待传感器完成测量

    // 读取测量结果
    if (uapi_i2c_master_read(bus_id, dev_addr, data) != ERRCODE_SUCC) {
        osal_printk("CH20 read data failed\r\n");
        return false;
    }

    return true;
}

/**
 * @brief 从 CHT20 传感器读取温湿度数据
 * @param bus_id   I2C 总线号
 * @param dev_addr 设备地址
 * @param temp     输出温度值（单位：℃）
 * @param humidity 输出湿度值（单位：%）
 * @return bool     true: 成功，false: 失败
 */
bool CHT20_Read(uint8_t bus_id, uint8_t dev_addr, float *temp, float *humidity)
{
    i2c_data_t data = {0};
    uint8_t tx_buff_measure[3] = {0xAC, 0x33, 0x00};
    uint8_t tx_buff_state[1] = {0x71};
    uint8_t rx_buff[6] = {0};

    data.send_buf = tx_buff_state;
    data.send_len = I2C_TRANSFER_LEN;

    while (uapi_i2c_master_write(bus_id, dev_addr, &data) != ERRCODE_SUCC) {
        osal_printk("No CHT20 detected\r\n");
        osal_msleep(500); // 500:延时500ms
        continue;         // i2c发送失败
    }

    osal_msleep(I2C_READ_DELAY_MS);
    data.receive_buf = rx_buff;
    data.receive_len = 2; // 2:发送两个字节长度

    // 1. 尝试读取设备状态
    if (uapi_i2c_master_read(bus_id, dev_addr, &data) != ERRCODE_SUCC) {
        osal_printk("CHT20 read error\r\n");
        return false;
    }

    // 2. 检查设备是否就绪（bit[3:4] = 0x18
    if ((data.receive_buf[0] & 0x18) != 0x18) {
        osal_printk("CHT20 not ready: %02X %02X\r\n", data.receive_buf[0], data.receive_buf[1]);
        data.send_buf = tx_buff_measure;
        data.send_len = 3; // 3:发送三个字节长度
        uapi_i2c_master_write(bus_id, dev_addr, &data);
        return false;
    }

    // 3. 发送测量指令并读取
    data.send_buf = tx_buff_measure;
    data.send_len = 3; // 3:发送三个字节长度
    data.receive_buf = rx_buff;
    data.receive_len = 6; // 6:发送六个字节长度

    if (!(CH20_MeasureAndRead(bus_id, dev_addr, &data))) {
        return false;
    }

    // 4. 解析温湿度
    return Parse_temperature_humidity(data.receive_buf, data.receive_len, temp, humidity);
}

static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_CHT20_PIN, CONFIG_I2C_CHT20_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_CHT20_PIN, CONFIG_I2C_CHT20_PIN_MODE);
}

static void *i2c_cht20_task(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
    uint16_t dev_addr = CONFIG_I2C_CHT20_SLAVE_ADDR;
    float temperature = 0;
    float humidity = 0;

    /* I2C CHT20 init config. */
    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_CHT20_BUS_ID, baudrate, hscode);

    while (1) {
        if (CHT20_Read(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &temperature, &humidity)) {
            // 100:乘以100然后取100的余数,得到小数点后的数值
            osal_printk("Temperature: %d.%02dC,   Humidity: %d.%02d%%\r\n", (int)temperature,
                        abs((int)(temperature * 100) % 100), (int)humidity, abs((int)(humidity * 100) % 100));
        }

        osal_msleep(1500); // 1500:延时1500ms 读取一次温湿度
    }

    return NULL;
}

static void i2c_cht20_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2c_cht20_task, 0, "I2cCht20Task", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the i2c_master_entry. */
app_run(i2c_cht20_entry);