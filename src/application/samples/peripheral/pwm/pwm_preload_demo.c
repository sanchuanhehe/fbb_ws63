/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: PWM Sample Source. \n
 *
 * History: \n
 * 2026-02-24, Create file. \n
 */
#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"

#define PWM_TASK_PRIO              24
#define PWM_TASK_STACK_SIZE        0x1000
#define TEST_TCXO_DELAY_10MS       10
#define PWM_TIME_100               100
#define PWM_TIME_200               200
#define PWM_CYCLE_16               16
#define PWM_CFG_CHANGE_COUNT       3

static uint32_t g_pwm_time_log[PWM_CFG_CHANGE_COUNT + 1] = {0};
static uint32_t g_pwm_cfg_time = PWM_TIME_100;
static bool g_pwm_stop = false;
static uint32_t g_pwm_cyc_done_cnt = 0;

static errcode_t pwm_sample_callback(uint8_t channel)
{
    unused(channel);

    pwm_config_t pwm_cfg = {
        PWM_TIME_100,
        PWM_TIME_100,
        0,
        PWM_CYCLE_16,
        false
    };

    if (g_pwm_cyc_done_cnt <= PWM_CFG_CHANGE_COUNT) {
        g_pwm_time_log[g_pwm_cyc_done_cnt] = g_pwm_cfg_time;

        if (g_pwm_cyc_done_cnt < PWM_CFG_CHANGE_COUNT) {
            g_pwm_cfg_time = ((g_pwm_cfg_time == PWM_TIME_100) ? PWM_TIME_200 : PWM_TIME_100);
            pwm_cfg.low_time = g_pwm_cfg_time;
            pwm_cfg.high_time = g_pwm_cfg_time;
        } else {
            pwm_cfg.high_time = 0;
        }

        uapi_pwm_config_preload(CONFIG_PWM_GROUP_ID, CONFIG_PWM_CHANNEL, &pwm_cfg);
    } else {
        uapi_pwm_stop_group(CONFIG_PWM_GROUP_ID);
        g_pwm_stop = true;
    }

    g_pwm_cyc_done_cnt++;
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {
        PWM_TIME_100,
        PWM_TIME_100,
        0,
        PWM_CYCLE_16,
        false
    };

    uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    uapi_pwm_init();
    uapi_pwm_open(CONFIG_PWM_CHANNEL, &cfg_no_repeat);
    uapi_pwm_register_interrupt(CONFIG_PWM_CHANNEL, pwm_sample_callback);

    uint8_t channel_id = CONFIG_PWM_CHANNEL;
    /* channel_id can also choose to configure multiple channels, and the third parameter also needs to be adjusted
        accordingly. */
    uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
    /* Here you can also call the uapi_pwm_start interface to open each channel individually. */
    uapi_pwm_start_group(CONFIG_PWM_GROUP_ID);

    while (1) {
        if (g_pwm_stop) {
            break;
        }
        uapi_tcxo_delay_ms(TEST_TCXO_DELAY_10MS);
    }

    for (uint32_t i = 0; i < (PWM_CFG_CHANGE_COUNT + 1); i++) {
        osal_printk("pwm cyc_done_cnt:%u, duty_radio:[%u/%u]\r\n", i,
            g_pwm_time_log[i], g_pwm_time_log[i] + g_pwm_time_log[i]);
    }

    uapi_pwm_close(CONFIG_PWM_CHANNEL);
    uapi_pwm_deinit();

    return NULL;
}

static void pwm_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)pwm_task, 0, "PwmTask", PWM_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, PWM_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(pwm_entry);