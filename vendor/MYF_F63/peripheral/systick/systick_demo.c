/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Systick Sample Source. \n
 *
 */

#include "pinctrl.h"
#include "systick.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define SYSTICK_TASK_PRIO 24
#define SYSTICK_TASK_STACK_SIZE 0x1000
#define SYSTICK_DELAY_MS 1000

static void *systick_task(const char *arg)
{
    unused(arg);

    uint64_t count_before_get_us = 0;
    uint64_t count_after_get_us = 0;

    /* systick模块初始化 */
    uapi_systick_init();
    /* 通过count数差值验证延迟时间 */
    count_before_get_us = uapi_systick_get_us();
    uapi_systick_delay_ms(SYSTICK_DELAY_MS);
    count_after_get_us = uapi_systick_get_us();
    osal_printk("count_before_get_us = %llu, count_after_get_us = %llu\r\n", count_before_get_us, count_after_get_us);
    osal_printk("test case delay count %llu.\r\n", count_after_get_us - count_before_get_us);

    return NULL;
}

static void systick_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)systick_task, 0, "SystickTask", SYSTICK_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SYSTICK_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the systick_entry. */
app_run(systick_entry);