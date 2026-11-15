/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: defintion for Cilent Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#ifndef __VND_CLIENT_H__
#define __VND_CLIENT_H__

#include "vnd_opcode.h"
#include "vnd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MSEH_VND_MOD_GET_LEN 3
#define BT_MSEH_VND_MOD_ONOFF_SET_LEN 4
#define BT_MSEH_VND_MOD_UNIX_TIME_SET_LEN 9
#define BT_MSEH_VND_MOD_POWER_PERCENT_SET_LEN 4

#define BT_MESH_VND_ATTR_DATA_UNIX_TIME_LEN (6)  // unix time len
#define BT_MESH_VND_ATTR_DATA_MAX_LEN (48)  // data max len

extern const struct bt_mesh_model_op g_bt_mesh_vnd_attr_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_vnd_attr_cli_cb;

#define HW_MESH_MODEL_VND_ATTR_CLI(user_data, pub) \
    BT_MESH_MODEL_VND_CB( \
        HW_CID, \
        HW_MESH_MODEL_VND_ID_CLI, \
        g_bt_mesh_vnd_attr_cli_op, \
        pub, \
        user_data, \
        &bt_mesh_vnd_attr_cli_cb)

int bt_mesh_vnd_attr_set_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_attr_get_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_attr_notify(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);
int bt_mesh_vnd_attr_notify_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg);

typedef union {
    uint8_t onoff;
    uint8_t unix_time[BT_MESH_VND_ATTR_DATA_UNIX_TIME_LEN];
    uint8_t power_percent;
} bt_mesh_vnd_data_union_t;

typedef struct {
    uint16_t attr_type;  // attr
    uint8_t data[BT_MESH_VND_ATTR_DATA_MAX_LEN];    // val
} bt_mesh_vnd_op_t;

typedef struct {
    uint8_t op_en;
    uint8_t data[BT_MESH_VND_ATTR_DATA_MAX_LEN];  // val
} bt_mesh_vnd_status_t;

void *bt_mesh_vnd_get_cli_model(void);
int bt_mesh_vnd_cli_get(bt_mesh_client_common_param_t *common, bt_mesh_vnd_op_t *set, bt_mesh_vnd_status_t *status);
int bt_mesh_vnd_cli_set(bt_mesh_client_common_param_t *common, bt_mesh_vnd_op_t *set, bt_mesh_vnd_status_t *status);

#ifdef __cplusplus
}
#endif

#endif
