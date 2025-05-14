/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PWM Sample Source. \n
 *
 */

/**
 * 本例程通过PWM来实现呼吸灯功能，需将 BREATHING_LIGHT_PIN 配置成呼吸灯的IO，
 *                                  BREATHING_LIGHT_CHANNEL 换成手册里面对应的PWM通道。
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

#define BREATHING_LIGHT_TASK_PRIO 24
#define BREATHING_LIGHT_TASK_STACK_SIZE 0x1000
#define BREATHING_LIGHT_PIN GPIO_02
#define BREATHING_LIGHT_PIN_MODE 1
#define BREATHING_LIGHT_CHANNEL 2
#define BREATHING_LIGHT_GROUP_ID 0
#define BREATHING_LIGHT_MAX_TIME 300
#define BREATHING_LIGHT_MIN_TIME 50
#define DELAY_TIME 50

static void *breathing_task(const char *arg)
{
    UNUSED(arg);

    int brightness = 50; // 亮度从最小值开始
    int step = 10;

    pwm_config_t cfg_repeat = {50, 300, 0, 0, true}; // 配置PWM参数

    uapi_pin_set_mode(BREATHING_LIGHT_PIN, BREATHING_LIGHT_PIN_MODE); // 配置指定GPIO引脚的模式

    // 循环执行PWM呼吸灯效果
    while (1) {
        uapi_pwm_deinit(); // 重置PWM
        uapi_pwm_init();   // 初始化PWM模块

        // 配置PWM占空比
        cfg_repeat.low_time = brightness;                             // 设置低电平持续时间
        cfg_repeat.high_time = BREATHING_LIGHT_MAX_TIME - brightness; // 设置高电平持续时间
        uapi_pwm_open(BREATHING_LIGHT_CHANNEL, &cfg_repeat);          // 打开PWM通道并配置参数

        uint8_t channel_id = BREATHING_LIGHT_CHANNEL;
        uapi_pwm_set_group(BREATHING_LIGHT_GROUP_ID, &channel_id, 1); // 设置 PWM 通道组
        uapi_pwm_start_group(BREATHING_LIGHT_GROUP_ID);               // 启动 PWM 通道组

        // 更新亮度值，逐步增加或减少
        brightness += step;

        // 判断是否达到亮度极限，反向改变步长
        if (brightness >= BREATHING_LIGHT_MAX_TIME || brightness <= BREATHING_LIGHT_MIN_TIME) {
            step = -step; // 反向变化亮度
        }

        uapi_tcxo_delay_ms(DELAY_TIME);
        uapi_pwm_close(BREATHING_LIGHT_GROUP_ID);
        uapi_pwm_deinit();
    }

    uapi_pwm_close(BREATHING_LIGHT_GROUP_ID);
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