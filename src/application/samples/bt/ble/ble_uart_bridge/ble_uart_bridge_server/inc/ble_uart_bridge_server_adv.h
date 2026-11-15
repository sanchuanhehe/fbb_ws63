/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the BLE UART bridge advertising interface.
 * @else
 * @brief 声明 BLE UART 透传广播接口。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#ifndef BLE_UART_BRIDGE_SERVER_ADV_H
#define BLE_UART_BRIDGE_SERVER_ADV_H

#include <stdbool.h>
#include "errcode.h"

#define BLE_UART_BRIDGE_ADV_ID 1

/**
 * @if Eng
 * @brief Configures and starts connectable advertising.
 * @else
 * @brief 配置并启动可连接广播。
 * @endif
 */
errcode_t ble_uart_bridge_server_start_adv(void);

/**
 * @if Eng
 * @brief Updates the state encoded in subsequent advertising payloads.
 * @else
 * @brief 更新后续广播数据中编码的状态。
 * @endif
 */
void ble_uart_bridge_server_set_adv_default_state(bool is_default);

#endif /* BLE_UART_BRIDGE_SERVER_ADV_H */
