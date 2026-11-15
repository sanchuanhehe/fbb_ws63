/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "i2c.h"
#include "osal_debug.h"
#include "systick.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "app_init.h"

#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SLAVE1_ADDR 0x38
#define I2C_SET_BANDRATE 400000
#define I2C_TASK_STACK_SIZE 0x1000
#define I2C_TASK_PRIO 17
#define DELAY_S 1000

#define LARGE_FONT_HEIGHT 15

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

void oled_task(void)
{
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    app_i2c_init_pin();
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }
    ssd1306_init();

    // 定义不同字号的字体数组
    font_def_t fonts[] = {g_font_6x8, g_font_7x10, g_font_11x18, g_font_16x26};
    char *font_names[] = {"6x8", "7x10", "11x18", "16x26"};
    int font_count = sizeof(fonts) / sizeof(fonts[0]);
    int current_font = 0;

    while (1) {
        // 清屏
        ssd1306_fill(SSD1306_COLOR_BLACK);

        // 显示当前使用的字体信息
        ssd1306_set_cursor(0, 0);
        ssd1306_draw_string("Font:", g_font_6x8, SSD1306_COLOR_WHITE);
        ssd1306_draw_string(font_names[current_font], g_font_6x8, SSD1306_COLOR_WHITE);

        // 根据字体大小调整显示位置
        int y_pos = 20;
        if (fonts[current_font].font_height > LARGE_FONT_HEIGHT) {
            y_pos = LARGE_FONT_HEIGHT; // 大字体需要更靠上的位置
        }

        // 显示主要文本
        ssd1306_set_cursor(0, y_pos);
        ssd1306_draw_string("Hello,DFRobot!", fonts[current_font], SSD1306_COLOR_WHITE);

        // 更新屏幕
        ssd1306_update_screen();

        // 延时2秒
        uapi_systick_delay_ms(DELAY_S * 2);

        // 切换到下一个字体
        current_font = (current_font + 1) % font_count;
    }
}

void oled_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)oled_task, NULL, "oled_task", I2C_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, I2C_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

app_run(oled_entry);