/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */

#ifndef _ST7789_H
#define _ST7789_H

#include "spi_driver.h"

#define ST7789_NOP 0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID 0x04
#define ST7789_RDDST 0x09

#define ST7789_SLPIN 0x10
#define ST7789_SLPOUT 0x11
#define ST7789_PTLON 0x12
#define ST7789_NORON 0x13

#define ST7789_INVOFF 0x20
#define ST7789_INVON 0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON 0x29
#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_PTLAR 0x30
#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY 0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX 0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV 0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML 0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00

#define ST7789_COLOR_MODE_16bit 0x55 //  RGB565 (16bit)

#define HIGH_BYTE_SHIFT 8
#define LOW_BYTE_MASK 0xFF

#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320

void st7789_init(void);
void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void st7789_write_data(uint8_t *data, uint32_t len);

#endif
