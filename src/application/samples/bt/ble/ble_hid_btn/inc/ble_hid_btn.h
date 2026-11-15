/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE HID Button — HID keyboard service API.
 *              Boot Keyboard mode (Protocol Mode = 0).
 */

#ifndef BLE_HID_BTN_H
#define BLE_HID_BTN_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* HID keyboard input report (fixed 8 bytes, Boot mode) */
typedef struct __attribute__((packed)) {
    uint8_t modifiers;          /* bitmask: LCTRL,LSHIFT,LALT,... */
    uint8_t reserved;           /* always 0x00 */
    uint8_t keys[6];            /* up to 6 simultaneous keycodes */
} hid_kb_report_t;

/**
 * @brief Initialize HID keyboard GATT service and start advertising.
 */
errcode_t ble_hid_btn_init(void);

/**
 * @brief Send a keyboard input report via BLE Notify.
 * @param report  Pointer to 8-byte keyboard report.
 * @return ERRCODE_SUCC on success.
 */
errcode_t ble_hid_btn_send_report(const hid_kb_report_t *report);

/**
 * @brief Check whether the HID host (PC) is currently connected.
 */
bool ble_hid_btn_is_connected(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* BLE_HID_BTN_H */
