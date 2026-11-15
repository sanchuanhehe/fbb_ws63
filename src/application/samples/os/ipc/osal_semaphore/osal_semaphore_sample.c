/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief OSAL producer-consumer semaphore sample.
 * @else
 * @brief OSAL 生产者消费者信号量同步示例。
 * @endif
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define SEMAPHORE_SAMPLE_TASK_STACK_SIZE 0x1000U
#define SEMAPHORE_SAMPLE_PRODUCER_PRIORITY OSAL_TASK_PRIORITY_MIDDLE
#define SEMAPHORE_SAMPLE_CONSUMER_PRIORITY OSAL_TASK_PRIORITY_LOW
#define SEMAPHORE_SAMPLE_PERIOD_MS 1000U

static osal_semaphore g_sample_sem;

static int semaphore_sample_producer_handler(const char *data)
{
    unused(data);
    while (1) {
        (void)osal_msleep(SEMAPHORE_SAMPLE_PERIOD_MS);
        osal_sem_up(&g_sample_sem);
    }
    return 0;
}

static int semaphore_sample_consumer_handler(const char *data)
{
    uint32_t notify_count = 0;
    unused(data);

    while (1) {
        if (osal_sem_down(&g_sample_sem) != OSAL_SUCCESS) {
            osal_printk("[semaphore] wait failed\r\n");
            continue;
        }
        notify_count++;
        osal_printk("[semaphore] consumer notified: %u\r\n", notify_count);
    }
    return 0;
}

static osal_task *semaphore_sample_create_task(osal_kthread_handler handler, const char *name, uint32_t priority)
{
    osal_task *task = osal_kthread_create(handler, NULL, name, SEMAPHORE_SAMPLE_TASK_STACK_SIZE);
    if (task == NULL) {
        osal_printk("[semaphore] create %s failed\r\n", name);
        return NULL;
    }
    if (osal_kthread_set_priority(task, priority) != OSAL_SUCCESS) {
        osal_printk("[semaphore] set %s priority failed\r\n", name);
        osal_kthread_destroy(task, 1);
        return NULL;
    }
    return task;
}

static void semaphore_sample_entry(void)
{
    osal_task *producer = NULL;
    osal_task *consumer = NULL;

    if (osal_sem_init(&g_sample_sem, 0) != OSAL_SUCCESS) {
        osal_printk("[semaphore] init failed\r\n");
        return;
    }

    osal_kthread_lock();
    consumer = semaphore_sample_create_task((osal_kthread_handler)semaphore_sample_consumer_handler,
                                            "SemConsumer", SEMAPHORE_SAMPLE_CONSUMER_PRIORITY);
    producer = semaphore_sample_create_task((osal_kthread_handler)semaphore_sample_producer_handler,
                                            "SemProducer", SEMAPHORE_SAMPLE_PRODUCER_PRIORITY);
    if ((producer == NULL) || (consumer == NULL)) {
        if (producer != NULL) {
            osal_kthread_destroy(producer, 1);
        }
        if (consumer != NULL) {
            osal_kthread_destroy(consumer, 1);
        }
        osal_sem_destroy(&g_sample_sem);
        osal_kthread_unlock();
        return;
    }

    osal_kfree(producer);
    osal_kfree(consumer);
    osal_printk("[semaphore] started\r\n");
    osal_kthread_unlock();
}

app_run(semaphore_sample_entry);
