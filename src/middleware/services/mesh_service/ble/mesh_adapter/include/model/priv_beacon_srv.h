/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon Server Model.
 */

#ifndef __PRIV_BEACON_SRV_H__
#define __PRIV_BEACON_SRV_H__

#include "mesh/mesh.h"
#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op bt_mesh_priv_beacon_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_priv_beacon_srv_cb;

#define BT_MESH_MODEL_PRIV_BEACON_SRV                                      \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_PRIV_BEACON_SRV,                     \
                     bt_mesh_priv_beacon_srv_op, NULL, NULL,               \
                     &bt_mesh_priv_beacon_srv_cb)

#ifdef __cplusplus
}
#endif

#endif /* __PRIV_BEACON_SRV_H__ */
