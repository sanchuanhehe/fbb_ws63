/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning — button module for clearing NV credentials.
 *              Long-press (3 seconds) on the configured GPIO triggers a factory
 *              reset of provisioning data, then reboots the chip.
 */

#ifndef BLE_WIFI_PROV_BTN_H
#define BLE_WIFI_PROV_BTN_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Initialise the button-detection task.
 * @param pin  GPIO pin number (active-low, internal pull-up assumed).
 *             Pass 0 to disable button monitoring.
 */
void ble_wifi_prov_btn_init(uint8_t pin);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* BLE_WIFI_PROV_BTN_H */
