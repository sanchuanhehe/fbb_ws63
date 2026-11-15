/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the BLE UART bridge GATT server interface.
 * @else
 * @brief 声明 BLE UART 透传 GATT 服务端接口。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#ifndef BLE_UART_BRIDGE_SERVER_H
#define BLE_UART_BRIDGE_SERVER_H

#include <stdint.h>
#include "errcode.h"
#include "ble_uart_bridge.h"

#define BLE_UART_BRIDGE_SERVICE_UUID 0x4444
#define BLE_UART_BRIDGE_DATA_UUID 0x4545
#define BLE_UART_BRIDGE_NOTIFY_UUID 0x4546
#define BLE_UART_BRIDGE_CCCD_UUID 0x2902
#define BLE_UART_BRIDGE_PROPERTY_MAX_LEN BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN

/**
 * @if Eng
 * @brief Initializes the BLE UART bridge server.
 * @else
 * @brief 初始化 BLE UART 透传服务端。
 * @endif
 */
errcode_t ble_uart_bridge_server_init(void);

/**
 * @if Eng
 * @brief Sends one UART data fragment through the indication characteristic.
 * @else
 * @brief 通过指示特征发送一段 UART 数据。
 * @endif
 */
errcode_t ble_uart_bridge_server_send_notification(const uint8_t *data, uint16_t len);

#endif /* BLE_UART_BRIDGE_SERVER_H */
