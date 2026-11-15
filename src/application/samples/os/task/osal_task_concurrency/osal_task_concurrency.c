/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief OSAL task concurrency and priority preemption sample.
 * @else
 * @brief OSAL 多任务并发与优先级抢占示例。
 * @endif
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define TASK_CONCURRENCY_STACK_SIZE 0x1000
#define TASK_CONCURRENCY_MEDIUM_PERIOD_MS 400
#define TASK_CONCURRENCY_LOW_PERIOD_MS 1000

#define TASK_CONCURRENCY_HIGH_PRIORITY OSAL_TASK_PRIORITY_HIGH
#define TASK_CONCURRENCY_MEDIUM_PRIORITY OSAL_TASK_PRIORITY_MIDDLE
#define TASK_CONCURRENCY_LOW_PRIORITY OSAL_TASK_PRIORITY_LOW

/* Wake-up semaphore for the high-priority task. / 高优先级任务的唤醒信号量。 */
static osal_semaphore g_high_task_sem;

/**
 * @if Eng
 * @brief Wait for events and demonstrate immediate execution after the high-priority task is awakened.
 * @param data Unused task argument.
 * @return This persistent task does not return.
 * @else
 * @brief 等待事件，并演示高优先级任务被唤醒后立即执行。
 * @param data 未使用的任务参数。
 * @return 常驻任务不会返回。
 * @endif
 */
static int task_concurrency_high_handler(const char *data)
{
    uint32_t event_count = 0;
    unused(data);

    while (1) {
        /* Block without consuming CPU until the low-priority task posts an event. /
         * 无事件时阻塞且不占用 CPU，等待低优先级任务发送事件。 */
        if (osal_sem_down(&g_high_task_sem) != OSAL_SUCCESS) {
            osal_printk("[task_concurrency] high task wait failed\r\n");
            (void)osal_msleep(TASK_CONCURRENCY_LOW_PERIOD_MS);
            continue;
        }

        event_count++;
        osal_printk("[high] event=%u running\r\n", event_count);
    }

    return 0;
}

/**
 * @if Eng
 * @brief Print an independent periodic heartbeat to show interleaved task execution.
 * @param data Unused task argument.
 * @return This persistent task does not return.
 * @else
 * @brief 周期打印独立心跳，展示多个任务交错执行。
 * @param data 未使用的任务参数。
 * @return 常驻任务不会返回。
 * @endif
 */
static int task_concurrency_medium_handler(const char *data)
{
    uint32_t heartbeat_count = 0;
    unused(data);

    while (1) {
        heartbeat_count++;
        osal_printk("[medium] heartbeat=%u\r\n", heartbeat_count);
        /* Sleeping moves the task to the blocked state and yields the CPU. /
         * 延时会使任务进入阻塞态并让出 CPU。 */
        (void)osal_msleep(TASK_CONCURRENCY_MEDIUM_PERIOD_MS);
    }

    return 0;
}

/**
 * @if Eng
 * @brief Wake the high-priority task periodically and expose the preemption order in the serial log.
 * @param data Unused task argument.
 * @return This persistent task does not return.
 * @else
 * @brief 周期唤醒高优先级任务，并通过串口日志展示抢占顺序。
 * @param data 未使用的任务参数。
 * @return 常驻任务不会返回。
 * @endif
 */
static int task_concurrency_low_handler(const char *data)
{
    uint32_t round_count = 0;
    unused(data);

    while (1) {
        (void)osal_msleep(TASK_CONCURRENCY_LOW_PERIOD_MS);
        round_count++;
        osal_printk("[low] round=%u before wake\r\n", round_count);

        /* Posting the semaphore wakes HighTask. LiteOS schedules it before LowTask resumes. /
         * 释放信号量会唤醒 HighTask，LiteOS 会先调度该任务，再恢复 LowTask。 */
        osal_sem_up(&g_high_task_sem);
        osal_printk("[low] round=%u resumed\r\n", round_count);
    }

    return 0;
}

/**
 * @if Eng
 * @brief Create a task and assign its sample priority while scheduling is locked.
 * @param handler Task entry function.
 * @param name Task name.
 * @param priority OSAL task priority.
 * @return Task handle on success, or NULL on failure.
 * @else
 * @brief 在调度锁定期间创建任务并设置示例优先级。
 * @param handler 任务入口函数。
 * @param name 任务名称。
 * @param priority OSAL 任务优先级。
 * @return 成功返回任务句柄，失败返回 NULL。
 * @endif
 */
static osal_task *task_concurrency_create(osal_kthread_handler handler, const char *name, uint32_t priority)
{
    osal_task *task = osal_kthread_create(handler, NULL, name, TASK_CONCURRENCY_STACK_SIZE);
    if (task == NULL) {
        osal_printk("[task_concurrency] create %s failed\r\n", name);
        return NULL;
    }

    if (osal_kthread_set_priority(task, priority) != OSAL_SUCCESS) {
        osal_printk("[task_concurrency] set %s priority failed\r\n", name);
        osal_kthread_destroy(task, 1);
        return NULL;
    }
    return task;
}

/**
 * @if Eng
 * @brief Initialize synchronization and start the three-task concurrency demonstration.
 * @else
 * @brief 初始化同步对象并启动三任务并发演示。
 * @endif
 */
static void task_concurrency_entry(void)
{
    osal_task *high_task = NULL;
    osal_task *medium_task = NULL;
    osal_task *low_task = NULL;

    if (osal_sem_init(&g_high_task_sem, 0) != OSAL_SUCCESS) {
        osal_printk("[task_concurrency] semaphore init failed\r\n");
        return;
    }

    /* Prevent a newly created task from running before all priorities are configured. /
     * 防止新任务在全部优先级配置完成前开始运行。 */
    osal_kthread_lock();
    high_task = task_concurrency_create((osal_kthread_handler)task_concurrency_high_handler, "HighTask",
                                        TASK_CONCURRENCY_HIGH_PRIORITY);
    medium_task = task_concurrency_create((osal_kthread_handler)task_concurrency_medium_handler, "MediumTask",
                                          TASK_CONCURRENCY_MEDIUM_PRIORITY);
    low_task = task_concurrency_create((osal_kthread_handler)task_concurrency_low_handler, "LowTask",
                                       TASK_CONCURRENCY_LOW_PRIORITY);
    if ((high_task == NULL) || (medium_task == NULL) || (low_task == NULL)) {
        if (high_task != NULL) {
            osal_kthread_destroy(high_task, 1);
        }
        if (medium_task != NULL) {
            osal_kthread_destroy(medium_task, 1);
        }
        if (low_task != NULL) {
            osal_kthread_destroy(low_task, 1);
        }
        osal_sem_destroy(&g_high_task_sem);
        osal_kthread_unlock();
        osal_printk("[task_concurrency] start failed\r\n");
        return;
    }

    /* OSAL task objects are wrappers; the detached LiteOS tasks continue after the wrappers are released. /
     * OSAL 任务对象是包装结构，释放后已创建的 LiteOS 分离任务仍会继续运行。 */
    osal_kfree(high_task);
    osal_kfree(medium_task);
    osal_kfree(low_task);

    osal_printk("[task_concurrency] started: high=%u medium=%u low=%u\r\n", TASK_CONCURRENCY_HIGH_PRIORITY,
                TASK_CONCURRENCY_MEDIUM_PRIORITY, TASK_CONCURRENCY_LOW_PRIORITY);
    osal_kthread_unlock();
}

/* Register the sample entry. / 注册案例入口。 */
app_run(task_concurrency_entry);
