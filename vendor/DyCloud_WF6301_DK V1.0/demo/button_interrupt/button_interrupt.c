/*
 * Copyright (c) 2025 YunQiHui Network Technology (Shenzhen) Co., Ltd.
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
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
 *
 * Website: http://www.siotmate.com/
 */

#include "pinctrl.h"
#include "gpio.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#define BUTTON_TASK_PRIO 24         //任务优先级
#define BUTTON_TASK_STACK_SIZE 1024 //任务栈大小
#define CONFIG_BUTTON_PIN 12

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    osal_printk("按键已按下\r\n");
}

static void button_task(void)
{
    uapi_pin_set_mode(CONFIG_BUTTON_PIN, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(CONFIG_BUTTON_PIN, PIN_PULL_TYPE_UP);
    uapi_gpio_set_dir(CONFIG_BUTTON_PIN, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(CONFIG_BUTTON_PIN, GPIO_INTERRUPT_RISING_EDGE, gpio_callback_func);
}

static void button_interrupt_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)button_task, 0, "BUTTONTask", BUTTON_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BUTTON_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(button_interrupt_entry);