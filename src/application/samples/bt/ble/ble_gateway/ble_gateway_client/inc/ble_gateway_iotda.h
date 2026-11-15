/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
 * Description: IoTDA adapter for the BLE environment gateway.
 */

#ifndef BLE_GATEWAY_IOTDA_H
#define BLE_GATEWAY_IOTDA_H

#include <stdbool.h>
#include "ble_gateway_protocol.h"

void ble_gateway_iotda_init(void);
void ble_gateway_iotda_run(void);
void ble_gateway_iotda_enqueue_report(const ble_gateway_report_t *report);
void ble_gateway_iotda_command_result(bool success);
void ble_gateway_iotda_node_state(bool online);

#endif
