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

#include "common.h"
#include "osal_wait.h"
#include "soc_errno.h"
#include "soc_osal.h"
#include "app_init.h"

#define TASK1_PRIO 21          // 等待任务优先级
#define TASK2_PRIO 20          // 发送任务优先级
#define MAIN_TASK_PRIO 24      // 主任务优先级
#define TASK_STACK_SIZE 0x1000 // 任务栈大小

static osal_semaphore g_sync_sem;

void wait_task(void)
{
    printf("等待任务启动，正在等待信号量...\n");

    if (osal_sem_down_timeout(&g_sync_sem, OSAL_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("等待任务收到信号量，继续执行\n");
    }

    printf("等待任务正在执行操作...\n");
    osal_mdelay(100); // 模拟任务执行 (100ms延迟)

    printf("等待任务完成\n");
}

void signal_task(void)
{
    printf("发送任务启动，延时300ms后发送信号量\n");

    osal_mdelay(300); // 任务延时 (300ms)

    printf("发送任务释放信号量\n");
    osal_sem_up(&g_sync_sem);

    printf("发送任务完成\n");
}

static void simple_sem_example(void)
{
    uint32_t ret;
    osal_task *wait_task_id, *signal_task_id;

    osal_sem_init(&g_sync_sem, 0); // 信号量初始值=0

    printf("信号量示例开始\n");

    osal_kthread_lock();

    wait_task_id = osal_kthread_create((osal_kthread_handler)wait_task, NULL, "wait_task", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(wait_task_id, TASK1_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建等待任务失败\n");
    }

    signal_task_id = osal_kthread_create((osal_kthread_handler)signal_task, NULL, "signal_task", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(signal_task_id, TASK2_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建发送任务失败\n");
    }

    osal_kthread_unlock();

    osal_mdelay(600); // 等待任务完成 (600ms超时)

    osal_sem_destroy(&g_sync_sem);

    osal_kthread_destroy(wait_task_id, 0);
    osal_kthread_destroy(signal_task_id, 0);

    printf("信号量示例结束\n");
}

static void sem_example_entry(void)
{
    uint32_t ret;
    osal_task *task_id;

    osal_kthread_lock();

    task_id = osal_kthread_create((osal_kthread_handler)simple_sem_example, NULL, "sem_example", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(task_id, MAIN_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建主任务失败\n");
    }

    osal_kthread_unlock();
}

app_run(sem_example_entry);