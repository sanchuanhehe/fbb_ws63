/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Button Sample Source. \n
 *
 */

/**
 * 本例程通过配置GPIO来实现按键功能，只需将 BUTTON_GPIO 配置成按键的IO口即可。
 */

#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

#define BUTTON_DURATION_MS 500
#define BUTTON_TASK_PRIO 24
#define BUTTON_TASK_STACK_SIZE 0x1000
#define BUTTON_GPIO GPIO_12

void NetCfgCallbackFunc(pin_t pin, uintptr_t param)
{
    unused(param);
    osal_printk("PIN:%d interrupt success.\r\n", pin);
}

static int button_task(const char *arg)
{
    unused(arg);

    uapi_pin_init();
    uapi_gpio_init();

    // 设置复用模式
    uapi_pin_set_mode(BUTTON_GPIO, 0);
    // 设置输入输出方向
    uapi_gpio_set_dir(BUTTON_GPIO, GPIO_DIRECTION_INPUT);
    // 注册gpio中断，上升沿或下降沿触发
    if (uapi_gpio_register_isr_func(BUTTON_GPIO, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)NetCfgCallbackFunc) !=
        ERRCODE_SUCC) {
        uapi_gpio_unregister_isr_func(BUTTON_GPIO);
        return ERRCODE_FAIL;
    }

    uapi_gpio_enable_interrupt(BUTTON_GPIO);

    return 0;
}

static void button_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)button_task, 0, "BUTTONTask", BUTTON_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BUTTON_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the button_entry. */
app_run(button_entry);