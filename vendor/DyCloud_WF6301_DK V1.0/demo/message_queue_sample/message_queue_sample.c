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

#define QUEUE_SENDER_PRIO 25   // 发送任务优先级
#define QUEUE_RECEIVER_PRIO 25 // 接收任务优先级
#define TASK_STACK_SIZE 0x1000 // 任务栈大小
#define QUEUE_SIZE 10          // 队列最大消息数
#define MSG_MAX_LEN 16         // 每条消息最大长度

static unsigned long g_queue_id; // 队列标识符

void queue_sender_task(void)
{
    printf("发送任务启动\r\n");
    uint32_t i = 0, ret = 0;
    char message[MSG_MAX_LEN];

    while (i < 5) { // 发送5条消息
        snprintf(message, sizeof(message), "Message No. %d", i);

        ret = osal_msg_queue_write_copy(g_queue_id, message, sizeof(message), OSAL_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            printf("发送消息失败，错误码: %#x\n", ret);
        } else {
            printf("发送消息: %s\n", message);
        }

        i++;
        osal_msleep(100); // 100ms发送间隔
    }
}

void queue_receiver_task(void)
{
    printf("接收任务启动\r\n");
    char receive_buffer[64]; // 接收缓冲区
    uint32_t ret = 0;
    uint32_t buffer_size = sizeof(receive_buffer);
    int count = 0;

    while (count < 5) {                       // 接收5条消息
        buffer_size = sizeof(receive_buffer); // 重置缓冲区大小
        ret = osal_msg_queue_read_copy(g_queue_id, receive_buffer, &buffer_size, OSAL_WAIT_FOREVER);

        if (ret != OSAL_SUCCESS) {
            printf("接收消息失败，错误码: %#x\n", ret);
        } else {
            receive_buffer[buffer_size < sizeof(receive_buffer) ? buffer_size : sizeof(receive_buffer) - 1] = '\0';
            printf("接收消息: %s\n", receive_buffer);
            count++;
        }

        osal_msleep(50); // 50ms接收间隔
    }

    osal_msg_queue_delete(g_queue_id);
    if (ret != OSAL_SUCCESS) {
        printf("删除队列失败，错误码: %#x\n", ret);
    } else {
        printf("队列已删除\r\n");
    }
}

static void queue_example_entry(void)
{
    uint32_t ret;
    osal_task *sender_id, *receiver_id;

    osal_kthread_lock(); // 锁定任务调度

    ret = osal_msg_queue_create("queue_example", QUEUE_SIZE, &g_queue_id, 0, MSG_MAX_LEN);
    if (ret != OSAL_SUCCESS) {
        printf("创建队列失败，错误码: %#x\n", ret);
        return;
    }
    printf("队列创建成功，队列ID: %lu\n", g_queue_id);

    sender_id = osal_kthread_create((osal_kthread_handler)queue_sender_task, NULL, "Sender", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(sender_id, QUEUE_SENDER_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建发送任务失败\n");
    }

    receiver_id = osal_kthread_create((osal_kthread_handler)queue_receiver_task, NULL, "Receiver", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(receiver_id, QUEUE_RECEIVER_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("创建接收任务失败\n");
    }

    osal_kthread_unlock(); // 解锁任务调度
}

app_run(queue_example_entry);