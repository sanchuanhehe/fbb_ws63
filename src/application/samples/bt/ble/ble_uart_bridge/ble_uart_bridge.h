/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the shared buffering interface for the BLE UART bridge.
 * @else
 * @brief 声明 BLE UART 透传示例的公共缓冲接口。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#ifndef BLE_UART_BRIDGE_H
#define BLE_UART_BRIDGE_H

#include <stdint.h>
#include "errcode.h"

#define BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN 244

/**
 * @if Eng
 * @brief Queues one BLE payload for bounded task-context UART1 transmission.
 * @else
 * @brief 通过公共透传接口将一包 BLE 数据加入有界队列，等待任务上下文写入 UART1。
 * @endif
 */
errcode_t ble_uart_bridge_uart_enqueue(const uint8_t *data, uint16_t length);

/**
 * @if Eng
 * @brief Reports completion of the current queued UART-to-BLE fragment.
 * @else
 * @brief 上报当前已排队 UART 到 BLE 数据分片的完成结果。
 * @endif
 */
void ble_uart_bridge_ble_send_complete(errcode_t status);

#endif /* BLE_UART_BRIDGE_H */
