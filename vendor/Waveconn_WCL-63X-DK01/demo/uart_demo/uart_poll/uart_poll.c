#include "app_init.h"   
#include "stdio.h"
#include "common_def.h"  
#include "soc_osal.h"    
#include "securec.h"    
#include "driver/uart.h"
#include "pinctrl.h"

#define CONFIG_UART_TRANSFER_SIZE 2500
#define CONFIG_UART_TXD_PIN 17
#define CONFIG_UART_RXD_PIN 18
#define CONFIG_UART_TXD_PIN_MODE 1
#define CONFIG_UART_RXD_PIN_MODE 1
#define CONFIG_UART_BUS_ID 1
#define CONFIG_UART_BAUDRATE 115200
 
uint16_t uart_len = 0;
uint8_t g_app_uart_rx_buff[CONFIG_UART_TRANSFER_SIZE] = {0};


static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = CONFIG_UART_TRANSFER_SIZE};
static void app_uart_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = CONFIG_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE};

    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN,
        .rx_pin = CONFIG_UART_RXD_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE};

    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
}


static void *uart_thread(void)
{
    // 此处初始化UART1作为串口初始化演示，实际上使用UART0可不初始化
    /* UART pinmux. */
    app_uart_init_pin();
    /* UART init config. */
    app_uart_init_config();
    
    while (1)
    {
        // 预留读取UART1数据的示例代码
        // uart_len = uapi_uart_read(CONFIG_UART_BUS_ID, g_app_uart_rx_buff , CONFIG_UART_TRANSFER_SIZE, 200);

        // 此处读取来自数据传输线的数据，也就是UART0的数据
        uart_len = uapi_uart_read(UART_BUS_0, g_app_uart_rx_buff , CONFIG_UART_TRANSFER_SIZE, 200);
        if(uart_len > 0)
        {
            osal_printk("g_app_uart_rx_buff:%s\r\n", g_app_uart_rx_buff);
            memset(g_app_uart_rx_buff, 0, CONFIG_UART_TRANSFER_SIZE); // 及时清空缓存，避免数据混乱
        }
        uapi_watchdog_kick();
    }
    return NULL;
}

static void uart_entry(void)
{


    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_thread, 0, "UartTask", 0x2000);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, 25);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);