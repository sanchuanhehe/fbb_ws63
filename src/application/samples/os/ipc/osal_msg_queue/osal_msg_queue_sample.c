/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief OSAL interrupt-to-task message queue sample.
 * @else
 * @brief OSAL 中断到任务消息队列示例。
 * @endif
 */

#include <stdint.h>
#include "common_def.h"
#include "chip_core_irq.h"
#include "hal_timer_v150.h"
#include "soc_osal.h"
#include "app_init.h"

#define MSG_QUEUE_TIMER_INDEX TIMER_INDEX_2
#define MSG_QUEUE_TIMER_IRQ TIMER_2_IRQN
#define MSG_QUEUE_TIMER_IRQ_PRIORITY 1U
#define MSG_QUEUE_TIMER_INTERVAL_US 1000000U
#define MSG_QUEUE_US_PER_SECOND 1000000U
#define MSG_QUEUE_DEPTH 4U
#define MSG_QUEUE_TASK_STACK_SIZE 0x1000U
#define MSG_QUEUE_BRIDGE_PRIORITY OSAL_TASK_PRIORITY_MIDDLE
#define MSG_QUEUE_WORKER_PRIORITY OSAL_TASK_PRIORITY_LOW
#define MSG_QUEUE_EVENT_TYPE_TIMER 1U

typedef struct {
    uint32_t type;
    uint32_t sequence;
} msg_queue_event_t;

static osal_semaphore g_msg_queue_isr_sem;
static unsigned long g_msg_queue_id;
static volatile uint32_t g_msg_queue_irq_sequence;
static hal_timer_funcs_t *g_msg_queue_timer_funcs;

static void msg_queue_start_timer(void)
{
    uint64_t timer_cycles =
        (uint64_t)MSG_QUEUE_TIMER_INTERVAL_US * (CONFIG_TIMER_CLOCK_VALUE / MSG_QUEUE_US_PER_SECOND);

    g_msg_queue_timer_funcs->stop(MSG_QUEUE_TIMER_INDEX);
    g_msg_queue_timer_funcs->config_load(MSG_QUEUE_TIMER_INDEX, timer_cycles);
    g_msg_queue_timer_funcs->start(MSG_QUEUE_TIMER_INDEX);
}

static int msg_queue_timer_handler(int irq, void *dev)
{
    unused(irq);
    unused(dev);

    hal_timer_v150_interrupt_clear(MSG_QUEUE_TIMER_INDEX);
    (void)osal_irq_clear(MSG_QUEUE_TIMER_IRQ);
    g_msg_queue_irq_sequence++;
    osal_sem_up(&g_msg_queue_isr_sem);
    msg_queue_start_timer();
    return OSAL_IRQ_HANDLED;
}

static int msg_queue_bridge_handler(const char *data)
{
    msg_queue_event_t event = {0};
    unused(data);

    while (1) {
        if (osal_sem_down(&g_msg_queue_isr_sem) != OSAL_SUCCESS) {
            osal_printk("[msg_queue] bridge wait failed\r\n");
            continue;
        }
        event.type = MSG_QUEUE_EVENT_TYPE_TIMER;
        event.sequence = g_msg_queue_irq_sequence;
        if (osal_msg_queue_write_copy(g_msg_queue_id, &event, sizeof(event), OSAL_MSGQ_NO_WAIT) != OSAL_SUCCESS) {
            osal_printk("[msg_queue] queue full, dropped sequence=%u\r\n", event.sequence);
        }
    }
    return 0;
}

static int msg_queue_worker_handler(const char *data)
{
    msg_queue_event_t event = {0};
    unsigned int event_size;
    unused(data);

    while (1) {
        event_size = sizeof(event);
        if (osal_msg_queue_read_copy(g_msg_queue_id, &event, &event_size, OSAL_MSGQ_WAIT_FOREVER) != OSAL_SUCCESS) {
            osal_printk("[msg_queue] read failed\r\n");
            continue;
        }
        osal_printk("[msg_queue] worker received type=%u sequence=%u\r\n", event.type, event.sequence);
    }
    return 0;
}

static osal_task *msg_queue_create_task(osal_kthread_handler handler, const char *name, uint32_t priority)
{
    osal_task *task = osal_kthread_create(handler, NULL, name, MSG_QUEUE_TASK_STACK_SIZE);
    if (task == NULL) {
        osal_printk("[msg_queue] create %s failed\r\n", name);
        return NULL;
    }
    if (osal_kthread_set_priority(task, priority) != OSAL_SUCCESS) {
        osal_printk("[msg_queue] set %s priority failed\r\n", name);
        osal_kthread_destroy(task, 1);
        return NULL;
    }
    return task;
}

static int msg_queue_initialize_interrupt(void)
{
    errcode_t ret;

    timer_port_register_hal_funcs(MSG_QUEUE_TIMER_INDEX);
    g_msg_queue_timer_funcs = hal_timer_get_funcs(MSG_QUEUE_TIMER_INDEX);
    if (g_msg_queue_timer_funcs == NULL) {
        return OSAL_FAILURE;
    }
    ret = g_msg_queue_timer_funcs->init(MSG_QUEUE_TIMER_INDEX, NULL);
    if (ret != ERRCODE_SUCC) {
        return OSAL_FAILURE;
    }
    osal_irq_disable(MSG_QUEUE_TIMER_IRQ);
    if (osal_irq_request(MSG_QUEUE_TIMER_IRQ, msg_queue_timer_handler, NULL, "MsgQueueTimer", NULL) != OSAL_SUCCESS) {
        g_msg_queue_timer_funcs->deinit(MSG_QUEUE_TIMER_INDEX);
        timer_port_unregister_hal_funcs(MSG_QUEUE_TIMER_INDEX);
        return OSAL_FAILURE;
    }
    if (osal_irq_set_priority(MSG_QUEUE_TIMER_IRQ, MSG_QUEUE_TIMER_IRQ_PRIORITY) != OSAL_SUCCESS) {
        osal_irq_free(MSG_QUEUE_TIMER_IRQ, NULL);
        g_msg_queue_timer_funcs->deinit(MSG_QUEUE_TIMER_INDEX);
        timer_port_unregister_hal_funcs(MSG_QUEUE_TIMER_INDEX);
        return OSAL_FAILURE;
    }
    osal_irq_enable(MSG_QUEUE_TIMER_IRQ);
    return OSAL_SUCCESS;
}

static void msg_queue_entry(void)
{
    osal_task *bridge = NULL;
    osal_task *worker = NULL;

    if (osal_sem_binary_sem_init(&g_msg_queue_isr_sem, 0) != OSAL_SUCCESS) {
        osal_printk("[msg_queue] semaphore init failed\r\n");
        return;
    }
    if (osal_msg_queue_create("IsrEventQueue", MSG_QUEUE_DEPTH, &g_msg_queue_id, 0,
                              sizeof(msg_queue_event_t)) != OSAL_SUCCESS) {
        osal_printk("[msg_queue] queue create failed\r\n");
        osal_sem_destroy(&g_msg_queue_isr_sem);
        return;
    }

    osal_kthread_lock();
    bridge = msg_queue_create_task((osal_kthread_handler)msg_queue_bridge_handler, "QueueBridge",
                                   MSG_QUEUE_BRIDGE_PRIORITY);
    worker = msg_queue_create_task((osal_kthread_handler)msg_queue_worker_handler, "QueueWorker",
                                   MSG_QUEUE_WORKER_PRIORITY);
    if ((bridge == NULL) || (worker == NULL) || (msg_queue_initialize_interrupt() != OSAL_SUCCESS)) {
        if (bridge != NULL) {
            osal_kthread_destroy(bridge, 1);
        }
        if (worker != NULL) {
            osal_kthread_destroy(worker, 1);
        }
        osal_msg_queue_delete(g_msg_queue_id);
        osal_sem_destroy(&g_msg_queue_isr_sem);
        osal_kthread_unlock();
        osal_printk("[msg_queue] start failed\r\n");
        return;
    }

    osal_kfree(bridge);
    osal_kfree(worker);
    g_msg_queue_irq_sequence = 0;
    msg_queue_start_timer();
    osal_printk("[msg_queue] started: ISR -> semaphore -> queue -> worker\r\n");
    osal_kthread_unlock();
}

app_run(msg_queue_entry);
