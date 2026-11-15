/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the BLE sensor report data collector interface.
 * @else
 * @brief 声明 BLE 传感器上报数据采集端接口。
 * @endif
 *
 * History: \n
 * 2026-07-23, Create file. \n
 */

#ifndef BLE_SENSOR_REPORT_CLIENT_H
#define BLE_SENSOR_REPORT_CLIENT_H

#include "errcode.h"

/**
 * @if Eng
 * @brief Initializes scanning, connection, and GATT client callbacks.
 * @else
 * @brief 初始化扫描、连接和 GATT 客户端回调。
 * @endif
 */
errcode_t ble_sensor_report_client_init(void);

#endif /* BLE_SENSOR_REPORT_CLIENT_H */
