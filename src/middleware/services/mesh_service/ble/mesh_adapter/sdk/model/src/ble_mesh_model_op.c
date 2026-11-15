/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh model.
 *
 * History:
 * 2025-4-15, Create file.
 */
#include "generic_client.h"
#include "mesh/access.h"
#include "mesh/cfg_srv.h"
#include "health_client.h"
#include "model_opcode.h"
#include "mesh/main.h"
#include "mesh/health_srv.h"
#include "common_def.h"
#include "lightness_server.h"
#include "time_server.h"
#include "scene_server.h"
#include "scheduler_server.h"
#include "ble_mesh_model.h"
#include "ble_mesh_model_op.h"

#define FAULT_ARR_SIZE 2
static bool has_reg_fault = false;
#define BLE_MESH_CID 0x010F
#define STANDARD_TEST_ID 0x00
#define TEST_ID 0x01
#define SMALL_MAX_DAYS 30           // 小月最大天数（4/6/9/11月）
#define COMM_YEARS_FEB_MAX_DAYS 28  // 平年二月最大天数
#define LEAP_YEARS_FEB_MAX_DAYS 29  // 闰年二月最大天数

static int recent_test_id = STANDARD_TEST_ID;

// health检测相关操作
static void show_faults(uint8_t test_id, uint16_t cid, uint8_t *faults, size_t fault_count)
{
    size_t i;

    if (!fault_count) {
        BT_INFO("Health Test ID 0x%02x Company ID 0x%04x: no faults", test_id, cid);
        return;
    }

    BT_INFO("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:", test_id, cid, fault_count);

    for (i = 0; i < fault_count; i++) {
        BT_INFO("\t0x%02x", faults[i]);
    }
}

void ble_mesh_health_current_status(struct bt_mesh_health_cli *cli, uint16_t addr, uint8_t test_id, uint16_t cid,
    uint8_t *faults, size_t fault_count)
{
    BT_INFO("Health Current Status from 0x%04x", addr);
    show_faults(test_id, cid, faults, fault_count);
}


// gen onoff server 操作
int ble_mesh_gen_onoff_set_state(struct bt_mesh_model *model, uint8_t state)
{
    BT_INFO("set_state:%d\r\n", state);
#if !defined(CONFIG_BLE_MESH_PROVISIONER_ADV) && defined(CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
    set_led_state(state);
#endif
    return 0;
}

int set_by_lightness_state(struct bt_mesh_model *model, struct light_lightness_state *state)
{
    BT_DBG("SET_LIGHTNESS current_linear=%u, target_linear=%u, current_actual=%u, target_actual=%u",
        state->linear, state->target_linear, state->actual, state->target_actual);
    BT_DBG("SET_LIGHTNESS user operation");
#if !defined(CONFIG_BLE_MESH_PROVISIONER_ADV) && defined(CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
    set_led_light(state->target_actual);
#endif
    return 0;
}

int set_by_lightness_setup_state(struct bt_mesh_model *model, struct light_lightness_state *state)
{
    BT_INFO("SET_LIGHTNESS_SETUP", "def_lightness=%u, light_range_min=%u, light_range_max=%u",
        state->def, state->light_range_min, state->light_range_max);
    BT_INFO("SET_LIGHTNESS_SETUP", "user operation");
    return 0;
}

int ble_mesh_light_ctl_set_cb(struct bt_mesh_model *model, ble_mesh_light_ctl_state *ctl_state)
{
    if (ctl_state == NULL) {
        BT_INFO("param is NULL!");
        return -1;
    }
    BT_INFO("lightness %u", ctl_state->lightness);
    BT_INFO("target_lightness %u", ctl_state->target_lightness);
    BT_INFO("temp %u", ctl_state->temp);
    BT_INFO("target_temp %u", ctl_state->target_temp);
    BT_INFO("delta_uv %d", ctl_state->delta_uv);
    BT_INFO("target_delta_uv %d", ctl_state->target_delta_uv);
    BT_INFO("status_code %u", ctl_state->status_code);
    BT_INFO("temp_range_min %u", ctl_state->temp_range_min);
    BT_INFO("temp_range_max %u", ctl_state->temp_range_max);
    BT_INFO("lightness_def %u", ctl_state->lightness_def);
    BT_INFO("temp_def %u", ctl_state->temp_def);
    BT_INFO("delta_uv_def %u", ctl_state->delta_uv_def);
#if !defined(CONFIG_BLE_MESH_PROVISIONER_ADV) && defined(CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
    set_led_light(ctl_state->target_lightness);
#endif
    return 0;
}

int set_by_level_state(struct bt_mesh_model *model, struct generic_level_state *state)
{
    if (!state) {
        BT_ERR("state is NULL")
        return -EINVAL;
    }
    BT_DBG("current_level=%d, target_level=%d", state->current_level, state->target_level);
#if !defined(CONFIG_BLE_MESH_PROVISIONER_ADV) && defined(CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
    set_led_level(state->target_level);
#endif
    return 0;
}

int ble_mesh_current_fault_get(struct bt_mesh_model *model, uint8_t *test_id, uint16_t *company_id,
    uint8_t *faults, uint8_t *fault_count)
{
    uint8_t reg_faults[FAULT_ARR_SIZE] = {[0 ... FAULT_ARR_SIZE - 1] = 0xff};

    BT_INFO("has_reg_fault %u,fault_count is %d", has_reg_fault, *fault_count);

    *test_id = recent_test_id;
    *company_id = BLE_MESH_CID;

    if (has_reg_fault) {
        *fault_count = min(*fault_count, sizeof(reg_faults));
        BT_DBG("has_reg_fault %u,fault_count is %d", has_reg_fault, *fault_count);
        if (memcpy_s(faults, *fault_count, reg_faults, *fault_count) != EOK) {
            BT_ERR("memcpy_s failed");
            return -1;
        }
    } else {
        *fault_count = 0;  // 0: count
    }

    return 0;
}

int ble_mesh_registered_fault_get(
    struct bt_mesh_model *model, uint16_t company_id, uint8_t *test_id, uint8_t *faults, uint8_t *fault_count)
{
    if (company_id != BLE_MESH_CID) {
        return -EINVAL;
    }

    BT_INFO("fault_get_reg() has_reg_fault %u", has_reg_fault);

    *test_id = recent_test_id;

    if (has_reg_fault) {
        uint8_t reg_faults[FAULT_ARR_SIZE] = {[0 ... FAULT_ARR_SIZE - 1] = 0xff};

        *fault_count = min(*fault_count, sizeof(reg_faults));
        BT_INFO("fault_count is  %d", *fault_count);
        if (memcpy_s(faults, *fault_count, reg_faults, *fault_count) != EOK) {
            BT_INFO("memcpy_s failed");
            return -1;
        }
    } else {
        *fault_count = 0;
    }

    return 0;
}

int ble_mesh_fault_clear(struct bt_mesh_model *model, uint16_t company_id)
{
    if (company_id != BLE_MESH_CID) {
        return -EINVAL;
    }

    has_reg_fault = false;
    BT_INFO("set has_reg_fault to %d", has_reg_fault);

    return 0;
}

int ble_mesh_fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id)
{
    BT_INFO("");
    if (company_id != BLE_MESH_CID) {
        BT_INFO("company_id[%d] != CID_HIS[[%d]] ,return", company_id, BLE_MESH_CID);
        return -EINVAL;
    }

    if (test_id != STANDARD_TEST_ID && test_id != TEST_ID) {
        BT_INFO("test_id[[%d]] != STANDARD_TEST_ID[%d] && test_id != TEST_ID[%d] ,return",
            test_id,
            STANDARD_TEST_ID,
            TEST_ID);
        return -EINVAL;
    }

    recent_test_id = test_id;
    has_reg_fault = true;
    BT_INFO("recent_test_id is %d", recent_test_id);
    if (model == NULL) {
        BT_INFO("model is null.");
        return -EINVAL;
    }
    bt_mesh_fault_update(bt_mesh_model_elem(model));
    return 0;
}

void ble_mesh_attention_on(struct bt_mesh_model *model)
{
    BT_INFO("health_srv_cb:attn_on");
    return;
}

void ble_mesh_attention_off(struct bt_mesh_model *model)
{
    BT_INFO("health_srv_cb:attn_off");
    return;
}

int ble_mesh_dtt_set_cb(struct bt_mesh_model *model, uint8_t state)
{
    BT_INFO("DTT dtt=%u", state);
    BT_INFO("DTT user operation");
    return 0;
}

int set_scene(struct bt_mesh_model *model, uint16_t scene_number)
{
    BT_ERR("set_scene = %u", scene_number);
    return 0;
}

int ble_mesh_scene_set_cb(struct bt_mesh_model *model, uint32_t opcode, const uint8_t *state)
{
    if (state == NULL) {
        BT_ERR("state is NULL")
        return -EINVAL;
    }
    struct bt_mesh_scenes_state *scene_state = (struct bt_mesh_scenes_state *)state;
    
    return set_scene(model, scene_state->current_scene);
}

bool scheduler_act_month_check(uint64_t sch_month, int month)
{
    if (month < 1 || month > 12) { /* 12:the number of month of a year */
        return false;
    }

    return (sch_month >> (month - 1)) & 0x1;
}

bool scheduler_act_week_check(uint64_t sch_week, int week)
{
    if (week < 1 || week > 7) { /* 7:the number of day of a week */
        return false;
    }

    return (sch_week >> (week - 1)) & 0x1;
}

bool scheduler_act_hour_check(uint64_t sch_hour, int hour)
{
    if (sch_hour == SCHEDULE_ANY_HOUR) {
        return true;
    }
    if (sch_hour != SCHEDULE_HOUR_ONCE_A_DAY) {
        return (int)sch_hour == hour;
    }
    return false;
}

bool scheduler_act_minute_check(uint64_t sch_min, int min)
{
    if (sch_min == SCHEDULE_ANY_MINUTE) {
        return true;
    }
    if (sch_min < SCHEDULE_ANY_MINUTE) {
        return (int)sch_min == min;
    }
    if (sch_min == SCHEDULE_MINUTE_15) {
        return (min % 15) == 0; /* Check if the current minutes is a multiple of 15 */
    }
    if (sch_min == SCHEDULE_MINUTE_20) {
        return (min % 20) == 0; /* Check if the current minutes is a multiple of 20 */
    }
    return false;
}

bool scheduler_act_second_check(uint64_t sch_sec, int sec)
{
    if (sch_sec == SCHEDULE_ANY_SEC) {
        return true;
    }
    if (sch_sec < SCHEDULE_ANY_SEC) {
        return (int)sch_sec == sec;
    }
    if (sch_sec == SCHEDULE_SEC_15) {
        return (sec % 15) == 0; /* Check if the current seconds is a multiple of 15 */
    }
    if (sch_sec == SCHEDULE_SEC_20) {
        return (sec % 20) == 0; /* Check if the current seconds is a multiple of 20 */
    }
    return false;
}

void ble_mesh_scheduler_action(struct bt_mesh_model *model, struct schedule_register_t *schedules)
{
    if (schedules == NULL) {
        BT_ERR("schedulers is NULL")
        return;
    }
    if (schedules->action == SCHEDULE_ACT_ON) {
        BT_ERR("scheduler act on");
    } else if (schedules->action == SCHEDULE_ACT_OFF) {
        BT_ERR("scheduler act off");
    } else if (schedules->action == SCHEDULE_ACT_SCENE_RECALL) {
        set_scene(model, schedules->scene_number);
    } else {
        BT_ERR("action = %u", schedules->action);
    }
}

// 判断是否为闰年
static bool ble_mesh_find_leap_year(int year)
{
    if ((year % 400 == 0) ||                   // 400:能被400整除是闰年
        (year % 4 == 0 && year % 100 != 0)) {  // 4、100:能被4整除且不能被100整除则为闰年
        return true;
    }

    return false;
}

// 检查当前月份天数是否超过最大值（超过则按最大值处理）
static void scheduler_act_day_check(int year, int mon, int *day)
{
    if (mon == 4 || mon == 6 || mon == 9 || mon == 11) {  // 4/6/9/11:天数为30的月份
        *day = *day > SMALL_MAX_DAYS ? SMALL_MAX_DAYS : *day;
    } else if (mon == 2) {  // 2:二月
        if (ble_mesh_find_leap_year(year)) {
            *day = *day > LEAP_YEARS_FEB_MAX_DAYS ? LEAP_YEARS_FEB_MAX_DAYS : *day;
        } else {
            *day = *day > COMM_YEARS_FEB_MAX_DAYS ? COMM_YEARS_FEB_MAX_DAYS : *day;
        }
    }
}

int ble_mesh_scheduler_set_cb(struct bt_mesh_model *model, struct bt_mesh_scheduler_state_t *scheduler_state)
{
    if (scheduler_state == NULL) {
        return -EINVAL;
    }
    uint32_t npl_time_sec = 0;
    struct tm *utc_time;
    time_t utc_sec = 0;

    npl_time_sec = ble_npl_time_ticks_to_ms32(ble_npl_time_get());
    utc_sec = local_utc_time.delta + (npl_time_sec / MSEC_PER_SEC);
    utc_time = gmtime(&utc_sec);
    if (utc_time == NULL) {
        return -EINVAL;
    }
    for (int i = 0; i < scheduler_state->schedule_count; i++) {
        if (scheduler_state->schedules[i].exist == false) {
            continue;
        }
        if (scheduler_state->schedules[i].year != SCHEDULE_ANY_YEAR) {
            if (scheduler_state->schedules[i].year != (utc_time->tm_year + BASE_YEAR) - DELAY_YEAR) {
                continue;
            }
        }
        if (scheduler_act_month_check(scheduler_state->schedules[i].month, utc_time->tm_mon + 1) != true) {
            continue;
        }
        if (scheduler_act_week_check(scheduler_state->schedules[i].day_of_week, utc_time->tm_wday) != true) {
            continue;
        }
        if (scheduler_state->schedules[i].day != SCHEDULE_ANY_DAY) {
            int day = scheduler_state->schedules[i].day;
            scheduler_act_day_check(utc_time->tm_year + BASE_YEAR, utc_time->tm_mon + 1, &day);
            scheduler_state->schedules[i].day = day;
            if (scheduler_state->schedules[i].day != utc_time->tm_mday) {
                continue;
            }
        }
        if (scheduler_act_hour_check(scheduler_state->schedules[i].hour, utc_time->tm_hour) != true) {
            continue;
        }
        if (scheduler_act_minute_check(scheduler_state->schedules[i].minute, utc_time->tm_min) != true) {
            continue;
        }
        if (scheduler_act_second_check(scheduler_state->schedules[i].second, utc_time->tm_sec) != true) {
            continue;
        }
        ble_mesh_scheduler_action(model, &scheduler_state->schedules[i]);
    }

    return 0;
}
