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
#include "common_def.h"
#include "soc_osal.h"
#include "osal_wait.h"
#include "app_init.h"

#define TASK_STACK_SIZE 0x1000  // 任务栈大小
#define MUTEX_DEMO_TASK_PRIO 17 // 主任务优先级
#define TASK_PRIO_HIGH 21       // 高优先级任务优先级
#define TASK_PRIO_MED 20        // 中优先级任务优先级
#define TASK_PRIO_LOW 19        // 低优先级任务优先级

static osal_mutex g_counter_mutex; // 互斥锁
static int g_shared_counter = 0;   // 共享计数器

void task_a(void *arg)
{
    unused(arg);
    int i;
    for (i = 0; i < 5; i++) { // 执行5次
        osal_mutex_lock_timeout(&g_counter_mutex, OSAL_WAIT_FOREVER);
        g_shared_counter++;
        printf("TaskA: counter = %d\n", g_shared_counter);
        osal_mutex_unlock(&g_counter_mutex);
        osal_msleep(50); // 延迟50ms
    }
}

void task_b(void *arg)
{
    unused(arg);
    int i;
    for (i = 0; i < 5; i++) { // 执行5次
        osal_mutex_lock_timeout(&g_counter_mutex, OSAL_WAIT_FOREVER);
        g_shared_counter++;
        printf("TaskB: counter = %d\n", g_shared_counter);
        osal_mutex_unlock(&g_counter_mutex);
        osal_msleep(50); // 延迟50ms
    }
}

void task_c(void *arg)
{
    unused(arg);
    int i;
    for (i = 0; i < 5; i++) { // 执行5次
        osal_mutex_lock_timeout(&g_counter_mutex, OSAL_WAIT_FOREVER);
        g_shared_counter++;
        printf("TaskC: counter = %d\n", g_shared_counter);
        osal_mutex_unlock(&g_counter_mutex);
        osal_msleep(50); // 延迟50ms
    }
}

void mutex_demo_task(void)
{
    osal_task *task_high, *task_med, *task_low;

    printf("=== 互斥锁共享计数器示例开始 ===\n");
    osal_mutex_init(&g_counter_mutex);

    osal_kthread_lock();

    task_high = osal_kthread_create((osal_kthread_handler)task_a, NULL, "TaskA", TASK_STACK_SIZE);
    osal_kthread_set_priority(task_high, TASK_PRIO_HIGH);

    task_med = osal_kthread_create((osal_kthread_handler)task_b, NULL, "TaskB", TASK_STACK_SIZE);
    osal_kthread_set_priority(task_med, TASK_PRIO_MED);

    task_low = osal_kthread_create((osal_kthread_handler)task_c, NULL, "TaskC", TASK_STACK_SIZE);
    osal_kthread_set_priority(task_low, TASK_PRIO_LOW);

    osal_kthread_unlock();

    osal_msleep(2000); // 等待2000ms让任务完成

    osal_mutex_destroy(&g_counter_mutex);
    printf("=== 互斥锁共享计数器示例结束 ===\n");
    printf("最终计数器值: %d\n", g_shared_counter);
}

static void mutex_sample_entry(void)
{
    uint32_t ret;
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)mutex_demo_task, 0, "mutex_demo_task", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(task_handle, MUTEX_DEMO_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建任务失败\n");
    }
    osal_kthread_unlock();
}

app_run(mutex_sample_entry);