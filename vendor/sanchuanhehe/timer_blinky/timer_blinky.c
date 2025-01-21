/**
 * Copyright (c) sanchuanhehe
 *
 * Description: Blinky with Timer Sample Source. \n
 *              This file implements a LED blinking example using a timer.
 *              The LED toggles its state at a fixed interval controlled by a timer. \n
 *
 * History: \n
 * 2025-01-22, Create file. \n
 */

#include "pinctrl.h"       // 引脚控制相关头文件
#include "gpio.h"          // GPIO操作相关头文件
#include "soc_osal.h"      // 操作系统抽象层头文件
#include "app_init.h"      // 应用程序初始化头文件
#include "timer.h"         // 定时器相关头文件
#include "osal_debug.h"    // 调试打印相关头文件
#include "chip_core_irq.h" // 芯片核心中断相关头文件

#define BLINKY_DURATION_MS 500       // LED闪烁间隔时间，单位为毫秒
#define TIMER_TASK_STACK_SIZE 0x1000 // 任务栈大小
#define TIMER_TASK_PRIO 17           // 任务的优先级，数值越小优先级越高
#define TIMER_INDEX 1                // 定时器索引
#define TIMER_PRIO 1                 // 定时器优先级

static timer_handle_t timer_handle = NULL; // 定时器句柄

/**
 * @brief 定时器触发时的回调函数
 * @param data 传递给回调函数的参数（未使用）
 */
void TimerCallback(uintptr_t data)
{
    unused(data); // 标记未使用的参数，避免编译器警告

    // 切换LED状态
    uapi_gpio_toggle(CONFIG_BLINKY_PIN); // 切换指定GPIO引脚的电平状态
    osal_printk("LED toggled.\r\n");     // 打印调试信息，表示LED状态已切换

    // 重新启动定时器，实现周期性闪烁
    uapi_timer_start(timer_handle, BLINKY_DURATION_MS * 1000, TimerCallback, 0);
}

/**
 * @brief 定时器控制LED闪烁任务的主入口函数
 * @param arg 任务参数（未使用）
 * @return 无返回值
 */
static void *blinky_timer_task(const char *arg)
{
    unused(arg); // 标记未使用的参数，避免编译器警告

    // 初始化GPIO
    uapi_pin_set_mode(CONFIG_BLINKY_PIN, HAL_PIO_FUNC_GPIO);     // 设置引脚为GPIO功能
    uapi_gpio_set_dir(CONFIG_BLINKY_PIN, GPIO_DIRECTION_OUTPUT); // 设置引脚为输出模式
    uapi_gpio_set_val(CONFIG_BLINKY_PIN, GPIO_LEVEL_LOW);        // 设置引脚初始电平为低

    // 初始化定时器
    uapi_timer_init();                                         // 初始化定时器模块
    uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO); // 配置定时器适配器

    // 创建定时器
    uapi_timer_create(TIMER_INDEX, &timer_handle); // 创建定时器，并获取定时器句柄

    // 启动定时器
    uapi_timer_start(timer_handle, BLINKY_DURATION_MS * 1000, TimerCallback, 0); // 启动定时器，设置触发时间和回调函数

    return NULL; // 任务函数返回
}

/**
 * @brief 定时器控制LED闪烁任务的入口函数
 */
static void blinky_timer_entry(void)
{
    uint32_t ret;
    osal_task *taskid;

    // 创建任务调度
    osal_kthread_lock(); // 加锁，确保任务创建过程是原子的

    // 创建任务
    taskid = osal_kthread_create((osal_kthread_handler)blinky_timer_task, NULL, "blinky_timer_task",
                                 TIMER_TASK_STACK_SIZE); // 创建任务，指定任务函数、参数、名称和栈大小
    ret = osal_kthread_set_priority(taskid, TIMER_TASK_PRIO); // 设置任务优先级
    if (ret != OSAL_SUCCESS) {
        printf("create task failed .\n"); // 如果任务创建失败，打印错误信息
    }

    osal_kthread_unlock(); // 解锁
}

/* Run the blinky_timer_entry. */
app_run(blinky_timer_entry); // 运行任务入口函数