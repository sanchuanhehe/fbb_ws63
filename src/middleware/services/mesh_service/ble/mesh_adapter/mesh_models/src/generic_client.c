 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Generic Client Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "ble_mesh_model_op.h"
#include "client_common.h"
#include "device_property.h"
#include "model/generic_client.h"

#define ONOFF_VAL_LEN 4
#define LEVEL_VAL_LEN 7
#define DTT_VAL_LEN 1
#define POWER_ONOFF_VAL_LEN 1
#define POWER_LEVEL_VAL_LEN 5
#define PROPERTY_ID_LEN 2
#define MANU_PROPERTY_VAL_LEN 3

#define OVEN_NUMBER_CHECK_DIVISOR 2

struct bt_mesh_gen_model_cli *gen_onoff_cli;
struct bt_mesh_gen_model_cli *gen_level_cli;
struct bt_mesh_gen_model_cli *gen_dtt_cli;
struct bt_mesh_gen_model_cli *gen_power_onoff_cli;
struct bt_mesh_gen_model_cli *gen_battery_cli;
struct bt_mesh_gen_model_cli *gen_power_level_cli;
struct bt_mesh_gen_model_cli *gen_prop_cli;
struct bt_mesh_gen_model_cli *gen_location_cli;

static int check_cli_param(struct bt_mesh_gen_model_cli *cli, uint32_t recv_op)
{
    if (!cli->op_param) {
        BT_ERR("Invaild cli wait param");
        return BLE_MESH_FAIL;
    }

    if (cli->op_pending != recv_op) {
        BT_WARN("Unexpected Status message, opcode %d", recv_op);
        return BLE_MESH_FAIL;
    }
    return BLE_MESH_SUCC;
}

static int gen_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_onoff_status *param = (struct gen_onoff_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));
    BT_WARN("gen_onoff_status");
    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS) {
        BT_WARN("Unexpected Generic OnOff Status message");
        return -EINVAL;
    }

    param->onoff = net_buf_simple_pull_u8(buf);
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            param->op_en = 0;
            break;
        case 0x02: /* Optional fields are available */
            param->target_onoff = net_buf_simple_pull_u8(buf);
            param->remain_time = net_buf_simple_pull_u8(buf);
            param->op_en = 1;
            break;
        default:
            return -EINVAL;
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_level_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_level_status *param = (struct gen_level_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS) {
        BT_WARN("Unexpected Generic LEVEL Status message");
        return -EINVAL;
    }

    param->level = net_buf_simple_pull_le16(buf);
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            param->op_en = 0;
            break;
        case 0x03: /* Optional fields are available */
            param->target_level = net_buf_simple_pull_le16(buf);
            param->remain_time = net_buf_simple_pull_u8(buf);
            param->op_en = 1;
            break;
        default:
            return -EINVAL;
    }

    BT_DBG("level: %d", param->level);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_dtt_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint8_t *param = (uint8_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS) {
        BT_WARN("Unexpected Generic LEVEL Status message");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_u8(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_power_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint8_t *param = (uint8_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS) {
        BT_WARN("Unexpected Generic Power Onoff Status message");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_u8(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_battery_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_battery_status *param = (struct gen_battery_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS) {
        BT_WARN("Unexpected Generic Battery Status message");
        return -EINVAL;
    }

    param->battery_level = net_buf_simple_pull_u8(buf);
    param->time_to_discharge = net_buf_simple_pull_le24(buf);
    param->time_to_charge = net_buf_simple_pull_le24(buf);
    param->flags = net_buf_simple_pull_u8(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_location_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_location_status *param = (struct gen_location_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS) {
        BT_WARN("Unexpected Generic Location Status message");
        return -EINVAL;
    }

    param->global_latitude = net_buf_simple_pull_le24(buf);
    param->global_longitude = net_buf_simple_pull_le24(buf);
    param->global_altitude = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_power_level_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_power_level_status *param = (struct gen_power_level_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS) {
        BT_WARN("Unexpected Generic Power Level Status message");
        return -EINVAL;
    }

    param->power_level = net_buf_simple_pull_le16(buf);
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            param->op_en = 0;
            break;
        case 0x03: /* Optional fields are available */
            param->target_power_level = net_buf_simple_pull_le16(buf);
            param->remain_time = net_buf_simple_pull_u8(buf);
            param->op_en = 1;
            break;
        default:
            return -EINVAL;
    }

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_power_last_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint16_t *param = (uint16_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS) {
        BT_WARN("Unexpected Last Generic Power Status message");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_power_default_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    uint16_t *param = (uint16_t *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS) {
        BT_WARN("Unexpected Generic Default Power Level message");
        return -EINVAL;
    }

    *param = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_power_range_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    struct gen_power_level_range_status *param = (struct gen_power_level_range_status *)cli->op_param;

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -EINVAL;
    }

    if (cli->op_pending != BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS) {
        BT_WARN("Unexpected Generic Range Status message");
        return -EINVAL;
    }

    param->status_code = net_buf_simple_pull_u8(buf);
    param->min_power_level = net_buf_simple_pull_le16(buf);
    param->max_power_level = net_buf_simple_pull_le16(buf);

    k_sem_give(&cli->op_sync);
    return 0;
}

static int gen_usr_prop_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;

    int ret = check_cli_param(cli, ctx->recv_op);
    if (ret == BLE_MESH_FAIL) {
        return ret;
    }

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS) {
        struct gen_user_property_param *param = (struct gen_user_property_param *)cli->op_param;
        struct gen_user_property_status status = { 0 };
        status.user_prop_id = net_buf_simple_pull_le16(buf);
        status.user_access = net_buf_simple_pull_u8(buf);
        int len = bt_mesh_get_dev_prop_len(status.user_prop_id);
        if (len != buf->om_len) {
            BT_ERR("Invaild value len for user_prop_id, id=%u, err len=%u", status.user_prop_id, buf->om_len);
            return -EINVAL;
        }
        status.user_prop_val = NET_BUF_SIMPLE(len);
        if (!status.user_prop_val) {
            BT_ERR("user_property ids alloc memery err");
            return -EINVAL;
        }

        net_buf_simple_add_mem(status.user_prop_val, buf->om_data, len);
        if (param->status) {
            param->status(cli->model, &status);
        }
        os_mbuf_free_chain(status.user_prop_val);
        k_sem_give(&cli->op_sync);
    }
    return 0;
}

static int gen_usr_props_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;

    int ret = check_cli_param(cli, ctx->recv_op);
    if (ret == BLE_MESH_FAIL) {
        return ret;
    }

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS) {
        struct gen_user_properties_param *param = (struct gen_user_properties_param *)cli->op_param;
        if (buf->om_len % OVEN_NUMBER_CHECK_DIVISOR != 0) {
            BT_ERR("Invaild value len for user_prop_ids, err len=%u", buf->om_len);
        }
        struct gen_user_properties_status status = { 0 };
        status.user_prop_ids = NET_BUF_SIMPLE(buf->om_len);
        if (!status.user_prop_ids) {
            BT_ERR("user_properties ids alloc memery err");
            return -EINVAL;
        }

        net_buf_simple_add_mem(status.user_prop_ids, buf->om_data, buf->om_len);
        if (param->status) {
            param->status(cli->model, &status);
        }

        os_mbuf_free_chain(status.user_prop_ids);
        k_sem_give(&cli->op_sync);
    }
    return 0;
}


static int gen_manu_prop_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;

    int ret = check_cli_param(cli, ctx->recv_op);
    if (ret == BLE_MESH_FAIL) {
        return ret;
    }

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS) {
        struct gen_manu_property_param *param = (struct gen_manu_property_param *)cli->op_param;
        struct gen_manu_property_status status;
        status.manu_prop_id = net_buf_simple_pull_le16(buf);
        status.manu_access = net_buf_simple_pull_u8(buf);
        int len = bt_mesh_get_dev_prop_len(status.manu_prop_id);
        if (len != buf->om_len) {
            BT_ERR("Invaild value len for manu_prop_id, id=%u, err len=%u", status.manu_prop_id, buf->om_len);
            return -EINVAL;
        }
        status.manu_prop_val = NET_BUF_SIMPLE(len);
        if (!status.manu_prop_val) {
            BT_ERR("manu_property ids alloc memery err");
            return -EINVAL;
        }

        net_buf_simple_add_mem(status.manu_prop_val, buf->om_data, len);
        if (param->status) {
            param->status(cli->model, &status);
        }

        os_mbuf_free_chain(status.manu_prop_val);
        k_sem_give(&cli->op_sync);
    }
    return 0;
}

static int gen_manu_props_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_gen_model_cli *cli = model->user_data;

    int ret = check_cli_param(cli, ctx->recv_op);
    if (ret == BLE_MESH_FAIL) {
        return ret;
    }

    BT_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s", ctx->net_idx, ctx->app_idx, ctx->addr, buf->om_len,
        bt_hex(buf->om_data, buf->om_len));

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS) {
        struct gen_manu_properties_param *param = (struct gen_manu_properties_param *)cli->op_param;
        if (buf->om_len % OVEN_NUMBER_CHECK_DIVISOR != 0) {
            BT_ERR("Invaild value len for manu_prop_ids, err len=%u", buf->om_len);
        }
        struct gen_manu_properties_status status;
        status.manu_prop_ids = NET_BUF_SIMPLE(buf->om_len);
        if (!status.manu_prop_ids) {
            BT_ERR("manu_properties ids alloc memery err");
            return -EINVAL;
        }

        net_buf_simple_add_mem(status.manu_prop_ids, buf->om_data, buf->om_len);
        if (param->status) {
            param->status(cli->model, &status);
        }

        os_mbuf_free_chain(status.manu_prop_ids);
        k_sem_give(&cli->op_sync);
    }
    return 0;
}

const struct bt_mesh_model_op gen_onoff_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, 1, gen_onoff_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_level_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS, 2, gen_level_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_dtt_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS, 1, gen_dtt_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_onoff_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS, 1, gen_power_onoff_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_battery_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS, 8, gen_battery_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_location_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS, 10, gen_location_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_level_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS,     2, gen_power_level_status },
    { BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS,         2, gen_power_last_status },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS,     2, gen_power_default_status },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS,     5, gen_power_range_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_prop_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS,     4, gen_usr_prop_status },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS, 2, gen_usr_props_status },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS,     4, gen_manu_prop_status },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS, 2, gen_manu_props_status },
    BT_MESH_MODEL_OP_END,
};

static int onoff_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int onoff_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic OnOff Client context provided");
        return -EINVAL;
    }
    cli->model = model;

    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = onoff_pub_update;
    }
    if (model->pub->msg == NULL) {
        model->pub->msg = NET_BUF_SIMPLE(MODEL_ONOFF_PUB_MSG_LEN);
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_onoff_cli == NULL) {
        gen_onoff_cli = cli;
    }

    return 0;
}

static int onoff_cli_deinit(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic OnOff Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int level_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int level_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Level Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = level_cli_pub_update;
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_level_cli == NULL) {
        gen_level_cli = cli;
    }

    return 0;
}

static int level_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Level Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int dtt_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int dtt_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Dtt Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = dtt_cli_pub_update;
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_dtt_cli == NULL) {
        gen_dtt_cli = cli;
    }

    return 0;
}

static int dtt_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Dtt Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int power_onoff_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Power Onoff Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_power_onoff_cli == NULL) {
        gen_power_onoff_cli = cli;
    }

    return 0;
}

static int power_onoff_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Power Onoff Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}


static int battery_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Battery Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_battery_cli == NULL) {
        gen_battery_cli = cli;
    }

    return 0;
}

static int battery_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Battery Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int power_level_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Power Level Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_power_level_cli == NULL) {
        gen_power_level_cli = cli;
    }

    return 0;
}

static int power_level_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Power Level Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int property_cli_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Property Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_prop_cli == NULL) {
        gen_prop_cli = cli;
    }

    return 0;
}

static int property_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Property Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int location_cli_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("No Generic Location Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    k_sem_init(&cli->op_sync, 0, 1);

    if (gen_location_cli == NULL) {
        gen_location_cli = cli;
    }

    return 0;
}

static int location_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No Generic Location Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_gen_onoff_cli_cb = {
    .init = onoff_cli_init,
    .deinit = onoff_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_level_cli_cb = {
    .init = level_cli_init,
    .deinit = level_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_dtt_cli_cb = {
    .init = dtt_cli_init,
    .deinit = dtt_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_power_onoff_cli_cb = {
    .init = power_onoff_cli_init,
    .deinit = power_onoff_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_battery_cli_cb = {
    .init = battery_cli_init,
    .deinit = battery_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_power_level_cli_cb = {
    .init = power_level_cli_init,
    .deinit = power_level_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_prop_cli_cb = {
    .init = property_cli_init,
    .deinit = property_cli_deinit,
};

const struct bt_mesh_model_cb bt_mesh_gen_location_cli_cb = {
    .init = location_cli_init,
    .deinit = location_cli_deinit,
};

int bt_mesh_gen_onoff_get(bt_mesh_client_common_param_t *common, struct gen_onoff_status *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONOFF_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_onoff_set(bt_mesh_client_common_param_t *common, struct gen_onoff_set *set,
    struct gen_onoff_status *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + ONOFF_VAL_LEN);
    struct gen_onoff_set *value = set;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK: {
            bt_mesh_model_msg_init(msg, common->opcode);
            break;
        }
        default:
            err = -EINVAL;
            BT_ERR("Invaild opcode");
            goto done;
    }
    net_buf_simple_add_u8(msg, value->onoff);
    net_buf_simple_add_u8(msg, value->tid);
    if (value->op_en) {
        net_buf_simple_add_u8(msg, value->trans_time);
        net_buf_simple_add_u8(msg, value->delay);
    }

    BT_INFO("dst 0x%04x seq 0x%06x", common->ctx.addr, bt_mesh_get_seq());
    
    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    BT_WARN("gen_onoff_set");
    if (!need_ack || !status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_level_get(bt_mesh_client_common_param_t *common, struct gen_level_status *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LEVEL_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_gen_level_set_msg(
    struct os_mbuf *msg, bt_mesh_client_common_param_t *common, struct gen_level_set *value, bool *need_ack)
{
    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
            *need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:
            net_buf_simple_add_le16(msg, value->level);
            break;
        case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
            *need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK:
            net_buf_simple_add_le32(msg, value->delta_level);
            break;
        case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
            *need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK:
            net_buf_simple_add_le16(msg, value->move_level);
            break;
        default:
            BT_ERR("Invalid Set opcode 0x%04x", common->opcode);
            return -EINVAL;
    }

    net_buf_simple_add_u8(msg, value->tid);
    if (value->op_en) {
        net_buf_simple_add_u8(msg, value->trans_time);
        net_buf_simple_add_u8(msg, value->delay);
    }
    return BLE_MESH_SUCC;
}

int bt_mesh_gen_level_set(bt_mesh_client_common_param_t *common, struct gen_level_set *set,
    struct gen_level_status *status)
{
    int err;
    bool need_ack = false;

    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct gen_level_set *value = set;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LEVEL_VAL_LEN);
    if (!msg) {
        BT_ERR("allock mem failed");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, common->opcode);
    err = bt_mesh_gen_level_set_msg(msg, common, value, &need_ack);
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

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_dtt_get(bt_mesh_client_common_param_t *common, uint8_t *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_dtt_set(bt_mesh_client_common_param_t *common, uint8_t *set, uint8_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + DTT_VAL_LEN);

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
            need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK: {
            bt_mesh_model_msg_init(msg, common->opcode);
            break;
        }
        default:
            err = -EINVAL;
            goto done;
    }

    net_buf_simple_add_u8(msg, *(uint8_t *)set);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    if (!need_ack || !status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_power_onoff_get(bt_mesh_client_common_param_t *common, uint8_t *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_power_onoff_set(bt_mesh_client_common_param_t *common, uint8_t *set, uint8_t *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + POWER_ONOFF_VAL_LEN);

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
            need_ack = true;
        case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK: {
            bt_mesh_model_msg_init(msg, common->opcode);
            net_buf_simple_add_u8(msg, *(uint8_t *)set);
            break;
        }
        default:
            err = -EINVAL;
            goto done;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    if (!need_ack || !status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_battery_get(bt_mesh_client_common_param_t *common, struct gen_battery_status *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_BATTERY_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_location_get(bt_mesh_client_common_param_t *common, struct gen_location_status *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_power_level_get(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    int err;
    int32_t wait_op;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS;
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS;
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS;
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS;
            break;
        default:
            err = -EINVAL;
            goto done;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send err %d", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, status, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_gen_power_level_msg(struct os_mbuf *msg, bt_mesh_client_common_param_t *common,
    struct gen_power_level_set *value, bool *need_ack, uint32_t *wait_op)
{
    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS;
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK:
            net_buf_simple_add_le16(msg, value->power_level);
            net_buf_simple_add_u8(msg, value->tid);
            if (value->op_en) {
                net_buf_simple_add_u8(msg, value->trans_time);
                net_buf_simple_add_u8(msg, value->delay);
            }
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS;
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK:
            net_buf_simple_add_le16(msg, value->power_def_level);
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
            *need_ack = true;
            *wait_op = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS;
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK:
            net_buf_simple_add_le16(msg, value->power_min_level);
            net_buf_simple_add_le16(msg, value->power_max_level);
            break;
        default:
            BT_ERR("err opcode");
            return -EINVAL;
    }
    return BLE_MESH_SUCC;
}

int bt_mesh_gen_power_level_set(bt_mesh_client_common_param_t *common, struct gen_power_level_set *set, void *status)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    int err;
    bool need_ack = false;
    uint32_t wait_op;
    struct gen_power_level_set *value = set;
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + POWER_LEVEL_VAL_LEN);
    if (!msg) {
        BT_ERR("allock mem failed");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, common->opcode);

    err = bt_mesh_gen_power_level_msg(msg, common, value, &need_ack, &wait_op);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("level_msg failed (err %d)", err);
        goto done;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    if (need_ack && (status != NULL)) {
        MODEL_CLI_WAIT(err, common->model->user_data, status, wait_op);
    }

done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_properties_get(bt_mesh_client_common_param_t *common, void *param)
{
    if (!common || !common->model || !common->model->user_data || !param) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    int err;
    int32_t wait_op;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS;
            break;
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS;
            break;
        default:
            err = -EINVAL;
            goto done;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("properties model send failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, param, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_gen_property_get(bt_mesh_client_common_param_t *common, uint16_t prop_id, void *param)
{
    if (!common || !common->model || !common->model->user_data || !param) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + PROPERTY_ID_LEN);
    int err;
    int32_t wait_op;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS;
            break;
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET:
            wait_op = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS;
            break;
        default:
            err = -EINVAL;
            goto done;
    }

    bt_mesh_model_msg_init(msg, common->opcode);
    net_buf_simple_add_le16(msg, prop_id);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("property model send failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, param, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}

static struct os_mbuf *bt_mesh_gen_prop_msg_usr(
    bt_mesh_client_common_param_t *common, struct gen_user_property_set *usr_value)
{
    int len = bt_mesh_get_dev_prop_len(usr_value->user_property_id);
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + PROPERTY_ID_LEN + len);
    if (!msg) {
        BT_ERR("alloc mem failed");
        return msg;
    }
    bt_mesh_model_msg_init(msg, common->opcode);
    net_buf_simple_add_le16(msg, usr_value->user_property_id);
    net_buf_simple_add_mem(msg, usr_value->user_property_value->om_data, usr_value->user_property_value->om_len);
    return msg;
}

static struct os_mbuf *bt_mesh_gen_prop_msg_manu(
    bt_mesh_client_common_param_t *common, struct gen_manu_property_set *manu_value)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + MANU_PROPERTY_VAL_LEN);
    if (!msg) {
        BT_ERR("alloc mem failed");
        return msg;
    }
    bt_mesh_model_msg_init(msg, common->opcode);
    net_buf_simple_add_le16(msg, manu_value->manu_property_id);
    net_buf_simple_add_u8(msg, manu_value->manu_user_access);
    return msg;
}

int bt_mesh_gen_property_set(bt_mesh_client_common_param_t *common, void *set, void *param)
{
    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    int err = BLE_MESH_SUCC;
    bool need_ack = false;
    uint32_t wait_op;
    struct os_mbuf *msg = NULL;

    switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET:
            need_ack = true;
            wait_op = BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS;
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK: {
            struct gen_user_property_set *usr_value = (struct gen_user_property_set *)set;
            msg = bt_mesh_gen_prop_msg_usr(common, usr_value);
            break;
        }
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET:
            need_ack = true;
            wait_op = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS;
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK: {
            struct gen_manu_property_set *manu_value = (struct gen_manu_property_set *)set;
            msg = bt_mesh_gen_prop_msg_manu(common, manu_value);
            break;
        }
        default:
            BT_ERR("Invaild opcode");
            return -EINVAL;
    }

    if (!msg) {
        return -EINVAL;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }
    if (!need_ack || !param) {
        goto done;
    }
    MODEL_CLI_WAIT(err, common->model->user_data, param, wait_op);
done:
    os_mbuf_free_chain(msg);
    return err;
}

struct bt_mesh_model *bt_mesh_get_gen_onoff_cli_model(void)
{
    if (gen_onoff_cli) {
        return gen_onoff_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_level_cli_model(void)
{
    if (gen_level_cli) {
        return gen_level_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_dtt_cli_model(void)
{
    if (gen_dtt_cli) {
        return gen_dtt_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_power_onoff_cli_model(void)
{
    if (gen_power_onoff_cli) {
        return gen_power_onoff_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_power_level_cli_model(void)
{
    if (gen_power_level_cli) {
        return gen_power_level_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_battery_cli_model(void)
{
    if (gen_battery_cli) {
        return gen_battery_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_location_cli_model(void)
{
    if (gen_location_cli) {
        return gen_location_cli->model;
    }

    return NULL;
}

struct bt_mesh_model *bt_mesh_get_gen_prop_cli_model(void)
{
    if (gen_prop_cli) {
        return gen_prop_cli->model;
    }

    return NULL;
}