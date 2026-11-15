/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Key demo source. \n
 *
 * History: \n
 */

#include "app_init.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_gpio.h"

// Configuration macros
#define KEY_TASK_PRIO          24    // Priority level for the KEY task
#define KEY_TASK_STACK_SIZE    0x1000  // Stack size allocated for the KEY task


static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(param);
    osal_printk("Key(pin%d) is pressed.\r\n", pin);
}

/**
 * @brief Key control task function
 * @param arg - Task argument (unused)
 * @return int - 0 success; -1 error
 */
static int key_task(const char *arg)
{
    unused(arg);    // Explicitly mark unused parameter

    // Configure key pin mode
    errcode_t ret = uapi_pin_set_mode(CONFIG_KEY_PIN, PIN_MODE_0);
    if (ERRCODE_SUCC != ret) {
        osal_printk("ERROR: Failed to set pin mode for key\r\n");
        return -1;
    }

    // Configure key pin direction input
    ret = uapi_gpio_set_dir(CONFIG_KEY_PIN, GPIO_DIRECTION_INPUT);
    if (ERRCODE_SUCC != ret) {
        osal_printk("ERROR: Failed to set GPIO direction\r\n");
        return -1;
    }

    // Register KEY interrupt for both rising and falling edges
    ret = uapi_gpio_register_isr_func(CONFIG_KEY_PIN, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)gpio_callback_func);
    if (ERRCODE_SUCC != ret) {
        // Cleanup any potentially registered interrupt on failure
        uapi_gpio_unregister_isr_func(CONFIG_KEY_PIN);
        osal_printk("ERROR: Failed to register gpio isr\r\n");
        return -1;
    }

    // Enable interrupts for the configured pin
    ret = uapi_gpio_enable_interrupt(CONFIG_KEY_PIN);
    if (ERRCODE_SUCC != ret) {
        uapi_gpio_unregister_isr_func(CONFIG_KEY_PIN);
        osal_printk("Error: Failed to enable GPIO interrupts\r\n");
        return -1;
    }
    osal_printk("Key detection task is now active. Press key to test.\r\n");

    return 0;
}

/**
 * @brief Creates and configures the Key task
 */
static void key_entry(void)
{
    osal_task *task_handle = NULL;

    // Protect critical section during thread creation and create kernel thread for Key control
    osal_kthread_lock();

    task_handle = osal_kthread_create((osal_kthread_handler)key_task, 0, "KeyTask", KEY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, KEY_TASK_PRIO); // Set task priority
        osal_kfree(task_handle);
    } else {
        osal_printk("ERROR: Failed to create KeyTask\r\n");
    }

    osal_kthread_unlock();
}

/* Application entry point - Run key_entry */
app_run(key_entry);