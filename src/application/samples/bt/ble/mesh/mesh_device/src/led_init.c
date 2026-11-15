 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: led control sample. \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
 
#include "boards.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "led_init.h"

uint8_t g_mgpio_pin_r = S_MGPIO7;
uint8_t g_mgpio_pin_b = S_MGPIO8;
uint8_t g_mgpio_pin_g = S_MGPIO9;

void set_led_gpio(char* color, uint8_t gpio_num)
{
    if ((strcmp(color, "r") == 0) || (strcmp(color, "R") == 0)) {
        g_mgpio_pin_r = gpio_num;
    } else if ((strcmp(color, "b") == 0) || (strcmp(color, "B") == 0)) {
        g_mgpio_pin_b = gpio_num;
    } else if ((strcmp(color, "g") == 0) || (strcmp(color, "G") == 0)) {
        g_mgpio_pin_g = gpio_num;
    }
}

mesh_led_bool_t read_led_state(pin_t led_pin)
{
    osal_printk("LED read_led_state(led_pin = %d)\n", led_pin);
    return (uapi_gpio_get_output_val(led_pin) > 0) ? MESH_LED_OFF : MESH_LED_ON;
}

void set_led_state(mesh_led_bool_t on)
{
    static bool init_led_flag = true;
    if (init_led_flag) {
        init_led_flag = false;
        uapi_gpio_set_dir(g_mgpio_pin_r, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_dir(g_mgpio_pin_b, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_dir(g_mgpio_pin_g, GPIO_DIRECTION_OUTPUT);
    }
    osal_printk("LED set_led_state(on = %d)\n", on);
    if (on == 0) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
    } else {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
    }
}

void set_led_level(int16_t valve)
{
    osal_printk("LED level: %d\n", valve);
    if (valve <= -MAX_LEVEL2) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > -MAX_LEVEL2) && (valve <= -MAX_LEVEL1)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > -MAX_LEVEL1) && (valve <= -MAX_LEVEL0)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > -MAX_LEVEL0) && (valve <= 0)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > 0) && (valve <= MAX_LEVEL0)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > MAX_LEVEL0) && (valve <= MAX_LEVEL1)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > MAX_LEVEL1) && (valve <= MAX_LEVEL2)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if (valve >= MAX_LEVEL3) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    }
}

void set_led_light(uint16_t valve)
{
    osal_printk("LED light: %d\n", valve);
    if (valve < MAX_LEVEL0) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > MAX_LEVEL0) && (valve <= MAX_LEVEL1)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > MAX_LEVEL1) && (valve <= MAX_LEVEL2)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if ((valve > MAX_LEVEL2) && (valve <= MAX_LEVEL3)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > MAX_LEVEL3) && (valve <= MAX_LEVEL4)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > MAX_LEVEL4) && (valve <= MAX_LEVEL5)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_HIGH);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    } else if ((valve > MAX_LEVEL5) && (valve <= MAX_LEVEL6)) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_HIGH);
    } else if (valve >= MAX_LEVEL6) {
        uapi_gpio_set_val(g_mgpio_pin_r, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_b, GPIO_LEVEL_LOW);
        uapi_gpio_set_val(g_mgpio_pin_g, GPIO_LEVEL_LOW);
    }
}