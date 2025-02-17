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
#include "hal_bsp_oled/hal_bsp_ssd1306.h"
#include "app_init.h"

osThreadId_t Task1_ID; // 任务1设置为低优先级任务

#define SEC_MAX 60
#define MIN_MAX 60
#define HOUR_MAX 24

void Task1(void)
{
    char displayBuff[20] = {0};
    uint8_t hour = 10, min = 30, sec = 0;
    printf("SSD1306_Init!\r\n");
    SSD1306_Init(); // OLED 显示屏初始化
    SSD1306_CLS(); // 清屏
    SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, "  Analog Clock ", TEXT_SIZE_16);
    SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_3, "   2025-01-01  ", TEXT_SIZE_16);

    while (1) {
        sec++;
        if (sec > (SEC_MAX - 1)) {
            sec = 0;
            min++;
        }
        if (min > (MIN_MAX - 1)) {
            min = 0;
            hour++;
        }
        if (hour > (HOUR_MAX - 1)) {
            hour = 0;
        }
        memset_s(displayBuff, sizeof(displayBuff), 0, sizeof(displayBuff));
        if (sprintf_s(displayBuff, sizeof(displayBuff), "    %02d:%02d:%02d   ", hour, min, sec) > 0) {
            SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_2, displayBuff, TEXT_SIZE_16);
        }
        osal_msleep(1000);
    }
}
static void base_ssd1306_demo(void)
{
    printf("Enter base_sdd1306_demo()!\r\n");

  
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
app_run(base_ssd1306_demo);