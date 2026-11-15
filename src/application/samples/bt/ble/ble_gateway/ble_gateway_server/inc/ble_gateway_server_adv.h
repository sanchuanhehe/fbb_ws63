/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello advertising interface.
 */

#ifndef BLE_GATEWAY_SERVER_ADV_H
#define BLE_GATEWAY_SERVER_ADV_H

#include <stdbool.h>
#include "errcode.h"

#define BLE_GATEWAY_ADV_ID 1

errcode_t ble_gateway_server_start_adv(void);
void ble_gateway_server_set_adv_default_state(bool is_default);

#endif
