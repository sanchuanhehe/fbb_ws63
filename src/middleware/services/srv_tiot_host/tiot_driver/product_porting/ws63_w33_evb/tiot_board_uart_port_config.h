/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2024-05-23, Create file. \n
 */
#ifndef TIOT_BOARD_UART_PORT_CONFIG_H
#define TIOT_BOARD_UART_PORT_CONFIG_H

#include "tiot_board_uart_port.h"
#include "tiot_board_log.h"

#include "uart.h"
#include "uart_porting.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef CONFIG_TIOT_WS63_UART_BUS
#define CONFIG_TIOT_WS63_UART_BUS   1
#endif

#if CONFIG_TIOT_WS63_UART_BUS == 1
#define TIOT_UART_GPIO   {GPIO_15, GPIO_16, GPIO_13, GPIO_14}
#define TIOT_PIN_MUX     {PIN_MODE_1, PIN_MODE_1, PIN_MODE_1, PIN_MODE_1}
#else
#define TIOT_UART_GPIO   {GPIO_08, GPIO_07, GPIO_05, GPIO_06}
#define TIOT_PIN_MUX     {PIN_MODE_2, PIN_MODE_2, PIN_MODE_2, PIN_MODE_2}
#endif

#define GPIO_SET_DIR_INPUT
#define UART_RX_BUFFER_SIZE     56
#define PIN_PULL_NONE           PIN_PULL_TYPE_DISABLE
#define PIN_PULL_DOWN           PIN_PULL_TYPE_DOWN
#define PIN_PULL_UP             PIN_PULL_TYPE_UP

/* TIoT驱动使用MCU UART个数. */
enum tiot_uart_port {
    TIOT_UART_PORT0,
    TIOT_UART_NUM,
    TIOT_UART_NONE = TIOT_UART_NUM
};

enum tiot_uart_pin_type {
    UART_PIN_TYPE_TX,
    UART_PIN_TYPE_RX,
    UART_PIN_TYPE_CTS,
    UART_PIN_TYPE_RTS,
    UART_PIN_TYPE_NUM
};

typedef struct {
    tiot_xmit *xmit;
    uint8_t flow_ctrl_bk;
    uint8_t rx_buff[UART_RX_BUFFER_SIZE];
    const uart_pin_config_t pin_config_orig;
    pin_mode_t pinmux[UART_PIN_TYPE_NUM];
    uart_rx_callback_t rx_callback;
    const uart_extra_attr_t *extra_attr;
} tiot_board_uart_port;

static tiot_xmit_callbacks g_tiot_xmit_cbs;
static const uart_extra_attr_t uart_extra_attr = {
    .tx_dma_enable = 0,                                           /* 0: TX没有使用DMA, 1: TX使用DMA */
    .tx_int_threshold = UART_FIFO_INT_TX_LEVEL_EQ_2_CHARACTER,    /* 设置触发中断的Tx fifo水线. */
    .rx_dma_enable = 0,                                           /* 0: RX没有使用DMA, 1: RX使用DMA */
    .rx_int_threshold = UART_FIFO_INT_RX_LEVEL_1_2,               /* 设置触发中断的Rx FIFO水线. */
};

/* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
static void board_uart_rx_callback0(const void *buff, uint16_t length, bool error);
/* TIoT驱动使用的UART初始信息，需要按实际板级情况适配。 */
static tiot_board_uart_port g_tiot_board_uart_port[TIOT_UART_NUM] = {
    /* TIOT_UART_PORT0. */
    {
        .flow_ctrl_bk = TIOT_UART_ATTR_FLOW_CTRL_ENABLE,  /* WS63 UART默认开启流控。 */
        .rx_buff = { 0 },
        .pin_config_orig = TIOT_UART_GPIO,   /* MCU UART管脚，按实际板级情况适配。 */
        /* MCU UART管脚pinmux，按实际板级情况适配。 */
        .pinmux = TIOT_PIN_MUX,
        .rx_callback = board_uart_rx_callback0,             /* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
        .extra_attr = &uart_extra_attr
    }
};

/* MCU UART bus号与TIoT驱动使用的UART 对应关系，需要按实际情况适配。 */
static inline enum tiot_uart_port board_uart_bus_convert_port(uart_bus_t bus)
{
    /* WS63 evb上 UART_BUS_1 对应 TIoT驱动使用的第一个UART。 */
    if (bus == CONFIG_TIOT_WS63_UART_BUS) {
        return TIOT_UART_PORT0;
    }
    return TIOT_UART_NONE;
}

static inline void board_rx_callback_common(tiot_board_uart_port *port, const void *buff, uint16_t length)
{
    g_tiot_xmit_cbs.rx_notify(port->xmit, (uint8_t *)buff, length);
}

/* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
static void board_uart_rx_callback0(const void *buff, uint16_t length, bool error)
{
    if (error == false) {
        board_rx_callback_common(&g_tiot_board_uart_port[TIOT_UART_PORT0], buff, length);
    } else {
        tiot_print_err("[TIoT]uart port0 receive error.\r\n");
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif