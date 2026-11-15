/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief Demonstrates the complete OSAL hardware interrupt management lifecycle.
 * @else
 * @brief 演示 OSAL 硬件中断管理的完整生命周期。
 * @endif
 */

#include <stdbool.h>
#include <stdint.h>
#include "common_def.h"
#include "chip_core_irq.h"
#include "hal_timer_v150.h"
#include "soc_osal.h"
#include "app_init.h"

#define OSAL_INTERRUPT_SOURCE_INDEX TIMER_INDEX_2
#define OSAL_INTERRUPT_IRQ_NUMBER TIMER_2_IRQN
#define OSAL_INTERRUPT_IRQ_PRIORITY 1U
#define OSAL_INTERRUPT_INTERVAL_US 500000U
#define OSAL_INTERRUPT_US_PER_SECOND 1000000U
#define OSAL_INTERRUPT_EXPECTED_COUNT 5U
#define OSAL_INTERRUPT_TASK_PRIORITY 24U
#define OSAL_INTERRUPT_WORKER_PRIORITY 25U
#define OSAL_INTERRUPT_TASK_STACK_SIZE 0x1000U

#define OSAL_INTERRUPT_LOG "[osal interrupt]"

/**
 * @if Eng
 * @brief Semaphore used by the ISR to notify the processing task.
 * @else
 * @brief 中断服务程序用于通知处理任务的信号量。
 * @endif
 */
static osal_semaphore g_osal_interrupt_sem;

/**
 * @if Eng
 * @brief Number of hardware interrupts received by the ISR.
 * @else
 * @brief 中断服务程序接收到的硬件中断次数。
 * @endif
 */
static volatile uint32_t g_osal_interrupt_count = 0;

/**
 * @if Eng
 * @brief Number of callbacks that were not executed in interrupt context.
 * @else
 * @brief 未在中断上下文执行的回调次数。
 * @endif
 */
static volatile uint32_t g_osal_interrupt_context_failures = 0;

/**
 * @if Eng
 * @brief Controls the continuous workload used to demonstrate asynchronous interrupt preemption.
 * @else
 * @brief 控制用于演示异步中断抢占的连续工作任务。
 * @endif
 */
static volatile bool g_osal_interrupt_worker_running = false;

/**
 * @if Eng
 * @brief Progress counter continuously updated by the lower-priority worker task.
 * @else
 * @brief 由低优先级工作任务持续更新的进度计数器。
 * @endif
 */
static volatile uint32_t g_osal_interrupt_worker_progress = 0;

/**
 * @if Eng
 * @brief Worker progress captured by the ISR at each preemption point.
 * @else
 * @brief ISR 在每个抢占时刻保存的工作任务进度。
 * @endif
 */
static volatile uint32_t g_osal_interrupt_progress_snapshot[OSAL_INTERRUPT_EXPECTED_COUNT] = {0};

/**
 * @if Eng
 * @brief Interrupt-context result captured independently for every interrupt.
 * @else
 * @brief 每次中断分别保存的中断上下文检查结果。
 * @endif
 */
static volatile bool g_osal_interrupt_context_snapshot[OSAL_INTERRUPT_EXPECTED_COUNT] = {false};

/**
 * @if Eng
 * @brief Timer HAL function table used only to generate the sample interrupt source.
 * @else
 * @brief 仅用于产生案例中断源的 Timer HAL 函数表。
 * @endif
 */
static hal_timer_funcs_t *g_osal_interrupt_timer_funcs = NULL;

typedef struct {
    bool semaphore_initialized;
    bool timer_hal_registered;
    bool timer_initialized;
    bool irq_requested;
} osal_interrupt_resources_t;

/**
 * @if Eng
 * @brief Load and start one Timer2 one-shot period as the next interrupt source.
 * @else
 * @brief 装载并启动一次 Timer2 单次计时，作为下一次中断源。
 * @endif
 */
static void osal_interrupt_start_source(void)
{
    uint64_t timer_cycles =
        (uint64_t)OSAL_INTERRUPT_INTERVAL_US * (CONFIG_TIMER_CLOCK_VALUE / OSAL_INTERRUPT_US_PER_SECOND);

    g_osal_interrupt_timer_funcs->stop(OSAL_INTERRUPT_SOURCE_INDEX);
    g_osal_interrupt_timer_funcs->config_load(OSAL_INTERRUPT_SOURCE_INDEX, timer_cycles);
    g_osal_interrupt_timer_funcs->start(OSAL_INTERRUPT_SOURCE_INDEX);
}

/**
 * @if Eng
 * @brief Clear the Timer2 interrupt, record its context, and notify the task.
 * @param [in] irq Interrupt number supplied by OSAL.
 * @param [in] dev Private device parameter supplied during registration.
 * @return OSAL_IRQ_HANDLED after the interrupt is handled.
 * @else
 * @brief 清除 Timer2 中断、记录中断上下文并通知任务。
 * @param [in] irq OSAL 传入的中断号。
 * @param [in] dev 注册中断时传入的私有设备参数。
 * @return 中断处理完成后返回 OSAL_IRQ_HANDLED。
 * @endif
 */
static int osal_interrupt_handler(int irq, void *dev)
{
    uint32_t snapshot_index = g_osal_interrupt_count;

    unused(irq);
    unused(dev);

    hal_timer_v150_interrupt_clear(OSAL_INTERRUPT_SOURCE_INDEX);
    (void)osal_irq_clear(OSAL_INTERRUPT_IRQ_NUMBER);
    if (snapshot_index < OSAL_INTERRUPT_EXPECTED_COUNT) {
        g_osal_interrupt_progress_snapshot[snapshot_index] = g_osal_interrupt_worker_progress;
        g_osal_interrupt_context_snapshot[snapshot_index] = (osal_in_interrupt() != 0);
        if (!g_osal_interrupt_context_snapshot[snapshot_index]) {
            g_osal_interrupt_context_failures++;
        }
        g_osal_interrupt_count = snapshot_index + 1U;
        if (g_osal_interrupt_count < OSAL_INTERRUPT_EXPECTED_COUNT) {
            /* Rearm in the ISR so interrupts continue independently of task scheduling. /
             * 在 ISR 中重装定时器，使中断不依赖任务调度而持续产生。 */
            osal_interrupt_start_source();
        }
    }
    osal_sem_up(&g_osal_interrupt_sem);
    return OSAL_IRQ_HANDLED;
}

/**
 * @if Eng
 * @brief Continuously update task progress without waiting or sleeping.
 * @param [in] data Unused task argument.
 * @return The task returns after the interrupt test stops the workload.
 * @else
 * @brief 不等待、不休眠，持续更新任务进度。
 * @param [in] data 未使用的任务参数。
 * @return 中断测试停止工作负载后任务返回。
 * @endif
 */
static int osal_interrupt_worker_handler(const char *data)
{
    unused(data);

    while (g_osal_interrupt_worker_running) {
        g_osal_interrupt_worker_progress++;
    }
    return 0;
}

/**
 * @if Eng
 * @brief Create the lower-priority continuous workload task.
 * @return true if the worker task is created and configured successfully; otherwise false.
 * @else
 * @brief 创建低优先级连续工作任务。
 * @return 工作任务创建并配置成功返回 true，否则返回 false。
 * @endif
 */
static bool osal_interrupt_start_worker(void)
{
    osal_task *worker_task = NULL;
    bool worker_started = false;

    osal_kthread_lock();
    worker_task = osal_kthread_create((osal_kthread_handler)osal_interrupt_worker_handler, NULL, "InterruptWorker",
                                      OSAL_INTERRUPT_TASK_STACK_SIZE);
    if (worker_task != NULL) {
        if (osal_kthread_set_priority(worker_task, OSAL_INTERRUPT_WORKER_PRIORITY) == OSAL_SUCCESS) {
            osal_kfree(worker_task);
            worker_started = true;
        } else {
            osal_kthread_destroy(worker_task, 1);
        }
    }
    osal_kthread_unlock();
    return worker_started;
}

static void osal_interrupt_reset_state(void)
{
    g_osal_interrupt_count = 0;
    g_osal_interrupt_context_failures = 0;
    g_osal_interrupt_worker_progress = 0;
    for (uint32_t i = 0; i < OSAL_INTERRUPT_EXPECTED_COUNT; i++) {
        g_osal_interrupt_progress_snapshot[i] = 0;
        g_osal_interrupt_context_snapshot[i] = false;
    }
}

static bool osal_interrupt_initialize(osal_interrupt_resources_t *resources)
{
    errcode_t timer_ret;
    int osal_ret = osal_sem_binary_sem_init(&g_osal_interrupt_sem, 0);
    if (osal_ret != OSAL_SUCCESS) {
        osal_printk("%s semaphore init failed\r\n", OSAL_INTERRUPT_LOG);
        return false;
    }
    resources->semaphore_initialized = true;

    timer_port_register_hal_funcs(OSAL_INTERRUPT_SOURCE_INDEX);
    resources->timer_hal_registered = true;
    g_osal_interrupt_timer_funcs = hal_timer_get_funcs(OSAL_INTERRUPT_SOURCE_INDEX);
    if (g_osal_interrupt_timer_funcs == NULL) {
        osal_printk("%s get Timer2 HAL functions failed\r\n", OSAL_INTERRUPT_LOG);
        return false;
    }

    timer_ret = g_osal_interrupt_timer_funcs->init(OSAL_INTERRUPT_SOURCE_INDEX, NULL);
    if (timer_ret != ERRCODE_SUCC) {
        osal_printk("%s Timer2 init failed ret=0x%x\r\n", OSAL_INTERRUPT_LOG, timer_ret);
        return false;
    }
    resources->timer_initialized = true;

    osal_irq_disable(OSAL_INTERRUPT_IRQ_NUMBER);
    osal_ret = osal_irq_request(OSAL_INTERRUPT_IRQ_NUMBER, osal_interrupt_handler, NULL, "OsalInterrupt", NULL);
    if (osal_ret != OSAL_SUCCESS) {
        osal_printk("%s irq request failed irq=%u\r\n", OSAL_INTERRUPT_LOG, OSAL_INTERRUPT_IRQ_NUMBER);
        return false;
    }
    resources->irq_requested = true;

    osal_ret = osal_irq_set_priority(OSAL_INTERRUPT_IRQ_NUMBER, OSAL_INTERRUPT_IRQ_PRIORITY);
    if (osal_ret != OSAL_SUCCESS) {
        osal_printk("%s set priority failed irq=%u\r\n", OSAL_INTERRUPT_LOG, OSAL_INTERRUPT_IRQ_NUMBER);
        return false;
    }
    osal_irq_enable(OSAL_INTERRUPT_IRQ_NUMBER);
    osal_printk("%s registered irq=%u priority=%u\r\n", OSAL_INTERRUPT_LOG, OSAL_INTERRUPT_IRQ_NUMBER,
                OSAL_INTERRUPT_IRQ_PRIORITY);
    return true;
}

static bool osal_interrupt_collect(uint32_t *progress_failures)
{
    uint32_t reported_count = 0;
    uint32_t previous_progress = 0;

    while (reported_count < OSAL_INTERRUPT_EXPECTED_COUNT) {
        if ((reported_count >= g_osal_interrupt_count) && (osal_sem_down(&g_osal_interrupt_sem) != OSAL_SUCCESS)) {
            osal_printk("%s task wait failed\r\n", OSAL_INTERRUPT_LOG);
            return false;
        }

        uint32_t received_count = g_osal_interrupt_count;
        while (reported_count < received_count) {
            uint32_t current_progress = g_osal_interrupt_progress_snapshot[reported_count];
            uint32_t progress_delta = current_progress - previous_progress;
            if (progress_delta == 0U) {
                (*progress_failures)++;
            }
            osal_printk("%s irq=%u context=%s interrupted_progress=%u delta=%u\r\n", OSAL_INTERRUPT_LOG,
                        reported_count + 1U, g_osal_interrupt_context_snapshot[reported_count] ? "ISR" : "TASK",
                        current_progress, progress_delta);
            previous_progress = current_progress;
            reported_count++;
        }
    }
    return true;
}

static void osal_interrupt_cleanup(const osal_interrupt_resources_t *resources)
{
    g_osal_interrupt_worker_running = false;
    osal_irq_disable(OSAL_INTERRUPT_IRQ_NUMBER);
    if (resources->timer_initialized) {
        g_osal_interrupt_timer_funcs->stop(OSAL_INTERRUPT_SOURCE_INDEX);
        hal_timer_v150_interrupt_clear(OSAL_INTERRUPT_SOURCE_INDEX);
    }
    (void)osal_irq_clear(OSAL_INTERRUPT_IRQ_NUMBER);
    if (resources->irq_requested) {
        osal_irq_free(OSAL_INTERRUPT_IRQ_NUMBER, NULL);
    }
    if (resources->timer_initialized) {
        g_osal_interrupt_timer_funcs->deinit(OSAL_INTERRUPT_SOURCE_INDEX);
    }
    if (resources->timer_hal_registered) {
        timer_port_unregister_hal_funcs(OSAL_INTERRUPT_SOURCE_INDEX);
        g_osal_interrupt_timer_funcs = NULL;
    }
    if (resources->semaphore_initialized) {
        osal_sem_destroy(&g_osal_interrupt_sem);
    }
}

/**
 * @if Eng
 * @brief Register, verify, disable, and release one OSAL-managed hardware interrupt.
 * @param [in] data Unused task argument.
 * @return The task returns after completing the interrupt lifecycle test.
 * @else
 * @brief 注册、验证、禁用并释放一个由 OSAL 管理的硬件中断。
 * @param [in] data 未使用的任务参数。
 * @return 中断生命周期测试完成后任务返回。
 * @endif
 */
static int osal_interrupt_task_handler(const char *data)
{
    osal_interrupt_resources_t resources = {0};
    bool test_passed = false;
    uint32_t progress_failures = 0;
    unused(data);

    osal_interrupt_reset_state();
    if (osal_interrupt_initialize(&resources)) {
        g_osal_interrupt_worker_running = true;
        if (osal_interrupt_start_worker()) {
            osal_printk("%s worker running: continuous loop, no wait or sleep\r\n", OSAL_INTERRUPT_LOG);
            osal_interrupt_start_source();
            test_passed = osal_interrupt_collect(&progress_failures) &&
                          (g_osal_interrupt_count == OSAL_INTERRUPT_EXPECTED_COUNT) &&
                          (g_osal_interrupt_context_failures == 0U) && (progress_failures == 0U);
        } else {
            g_osal_interrupt_worker_running = false;
            osal_printk("%s create continuous worker failed\r\n", OSAL_INTERRUPT_LOG);
        }
    }

    osal_interrupt_cleanup(&resources);
    osal_printk("%s cleanup disable=PASS free=%s\r\n", OSAL_INTERRUPT_LOG, resources.irq_requested ? "PASS" : "SKIP");
    osal_printk("%s summary irq=%u context_fail=%u progress_fail=%u\r\n", OSAL_INTERRUPT_LOG, g_osal_interrupt_count,
                g_osal_interrupt_context_failures, progress_failures);
    osal_printk("%s %s\r\n", OSAL_INTERRUPT_LOG, test_passed ? "ALL TESTS PASS" : "TESTS FAILED");
    return test_passed ? 0 : -1;
}

/**
 * @if Eng
 * @brief Create the OSAL interrupt management sample task.
 * @else
 * @brief 创建 OSAL 中断管理案例任务。
 * @endif
 */
static void osal_interrupt_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)osal_interrupt_task_handler, NULL, "OsalInterruptTask",
                                      OSAL_INTERRUPT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        (void)osal_kthread_set_priority(task_handle, OSAL_INTERRUPT_TASK_PRIORITY);
        osal_kfree(task_handle);
    } else {
        osal_printk("%s create task failed\r\n", OSAL_INTERRUPT_LOG);
    }
    osal_kthread_unlock();
}

/**
 * @if Eng
 * @brief Register the OSAL interrupt management sample entry.
 * @else
 * @brief 注册 OSAL 中断管理案例入口。
 * @endif
 */
app_run(osal_interrupt_entry);
