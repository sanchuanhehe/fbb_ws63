 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: ble mesh model inner.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __BLE_MESH_MODEL_INNER_H__
#define __BLE_MESH_MODEL_INNER_H__
#include "mesh/access.h"
#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_comp* ble_mesh_get_comp(void);
void ble_mesh_model_init(void);
#ifdef __cplusplus
}
#endif

#endif /* __BLE_MESH_MODEL_INNER_H__ */
