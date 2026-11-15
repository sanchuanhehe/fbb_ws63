/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scheduler Client Model Function Implementation
 *
 * History:
 * 2025-5-21, Create file.
 */
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "mesh/mesh.h"
#include "model/scheduler_client.h"

#define SCHEDULER_ACT_VAL_LEN (10 + 4)

struct bt_mesh_gen_model_cli *scheduler_cli;

static int bt_mesh_scheduler_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_scheduler_status_t *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_SCHEDULER_STATUS) {
        BT_WARN("Unexpected Scheduler Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_scheduler_status_t *)cli->op_param;

    param->schedules = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int bt_mesh_scheduler_act_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_scheduler_act_t *param;
    uint64_t value = 0;

    if (cli->op_pending != BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS) {
        BT_WARN("Unexpected Scheduler Act Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_scheduler_act_t *)cli->op_param;

    value = net_buf_simple_pull_le32(buf);
    value |= ((uint64_t)net_buf_simple_pull_le32(buf) << 32); // 32:bit offset

    param->index = value & BIT_MASK(4); // 4:bit width
    param->year = (value >> 4) & BIT_MASK(7); // 4:bit offset, 7:bit width
    param->month = (value >> 11) & BIT_MASK(12); // 11:bit offset, 12:bit width
    param->day = (value >> 23) & BIT_MASK(5); // 23:bit offset, 5:bit width
    param->hour = (value >> 28) & BIT_MASK(5); // 28:bit offset, 5:bit width
    param->minute = (value >> 33) & BIT_MASK(6); // 33:bit offset, 6:bit width
    param->second = (value >> 39) & BIT_MASK(6); // 39:bit offset, 6:bit width
    param->day_of_week = (value >> 45) & BIT_MASK(7); // 45:bit offset, 7:bit width
    param->action = (value >> 52) & BIT_MASK(4); // 52:bit offset, 4:bit width
    param->trans_time = (value >> 56) & BIT_MASK(8); // 56:bit offset, 8:bit width
    param->scene_number = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

const struct bt_mesh_model_op bt_mesh_scheduler_cli_op[] = {
    {BLE_MESH_MODEL_OP_SCHEDULER_STATUS, 2, bt_mesh_scheduler_status},
    {BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS, 10, bt_mesh_scheduler_act_status},
    BT_MESH_MODEL_OP_END,
};

int bt_mesh_scheduler_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + 4);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCHEDULER_GET);
    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_SCHEDULER_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_scheduler_act_get(bt_mesh_client_common_param_t *common, void *status, uint8_t index)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + 1 + 4);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET);
    net_buf_simple_add_u8(msg, index);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_scheduler_act_set(bt_mesh_client_common_param_t *common, struct bt_mesh_scheduler_act_t *set,
    struct bt_mesh_scheduler_act_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + SCHEDULER_ACT_VAL_LEN);
    uint64_t val = 0;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET:
            need_ack = true;
        case BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK: {
            bt_mesh_model_msg_init(msg, common->opcode);
            break;
        }
        default:
            err = -EINVAL;
            BT_ERR("Invaild opcode");
            goto done;
    }

    val = ((uint64_t)(set->year) << 4) | set->index; // 4:bit offset
    val |= ((uint64_t)(set->day) << 23) | ((uint64_t)(set->month) << 11); // 23:bit offset, 11:bit offset
    val |= ((uint64_t)(set->minute) << 33) | ((uint64_t)(set->hour) << 28); // 33:bit offset, 28:bit offset
    val |= ((uint64_t)(set->day_of_week) << 45) | ((uint64_t)(set->second) << 39); // 45:bit offset, 39:bit offset
    val |= ((uint64_t)(set->trans_time) << 56) | ((uint64_t)(set->action) << 52); // 56:bit offset, 52:bit offset

    net_buf_simple_add_le32(msg, (uint32_t)val);
    net_buf_simple_add_le32(msg, (uint32_t)(val >> 32)); // 32:bit offset
    net_buf_simple_add_le16(msg, set->scene_number);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    if (!need_ack || !status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}
static int scheduler_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}
static int bt_mesh_scheduler_client_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Scheduler Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = scheduler_cli_pub_update;
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (scheduler_cli == NULL) {
        scheduler_cli = cli;
    }

    return 0;
}

static int bt_mesh_scheduler_client_deinit(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Scheduler Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_scheduler_cli_cb = {
    .init = bt_mesh_scheduler_client_init,
    .deinit = bt_mesh_scheduler_client_deinit,
};

void *bt_mesh_get_scheduler_cli_model(void)
{
    if (scheduler_cli) {
        return (void *)scheduler_cli->model;
    }

    return NULL;
}
