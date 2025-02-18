/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @defgroup bluetooth_bts_hid_server HID SERVER API
 * @ingroup
 * @{
 */
#ifndef BLE_GATT_CLIENT_H
#define BLE_GATT_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Use this funtion to init gatt client.
 * @par Description:init gatt client.
 * @attention  NULL
 * @param  NULL
 * @retval error code.
 * @else
 * @brief  初始化gatt 客户端。
 * @par 说明:初始化gatt 客户端。
 * @attention  NULL
 * @param  NULL
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t ble_gatt_client_init(void);

/**
 * @if Eng
 * @brief  discover all service, character and descriptor of remote device.
 * @par Description:discover all service of remote device.
 * @attention  NULL
 * @param  conn_id connection ID
 * @retval error code.
 * @else
 * @brief  发现对端设备所有服务、特征和描述符。
 * @par 说明：发现对端设备所有服务、特征和描述符。
 * @attention  NULL
 * @param  conn_id 连接 ID
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t ble_gatt_client_discover_all_service(uint16_t conn_id);

void app_uart_init_config(void);

#ifdef __cplusplus
}
#endif
#endif
