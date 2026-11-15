/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scheduler Server Model Function Implementation
 *
 * History:
 * 2025-5-21, Create file.
 */
#include "include/syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model/scheduler_server.h"

struct bt_mesh_scheduler_srv_t *scheduler_server;

static uint16_t bt_mesh_schedules_fill(struct bt_mesh_scheduler_state_t *state)
{
    uint16_t val = 0;
    int i = 0;

    for (i = 0; i < state->schedule_count; i++) {
        if (state->schedules[i].exist == true) {
            val |= (1 << i);
        }
    }

    return val;
}

static uint64_t bt_mesh_schedule_register_fill(struct bt_mesh_scheduler_state_t *state, uint8_t index)
{
    struct schedule_register_t *reg = &state->schedules[index];
    uint64_t val = 0;

    val = ((uint64_t)(reg->year) << 4) | index; // 4:bit offset
    val |= ((uint64_t)(reg->day) << 23) | ((uint64_t)(reg->month) << 11); // 23:bit offset, 11:bit offset
    val |= ((uint64_t)(reg->minute) << 33) | ((uint64_t)(reg->hour) << 28); // 33:bit offset, 28:bit offset
    val |= ((uint64_t)(reg->day_of_week) << 45) | ((uint64_t)(reg->second) << 39); // 45:bit offset, 39:bit offset
    val |= ((uint64_t)(reg->trans_time) << 56) | ((uint64_t)(reg->action) << 52); // 56:bit offset, 52:bit offset

    return val;
}

static int bt_mesh_srv_scheduler_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scheduler_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;
    uint16_t val = 0;

    msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + SCHEDULE_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCHEDULER_STATUS);
    val = bt_mesh_schedules_fill(srv->state);
    net_buf_simple_add_le16(msg, val);

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send Scheduler Status response\n");
    }
    os_mbuf_free_chain(msg);
    return 0;
}

static int bt_mesh_srv_scheduler_act_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scheduler_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;
    uint8_t index = 0;
    uint64_t value = 0;

    index = net_buf_simple_pull_u8(buf);
    if (index > SCHEDULE_MAX_INDEX) {
        BT_ERR("Invalid Scheduler Register index %u", index);
        return -EINVAL;
    }

    msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + SCHEDULE_ACT_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS);
    value = bt_mesh_schedule_register_fill(srv->state, index);
    net_buf_simple_add_le32(msg, (uint32_t)value);
    net_buf_simple_add_le32(msg, (uint32_t)(value >> 32)); // 32:bit offset
    net_buf_simple_add_le16(msg, srv->state->schedules[index].scene_number);

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send Scheduler Act Status response\n");
    }
    os_mbuf_free_chain(msg);
    return 0;
}

static void bt_mesh_send_scheduler_act_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint8_t index)
{
    struct bt_mesh_scheduler_setup_srv_t *srv = model->user_data;
    struct os_mbuf *msg = NULL;
    uint64_t value = 0;

    msg = NET_BUF_SIMPLE(MODEL_OPCODE_LEN + SCHEDULE_ACT_STATUS_LEN + BLE_MESH_SERVER_TRANS_MIC_SIZE);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS);

    value = bt_mesh_schedule_register_fill(srv->state, index);
    net_buf_simple_add_le32(msg, (uint32_t)value);
    net_buf_simple_add_le32(msg, (uint32_t)(value >> 32)); // 32:bit offset
    net_buf_simple_add_le16(msg, srv->state->schedules[index].scene_number);

    int err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Unable to send Scheduler Act Status response\n");
    }

    os_mbuf_free_chain(msg);
}

void convert_month_bit_mask(uint32_t num, char *mon_buff, uint8_t buff_len)
{
    char *p = mon_buff;
    uint32_t written_len = 0;
    uint32_t total_written_len = 0;

    for (uint32_t loop = 0; loop < 12; loop++) {  // 12:月份
        if ((num & (1 << loop)) != 0) {
            written_len = sprintf_s(p, buff_len - total_written_len, "%02u,", loop + 1);
            if (written_len < 0) {
                break;
            }

            p += written_len;
            total_written_len += written_len;
        }
    }

    if (total_written_len > 0) {
        mon_buff[total_written_len - 1] = '\0';
    }
}

void convert_week_bit_mask(uint32_t num, char *week_buff, uint8_t buff_len)
{
    char *p = week_buff;
    uint32_t written_len = 0;
    uint32_t total_written_len = 0;
    for (uint32_t loop = 0; loop < 7; loop++) {  // 7:星期
        if ((num & (1 << loop)) != 0) {
            written_len = sprintf_s(p, buff_len - total_written_len, "%u,", loop + 1);
            if (written_len < 0) {
                break;
            }

            p += written_len;
            total_written_len += written_len;
        }
    }

    if (total_written_len > 0) {
        week_buff[total_written_len - 1] = '\0';
    }
}

static void bt_mesh_print_scheduler_entry(const struct bt_mesh_scheduler_setup_srv_t *srv, uint8_t index)
{
    char mon_buff[BT_MESH_MON_LEN] = {0};
    char week_buff[BT_MESH_WEEK_LEN] = {0};
    BT_INFO("index:%u", index);
    BT_INFO("scheduler time:");
    BT_INFO("year:%u", srv->state->schedules[index].year + 2000);  // 2000:base year
    convert_month_bit_mask(srv->state->schedules[index].month, mon_buff, BT_MESH_MON_LEN);
    BT_INFO("month:%s", mon_buff);
    BT_INFO("day:%02u", srv->state->schedules[index].day);
    BT_INFO("time:%02u:%02u:%02u",
        srv->state->schedules[index].hour,
        srv->state->schedules[index].minute,
        srv->state->schedules[index].second);
    convert_week_bit_mask(srv->state->schedules[index].day_of_week, week_buff, BT_MESH_WEEK_LEN);
    BT_INFO("day_of_week:%s", week_buff);
    BT_INFO("action:%u", srv->state->schedules[index].action);
    BT_INFO("trans_time:%u", srv->state->schedules[index].trans_time);
    BT_INFO("scene_number:%u", srv->state->schedules[index].scene_number);
}

static int bt_mesh_srv_scheduler_time_parse(uint64_t value, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    uint64_t random_hour;
    *hour = (value >> 28) & BIT_MASK(5);  // 28:bit offset, 5:bit width
    if (*hour > SCHEDULE_HOUR_ONCE_A_DAY) {
        BT_ERR("Invalid Scheduler Register hour %u", *hour);
        return -EINVAL;
    } else if (*hour == SCHEDULE_HOUR_ONCE_A_DAY) {
        bt_rand(&random_hour, sizeof(random_hour));
        *hour = random_hour % 24; /* 24:the number of hours of a day */
    }

    uint64_t random_minute;
    *minute = (value >> 33) & BIT_MASK(6);  // 33:bit offset, 6:bit width
    if (*minute == SCHEDULE_MINUTE_ONCE_A_HOUR) {
        bt_rand(&random_minute, sizeof(random_minute));
        *minute = random_minute % 60; /* 60:the number of minutes of an hour */
    }

    uint64_t random_second;
    *second = (value >> 39) & BIT_MASK(6);  // 39:bit offset, 6:bit width
    if (*second == SCHEDULE_SEC_ONCE_A_MINUTE) {
        bt_rand(&random_second, sizeof(random_second));
        *second = random_second % 60; /* 60:the number of minutes of an hour */
    }

    return BLE_MESH_SUCC;
}

static int bt_mesh_srv_scheduler_act_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scheduler_setup_srv_t *srv = model->user_data;
    uint64_t value = 0;
    uint8_t index = 0, year = 0, hour = 0, minute = 0, second = 0, action = 0;

    value = net_buf_simple_pull_le32(buf);
    value |= ((uint64_t)net_buf_simple_pull_le32(buf) << 32);  // 32:bit offset

    index = value & BIT_MASK(4);  // 4:bit width
    if (index > SCHEDULE_MAX_INDEX) {
        BT_ERR("Invalid Scheduler Register index %u", index);
        return -EINVAL;
    }

    year = (value >> 4) & BIT_MASK(7);  // 4:bit offset, 7:bit width
    if (year > SCHEDULE_ANY_YEAR) {
        BT_ERR("Invalid Scheduler Register year %u", year);
        return -EINVAL;
    }

    if (bt_mesh_srv_scheduler_time_parse(value, &hour, &minute, &second) != BLE_MESH_SUCC) {
        return -EINVAL;
    }

    action = (value >> 52) & BIT_MASK(4);  // 52:bit offset, 4:bit width
    if (action > SCHEDULE_ACT_SCENE_RECALL && action != SCHEDULE_ACT_INACTIVE) {
        BT_ERR("Invalid Scheduler Register action %u", action);
        return -EINVAL;
    }

    srv->state->schedules[index].exist = true;
    srv->state->schedules[index].year = year;
    srv->state->schedules[index].month = (value >> 11) & BIT_MASK(12);  // 11:bit offset, 12:bit width
    srv->state->schedules[index].day = (value >> 23) & BIT_MASK(5);     // 23:bit offset, 5:bit width
    srv->state->schedules[index].hour = hour;
    srv->state->schedules[index].minute = minute;
    srv->state->schedules[index].second = second;
    srv->state->schedules[index].day_of_week = (value >> 45) & BIT_MASK(7);  // 45:bit offset, 7:bit width
    srv->state->schedules[index].action = action;
    srv->state->schedules[index].trans_time = (value >> 56) & BIT_MASK(8);  // 56:bit offset, 8:bit width
    srv->state->schedules[index].scene_number = net_buf_simple_pull_le16(buf);

    bt_mesh_print_scheduler_entry(srv, index);

    if (ctx->recv_op == BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET) {
        bt_mesh_send_scheduler_act_status(model, ctx, index);
    }
    k_work_schedule(&srv->scheduler_transition.timer, K_SECONDS(1));
    return 0;
}

const struct bt_mesh_model_op bt_mesh_scheduler_srv_op[] = {
    {BLE_MESH_MODEL_OP_SCHEDULER_GET, 0, bt_mesh_srv_scheduler_get},
    {BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET, 1, bt_mesh_srv_scheduler_act_get},
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_scheduler_setup_srv_op[] = {
    {BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET, 10, bt_mesh_srv_scheduler_act_set},
    {BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK, 10, bt_mesh_srv_scheduler_act_set},
    BT_MESH_MODEL_OP_END,
};

static int bt_mesh_scheduler_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_scheduler_srv_t *cfg = model->user_data;

    if (!cfg || !cfg->state) {
        BT_ERR("No Scheduler Server context provided");
        return -EINVAL;
    }

    cfg->model = model;

    if (scheduler_server == NULL) {
        scheduler_server = cfg;
    }

    return 0;
}
static int bt_mesh_scheduler_srv_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    return 0;
}
const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb = {
    .init = bt_mesh_scheduler_srv_init,
    .deinit = bt_mesh_scheduler_srv_deinit,
};

static int bt_mesh_scheduler_setup_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_scheduler_setup_srv_t *cfg = model->user_data;
    if (cfg) {
        k_work_cancel_delayable_delete(&(cfg->scheduler_transition.timer));
    }

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }
    return 0;
}

static int bt_mesh_scheduler_setup_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_scheduler_setup_srv_t *cfg = model->user_data;

    if (!cfg || !cfg->state) {
        BT_ERR("No Scheduler Server Setup context provided");
        return -EINVAL;
    }

    cfg->model = model;

    k_work_init_delayable(&(cfg->scheduler_transition.timer), scheduler_act_work_handler);
    k_work_add_arg_delayable(&(cfg->scheduler_transition.timer), cfg);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb = {
    .init = bt_mesh_scheduler_setup_srv_init,
    .deinit = bt_mesh_scheduler_setup_srv_deinit,
};
