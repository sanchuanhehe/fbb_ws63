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

#include "stdio.h"
#include "string.h"
#include "soc_osal.h"
#include "i2c.h"
#include "securec.h"
#include "cmsis_os2.h"
#include "hal_bsp_rgb/hal_bsp_aw2013.h"
#include "app_init.h"
osThreadId_t Task1_ID; // 任务1
#define RGB_ON 255
#define RGB_OFF 0
#define TASK_DELAY_TIME 100
void Task1(void)
{
    AW2013_Init(); // 三色LED灯的初始化
    AW2013_Control_Red(RGB_OFF);
    AW2013_Control_Green(RGB_OFF);
    AW2013_Control_Blue(RGB_OFF);
  
    while (1) 
    {
        AW2013_Control_Red(RGB_ON);
        AW2013_Control_Green(RGB_OFF);
        AW2013_Control_Blue(RGB_OFF);
        osDelay(TASK_DELAY_TIME);

        AW2013_Control_Red(RGB_OFF);
        AW2013_Control_Green(RGB_ON);
        AW2013_Control_Blue(RGB_OFF);
        osDelay(TASK_DELAY_TIME);

        AW2013_Control_Red(RGB_OFF);
        AW2013_Control_Green(RGB_OFF);
        AW2013_Control_Blue(RGB_ON);
        osDelay(TASK_DELAY_TIME);
    }
}
static void base_rgb_demo(void)
{
    printf("Enter base_rgb_demo()!\r\n");

    osThreadAttr_t attr;
    attr.name       = "Task1";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = 0x2000;
    attr.priority   = osPriorityNormal;

    Task1_ID = osThreadNew((osThreadFunc_t)Task1, NULL, &attr);
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\r\n", Task1_ID);
    }
}
app_run(base_rgb_demo);