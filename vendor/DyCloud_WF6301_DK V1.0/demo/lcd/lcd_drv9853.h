/*
 * @Description:
 * @Author: Condi
 * @Date: 2025-03-25 13:35:16
 * @FilePath: \src\application\samples\peripheral\lcd\lcd_drv9853.h
 * @LastEditTime: 2025-03-25 17:46:48
 * @LastEditors: Condi
 */
#ifndef LCD_DRV9853_H
#define LCD_DRV9853_H

#include "stdio.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t __attribute__((aligned(4))) LCD_SPI_JD9853_CMDLIST[];
extern const uint16_t LCD_SPI_JD9853_CMDLIST_LENGTH;

#ifdef __cplusplus
}
/*extern "C"*/
#endif
#endif