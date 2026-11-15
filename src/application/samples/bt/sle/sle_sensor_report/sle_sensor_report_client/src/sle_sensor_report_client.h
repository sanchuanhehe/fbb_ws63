/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * @if Eng
 * @brief Declares the public interface of the SLE sensor report client.
 * @else
 * @brief 声明 SLE 传感器上报客户端的公共接口。
 * @endif
 *
 * History: \n
 * 2024-06-01, Create file. \n
 */

#ifndef SLE_SENSOR_REPORT_CLIENT_H
#define SLE_SENSOR_REPORT_CLIENT_H

#include "sle_ssap_client.h"

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_sensor_report_client_init.
 * @else
 * @brief 初始化 \c sle_sensor_report_client_init 对应的功能。
 * @endif
 */
void sle_sensor_report_client_init(ssapc_notification_callback notification_cb,
                                   ssapc_indication_callback indication_cb);
/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_sensor_report_client_is_connected(void);
/**
 * @if Eng
 * @brief Starts scanning for the target SLE server.
 * @else
 * @brief 开始扫描目标 SLE 服务端。
 * @endif
 */
void sle_sensor_report_client_start_scan(void);

#endif /* SLE_SENSOR_REPORT_CLIENT_H */
