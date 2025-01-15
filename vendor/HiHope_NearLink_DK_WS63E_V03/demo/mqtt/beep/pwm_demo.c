/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
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

#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"

#define PWM_CHANNEL 1
#define PWM_GROUP_ID 1
#define CONFIG_PWM_PIN 9
#define CONFIG_PWM_PIN_MODE 1

void pwm_init(void)
{
    pwm_config_t cfg_no_repeat = {20000,
                                  10000, // 高电平持续tick 时间 = tick * (1/32000000)
                                  0,     // 相位偏移位
                                  1,     // 发多少个波形
                                  true}; // 是否循环
    uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);
    uapi_tcxo_delay_ms(1000); // 等待1000ms初始化完成
    uint8_t channel_id = PWM_CHANNEL;
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
}

void pwm_task(char *beep_status)
{
    if (strcmp(beep_status, "ON") == 0) {
        uapi_pwm_start_group(PWM_GROUP_ID);
    } else if (strcmp(beep_status, "OFF") == 0) {
        uapi_pwm_stop_group(PWM_GROUP_ID);
    }
}
