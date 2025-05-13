/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#ifndef WS2812B_H
#define WS2812B_H

#include "pinctrl.h"
#include "watchdog.h"
#include "tcxo.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog.h"

void rgb_display(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);

#endif