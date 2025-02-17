/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog.h"
#include "string.h"
#define UART_BAUDRATE 115200
#define UART_DATA_BITS 3
#define UART_STOP_BITS 1
#define UART_PARITY_BIT 0
#define UART_TRANSFER_SIZE 20
#define CONFIG_UART1_TXD_PIN 17
#define CONFIG_UART1_RXD_PIN 18
#define CONFIG_UART1_PIN_MODE 1
#define CONFIG_UART_INT_WAIT_MS 5

#define UART_TASK_STACK_SIZE 0x1000
#define UART_TASK_DURATION_MS 1000
#define UART_TASK_PRIO 17

static uint8_t g_app_uart_rx_buff[UART_TRANSFER_SIZE] = {0};

#define UART_INT_MODE 0
#if(UART_INT_MODE)
static uint8_t g_app_uart_int_rx_flag = 0;
#endif
static uart_buffer_config_t g_app_uart_buffer_config = {
                .rx_buffer = g_app_uart_rx_buff,
                .rx_buffer_size = UART_TRANSFER_SIZE};

static void app_uart_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_UART1_TXD_PIN, CONFIG_UART1_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART1_RXD_PIN, CONFIG_UART1_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {.baud_rate = UART_BAUDRATE,
                        .data_bits = UART_DATA_BITS,
                        .stop_bits = UART_STOP_BITS,
                        .parity = UART_PARITY_BIT};

    uart_pin_config_t pin_config = {.tx_pin = S_MGPIO0, .rx_pin = S_MGPIO1, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(UART_BUS_0); // 重点，UART初始化之前需要去初始化，否则会报0x80001044
    int res = uapi_uart_init(UART_BUS_0, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    if (res != 0) {
        printf("uart init failed res = %02x\r\n", res);
    }
}

#if(UART_INT_MODE)
static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (buffer == NULL || length == 0) {
        osal_printk("uart%d int mode transfer illegal data!\r\n", UART_BUS_0);
        return;
    }
    if (memcpy_s(g_app_uart_rx_buff, length, buffer, length) != EOK) {
        osal_printk("uart%d int mode data copy fail!\r\n", UART_BUS_0);
        return;
    }
   
    g_app_uart_int_rx_flag = 1;
}
#endif


void *uart_task(const char *arg)
{
    unused(arg);
    /* UART pinmux. */
    app_uart_init_pin();
    /* UART init config. */
    app_uart_init_config();

#if(UART_INT_MODE)
    osal_printk("uart%d int mode register receive callback start!\r\n", UART_BUS_0);
    if (uapi_uart_register_rx_callback(0, UART_RX_CONDITION_MASK_IDLE, 1, app_uart_read_int_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", UART_BUS_0);
    }
#endif

    while (1) {
#if(UART_INT_MODE)
        while (g_app_uart_int_rx_flag != 1) {
            osal_msleep(CONFIG_UART_INT_WAIT_MS);
        }
        g_app_uart_int_rx_flag = 0;
        osal_printk("uart rx data = [%s]\r\n", g_app_uart_rx_buff);
        memset(g_app_uart_rx_buff,0,UART_TRANSFER_SIZE);
#else
        if (uapi_uart_read(UART_BUS_0, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0)) {
            osal_printk("uart%d poll mode receive succ!\r\n", UART_BUS_0);
        }
        osal_printk("uart%d poll mode send back!\r\n", UART_BUS_0);
        if (uapi_uart_write(UART_BUS_0, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0)) {
            osal_printk("uart%d poll mode send back succ!\r\n", UART_BUS_0);
        }
    }
#endif
    return NULL;
}

static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    uapi_watchdog_disable();
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_task, 0, "UartTask", UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);