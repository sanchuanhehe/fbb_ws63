/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Uart Sample Source. \n
 *
 */

/**
 * 本例程通过串口调试工具往串口1发数据，然后在串口1接收回调里面将数据发送到串口0
 */

#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"

#define UART_BAUDRATE 115200
#define CONFIG_UART_INT_WAIT_MS 5
#define UART_TASK_PRIO 24
#define UART_TASK_STACK_SIZE 0x1000

#define UART_TXD_PIN 26
#define UART_RXD_PIN 27
#define UART_TXD_PIN_MODE 1
#define UART_RXD_PIN_MODE 1
#define CONFIG_UART_TRANSFER_SIZE 64
#define CONFIG_UART_BUS_ID UART_BUS_1

static uint8_t g_app_uart_rx_buff[CONFIG_UART_TRANSFER_SIZE] = {0};
static volatile uint8_t g_app_uart_int_index = 0;
static uart_buffer_config_t g_app_uart_buffer_config = {.rx_buffer = g_app_uart_rx_buff,
                                                        .rx_buffer_size = CONFIG_UART_TRANSFER_SIZE};

static void app_uart_init_pin(void)
{
    uapi_pin_set_mode(UART_TXD_PIN, UART_TXD_PIN_MODE);
    uapi_pin_set_mode(UART_RXD_PIN, UART_RXD_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {.baud_rate = UART_BAUDRATE,
                        .data_bits = UART_DATA_BIT_8,
                        .stop_bits = UART_STOP_BIT_1,
                        .parity = UART_PARITY_NONE};

    uart_pin_config_t pin_config = {
        .tx_pin = UART_TXD_PIN, .rx_pin = UART_RXD_PIN, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
}

static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    // 将串口1接收到的数据发送到串口0
    uapi_uart_write(UART_BUS_0, buffer, length, 0);
}

static void *uart_task(const char *arg)
{
    unused(arg);

    app_uart_init_pin();
    app_uart_init_config();

    // 注册串口接收中断
    if (uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                       CONFIG_UART_TRANSFER_SIZE, app_uart_read_int_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", CONFIG_UART_BUS_ID);
    }

    return NULL;
}

static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_task, 0, "UartTask", UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);