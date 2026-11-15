/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define DEFAULT_TASK_STACK_SIZE         0x1000
#define DEFAULT_TASK_PRIORITY           26
#define DELAYS_MS                       1000


static void *hw_task(const char *arg)
{
    unused(arg);
    osal_printk("start helloworld sample\r\n");
    for(;;){
        osal_printk("hello world\r\n");
        osal_msleep(DELAYS_MS);
    }
    return NULL;

}


static void helloworld_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)hw_task, 0, "HW_Task", DEFAULT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, DEFAULT_TASK_PRIORITY);
    }
    osal_kthread_unlock();
}

/* Run the helloword_entry. */
app_run(helloworld_entry);