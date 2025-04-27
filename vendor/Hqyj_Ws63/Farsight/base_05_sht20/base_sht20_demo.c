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
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "sht20/hal_bsp_sht20.h"
#include "app_init.h"

osThreadId_t task1_ID; // 任务1

#define DELAY_TIME_MS 100

void task1(void)
{
    float temperature = 0;
    float humidity = 0;
    SHT20_Init(); // SHT20初始化

    while (1) {
        SHT20_ReadData(&temperature, &humidity);

        printf("temperature = %d  humidity = %d\r\n", (int)temperature, (int)humidity);
        osDelay(DELAY_TIME_MS);
    }
}
static void base_sht20_demo(void)
{
    printf("Enter base_sht20_demo()!\r\n");

    osThreadAttr_t attr;
    attr.name = "task1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x2000;
    attr.priority = osPriorityNormal;

    task1_ID = osThreadNew((osThreadFunc_t)task1, NULL, &attr);
    if (task1_ID != NULL) {
        printf("ID = %d, Create task1_ID is OK!\r\n", task1_ID);
    }
}
app_run(base_sht20_demo);