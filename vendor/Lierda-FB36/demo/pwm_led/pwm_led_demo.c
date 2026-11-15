/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Pwm led demo source. \n
 *
 * History: \n
 */

#include "app_init.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "pwm.h"

// Configuration macros
#define LED_TASK_PRIO          24       // Priority level for the LED task
#define LED_TASK_STACK_SIZE    0x1000   // Stack size allocated for the LED task

#define PWM_HIGH_TIME   70      // pwm high time
#define PWM_LOW_TIME    30      // pwm low time
#define PWM_STEP        5       // Duty change step
#define BREATH_DELAY_MS 50     // Delay to control breathing speed
#define PWM_CHANNEL     4       // PWM channel to use
#define PWM_GROUP_ID    0       // PWM group ID

static int pwm_led_task(const char *arg)
{
    (void)arg;  // Explicitly mark unused parameter

    // Initialize duty value and step size
    int duty = PWM_HIGH_TIME;
    int step = PWM_STEP;
    int t = PWM_HIGH_TIME + PWM_LOW_TIME;
    uint8_t channel_id = PWM_CHANNEL;
    
    // Initialize PWM configuration structure
    pwm_config_t pwm_cfg = {
        .low_time = PWM_LOW_TIME,       // Low level time
        .high_time = PWM_HIGH_TIME,     // High level time
        .offset_time = 0,               // Phase offset
        .cycles = 0,                    // Cycle count (0 means infinite)
        .repeat = true                  // Repeat mode
    };

    // Configure LED pin for PWM mode
    errcode_t ret = uapi_pin_set_mode(CONFIG_LED_PIN, PIN_MODE_1);
    if (ERRCODE_SUCC != ret) {
        osal_printk("PWM setup error: Failed to configure pin mode\r\n");
        return -1;
    }

    // Reset and initialize PWM module
    uapi_pwm_deinit();
    ret = uapi_pwm_init();
    if (ERRCODE_SUCC != ret) {
        osal_printk("PWM setup error: Module initialization failed\r\n");
        return -1;
    }

    osal_printk("PWM breathing light started on channel %d\r\n", PWM_CHANNEL);

    // Main loop - implement breathing light effect
    while (1) {
        // Update PWM configuration with current low_tmp

        pwm_cfg.high_time = (duty * t) / t;
        pwm_cfg.low_time = t - pwm_cfg.high_time;
        
        // Open PWM channel with updated configuration
        ret = uapi_pwm_open(PWM_CHANNEL, &pwm_cfg);
        if (ERRCODE_SUCC != ret) {
            osal_printk("PWM operation warning: Channel open failed\r\n");
        }
        
        // Start PWM group output
        uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);   // Set PWM channel group
        uapi_pwm_start_group(PWM_GROUP_ID);                 // Start PWM channel group

        // Update low_tmp value
        duty -= step;

        // Check if low_tmp limits are reached and reverse direction
        if (duty >= PWM_HIGH_TIME || duty <= 0) {
            step = -step;
        }

        // Add delay to control breathing speed
        osal_msleep(BREATH_DELAY_MS);

        // Close PWM channel before reconfiguring
        uapi_pwm_close(PWM_GROUP_ID);
    }

    // Cleanup resources (theoretically unreachable)
    uapi_pwm_deinit();

    return 0;
}

/**
 * @brief Creates and configures the Pwmled task
 */
static void pwm_led_entry(void)
{
    osal_task *task_handle = NULL;

    // Protect critical section during thread creation and create kernel thread for LED control
    osal_kthread_lock();

    task_handle = osal_kthread_create((osal_kthread_handler)pwm_led_task, 0, "PwmledTask", LED_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, LED_TASK_PRIO); // Set task priority
        osal_kfree(task_handle);
    } else {
        osal_printk("ERROR: Failed to create PwmledTask\r\n");
    }

    osal_kthread_unlock();
}

/* Application entry point - Run pwm_led_entry */
app_run(pwm_led_entry);