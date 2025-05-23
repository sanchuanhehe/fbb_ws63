/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_gpio.h"

#define TEST_TCXO_DELAY_1000MS 1000

#define PWM_TASK_PRIO 24
#define PWM_TASK_STACK_SIZE 0x1000
#define CONFIG_PWM_PIN 2
#define CONFIG_PWM_CHANNEL 2
#define CONFIG_PWM_PIN_MODE 1
#define CONFIG_PWM_GROUP_ID 0
#define PWM2_LOAD_DIV_EN 8
#define PWM2_DIV1_CFG 4
#define PWM2_DIV1_CFG_LEN 4
#define CLDO_CRG_DIV_CTL4 0x44001118
#define CLDO_CRG_CLK_SEL 0x44001134
#define PWM_CKSEL_BIT 7
#define CLDO_SUB_CRG_CKEN_CTL0 0x44004400
#define PWM_BUS_CKEN 2
#define PWM_CHANNEL_CKEN_LEN 9

void setangle(uint8_t angle)
{
    if (angle > 180) {
        angle = 180;
    }
    // 计算 pulse (500us~2500us)
    uint32_t pulse = 500 + angle * (2000) / 180; // us
    // 计算 low (10666~53333)
    uint32_t high = (uint32_t)((uint64_t)pulse * 53333 / 20000);
    // 计算 high (53333 - low)
    uint32_t low = 53333 - high;
    pwm_config_t cfg_no_repeat = {low, high, 0, 1, true};
    uapi_pwm_open(CONFIG_PWM_CHANNEL, &cfg_no_repeat);
    osal_printk("low_time= %u, high_time = %u\r\n", cfg_no_repeat.low_time, cfg_no_repeat.high_time);
    uint8_t channel_id = CONFIG_PWM_CHANNEL;
    uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(CONFIG_PWM_GROUP_ID);
    uapi_tcxo_delay_ms(1000); // 延时1000ms,1、角度每隔1S变化一次，2.舵机大概需要9个以上数量的波形,才能转到对应角度
    uapi_pwm_close(CONFIG_PWM_CHANNEL);
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    osal_printk("PWM start. \r\n");
    uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();
    uapi_reg_clrbit(CLDO_CRG_CLK_SEL, PWM_CKSEL_BIT);
    reg32_setbits(CLDO_SUB_CRG_CKEN_CTL0, PWM_BUS_CKEN, PWM_CHANNEL_CKEN_LEN, 0x1FF);
    reg32_clrbit(CLDO_CRG_DIV_CTL4, PWM2_LOAD_DIV_EN);
    reg32_setbits(CLDO_CRG_DIV_CTL4, PWM2_DIV1_CFG, PWM2_DIV1_CFG_LEN, 0xF);
    reg32_setbit(CLDO_CRG_DIV_CTL4, PWM2_LOAD_DIV_EN);
    setangle(0); // 0°
    setangle(45); // 45°
    setangle(90); // 90°
    setangle(135); // 135°
    setangle(180); // 180°
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