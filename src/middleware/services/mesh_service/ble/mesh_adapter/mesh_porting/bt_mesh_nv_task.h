/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell config
 *
 * History:
 * 2025-6-04, Create file.
 */
#ifndef __BT_MESH_NV_TASK_H__
#define __BT_MESH_NV_TASK_H__

#include <stdint.h>

void bt_mesh_nv_write_task_init(void);
int32_t bt_mesh_nv_write_task_deinit(void);
int32_t ble_mesh_nv_push(uint16_t key, const uint8_t *data, uint16_t len);
int32_t ble_mesh_nv_push_encrypt(uint16_t key, const uint8_t *data, uint16_t len);
#endif