#include "app_init.h"
#include "systick.h"
#include "tcxo.h"
#include "los_memory.h"
#include "securec.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "gpio.h"
#include "pinctrl.h"
#include "osal_addr.h"
#define LED_GPIO 2
#define GPIO_MODE PIN_MODE_0 // 普通GPIO模式

void GPIO_LED_task(void)
{   
    uapi_gpio_init();
    uapi_pin_set_mode(LED_GPIO, GPIO_MODE);
    uapi_gpio_set_dir(LED_GPIO, GPIO_DIRECTION_OUTPUT);
    while (1)
    {
        uapi_gpio_set_val(LED_GPIO, GPIO_LEVEL_LOW); // 灭灯
        osal_printk("LED_OFF\r\n");
        osal_msleep(1000);
        uapi_gpio_set_val(LED_GPIO, GPIO_LEVEL_HIGH); // 亮灯
        osal_printk("LED_ON\r\n");
        osal_msleep(1000);
    }
}

void task_entry(void)
{
    // 定义任务句柄
    osal_task *GPIO_LED_task_handle = NULL;
    osal_kthread_lock();
    // 创建线程
    GPIO_LED_task_handle = osal_kthread_create((osal_kthread_handler)GPIO_LED_task, 0, "GPIO_LED_task", 0x2000);
    if (GPIO_LED_task_handle != NULL)
    {
        osal_kthread_set_priority(GPIO_LED_task_handle, 24);
        osal_kfree(GPIO_LED_task_handle);
    }
    osal_kthread_unlock();
}

app_run(task_entry);
