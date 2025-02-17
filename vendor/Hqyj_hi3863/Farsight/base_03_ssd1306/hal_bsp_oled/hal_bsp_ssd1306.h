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

#ifndef HAL_BSP_SSD1306_H
#define HAL_BSP_SSD1306_H

#include "cmsis_os2.h"
#define I2C_MASTER_ADDRESS 0x0
#define SSD1306_I2C_ADDR 0X3c    // 器件的I2C从机地址
#define SSD1306_I2C_IDX 1        // 模块的I2C总线号
#define SSD1306_I2C_SPEED 100000 // 100KHz
#define TEXT_SIZE_8 8
#define TEXT_SIZE_16 16

// OLED列坐标 - 字体大小为8
typedef enum {
    OLED_TEXT8_COLUMN_0,
    OLED_TEXT8_COLUMN_1,
    OLED_TEXT8_COLUMN_2,
    OLED_TEXT8_COLUMN_3,
    OLED_TEXT8_COLUMN_5,
    OLED_TEXT8_COLUMN_6,
    OLED_TEXT8_COLUMN_7,
    OLED_TEXT8_COLUMN_8,
    OLED_TEXT8_COLUMN_9,
    OLED_TEXT8_COLUMN_10,
    OLED_TEXT8_COLUMN_11,
    OLED_TEXT8_COLUMN_12,
    OLED_TEXT8_COLUMN_13,
    OLED_TEXT8_COLUMN_14,
    OLED_TEXT8_COLUMN_15,
} te_oled_text8_column_t;

// OLED列坐标 - 字体大小为8
typedef enum {
    OLED_TEXT16_COLUMN_0,
    OLED_TEXT16_COLUMN_1,
    OLED_TEXT16_COLUMN_2,
    OLED_TEXT16_COLUMN_3,
    OLED_TEXT16_COLUMN_4,
    OLED_TEXT16_COLUMN_5,
    OLED_TEXT16_COLUMN_6,
    OLED_TEXT16_COLUMN_7,
    OLED_TEXT16_COLUMN_8,
    OLED_TEXT16_COLUMN_9,
    OLED_TEXT16_COLUMN_10,
    OLED_TEXT16_COLUMN_11,
    OLED_TEXT16_COLUMN_12,
    OLED_TEXT16_COLUMN_13,
    OLED_TEXT16_COLUMN_14,
    OLED_TEXT16_COLUMN_15,
    OLED_TEXT16_COLUMN_16,
    OLED_TEXT16_COLUMN_17,
    OLED_TEXT16_COLUMN_18,
    OLED_TEXT16_COLUMN_19,
    OLED_TEXT16_COLUMN_20,
    OLED_TEXT16_COLUMN_21,
} te_oled_text16_column_t;

// OLED行坐标 - 字体大小为8
typedef enum {
    // (字体大小8: 0 ~ 7)
    OLED_TEXT8_LINE_0 = 0,
    OLED_TEXT8_LINE_1,
    OLED_TEXT8_LINE_2,
    OLED_TEXT8_LINE_3,
    OLED_TEXT8_LINE_4,
    OLED_TEXT8_LINE_5,
    OLED_TEXT8_LINE_6,
    OLED_TEXT8_LINE_7,
} TeOledText8Line_t;

// OLED行坐标 - 字体大小为16
typedef enum {
    // (字体大小16: 0 ~ 3)
    OLED_TEXT16_LINE_0 = 0,
    OLED_TEXT16_LINE_1,
    OLED_TEXT16_LINE_2,
    OLED_TEXT16_LINE_3,
} TeOledText16Line_t;

/**
 * @brief SSD1306 初始化
 * @return Returns {@link HI_ERR_SUCCESS} 成功;
 *         Returns {@link HI_ERR_FAILURE} 失败.
 */
uint32_t SSD1306_Init(void);
/**
 * @brief SSD1306 设置坐标的起始点坐标
 * @param x X轴坐标
 * @param y Y轴坐标
 * @return none
 */
void SSD1306_SetPosition(unsigned char x, unsigned char y);
/**
 * @brief SSD1306 全屏填充
 * @return none
 */
void SSD1306_Fill(unsigned char fillData);
/**
 * @brief SSD1306 清屏
 * @return none
 */
void SSD1306_Cls(void);
/**
 * @brief SSD1306 打开OLED显示
 * @return none
 */
void SSD1306_On(void);
/**
 * @brief SSD1306 关闭OLED显示
 * @return none
 *
 */
void SSD1306_Off(void);
/**
 * @brief SSD1306 OLED显示屏显示字符串
 * @param x X轴坐标 0~128
 * @param y Y轴坐标 (字体大小8: 0 ~ 7)、(字体大小16: 0 ~ 3)
 * @param ch 显示的字符串
 * @param TextSize 显示的字体大小  8：6*8   16：8*16
 * @return none
 */
void SSD1306_ShowStr(uint8_t x, uint8_t y, char *, uint8_t TextSize);

/**
 * @brief  显示BMP图片
 * @param  x0: 左上角坐标
 * @param  x1: 右下角坐标
 * @param  BMP[]: 图片数组
 * @retval None
 */
void SSD1306DrawBMP(unsigned char xmove,
                     unsigned char ymove,
                     unsigned char width,
                     unsigned char height,
                     unsigned char *bmp);

#endif