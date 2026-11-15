/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Time Client Model Function Implementation
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
#include "model/time_client.h"

// Payload + MIC
#define TIME_SET_VAL_LEN (10 + 4)
#define TIMEZONE_SET_VAL_LEN (6 + 4)
#define TAI_UTC_DELTA_SET_VAL_LEN (7 + 4)
#define TIME_ROLE_SET_VAL_LEN (1 + 4)
#define MAX_MSG_LENGTH 10

struct bt_mesh_gen_model_cli *time_cli;

/* Time Client message handlers */
static int bt_mesh_client_time_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_time_status_t *param;
    uint8_t tai_seconds[TAI_SECONDS_LEN] = {0};
    uint16_t val = 0;

    if (cli->op_pending != BLE_MESH_MODEL_OP_TIME_STATUS) {
        BT_WARN("Unexpected Time Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_time_status_t *)cli->op_param;

    BT_WARN("buf->om_len %u", buf->om_len);
    if (buf->om_len < TAI_SECONDS_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(param->tai_seconds, TAI_SECONDS_LEN, net_buf_simple_pull_mem(buf, TAI_SECONDS_LEN), TAI_SECONDS_LEN);
    if ((memcmp(param->tai_seconds, tai_seconds, TAI_SECONDS_LEN) != 0) &&
        buf->om_len == MAX_MSG_LENGTH - TAI_SECONDS_LEN) {
        param->subsecond = net_buf_simple_pull_u8(buf);
        param->uncertainty = net_buf_simple_pull_u8(buf);
        val = net_buf_simple_pull_le16(buf);
        param->time_authority = val & 1;
        param->tai_utc_delta_curr = val >> 1;
        param->timezone_offset_curr = net_buf_simple_pull_u8(buf);
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int bt_mesh_timezone_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_timezone_status_t *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_TIME_ZONE_STATUS) {
        BT_WARN("Unexpected TimeZone Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_timezone_status_t *)cli->op_param;
    param->timezone_offset_curr = net_buf_simple_pull_u8(buf);
    param->timezone_offset_new = net_buf_simple_pull_u8(buf);
    if (buf->om_len < TAI_OF_ZONE_CHANGE_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(param->tai_zone_change,
             TAI_OF_ZONE_CHANGE_LEN,
             net_buf_simple_pull_mem(buf, TAI_OF_ZONE_CHANGE_LEN),
             TAI_OF_ZONE_CHANGE_LEN);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int bt_mesh_tai_utc_delta_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_tai_utc_delta_status_t *param;
    uint16_t val = 0;

    if (cli->op_pending != BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS) {
        BT_WARN("Unexpected Tai_utc_delta Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_tai_utc_delta_status_t *)cli->op_param;

    val = net_buf_simple_pull_le16(buf);
    param->tai_utc_delta_curr = val & 0x7FFF;
    val = net_buf_simple_pull_le16(buf);
    param->tai_utc_delta_new = val & 0x7FFF;
    if (buf->om_len < TAI_OF_DELTA_CHANGE_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(param->tai_delta_change,
             TAI_OF_ZONE_CHANGE_LEN,
             net_buf_simple_pull_mem(buf, TAI_OF_DELTA_CHANGE_LEN),
             TAI_OF_DELTA_CHANGE_LEN);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int bt_mesh_time_role_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct bt_mesh_time_role_status_t *param;

    if (cli->op_pending != BLE_MESH_MODEL_OP_TIME_ROLE_STATUS) {
        BT_WARN("Unexpected Time Role Status message");
        return -EINVAL;
    }

    param = (struct bt_mesh_time_role_status_t *)cli->op_param;

    param->time_role = net_buf_simple_pull_u8(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

const struct bt_mesh_model_op bt_mesh_time_cli_op[] = {
    {BLE_MESH_MODEL_OP_TIME_STATUS, 5, bt_mesh_client_time_status},
    {BLE_MESH_MODEL_OP_TIME_ZONE_STATUS, 7, bt_mesh_timezone_status},
    {BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS, 9, bt_mesh_tai_utc_delta_status},
    {BLE_MESH_MODEL_OP_TIME_ROLE_STATUS, 1, bt_mesh_time_role_status},
    BT_MESH_MODEL_OP_END,
};

int bt_mesh_time_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_GET);
    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_TIME_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_time_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_time_set_t *set,
    struct bt_mesh_time_status_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    int err;
    uint16_t val;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIME_SET_VAL_LEN);
    struct bt_mesh_time_set_t *value = set;

    bt_mesh_model_msg_init(msg, common->opcode);

    net_buf_simple_add_mem(msg, value->tai_seconds, TAI_SECONDS_LEN);
    net_buf_simple_add_u8(msg, value->subsecond);
    net_buf_simple_add_u8(msg, value->uncertainty);
    val = (value->tai_utc_delta_curr << 1) | value->time_authority;
    net_buf_simple_add_le16(msg, val);
    net_buf_simple_add_u8(msg, value->timezone_offset_curr);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    BT_WARN("time_set");
    if (!status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_TIME_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_timezone_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_ZONE_GET);
    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_TIME_ZONE_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_timezone_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_timezone_set_t *set,
    struct bt_mesh_timezone_status_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    int err;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIMEZONE_SET_VAL_LEN);
    struct bt_mesh_timezone_set_t *value = set;

    bt_mesh_model_msg_init(msg, common->opcode);

    net_buf_simple_add_u8(msg, value->timezone_offset_new);
    net_buf_simple_add_mem(msg, value->tai_zone_change, TAI_OF_ZONE_CHANGE_LEN);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    BT_WARN("timezone_set");
    if (!status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_TIME_ZONE_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_tai_utc_delta_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET);
    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_tai_utc_delta_set(bt_mesh_client_common_param_t *common, struct bt_mesh_tai_utc_delta_set_t *set,
    struct bt_mesh_tai_utc_delta_status_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    int err;
    uint16_t val = 0;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TAI_UTC_DELTA_SET_VAL_LEN);
    struct bt_mesh_tai_utc_delta_set_t *value = set;

    bt_mesh_model_msg_init(msg, common->opcode);

    val = value->tai_utc_delta_new;
    net_buf_simple_add_le16(msg, val);
    net_buf_simple_add_mem(msg, value->tai_delta_change, TAI_OF_DELTA_CHANGE_LEN);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    BT_WARN("tai_utc_delta_set");
    if (!status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_time_role_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_ROLE_GET);
    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, BLE_MESH_MODEL_OP_TIME_ROLE_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_time_role_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_time_role_set_t *set,
    struct bt_mesh_time_role_status_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    int err;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIME_ROLE_SET_VAL_LEN);
    struct bt_mesh_time_role_set_t *value = set;

    bt_mesh_model_msg_init(msg, common->opcode);
    net_buf_simple_add_u8(msg, value->time_role);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    BT_WARN("time_role_set");
    if (!status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_TIME_ROLE_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}
static int time_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}
static int bt_mesh_time_client_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Time Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = time_cli_pub_update;
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (time_cli == NULL) {
        time_cli = cli;
    }

    return 0;
}

static int bt_mesh_time_client_deinit(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Time Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_cli_cb = {
    .init = bt_mesh_time_client_init,
    .deinit = bt_mesh_time_client_deinit,
};

void *bt_mesh_get_time_cli_model(void)
{
    if (time_cli) {
        return (void *)time_cli->model;
    }

    return NULL;
}
