/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: WS63 RTC software timer sample source. \n
 */

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "chip_core_irq.h"
#include "rtc.h"
#include "soc_osal.h"
#include "app_init.h"

#define RTC_SAMPLE_TASK_STACK_SIZE       0x1000U
#define RTC_SAMPLE_TASK_PRIORITY         OSAL_TASK_PRIORITY_MIDDLE
#define RTC_SAMPLE_IRQ_PRIORITY          1U
#define RTC_SAMPLE_EXPECTED_CALLBACKS    3U
#define RTC_SAMPLE_POLL_INTERVAL_MS      10U
#define RTC_SAMPLE_TIMEOUT_MARGIN_MS     1000U

static volatile uint32_t g_rtc_callback_count = 0U;
static volatile uint32_t g_rtc_callback_data = 0U;

/**
 * @brief Record an RTC expiration. This callback runs in interrupt context. \n
 * @param data Value passed by uapi_rtc_start(). \n
 */
static void rtc_sample_timeout_callback(uintptr_t data)
{
    g_rtc_callback_data = (uint32_t)data;
    g_rtc_callback_count++;
}

static bool rtc_sample_wait_callback(uint32_t expected_count)
{
    uint32_t waited_ms = 0U;
    uint32_t timeout_ms = CONFIG_RTC_SAMPLE_PERIOD_MS + RTC_SAMPLE_TIMEOUT_MARGIN_MS;

    while ((g_rtc_callback_count < expected_count) && (waited_ms < timeout_ms)) {
        (void)osal_msleep(RTC_SAMPLE_POLL_INTERVAL_MS);
        waited_ms += RTC_SAMPLE_POLL_INTERVAL_MS;
    }

    return g_rtc_callback_count >= expected_count;
}

static errcode_t rtc_sample_init(rtc_handle_t *rtc)
{
    errcode_t ret = uapi_rtc_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("[rtc] uapi_rtc_init failed: 0x%x\r\n", ret);
        return ret;
    }

    ret = uapi_rtc_adapter(RTC_0, RTC_0_IRQN, RTC_SAMPLE_IRQ_PRIORITY);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[rtc] uapi_rtc_adapter failed: 0x%x\r\n", ret);
        (void)uapi_rtc_deinit();
        return ret;
    }

    ret = uapi_rtc_create(RTC_0, rtc);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[rtc] uapi_rtc_create failed: 0x%x\r\n", ret);
        (void)uapi_rtc_deinit();
        return ret;
    }
    return ERRCODE_SUCC;
}

static bool rtc_sample_verify_once(rtc_handle_t rtc, uint32_t sample, uint32_t irq_count_before)
{
    errcode_t ret = uapi_rtc_start(rtc, CONFIG_RTC_SAMPLE_PERIOD_MS,
                                   rtc_sample_timeout_callback, (uintptr_t)sample);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[rtc] sample #%u start failed: 0x%x\r\n", sample, ret);
        return false;
    }

    if (!rtc_sample_wait_callback(sample)) {
        osal_printk("[rtc] sample #%u timeout\r\n", sample);
        (void)uapi_rtc_stop(rtc);
        return false;
    }

    if (g_rtc_callback_data != sample) {
        osal_printk("[rtc] sample #%u data mismatch: actual=%u\r\n", sample, g_rtc_callback_data);
        return false;
    }

    osal_printk("[rtc] sample #%u callback received, irq_count=%u\r\n", sample,
                uapi_rtc_int_cnt_record_get(RTC_0) - irq_count_before);
    return true;
}

static int rtc_sample_task(void *data)
{
    rtc_handle_t rtc = NULL;
    uint32_t passed_callbacks = 0U;
    uint32_t irq_count_before;
    errcode_t ret;

    unused(data);
    ret = rtc_sample_init(&rtc);
    if (ret != ERRCODE_SUCC) {
        return (int)ret;
    }

    irq_count_before = uapi_rtc_int_cnt_record_get(RTC_0);
    osal_printk("[rtc] ready: period=%ums max=%ums samples=%u\r\n",
                CONFIG_RTC_SAMPLE_PERIOD_MS, uapi_rtc_get_max_ms(), RTC_SAMPLE_EXPECTED_CALLBACKS);
    for (uint32_t sample = 1U; sample <= RTC_SAMPLE_EXPECTED_CALLBACKS; sample++) {
        if (!rtc_sample_verify_once(rtc, sample, irq_count_before)) {
            break;
        }
        passed_callbacks++;
    }

    (void)uapi_rtc_stop(rtc);
    (void)uapi_rtc_delete(rtc);
    (void)uapi_rtc_deinit();

    if (passed_callbacks == RTC_SAMPLE_EXPECTED_CALLBACKS) {
        osal_printk("[rtc] verification passed: callbacks=%u\r\n", passed_callbacks);
    } else {
        osal_printk("[rtc] verification failed: passed=%u expected=%u\r\n",
                    passed_callbacks, RTC_SAMPLE_EXPECTED_CALLBACKS);
    }
    return 0;
}

static void rtc_sample_entry(void)
{
    osal_task *task = NULL;

    osal_kthread_lock();
    task = osal_kthread_create(rtc_sample_task, NULL, "RtcSample", RTC_SAMPLE_TASK_STACK_SIZE);
    if (task == NULL) {
        osal_kthread_unlock();
        osal_printk("[rtc] create task failed\r\n");
        return;
    }
    (void)osal_kthread_set_priority(task, RTC_SAMPLE_TASK_PRIORITY);
    osal_kfree(task);
    osal_kthread_unlock();
}

app_run(rtc_sample_entry);
