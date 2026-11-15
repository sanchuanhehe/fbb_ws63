/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Initializing the mesh device sample.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __BLE_MESH_DEVICE_SAMPLE_H__
#define __BLE_MESH_DEVICE_SAMPLE_H__
#include "ble_mesh_prov.h"
errcode_t ble_mesh_sample_enable(ble_mesh_prov_bearer_t bearers);
errcode_t ble_mesh_sample_disable(void);
#endif
