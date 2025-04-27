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
#define DELAY_TASK2_MS 500
osThreadId_t Task1_ID; //  任务1 ID
osThreadId_t Task2_ID; //  任务2 ID

/**
 * @description: 任务1为低优先级任务
 * @param {*}
 * @return {*}
 */
void task1(const char *argument)
{
    unused(argument);
    while (1) {
        printf("Task 1.......\n");
        osDelay(DELAY_TASK1_MS);
    }
}
/**
 * @description: 任务2为高优先级任务
 * @param {*}
 * @return {*}
 */
void task2(const char *argument)
{
    unused(argument);
    while (1) {
        printf("Task 2: 挂起task1\n");
        osThreadSuspend(Task1_ID); // 挂起任务1
        osDelay(DELAY_TASK2_MS);              // 延时5秒钟

        printf("Task 2: 恢复task1\n");
        osThreadResume(Task1_ID); // 恢复任务1
        osDelay(DELAY_TASK2_MS);             // 延时5秒钟
    }
}
static void kernel_task_example(void)
{
    osThreadAttr_t attr;
    attr.name = "Task1";              // 任务的名字
    attr.attr_bits = 0U;              // 属性位
    attr.cb_mem = NULL;               // 堆空间地址
    attr.cb_size = 0U;                // 堆空间大小
    attr.stack_mem = NULL;            // 栈空间地址
    attr.stack_size = 0x1000;         // 栈空间大小 单位:字节
    attr.priority = osPriorityNormal; // 任务的优先级，数据越大，优先级越高

    Task1_ID = osThreadNew((osThreadFunc_t)task1, NULL, &attr); // 创建任务1
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\n", Task1_ID);
    }
    attr.name = "Task2";
    attr.priority = osPriorityNormal1;
    Task2_ID = osThreadNew((osThreadFunc_t)task2, NULL, &attr); // 创建任务2
    if (Task2_ID != NULL) {
        printf("ID = %d, Create Task2_ID is OK!\n", Task2_ID);
    }
}

/* Run the sample. */
app_run(kernel_task_example);