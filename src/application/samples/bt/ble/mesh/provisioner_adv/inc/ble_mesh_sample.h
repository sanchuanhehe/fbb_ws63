/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: BLE MESH Provisioner Adv Source.
 *
 * History:
 * 2024-12-23, Create file.
 */

#ifndef __BLE_MESH_SAMPLE_H__
#define __BLE_MESH_SAMPLE_H__
#include "ble_mesh_prov.h"

errcode_t ble_mesh_sample_enable(ble_mesh_prov_bearer_t bearers);
int ble_mesh_sample_rmt_prov_scan_start(uint16_t net_idx, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN],
                                        uint8_t items_limit, uint8_t timeout);
int ble_mesh_sample_rmt_prov_link_open(const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx,
                                       uint16_t rmt_addr, uint8_t refresh);
errcode_t ble_mesh_sample_disable(void);
#endif
