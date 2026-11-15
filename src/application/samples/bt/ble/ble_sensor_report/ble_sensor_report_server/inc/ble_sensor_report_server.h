/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the BLE sensor report GATT server interface.
 * @else
 * @brief 声明 BLE 传感器上报 GATT 服务端接口。
 * @endif
 *
 * History: \n
 * 2026-07-23, Create file. \n
 */

#ifndef BLE_SENSOR_REPORT_SERVER_H
#define BLE_SENSOR_REPORT_SERVER_H

#include <stdint.h>
#include "errcode.h"

#define BLE_SENSOR_REPORT_SERVICE_UUID 0x3333
#define BLE_SENSOR_REPORT_DATA_UUID 0x3434
#define BLE_SENSOR_REPORT_NOTIFY_UUID 0x3435
#define BLE_SENSOR_REPORT_CCCD_UUID 0x2902
#define BLE_SENSOR_REPORT_PROPERTY_MAX_LEN 32

/**
 * @if Eng
 * @brief Initializes the sensor, GATT server, and BLE stack.
 * @else
 * @brief 初始化传感器、GATT 服务端和 BLE 协议栈。
 * @endif
 */
errcode_t ble_sensor_report_server_init(void);

/**
 * @if Eng
 * @brief Periodically samples the sensors and reports available data.
 * @else
 * @brief 周期采集传感器并上报可用数据。
 * @endif
 */
void ble_sensor_report_server_report_loop(void);

/**
 * @if Eng
 * @brief Sends one sensor report notification to the connected collector.
 * @else
 * @brief 向已连接的数据采集端发送一条传感器通知。
 * @endif
 */
errcode_t ble_sensor_report_server_send_notification(const uint8_t *data, uint16_t len);

#endif /* BLE_SENSOR_REPORT_SERVER_H */
