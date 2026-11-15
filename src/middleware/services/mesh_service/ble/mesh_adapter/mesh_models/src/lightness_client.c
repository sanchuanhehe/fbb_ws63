/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light Lightness Client Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "mesh/mesh.h"
#include "model/lightness_client.h"

#define LIGHTNESS_VAL_LEN 5

struct bt_mesh_gen_model_cli *light_lightness_cli;

static int light_lightness_actual_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct light_lightness_actual_status *param = (struct light_lightness_actual_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS) {
        BT_WARN("Unexpected Light Lightness Actual Status message");
        return -EINVAL;
    }

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    param->actual = net_buf_simple_pull_le16(buf);
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            break;
        case 0x03: /* Optional fields are available */
            param->op_en = 1;
            param->target_actual = net_buf_simple_pull_le16(buf);
            param->remain_time = net_buf_simple_pull_u8(buf);
            break;
        default:
            return -EINVAL;
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_lightness_linear_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct light_lightness_linear_status *param = (struct light_lightness_linear_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS) {
        BT_WARN("Unexpected  Light Lightness Linear Status message");
        return -EINVAL;
    }

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    param->linear = net_buf_simple_pull_le16(buf);
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            break;
        case 0x03: /* Optional fields are available */
            param->op_en = 1;
            param->target_linear = net_buf_simple_pull_le16(buf);
            param->remain_time = net_buf_simple_pull_u8(buf);
            break;
        default:
            return -EINVAL;
    }
    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_lightness_last_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint16_t *param = (uint16_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS) {
        BT_WARN("Unexpected Last Light Lightness Status message");
        return -EINVAL;
    }

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_lightness_default_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
    struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint16_t *param = (uint16_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS) {
        BT_WARN("Unexpected Light Lightness Default Status message");
        return -EINVAL;
    }

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_lightness_range_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct light_lightness_range_status *param = (struct light_lightness_range_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS) {
        BT_WARN("Unexpected Light Lightness Range Status message");
        return -EINVAL;
    }

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    param->status_code = net_buf_simple_pull_u8(buf);
    param->light_range_min = net_buf_simple_pull_le16(buf);
    param->light_range_max = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

const struct bt_mesh_model_op light_lightness_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, 2, light_lightness_actual_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, 2, light_lightness_linear_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS, 2, light_lightness_last_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, 2, light_lightness_default_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS, 5, light_lightness_range_status },
    BT_MESH_MODEL_OP_END,
};

static int light_lightness_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int light_lightness_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Light Lightness Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int light_lightness_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Light Lightness Client context provided");
        return -EINVAL;
    }
    cli->model = model;

    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = light_lightness_pub_update;
    }

    k_sem_init(&cli->op_sync, 0, 1);

    if (light_lightness_cli == NULL) {
        light_lightness_cli = cli;
    }

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_lightness_cli_cb = {
    .init = light_lightness_cli_init,
    .deinit = light_lightness_cli_deinit,
};

int bt_mesh_lightness_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    int err;
    uint32_t wait_op;

    bt_mesh_model_msg_init(msg, common->opcode);
    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS;
            break;
        default:
            err = -EINVAL;
            BT_ERR("Invaild opcode");
            goto done;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}
static int bt_mesh_lightness_set_msg(struct os_mbuf *msg, bt_mesh_client_common_param_t *common,
    struct light_lightness_set *value, bool *need_ack, uint32_t *wait_op)
{
    bt_mesh_model_msg_init(msg, common->opcode);
    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
            net_buf_simple_add_le16(msg, value->actual);
            net_buf_simple_add_u8(msg, value->tid);
            if (value->op_en) {
                net_buf_simple_add_u8(msg, value->trans_time);
                net_buf_simple_add_u8(msg, value->delay);
            }
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK:
            net_buf_simple_add_le16(msg, value->linear);
            net_buf_simple_add_u8(msg, value->tid);
            if (value->op_en) {
                net_buf_simple_add_u8(msg, value->trans_time);
                net_buf_simple_add_u8(msg, value->delay);
            }
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK:
            net_buf_simple_add_le16(msg, value->def);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK:
            net_buf_simple_add_le16(msg, value->light_range_min);
            net_buf_simple_add_le16(msg, value->light_range_max);
            break;
        default:
            BT_ERR("Invaild opcode");
            return -EINVAL;
    }
    return BLE_MESH_SUCC;
}

int bt_mesh_lightness_set(bt_mesh_client_common_param_t *common, struct light_lightness_set *set, void *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    uint32_t wait_op;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHTNESS_VAL_LEN);
    struct light_lightness_set *value = set;

    err = bt_mesh_lightness_set_msg(msg, common, value, &need_ack, &wait_op);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("set_msg err %d", err);
        goto done;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send err %d", err);
        goto done;
    }

    if (!need_ack || !status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}

struct bt_mesh_model *bt_mesh_get_light_lightness_cli_model(void)
{
    if (light_lightness_cli) {
        return (void *)light_lightness_cli->model;
    }

    return NULL;
}