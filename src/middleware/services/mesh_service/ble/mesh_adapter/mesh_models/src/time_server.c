/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Time Server Model Function Implementation
 *
 * History:
 * 2025-5-21, Create file.
 */
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "os_time.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model/time_server.h"

#define TIME_STATUS_LEN 10
#define TIMEZONE_STATUS_LEN 7
#define TAI_UTC_DELTA_STATUS_LEN 9
#define TIME_ROLE_STATUS_LEN 1

#define TIME_NONE 0x00
#define TIME_AUTHORITY 0x01
#define TIME_RELAY 0x02
#define TIME_CLIENT 0x03

#define MAX_MSG_LENGTH 10

struct bt_mesh_time_srv_t *time_server;
struct bt_mesh_utc_time_t local_utc_time;

static void bt_mesh_send_time_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;
    uint8_t tai_seconds[TAI_SECONDS_LEN] = {0};
    uint16_t val = 0;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIME_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_STATUS);
    net_buf_simple_add_mem(msg, srv->state->tai_seconds, TAI_SECONDS_LEN);
    if (memcmp(srv->state->tai_seconds, tai_seconds, TAI_SECONDS_LEN) != 0) {
        net_buf_simple_add_u8(msg, srv->state->subsecond);
        net_buf_simple_add_u8(msg, srv->state->uncertainty);
        val = (srv->state->tai_utc_delta_curr << 1) | srv->state->time_authority;
        net_buf_simple_add_le16(msg, val);
        net_buf_simple_add_u8(msg, srv->state->timezone_offset_curr);
    }
    BT_INFO("Time Status");
    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_srv_time_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    BT_INFO("Time Get");
    bt_mesh_send_time_status(model, ctx, false);
    return 0;
}

void bt_mesh_tai_to_utc(struct bt_mesh_time_state_t *state)
{
    time_t tai_sec = 0;
    time_t utc_sec = 0;
    int32_t timezone_offset_sec = 0;
    uint32_t npl_time_sec = 0;

    for (int i = 0; i < TAI_SECONDS_LEN; i++) {
        tai_sec = (tai_sec << 8) | state->tai_seconds[i]; /* 8: bit offset */
    }

    timezone_offset_sec = (state->timezone_offset_curr - 0x40) * TIMEZONE_STEP * MINUTE_PER_SEC;
    utc_sec = (tai_sec - state->tai_utc_delta_curr) + timezone_offset_sec;
    npl_time_sec = ble_npl_time_ticks_to_ms32(ble_npl_time_get());

    local_utc_time.utc_sec = utc_sec;
    local_utc_time.delta = utc_sec - (npl_time_sec / MSEC_PER_SEC);
}

static int bt_mesh_srv_time_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    uint16_t val = 0;

    if (buf->om_len < 10) { /* 10:time set message minimal payload length */
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(srv->state->tai_seconds, TAI_SECONDS_LEN, net_buf_simple_pull_mem(buf, TAI_SECONDS_LEN), TAI_SECONDS_LEN);
    srv->state->subsecond = net_buf_simple_pull_u8(buf);
    srv->state->uncertainty = net_buf_simple_pull_u8(buf);
    val = net_buf_simple_pull_le16(buf);
    srv->state->time_authority = val & 1;
    srv->state->tai_utc_delta_curr = val >> 1;
    srv->state->timezone_offset_curr = net_buf_simple_pull_u8(buf);

    BT_INFO("tai_seconds:0x%02x%02x%02x%02x%02x",
        srv->state->tai_seconds[0], /* 0:index */
        srv->state->tai_seconds[1], /* 1:index */
        srv->state->tai_seconds[2], /* 2:index */
        srv->state->tai_seconds[3], /* 3:index */
        srv->state->tai_seconds[4]); /* 4:index */
    BT_INFO("subsecond:%u", srv->state->subsecond);
    BT_INFO("uncertainty:%u", srv->state->uncertainty);
    BT_INFO("authority:%u", srv->state->time_authority);
    BT_INFO("tai_utc_delta:%d", srv->state->tai_utc_delta_curr);
    BT_INFO("timezone_offset:%u", srv->state->timezone_offset_curr);

    bt_mesh_tai_to_utc(srv->state);
    bt_mesh_send_time_status(model, ctx, false);
    return 0;
}

static int bt_mesh_server_time_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    uint8_t tai_seconds[TAI_SECONDS_LEN] = {0};
    uint16_t val = 0;
    uint8_t ttl = 0;

    if (srv->state->time_role != TIME_RELAY && srv->state->time_role != TIME_CLIENT) {
        return -EINVAL;
    }

    if (buf->om_len < TAI_SECONDS_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(srv->state->tai_seconds, TAI_SECONDS_LEN, net_buf_simple_pull_mem(buf, TAI_SECONDS_LEN), TAI_SECONDS_LEN);
    if ((memcmp(srv->state->tai_seconds, tai_seconds, TAI_SECONDS_LEN) != 0) &&
        buf->om_len == MAX_MSG_LENGTH - TAI_SECONDS_LEN) {
        srv->state->subsecond = net_buf_simple_pull_u8(buf);
        srv->state->uncertainty = net_buf_simple_pull_u8(buf);
        val = net_buf_simple_pull_le16(buf);
        srv->state->time_authority = val & 1;
        srv->state->tai_utc_delta_curr = val >> 1;
        srv->state->timezone_offset_curr = net_buf_simple_pull_u8(buf);
    }

    if (srv->state->time_role == TIME_RELAY && srv->send_publish == false) {
        ttl = model->pub->ttl;
        model->pub->ttl = 0;
        ble_npl_time_mdelay(20); /* 20:Delay 20milliseconds */
        bt_mesh_send_time_status(model, ctx, true);
        k_work_schedule(&srv->time_transition.timer, K_SECONDS(30)); /* 30:schedule after 30seconds  */
        srv->send_publish = true;
        model->pub->ttl = ttl;
    }
    return 0;
}

static void bt_mesh_send_timezone_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIMEZONE_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_ZONE_STATUS);
    net_buf_simple_add_u8(msg, srv->state->timezone_offset_curr);
    net_buf_simple_add_u8(msg, srv->state->timezone_offset_new);
    net_buf_simple_add_mem(msg, srv->state->tai_zone_change, TAI_OF_ZONE_CHANGE_LEN);

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_srv_timezone_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    bt_mesh_send_timezone_status(model, ctx, false);
    return 0;
}

static int bt_mesh_srv_timezone_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;

    srv->state->timezone_offset_new = net_buf_simple_pull_u8(buf);
    if (buf->om_len < TAI_OF_ZONE_CHANGE_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(
        srv->state->tai_zone_change,
        TAI_OF_ZONE_CHANGE_LEN,
        net_buf_simple_pull_mem(buf, TAI_OF_ZONE_CHANGE_LEN),
        TAI_OF_ZONE_CHANGE_LEN);

    BT_INFO("timezone_offset_new:%u", srv->state->timezone_offset_new);
    BT_INFO("tai_zone_change:0x%02x%02x%02x%02x%02x",
        srv->state->tai_zone_change[0], /* 0:index */
        srv->state->tai_zone_change[1], /* 1:index */
        srv->state->tai_zone_change[2], /* 2:index */
        srv->state->tai_zone_change[3], /* 3:index */
        srv->state->tai_zone_change[4]); /* 4:index */

    bt_mesh_send_timezone_status(model, ctx, false);
    srv->state->timezone_offset_curr = srv->state->timezone_offset_new;
    return 0;
}

static void bt_mesh_send_tai_utc_delta_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TAI_UTC_DELTA_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS);
    net_buf_simple_add_le16(msg, srv->state->tai_utc_delta_curr);
    net_buf_simple_add_le16(msg, srv->state->tai_utc_delta_new);
    net_buf_simple_add_mem(msg, srv->state->tai_delta_change, TAI_OF_DELTA_CHANGE_LEN);

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_srv_tai_utc_delta_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    bt_mesh_send_tai_utc_delta_status(model, ctx, false);
    return 0;
}

static int bt_mesh_srv_tai_utc_delta_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;

    srv->state->tai_utc_delta_new = net_buf_simple_pull_le16(buf);
    if (buf->om_len < TAI_OF_DELTA_CHANGE_LEN) {
        BT_WARN("len = %u error", buf->om_len);
        return -EINVAL;
    }
    memcpy_s(srv->state->tai_delta_change, TAI_OF_DELTA_CHANGE_LEN,
        net_buf_simple_pull_mem(buf, TAI_OF_DELTA_CHANGE_LEN), TAI_OF_DELTA_CHANGE_LEN);

    BT_INFO("tai_utc_delta_new:%u", srv->state->tai_utc_delta_new);
    BT_INFO("tai_delta_change:0x%02x%02x%02x%02x%02x",
        srv->state->tai_delta_change[0], /* 0:index */
        srv->state->tai_delta_change[1], /* 1:index */
        srv->state->tai_delta_change[2], /* 2:index */
        srv->state->tai_delta_change[3], /* 3:index */
        srv->state->tai_delta_change[4]); /* 4:index */

    bt_mesh_send_tai_utc_delta_status(model, ctx, false);
    srv->state->tai_utc_delta_curr = srv->state->tai_utc_delta_new;
    return 0;
}

static void bt_mesh_send_time_role_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + TIME_ROLE_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (!msg) {
        BT_ERR("msg is NULL, publish=%d", publish);
        return;
    }

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_TIME_ROLE_STATUS);
    net_buf_simple_add_u8(msg, srv->state->time_role);

    if (publish) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    }
}

static int bt_mesh_srv_time_role_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    bt_mesh_send_time_role_status(model, ctx, false);
    return 0;
}

static int bt_mesh_srv_time_role_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_time_setup_srv_t *srv = model->user_data;

    uint8_t time_role_set = net_buf_simple_pull_u8(buf);

    BT_INFO("time_role:%u", time_role_set);
    if (time_role_set > 0x03) {    // MAX value. Time Role states.
        BT_ERR("time_role:%u, Invalid value.", srv->state->time_role);
        return -EINVAL;
    }
    srv->state->time_role = time_role_set;

    bt_mesh_send_time_role_status(model, ctx, false);
    return 0;
}

const struct bt_mesh_model_op bt_mesh_time_srv_op[] = {
    {BLE_MESH_MODEL_OP_TIME_GET, 0, bt_mesh_srv_time_get},
    {BLE_MESH_MODEL_OP_TIME_STATUS, 5, bt_mesh_server_time_status},
    {BLE_MESH_MODEL_OP_TIME_ZONE_GET, 0, bt_mesh_srv_timezone_get},
    {BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET, 0, bt_mesh_srv_tai_utc_delta_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_time_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_TIME_SET, 10, bt_mesh_srv_time_set},
    {BLE_MESH_MODEL_OP_TIME_ZONE_SET, 6, bt_mesh_srv_timezone_set},
    {BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET, 7, bt_mesh_srv_tai_utc_delta_set},
    {BLE_MESH_MODEL_OP_TIME_ROLE_GET, 0, bt_mesh_srv_time_role_get},
    {BLE_MESH_MODEL_OP_TIME_ROLE_SET, 1, bt_mesh_srv_time_role_set},
    BT_MESH_MODEL_OP_END,
};
static int time_srv_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}
static int bt_mesh_time_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_time_srv_t *cfg = model->user_data;

    if (!cfg || !cfg->state) {
        BT_ERR("No Time Server context provided");
        return -EINVAL;
    }

    cfg->model = model;
    if (model->pub->update == NULL) {
        model->pub->update = time_srv_pub_update;
    }
    if (time_server == NULL) {
        time_server = cfg;
    }

    return 0;
}

static int bt_mesh_time_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_srv_cb = {
    .init = bt_mesh_time_srv_init,
    .deinit = bt_mesh_time_srv_deinit,
};

static int bt_mesh_time_setup_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_time_setup_srv_t *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (cfg) {
        k_work_cancel_delayable_delete(&(cfg->time_transition.timer));
    }

    return 0;
}

static int bt_mesh_time_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_time_setup_srv_t *cfg = model->user_data;

    if (!cfg || !cfg->state) {
        BT_ERR("No Time Server Setup context provided");
        return -EINVAL;
    }

    cfg->model = model;

    k_work_init_delayable(&(cfg->time_transition.timer), time_work_handler);
    k_work_add_arg_delayable(&(cfg->time_transition.timer), cfg);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb = {
    .init = bt_mesh_time_setup_srv_init,
    .deinit = bt_mesh_time_setup_srv_deinit,
};