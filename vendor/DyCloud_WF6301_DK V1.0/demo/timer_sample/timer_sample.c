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

#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define SIMPLE_TIMER_DEVICE 2         // 使用的定时器设备号
#define SIMPLE_IRQ_PRIORITY 2         // 中断优先级
#define SIMPLE_DELAY_US 1000000       // 定时器周期1秒
#define SIMPLE_TASK_STACK_SIZE 0x1000 // 任务栈大小
#define SIMPLE_TASK_PRIORITY 25       // 任务优先级

static timer_handle_t g_timer_handle; // 定时器操作句柄

static void simple_timer_callback(uintptr_t param)
{
    unused(param); // 避免未使用参数警告
    static uint16_t count = 1;
    osal_printk("第%d次进入\r\n", count++);

    uapi_timer_start(g_timer_handle, SIMPLE_DELAY_US, simple_timer_callback, 0);
}

static void *simple_timer_task(const char *arg)
{
    unused(arg); // 避免未使用参数警告
    static uint16_t count = 0;

    uapi_timer_init();
    uapi_timer_adapter(SIMPLE_TIMER_DEVICE, TIMER_2_IRQN, SIMPLE_IRQ_PRIORITY);

    if (uapi_timer_create(SIMPLE_TIMER_DEVICE, &g_timer_handle) != 0) {
        osal_printk("Failed to create timer\n");
        return NULL;
    }

    if (uapi_timer_start(g_timer_handle, SIMPLE_DELAY_US, simple_timer_callback, 0) != 0) {
        osal_printk("Failed to start timer,%d\n", count++);
        uapi_timer_delete(g_timer_handle);
        return NULL;
    }

    while (1) {
        osal_msleep(1000); // 主循环休眠1秒 = 1000ms
    }

    return NULL;
}

static void simple_timer_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)simple_timer_task, NULL, "SimpleTimerTask", SIMPLE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SIMPLE_TASK_PRIORITY);
    } else {
        osal_printk("Failed to create timer task\n");
    }
    osal_kthread_unlock();
}

app_run(simple_timer_entry);