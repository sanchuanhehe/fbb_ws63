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
#define BUTTON 0
#define GPIO_MODE PIN_MODE_0 // 普通GPIO模式

void BOOT_init(void){
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(BUTTON, GPIO_MODE);
    uapi_gpio_set_dir(BUTTON, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(BUTTON, PIN_PULL_TYPE_UP);     //设置GPIO为上拉模式
}

#if defined CONFIG_GPIO_BUTTON_MODE_POLLING//轮询模式
void GPIO_Button_polling_task(void)
{
    BOOT_init();
    while (1)
    {
        if(uapi_gpio_get_val(BUTTON)==GPIO_LEVEL_LOW){//按键按下时低电平
            osal_msleep(100);//软件消抖
            osal_printk("Button down\r\n");
        }
        osal_msleep(200);
    }
}
#else//中断模式
void gpio_callback_func(pin_t pin, uintptr_t param)//中断回调函数
{
    unused(param);
    osal_printk("PIN:%d interrupt success. \r\n", pin);
}

void GPIO_Button_interrupt_task(void)
{
    BOOT_init();
    /* 注册指定GPIO下降沿中断，回调函数为gpio_callback_func */
    if (uapi_gpio_register_isr_func(BUTTON, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func) != ERRCODE_SUCC) {
        uapi_gpio_unregister_isr_func(BUTTON); /* 清理残留 */
    }
}
#endif


void task_entry(void)
{
    // 定义任务句柄
    osal_task *GPIO_Button_task_handle = NULL;
    osal_kthread_lock();
    // 创建线程
#if defined CONFIG_GPIO_BUTTON_MODE_POLLING
    GPIO_Button_task_handle = osal_kthread_create((osal_kthread_handler)GPIO_Button_polling_task, 0, "GPIO_Button_task", 0x2000);
#else
    GPIO_Button_task_handle = osal_kthread_create((osal_kthread_handler)GPIO_Button_interrupt_task, 0, "GPIO_Button_interrupt_task", 0x2000);
#endif
    if (GPIO_Button_task_handle != NULL)
    {
        osal_kthread_set_priority(GPIO_Button_task_handle, 24);
        osal_kfree(GPIO_Button_task_handle);
    }
    osal_kthread_unlock();
}

app_run(task_entry);