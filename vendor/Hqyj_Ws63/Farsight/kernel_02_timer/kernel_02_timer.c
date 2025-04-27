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
osThreadId_t Task1_ID; //  任务1 ID
osThreadId_t Timer_ID; // 定时器ID

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
        osDelay(DELAY_TASK1_MS);
    }
}
/**
 * @description: 定时器1回调函数
 * @param {*}
 * @return {*}
 */
void timer1_callback(void *argument)
{
    unused(argument);
    printf("enter timer1_callback.......\n");
}

static void kernel_timer_example(void)
{
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
    Timer_ID = osTimerNew(timer1_callback, osTimerPeriodic, NULL, NULL); // 创建定时器
    if (Timer_ID != NULL) {
        printf("ID = %d, Create Timer_ID is OK!\n", Timer_ID);

        osStatus_t timerStatus =
            osTimerStart(Timer_ID, 300U); // 开始定时器， 并赋予定时器的定时值（在ws63中，1U=10ms，100U=1S）
        if (timerStatus != osOK) {
            printf("timer is not startRun !\n");
        }
    }
}

/* Run the sample. */
app_run(kernel_timer_example);