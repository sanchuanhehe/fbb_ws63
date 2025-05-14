/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Watchdog Sample Source. \n
 *
 */

#include "pinctrl.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog_porting.h"

#define TIME_OUT 2
#define WDT_MODE 1
#define TEST_PARAM_KICK_TIME 10
#define WDT_TASK_DURATION_MS 500

#define WDT_TASK_PRIO 24
#define WDT_TASK_STACK_SIZE 0x1000

static void *watchdog_task(const char *arg)
{
    UNUSED(arg);
    uint32_t sample_remain_ms;
    // 设置开门狗超时时间
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
    // 使能看门狗
    uapi_watchdog_enable(WDT_MODE_RESET);
    // delay 5000 ms
    osal_mdelay(5000);
    // 喂狗
    uapi_watchdog_kick();
    // 获取剩余超时时间
    uapi_watchdog_get_left_time(&sample_remain_ms);
    osal_printk("sample_remain_ms = %x! \n", sample_remain_ms);
    // 关闭看门狗
    uapi_watchdog_disable();

    return NULL;
}

static void watchdog_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)watchdog_task, 0, "WatchdogTask", WDT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WDT_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the watchdog_entry. */
app_run(watchdog_entry);