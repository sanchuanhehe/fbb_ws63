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

/**
 ******************************************************************************
 * @file   bsp_ili9341_4line.h
 * @brief  2.8寸屏ili9341驱动文件，采用4线SPI
 *
 ******************************************************************************
 */
#ifndef BSP_ILI9341_H
#define BSP_ILI9341_H
#include "spi.h"
#include "gpio.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "font.h"
typedef struct {
    uint16_t width;  // ili9341 宽度
    uint16_t height; // ili9341 高度
    uint16_t id;     // ili9341 ID
    uint8_t wramcmd; // 开始写gram指令
    uint8_t setxcmd; // 设置x坐标指令
    uint8_t setycmd; // 设置y坐标指令
} ili_dev;
// 扫描方向定义
#define L2R_U2D 0 // 从左到右,从上到下
#define L2R_D2U 1 // 从左到右,从下到上
#define R2L_U2D 2 // 从右到左,从上到下
#define R2L_D2U 3 // 从右到左,从下到上

#define U2D_L2R 4 // 从上到下,从左到右
#define U2D_R2L 5 // 从上到下,从右到左
#define D2U_L2R 6 // 从下到上,从左到右
#define D2U_R2L 7 // 从下到上,从右到左

// 屏幕显示方式
typedef enum {
    SCANVERTICAL = 0U, // 竖屏
    SCANHORIZONTAL     // 横屏
} screen_show_dir;
// 画笔颜色
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 // 棕色
#define BRRED 0XFC07 // 棕红色
#define GRAY 0X8430  // 灰色
// GUI颜色
#define DARKBLUE 0X01CF  // 深蓝色
#define LIGHTBLUE 0X7D7C // 浅蓝色
#define GRAYBLUE 0X5458  // 灰蓝色
// 以上三色为PANEL的颜色
#define LIGHTGREEN 0X841F // 浅绿色
// #define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 0XC618 // 浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE 0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12    // 浅棕蓝色(选择条目的反色)

void ili9341_init(void);                                                  // 初始化
void ili9341_Clear(uint16_t Color);                                       // 清屏
void ili9341_SetCursor(uint16_t xpos, uint16_t ypos);                     // 设置光标
void ili9341_SetArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); // 设置显示区域

void ili9341_WriteReg(uint8_t);
void ili9341_WR_DATA(uint8_t);
void ili9341_WriteDevReg(uint8_t ili9341_Reg, uint8_t ili9341_RegValue);

void ili9341_write_ram_prepare(void);
void ili9341_WriteRam(uint16_t RGB_Code);
void ili9341_display_dir(screen_show_dir show_dir); // 设置屏幕显示方向

void ili9341_FillFrame(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_DrawRect(uint16_t _usX,
                  uint16_t _usY,
                  uint16_t _usHeight,
                  uint16_t _usWidth,
                  uint16_t _usColor);                                                     // 绘制水平放置的矩形
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor); // 绘制一个圆，笔宽为1个像素

void app_spi_init_pin(void);
void app_spi_master_init_config(void);
uint32_t LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t len, uint8_t size, uint8_t *p);
void LCD_DrawPicture(uint16_t StartX, uint16_t StartY, uint16_t Xend, uint16_t Yend, uint8_t *pic);
#endif /* __BSP_ili9341_4LINE_H__ */
