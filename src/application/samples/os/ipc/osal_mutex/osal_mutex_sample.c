/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief OSAL mutex exclusive-access sample.
 * @else
 * @brief OSAL 互斥锁排他访问示例。
 * @endif
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define MUTEX_SAMPLE_TASK_STACK_SIZE 0x1000U
#define MUTEX_SAMPLE_TASK_PRIORITY OSAL_TASK_PRIORITY_MIDDLE
#define MUTEX_SAMPLE_PERIOD_MS 500U
#define MUTEX_SAMPLE_LOCK_TIMEOUT_MS 100U

static osal_mutex g_print_mutex;

static int mutex_sample_print(const char *task_name)
{
    if (osal_mutex_lock_timeout(&g_print_mutex, MUTEX_SAMPLE_LOCK_TIMEOUT_MS) != OSAL_SUCCESS) {
        osal_printk("[mutex] %s lock timeout\r\n", task_name);
        return OSAL_FAILURE;
    }
    osal_printk("[mutex] %s: Hello World\r\n", task_name);
    osal_mutex_unlock(&g_print_mutex);
    return OSAL_SUCCESS;
}

static int mutex_sample_task_a_handler(const char *data)
{
    unused(data);
    while (1) {
        (void)mutex_sample_print("TaskA");
        (void)osal_msleep(MUTEX_SAMPLE_PERIOD_MS);
    }
    return 0;
}

static int mutex_sample_task_b_handler(const char *data)
{
    unused(data);
    while (1) {
        (void)mutex_sample_print("TaskB");
        (void)osal_msleep(MUTEX_SAMPLE_PERIOD_MS);
    }
    return 0;
}

static osal_task *mutex_sample_create_task(osal_kthread_handler handler, const char *name)
{
    osal_task *task = osal_kthread_create(handler, NULL, name, MUTEX_SAMPLE_TASK_STACK_SIZE);
    if (task == NULL) {
        osal_printk("[mutex] create %s failed\r\n", name);
        return NULL;
    }
    if (osal_kthread_set_priority(task, MUTEX_SAMPLE_TASK_PRIORITY) != OSAL_SUCCESS) {
        osal_printk("[mutex] set %s priority failed\r\n", name);
        osal_kthread_destroy(task, 1);
        return NULL;
    }
    return task;
}

static void mutex_sample_entry(void)
{
    osal_task *task_a = NULL;
    osal_task *task_b = NULL;

    if (osal_mutex_init(&g_print_mutex) != OSAL_SUCCESS) {
        osal_printk("[mutex] init failed\r\n");
        return;
    }

    osal_kthread_lock();
    task_a = mutex_sample_create_task((osal_kthread_handler)mutex_sample_task_a_handler, "MutexTaskA");
    task_b = mutex_sample_create_task((osal_kthread_handler)mutex_sample_task_b_handler, "MutexTaskB");
    if ((task_a == NULL) || (task_b == NULL)) {
        if (task_a != NULL) {
            osal_kthread_destroy(task_a, 1);
        }
        if (task_b != NULL) {
            osal_kthread_destroy(task_b, 1);
        }
        osal_mutex_destroy(&g_print_mutex);
        osal_kthread_unlock();
        return;
    }

    osal_kfree(task_a);
    osal_kfree(task_b);
    osal_printk("[mutex] started\r\n");
    osal_kthread_unlock();
}

app_run(mutex_sample_entry);
