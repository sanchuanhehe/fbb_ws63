/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BREATHING_LIGHT Sample Source. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */
#if defined(CONFIG_PWM_SUPPORT_LPM)
#include "pm_veto.h"
#endif
#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"
#include "math.h"

// 定义 PI 值
#define PI 3.1415926
#define BREATHING_LIGHT_START_VALUE_MAX 0.98f /* 呼吸灯亮度最大值(禁止为0) */
#define BREATHING_LIGHT_START_VALUE_MIN 0.02f /* 呼吸灯亮度最小值(禁止为0) */
#define BREATHING_LIGHT_STEP_SIZE 0.01f       /* 亮度递减步长 */
#define BREATHING_LIGHT_MIN_VALUE 0.0f        /* 亮度最小值   */
#define BREATHING_LIGHT_MAX_VALUE 1.0f        /* 亮度最最值   */
#define BREATHING_LIGHT_SPEED 3               /* 变化速度   */
#define TEST_TCXO_DELAY_1000MS 1000
#define BREATHING_LIGHT_TASK_PRIO 24
#define BREATHING_LIGHT_TASK_STACK_SIZE 0x1000

// PWM周期 = g_PwmCycle*0.0125;eg.60000*0.0125=1000us 即频率1K

uint16_t g_PwmCycle = 60000;

uint16_t CalculateCurveValue(float counter)
{
    /* counter 是归一化时间（0 到 1）使用 sin 函数生成 S 形曲线，范围从 -1 到 1，并调整为 0 到 1
     公式：0.5 * (1 + sin(π * (counter - 0.5)))
     目的：使得显示效果更佳的平滑*/

    float curve_value = 0.5f * (1.0f + sinf(PI * (counter - 0.5f)));

    // 将 S 形曲线值映射到占空比范围 0~60000，并四舍五入为整数
    return (uint16_t)(curve_value * g_PwmCycle + 0.5f);
}

static void *breathing_task(const char *arg)
{
    UNUSED(arg);

    uint16_t lowLevelTime = 7991;
    float counter = 0;
    pwm_config_t cfg_repeat = {lowLevelTime, g_PwmCycle - lowLevelTime, 0, 100, true};
#ifdef CONFIG_PWM_USING_V151
    uint8_t channelId = CONFIG_BREATHING_LIGHT_CHANNEL;
#endif

    // 指定管脚 若修改则需查询（GPIO复用管脚）修改复用模式和PWM通道

    uapi_pin_set_mode(CONFIG_BREATHING_LIGHT_PIN, CONFIG_BREATHING_LIGHT_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();

    osal_printk("breathing light start.\r\n");
    while (1) {
        for (counter = BREATHING_LIGHT_START_VALUE_MAX; counter > BREATHING_LIGHT_MIN_VALUE;
             counter -= BREATHING_LIGHT_STEP_SIZE) {
            lowLevelTime = CalculateCurveValue(counter);
            cfg_repeat.low_time = lowLevelTime;
            cfg_repeat.high_time = g_PwmCycle - lowLevelTime;
            uapi_pwm_open(CONFIG_BREATHING_LIGHT_CHANNEL, &cfg_repeat);
#ifdef CONFIG_PWM_USING_V151
            uapi_pwm_set_group(CONFIG_BREATHING_LIGHT_GROUP_ID, &channelId, 1);
            uapi_pwm_start_group(CONFIG_BREATHING_LIGHT_GROUP_ID);
#else
            uapi_pwm_start(CONFIG_BREATHING_LIGHT_CHANNEL);
#endif
            osal_msleep(BREATHING_LIGHT_SPEED);
        }

        for (counter = BREATHING_LIGHT_START_VALUE_MIN; counter < BREATHING_LIGHT_MAX_VALUE;
             counter += BREATHING_LIGHT_STEP_SIZE) {
            lowLevelTime = CalculateCurveValue(counter); // 计算当前占空比;
            cfg_repeat.low_time = lowLevelTime;
            cfg_repeat.high_time = g_PwmCycle - lowLevelTime;
            uapi_pwm_open(CONFIG_BREATHING_LIGHT_CHANNEL, &cfg_repeat);
#ifdef CONFIG_PWM_USING_V151
            uapi_pwm_set_group(CONFIG_BREATHING_LIGHT_GROUP_ID, &channelId, 1);
            uapi_pwm_start_group(CONFIG_BREATHING_LIGHT_GROUP_ID);
#else
            uapi_pwm_start(CONFIG_BREATHING_LIGHT_CHANNEL);
#endif
            osal_msleep(BREATHING_LIGHT_SPEED);
        }
    }

#ifdef CONFIG_PWM_USING_V151
    uapi_pwm_close(CONFIG_BREATHING_LIGHT_GROUP_ID);
#else
    uapi_pwm_close(CONFIG_BREATHING_LIGHT_CHANNEL);
#endif

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_deinit();
    return NULL;
}

static void breathing_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)breathing_task, 0, "BreathingLightTask",
                                      BREATHING_LIGHT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BREATHING_LIGHT_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(breathing_entry);