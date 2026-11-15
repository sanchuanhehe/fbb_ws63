/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon Client Model.
 */

#include "mesh/mesh.h"
#include "net.h"
#include "foundation.h"
#include "mesh/access.h"
#include "mesh/msg.h"
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "model/priv_beacon_cli.h"

#define PRIV_BEACON_2_BYTE_OP  BT_MESH_MODEL_OP_2(0xff, 0xff)
#define SUM_TIMEOUT 6

static struct bt_mesh_priv_beacon_cli_t *cli;
static int32_t msg_timeout;

static int bt_mesh_priv_msg_send_wait(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg,
    const struct bt_mesh_send_cb *cb, void *cb_data)
{
    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err) {
        BT_ERR("bt_mesh_model_send() failed (err %d)", err);
        bt_mesh_msg_ack_ctx_clear(&cli->ack_ctx);
        return err;
    }

    return bt_mesh_msg_ack_ctx_wait(&cli->ack_ctx, K_MSEC(msg_timeout));
}

static int bt_mesh_priv_handle_beacon_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_priv_beacon_t *rsp;
    uint8_t beacon, rand_int;

    beacon = net_buf_simple_pull_u8(buf);
    rand_int = net_buf_simple_pull_u8(buf);

    if (beacon != BT_MESH_BEACON_DISABLED &&
        beacon != BT_MESH_BEACON_ENABLED) {
        BT_WARN("Invalid beacon value 0x%02x", beacon);
        return -EINVAL;
    }

    BT_DBG("0x%02x (%u s)", beacon, 10U * rand_int);

    if (bt_mesh_msg_ack_ctx_match(&cli->ack_ctx, OP_PRIV_BEACON_STATUS, ctx->addr, (void **)&rsp)) {
        rsp->enabled = beacon;
        rsp->rand_interval = rand_int;

        bt_mesh_msg_ack_ctx_rx(&cli->ack_ctx);
    }

    if (cli->cb && cli->cb->priv_beacon_status) {
        struct bt_mesh_priv_beacon_t state = {
            .enabled = beacon,
            .rand_interval = rand_int,
        };

        cli->cb->priv_beacon_status(cli, ctx->addr, &state);
    }

    return 0;
}

static int bt_mesh_priv_handle_gatt_proxy_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t *rsp;
    uint8_t proxy;

    proxy = net_buf_simple_pull_u8(buf);
    if (proxy != BT_MESH_GATT_PROXY_DISABLED &&
        proxy != BT_MESH_GATT_PROXY_ENABLED &&
        proxy != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
        BT_WARN("Invalid GATT proxy value 0x%02x", proxy);
        return -EINVAL;
    }

    if (bt_mesh_msg_ack_ctx_match(&cli->ack_ctx,
                                  OP_PRIV_GATT_PROXY_STATUS,
                                  ctx->addr,
                                  (void **)&rsp)) {
        *rsp = proxy;

        bt_mesh_msg_ack_ctx_rx(&cli->ack_ctx);
    }

    if (cli->cb && cli->cb->priv_gatt_proxy_status) {
        cli->cb->priv_gatt_proxy_status(cli, ctx->addr, proxy);
    }

    return 0;
}

static int bt_mesh_priv_handle_node_id_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_priv_node_id_t *rsp;
    uint8_t status, node_id;
    uint16_t net_idx;

    status = net_buf_simple_pull_u8(buf);
    net_idx = net_buf_simple_pull_le16(buf);
    node_id = net_buf_simple_pull_u8(buf);
    if (node_id != BT_MESH_NODE_IDENTITY_STOPPED &&
        node_id != BT_MESH_NODE_IDENTITY_RUNNING &&
        node_id != BT_MESH_NODE_IDENTITY_NOT_SUPPORTED) {
        BT_WARN("Invalid node ID value 0x%02x", node_id);
        return -EINVAL;
    }

    if (bt_mesh_msg_ack_ctx_match(&cli->ack_ctx,
                                  OP_PRIV_NODE_ID_STATUS,
                                  ctx->addr,
                                  (void **)&rsp)) {
        rsp->net_idx = net_idx;
        rsp->status = status;
        rsp->state = node_id;

        bt_mesh_msg_ack_ctx_rx(&cli->ack_ctx);
    }

    if (cli->cb && cli->cb->priv_node_id_status) {
        struct bt_mesh_priv_node_id_t state = {
            .net_idx = net_idx,
            .status = status,
            .state = node_id,
        };

        cli->cb->priv_node_id_status(cli, ctx->addr, &state);
    }

    return 0;
}

const struct bt_mesh_model_op bt_mesh_priv_beacon_cli_op[] = {
    { OP_PRIV_BEACON_STATUS, BT_MESH_LEN_EXACT(2), bt_mesh_priv_handle_beacon_status },
    { OP_PRIV_GATT_PROXY_STATUS, BT_MESH_LEN_EXACT(1), bt_mesh_priv_handle_gatt_proxy_status },
    { OP_PRIV_NODE_ID_STATUS, BT_MESH_LEN_EXACT(4), bt_mesh_priv_handle_node_id_status },
    BT_MESH_MODEL_OP_END,
};

static int priv_beacon_cli_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!model->user_data) {
        BT_ERR("No Configuration Private Beacon Client context provided");
        return -EINVAL;
    }
    struct bt_mesh_priv_beacon_cli_t *cli_tmp = model->user_data;
    bt_mesh_msg_ack_ctx_deinit(&cli_tmp->ack_ctx);

    return 0;
}

static int priv_beacon_cli_init(struct bt_mesh_model *model)
{
    if (!bt_mesh_model_in_primary(model)) {
        BT_ERR("Private Beacon Client only allowed in primary element");
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("No Configuration Private Beacon Client context provided");
        return -EINVAL;
    }

    cli = model->user_data;
    cli->model = model;
    msg_timeout = SUM_TIMEOUT * MSEC_PER_SEC;
    model->keys[0] = BT_MESH_KEY_DEV_ANY;
    model->flags |= BT_MESH_MOD_DEVKEY_ONLY;

    bt_mesh_msg_ack_ctx_init(&cli->ack_ctx);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_priv_beacon_cli_cb_t = {
    .init = priv_beacon_cli_init,
    .deinit = priv_beacon_cli_deinit,
};

static int bt_mesh_priv_beacon_cli_prepare(uint8_t *param, uint32_t op, uint16_t addr)
{
    if (!cli) {
        BT_ERR("No available Configuration Client context!");
        return -EINVAL;
    }

    return bt_mesh_msg_ack_ctx_prepare(&cli->ack_ctx, op, addr, param);
}

int bt_mesh_priv_beacon_cli_set(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_beacon_t val,
                                struct bt_mesh_priv_beacon_t *rsp)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 2);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };
    
    int err = bt_mesh_priv_beacon_cli_prepare((uint8_t *)rsp, OP_PRIV_BEACON_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_BEACON_SET);
    net_buf_simple_add_u8(msg, val.enabled);
    if (val.op_en == 1) {
        net_buf_simple_add_u8(msg, val.rand_interval);
    }

    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_priv_beacon_cli_get(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_beacon_t *val)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 0);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };
    int err = 0;

    err = bt_mesh_priv_beacon_cli_prepare((uint8_t *)val, OP_PRIV_BEACON_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_BEACON_GET);

    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}


int bt_mesh_priv_beacon_cli_gatt_proxy_set(uint16_t net_idx, uint16_t addr, uint8_t val, uint8_t *rsp)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 1);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };

    if ((val != BT_MESH_GATT_PROXY_DISABLED && val != BT_MESH_GATT_PROXY_ENABLED)) {
        return -EINVAL;
    }

    int err = 0;

    err = bt_mesh_priv_beacon_cli_prepare(rsp, OP_PRIV_GATT_PROXY_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_GATT_PROXY_SET);
    net_buf_simple_add_u8(msg, val);

    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_priv_beacon_cli_gatt_proxy_get(uint16_t net_idx, uint16_t addr, uint8_t *val)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 0);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };
    
    int err = bt_mesh_priv_beacon_cli_prepare(val, OP_PRIV_GATT_PROXY_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_GATT_PROXY_GET);

    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_priv_beacon_cli_node_id_set(uint16_t net_idx, uint16_t addr, struct bt_mesh_priv_node_id_t val,
                                        struct bt_mesh_priv_node_id_t *rsp)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 3);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };
    int err = 0;

    if (val.net_idx > 0xfff || (val.state != BT_MESH_NODE_IDENTITY_STOPPED &&
        val.state != BT_MESH_NODE_IDENTITY_RUNNING)) {
        return -EINVAL;
    }

    err = bt_mesh_priv_beacon_cli_prepare((uint8_t *)rsp, OP_PRIV_NODE_ID_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_NODE_ID_SET);
    net_buf_simple_add_le16(msg, val.net_idx);
    net_buf_simple_add_u8(msg, val.state);

    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_priv_beacon_cli_node_id_get(uint16_t net_idx, uint16_t addr, uint16_t key_net_idx,
                                        struct bt_mesh_priv_node_id_t *val)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(PRIV_BEACON_2_BYTE_OP, 2);
    struct bt_mesh_msg_ctx ctx = {
        .net_idx = net_idx,
        .app_idx = BT_MESH_KEY_DEV_REMOTE,
        .addr = addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
    };

    int err = bt_mesh_priv_beacon_cli_prepare((uint8_t *)val, OP_PRIV_NODE_ID_STATUS, addr);
    if (err) {
        goto done;
    }

    bt_mesh_model_msg_init(msg, OP_PRIV_NODE_ID_GET);
    net_buf_simple_add_le16(msg, key_net_idx);
    err = bt_mesh_priv_msg_send_wait((struct bt_mesh_model *)(cli->model), &ctx, msg, NULL, NULL);
done:
    os_mbuf_free_chain(msg);
    return err;
}