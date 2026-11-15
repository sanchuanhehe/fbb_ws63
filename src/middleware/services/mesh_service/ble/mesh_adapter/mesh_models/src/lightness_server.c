/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light Lightness Server Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model/lightness_server.h"

#define LIGHTNESS_STATUS_LEN 5

struct bt_mesh_light_lightness_srv *light_lightness_srv;

static int light_lightness_status_msg(struct os_mbuf *msg, uint16_t opcode, struct bt_mesh_light_lightness_srv *srv)
{
    bt_mesh_model_msg_init(msg, opcode);

    switch (opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS:
            net_buf_simple_add_le16(msg, srv->light_state->actual);
            BT_INFO("lightness actual %u, target %u", srv->light_state->actual, srv->light_state->target_actual);
            if (srv->actual_transition.counter) {
                net_buf_simple_add_le16(msg, srv->light_state->target_actual);
                calculate_rt(&srv->actual_transition);
                net_buf_simple_add_u8(msg, srv->actual_transition.remain_time);
                BT_INFO("remain_time %u", srv->actual_transition.remain_time);
            }
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS:
            net_buf_simple_add_le16(msg, srv->light_state->linear);
            BT_INFO("lightness linear %u, target %u", srv->light_state->linear, srv->light_state->target_linear);
            if (srv->linear_transition.counter) {
                net_buf_simple_add_le16(msg, srv->light_state->target_linear);
                calculate_rt(&srv->linear_transition);
                net_buf_simple_add_u8(msg, srv->linear_transition.remain_time);
                BT_INFO("remain_time %u", srv->linear_transition.remain_time);
            }
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS:
            net_buf_simple_add_le16(msg, srv->light_state->last);
            BT_INFO("lightness last %u", srv->light_state->last);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS:
            net_buf_simple_add_le16(msg, srv->light_state->def);
            BT_INFO("lightness default %u", srv->light_state->def);
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS:
            net_buf_simple_add_u8(msg, srv->light_state->status_code);
            net_buf_simple_add_le16(msg, srv->light_state->light_range_min);
            net_buf_simple_add_le16(msg, srv->light_state->light_range_max);
            BT_INFO("lightness range [%u,%u], status_code %u",
                srv->light_state->light_range_min,
                srv->light_state->light_range_max,
                srv->light_state->status_code);
            break;
        default:
            BT_ERR("Invaild opcode");
            return -EINVAL;
    }
    return BLE_MESH_SUCC;
}

static void send_light_lightness_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode,
    bool publish)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    struct os_mbuf *msg = NULL;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHTNESS_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    if (light_lightness_status_msg(msg, opcode, srv) != BLE_MESH_SUCC) {
        if (!publish) {
            os_mbuf_free_chain(msg);
        }
        return;
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
        return;
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
    os_mbuf_free_chain(msg);
}

static int light_lightness_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, false);
    return 0;
}

static int light_lightness_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    uint16_t lightness;
    int64_t now;

    lightness = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
            send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, false);
        }
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }

    bt_mesh_server_stop_transition(&srv->actual_transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);

    if (lightness < srv->light_state->light_range_min) {
        lightness = srv->light_state->light_range_min;
    } else if (lightness > srv->light_state->light_range_max) {
        lightness = srv->light_state->light_range_max;
    }

    BT_INFO("tid=%u, lightness_target_actual=%u, trans_time=%u, delay=%u", tid, lightness, trans_time, delay);
    srv->light_state->target_actual = lightness;
    if (srv->light_state->target_actual == srv->light_state->actual) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
            send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, false);
        }
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, true);
        return 0;
    }

    srv->actual_transition.trans_time = trans_time;
    srv->actual_transition.delay = delay;
    lightness_tt_values(srv);

    /* For Instantaneous Transition */
    if (srv->actual_transition.counter == 0) {
        srv->light_state->actual = srv->light_state->target_actual;
    }

    srv->actual_transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, false);
    }
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS, true);
    bt_mesh_server_start_transition(&srv->actual_transition);
    return 0;
}

static int light_lightness_linear_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, false);
    return 0;
}

static int light_lightness_linear_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_light_lightness_srv *srv = model->user_data;
    uint16_t linear;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    int64_t now;

    linear = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
            send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, false);
        }
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }

    bt_mesh_server_stop_transition(&srv->linear_transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);

    BT_INFO("tid=%u, target_linear=%u, trans_time=%u, delay=%u", tid, linear, trans_time, delay);
    srv->light_state->target_linear = linear;
    if (srv->light_state->target_linear == srv->light_state->linear) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
            send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, false);
        }
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, true);
        return 0;
    }

    srv->linear_transition.trans_time = trans_time;
    srv->linear_transition.delay = delay;
    lightness_linear_tt_values(srv);

    if (srv->linear_transition.counter == 0) {
        srv->light_state->linear = srv->light_state->target_linear;
    }

    srv->linear_transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, false);
    }
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS, true);
    bt_mesh_server_start_transition(&srv->linear_transition);
    return 0;
}

static int light_lightness_last_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS, false);
    return 0;
}

static int light_lightness_default_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, false);
    return 0;
}

static int light_lightness_range_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS, false);
    return 0;
}

static int light_lightness_default_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
    srv->light_state->def = net_buf_simple_pull_le16(buf);
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET) {
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, false);
    }
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, true);
    return 0;
}

static int light_lightness_range_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_light_lightness_setup_srv *srv = model->user_data;
    uint16_t light_min;
    uint16_t light_max;

    light_min = net_buf_simple_pull_le16(buf);
    light_max = net_buf_simple_pull_le16(buf);
    if (light_min < 1 || light_min > light_max) {
        BT_ERR("Invaild range");
        return -EINVAL;
    }
    srv->light_state->light_range_min = light_min;
    srv->light_state->light_range_max = light_max;
    BT_INFO("min=%u, max=%u", light_min, light_max);

    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET) {
        send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS, false);
    }
    send_light_lightness_status(model, ctx, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS, true);
    return 0;
}

const struct bt_mesh_model_op light_lightness_srv_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET,                 0, light_lightness_get },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET,                 2, light_lightness_set },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK,            2, light_lightness_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET,            0, light_lightness_linear_get },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET,            2, light_lightness_linear_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK,    2, light_lightness_linear_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET,            0, light_lightness_last_get },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET,            0, light_lightness_default_get },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET,            0, light_lightness_range_get },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op light_lightness_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET,            2, light_lightness_default_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK,    2, light_lightness_default_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET,            4, light_lightness_range_set },
    {BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK,        4, light_lightness_range_set },
    BT_MESH_MODEL_OP_END,
};

static int lightness_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int lightness_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_light_lightness_srv *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (cfg) {
        k_work_cancel_delayable_delete(&(cfg->actual_transition.timer));
        k_work_cancel_delayable_delete(&(cfg->linear_transition.timer));
    }

    return 0;
}

static int lightness_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_light_lightness_srv *cfg = model->user_data;

    if (!cfg || !cfg->light_state) {
        BT_ERR("No Light Lightness Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    if (light_lightness_srv == NULL) {
        light_lightness_srv = cfg;
    }

    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = lightness_pub_update;
    }

    k_work_init_delayable(&(cfg->actual_transition.timer), lightness_actual_work_handler);
    k_work_add_arg_delayable(&(cfg->actual_transition.timer), cfg);

    k_work_init_delayable(&(cfg->linear_transition.timer), lightness_linear_work_handler);
    k_work_add_arg_delayable(&(cfg->linear_transition.timer), cfg);

    return 0;
}

const struct bt_mesh_model_cb light_lightness_srv_cb = {
    .init = lightness_srv_init,
    .deinit = lightness_srv_deinit,
};

static int lightness_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_light_lightness_setup_srv *cfg = model->user_data;

    if (!cfg || !cfg->light_state) {
        BT_ERR("No Light Lightness Server Setup context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}
static int lightness_setup_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}
const struct bt_mesh_model_cb light_lightness_setup_srv_cb = {
    .init = lightness_setup_srv_init,
    .deinit = lightness_setup_srv_deinit,
};
