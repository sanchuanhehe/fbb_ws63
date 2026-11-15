/**！
 * @file button.c
 * @brief Onboard button interrupt example: Pressing the button toggles the onboard LED on or off.
 * @copyright  Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "watchdog.h"
#include "app_init.h"

#define BSP_LED 2      // BLUE
#define BUTTON_GPIO 12 // 按键

#define BUTTON_TASK_STACK_SIZE 0x1000
#define BUTTON_TASK_PRIO 17

static int g_led_state = 0;

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    g_led_state = !g_led_state;
    printf("Button pressed.\r\n");
}

static void *button_task(const char *arg)
{
    unused(arg);
    uapi_pin_set_mode(BSP_LED, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_LED, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_LOW);

    uapi_pin_set_mode(BUTTON_GPIO, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BUTTON_GPIO, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(BUTTON_GPIO, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func);
    if (ret != 0) {
        uapi_gpio_unregister_isr_func(BUTTON_GPIO);
    }
    while (1) {
        uapi_watchdog_kick(); // 喂狗，防止程序出现异常系统挂死
        if (g_led_state) {
            uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_HIGH);
        } else {
            uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_LOW);
        }
    }
    return NULL;
}

static void button_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)button_task, NULL, "button_task", BUTTON_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, BUTTON_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(button_entry);