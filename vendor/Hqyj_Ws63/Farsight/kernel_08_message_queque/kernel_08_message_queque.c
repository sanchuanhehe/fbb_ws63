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

#include "soc_osal.h"
#include "app_init.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include <stdio.h>
#define DELAY_TASK1_MS 100
#define TIME_OUT 100
osThreadId_t Task1_ID;          //  任务1 ID
osThreadId_t Task2_ID;          //  任务2 ID
osMessageQueueId_t MsgQueue_ID; // 消息队列的ID

#define MSG_QUEUE_NUMBER 16 // 定义消息队列对象的个数
typedef struct {
    uint8_t id;  // ID
    uint8_t age; // 年龄
    char *name;  // 名字
} message_people;
message_people g_msg_people;

/**
 * @description: 任务1 发送消息
 * @param {*}
 * @return {*}
 */
void task1(const char *argument)
{
    unused(argument);
    osStatus_t msgStatus;
    static uint8_t i = 0;
    static uint8_t j = 0;
    while (1) {
        printf("enter Task 1.......\n");
        g_msg_people.id = i++;
        g_msg_people.age = j++;
        g_msg_people.name = "xiao_ming";
        msgStatus = osMessageQueuePut(MsgQueue_ID, &g_msg_people, 0, TIME_OUT);
        if (msgStatus == osOK) {
            printf("osMessageQueuePut is ok.\n");
        }
        osDelay(DELAY_TASK1_MS);
    }
}
/**
 * @description: 任务2
 * @param {*}
 * @return {*}
 */
void task2(const char *argument)
{
    unused(argument);
    osStatus_t msgStatus;
    while (1) {
        printf("enter Task 2.......\n");
        msgStatus = osMessageQueueGet(MsgQueue_ID, &g_msg_people, 0, TIME_OUT);
        if (msgStatus == osOK) {
            printf("osMessageQueueGet is ok.\n");
            printf("Recv: id = %d, age = %d, name = %s\n", g_msg_people.id, g_msg_people.age, g_msg_people.name);
        }
    }
}

static void kernel_message_queue_example(void)
{
    // 创建消息队列
    MsgQueue_ID = osMessageQueueNew(MSG_QUEUE_NUMBER, sizeof(message_people),
                                    NULL); // 消息队列中的消息个数，消息队列中的消息大小，属性
    if (MsgQueue_ID != NULL) {
        printf("ID = %d, Create MsgQueue_ID is OK!\n", MsgQueue_ID);
    }

    osThreadAttr_t attr;
    attr.name = "Task1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0X2000;
    attr.priority = osPriorityNormal;

    Task1_ID = osThreadNew((osThreadFunc_t)task1, NULL, &attr); // 创建任务1
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\n", Task1_ID);
    }
    attr.name = "Task2";                                        // 任务的名字
    Task2_ID = osThreadNew((osThreadFunc_t)task2, NULL, &attr); // 创建任务2
    if (Task2_ID != NULL) {
        printf("ID = %d, Create Task2_ID is OK!\n", Task2_ID);
    }
}

/* Run the sample. */
app_run(kernel_message_queue_example);