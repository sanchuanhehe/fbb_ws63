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
#define DELAY_TASK2_MS 200
osThreadId_t Task1_ID;        //  任务1 ID
osThreadId_t Task2_ID;        //  任务2 ID
osSemaphoreId_t Semaphore_ID; // 信号量ID
#define SEM_MAX_COUNT 10

/**
 * @description: 任务1 模拟停车场的入口
 *  每隔一秒钟，停车场就会进入一辆车
 * @param {*}
 * @return {*}
 */
void task1(const char *argument)
{
    unused(argument);
    while (1) {
        if (osSemaphoreGetCount(Semaphore_ID)) {
            if (osSemaphoreAcquire(Semaphore_ID, 0xff) == osOK) { // 信号量 -1
                printf("[进入%d辆车, 停车场容量: %d] 信号量-1.\n", SEM_MAX_COUNT - osSemaphoreGetCount(Semaphore_ID),
                       SEM_MAX_COUNT);
            }
        } else {
            printf("[进入停车场失败, 请等待...]\n");
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
    while (1) { // 信号量 +1
        if (osSemaphoreRelease(Semaphore_ID) == osOK) {
            printf("[出去1辆车, 剩余停车场容量: %d] 信号量+1.\n", osSemaphoreGetCount(Semaphore_ID));
        } else {
            printf("[出停车场失败]\n");
        }
        osDelay(DELAY_TASK2_MS);
    }
}

static void kernel_count_semaphore_example(void)
{
    // 创建信号量     一个停车场最多可以停10辆车
    Semaphore_ID = osSemaphoreNew(SEM_MAX_COUNT, SEM_MAX_COUNT, NULL); // 参数: 最大计数值，初始计数值，参数配置
    if (Semaphore_ID != NULL) {
        printf("ID = %d, Create Semaphore_ID is OK!\n", Semaphore_ID);
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
app_run(kernel_count_semaphore_example);