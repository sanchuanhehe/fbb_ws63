/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the BLE sensor report advertising interface.
 * @else
 * @brief 声明 BLE 传感器上报广播接口。
 * @endif
 *
 * History: \n
 * 2026-07-23, Create file. \n
 */

#ifndef BLE_SENSOR_REPORT_SERVER_ADV_H
#define BLE_SENSOR_REPORT_SERVER_ADV_H

#include <stdbool.h>
#include "errcode.h"

#define BLE_SENSOR_REPORT_ADV_ID 1

/**
 * @if Eng
 * @brief Configures and starts sensor node advertising.
 * @else
 * @brief 配置并启动传感器节点广播。
 * @endif
 */
errcode_t ble_sensor_report_server_start_adv(void);

/**
 * @if Eng
 * @brief Updates the device state carried in service data.
 * @else
 * @brief 更新服务数据中携带的设备状态。
 * @endif
 */
void ble_sensor_report_server_set_adv_default_state(bool is_default);

#endif /* BLE_SENSOR_REPORT_SERVER_ADV_H */
