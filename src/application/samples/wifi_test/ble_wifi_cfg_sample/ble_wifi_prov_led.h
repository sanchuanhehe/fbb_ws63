/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning LED status indication module.
 *              Drives a single GPIO to show provisioning state visually.
 */

#ifndef BLE_WIFI_PROV_LED_H
#define BLE_WIFI_PROV_LED_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief LED indication states.
 */
typedef enum {
    PROV_LED_OFF = 0,           /**< LED off (NV-configured / idle)              */
    PROV_LED_FAST_BLINK,        /**< 200 ms fast blink (BLE advertising)          */
    PROV_LED_SLOW_BLINK,        /**< 800 ms slow blink (WiFi connecting)          */
    PROV_LED_ON,                /**< Solid on (provisioning success)              */
    PROV_LED_ERROR_FLASH,       /**< 3 fast blinks then off (provisioning failed) */
} prov_led_state_t;

/**
 * @brief Initialise the LED indication task.
 * @param pin  GPIO pin number to drive the LED (active-high assumed).
 *             Pass 0 to disable LED indication entirely.
 */
void ble_wifi_prov_led_init(uint8_t pin);

/**
 * @brief Set the current provisioning LED state.
 *        Thread-safe: safe to call from any task context.
 * @param state  Desired LED state.
 */
void ble_wifi_prov_led_set_state(prov_led_state_t state);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* BLE_WIFI_PROV_LED_H */
