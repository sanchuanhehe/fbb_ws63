/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: w33 board port config. \n
 *
 * History: \n
 * 2024-05-23, Create file. \n
 */
#ifndef W33_BOARD_PORT_CONFIG_H
#define W33_BOARD_PORT_CONFIG_H

#include "w33_board_port.h"
#include "gpio.h"
#include "uart.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define PIN_PULL_DOWN    PIN_PULL_TYPE_DOWN
#define PIN_PULL_NONE    PIN_PULL_TYPE_DISABLE

#ifndef CONFIG_TIOT_WS63_UART_BUS
#define CONFIG_TIOT_WS63_UART_BUS   1
#endif

static w33_board_hw_info g_w33_board_hw_info = { CONFIG_TIOT_WS63_UART_BUS, { GPIO_03, PIN_NONE, GPIO_04 } };
static w33_board_info g_w33_board_info = {
    .cfg_path = NULL,
    .hw_infos = &g_w33_board_hw_info
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif