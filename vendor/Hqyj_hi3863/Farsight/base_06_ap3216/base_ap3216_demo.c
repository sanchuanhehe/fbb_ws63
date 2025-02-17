/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
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
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "hal_bsp_ap3216/hal_bsp_ap3216.h"
#include "app_init.h"
#include "osal_task.h"
osThreadId_t Task1_ID; // 任务1设置为低优先级任务

void Task1(void)
{
    uint16_t ir = 0, als = 0, ps = 0; // 人体红外传感器 接近传感器 光照强度传感器
    AP3216C_Init();
    while (1) 
    {
        AP3216C_ReadData(&ir, &als, &ps);
        printf("ir = %d    als = %d    ps = %d\r\n", ir, als, ps);
        osal_msleep(1000);
    }
}
static void base_ap3216_demo(void)
{
    printf("Enter base_ap3216_demo()!\r\n");

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
app_run(base_ap3216_demo);