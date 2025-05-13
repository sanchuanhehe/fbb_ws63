/**
 *    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
 */

#include "pinctrl.h"
#include "watchdog.h"
#include "tcxo.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog.h"
#include "ws2812b.h"

uint32_t preg_placeholder = 0;

static __inline__ void one_num(void)
{
    uapi_reg_setbit(0x44028030, 5); // 0x44028030代表GPIO电平拉高寄存器，5代表GPIO5，将gpio5拉高,可以参考platform_core.h
    for (int i = 0; i < 20; i++) { // 重复20次，约1us
        uapi_reg_read32(0x44028030, preg_placeholder);
    }
    uapi_reg_setbit(0x44028034, 5); // 0x44028034代表GPIO电平拉低寄存器，5代表GPIO5，将gpio5拉低,可以参考platform_core.h
    for (int i = 0; i < 4; i++) { // 重复4次，约250ns
        uapi_reg_read32(0x44028030, preg_placeholder);
    }
}

static __inline__ void zero_num(void)
{
    uapi_reg_setbit(0x44028030, 5); // 5代表GPIO电平拉高寄存器
    for (int i = 0; i < 4; i++) {   // 重复4次，约250ns
        uapi_reg_read32(0x44028030, preg_placeholder);
    }
    uapi_reg_setbit(0x44028034, 5); // 5代表GPIO电平拉低重复20次，约1us
    for (int i = 0; i < 18; i++) {  // 18代表重复次数，计算时间
        uapi_reg_read32(0x44028030, preg_placeholder);
    }
}

void rgb_display(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    red = ((uint32_t)red) * brightness / 255;     // 255计算相对亮度
    green = ((uint32_t)green) * brightness / 255; // 255计算相对亮度
    blue = ((uint32_t)blue) * brightness / 255;   // 255计算相对亮度
    for (int i = 0; i < 8; i++) { // 每个颜色8位
        if (red & 0x80)
            one_num(); // 取最高位
        else
            zero_num();
        red <<= 1;
    }
    for (int i = 0; i < 8; i++) { // 每个颜色8位
        if (green & 0x80)
            one_num(); // 取最高位
        else
            zero_num();
        green <<= 1;
    }
    for (int i = 0; i < 8; i++) { // 每个颜色8位
        if (blue & 0x80)
            one_num(); // 取最高位
        else
            zero_num();
        blue <<= 1;
    }
}