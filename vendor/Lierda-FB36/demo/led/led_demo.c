/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: LED demo source. \n
 *
 * History: \n
 */

#include "app_init.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_gpio.h"

// Configuration macros
#define LED_DURATION_MS        500  // LED toggle interval in milliseconds
#define LED_TASK_PRIO          24    // Priority level for the LED task
#define LED_TASK_STACK_SIZE    0x1000  // Stack size allocated for the LED task

/**
 * @brief LED control task function
 * @param arg - Task argument (unused)
 * @return int - 0 success; -1 error
 */
static int led_task(const char *arg)
{
    unused(arg);    // Explicitly mark unused parameter

    // Configure LED pin mode
    errcode_t ret = uapi_pin_set_mode(CONFIG_LED_PIN, PIN_MODE_2);
    if (ERRCODE_SUCC != ret) {
        osal_printk("ERROR: Failed to set pin mode for LED\r\n");
        return -1;
    }

    // Configure LED pin direction output
    ret = uapi_gpio_set_dir(CONFIG_LED_PIN, GPIO_DIRECTION_OUTPUT);
    if (ERRCODE_SUCC != ret) {
        osal_printk("ERROR: Failed to set GPIO direction\r\n");
        return -1;
    }

    // Initialize LED pin to low level
    ret = uapi_gpio_set_val(CONFIG_LED_PIN, GPIO_LEVEL_LOW);
    if (ERRCODE_SUCC != ret) {
        osal_printk("ERROR: Failed to set initial GPIO value\r\n");
        return -1;
    }

    // Main task loop
    while (1) {
        osal_msleep(LED_DURATION_MS);

        uapi_gpio_toggle(CONFIG_LED_PIN);
        osal_printk("~LED toggled~\r\n");
    }

    return 0;
}

/**
 * @brief Creates and configures the LED task
 */
static void led_entry(void)
{
    osal_task *task_handle = NULL;

    // Protect critical section during thread creation and create kernel thread for LED control
    osal_kthread_lock();

    task_handle = osal_kthread_create((osal_kthread_handler)led_task, 0, "LedTask", LED_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, LED_TASK_PRIO); // Set task priority
        osal_kfree(task_handle);
    } else {
        osal_printk("ERROR: Failed to create LedTask\r\n");
    }

    osal_kthread_unlock();
}

/* Application entry point - Run led_entry */
app_run(led_entry);