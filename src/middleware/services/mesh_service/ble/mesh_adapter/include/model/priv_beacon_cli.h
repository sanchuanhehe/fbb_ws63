/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon Client Model.
 */

#ifndef __PRIV_BEACON_CLI_H__
#define __PRIV_BEACON_CLI_H__

#include "mesh/mesh.h"
#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op bt_mesh_priv_beacon_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_priv_beacon_cli_cb_t;

#define BT_MESH_MODEL_PRIV_BEACON_CLI(cli_data)                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_PRIV_BEACON_CLI,                     \
                     bt_mesh_priv_beacon_cli_op, NULL, cli_data,           \
                     &bt_mesh_priv_beacon_cli_cb_t)

struct bt_mesh_priv_beacon_t {
    uint8_t enabled;
    uint8_t rand_interval;
    uint8_t op_en;
};

/** Private Node Identity */
struct bt_mesh_priv_node_id_t {
    uint16_t net_idx;
    uint8_t state;
    uint8_t status;
};

struct bt_mesh_priv_beacon_cli_t {
    const struct bt_mesh_model *model;
    struct bt_mesh_msg_ack_ctx ack_ctx;
    const struct bt_mesh_priv_beacon_cli_cb_t *cb;
};

struct bt_mesh_priv_beacon_cli_cb_t {
    void (*priv_beacon_status)(struct bt_mesh_priv_beacon_cli_t *cli, uint16_t addr,
                               struct bt_mesh_priv_beacon_t *priv_beacon);
    void (*priv_gatt_proxy_status)(struct bt_mesh_priv_beacon_cli_t *cli, uint16_t addr,
                                   uint8_t gatt_proxy);
    void (*priv_node_id_status)(struct bt_mesh_priv_beacon_cli_t *cli, uint16_t addr,
                                struct bt_mesh_priv_node_id_t *priv_node_id);
};

int bt_mesh_priv_beacon_cli_set(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_beacon_t val,
                                struct bt_mesh_priv_beacon_t *rsp);
int bt_mesh_priv_beacon_cli_get(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_beacon_t *val);
int bt_mesh_priv_beacon_cli_gatt_proxy_set(uint16_t net_idx, uint16_t addr, uint8_t val, uint8_t *rsp);
int bt_mesh_priv_beacon_cli_gatt_proxy_get(uint16_t net_idx, uint16_t addr,  uint8_t *val);
int bt_mesh_priv_beacon_cli_node_id_set(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_node_id_t val,
                                        struct bt_mesh_priv_node_id_t *rsp);
int bt_mesh_priv_beacon_cli_node_id_get(uint16_t net_idx, uint16_t addr, uint16_t key_net_idx,
                                        struct bt_mesh_priv_node_id_t *val);
#ifdef __cplusplus
}
#endif

#endif /* __PRIV_BEACON_CLI_H__ */
