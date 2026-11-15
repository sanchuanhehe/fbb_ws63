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
#include "uart.h"

#define THREAD_A_PRIO 20       // 线程A优先级
#define THREAD_B_PRIO 20       // 线程B优先级
#define MAIN_TASK_PRIO 24      // 主任务优先级
#define TASK_STACK_SIZE 0x1000 // 任务栈大小

void thread_a_function(void)
{
    int count = 0;
    while (count < 5) { // 循环执行5次
        printf("线程A：正在运行 - 计数 %d\n", count);
        osal_mdelay(100); // 延时100ms，让出CPU
        count++;
    }
    osal_printk("线程A：执行完毕\n");
}

void thread_b_function(void)
{
    int count = 0;
    while (count < 5) { // 循环执行5次
        printf("线程B：正在运行 - 计数 %d\n", count);
        osal_mdelay(150); // 延时150ms，与线程A不同延时时间
        count++;
    }
    printf("线程B：执行完毕\n");
}

static void thread_example_entry(void)
{
    uint32_t ret;
    osal_task *thread_a_id, *thread_b_id;

    osal_kthread_lock(); // 锁定任务调度

    thread_a_id = osal_kthread_create((osal_kthread_handler)thread_a_function, NULL, "ThreadA", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(thread_a_id, THREAD_A_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建线程A失败\n");
    }

    thread_b_id = osal_kthread_create((osal_kthread_handler)thread_b_function, NULL, "ThreadB", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(thread_b_id, THREAD_B_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建线程B失败\n");
    }

    osal_kthread_unlock(); // 解锁任务调度
}

app_run(thread_example_entry);