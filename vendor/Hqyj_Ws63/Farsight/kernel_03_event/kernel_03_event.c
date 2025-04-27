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
osThreadId_t Task1_ID;               //  任务1 ID
osThreadId_t Task2_ID;               //  任务2 ID
osEventFlagsId_t event_ID;           // 事件 ID
uint32_t event1_Flags = 0x00000001U; // 事件掩码 每一位代表一个事件
uint32_t event2_Flags = 0x00000002U; // 事件掩码 每一位代表一个事件
uint32_t event3_Flags = 0x00000004U; // 事件掩码 每一位代表一个事件
/**
 * @description: 任务1
 * @param {*}
 * @return {*}
 */
void task1(const char *argument)
{
    unused(argument);
    while (1) {
        printf("enter Task 1.......\n");
        osEventFlagsSet(event_ID, event1_Flags); // 设置事件标记
        printf("send eventFlag1.......\n");
        osDelay(DELAY_TASK1_MS);                            // 1秒
        osEventFlagsSet(event_ID, event2_Flags); // 设置事件标记
        printf("send eventFlag2.......\n");
        osDelay(DELAY_TASK1_MS);                            // 1秒
        osEventFlagsSet(event_ID, event3_Flags); // 设置事件标记
        printf("send eventFlag3.......\n");
        osDelay(DELAY_TASK1_MS); // 1秒
    }
}
/**
 * @description: 任务2 用于接受事件
 * @param {*}
 * @return {*}
 */
void task2(const char *argument)
{
    unused(argument);
    uint32_t flags = 0;
    while (1) {
        // 永远等待事件标记触发，当接收到 event1_Flags 和 event2_Flags 和 event3_Flags时才会执行printf函数
        // osFlagsWaitAll ：全部事件标志位接收到    osFlagsWaitAny: 任意一个事件标志位接收到
        // 当只有一个事件的时候，事件的类型选择哪个都可以
        flags = osEventFlagsWait(event_ID, event1_Flags | event2_Flags | event3_Flags, osFlagsWaitAll, osWaitForever);
        printf("receive event is OK %d\n", flags); // 事件已经标记 0x07 ->0111
    }
}

static void kernel_event_example(void)
{
    event_ID = osEventFlagsNew(NULL); // 创建事件
    if (event_ID != NULL) {
        printf("ID = %d, Create event_ID is OK!\n", event_ID);
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
app_run(kernel_event_example);