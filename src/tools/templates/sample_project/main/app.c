/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
 * All rights reserved.
 *
 * Description: Out-of-tree project entry. The `app_run()` macro registers
 *              the entry with the SDK's app init framework; LiteOS calls
 *              it during boot. Replace this stub with your own logic.
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define HELLO_TASK_STACK_SIZE   0x1000
#define HELLO_TASK_PRIORITY     26
#define HELLO_INTERVAL_MS       1000

static void *hello_task(const char *arg)
{
    unused(arg);
    osal_printk("sample_project starting\r\n");
    for (;;) {
        osal_printk("Hello world\r\n");
        osal_msleep(HELLO_INTERVAL_MS);
    }
    return NULL;
}

static void hello_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)hello_task, 0,
                                      "HelloTask", HELLO_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, HELLO_TASK_PRIORITY);
    }
    osal_kthread_unlock();
}

app_run(hello_entry);
