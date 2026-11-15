/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light CTL Client Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include <stdio.h>
#include "syscfg/syscfg.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "model/light_ctl_client.h"

struct bt_mesh_gen_model_cli *light_ctl_cli;

#define BLE_MESH_LIGHT_CTL_SET_MSG_LEN (2 + 9 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN (2 + 7 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN (2 + 4 + 4)
#define BLE_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN (2 + 6 + 4)
#define BLE_MESH_LIGHT_CTL_GET_STATE_MSG_LEN (2 + 2 + 4)

#define LIGHT_CTL_OPTIONAL_LEN (5)
#define LIGHT_CTL_MSG_TIME K_SECONDS(5)

/* Light CTL Client message handlers */
static int light_ctl_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_light_ctl_status *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS) {
        BT_WARN("Unexpected Light Ctl Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_light_ctl_status *)cli->op_param;
    param->present_ctl_lightness = net_buf_simple_pull_le16(buf);
    param->present_ctl_temperature = net_buf_simple_pull_le16(buf);
    param->op_en = false;

    if (buf->om_len == LIGHT_CTL_OPTIONAL_LEN) {
        param->target_ctl_lightness = net_buf_simple_pull_le16(buf);
        param->target_ctl_temperature = net_buf_simple_pull_le16(buf);
        param->remain_time = net_buf_simple_pull_u8(buf);
        param->op_en = true;
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_ctl_temp_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_light_ctl_temperature_status *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS) {
        BT_WARN("Unexpected Light Ctl Temp Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_light_ctl_temperature_status *)cli->op_param;
    param->present_ctl_temperature = net_buf_simple_pull_le16(buf);
    param->present_ctl_delta_uv = net_buf_simple_pull_le16(buf);
    param->op_en = false;

    if (buf->om_len == LIGHT_CTL_OPTIONAL_LEN) {
        param->target_ctl_temperature = net_buf_simple_pull_le16(buf);
        param->target_ctl_delta_uv = net_buf_simple_pull_le16(buf);
        param->remain_time = net_buf_simple_pull_u8(buf);
        param->op_en = true;
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_ctl_temp_range_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_light_ctl_temperature_range_status *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS) {
        BT_WARN("Unexpected Light Ctl Temp Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_light_ctl_temperature_range_status *)cli->op_param;
    param->status_code = net_buf_simple_pull_u8(buf);
    param->range_min = net_buf_simple_pull_le16(buf);
    param->range_max = net_buf_simple_pull_le16(buf);

    BT_INFO("Acknownledgement from LIGHT_CTL_SRV (Temperature Range)");
    BT_INFO("Status Code = %u", param->status_code);
    BT_INFO("Range Min = %u", param->range_min);
    BT_INFO("Range Max = %u", param->range_max);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int light_ctl_default_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_light_ctl_default_status *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS) {
        BT_WARN("Unexpected Light Ctl Temp Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_light_ctl_default_status *)cli->op_param;
    param->lightness = net_buf_simple_pull_le16(buf);
    param->temperature = net_buf_simple_pull_le16(buf);
    param->delta_uv = net_buf_simple_pull_le16(buf);

    BT_INFO("Acknownledgement from LIGHT_CTL_SRV (Default)");
    BT_INFO("Lightness = %u", param->lightness);
    BT_INFO("Temperature = %u", param->temperature);
    BT_INFO("Delta UV = %d", param->delta_uv);

    k_sem_give(&cli->op_sync);
    return 0;
}

const struct bt_mesh_model_op bt_mesh_light_ctl_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS,                   4, light_ctl_status },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS,       4, light_ctl_temp_status },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS, 5, light_ctl_temp_range_status},
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS,           6, light_ctl_default_status },
    BT_MESH_MODEL_OP_END,
};

int bt_mesh_light_ctl_client_get_state(bt_mesh_client_common_param_t *common, void *get)
{
    if (light_ctl_cli == NULL || common == NULL || get == NULL) {
        BT_ERR("Model not exist or Invalid param!");
        return -EINVAL;
    }

    uint32_t wait_op;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
            wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS;
            break;
        default:
            BT_ERR("Invalid Lighting Get opcode 0x%04x", common->opcode);
            return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_LIGHT_CTL_GET_STATE_MSG_LEN);
    bt_mesh_model_msg_init(msg, common->opcode);

    int err = bt_mesh_model_send(light_ctl_cli->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model send failed (err %d)", err);
        goto done;
    }

    light_ctl_cli->op_param = get;
    light_ctl_cli->op_pending = wait_op;
    err = k_sem_take(&light_ctl_cli->op_sync, LIGHT_CTL_MSG_TIME);
    light_ctl_cli->op_param = NULL;
    light_ctl_cli->op_pending = 0;

done:
    os_mbuf_free_chain(msg);
    return err;
}

static int light_ctl_op_set(bt_mesh_client_common_param_t *common, struct bt_mesh_light_ctl_set *set,
    struct os_mbuf **msg, bool *need_ack, uint32_t *wait_op)
{
    if (common->opcode == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
        *need_ack = true;
        *wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS;
    }

    struct bt_mesh_light_ctl_set *value = set;
    if (value->op_en) {
        if ((value->trans_time & 0x3F) > 0x3E) {
            BT_ERR("Invalid Light CTL Set transition time");
            return -EINVAL;
        }
    }
    *msg = NET_BUF_SIMPLE(BLE_MESH_LIGHT_CTL_SET_MSG_LEN);
    bt_mesh_model_msg_init(*msg, common->opcode);
    net_buf_simple_add_le16(*msg, value->ctl_lightness);
    net_buf_simple_add_le16(*msg, value->ctl_temperature);
    net_buf_simple_add_le16(*msg, value->ctl_delta_uv);
    net_buf_simple_add_u8(*msg, value->tid);
    if (value->op_en) {
        net_buf_simple_add_u8(*msg, value->trans_time);
        net_buf_simple_add_u8(*msg, value->delay);
    }

    return 0;
}

static int light_ctl_temp_op_set(bt_mesh_client_common_param_t *common, struct bt_mesh_light_ctl_temperature_set *set,
    struct os_mbuf **msg, bool *need_ack, uint32_t *wait_op)
{
    if (common->opcode == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
        *need_ack = true;
        *wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS;
    }

    struct bt_mesh_light_ctl_temperature_set *value = set;
    if (value->op_en) {
        if ((value->trans_time & 0x3F) > 0x3E) {
            BT_ERR("Invalid Light CTL Temperature Set transition time");
            return -EINVAL;
        }
    }
    *msg = NET_BUF_SIMPLE(BLE_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN);
    bt_mesh_model_msg_init(*msg, common->opcode);
    net_buf_simple_add_le16(*msg, value->ctl_temperature);
    net_buf_simple_add_le16(*msg, value->ctl_delta_uv);
    net_buf_simple_add_u8(*msg, value->tid);
    if (value->op_en) {
        net_buf_simple_add_u8(*msg, value->trans_time);
        net_buf_simple_add_u8(*msg, value->delay);
    }

    return 0;
}

static int light_ctl_temp_range_op_set(bt_mesh_client_common_param_t *common,
    struct bt_mesh_light_ctl_temperature_range_set *set, struct os_mbuf **msg, bool *need_ack, uint32_t *wait_op)
{
    if (common->opcode == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET) {
        *need_ack = true;
        *wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS;
    }

    struct bt_mesh_light_ctl_temperature_range_set *value = set;
    if (value->range_min > value->range_max) {
        BT_ERR("Light CTL Temperature Range Set range min is greater than range max");
        return -EINVAL;
    }

    *msg = NET_BUF_SIMPLE(BLE_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN);
    bt_mesh_model_msg_init(*msg, common->opcode);
    net_buf_simple_add_le16(*msg, value->range_min);
    net_buf_simple_add_le16(*msg, value->range_max);

    return 0;
}

static int light_ctl_default_op_set(bt_mesh_client_common_param_t *common, struct bt_mesh_light_ctl_default_set *set,
    struct os_mbuf **msg, bool *need_ack, uint32_t *wait_op)
{
    if (common->opcode == BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET) {
        *need_ack = true;
        *wait_op = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS;
    }

    struct bt_mesh_light_ctl_default_set *value = set;

    *msg = NET_BUF_SIMPLE(BLE_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN);
    bt_mesh_model_msg_init(*msg, common->opcode);
    net_buf_simple_add_le16(*msg, value->lightness);
    net_buf_simple_add_le16(*msg, value->temperature);
    net_buf_simple_add_le16(*msg, value->delta_uv);

    return 0;
}

static void light_ctl_cli_wait(void *ack_status, uint32_t wait_op, int *err)
{
    light_ctl_cli->op_param = ack_status;
    light_ctl_cli->op_pending = wait_op;
    *err = k_sem_take(&light_ctl_cli->op_sync, LIGHT_CTL_MSG_TIME);
    light_ctl_cli->op_param = NULL;
    light_ctl_cli->op_pending = 0;
}

int bt_mesh_light_ctl_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *ack_status)
{
    bool need_ack = false;
    uint32_t wait_op;
    struct os_mbuf *msg = NULL;
    int ret;

    if (!light_ctl_cli || !common || !common->model || !set) {
        BT_ERR("Model not exist or Invalid param!");
        return -EINVAL;
    }

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
            ret = light_ctl_op_set(common, (struct bt_mesh_light_ctl_set *)set, &msg, &need_ack, &wait_op);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK:
            ret = light_ctl_temp_op_set(common, (struct bt_mesh_light_ctl_temperature_set *)set, &msg, &need_ack,
                &wait_op);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK:
            ret = light_ctl_temp_range_op_set(common, (struct bt_mesh_light_ctl_temperature_range_set *)set, &msg,
                &need_ack, &wait_op);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK:
            ret = light_ctl_default_op_set(common, (struct bt_mesh_light_ctl_default_set *)set, &msg, &need_ack,
                &wait_op);
            break;
        default:
            BT_ERR("Invalid Lighting Set opcode 0x%04x", common->opcode);
            return -EINVAL;
    }

    if (ret != 0) {
        return ret;
    }

    int err = bt_mesh_model_send(light_ctl_cli->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model send failed (err %d)", err);
        goto done;
    }

    if (!need_ack || ack_status == NULL) {
        goto done;
    }

    light_ctl_cli_wait(ack_status, wait_op, &err);

done:
    os_mbuf_free_chain(msg);
    return err;
}
static int light_ctl_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int light_ctl_client_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Light ctl Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int light_ctl_client_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Light ctl Client context provided");
        return -EINVAL;
    }
    cli->model = model;

    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = light_ctl_pub_update;
    }

    k_sem_init(&cli->op_sync, 0, 1);

    if (light_ctl_cli == NULL) {
        light_ctl_cli = cli;
    }

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_ctl_cli_cb = {
    .init = light_ctl_client_init,
    .deinit = light_ctl_client_deinit,
};

struct bt_mesh_model *bt_mesh_get_light_ctl_cli_model(void)
{
    if (light_ctl_cli) {
        return (void *)light_ctl_cli->model;
    }

    return NULL;
}
