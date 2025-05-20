/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: DMA Sample Source. \n
 *
 */

/**
 * 本例程通过DMA读取串口数据或发送数据
 */

#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#include "dma.h"
#include "hal_dma.h"

#define UART_BAUDRATE 115200
#define CONFIG_UART_INT_WAIT_MS 5
#define DMA_TASK_PRIO 24
#define DMA_TASK_STACK_SIZE 0x1000

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

uart_write_dma_config_t g_app_dma_cfg = {.src_width = HAL_DMA_TRANSFER_WIDTH_8,
                                         .dest_width = HAL_DMA_TRANSFER_WIDTH_8,
                                         .burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1,
                                         .priority = HAL_DMA_CH_PRIORITY_0};

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

    uart_extra_attr_t extra_attr = {.tx_dma_enable = true,
                                    .tx_int_threshold = UART_FIFO_INT_TX_LEVEL_EQ_0_CHARACTER,
                                    .rx_dma_enable = true,
                                    .rx_int_threshold = UART_FIFO_INT_RX_LEVEL_1_CHARACTER};
    uapi_dma_init();
    uapi_dma_open();
    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, &extra_attr, &g_app_uart_buffer_config);
}

static void *dma_task(const char *arg)
{
    unused(arg);

    app_uart_init_pin();
    app_uart_init_config();

    int32_t ret = CONFIG_UART_TRANSFER_SIZE;

    while (1) {
        osal_printk("uart%d dma mode receive start!\r\n", CONFIG_UART_BUS_ID);

        if (uapi_uart_read_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE, &g_app_dma_cfg) ==
            ret) {
            osal_printk("uart%d dma mode receive succ!\r\n", CONFIG_UART_BUS_ID);
        }
        osal_printk("uart%d dma mode send back!\r\n", CONFIG_UART_BUS_ID);
        if (uapi_uart_write_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE, &g_app_dma_cfg) ==
            ret) {
            osal_printk("uart%d dma mode send back succ!\r\n", CONFIG_UART_BUS_ID);
        }
    }

    return NULL;
}

static void dma_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)dma_task, 0, "DmaTask", DMA_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, DMA_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the dma_entry. */
app_run(dma_entry);