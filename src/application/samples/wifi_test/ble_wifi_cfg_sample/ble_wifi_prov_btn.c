/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Button module for BLE WiFi provisioning.
 *              Polls a GPIO pin in a low-priority task.  A continuous press
 *              of ~3 seconds triggers NV credential clear + chip reboot.
 */

#include <stdint.h>
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "securec.h"
#include "reboot_porting.h"
#include "ble_wifi_prov_btn.h"
#include "ble_wifi_prov_nv.h"

#define PROV_BTN_TAG "[PROV_BTN]"
#define PROV_BTN_TASK_PRIO 31     /* lowest priority */
#define PROV_BTN_STACK_SIZE 0x300 /* 768 B */

#define POLL_INTERVAL_MS 100 /* poll every 100 ms */
#define LONG_PRESS_COUNT 30  /* 30 × 100 ms = 3 seconds */
#define DEBOUNCE_COUNT 3     /* 3 consecutive reads to confirm state */
#define NV_FLUSH_DELAY_MS 200

static uint8_t g_btn_pin = 0;
static bool g_btn_enabled = false;

static int prov_btn_task(const char *arg)
{
    uint32_t press_count = 0;
    uint8_t stable_cnt = 0;
    bool last_stable = true; /* pulled high = released */

    unused(arg);
    osal_printk("%s task started, pin=%u\r\n", PROV_BTN_TAG, g_btn_pin);

    while (1) {
        osal_msleep(POLL_INTERVAL_MS);

        bool level = (uapi_gpio_get_val(g_btn_pin) == GPIO_LEVEL_LOW);
        /* active-low: LOW = pressed */
        /* Simple debounce: require N consecutive identical reads */
        if (level == last_stable) {
            stable_cnt = 0;
        } else {
            stable_cnt++;
            if (stable_cnt >= DEBOUNCE_COUNT) {
                last_stable = level;
                stable_cnt = 0;
            }
        }

        if (last_stable) {
            /* Button pressed */
            press_count++;
            if (press_count == 1) {
                osal_printk("%s press detected\r\n", PROV_BTN_TAG);
            }
            if (press_count >= LONG_PRESS_COUNT) {
                osal_printk("%s long press (%d ms), clearing NV and reboot...\r\n", PROV_BTN_TAG,
                            LONG_PRESS_COUNT * POLL_INTERVAL_MS);
                ble_wifi_prov_nv_clear();
                osal_msleep(NV_FLUSH_DELAY_MS);
                reboot_port_reboot_chip();
                /* not reached */
            }
        } else {
            /* Button released */
            if (press_count >= LONG_PRESS_COUNT) {
                /* already handled above */
            } else if (press_count > 0) {
                osal_printk("%s released after %d ms (short press ignored)\r\n", PROV_BTN_TAG,
                            press_count * POLL_INTERVAL_MS);
            }
            press_count = 0;
        }
    }
    return 0;
}

void ble_wifi_prov_btn_init(uint8_t pin)
{
    if (pin == 0) {
        osal_printk("%s disabled (pin=0)\r\n", PROV_BTN_TAG);
        return;
    }

    g_btn_pin = pin;
    g_btn_enabled = true;

    /* Configure as input with pull-up */
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    /* Enable internal pull-up — button connects pin to GND when pressed.
     * PIN_PULL_TYPE_UP = 3 (see pinctrl_porting.h) */
    uapi_pin_set_pull(pin, PIN_PULL_TYPE_UP);

    osal_kthread_lock();
    osal_task *task = osal_kthread_create((osal_kthread_handler)prov_btn_task, NULL, "prov_btn", PROV_BTN_STACK_SIZE);
    if (task != NULL) {
        osal_kthread_set_priority(task, PROV_BTN_TASK_PRIO);
        osal_kfree(task);
    }
    osal_kthread_unlock();

    osal_printk("%s init ok, pin=%u (long-press %d ms to clear NV)\r\n", PROV_BTN_TAG, pin,
                LONG_PRESS_COUNT * POLL_INTERVAL_MS);
}
