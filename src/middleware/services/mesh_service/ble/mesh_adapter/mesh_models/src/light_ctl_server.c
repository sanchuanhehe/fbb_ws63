/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light CTL Server Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include <stdio.h>
#include "syscfg/syscfg.h"

#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model_common.h"
#include "access.h"
#include "model/light_ctl_server.h"

#define LIGHT_CTL_STATUS_LEN 9
#define LIGHT_CTL_TEMP_RANGE_STATUS_LEN 5
#define LIGHT_CTL_DEF_STATUS_LEN 6
#define LIGHT_CTL_TEMP_STATUS_LEN 9
#define LIGHT_CTL_IS_LAST_MSG_TIME K_SECONDS(6)
#define LIGHT_CTL_ELEM_COUNT_MIN 2

ble_mesh_light_ctl_srv_t *light_ctl_svr_usr_data;
ble_mesh_light_ctl_setup_srv_t *light_ctl_setup_svr_usr_data;
ble_mesh_light_ctl_temp_srv_t *light_ctl_temp_svr_usr_data;

/* Light CTL Server message handlers */
static int bt_mesh_op_light_ctl_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHT_CTL_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    ble_mesh_light_ctl_srv_t *srv = model->user_data;

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
    net_buf_simple_add_le16(msg, srv->state->lightness);
    net_buf_simple_add_le16(msg, srv->state->temp);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_le16(msg, srv->state->target_lightness);
        net_buf_simple_add_le16(msg, srv->state->target_temp);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send LightCTL Status response\n");
    }

    os_mbuf_free_chain(msg);
    return err;
}

void light_ctl_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    ble_mesh_light_ctl_srv_t *srv = model->user_data;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);

    /* Here, as per Model specification, status should be
     * made up of lightness & temperature values only
     */
    net_buf_simple_add_le16(msg, srv->state->lightness);
    net_buf_simple_add_le16(msg, srv->state->temp);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_le16(msg, srv->state->target_lightness);
        net_buf_simple_add_le16(msg, srv->state->target_temp);
        net_buf_simple_add_u8(msg, srv->transition.trans_time);
    }

    err = bt_mesh_model_publish(model);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("bt_mesh_model_publish err %d\n", err);
    }
}

void constrain_lightness(uint16_t *lightness)
{
    uint16_t var = *lightness;

    if (!light_lightness_srv || !light_lightness_srv->light_state) {
        return;
    }

    if (var > 0 && var < light_lightness_srv->light_state->light_range_min) {
        var = light_lightness_srv->light_state->light_range_min;
    } else if (var > light_lightness_srv->light_state->light_range_max) {
        var = light_lightness_srv->light_state->light_range_max;
    }

    *lightness = var;
}

static bool light_ctl_is_server_recv_last_msg(
    ble_mesh_light_ctl_srv_t *srv, struct bt_mesh_msg_ctx *ctx, uint8_t tid, int64_t now)
{
    if (srv->last.tid == tid && srv->last.src == ctx->addr && srv->last.dst == ctx->recv_dst &&
        (now - srv->last.timestamp <= LIGHT_CTL_IS_LAST_MSG_TIME)) {
        return true;
    }

    return false;
}

static void light_ctl_update_last_msg(
    ble_mesh_light_ctl_srv_t *srv, struct bt_mesh_msg_ctx *ctx, uint8_t tid, int64_t now)
{
    srv->last.tid = tid;
    srv->last.src = ctx->addr;
    srv->last.dst = ctx->recv_dst;
    srv->last.timestamp = now;
}

static void light_ctl_update_target_state(
    ble_mesh_light_ctl_state *state, uint16_t lightness, uint16_t temp, int16_t delta_uv)
{
    state->target_lightness = lightness;
    state->target_temp = temp;
    state->target_delta_uv = delta_uv;
}

static void light_ctl_update_state(ble_mesh_light_ctl_state *state, uint16_t lightness, uint16_t temp, int16_t delta_uv)
{
    state->lightness = lightness;
    state->temp = temp;
    state->delta_uv = delta_uv;
}

static uint16_t get_temp_value(uint16_t temp_value, ble_mesh_light_ctl_state *state)
{
    uint16_t temp = temp_value;
    if (temp < state->temp_range_min) {
        temp = state->temp_range_min;
    } else if (temp > state->temp_range_max) {
        temp = state->temp_range_max;
    }
    return temp;
}

static int bt_mesh_op_light_ctl_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t tid, tt, delay;
    ble_mesh_light_ctl_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    uint16_t lightness = net_buf_simple_pull_le16(buf);
    uint16_t temp = net_buf_simple_pull_le16(buf);
    int16_t delta_uv = (int16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        BT_WARN("temp invalid %u", temp);
        return -EINVAL;
    }

    int64_t now = k_uptime_get();
    if (light_ctl_is_server_recv_last_msg(srv, ctx, tid, now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
            bt_mesh_op_light_ctl_get(model, ctx, buf);
        }
        return 0;
    }
    int ret = set_option_field(buf, &tt, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }
    bt_mesh_server_stop_transition(&srv->transition);

    light_ctl_update_last_msg(srv, ctx, tid, now);

    temp = get_temp_value(temp, state);

    constrain_lightness(&lightness);

    light_ctl_update_target_state(state, lightness, temp, delta_uv);

    if (state->target_lightness != state->lightness || state->target_temp != state->temp ||
        state->target_delta_uv != state->delta_uv) {
        light_ctl_tt_values(srv, tt, delay);
    } else {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
            bt_mesh_op_light_ctl_get(model, ctx, buf);
        }

        light_ctl_publish(model);
        return 0;
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0) {
        light_ctl_update_state(state, state->target_lightness, state->target_temp, state->target_delta_uv);
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
        bt_mesh_op_light_ctl_get(model, ctx, buf);
    }
    light_ctl_publish(model);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

static int bt_mesh_op_light_ctl_temp_range_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *msg =
        NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHT_CTL_TEMP_RANGE_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    ble_mesh_light_ctl_srv_t *srv = model->user_data;
    srv->state->status_code = RANGE_SUCCESSFULLY_UPDATED;

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS);
    net_buf_simple_add_u8(msg, srv->state->status_code);
    net_buf_simple_add_le16(msg, srv->state->temp_range_min);
    net_buf_simple_add_le16(msg, srv->state->temp_range_max);

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send LightCTL Temp Range Status response\n");
    }

    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_op_light_ctl_default_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHT_CTL_DEF_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, srv->state->lightness_def);
    net_buf_simple_add_le16(msg, srv->state->temp_def);
    net_buf_simple_add_le16(msg, srv->state->delta_uv_def);

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send LightCTL Default Status response\n");
    }

    os_mbuf_free_chain(msg);
    return err;
}

static void light_ctl_default_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS);
    net_buf_simple_add_le16(msg, state->lightness_def);
    net_buf_simple_add_le16(msg, state->temp_def);
    net_buf_simple_add_le16(msg, state->delta_uv_def);

    err = bt_mesh_model_publish(model);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("bt_mesh_model_publish err %d\n", err);
    }
}

static bool light_ctl_default_setunack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint16_t lightness, temp;
    int16_t delta_uv;
    ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    lightness = net_buf_simple_pull_le16(buf);
    temp = net_buf_simple_pull_le16(buf);
    delta_uv = (int16_t)net_buf_simple_pull_le16(buf);

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return false;
    }

    if (temp < state->temp_range_min) {
        temp = state->temp_range_min;
    } else if (temp > state->temp_range_max) {
        temp = state->temp_range_max;
    }

    if (state->lightness_def != lightness || state->temp_def != temp || state->delta_uv_def != delta_uv) {
        state->lightness_def = lightness;
        state->temp_def = temp;
        state->delta_uv_def = delta_uv;
    }
    return true;
}

static int bt_mesh_op_light_ctl_default_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (light_ctl_default_setunack(model, ctx, buf) == true) {
        bt_mesh_op_light_ctl_default_get(model, ctx, buf);
        light_ctl_default_publish(model);
    }
    return 0;
}

static int bt_mesh_op_light_ctl_default_set_unack(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (light_ctl_default_setunack(model, ctx, buf) == true) {
        light_ctl_default_publish(model);
    }
    return 0;
}

static bool light_ctl_temp_range_setunack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint16_t min, max;
    ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    min = net_buf_simple_pull_le16(buf);
    max = net_buf_simple_pull_le16(buf);
    /* Here, Model specification is silent about tid implementation */
    /* This is as per 6.1.3.1 in Mesh Model Specification */
    if (min < TEMP_MIN || min > TEMP_MAX || max < TEMP_MIN || max > TEMP_MAX) {
        return false;
    }

    if (min <= max) {
        state->status_code = RANGE_SUCCESSFULLY_UPDATED;

        if (state->temp_range_min != min || state->temp_range_max != max) {
            state->temp_range_min = min;
            state->temp_range_max = max;
        }
    } else {
        /* The provided value for Range Max cannot be set */
        state->status_code = CANNOT_SET_RANGE_MAX;
        return false;
    }

    if (state->temp < state->temp_range_min) {
        state->temp = state->temp_range_min;
    } else if (state->temp > state->temp_range_max) {
        state->temp = state->temp_range_max;
    }

    state->target_temp = state->temp;
    if (light_ctl_temp_svr_usr_data) {
        bt_mesh_server_stop_transition(&light_ctl_temp_svr_usr_data->transition);
    }

    return true;
}

static void light_ctl_temp_range_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS);
    net_buf_simple_add_u8(msg, state->status_code);
    net_buf_simple_add_le16(msg, state->temp_range_min);
    net_buf_simple_add_le16(msg, state->temp_range_max);

    err = bt_mesh_model_publish(model);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("bt_mesh_model_publish err %d\n", err);
    }
}

static int bt_mesh_op_light_ctl_temp_range_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (light_ctl_temp_range_setunack(model, ctx, buf) == true) {
        bt_mesh_op_light_ctl_temp_range_get(model, ctx, buf);
        light_ctl_temp_range_publish(model);
    }
    return 0;
}

static int bt_mesh_op_light_ctl_temp_range_set_unack(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (light_ctl_temp_range_setunack(model, ctx, buf) == true) {
        light_ctl_temp_range_publish(model);
    }
    return 0;
}

/* Light CTL Temp. Server message handlers */
static int bt_mesh_op_light_ctl_temp_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LIGHT_CTL_TEMP_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    ble_mesh_light_ctl_temp_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
    net_buf_simple_add_le16(msg, state->temp);
    net_buf_simple_add_le16(msg, state->delta_uv);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_le16(msg, state->target_temp);
        net_buf_simple_add_le16(msg, state->target_delta_uv);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send LightCTL Temp. Status response\n");
    }

    os_mbuf_free_chain(msg);
    return err;
}

void light_ctl_temp_publish(struct bt_mesh_model *model)
{
    int err;
    struct os_mbuf *msg = model->pub->msg;
    ble_mesh_light_ctl_temp_srv_t *srv = model->user_data;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
    net_buf_simple_add_le16(msg, srv->state->temp);
    net_buf_simple_add_le16(msg, srv->state->delta_uv);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_le16(msg, srv->state->target_temp);
        net_buf_simple_add_le16(msg, srv->state->target_delta_uv);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    err = bt_mesh_model_publish(model);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("bt_mesh_model_publish err %d\n", err);
    }
}

static void light_ctl_temp_set_last(
    ble_mesh_light_ctl_temp_srv_t *srv, struct bt_mesh_msg_ctx *ctx, uint8_t tid, int64_t now)
{
    srv->last.tid = tid;
    srv->last.src = ctx->addr;
    srv->last.dst = ctx->recv_dst;
    srv->last.timestamp = now;
}

static void light_ctl_temp_set_target(ble_mesh_light_ctl_state *state, uint16_t temp, int16_t delta_uv)
{
    state->target_temp = temp;
    state->target_delta_uv = delta_uv;
}

static int bt_mesh_op_light_ctl_temp_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t tid, tt, delay;
    int16_t delta_uv;
    uint16_t temp;
    ble_mesh_light_ctl_temp_srv_t *srv = model->user_data;
    ble_mesh_light_ctl_state *state = srv->state;

    temp = net_buf_simple_pull_le16(buf);
    delta_uv = (int16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    if (temp < TEMP_MIN || temp > TEMP_MAX) {
        return -EINVAL;
    }

    int64_t now = k_uptime_get();
    if (srv->last.tid == tid && srv->last.src == ctx->addr && srv->last.dst == ctx->recv_dst &&
        (now - srv->last.timestamp <= LIGHT_CTL_IS_LAST_MSG_TIME)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
            bt_mesh_op_light_ctl_temp_get(model, ctx, buf);
        }
        return 0;
    }
    int ret = set_option_field(buf, &tt, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }
    bt_mesh_server_stop_transition(&srv->transition);

    light_ctl_temp_set_last(srv, ctx, tid, now);

    temp = get_temp_value(temp, state);

    light_ctl_temp_set_target(state, temp, delta_uv);

    if (state->target_temp != state->temp || state->target_delta_uv != state->delta_uv) {
        light_ctl_temp_tt_values(srv, tt, delay);
    } else {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
            bt_mesh_op_light_ctl_temp_get(model, ctx, buf);
        }
        light_ctl_temp_publish(model);
        return 0;
    }

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0) {
        state->temp = state->target_temp;
        state->delta_uv = state->target_delta_uv;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET) {
        bt_mesh_op_light_ctl_temp_get(model, ctx, buf);
    }
    light_ctl_temp_publish(model);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

/* Mapping of message handlers for Light CTL Server (0x1303) */
const struct bt_mesh_model_op bt_mesh_light_ctl_srv_op[] = {
    {BLE_MESH_MODEL_OP_LIGHT_CTL_GET,                   0, bt_mesh_op_light_ctl_get},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_SET,                   7, bt_mesh_op_light_ctl_set},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK,             7, bt_mesh_op_light_ctl_set},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET, 0, bt_mesh_op_light_ctl_temp_range_get},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET,           0, bt_mesh_op_light_ctl_default_get},
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Setup Server (0x1304) */
const struct bt_mesh_model_op bt_mesh_light_ctl_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET,                   6, bt_mesh_op_light_ctl_default_set},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK,             6, bt_mesh_op_light_ctl_default_set_unack},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET,         4, bt_mesh_op_light_ctl_temp_range_set},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK,   4, bt_mesh_op_light_ctl_temp_range_set_unack},
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Light CTL Temperature Server (0x1306) */
const struct bt_mesh_model_op bt_mesh_light_ctl_temp_srv_op[] = {
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET,       0, bt_mesh_op_light_ctl_temp_get},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET,       5, bt_mesh_op_light_ctl_temp_set},
    {BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK, 5, bt_mesh_op_light_ctl_temp_set},
    BT_MESH_MODEL_OP_END,
};
static int bt_mesh_light_ctl_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}
static int bt_mesh_light_ctl_temp_init(struct bt_mesh_model *model)
{
    ble_mesh_light_ctl_temp_srv_t *srv = model->user_data;
    if (srv->state == NULL) {
        BT_ERR("Invalid Light CTL State");
        return -EINVAL;
    }

    k_work_init_delayable(&srv->transition.timer, light_ctl_temp_work_handler);
    k_work_add_arg_delayable(&srv->transition.timer, srv);

    srv->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = bt_mesh_light_ctl_pub_update;
    }

    if (light_ctl_temp_svr_usr_data == NULL) {
        light_ctl_temp_svr_usr_data = srv;
    }

    return 0;
}

static int bt_mesh_light_ctl_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Lighting Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    int ret = 0;
    switch (model->id) {
        case BT_MESH_MODEL_ID_LIGHT_CTL_SRV: {
            ble_mesh_light_ctl_srv_t *srv = model->user_data;
            if (srv->state == NULL) {
                BT_ERR("Invalid Light CTL State");
                return -EINVAL;
            }

            k_work_init_delayable(&srv->transition.timer, light_ctl_work_handler);
            k_work_add_arg_delayable(&srv->transition.timer, srv);

            srv->model = model;
            if (light_ctl_svr_usr_data == NULL) {
                light_ctl_svr_usr_data = srv;
            }

            break;
        }
        case BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV: {
            ble_mesh_light_ctl_setup_srv_t *srv = model->user_data;
            if (srv->state == NULL) {
                BT_ERR("Invalid Light CTL State");
                return -EINVAL;
            }
            srv->model = model;
            if (light_ctl_setup_svr_usr_data == NULL) {
                light_ctl_setup_svr_usr_data = srv;
            }

            break;
        }
        case BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
            ret = bt_mesh_light_ctl_temp_init(model);
            break;
        }
        default:
            BT_WARN("Unknown Light Server, model id 0x%04x", model->id);
            return -EINVAL;
    }

    return ret;
}

static int bt_mesh_light_ctl_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Server does not support the publish function.");
        return -EINVAL;
    }

    struct bt_mesh_elem *elem = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(elem, BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV) == NULL) {
        BT_WARN("Light CTL Setup Server does not exist");
        /* Just give a warning here, continue with the initialization */
    }
    if (bt_mesh_elem_count() < LIGHT_CTL_ELEM_COUNT_MIN) {
        BT_WARN("The Light CTL Server requires two elements");
        /* Just give a warning here, continue with the initialization */
    }
    return bt_mesh_light_ctl_server_init(model);
}

static int bt_mesh_light_ctl_setup_srv_init(struct bt_mesh_model *model)
{
    return bt_mesh_light_ctl_server_init(model);
}

static int bt_mesh_light_ctl_temp_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("Light CTL Temperature Server has no publication support");
        return -EINVAL;
    }

    return bt_mesh_light_ctl_server_init(model);
}


static int bt_mesh_light_server_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (model->user_data == NULL) {
        BT_ERR("Invalid Lighting Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    uint32_t ret;
    switch (model->id) {
        case BT_MESH_MODEL_ID_LIGHT_CTL_SRV: {
            ble_mesh_light_ctl_srv_t *srv = model->user_data;
            ret = k_work_cancel_delayable_delete(&srv->transition.timer);
            if (ret != BLE_MESH_SUCC) {
                BT_ERR("Light CTL timer delete err, ret %04x", ret);
            }

            break;
        }
        case BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV: {
            ble_mesh_light_ctl_temp_srv_t *srv = model->user_data;
            ret = k_work_cancel_delayable_delete(&srv->transition.timer);
            if (ret != BLE_MESH_SUCC) {
                BT_ERR("Light CTL Temp delete timer err, ret %u", ret);
            }
            break;
        }
        case BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV:
            break;
        default:
            BT_WARN("Unknown Light Server, model id 0x%04x", model->id);
            return -EINVAL;
    }

    return 0;
}

static int bt_mesh_light_ctl_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("light ctl server has no publication support");
        return -EINVAL;
    }

    return bt_mesh_light_server_deinit(model);
}

static int bt_mesh_light_ctl_setup_srv_deinit(struct bt_mesh_model *model)
{
    return bt_mesh_light_server_deinit(model);
}

static int bt_mesh_light_ctl_temp_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("light ctl tempe server has no publication support");
        return -EINVAL;
    }

    return bt_mesh_light_server_deinit(model);
}


const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb = {
    .init = bt_mesh_light_ctl_srv_init,
    .deinit = bt_mesh_light_ctl_srv_deinit,
};

const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb = {
    .init = bt_mesh_light_ctl_setup_srv_init,
    .deinit = bt_mesh_light_ctl_setup_srv_deinit,
};

const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb = {
    .init = bt_mesh_light_ctl_temp_srv_init,
    .deinit = bt_mesh_light_ctl_temp_srv_deinit,
};
