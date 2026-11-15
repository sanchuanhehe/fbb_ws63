/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Generic Server Model Function Implementation
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
#include "ble_mesh_model_op.h"
#include "state_transition.h"
#include "state_binding.h"
#include "device_property.h"
#include "scene_server.h"
#include "model/generic_server.h"

#define UNKNOWN_REMAIN_TIME 0x3F
#define ONOFF_STATUS_LEN 3
#define LEVEL_STATUS_LEN 5
#define DTT_STATUS_LEN 1
#define POWER_ONOFF_STATUS_LEN 1
#define BATTERY_STATUS_LEN 8
#define POWER_LEVEL_STATUS_LEN 5
#define LOC_GLOBAL_STATUS_LEN 10
#define LOC_LOCAL_STATUS_LEN 9
#define USR_PROP_LEN 3
#define MANU_PROP_LEN 3
/* latitude maximum range */
#define LATITUDE_MAX 90
/* longitude maximum range */
#define LONGITUDE_MAX 180
#define DEVICE_SPECIFIC_RESOLUTION 10
uint8_t g_gen_default_tt_flag = false;  // bt_mesh_op_gen_move_set 的过渡时间必须与0x3f取或, 其他场景不需要
struct bt_mesh_gen_onoff_srv *gen_onoff_srv;
struct bt_mesh_gen_level_srv *gen_level_srv;

uint8_t g_gen_def_trans_time = 0;
static void send_gen_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + ONOFF_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, srv->onoff);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_u8(msg, srv->target_onoff);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    scene_change_light_status(srv->target_onoff, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish_duration(model,
            (srv->transition.total_duration + K_MSEC(DELAY_TIME_EXTEND * srv->transition.delay)),
            srv->transition.trans_time));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_onoff_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_onoff_status(model, ctx, false);
    return 0;
}

static void delay_trans_time_assignment(struct transition *transition, uint8_t trans_time, uint8_t delay)
{
    transition->trans_time = trans_time;
    transition->delay = delay;
    transition->just_started = true;
}
static int bt_mesh_op_gen_onoff_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    uint8_t target_onoff, tid, delay, trans_time;
    int64_t now;

    if (!srv) {
        return -EINVAL;
    }
    target_onoff = net_buf_simple_pull_u8(buf);
    if (target_onoff > 1) {
        BT_ERR("net_buf_simple_pull_u8 failed\n");
        return -EINVAL;
    }
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            send_gen_onoff_status(model, ctx, false);
        }
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }

    BT_DBG("tid=%u, target_onoff=%u, trans_time=%u, delay=%u", tid, target_onoff, trans_time, delay);

    bt_mesh_server_stop_transition(&srv->transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);

    srv->target_onoff = target_onoff;
    if (srv->target_onoff == srv->onoff) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            send_gen_onoff_status(model, ctx, false);
        }
        send_gen_onoff_status(model, ctx, true);
        return 0;
    }

    delay_trans_time_assignment(&srv->transition, trans_time, delay);
    onoff_tt_values(srv);

    if (srv->transition.counter == 0) {
        srv->onoff = srv->target_onoff;
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
        send_gen_onoff_status(model, ctx, false);
    }
    send_gen_onoff_status(model, ctx, true);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

void bt_mesh_gen_onoff_set_scene(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    if (!model) {
        return;
    }
    struct bt_mesh_gen_onoff_srv *srv = model->user_data;
    uint8_t target_onoff;
    uint8_t tid = 0;
    int64_t now;

    if (!srv) {
        return;
    }
    target_onoff = srv->target_onoff;
    if (target_onoff > 1) {
        BT_ERR("net_buf_simple_pull_u8 failed\n");
        return;
    }

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);
    onoff_tt_values(srv);

    if (srv->transition.counter == 0) {
        srv->onoff = srv->target_onoff;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
        send_gen_onoff_status(model, ctx, false);
    }
    send_gen_onoff_status(model, ctx, true);
    bt_mesh_server_start_transition(&srv->transition);
}

static void send_gen_level_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    if (!model) {
        return;
    }
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    struct os_mbuf *msg = NULL;

    if (!srv) {
        return;
    }
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LEVEL_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, srv->level_state.current_level);

    if (srv->transition.counter > 0 && srv->transition.counter <= DEVICE_SPECIFIC_RESOLUTION) {
        calculate_rt(&srv->transition);
        if (g_gen_default_tt_flag) {
            srv->transition.remain_time |= UNKNOWN_REMAIN_TIME;
        }
        net_buf_simple_add_le16(msg, srv->level_state.target_level);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }

    BT_INFO("current_level=%d, target_level=%d, remain_time=%u, counter: %d",
        srv->level_state.current_level,
        srv->level_state.target_level,
        srv->transition.remain_time,
        srv->transition.counter);

    scene_change_light_status(srv->level_state.target_level, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);
    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish_duration(model,
            (srv->transition.total_duration +
                K_MSEC(DELAY_TIME_EXTEND * srv->transition.delay * srv->transition.counter)),
            srv->transition.trans_time));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_level_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_level_status(model, ctx, false);
    return 0;
}

static int bt_mesh_op_gen_level_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    g_gen_default_tt_flag = false;
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    int16_t target_level;
    uint8_t tid, delay, trans_time;
    int64_t now;

    if (!srv) {
        return -EINVAL;
    }

    target_level = (int16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            send_gen_level_status(model, ctx, false);
        }
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }
    BT_INFO("tid=%u, target_level=%u, trans_time=%u, delay=%u", tid, target_level, trans_time, delay);
    bt_mesh_server_stop_transition(&srv->transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);

    srv->level_state.target_level = target_level;
    if (srv->level_state.target_level == srv->level_state.current_level) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, ctx, true);
        return 0;
    }

    srv->transition.trans_time = trans_time;
    srv->transition.delay = delay;
    level_tt_values(srv);

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0) {
        srv->level_state.current_level = srv->level_state.target_level;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, ctx, true);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

void bt_mesh_gen_level_set_scene(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bt_mesh_scene_data_t *p, bt_mesh_scene_recall_t *p_recall)
{
    g_gen_default_tt_flag = false;
    if (!model) {
        return;
    }
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    uint8_t tid = 0;
    int64_t now = 0;

    if (!srv) {
        return;
    }
    bt_mesh_server_stop_transition(&srv->transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);
    if (srv->level_state.current_level == p->scene_level) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, ctx, true);
        return;
    }
    srv->level_state.target_level = p->scene_level;
    if (p_recall->transit_t != 0) {
        srv->transition.trans_time = p_recall->transit_t;
        srv->transition.delay = p_recall->delay;
    }
    level_tt_values(srv);

    BT_DBG("transition counter %u", srv->transition.counter);

    /* For Instantaneous Transition */
    if (srv->transition.counter == 0) {
        srv->level_state.current_level = srv->level_state.target_level;
    }

    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, ctx, true);
    bt_mesh_server_start_transition(&srv->transition);
}

static void restrict_level_value(int32_t *level)
{
    if (*level < INT16_MIN) {
        *level = INT16_MIN;
    } else if (*level > INT16_MAX) {
        *level = INT16_MAX;
    }
}

static void delta_level_assignment(struct bt_mesh_gen_level_srv *srv, int32_t delta_level, int32_t temp_level)
{
    srv->level_state.delta_level = delta_level;
    restrict_level_value(&temp_level);
    srv->level_state.target_level = temp_level;
}

static int bt_mesh_op_gen_delta_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    g_gen_default_tt_flag = false;
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    uint8_t delay, trans_time;
    int64_t now;
    int32_t temp_level;

    int32_t delta_level = (int32_t)net_buf_simple_pull_le32(buf);
    uint8_t tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (srv->level_state.delta_level == delta_level) {
            if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
                send_gen_level_status(model, ctx, false);
            }
            return 0;
        }
        temp_level = srv->level_state.last_level + delta_level;
    } else {
        srv->level_state.last_level = srv->level_state.current_level;
        temp_level = srv->level_state.current_level + delta_level;
    }

    if (set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time)) {
        BT_ERR("set_option_field failed");
        return BLE_MESH_FAIL;
    }

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);
    delta_level_assignment(srv, delta_level, temp_level);
    if (srv->level_state.target_level == srv->level_state.current_level) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
            send_gen_level_status(model, ctx, false);
        }
        send_gen_level_status(model, ctx, true);
        return 0;
    }
    delay_trans_time_assignment(&srv->transition, trans_time, delay);

    level_tt_values(srv);
    /* For Instantaneous Transition */
    if (srv->transition.counter == 0) {
        srv->level_state.current_level = srv->level_state.target_level;
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DELTA_SET) {
        send_gen_level_status(model, ctx, false);
    }
    send_gen_level_status(model, ctx, true);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

void gen_level_move_publish(struct bt_mesh_model *model)
{
    struct os_mbuf *msg = model->pub->msg;
    struct bt_mesh_gen_level_srv *srv = model->user_data;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, srv->level_state.current_level);

    if (srv->transition.counter) {
        if (srv->level_state.delta_level < 0) {
            net_buf_simple_add_le16(msg, INT16_MIN);
        } else { /* 0 should not be possible */
            net_buf_simple_add_le16(msg, INT16_MAX);
        }

        net_buf_simple_add_u8(msg, UNKNOWN_REMAIN_TIME);
    }

    BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
}

static void target_level_assignment(struct bt_mesh_gen_level_srv *srv, int16_t delta_level, int32_t temp_level)
{
    int32_t remaining_temp_level = temp_level;
    srv->level_state.delta_level = delta_level;
    remaining_temp_level = srv->level_state.current_level + delta_level;
    restrict_level_value(&remaining_temp_level);
    if (delta_level < 0) {
        remaining_temp_level = INT16_MIN;
    } else if (delta_level > 0) {
        remaining_temp_level = INT16_MAX;
    } else if (delta_level == 0) {
        remaining_temp_level = srv->level_state.current_level;
    }
    srv->level_state.target_level = remaining_temp_level;
}

static int bt_mesh_op_gen_move_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    g_gen_default_tt_flag = true;
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_level_srv *srv = model->user_data;
    int16_t delta_level;
    uint8_t tid, delay, trans_time;
    int64_t now;
    int32_t temp_level = 0;

    if (!srv) {
        return -EINVAL;
    }
    delta_level = (int16_t)net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
            send_gen_level_status(model, ctx, false);
        }
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }

    bt_mesh_server_stop_transition(&srv->transition);
    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);
    target_level_assignment(srv, delta_level, temp_level);
    if (srv->level_state.target_level == srv->level_state.current_level) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
            send_gen_level_status(model, ctx, false);
        }
        gen_level_move_publish(model);
        return 0;
    }

    srv->transition.trans_time = trans_time;
    srv->transition.delay = delay;
    level_tt_values(srv);
    srv->transition.just_started = true;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MOVE_SET) {
        send_gen_level_status(model, ctx, false);
    }
    gen_level_move_publish(model);

    // speeed = delta_level/trans_time 如果生成的trans_time等于0，则不会启动任何通用级别状态更改
    if (srv->transition.counter == 0) {
        return 0;
    }

    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

static void send_gen_def_trans_time_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + DTT_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, g_gen_def_trans_time);

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_def_trans_time_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_def_trans_time_status(model, ctx, false);
    return 0;
}

static int bt_mesh_op_gen_def_trans_time_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_dtt_srv *srv = model->user_data;

    if (!srv) {
        return -EINVAL;
    }
    uint8_t trans_time = net_buf_simple_pull_u8(buf);
    /* Here, Model specification is silent about tid implementation */
    if ((trans_time & 0x3F) == 0x3F) {
        return -EINVAL;
    }

    if (srv->dtt != trans_time) {
        srv->dtt = trans_time;
        g_gen_def_trans_time = trans_time;
        if (srv->set) {
            srv->set(model, trans_time);
        }
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET) {
        send_gen_def_trans_time_status(model, ctx, false);
    }
    send_gen_def_trans_time_status(model, ctx, true);
    return 0;
}

static void send_gen_onpowerup_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + POWER_ONOFF_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS);
    net_buf_simple_add_u8(msg, srv->power_onoff_state->power_onoff);

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_onpowerup_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_onpowerup_status(model, ctx, false);
    return 0;
}

static int bt_mesh_op_gen_onpowerup_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    uint8_t set_onpowerup;
    struct bt_mesh_gen_power_onoff_setup_srv *srv = model->user_data;

    if (!srv) {
        return -EINVAL;
    }
    set_onpowerup = net_buf_simple_pull_u8(buf);
    if (set_onpowerup > 0x02) {
        BT_ERR("Invaild onpowerup value");
        return -EINVAL;
    }

    BT_INFO("current %u, target %u", srv->power_onoff_state->power_onoff, set_onpowerup);
    if (srv->power_onoff_state->power_onoff != set_onpowerup) {
        srv->power_onoff_state->power_onoff = set_onpowerup;
        gen_onpowerup_bind_state(set_onpowerup, model);
        if (srv->set) {
            srv->set(model, set_onpowerup);
        }
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET) {
        send_gen_onpowerup_status(model, ctx, false);
    }
    send_gen_onpowerup_status(model, ctx, true);
    return 0;
}

static void send_gen_battery_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + BATTERY_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    struct bt_mesh_gen_battery_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS);
    net_buf_simple_add_u8(msg, srv->battery_state.battery_level);
    net_buf_simple_add_le24(msg, srv->battery_state.time_to_discharge);
    net_buf_simple_add_le24(msg, srv->battery_state.time_to_charge);
    net_buf_simple_add_u8(msg, srv->battery_state.flags);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        BT_ERR("Unable to send GEN_BATTERY Status response");
    }

    os_mbuf_free_chain(msg);
}

static int bt_mesh_op_gen_battery_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_battery_status(model, ctx);
    return 0;
}

static int gen_power_level_status_msg(struct os_mbuf *msg, uint16_t opcode, struct bt_mesh_gen_power_level_srv *srv)
{
    bt_mesh_model_msg_init(msg, opcode);
    switch (opcode) {
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS:
            net_buf_simple_add_le16(msg, srv->pwr_level_state->current_level);
            BT_INFO("current_level=%u, target_level=%u",
                srv->pwr_level_state->current_level,
                srv->pwr_level_state->target_level);
            if (srv->transition.counter) {
                net_buf_simple_add_le16(msg, srv->pwr_level_state->target_level);
                calculate_rt(&srv->transition);
                net_buf_simple_add_u8(msg, srv->transition.remain_time);
            }
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS:
            net_buf_simple_add_le16(msg, srv->pwr_level_state->last_level);
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS:
            net_buf_simple_add_le16(msg, srv->pwr_level_state->default_level);
            break;
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS:
            net_buf_simple_add_u8(msg, srv->pwr_level_state->status_code);
            net_buf_simple_add_le16(msg, srv->pwr_level_state->min_level);
            net_buf_simple_add_le16(msg, srv->pwr_level_state->max_level);
            break;
        default:
            BT_ERR("Invaild opcode");
            return -EINVAL;
    }
    return BLE_MESH_SUCC;
}

static void send_gen_power_level_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + POWER_LEVEL_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    if (gen_power_level_status_msg(msg, opcode, srv) != BLE_MESH_SUCC) {
        if (!publish) {
            os_mbuf_free_chain(msg);
        }
        return;
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_power_level_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, false);
    return 0;
}

static void target_pwr_level_assignment(struct bt_mesh_gen_power_level_srv *srv, uint16_t target_pwr_level)
{
    uint16_t remaining_target_pwr_level = target_pwr_level;
    if (remaining_target_pwr_level > srv->pwr_level_state->max_level) {
        remaining_target_pwr_level = srv->pwr_level_state->max_level;
    } else if (remaining_target_pwr_level < srv->pwr_level_state->min_level) {
        remaining_target_pwr_level = srv->pwr_level_state->min_level;
    }
    srv->pwr_level_state->target_level = remaining_target_pwr_level;
}
static int bt_mesh_op_gen_power_level_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;
    uint16_t target_pwr_level;
    uint8_t tid, delay, trans_time;
    int64_t now;

    if (!srv) {
        return -EINVAL;
    }
    target_pwr_level = net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);
    if (bt_mesh_is_server_recv_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now)) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
            send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, false);
        }
        send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, true);
        return 0;
    }

    int ret = set_option_field(buf, &trans_time, &delay, g_gen_def_trans_time);
    if (ret == BLE_MESH_FAIL) {
        BT_ERR("set_option_field failed");
        return ret;
    }

    bt_mesh_server_stop_transition(&srv->transition);

    bt_mesh_server_update_last_msg(&srv->last_msg, tid, ctx->addr, ctx->recv_dst, &now);

    target_pwr_level_assignment(srv, target_pwr_level);
    if (srv->pwr_level_state->target_level == srv->pwr_level_state->current_level) {
        if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
            send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, false);
        }
        send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, true);
        return 0;
    }

    delay_trans_time_assignment(&srv->transition, trans_time, delay);
    power_level_tt_values(srv);

    if (srv->transition.counter == 0) {
        srv->pwr_level_state->current_level = srv->pwr_level_state->target_level;
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
        send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, false);
    }
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, true);
    bt_mesh_server_start_transition(&srv->transition);
    return 0;
}

static int bt_mesh_op_gen_power_last_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS, false);
    return 0;
}

static int bt_mesh_op_gen_power_default_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS, false);
    return 0;
}

static int bt_mesh_op_gen_power_default_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;
    if (!srv) {
        return -EINVAL;
    }
    uint16_t default_pwr_level = net_buf_simple_pull_le16(buf);
    if (default_pwr_level > srv->pwr_level_state->max_level) {
        default_pwr_level = srv->pwr_level_state->max_level;
    } else if (default_pwr_level < srv->pwr_level_state->min_level) {
        default_pwr_level = srv->pwr_level_state->min_level;
    }
    srv->pwr_level_state->default_level = default_pwr_level;
    BT_INFO("default_pwr_level=%u", default_pwr_level);
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET) {
        send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS, false);
    }
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS, true);
    return 0;
}

static int bt_mesh_op_gen_power_range_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS, false);
    return 0;
}

static int bt_mesh_op_gen_power_range_set(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_power_level_srv *srv = model->user_data;
    uint16_t min_level;
    uint16_t max_level;

    if (!srv) {
        return -EINVAL;
    }
    min_level = net_buf_simple_pull_le16(buf);
    max_level = net_buf_simple_pull_le16(buf);
    if (min_level < 1 || min_level > max_level) {
        BT_ERR("Invaild range");
        return -EINVAL;
    }
    srv->pwr_level_state->min_level = min_level;
    srv->pwr_level_state->max_level = max_level;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET) {
        send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS, false);
    }
    send_gen_power_level_status(model, ctx, BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS, true);
    return 0;
}

static void send_gen_loc_global_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LOC_GLOBAL_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    struct bt_mesh_gen_location_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS);
    net_buf_simple_add_le32(msg, srv->location_state->global_latitude);
    net_buf_simple_add_le32(msg, srv->location_state->global_longitude);
    net_buf_simple_add_le16(msg, srv->location_state->global_altitude);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL) != BLE_MESH_SUCC) {
        BT_ERR("Unable to send gen_loc_global Status response \r\n");
    }

    os_mbuf_free_chain(msg);
}

static int bt_mesh_op_gen_loc_global_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_loc_global_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET, false);
    return 0;
}

static void send_gen_loc_local_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + LOC_LOCAL_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    struct bt_mesh_gen_location_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS);
    net_buf_simple_add_le16(msg, srv->loc_local_state->local_altitude);
    net_buf_simple_add_le16(msg, srv->loc_local_state->local_east);
    net_buf_simple_add_le16(msg, srv->loc_local_state->local_north);
    net_buf_simple_add_u8(msg, srv->loc_local_state->floor_number);
    net_buf_simple_add_le16(msg, srv->loc_local_state->uncertainty);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL) != BLE_MESH_SUCC) {
        BT_ERR("Unable to send gen_loc_local Status response \r\n");
    }

    os_mbuf_free_chain(msg);
}

static int bt_mesh_op_gen_loc_local_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_loc_local_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET, false);
    return 0;
}

static int bt_mesh_op_gen_loc_global_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
    uint32_t global_latitude;
    uint32_t global_longitude;

    if (!srv) {
        return -EINVAL;
    }
    global_latitude = net_buf_simple_pull_le32(buf);
    global_longitude = net_buf_simple_pull_le32(buf);
    if (global_latitude > LATITUDE_MAX || global_longitude > LONGITUDE_MAX) {
        BT_ERR("Invaild Location information. \r\n");
        return -EINVAL;
    }
    srv->location_state->global_latitude = global_latitude;
    srv->location_state->global_longitude = global_longitude;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET) {
        send_gen_loc_global_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS, false);
    }
    send_gen_loc_global_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS, true);
    return 0;
}

static int bt_mesh_op_gen_loc_local_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    struct bt_mesh_gen_location_setup_srv *srv = model->user_data;
    uint16_t local_north;
    uint16_t local_east;

    if (!srv) {
        return -EINVAL;
    }
    local_north = net_buf_simple_pull_le16(buf);
    local_east = net_buf_simple_pull_le16(buf);
    if (local_north == 0 || local_east == 0) {
        BT_ERR("Invaild Location local information. \r\n");
        return -EINVAL;
    }
    srv->loc_local_state->local_north = local_north;
    srv->loc_local_state->local_east = local_east;
    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET) {
        send_gen_loc_local_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS, false);
    }
    send_gen_loc_local_status(model, ctx, BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS, true);
    return 0;
}

static void send_gen_user_prop_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t user_prop_id, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    if (publish) {
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
        msg = model->pub->msg;
    }

    bool recv_op = ((ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK) ||
                    (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET) ||
                    (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET));
    if (recv_op) {
        struct generic_user_property_state user_prop_state = {0};
        user_prop_state.user_prop_id = user_prop_id;

        if (srv->get_prop) {
            srv->get_prop(model, &user_prop_state);
        }
        if (!user_prop_state.user_prop_val) {
            BT_ERR("get user property from server failed");
            return;
        }
        if (!publish) {
            int len = bt_mesh_get_dev_prop_len(user_prop_state.user_prop_id);
            msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + USR_PROP_LEN + len + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        }
        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS);
        net_buf_simple_add_le16(msg, user_prop_state.user_prop_id);
        net_buf_simple_add_u8(msg, user_prop_state.user_access);
        net_buf_simple_add_mem(msg, user_prop_state.user_prop_val->om_data, user_prop_state.user_prop_val->om_len);
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static void send_gen_user_props_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t user_prop_id, bool publish)
{
    if (!model) {
        return;
    }
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;

    if (!srv) {
        return;
    }
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET) {
        struct generic_user_properties_state user_props_state = {0};
        if (srv->get_props) {
            srv->get_props(model, &user_props_state);
        }

        if (!user_props_state.user_prop_ids) {
            BT_ERR("get user properties from server failed");
            return;
        }

        if (!publish) {
            msg = NET_BUF_SIMPLE(
                MODEL_OPCODE_LEN + user_props_state.user_prop_ids->om_len + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        }

        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS);
        net_buf_simple_add_mem(msg, user_props_state.user_prop_ids->om_data, user_props_state.user_prop_ids->om_len);
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_user_prop_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint16_t user_prop_id = net_buf_simple_pull_le16(buf);
    send_gen_user_prop_status(model, ctx, user_prop_id, false);
    return 0;
}

static int bt_mesh_op_gen_user_props_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_user_props_status(model, ctx, 0, false);
    return 0;
}

static int bt_mesh_op_gen_user_prop_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    uint16_t user_prop_id;
    struct os_mbuf *user_prop_val = NULL;
    struct bt_mesh_gen_user_prop_srv *srv = model->user_data;

    if (!srv) {
        return -EINVAL;
    }
    user_prop_id = net_buf_simple_pull_le16(buf);
    int len = bt_mesh_get_dev_prop_len(user_prop_id);
    if (len != buf->om_len) {
        BT_ERR("Invaild value len for user_prop_id, id=%u, err len=%u", user_prop_id, buf->om_len);
        return -EINVAL;
    }
    user_prop_val = NET_BUF_SIMPLE(len);
    net_buf_simple_add_mem(user_prop_val, buf->om_data, buf->om_len);

    if (srv->set) {
        srv->set(model, user_prop_id, user_prop_val);
    }

    os_mbuf_free_chain(user_prop_val);

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET) {
        send_gen_user_prop_status(model, ctx, user_prop_id, false);
    }
    send_gen_user_prop_status(model, ctx, user_prop_id, true);
    return 0;
}

static void send_gen_manu_prop_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t manu_prop_id, bool publish)
{
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_manufacturer_prop_srv *srv = model->user_data;
    if (publish) {
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
        msg = model->pub->msg;
    }

    bool recv_op = ((ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK) ||
                    (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET) ||
                    (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET));
    if (recv_op) {
        struct generic_manu_property_state manu_prop_state = {0};
        manu_prop_state.manu_prop_id = manu_prop_id;
        if (srv->get_prop) {
            srv->get_prop(model, &manu_prop_state);
        }
        if (!manu_prop_state.manu_prop_val) {
            BT_ERR("get manufacturer property from server failed");
            return;
        }

        if (!publish) {
            int len = bt_mesh_get_dev_prop_len(manu_prop_state.manu_prop_id);
            msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + MANU_PROP_LEN + len + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        }

        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS);
        net_buf_simple_add_le16(msg, manu_prop_state.manu_prop_id);
        net_buf_simple_add_u8(msg, manu_prop_state.manu_access);
        net_buf_simple_add_mem(msg, manu_prop_state.manu_prop_val->om_data, manu_prop_state.manu_prop_val->om_len);
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static void send_gen_manu_props_status(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t manu_prop_id, bool publish)
{
    struct os_mbuf *msg = NULL;
    struct bt_mesh_gen_manufacturer_prop_srv *srv = model->user_data;
    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET) {
        struct generic_manu_properties_state manu_props_state = {0};
        if (srv->get_props) {
            srv->get_props(model, &manu_props_state);
        }

        if (!manu_props_state.manu_prop_ids) {
            BT_ERR("get manufacturer properties from server failed");
            return;
        }

        if (!publish) {
            msg = NET_BUF_SIMPLE(
                MODEL_OPCODE_LEN + manu_props_state.manu_prop_ids->om_len + BLE_MESH_SERVER_TRANS_MIC_SIZE);
        }

        bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS);
        net_buf_simple_add_mem(msg, manu_props_state.manu_prop_ids->om_data, manu_props_state.manu_prop_ids->om_len);
    }

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_op_gen_manu_prop_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint16_t manu_prop_id = net_buf_simple_pull_le16(buf);
    send_gen_manu_prop_status(model, ctx, manu_prop_id, false);
    return 0;
}

static int bt_mesh_op_gen_manu_props_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    send_gen_manu_props_status(model, ctx, 0, false);
    return 0;
}

static int bt_mesh_op_gen_manu_prop_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (!model) {
        return -EINVAL;
    }
    uint16_t manu_prop_id;
    uint16_t manu_access;
    struct bt_mesh_gen_manufacturer_prop_srv *srv = model->user_data;

    if (!srv) {
        return -EINVAL;
    }
    manu_prop_id = net_buf_simple_pull_le16(buf);
    manu_access = net_buf_simple_pull_u8(buf);

    if (srv->set) {
        srv->set(model, manu_prop_id, manu_access);
    }

    if (ctx->recv_op == BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET) {
        send_gen_manu_prop_status(model, ctx, manu_prop_id, false);
    }
    send_gen_manu_prop_status(model, ctx, manu_prop_id, true);
    return 0;
}

const struct bt_mesh_model_op gen_onoff_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_ONOFF_GET,       0, bt_mesh_op_gen_onoff_get},
    {BLE_MESH_MODEL_OP_GEN_ONOFF_SET,       2, bt_mesh_op_gen_onoff_set},
    {BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, bt_mesh_op_gen_onoff_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_level_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_LEVEL_GET,       0, bt_mesh_op_gen_level_get},
    {BLE_MESH_MODEL_OP_GEN_LEVEL_SET,       3, bt_mesh_op_gen_level_set},
    {BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK, 3, bt_mesh_op_gen_level_set},
    {BLE_MESH_MODEL_OP_GEN_DELTA_SET,       5, bt_mesh_op_gen_delta_set},
    {BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK, 5, bt_mesh_op_gen_delta_set},
    {BLE_MESH_MODEL_OP_GEN_MOVE_SET,        3, bt_mesh_op_gen_move_set},
    {BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK,  3, bt_mesh_op_gen_move_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_def_trans_time_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET,       0, bt_mesh_op_gen_def_trans_time_get},
    {BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET,       1, bt_mesh_op_gen_def_trans_time_set},
    {BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK, 1, bt_mesh_op_gen_def_trans_time_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_onoff_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET, 0, bt_mesh_op_gen_onpowerup_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET,       1, bt_mesh_op_gen_onpowerup_set},
    {BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK, 1, bt_mesh_op_gen_onpowerup_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_battery_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_BATTERY_GET, 0, bt_mesh_op_gen_battery_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_level_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET,         0, bt_mesh_op_gen_power_level_get},
    {BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET,         3, bt_mesh_op_gen_power_level_set},
    {BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK,   3, bt_mesh_op_gen_power_level_set},
    {BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET,          0, bt_mesh_op_gen_power_last_get},
    {BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET,       0, bt_mesh_op_gen_power_default_get},
    {BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET,         0, bt_mesh_op_gen_power_range_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_level_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET,       2, bt_mesh_op_gen_power_default_set},
    {BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK, 2, bt_mesh_op_gen_power_default_set},
    {BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET,         4, bt_mesh_op_gen_power_range_set},
    {BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK,   4, bt_mesh_op_gen_power_range_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_location_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET, 0, bt_mesh_op_gen_loc_global_get},
    {BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET,  0, bt_mesh_op_gen_loc_local_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_location_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET,       10, bt_mesh_op_gen_loc_global_set},
    {BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK, 10, bt_mesh_op_gen_loc_global_set},
    {BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET,        9,  bt_mesh_op_gen_loc_local_set},
    {BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK,  9,  bt_mesh_op_gen_loc_local_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_user_prop_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET,       2, bt_mesh_op_gen_user_prop_get},
    {BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET,     0, bt_mesh_op_gen_user_props_get},
    {BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET,       3, bt_mesh_op_gen_user_prop_set},
    {BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK, 3, bt_mesh_op_gen_user_prop_set},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_manu_prop_srv_op[] = {
    {BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET,       2, bt_mesh_op_gen_manu_prop_get},
    {BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET,     0, bt_mesh_op_gen_manu_props_get},
    {BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET,       3, bt_mesh_op_gen_manu_prop_set},
    {BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK, 3, bt_mesh_op_gen_manu_prop_set},
    BT_MESH_MODEL_OP_END,
};

static int onoff_pub_update(struct bt_mesh_model *mod)
{
    struct bt_mesh_gen_onoff_srv *srv = mod->user_data;
    struct os_mbuf *msg = mod->pub->msg;
    if (!srv || !msg) {
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, srv->onoff);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_u8(msg, srv->target_onoff);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }
    return 0;
}

static int level_srv_pub_update(struct bt_mesh_model *mod)
{
    struct bt_mesh_gen_level_srv *srv = mod->user_data;
    struct os_mbuf *msg = mod->pub->msg;
    if (!srv || !msg) {
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, srv->level_state.current_level);

    if (srv->transition.counter) {
        calculate_rt(&srv->transition);
        net_buf_simple_add_le16(msg, srv->level_state.target_level);
        net_buf_simple_add_u8(msg, srv->transition.remain_time);
    }
    return 0;
}

static int onoff_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_onoff_srv *cfg = model->user_data;
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cfg) {
        BT_ERR("No Generic OnOff Server context provided");
        return -EINVAL;
    }

    k_work_cancel_delayable_delete(&cfg->transition.timer);

    return 0;
}

static int onoff_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_onoff_srv *cfg = model->user_data;

    BT_DBG("");

    if (!cfg) {
        BT_ERR("No Generic OnOff Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    if (gen_onoff_srv == NULL) {
        gen_onoff_srv = cfg;
    }

    if (!model->pub) {
        BT_ERR("OnOff Server has no publication support");
        return -EINVAL;
    }

    model->pub->update = onoff_pub_update;
    if (model->pub->msg == NULL) {
        model->pub->msg = NET_BUF_SIMPLE(MODEL_ONOFF_PUB_MSG_LEN);
    }
    k_work_init_delayable(&cfg->transition.timer, generic_onoff_work_handler);
    k_work_add_arg_delayable(&cfg->transition.timer, cfg);

    return 0;
}

const struct bt_mesh_model_cb gen_onoff_srv_cb = {
    .init = onoff_srv_init,
    .deinit = onoff_srv_deinit,
};

static int level_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_level_srv *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cfg) {
        BT_ERR("No Generic Level Server context provided");
        return -EINVAL;
    }

    k_work_cancel_delayable_delete(&cfg->transition.timer);

    return 0;
}

static int level_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_level_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Generic Level Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    if (gen_level_srv == NULL) {
        gen_level_srv = cfg;
    }
    model->pub->update = level_srv_pub_update;
    if (model->pub->msg == NULL) {
        model->pub->msg = NET_BUF_SIMPLE(MODEL_LEVEL_PUB_MSG_LEN);
    }
    k_work_init_delayable(&cfg->transition.timer, generic_level_work_handler);
    k_work_add_arg_delayable(&cfg->transition.timer, cfg);

    return 0;
}

const struct bt_mesh_model_cb gen_level_srv_cb = {
    .init = level_srv_init,
    .deinit = level_srv_deinit,
};

static int gen_def_trans_time_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_dtt_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Generic Default Transition Time Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}
static int gen_def_trans_time_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }
    return 0;
}
const struct bt_mesh_model_cb gen_dtt_srv_cb = {
    .init = gen_def_trans_time_srv_init,
    .deinit = gen_def_trans_time_srv_deinit,
};

static int gen_battery_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_battery_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Generic Battery Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}
static int gen_battery_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_battery_srv *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}
const struct bt_mesh_model_cb gen_battery_srv_cb = {
    .init = gen_battery_srv_init,
    .deinit = gen_battery_srv_deinit,
};

static int gen_power_level_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_level_srv *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cfg) {
        k_work_cancel_delayable_delete(&(cfg->transition.timer));
    }

    return 0;
}

static int gen_power_level_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_level_srv *cfg = model->user_data;

    bool cond = !cfg || !cfg->pwr_level_state;
    if (cond) {
        BT_ERR("No Power Level Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    k_work_init_delayable(&(cfg->transition.timer), generic_power_level_work_handler);
    k_work_add_arg_delayable(&cfg->transition.timer, cfg);

    return 0;
}

const struct bt_mesh_model_cb gen_power_level_srv_cb = {
    .init = gen_power_level_srv_init,
    .deinit = gen_power_level_srv_deinit,
};

static int gen_power_level_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_level_setup_srv *cfg = model->user_data;

    bool cond = !cfg || !cfg->pwr_level_state;
    if (cond) {
        BT_ERR("No Power Level Server Setup context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}
static int gen_power_level_setup_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

const struct bt_mesh_model_cb gen_power_level_setup_srv_cb = {
    .init = gen_power_level_setup_srv_init,
    .deinit = gen_power_level_setup_srv_deinit,
};

static int gen_power_onoff_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

static int gen_power_onoff_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_onoff_srv *cfg = model->user_data;

    bool cond = !cfg || !cfg->power_onoff_state;
    if (cond) {
        BT_ERR("No Power Onoff Server context provided");
        return -EINVAL;
    }

    cfg->model = model;
    if (model->pub->msg == NULL) {
        model->pub->msg = NET_BUF_SIMPLE(MODEL_ONPOWERUP_PUB_MSG_LEN);
    }
    return 0;
}

const struct bt_mesh_model_cb gen_power_onoff_srv_cb = {
    .init = gen_power_onoff_srv_init,
    .deinit = gen_power_onoff_srv_deinit,
};

static int gen_power_onoff_setup_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_onoff_setup_srv *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

static int gen_power_onoff_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_power_onoff_setup_srv *cfg = model->user_data;

    bool cond = !cfg || !cfg->power_onoff_state;
    if (cond) {
        BT_ERR("No Power Onoff Server Setup context provided");
        return -EINVAL;
    }

    cfg->model = model;

    if (model->pub->msg == NULL) {
        model->pub->msg = NET_BUF_SIMPLE(MODEL_ONPOWERUP_PUB_MSG_LEN);
    }
    return 0;
}

const struct bt_mesh_model_cb gen_power_onoff_setup_srv_cb = {
    .init = gen_power_onoff_setup_srv_init,
    .deinit = gen_power_onoff_setup_srv_deinit,
};

static int gen_location_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_location_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Location Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}
static int gen_location_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}
const struct bt_mesh_model_cb gen_location_srv_cb = {
    .init = gen_location_srv_init,
    .deinit = gen_location_srv_deinit,
};

static int gen_location_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_location_setup_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Location Setup Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}

static int gen_location_setup_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

const struct bt_mesh_model_cb gen_location_setup_srv_cb = {
    .init = gen_location_setup_srv_init,
    .deinit = gen_location_setup_srv_deinit,
};

static int gen_user_prop_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_user_prop_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No User Property Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}

static int gen_user_prop_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }
    return 0;
}

const struct bt_mesh_model_cb gen_user_prop_srv_cb = {
    .init = gen_user_prop_srv_init,
    .deinit = gen_user_prop_srv_deinit,
};

static int gen_manu_prop_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_manufacturer_prop_srv *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No Manufacturer Property Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    return 0;
}

static int gen_manu_prop_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

const struct bt_mesh_model_cb gen_manu_prop_srv_cb = {
    .init = gen_manu_prop_srv_init,
    .deinit = gen_manu_prop_srv_deinit,
};