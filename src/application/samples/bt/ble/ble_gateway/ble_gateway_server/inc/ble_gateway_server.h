/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello GATT server interface.
 */

#ifndef BLE_GATEWAY_SERVER_H
#define BLE_GATEWAY_SERVER_H

#include <stdint.h>
#include "errcode.h"

#define BLE_GATEWAY_SERVICE_UUID 0x3333
#define BLE_GATEWAY_DATA_UUID 0x3434
#define BLE_GATEWAY_NOTIFY_UUID 0x3435
#define BLE_GATEWAY_CCCD_UUID 0x2902
#define BLE_GATEWAY_PROPERTY_MAX_LEN 32

errcode_t ble_gateway_server_init(void);
errcode_t ble_gateway_server_send_notification(const uint8_t *data, uint16_t len);
void ble_gateway_server_report_loop(void);

#endif
