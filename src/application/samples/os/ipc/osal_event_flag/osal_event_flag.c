/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief OSAL event flag AND-wait sample.
 * @else
 * @brief OSAL 事件标志 AND 等待示例。
 * @endif
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define EVENT_FLAG_TASK_A_READY 0x1U
#define EVENT_FLAG_TASK_B_READY 0x2U
#define EVENT_FLAG_ALL_READY (EVENT_FLAG_TASK_A_READY | EVENT_FLAG_TASK_B_READY)
#define EVENT_FLAG_TASK_A_DELAY_MS 1000U
#define EVENT_FLAG_TASK_B_DELAY_MS 2000U
#define EVENT_FLAG_TASK_STACK_SIZE 0x1000U
#define EVENT_FLAG_TASK_PRIORITY OSAL_TASK_PRIORITY_MIDDLE
#define EVENT_FLAG_MAIN_PRIORITY OSAL_TASK_PRIORITY_LOW

static osal_event g_event_flag;

static int event_flag_task_a_handler(const char *data)
{
    unused(data);
    (void)osal_msleep(EVENT_FLAG_TASK_A_DELAY_MS);
    osal_printk("[event_flag] task A ready\r\n");
    if (osal_event_write(&g_event_flag, EVENT_FLAG_TASK_A_READY) != OSAL_SUCCESS) {
        osal_printk("[event_flag] task A write failed\r\n");
    }
    return 0;
}

static int event_flag_task_b_handler(const char *data)
{
    unused(data);
    (void)osal_msleep(EVENT_FLAG_TASK_B_DELAY_MS);
    osal_printk("[event_flag] task B ready\r\n");
    if (osal_event_write(&g_event_flag, EVENT_FLAG_TASK_B_READY) != OSAL_SUCCESS) {
        osal_printk("[event_flag] task B write failed\r\n");
    }
    return 0;
}

static int event_flag_main_handler(const char *data)
{
    int ret;
    unused(data);

    ret = osal_event_read(&g_event_flag, EVENT_FLAG_ALL_READY, OSAL_EVENT_FOREVER,
                          OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR);
    if (ret != (int)EVENT_FLAG_ALL_READY) {
        osal_printk("[event_flag] wait failed ret=0x%x\r\n", ret);
        return ret;
    }
    osal_printk("[event_flag] all tasks ready, mask=0x%x\r\n", ret);
    return 0;
}

static osal_task *event_flag_create_task(osal_kthread_handler handler, const char *name, uint32_t priority)
{
    osal_task *task = osal_kthread_create(handler, NULL, name, EVENT_FLAG_TASK_STACK_SIZE);
    if (task == NULL) {
        osal_printk("[event_flag] create %s failed\r\n", name);
        return NULL;
    }
    if (osal_kthread_set_priority(task, priority) != OSAL_SUCCESS) {
        osal_printk("[event_flag] set %s priority failed\r\n", name);
        osal_kthread_destroy(task, 1);
        return NULL;
    }
    return task;
}

static void event_flag_entry(void)
{
    osal_task *task_a = NULL;
    osal_task *task_b = NULL;
    osal_task *main_task = NULL;

    if (osal_event_init(&g_event_flag) != OSAL_SUCCESS) {
        osal_printk("[event_flag] init failed\r\n");
        return;
    }

    osal_kthread_lock();
    main_task = event_flag_create_task((osal_kthread_handler)event_flag_main_handler, "EventMain",
                                       EVENT_FLAG_MAIN_PRIORITY);
    task_a = event_flag_create_task((osal_kthread_handler)event_flag_task_a_handler, "EventTaskA",
                                    EVENT_FLAG_TASK_PRIORITY);
    task_b = event_flag_create_task((osal_kthread_handler)event_flag_task_b_handler, "EventTaskB",
                                    EVENT_FLAG_TASK_PRIORITY);
    if ((main_task == NULL) || (task_a == NULL) || (task_b == NULL)) {
        if (main_task != NULL) {
            osal_kthread_destroy(main_task, 1);
        }
        if (task_a != NULL) {
            osal_kthread_destroy(task_a, 1);
        }
        if (task_b != NULL) {
            osal_kthread_destroy(task_b, 1);
        }
        (void)osal_event_destroy(&g_event_flag);
        osal_kthread_unlock();
        return;
    }

    osal_kfree(main_task);
    osal_kfree(task_a);
    osal_kfree(task_b);
    osal_printk("[event_flag] started, waiting mask=0x%x\r\n", EVENT_FLAG_ALL_READY);
    osal_kthread_unlock();
}

app_run(event_flag_entry);
