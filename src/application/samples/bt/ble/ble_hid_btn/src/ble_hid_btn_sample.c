/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE HID button sample. A physical button sends one keyboard keycode.
 */

#include <stdbool.h>
#include <stdint.h>
#include "pinctrl.h"
#include "gpio.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "app_init.h"
#include "../inc/ble_hid_btn.h"
#include "../inc/ble_hid_adv.h"

#define BLE_HID_SAMPLE_TAG "[ble_hid_btn_sample]"

#ifndef CONFIG_BLE_HID_BTN_PIN
#define CONFIG_BLE_HID_BTN_PIN 13
#endif
#ifndef CONFIG_BLE_HID_BTN_KEYCODE
#define CONFIG_BLE_HID_BTN_KEYCODE 0x4E
#endif
#ifndef CONFIG_BLE_HID_BTN_LONGPRESS
#define CONFIG_BLE_HID_BTN_LONGPRESS 1
#endif

#define POLL_INTERVAL_MS 20
#define DEBOUNCE_CNT 2
#define LONGPRESS_MS 500
#define HID_KEY_RELEASE_INTERVAL_MS 10
#define BLE_STACK_READY_DELAY_MS 3000
#define HID_MAIN_TASK_STACK 0x1000
#define HID_MAIN_TASK_PRIO 26
#define HID_BTN_TASK_STACK 0x400
#define HID_BTN_TASK_PRIO 30

static const hid_kb_report_t REPORT_RELEASE = {0, 0, {0, 0, 0, 0, 0, 0}};

typedef struct {
    uint8_t pin;
    uint8_t keycode;
    bool pressed;
    bool last_stable;
    uint8_t debounce;
    uint32_t press_start_tick;
    bool sent_repeat;
} hid_button_state_t;

static void hid_button_update_level(hid_button_state_t *state)
{
    bool raw = (uapi_gpio_get_val(state->pin) == GPIO_LEVEL_LOW);
    if (raw == state->last_stable) {
        state->debounce = 0;
        return;
    }
    state->debounce++;
    if (state->debounce >= DEBOUNCE_CNT) {
        state->last_stable = raw;
        state->debounce = 0;
    }
}

static void hid_button_send_key(uint8_t keycode)
{
    hid_kb_report_t report = {0};
    report.keys[0] = keycode;
    (void)ble_hid_btn_send_report(&report);
}

static void hid_button_handle_state(hid_button_state_t *state)
{
    if (state->last_stable && !state->pressed) {
        state->pressed = true;
        state->press_start_tick = (uint32_t)osKernelGetTickCount();
        state->sent_repeat = false;
        hid_button_send_key(state->keycode);
        osal_printk("%s press key=0x%02X\r\n", BLE_HID_SAMPLE_TAG, state->keycode);
        return;
    }
#if CONFIG_BLE_HID_BTN_LONGPRESS
    uint32_t elapsed = (uint32_t)osKernelGetTickCount() - state->press_start_tick;
    if (state->last_stable && state->pressed && (elapsed >= LONGPRESS_MS) && !state->sent_repeat) {
        (void)ble_hid_btn_send_report(&REPORT_RELEASE);
        osal_msleep(HID_KEY_RELEASE_INTERVAL_MS);
        hid_button_send_key(state->keycode);
        state->sent_repeat = true;
        osal_printk("%s repeat key=0x%02X\r\n", BLE_HID_SAMPLE_TAG, state->keycode);
        return;
    }
#endif
    if (!state->last_stable && state->pressed) {
        state->pressed = false;
        (void)ble_hid_btn_send_report(&REPORT_RELEASE);
        osal_printk("%s release\r\n", BLE_HID_SAMPLE_TAG);
    }
}

static int hid_btn_task(const char *arg)
{
    hid_button_state_t state = {
        .pin = (uint8_t)CONFIG_BLE_HID_BTN_PIN,
        .keycode = (uint8_t)CONFIG_BLE_HID_BTN_KEYCODE,
    };
    unused(arg);

    uapi_pin_set_mode(state.pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(state.pin, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(state.pin, PIN_PULL_TYPE_UP);
    osal_printk("%s task started, pin=%u keycode=0x%02X\r\n", BLE_HID_SAMPLE_TAG, state.pin, state.keycode);

    while (1) {
        osal_msleep(POLL_INTERVAL_MS);
        hid_button_update_level(&state);
        hid_button_handle_state(&state);
    }
    return 0;
}

static int hid_main_task(const char *arg)
{
    unused(arg);
    osal_printk("%s main task start\r\n", BLE_HID_SAMPLE_TAG);
    osal_msleep(BLE_STACK_READY_DELAY_MS);
    if (ble_hid_btn_init() != ERRCODE_SUCC) {
        osal_printk("%s HID init fail\r\n", BLE_HID_SAMPLE_TAG);
        return -1;
    }
    ble_hid_adv_start();

    osal_kthread_lock();
    osal_task *btn = osal_kthread_create((osal_kthread_handler)hid_btn_task, NULL, "hid_btn", HID_BTN_TASK_STACK);
    if (btn != NULL) {
        osal_kthread_set_priority(btn, HID_BTN_TASK_PRIO);
        osal_kfree(btn);
    }
    osal_kthread_unlock();
    osal_printk("%s ready\r\n", BLE_HID_SAMPLE_TAG);
    return 0;
}

static void ble_hid_btn_sample_entry(void)
{
    osal_printk("%s entry\r\n", BLE_HID_SAMPLE_TAG);
    osal_kthread_lock();
    osal_task *task = osal_kthread_create((osal_kthread_handler)hid_main_task, NULL, "hid_main", HID_MAIN_TASK_STACK);
    if (task != NULL) {
        osal_kthread_set_priority(task, HID_MAIN_TASK_PRIO);
        osal_kfree(task);
    }
    osal_kthread_unlock();
}

app_run(ble_hid_btn_sample_entry);
