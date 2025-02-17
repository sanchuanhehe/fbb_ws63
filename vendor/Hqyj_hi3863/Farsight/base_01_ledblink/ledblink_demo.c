#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "platform_core_rom.h"

#define BLINK_TASK_PRIO          24
#define BLINK_TASK_STACK_SIZE    0x1000

static int blink_task(const char *arg)
{
    unused(arg);

    uapi_pin_set_mode(GPIO_10, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(GPIO_13, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(GPIO_10, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_10, GPIO_LEVEL_LOW);

    uapi_gpio_set_dir(GPIO_13, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_HIGH);
    while (1) {
        osal_msleep(500);
        uapi_gpio_toggle(GPIO_10);
        uapi_gpio_toggle(GPIO_13);
        osal_printk("gpio blink.\r\n");
    }

    return 0;
}

static void blink_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)blink_task, 0, "blinkTask", BLINK_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLINK_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the sample. */
app_run(blink_entry);