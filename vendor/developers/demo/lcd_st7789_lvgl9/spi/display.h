/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "st7789.h"
#include "timer.h"
#include "chip_core_irq.h"
#include "lvgl.h"

#define TIMER_INDEX 1
#define TIMER_PRIORITY 1
#define TICK_US 1000

#define MY_DISP_HOR_RES ST7789_WIDTH
#define MY_DISP_VER_RES ST7789_HEIGHT
#define BYTE_PER_PIXEL 2
#define SPI_TASK_DURATION_MS 33
#define SPI_TASK_PRIORITY 24
#define SPI_TASK_STACK_SIZE 0x1000

#endif
