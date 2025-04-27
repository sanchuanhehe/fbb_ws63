/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "cmsis_os2.h"
#include "platform_core_rom.h"
osThreadId_t task1_ID; // 任务1

#define DELAY_TIME_MS 50 // 延时时间，单位为毫秒
static int blink_task(const char *arg)
{
    unused(arg);
    // 配置引脚为普通GPIO模式
    uapi_pin_set_mode(GPIO_10, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(GPIO_13, HAL_PIO_FUNC_GPIO);
    // 配置GPIO为输出模式 低电平
    uapi_gpio_set_dir(GPIO_10, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_10, GPIO_LEVEL_LOW);
    // 配置GPIO为输出模式 高电平
    uapi_gpio_set_dir(GPIO_13, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_HIGH);
    while (1) {
        osDelay(DELAY_TIME_MS);    // 延时5s
        uapi_gpio_toggle(GPIO_10); // 电平反转
        uapi_gpio_toggle(GPIO_13);
        printf("gpio toggle.\n");
    }

    return 0;
}

static void blink_entry(void)
{
    printf("Enter blink_entry()!\r\n");

    osThreadAttr_t attr;
    attr.name = "Task1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x1000;
    attr.priority = osPriorityNormal;

    task1_ID = osThreadNew((osThreadFunc_t)blink_task, NULL, &attr);
    if (task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\r\n", task1_ID);
    }
}

/* Run the sample. */
app_run(blink_entry);