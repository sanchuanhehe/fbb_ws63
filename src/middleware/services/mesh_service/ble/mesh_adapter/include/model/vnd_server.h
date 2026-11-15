/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: defintion for Server Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#ifndef __VND_SERVER_H__
#define __VND_SERVER_H__

#include "vnd_opcode.h"
#include "vnd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op g_bt_mesh_vnd_attr_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_vnd_attr_srv_cb;

#define HW_MESH_MODEL_VND_ATTR_SRV(pub) \
    BT_MESH_MODEL_VND_CB( \
        HW_CID, \
        HW_MESH_MODEL_VND_ID_SRV, \
        g_bt_mesh_vnd_attr_srv_op, \
        pub, \
        NULL, \
        &bt_mesh_vnd_attr_srv_cb)

int bt_mesh_vnd_attr_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_attr_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_attr_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_notify_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);

#ifdef __cplusplus
}
#endif

#endif
