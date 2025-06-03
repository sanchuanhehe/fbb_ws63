/**
 * 描述：
 * SSD1306 OLED 驱动程序头文件
 *
 * Project Description:
 * SSD1306 OLED driver Header file for Hispark ws63 development board, supporting I2C communication and basic display functions.
 *
 * Copyright (c) lamonce
 *
 *
 * History: \n
 * 2025-06-03, Edit file. \n
 */

#ifndef SSD1306_OLED_H
#define SSD1306_OLED_H

#include "pinctrl.h"
#include "i2c.h"
#include "soc_osal.h"
#include "app_init.h"

extern const uint8_t SSD1306_OLED_F8x16[95][16];       // 字体库
extern const uint8_t SSD1306_OLED_init_Command[23][2]; // 初始化命令

void SSD1306_OLED_Init(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data);  // 初始化 OLED
void SSD1306_OLED_Clear(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data); // 清屏

void SSD1306_OLED_ShowString(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data, uint8_t line, uint8_t column, char *str);                        // 显示字符串
void SSD1306_OLED_ShowIntNum(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data, uint8_t line, uint8_t column, int num, int len, bool is_signed); // 显示整形数字

#endif // !SSD1306_OLED_H