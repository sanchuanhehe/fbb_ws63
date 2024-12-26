/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
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
#include "i2c.h"
#include "osal_debug.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "soc_osal.h"
#include "aht20.h"
#include "app_init.h"
#include "aht20_test.h"

#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SLAVE1_ADDR 0x38
#define I2C_SET_BANDRATE 400000
#define I2C_TASK_STACK_SIZE 0x1000
#define I2C_TASK_PRIO 17

const unsigned char HEAD_SIZE[] = {64, 64};
static char g_templine[32] = {0};
static char g_humiline[32] = {0};

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

/**
 * 汉字字模在线： https://www.23bei.com/tool-223.html
 * 数据排列：从左到右从上到下
 * 取模方式：横向8位左高位
 * 字体总类：[HZK1616宋体]
 **/
void temp_hum_chinese(void)
{
    const uint32_t w = 16;
    /* -- ID:0,字符:"温",ASCII编码:CEC2,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
    /* -- ID:0,字符:"度",ASCII编码:B6C8,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
    /* -- ID:0,字符:"湿",ASCII编码:CEC2,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
    /* -- ID:0,字符:"度",ASCII编码:B6C8,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
    uint8_t fonts[][32] = {
        {
            0x00, 0x08, 0x43, 0xFC, 0x32, 0x08, 0x12, 0x08, 0x83, 0xF8, 0x62, 0x08, 0x22, 0x08, 0x0B, 0xF8,
            0x10, 0x00, 0x27, 0xFC, 0xE4, 0xA4, 0x24, 0xA4, 0x24, 0xA4, 0x24, 0xA4, 0x2F, 0xFE, 0x20, 0x00
        },
        {
            0x01, 0x00, 0x00, 0x84, 0x3F, 0xFE, 0x22, 0x20, 0x22, 0x28, 0x3F, 0xFC, 0x22, 0x20, 0x23, 0xE0,
            0x20, 0x00, 0x2F, 0xF0, 0x22, 0x20, 0x21, 0x40, 0x20, 0x80, 0x43, 0x60, 0x8C, 0x1E, 0x30, 0x04
        }
    };
    uint8_t fonts2[][32] = {
        {
            0x00, 0x08, 0x47, 0xFC, 0x34, 0x08, 0x14, 0x08, 0x87, 0xF8, 0x64, 0x08, 0x24, 0x08, 0x0F, 0xF8,
            0x11, 0x20, 0x21, 0x20, 0xE9, 0x24, 0x25, 0x28, 0x23, 0x30, 0x21, 0x24, 0x3F, 0xFE, 0x20, 0x00
        },
        {
            0x01, 0x00, 0x00, 0x84, 0x3F, 0xFE, 0x22, 0x20, 0x22, 0x28, 0x3F, 0xFC, 0x22, 0x20, 0x23, 0xE0,
            0x20, 0x00, 0x2F, 0xF0, 0x22, 0x20, 0x21, 0x40, 0x20, 0x80, 0x43, 0x60, 0x8C, 0x1E, 0x30, 0x04
        }
    };
    for (size_t i = 0; i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        ssd1306_draw_region(i * w, 3, w, fonts[i], sizeof(fonts[0])); // x轴坐标i*w，y轴坐标3，宽度为16
    }
    for (size_t j = 0; j < sizeof(fonts2) / sizeof(fonts2[0]); j++) {
        ssd1306_draw_region(j * w, 35, w, fonts2[j], sizeof(fonts2[0])); // x轴坐标i*w，y轴坐标35，宽度为16
    }
}

void aht20_init(void)
{
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    app_i2c_init_pin();
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }
    ssd1306_init();
    ssd1306_fill(BLACK);
    ssd1306_set_cursor(0, 0);
    while (aht20_calibrate() != 0) {
        printf("AHT20 sensor init failed!\r\n");
        osal_mdelay(100); // 100ms在判断设备是否复位成功
    }
}

void aht20_test_task(environment_msg *msg)
{
    temp_hum_chinese();
    errcode_t retval = aht20_start_measure();
    retval = aht20_get_measure_result(&msg->temperature, &msg->humidity);
    if (retval != 0) {
        printf("get humidity data failed!\r\n");
    }
    ssd1306_set_cursor(32, 8); /* x坐标为32，y轴坐标为8 */
    int ret = sprintf(g_templine, ": %.2f", msg->temperature);
    if (ret < 0) {
        printf("temp failed\r\n");
    }
    ssd1306_draw_string(g_templine, g_font_7x10, WHITE);
    ret = sprintf(g_humiline, ": %.2f", msg->humidity);
    if (ret < 0) {
        printf("humi failed\r\n");
    }
    ssd1306_set_cursor(32, 40); /* x坐标为32，y轴坐标为40 */
    ssd1306_draw_string(g_humiline, g_font_7x10, WHITE);
    ssd1306_update_screen();
}