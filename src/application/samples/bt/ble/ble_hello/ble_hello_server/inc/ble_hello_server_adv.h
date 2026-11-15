/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello advertising interface.
 */

#ifndef BLE_HELLO_SERVER_ADV_H
#define BLE_HELLO_SERVER_ADV_H

#include <stdbool.h>
#include "errcode.h"

#define BLE_HELLO_ADV_ID 1

errcode_t ble_hello_server_start_adv(void);
void ble_hello_server_set_adv_default_state(bool is_default);

#endif
