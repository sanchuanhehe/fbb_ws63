/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of the State Transition Function
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "model_common.h"
#include "mesh/access.h"
#include "model/generic_server.h"
#include "model/lightness_server.h"
#include "model/light_ctl_server.h"
#include "model/time_server.h"
#include "model/scene_server.h"
#include "model/scheduler_server.h"
#include "state_binding.h"
#include "securec.h"
#include "adapter.h"
#include "atomic_adapter.h"  // tx add

#define DEVICE_SPECIFIC_RESOLUTION 10

#define TRANS_TIME_10S_STEP_RANGE_MAX 620000  // ms
#define TRANS_TIME_1S_STEP_RANGE_MAX 62000    // ms
#define TRANS_TIME_100MS_STEP_RANGE_MAX 6200  // ms

#define TRANS_TIME_10MIN_STEP_DIVISOR 600000
#define TRANS_TIME_10S_STEP_DIVISOR 10000
#define TRANS_TIME_1S_STEP_DIVISOR 1000
#define TRANS_TIME_100MS_STEP_DIVISOR 100

#define TRANS_TIME_10MIN_STEP_RESOLUTION 3
#define TRANS_TIME_10S_STEP_RESOLUTION 2
#define TRANS_TIME_1S_STEP_RESOLUTION 1
#define TRANS_TIME_100MS_STEP_RESOLUTION 0

#define TRANS_NUMBER_OF_STEP 6

#define TRANS_TIME_PERIOD 100  // ms
#define DELAY_TIME_EXTEND 5

#define BLE_MESH_STATE_ON 1

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

void calculate_rt(struct transition *transition)
{
    if (transition->just_started) {
        transition->remain_time = transition->trans_time;
    } else {
        uint8_t steps, resolution;

        int64_t now = k_uptime_get();
        int32_t duration_remainder = transition->total_duration - (now - transition->start_timestamp);

        if (duration_remainder > TRANS_TIME_10S_STEP_RANGE_MAX) {
            /* > 620 seconds -> resolution = 0b11 [10 minutes] */
            resolution = TRANS_TIME_10MIN_STEP_RESOLUTION;
            steps = duration_remainder / TRANS_TIME_10MIN_STEP_DIVISOR;
        } else if (duration_remainder > TRANS_TIME_1S_STEP_RANGE_MAX) {
            /* > 62 seconds -> resolution = 0b10 [10 seconds] */
            resolution = TRANS_TIME_10S_STEP_RESOLUTION;
            steps = duration_remainder / TRANS_TIME_10S_STEP_DIVISOR;
        } else if (duration_remainder > TRANS_TIME_100MS_STEP_RANGE_MAX) {
            /* > 6.2 seconds -> resolution = 0b01 [1 seconds] */
            resolution = TRANS_TIME_1S_STEP_RESOLUTION;
            steps = duration_remainder / TRANS_TIME_1S_STEP_DIVISOR;
        } else if (duration_remainder > 0) {
            /* <= 6.2 seconds -> resolution = 0b00 [100 ms] */
            resolution = TRANS_TIME_100MS_STEP_RESOLUTION;
            steps = duration_remainder / TRANS_TIME_100MS_STEP_DIVISOR;
        } else {
            resolution = 0x00;
            steps = 0x00;
        }

        transition->remain_time = (resolution << TRANS_NUMBER_OF_STEP) | steps;
    }
}

void tt_values_calculator(struct transition *transition)
{
    uint8_t steps_multiplier, resolution;

    resolution = (transition->trans_time >> TRANS_NUMBER_OF_STEP);
    steps_multiplier = (transition->trans_time & 0x3F);

    switch (resolution) {
        case TRANS_TIME_100MS_STEP_RESOLUTION: /* 100ms */
            transition->total_duration = steps_multiplier * TRANS_TIME_100MS_STEP_DIVISOR;
            break;
        case TRANS_TIME_1S_STEP_RESOLUTION: /* 1 second */
            transition->total_duration = steps_multiplier * TRANS_TIME_1S_STEP_DIVISOR;
            break;
        case TRANS_TIME_10S_STEP_RESOLUTION: /* 10 seconds */
            transition->total_duration = steps_multiplier * TRANS_TIME_10S_STEP_DIVISOR;
            break;
        case TRANS_TIME_10MIN_STEP_RESOLUTION: /* 10 minutes */
            transition->total_duration = steps_multiplier * TRANS_TIME_10MIN_STEP_DIVISOR;
            break;
        default:
            BT_ERR("invalid resolution %u", resolution);
            return;
    }

    transition->counter = ((float)transition->total_duration / TRANS_TIME_PERIOD);

    if (transition->counter > DEVICE_SPECIFIC_RESOLUTION) {
        transition->counter = DEVICE_SPECIFIC_RESOLUTION;
    }
}

void bt_mesh_server_start_transition(struct transition *transition)
{
    BT_DBG("Begin to transition");
    if (transition->delay) {
        k_work_schedule(&transition->timer, K_MSEC(DELAY_TIME_EXTEND * transition->delay));
        atomic_set_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
    } else {
        k_work_submit(&transition->timer.work);
    }
}

void bt_mesh_server_stop_transition(struct transition *transition)
{
    if (memset_s(transition, sizeof(struct transition), 0x0, offsetof(struct transition, flag)) != EOK) {
        BT_ERR("memset_s failed");
        return;
    }

    if (atomic_test_and_clear_bit(transition->flag, BLE_MESH_TRANS_TIMER_START)) {
        k_work_cancel_delayable(&transition->timer);
    }
}

static void bt_mesh_statetransition_timer_start(struct transition *transition)
{
    transition->start_timestamp = k_uptime_get();
    k_work_schedule_periodic(&transition->timer, K_MSEC(transition->quo_tt));
    atomic_set_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
}

static void bt_mesh_state_transition_timer_stop(struct transition *transition)
{
    k_work_cancel_delayable(&transition->timer);
    atomic_clear_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
}

void onoff_tt_values(struct bt_mesh_gen_onoff_srv *srv)
{
    if ((srv->transition.trans_time & 0x3F) == 0) {
        return;
    }
    tt_values_calculator(&srv->transition);
    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;  // period
}

void level_tt_values(struct bt_mesh_gen_level_srv *srv)
{
    if ((srv->transition.trans_time & 0x3F) == 0) {
        return;
    }
    tt_values_calculator(&srv->transition);
    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;  // period
    srv->tt_delta = ((float)(srv->level_state.target_level - srv->level_state.current_level) / srv->transition.counter);
}

void power_level_tt_values(struct bt_mesh_gen_power_level_srv *srv)
{
    if ((srv->transition.trans_time & 0x3F) == 0) {
        return;
    }
    tt_values_calculator(&srv->transition);
    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;  // period
    srv->tt_delta =
        ((float)(srv->pwr_level_state->target_level - srv->pwr_level_state->current_level) / srv->transition.counter);
}

void scene_tt_values(struct bt_mesh_scene_srv *srv, uint8_t trans_time, uint8_t delay)
{
    if ((srv->transition.trans_time & 0x3F) == 0) {
        BT_ERR("transition.trans_time & 0x3F) == 0");
        return;
    }

    if (trans_time == 0U) {
        BT_ERR("trans_time == 0");
        return;
    }

    tt_values_calculator(&srv->transition);
    BT_INFO("transition.counter is %d", srv->transition.counter);
    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;
    unused(delay);
}

void light_ctl_tt_values(ble_mesh_light_ctl_srv_t *srv, uint8_t tt, uint8_t delay)
{
    srv->transition.trans_time = tt;
    srv->transition.delay = delay;

    if ((tt & 0x3F) != 0) {
        tt_values_calculator(&srv->transition);
    } else {
        return;
    }

    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;

    srv->tt_delta_lightness = ((float)(srv->state->lightness - srv->state->target_lightness) / srv->transition.counter);

    srv->tt_delta_temp = ((float)(srv->state->temp - srv->state->target_temp) / srv->transition.counter);

    srv->tt_delta_duv = ((float)(srv->state->delta_uv - srv->state->target_delta_uv) / srv->transition.counter);
}

void light_ctl_temp_tt_values(ble_mesh_light_ctl_temp_srv_t *srv, uint8_t tt, uint8_t delay)
{
    srv->transition.trans_time = tt;
    srv->transition.delay = delay;

    if ((tt & 0x3F) != 0) {
        tt_values_calculator(&srv->transition);
    } else {
        return;
    }

    srv->transition.quo_tt = srv->transition.total_duration / srv->transition.counter;

    srv->tt_delta_temp = ((float)(srv->state->temp - srv->state->target_temp) / srv->transition.counter);

    srv->tt_delta_duv = ((float)(srv->state->delta_uv - srv->state->target_delta_uv) / srv->transition.counter);
}

void lightness_tt_values(struct bt_mesh_light_lightness_srv *srv)
{
    if ((srv->actual_transition.trans_time & 0x3F) == 0) {
        return;
    }
    tt_values_calculator(&srv->actual_transition);
    srv->actual_transition.quo_tt = srv->actual_transition.total_duration / srv->actual_transition.counter;  // period
    srv->tt_delta_actual =
        ((float)(srv->light_state->target_actual - srv->light_state->actual) / srv->actual_transition.counter);
}

void lightness_linear_tt_values(struct bt_mesh_light_lightness_srv *srv)
{
    if ((srv->linear_transition.trans_time & 0x3F) == 0) {
        return;
    }
    tt_values_calculator(&srv->linear_transition);
    srv->linear_transition.quo_tt = srv->linear_transition.total_duration / srv->linear_transition.counter;  // period
    srv->tt_delta_linear =
        ((float)(srv->light_state->target_linear - srv->light_state->linear) / srv->linear_transition.counter);
}

static void set_onoff_status(struct bt_mesh_gen_onoff_srv *srv)
{
    gen_onoff_bind_state(srv->onoff, srv->model);
    if (srv->set) {
        srv->set(srv->model, srv->onoff);
    }
}

void generic_onoff_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_gen_onoff_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL) {
        BT_ERR("generic onoff srv from work is null");
        return;
    }
    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0) {
            srv->onoff = srv->target_onoff;
            set_onoff_status(srv);
            atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            if (srv->target_onoff == BLE_MESH_STATE_ON) {
                srv->onoff = BLE_MESH_STATE_ON;
                set_onoff_status(srv);
            }
            bt_mesh_statetransition_timer_start(&srv->transition);
        }
        return;
    }

    srv->transition.counter--;

    if (srv->transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        srv->onoff = srv->target_onoff;
        if (srv->target_onoff != BLE_MESH_STATE_ON) {
            set_onoff_status(srv);
        }
    }

    return;
}

void generic_level_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_gen_level_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL) {
        BT_ERR("generic level srv from work is null");
        return;
    }
    struct generic_level_state *state = &srv->level_state;
    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0) {
            state->current_level = state->target_level;
            gen_level_bind_state(state->current_level, srv->model);
            if (srv->set) {
                srv->set(srv->model, state);
            }
            atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            bt_mesh_statetransition_timer_start(&srv->transition);
        }
        return;
    }

    srv->transition.counter--;
    state->current_level += srv->tt_delta;

    if (srv->transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        state->current_level = state->target_level;
    }

    gen_level_bind_state(state->current_level, srv->model);
    if (srv->set) {
        srv->set(srv->model, state);
    }

    return;
}

void generic_power_level_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_gen_power_level_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL || srv->pwr_level_state == NULL) {
        BT_ERR("generic power srv from work is null");
        return;
    }
    struct generic_power_level_state *state = srv->pwr_level_state;
    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0) {
            state->current_level = state->target_level;
            gen_power_level_bind_state(state->current_level, srv->model);
            if (srv->set) {
                srv->set(srv->model, state);
            }
            atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            bt_mesh_statetransition_timer_start(&srv->transition);
        }
        return;
    }

    srv->transition.counter--;
    state->current_level += srv->tt_delta;

    if (srv->transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        state->current_level = state->target_level;
    }

    gen_power_level_bind_state(state->current_level, srv->model);
    if (srv->set) {
        srv->set(srv->model, state);
    }

    return;
}

static void scene_recall(struct bt_mesh_scene_srv *srv)
{
    srv->state->in_progress = false;
    srv->state->target_scene = INVALID_SCENE_NUMBER;
    if (srv->set) {
        srv->set(srv->model, srv->model->op->opcode, (const uint8_t *)srv->state);  // 回调app
    }
}

void scene_recall_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_scene_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL) {
        BT_ERR("scene srv from work is null");
        return;
    }

    BT_INFO("transition.counter:%u ", srv->transition.counter);

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            scene_recall(srv);
            atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            bt_mesh_statetransition_timer_start(&srv->transition);
        }
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
    }

    if (srv->transition.counter == 0U) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        srv->state->current_scene = srv->state->target_scene;
        scene_recall(srv);
    }

    return;
}

void lightness_actual_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_light_lightness_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL || srv->light_state == NULL) {
        BT_ERR("lightness actual srv from work is null");
        return;
    }
    struct light_lightness_state *state = srv->light_state;
    if (srv->actual_transition.just_started) {
        srv->actual_transition.just_started = false;
        if (srv->actual_transition.counter == 0) {
            state->actual = state->target_actual;
            lightness_actual_bind_state(state->actual, srv->model);
            if (srv->set) {
                srv->set(srv->model, state);
            }
            atomic_clear_bit(srv->actual_transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            bt_mesh_statetransition_timer_start(&srv->actual_transition);
        }
        return;
    }

    srv->actual_transition.counter--;
    state->actual += srv->tt_delta_actual;

    if (srv->actual_transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->actual_transition);
        state->actual = state->target_actual;
    }

    lightness_actual_bind_state(state->actual, srv->model);
    if (srv->set) {
        srv->set(srv->model, state);
    }

    return;
}

void lightness_linear_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_light_lightness_srv *srv = ble_npl_event_get_arg(work);
    if (srv == NULL || srv->light_state == NULL) {
        BT_ERR("lightness linear srv from work is null");
        return;
    }
    struct light_lightness_state *state = srv->light_state;
    if (srv->linear_transition.just_started) {
        srv->linear_transition.just_started = false;
        if (srv->linear_transition.counter == 0) {
            state->linear = state->target_linear;
            lightness_linear_bind_state(state->linear, srv->model);
            if (srv->set) {
                srv->set(srv->model, state);
            }
            atomic_clear_bit(srv->linear_transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            bt_mesh_statetransition_timer_start(&srv->linear_transition);
        }
        return;
    }

    srv->linear_transition.counter--;
    state->linear += srv->tt_delta_linear;

    if (srv->linear_transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->linear_transition);
        state->linear = state->target_linear;
    }

    lightness_linear_bind_state(state->linear, srv->model);
    if (srv->set) {
        srv->set(srv->model, state);
    }

    return;
}

void light_ctl_work_handler(struct ble_npl_event *work)
{
    ble_mesh_light_ctl_srv_t *srv = ble_npl_event_get_arg(work);

    if (srv == NULL || srv->state == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            light_ctl_light_update_state_binding(srv->state->lightness);
            light_ctl_temp_update_state_binding(srv->state->temp);
            if (srv->set) {
                srv->set(srv->model, srv->state);
            }
        } else {
            bt_mesh_statetransition_timer_start(&srv->transition);
        }

        return;
    }

    srv->state->lightness -= srv->tt_delta_lightness;
    srv->state->temp -= srv->tt_delta_temp;
    srv->state->delta_uv -= srv->tt_delta_duv;

    srv->transition.counter--;

    if (srv->transition.counter == 0U) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        srv->state->lightness = srv->state->target_lightness;
        srv->state->temp = srv->state->target_temp;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    light_ctl_light_update_state_binding(srv->state->lightness);
    light_ctl_temp_update_state_binding(srv->state->temp);

    if (srv->set) {
        srv->set(srv->model, srv->state);
    }

    return;
}

void light_ctl_temp_work_handler(struct ble_npl_event *work)
{
    ble_mesh_light_ctl_temp_srv_t *srv = ble_npl_event_get_arg(work);
    if (srv == NULL) {
        BT_ERR("light ctl temp srv from work is null");
        return;
    }

    if (srv->transition.just_started) {
        srv->transition.just_started = false;

        if (srv->transition.counter == 0) {
            light_ctl_temp_update_state_binding(srv->state->temp);
            if (srv->set) {
                srv->set(srv->model, srv->state);
            }
        } else {
            bt_mesh_statetransition_timer_start(&srv->transition);
        }

        return;
    }

    /* Temperature */
    srv->state->temp -= srv->tt_delta_temp;
    /* Delta UV */
    srv->state->delta_uv -= srv->tt_delta_duv;

    srv->transition.counter--;

    if (srv->transition.counter == 0) {
        bt_mesh_state_transition_timer_stop(&srv->transition);
        srv->state->temp = srv->state->target_temp;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    light_ctl_temp_update_state_binding(srv->state->temp);

    if (srv->set) {
        srv->set(srv->model, srv->state);
    }
}

void time_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_time_setup_srv_t *srv = ble_npl_event_get_arg(work);
    if (srv == NULL) {
        BT_ERR("time srv from work is null");
        return;
    }

    srv->send_publish = false;
}

void scheduler_act_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_scheduler_setup_srv_t *srv = ble_npl_event_get_arg(work);

    if (srv == NULL) {
        return;
    }
    if (srv->set != NULL) {
        srv->set(srv->model, srv->state);
    }
    k_work_schedule(&srv->scheduler_transition.timer, K_SECONDS(1));
}
