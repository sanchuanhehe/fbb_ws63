/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-06-13, Create file. \n
 */

#include "w33_board_port.h"
#include "tiot_board_pin_port.h"
#include "gpio.h"
#include "uart.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "w33_board_port_config.h"

static const uint8_t g_w33_pin_dirs[W33_PIN_NUM] = {
    GPIO_DIRECTION_OUTPUT, GPIO_DIRECTION_OUTPUT, GPIO_DIRECTION_INPUT
};

#if defined(CONFIG_TIOT_PORTING_AIR_MOUSE)
static void w33_board_set_power_enable(void)
{
    if (W33_PIN_POWER_EN == TIOT_PIN_NONE) {
        return;
    }
    (void)uapi_pin_set_mode((pin_t)W33_PIN_POWER_EN, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    (void)uapi_gpio_set_dir((pin_t)W33_PIN_POWER_EN, GPIO_DIRECTION_OUTPUT);
    (void)uapi_gpio_set_val((pin_t)W33_PIN_POWER_EN, GPIO_LEVEL_HIGH);
    (void)uapi_pin_set_pull((pin_t)W33_PIN_POWER_EN, PIN_PULL_UP);
    osal_mdelay(W33_PWEN_TON_WAIT_MS);
}

static void w33_board_set_power_disable(void)
{
    if (W33_PIN_POWER_EN == TIOT_PIN_NONE) {
        return;
    }
    (void)uapi_gpio_set_val((pin_t)W33_PIN_POWER_EN, GPIO_LEVEL_LOW);
    (void)uapi_pin_set_pull((pin_t)W33_PIN_POWER_EN, PIN_PULL_NONE);
    osal_mdelay(W33_PWEN_TOFF_WAIT_MS);
}
#endif

int32_t w33_board_init(void *param)
{
    tiot_unused(param);

    w33_board_hw_info *hw_info = g_w33_board_info.hw_infos;
    const uint32_t *w33_pins = hw_info->pm_info;
    uint8_t pin_dir;
    uint32_t pin_num;
#if defined(CONFIG_TIOT_PORTING_AIR_MOUSE)
    w33_board_set_power_enable();
#endif

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    /* bs2x ie默认关闭，输入管脚使能时需要打开 */
    (void)uapi_pin_set_ie(w33_pins[W33_PIN_DEVICE_WAKEUP_HOST], PIN_IE_1);
#endif

    /* 设置GPIO 初始pinmux. */
    for (uint8_t i = 0; i < W33_PIN_NUM; i++) {
        pin_num = w33_pins[i];
        if (pin_num == TIOT_PIN_NONE) {
            continue;
        }
        pin_dir = g_w33_pin_dirs[i];
        (void)uapi_pin_set_mode((pin_t)pin_num, (pin_mode_t)HAL_PIO_FUNC_GPIO);
        (void)uapi_pin_set_pull((pin_t)pin_num, PIN_PULL_DOWN);
        /* 输出设置drvie strenth. */
        if (pin_dir == GPIO_DIRECTION_OUTPUT) {
            (void)uapi_pin_set_ds((pin_t)pin_num, (pin_drive_strength_t)(PIN_DS_MAX - 1));
        }
        (void)uapi_gpio_set_dir((pin_t)pin_num, pin_dir);
        (void)uapi_gpio_set_val((pin_t)pin_num, GPIO_LEVEL_LOW);
    }
    /* UART pinmux已经在板级完成初始化, 或在UART open时进行初始化。 */
    return 0;
}

void w33_board_deinit(void *param)
{
    tiot_unused(param);

    w33_board_hw_info *hw_info = g_w33_board_info.hw_infos;
    const uint32_t *w33_pins = hw_info->pm_info;
    /* 确保PWREN管脚下拉 */
    (void)uapi_gpio_set_val((pin_t)w33_pins[W33_PIN_POWER_CTRL], GPIO_LEVEL_LOW);
    (void)uapi_pin_set_pull((pin_t)w33_pins[W33_PIN_POWER_CTRL], PIN_PULL_NONE);

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    /* 下电后输入管脚关闭ie */
    (void)uapi_pin_set_ie((pin_t)w33_pins[W33_PIN_DEVICE_WAKEUP_HOST], PIN_IE_0);
#endif
#if defined(CONFIG_TIOT_PORTING_AIR_MOUSE)
    osal_mdelay(W33_PWO_TOFF_WAIT_MS);
    w33_board_set_power_disable();
#endif
}

const w33_board_info *w33_board_get_info(void)
{
    return &g_w33_board_info;
}
