/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon Server Model.
 */

#include "mesh/mesh.h"
#include "net.h"
#include "proxy.h"
#include "foundation.h"
#include "beacon.h"
#include "priv_beacon.h"
#include "cfg.h"
#include "settings.h"
#include "mesh/access.h"
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "model/priv_beacon_srv.h"

static struct bt_mesh_model *priv_beacon_srv;

struct {
    uint8_t state;
    uint8_t interval;
    uint8_t proxy_state;
} priv_beacon_state;

static int bt_mesh_priv_beacon_store(bool delete)
{
    if (!(IS_ENABLED(CONFIG_BT_SETTINGS) || IS_ENABLED(CONFIG_BT_SETTINGS_EXT))) {
        return 0;
    }

    const void *data = delete ? NULL : &priv_beacon_state;
    size_t len = delete ? 0 : sizeof(priv_beacon_state);

    return bt_mesh_model_data_store(priv_beacon_srv, false, "pb", data, len);
}

static int bt_mesh_priv_beacon_status_rsp(struct bt_mesh_model *mod,
                                          struct bt_mesh_msg_ctx *ctx)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(OP_PRIV_BEACON_STATUS, 2);
    bt_mesh_model_msg_init(msg, OP_PRIV_BEACON_STATUS);

    net_buf_simple_add_u8(msg, bt_mesh_priv_beacon_get());
    net_buf_simple_add_u8(msg, bt_mesh_priv_beacon_update_interval_get());

    bt_mesh_model_send(mod, ctx, msg, NULL, NULL);

    os_mbuf_free_chain(msg);
    return 0;
}

static int bt_mesh_handle_priv_beacon_get(struct bt_mesh_model *mod,
                                          struct bt_mesh_msg_ctx *ctx,
                                          struct os_mbuf *buf)
{
    bt_mesh_priv_beacon_status_rsp(mod, ctx);

    return 0;
}

static int bt_mesh_handle_priv_beacon_set(struct bt_mesh_model *mod,
                                          struct bt_mesh_msg_ctx *ctx,
                                          struct os_mbuf *buf)
{
    uint8_t beacon;

    if (buf->om_len > 2U) {
        return -EMSGSIZE;
    }

    beacon = net_buf_simple_pull_u8(buf);
    BT_INFO("beacon: %u", beacon);
    if (beacon != BT_MESH_BEACON_DISABLED &&
        beacon != BT_MESH_BEACON_ENABLED) {
        BT_WARN("Invalid beacon value %u", beacon);
        return -EINVAL;
    }

    if (buf->om_len >= 1) { // 1：len
        bt_mesh_priv_beacon_update_interval_set(net_buf_simple_pull_u8(buf));
    }

    (void)bt_mesh_priv_beacon_set(beacon);
    bt_mesh_priv_beacon_status_rsp(mod, ctx);

    return 0;
}

static void gatt_proxy_status_rsp(struct bt_mesh_model *mod,
                                  struct bt_mesh_msg_ctx *ctx)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(OP_PRIV_GATT_PROXY_STATUS, 1);
    bt_mesh_model_msg_init(msg, OP_PRIV_GATT_PROXY_STATUS);

    net_buf_simple_add_u8(msg, bt_mesh_priv_gatt_proxy_get());

    bt_mesh_model_send(mod, ctx, msg, NULL, NULL);

    os_mbuf_free_chain(msg);
}

static int bt_mesh_handle_gatt_proxy_get(struct bt_mesh_model *mod,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct os_mbuf *buf)
{
    BT_DBG("");

    gatt_proxy_status_rsp(mod, ctx);

    return 0;
}

static int bt_mesh_handle_gatt_proxy_set(struct bt_mesh_model *mod,
                                         struct bt_mesh_msg_ctx *ctx,
                                         struct os_mbuf *buf)
{
    uint8_t gatt_proxy;

    gatt_proxy = net_buf_simple_pull_u8(buf);
    if (gatt_proxy != BT_MESH_GATT_PROXY_DISABLED &&
        gatt_proxy != BT_MESH_GATT_PROXY_NOT_SUPPORTED &&
        gatt_proxy != BT_MESH_GATT_PROXY_ENABLED) {
        BT_WARN("Invalid GATT proxy value %u", gatt_proxy);
        return -EINVAL;
    }

    BT_DBG("%u", gatt_proxy);

    bt_mesh_priv_gatt_proxy_set(gatt_proxy);

    gatt_proxy_status_rsp(mod, ctx);

    return 0;
}

static void bt_mesh_node_id_status_rsp(struct bt_mesh_model *mod,
                                       struct bt_mesh_msg_ctx *ctx, uint8_t status,
                                       uint16_t net_idx, uint8_t node_id)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(OP_PRIV_NODE_ID_STATUS, 4);
    bt_mesh_model_msg_init(msg, OP_PRIV_NODE_ID_STATUS);

    net_buf_simple_add_u8(msg, status);
    net_buf_simple_add_le16(msg, net_idx);
    net_buf_simple_add_u8(msg, node_id);

    bt_mesh_model_send(mod, ctx, msg, NULL, NULL);

    os_mbuf_free_chain(msg);
}

static int bt_mesh_handle_node_id_get(struct bt_mesh_model *mod,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct os_mbuf *buf)
{
    uint8_t node_id, status;
    uint16_t net_idx;

    net_idx = net_buf_simple_pull_le16(buf) & 0xfff;

    status = bt_mesh_subnet_priv_node_id_get(net_idx, (enum bt_mesh_feat_state *)&node_id);
    bt_mesh_node_id_status_rsp(mod, ctx, status, net_idx, node_id);

    return 0;
}

static int bt_mesh_handle_node_id_set(struct bt_mesh_model *mod,
                                      struct bt_mesh_msg_ctx *ctx,
                                      struct os_mbuf *buf)
{
    uint8_t node_id, status;
    uint16_t net_idx;

    net_idx = net_buf_simple_pull_le16(buf) & 0xfff;
    node_id = net_buf_simple_pull_u8(buf);
    if (node_id != BT_MESH_NODE_IDENTITY_RUNNING &&
        node_id != BT_MESH_NODE_IDENTITY_STOPPED) {
        BT_ERR("Invalid node ID value 0x%02x", node_id);
        return -EINVAL;
    }

    status = bt_mesh_subnet_priv_node_id_set(net_idx, node_id);
    bt_mesh_node_id_status_rsp(mod, ctx, status, net_idx, node_id);

    return 0;
}

const struct bt_mesh_model_op bt_mesh_priv_beacon_srv_op[] = {
    { OP_PRIV_BEACON_GET, BT_MESH_LEN_EXACT(0), bt_mesh_handle_priv_beacon_get },
    { OP_PRIV_BEACON_SET, BT_MESH_LEN_MIN(1), bt_mesh_handle_priv_beacon_set },
    { OP_PRIV_GATT_PROXY_GET, BT_MESH_LEN_EXACT(0), bt_mesh_handle_gatt_proxy_get },
    { OP_PRIV_GATT_PROXY_SET, BT_MESH_LEN_EXACT(1), bt_mesh_handle_gatt_proxy_set },
    { OP_PRIV_NODE_ID_GET, BT_MESH_LEN_EXACT(2), bt_mesh_handle_node_id_get },
    { OP_PRIV_NODE_ID_SET, BT_MESH_LEN_EXACT(3), bt_mesh_handle_node_id_set },
    BT_MESH_MODEL_OP_END
};

static int priv_beacon_srv_init(struct bt_mesh_model *model)
{
    int err;
    struct bt_mesh_model *config_srv =
        bt_mesh_model_find(bt_mesh_model_elem(model), BT_MESH_MODEL_ID_CFG_SRV);

    if (config_srv == NULL) {
        BT_ERR("Private Beacon server cannot extend Configuration server");
        return -EINVAL;
    }

    priv_beacon_srv = model;
    model->keys[0] = BT_MESH_KEY_DEV_LOCAL;

#if MYNEWT_VAL(BLE_MESH_MODEL_EXTENSIONS)
    err = bt_mesh_model_extend(model, config_srv);
    if (err) {
        return err;
    }
#endif

    return 0;
}

static int priv_beacon_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}
static void priv_beacon_srv_reset(struct bt_mesh_model *model)
{
    (void)memset_s(&priv_beacon_state, sizeof(priv_beacon_state), 0, sizeof(priv_beacon_state));
    bt_mesh_priv_beacon_store(true);
}

const struct bt_mesh_model_cb bt_mesh_priv_beacon_srv_cb = {
    .init = priv_beacon_srv_init,
    .reset = priv_beacon_srv_reset,
    .deinit = priv_beacon_srv_deinit,
};

void bt_mesh_priv_beacon_srv_store_schedule(void)
{
    return;
}
