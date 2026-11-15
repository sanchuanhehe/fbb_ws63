/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Create Date : 2026.05
 * Description: led_ctrl.c
 */

#include "osal_debug.h"
#include "gpio.h"

/* GPIO0控制RED Color */
uint8_t g_sle_mgpio_pin_r = S_MGPIO0;
/* GPIO0控制BLUE Color */
uint8_t g_sle_mgpio_pin_b = S_MGPIO1;
/* GPIO0控制GREEN Color */
uint8_t g_sle_mgpio_pin_g = S_MGPIO2;

void led_init(void)
{
    /* 启动灯控demo任务 */
    osal_printk("[Demo] led control demo start.\r\n");
    uapi_gpio_set_dir(g_sle_mgpio_pin_r, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(g_sle_mgpio_pin_b, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(g_sle_mgpio_pin_g, GPIO_DIRECTION_OUTPUT);
}


void sample_light_switch(uint32_t on)
{
    osal_printk("[Demo] light value %u", on);
    if (on == 1) {
        uapi_gpio_set_val(g_sle_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_sle_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_sle_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(g_sle_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_sle_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_sle_mgpio_pin_g, GPIO_LEVEL_HIGH);
    }
}
