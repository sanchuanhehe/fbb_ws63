/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Timer Sample Source. \n
 *
 */

/**
 * 本例程通过timer1实现了一个1s的定时器。
 */

#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define TIMER_TIMERS_NUM 4
#define TIMER_INDEX 1
#define TIMER_PRIO 1
#define TIMER_DELAY_INT 5
#define TIMER1_DELAY_1000US 1000
#define TIMER_TASK_PRIO 24
#define TIMER_TASK_STACK_SIZE 0x1000
#define TIMER1_DELAY_1000 1000

timer_handle_t timer1_handle;

/* Timed task callback function list. */
static void timer_timeout_callback(uintptr_t data)
{
    unused(data);
    osal_printk("------1s------\r\n");
    uapi_timer_start(timer1_handle, TIMER1_DELAY_1000 * TIMER1_DELAY_1000US, timer_timeout_callback, 0);
}

static void *timer_task(const char *arg)
{
    unused(arg);

    uapi_timer_init();
    // 设置 timer1 硬件初始化
    uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);
    // 创建 timer1 软件定时器控制句柄
    uapi_timer_create(TIMER_INDEX_1, &timer1_handle);
    // 启动定时器
    uapi_timer_start(timer1_handle, TIMER1_DELAY_1000 * TIMER1_DELAY_1000US, timer_timeout_callback, 0);

    return NULL;
}

static void timer_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)timer_task, 0, "TimerTask", TIMER_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TIMER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the timer_entry. */
app_run(timer_entry);