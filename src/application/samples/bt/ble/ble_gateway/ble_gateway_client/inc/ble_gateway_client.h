/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello GATT client interface.
 */

#ifndef BLE_GATEWAY_CLIENT_H
#define BLE_GATEWAY_CLIENT_H

#include <stdint.h>
#include "errcode.h"

errcode_t ble_gateway_client_init(void);
void ble_gateway_client_restart_scan(void);
errcode_t ble_gateway_client_set_sample_interval(uint32_t interval_s);

#endif
