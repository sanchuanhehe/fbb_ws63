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
#include "hal_bsp_oled/hal_bsp_ssd1306.h"
#include "app_init.h"

osThreadId_t task1_ID; // 任务1

#define DELAY_TIME_MS 100
#define SEC_MAX 60
#define MIN_MAX 60
#define HOUR_MAX 24

void task1(void)
{
    char display_buffer[20] = {0};
    uint8_t hour = 10;
    uint8_t min = 30;
    uint8_t sec = 0;
    printf("ssd1306_Init!\n");
    ssd1306_init(); // OLED 显示屏初始化
    ssd1306_cls();  // 清屏
    ssd1306_show_str(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, "  Analog Clock ", TEXT_SIZE_16);
    ssd1306_show_str(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_3, "   2025-01-01  ", TEXT_SIZE_16);

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
        memset_s(display_buffer, sizeof(display_buffer), 0, sizeof(display_buffer));
        if (sprintf_s(display_buffer, sizeof(display_buffer), "    %02d:%02d:%02d   ", hour, min, sec) > 0) {
            ssd1306_show_str(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_2, display_buffer, TEXT_SIZE_16);
        }
        osDelay(DELAY_TIME_MS);
    }
}
static void base_ssd1306_demo(void)
{
    printf("Enter base_sdd1306_demo()!\r\n");

    osThreadAttr_t attr;
    attr.name = "Task1";
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
app_run(base_ssd1306_demo);