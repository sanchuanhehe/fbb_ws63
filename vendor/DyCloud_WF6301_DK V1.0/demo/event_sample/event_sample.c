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

#define DATA_READY_EVENT (1 << 0) // 数据就绪事件
#define TASK_STACK_SIZE 0x1000    //任务栈大小

// 任务优先级
#define PRODUCER_TASK_PRIO 24
#define CONSUMER_TASK_PRIO 25

// 全局变量
static osal_event g_event; // 事件控制块
static uint32_t g_data;    // 共享数据

// 产生数据并发送事件通知
static void producer_task(const char *arg)
{
    unused(arg);
    uint32_t ret;
    uint32_t count = 0;

    printf("生产者任务启动\r\n");

    // 循环5个数据项
    while (count < 5) {
        // 生成数据（简单计数）
        g_data = count + 1;
        printf("生产者：生成数据 %u\r\n", g_data);
        // 发送数据就绪事件
        ret = osal_event_write(&g_event, DATA_READY_EVENT);
        if (ret != OSAL_SUCCESS) {
            printf("生产者：事件发送失败\r\n");
        } else {
            printf("生产者：发送数据就绪事件\r\n");
        }

        count++;
        osal_msleep(1000); // 等待1秒 = 1000ms
    }
}

// 等待事件并处理数据
static void consumer_task(const char *arg)
{
    unused(arg);
    uint32_t ret;
    uint32_t count = 0;

    printf("消费者任务启动\r\n");

    // 循环处理5个数据项
    while (count < 5) {
        // 等待数据就绪事件
        printf("消费者: 等待数据就绪事件...\r\n");
        ret = osal_event_read(&g_event, DATA_READY_EVENT, OSAL_WAIT_FOREVER, OSAL_WAITMODE_AND);

        if (ret != 1) {
            printf("消费者: 等待事件失败\r\n");
        } else {
            // 处理数据
            printf("消费者: 收到数据就绪事件\r\n");
            printf("消费者: 处理数据 %u\r\n", g_data);

            // 清除事件标志
            ret = osal_event_clear(&g_event, DATA_READY_EVENT);
            if (ret != OSAL_SUCCESS) {
                printf("消费者: 清除事件失败\r\n");
            }

            count++;
        }
    }
}

void event_example_entry(void)
{
    uint32_t ret;
    osal_task *producer_handle, *consumer_handle;

    // 初始化事件
    ret = osal_event_init(&g_event);
    if (ret != OSAL_SUCCESS) {
        printf("事件初始化失败\r\n");
        return;
    }
    printf("事件初始化成功\r\n");

    // 创建任务期间锁住任务调度
    osal_kthread_lock();

    producer_handle = osal_kthread_create((osal_kthread_handler)producer_task, NULL, "producer_task", TASK_STACK_SIZE);
    if (producer_handle == NULL) {
        printf("生产者任务创建失败\r\n");
        osal_kthread_unlock();
        return;
    }

    ret = osal_kthread_set_priority(producer_handle, PRODUCER_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("设置生产者任务优先级失败\r\n");
    }

    consumer_handle = osal_kthread_create((osal_kthread_handler)consumer_task, NULL, "consumer_task", TASK_STACK_SIZE);
    if (consumer_handle == NULL) {
        printf("消费者任务创建失败\r\n");
        osal_kthread_unlock();
        return;
    }

    ret = osal_kthread_set_priority(consumer_handle, CONSUMER_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("设置消费者任务优先级失败\r\n");
    }

    // 解锁任务调度
    osal_kthread_unlock();

    printf("所有任务已创建并启动\r\n");
}

app_run(event_example_entry);
