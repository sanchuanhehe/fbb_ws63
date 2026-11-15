/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning LED status indication implementation.
 *              Spawns a low-priority task that drives a GPIO according to
 *              the current provisioning state.
 */

#include <stdint.h>
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "ble_wifi_prov_led.h"

#define PROV_LED_TAG "[PROV_LED]"
#define PROV_LED_TASK_PRIO 30     /* Low priority — cosmetic only */
#define PROV_LED_STACK_SIZE 0x400 /* 1 KB stack */

/* Blink timing (ms) */
#define FAST_BLINK_MS 200
#define SLOW_BLINK_MS 800
#define ERROR_BLINK_MS 150
#define ERROR_BLINK_CNT 6 /* 3 on-off cycles = 6 toggles */
#define LED_STEADY_POLL_MS 500
#define LED_DEFAULT_POLL_MS 200
#define LED_TOGGLE_DIVISOR 2

/* LED active level: 1 = active-high, 0 = active-low */
#define LED_ACTIVE_HIGH 1

static volatile prov_led_state_t g_led_state = PROV_LED_OFF;
static uint8_t g_led_pin = 0;
static bool g_led_enabled = false;

static void led_gpio_set(bool on)
{
#if LED_ACTIVE_HIGH
    uapi_gpio_set_val(g_led_pin, on ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
#else
    uapi_gpio_set_val(g_led_pin, on ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
#endif
}

static void led_error_flash(void)
{
    for (int i = 0; i < ERROR_BLINK_CNT; i++) {
        led_gpio_set((bool)(i % LED_TOGGLE_DIVISOR == 0)); /* toggle on/off */
        osal_msleep(ERROR_BLINK_MS);
    }
    led_gpio_set(false); /* end with LED off */
}

static int prov_led_task(const char *arg)
{
    bool toggle = false;

    unused(arg);
    osal_printk("%s task started, pin=%u\r\n", PROV_LED_TAG, g_led_pin);

    while (1) {
        switch (g_led_state) {
            case PROV_LED_OFF:
                led_gpio_set(false);
                osal_msleep(LED_STEADY_POLL_MS);
                break;

            case PROV_LED_ON:
                led_gpio_set(true);
                osal_msleep(LED_STEADY_POLL_MS);
                break;

            case PROV_LED_FAST_BLINK:
                toggle = !toggle;
                led_gpio_set(toggle);
                osal_msleep(FAST_BLINK_MS);
                break;

            case PROV_LED_SLOW_BLINK:
                toggle = !toggle;
                led_gpio_set(toggle);
                osal_msleep(SLOW_BLINK_MS);
                break;

            case PROV_LED_ERROR_FLASH:
                led_error_flash();
                g_led_state = PROV_LED_OFF; /* auto-reset to off after flash */
                break;

            default:
                osal_msleep(LED_DEFAULT_POLL_MS);
                break;
        }
    }
    return 0;
}

void ble_wifi_prov_led_init(uint8_t pin)
{
    if (pin == 0) {
        osal_printk("%s disabled (pin=0)\r\n", PROV_LED_TAG);
        return;
    }

    g_led_pin = pin;
    g_led_enabled = true;

    /* Configure pin as GPIO output */
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    led_gpio_set(false); /* start with LED off */

    /* Spawn LED control task */
    osal_kthread_lock();
    osal_task *task = osal_kthread_create((osal_kthread_handler)prov_led_task, NULL, "prov_led", PROV_LED_STACK_SIZE);
    if (task != NULL) {
        osal_kthread_set_priority(task, PROV_LED_TASK_PRIO);
        osal_kfree(task);
    }
    osal_kthread_unlock();

    osal_printk("%s init ok, pin=%u\r\n", PROV_LED_TAG, pin);
}

void ble_wifi_prov_led_set_state(prov_led_state_t state)
{
    if (g_led_enabled) {
        g_led_state = state;
    }
}
