/**
 * Copyright (c) Adragon
 *
 * Description: WS63 Uart receiving and transmitting case based on interrupt mode . \n
 *              This case also includes a set of AT transceiver frameworks.
 *
 *
 * History: \n
 * 2025-5-20, Create file. \n
 */
#include <ws63_uart.h>

#define UART1_TX_PIN 15
#define UART1_RX_PIN 16
// #define UART0_TX_PIN 17
// #define UART0_RX_PIN 18
#define BUFFER_SIZE 2000
#define UART1_RX_EVENT_READ 0x01
#define BAUD_RATE 115200

uint8_t g_app_uart_rx_buff[1] = {0};
uint8_t g_buff[BUFFER_SIZE] = {0};
uint8_t g_app_uart_int_rx_buff[BUFFER_SIZE] = {0};
uint8_t g_app_uart_tx_buff[BUFFER_SIZE] = {0};
volatile uint8_t g_app_uart_rx_flag = 0;
uint16_t g_app_uart_int_rx_buff_len = 0;
uint16_t g_buff_len = 0;

osal_event g_app_uart_event = {0};
WS63Uart_Rec_State_t WS63Uart_Rec_State = {0};

// 用于在发送完成后执行某些操作的回调函数 //
void app_uart_write_int_handler(uint8_t *buffer, uint16_t length, uint8_t *params)
{
    uart_write_int_param_t *uart_write_int_param = (uart_write_int_param_t *)params;
    uint8_t expect_rbk_flag = 0;
    // 等待接收数据
    uint32_t timeout = 1000;                    // 定义超时时间，单位为毫秒
    uint32_t start_time = uapi_tcxo_get_ms();   // 获取当前时间戳
    //存在AT返回码未完全接收完毕就超时的情况
    while (g_app_uart_rx_flag !=1 || g_buff_len == 0 || expect_rbk_flag != 1)
    {
        osal_msleep(5);
        uapi_watchdog_kick();

        if(strstr(g_buff, uart_write_int_param->expect_rbk) != NULL)
        {
            expect_rbk_flag = 1;
        }

        uint32_t current_time = uapi_tcxo_get_ms(); // 获取当前时间戳
        if ((current_time - start_time) >= timeout) // 检查是否超时
        {
            osal_printk("UartInit Send Cmd Timeout occurred!\r\n");
            g_app_uart_rx_flag = 0;
            break;
        }
    }

    osal_printk("[app_uart_write_int_handler]g_buff(%d):", g_buff_len);
    for(int i = 0; i < length; i++)
    {
        osal_printk("%c", g_buff[i]);
    }
    osal_printk("\r\nexpect_rbk: %s\r\n", uart_write_int_param->expect_rbk);
    if(expect_rbk_flag == 1)
    {
        osal_printk("****************************************\r\n");
        osal_printk("Get Correct Cmd!\r\n");
        osal_printk("****************************************\r\n");
    }
    else
    {
        osal_printk("Get Wrong Cmd!!!\r\n");
        if(uart_write_int_param->send_react_times > 0)
        {
            osal_printk("Send React Times: %d\r\n", uart_write_int_param->send_react_times);
            uart_write_int_param->send_react_times--;
            // 缓冲区中无期待的返回数据，重新发送（此处存在些许Bug待修复）
            // memset(g_buff, 0, BUFFER_SIZE);
            // g_buff_len = 0;
            // UartInt_Send_Cmd(uart_write_int_param->cmd, uart_write_int_param->expect_rbk, uart_write_int_param->send_react_times);
        }
    }
    memset(g_buff, 0, BUFFER_SIZE);
    g_buff_len = 0;
}

void UartInt_Send_Cmd(uint8_t *cmd, uint8_t *expect_rbk, uint8_t send_react_times)
{
    osal_printk("UartInt Send Cmd : %s\r\n", cmd);
    // 初始化回调函数传参结构体
    uart_write_int_param_t uart_write_int_param = {
        .cmd = cmd,
        .expect_rbk = expect_rbk,
        .send_react_times = send_react_times
    };
    // 发送数据前，清空接收缓冲区，避免数据混乱
    memset(g_buff, 0, BUFFER_SIZE);
    g_buff_len = 0;
    // 中断模式发送数据
    if(uapi_uart_write_int(UART_BUS_1, cmd, strlen(cmd), &uart_write_int_param, app_uart_write_int_handler) == ERRCODE_SUCC)
    {
        osal_printk("uart%d int mode send cmd succ!\r\n", UART_BUS_1);
    }
    else
    {
        osal_printk("uart%d int mode send cmd failed!\r\n", UART_BUS_1);
        uapi_uart_write_int(UART_BUS_1, cmd, strlen(cmd), &uart_write_int_param, app_uart_write_int_handler);
    }
    osal_msleep(1000);
}

void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    unused(buffer);
    if (buffer == NULL || length == 0)
    {
        osal_printk("uart%d int mode transfer illegal data!\r\n", UART_BUS_1);
        return;
    }
    uint8_t *buff = (uint8_t *)buffer;
    // 单字节写入缓冲区，避免长期占用内存
    g_app_uart_int_rx_buff[g_app_uart_int_rx_buff_len++] = buff[0];   // 用于判断特殊返回结果的缓冲区
    g_buff[g_buff_len++] = buff[0];                                   // 用于处理特殊返回结果的缓冲区
    if (g_app_uart_int_rx_buff_len >= BUFFER_SIZE)
    {
        g_app_uart_int_rx_buff_len = 0;
        g_buff_len = 0;
    }
    osal_event_write(&g_app_uart_event, UART1_RX_EVENT_READ); // 触发事件
    g_app_uart_rx_flag = 1;
}

void app_uart_register_rx_callback(uart_bus_t bus)
{
    osal_printk("uart%d int mode register receive callback start!\r\n", bus);
    if (uapi_uart_register_rx_callback(bus, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1, app_uart_read_int_handler) == ERRCODE_SUCC)
    {
        osal_printk("uart%d int mode register receive callback succ!\r\n", bus);
    }
}

void app_uart_event_init(void)
{
    if(osal_event_init(&g_app_uart_event) != OSAL_SUCCESS)
    {
        osal_printk("uart event init failed\r\n");
    }
    else
    {
        osal_printk("uart event init success\r\n");
    }
}

void init_uart(uart_bus_t bus,pin_t tx_pin, pin_t rx_pin, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity)
{
    uapi_uart_deinit(bus);
    uapi_pin_set_mode(tx_pin, PIN_MODE_1);
    uapi_pin_set_mode(rx_pin, PIN_MODE_1);
    uart_attr_t attr =
        {
            .baud_rate = baud_rate,
            .data_bits = data_bits,
            .stop_bits = stop_bits,
            .parity = parity};
    uart_pin_config_t pin_config =
        {
            .tx_pin = tx_pin,
            .rx_pin = rx_pin,
            .cts_pin = PIN_NONE,
            .rts_pin = PIN_NONE};
    uart_buffer_config_t g_app_uart_buffer_config = {
        .rx_buffer = g_app_uart_rx_buff,
        .rx_buffer_size = 1};
    uapi_uart_init(bus, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    app_uart_register_rx_callback(bus);
    app_uart_event_init();
}



void Uart_Rec_Task()
{
   init_uart(UART_BUS_1, UART1_TX_PIN, UART1_RX_PIN, BAUD_RATE, UART_DATA_BIT_8, 1, UART_PARITY_NONE);
   osal_printk("uart%d int mode init succ!\r\n", UART_BUS_1);
   while (1)
   {
    // 等待接收数据 //
       if (osal_event_read(&g_app_uart_event, UART1_RX_EVENT_READ, OSAL_WAIT_FOREVER, OSAL_WAITMODE_AND) != OSAL_FAILURE)
       {        
       loop:
           osal_msleep(5);
           if (osal_event_read(&g_app_uart_event, UART1_RX_EVENT_READ, OSAL_WAIT_CONDITION_TRUE, OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR) != OSAL_FAILURE)
           {
               goto loop;
           }
    // 等待接收数据 //
           else
           {
    // 处理接收数据 //
               osal_printk("[Uart_Rec_Task]g_app_uart_int_rx_buff(%d):\r\n", g_app_uart_int_rx_buff_len);
               for(int i=0; i<g_app_uart_int_rx_buff_len; i++)
               {
                   osal_printk("%c", g_app_uart_int_rx_buff[i]);
               }
               osal_printk("------------------------------------------------------\r\n");

// 串口接收数据后的判断操作 //
               if(strstr(g_app_uart_int_rx_buff, "+SIM READY")!=NULL)
               {
                     osal_printk("L610 4G Cat.1 Init Success!\r\n");
                     WS63Uart_Rec_State.L610_Init_Flag = 1;
               }

               // 及时清空接收缓冲区，避免数据混乱
               memset(g_app_uart_int_rx_buff, 0, BUFFER_SIZE);
               g_app_uart_int_rx_buff_len = 0;
           }
    // 处理接收数据 //
       }
    }
    return NULL;
}

void Uart_Progress_Task()
{
    while (1)
    {
        uapi_watchdog_kick();
        osal_msleep(50);

        if(WS63Uart_Rec_State.mqtt_break_flag == 1) //MQTT断开重连
        {
            // 执行MQTT重连操作 //
            osal_printk("[Uart_Progress_Task]:WS63Uart_Rec_State.mqtt_break_flag = %d\r\n", WS63Uart_Rec_State.mqtt_break_flag);
            UartInt_Send_Cmd("AT+MQTTUSER=1,\"username\",\"userid\",\"password\"\r\n", "OK", 0);
            UartInt_Send_Cmd("AT+MQTTOPEN=1,\"xxx.xxx.xx.xxx\",port,0,60\r\n", "+MQTTOPEN", 0);
            WS63Uart_Rec_State.mqtt_break_flag = 0;
        }

        if (WS63Uart_Rec_State.upg_flag == 1) //OTA升级
        {   
            osal_printk("[Uart_Progress_Task]:WS63Uart_Rec_State.upg_flag = %d\r\n", WS63Uart_Rec_State.upg_flag);
            if(WS63Uart_Rec_State.file_size > 0)
            {
                // 执行升级相关操作... //
                WS63Uart_Rec_State.upg_flag = 0;
            }
        }
    }
}