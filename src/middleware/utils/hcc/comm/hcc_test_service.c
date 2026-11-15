/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: HCC TEST
 * Author: CompanyName
 */

#include "hcc_test.h"
#include "hcc_comm.h"
#include "hcc_if.h"
#include "hcc.h"
#include "hcc_bus.h"
#include "soc_osal.h"
#include "hcc_channel.h"
#include "hcc_adapt.h"
#include "hcc_service.h"
#include "securec.h"
#include "common_def.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_TEST_SERVICE_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

#ifdef CONFIG_HCC_SUPPORT_TEST

#define PKT_LEN_FOR_PERFORMANCE_TEST 1664
#define HCC_TEST_TASK_PRIORITY 4
#define HCC_TEST_TASK_STACK_SIZE 0x800
/* the memory will be exhausted in device when perform the performance test(send more than 40 packets in assemble mode)
 * we will reuse one buf without freeing to prevent allocation failure as we do not care about the contents of the
 * packet  in performance  test  */
STATIC td_u8 *g_hcc_perf_test_buf = TD_NULL;

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
STATIC td_void hcc_test_stop_subq(hcc_queue_type queue_id)
{
    uapi_unused(queue_id);
    hcc_test_set_test_stop(TD_TRUE);
    hcc_printf("test stop subq\r\n");
}

STATIC td_void hcc_test_start_subq(hcc_queue_type queue_id)
{
    uapi_unused(queue_id);
    hcc_test_set_test_stop(TD_FALSE);
    hcc_printf("test start subq\r\n");
}
#endif

STATIC td_u32 hcc_adapt_test_alloc(hcc_queue_type queue_id, td_u32 len, td_u8 **buf, td_u8 **user_param)
{
    uapi_unused(queue_id);
    if (len == PKT_LEN_FOR_PERFORMANCE_TEST && g_hcc_perf_test_buf != TD_NULL) {
        *buf = g_hcc_perf_test_buf;
    } else {
        *buf = (td_u8 *)osal_kmalloc(len, OSAL_GFP_KERNEL);
        if (*buf == TD_NULL) {
            hcc_debug("len:%u\r\n", len);
            return EXT_ERR_FAILURE;
        }

        if (len == PKT_LEN_FOR_PERFORMANCE_TEST) {
            g_hcc_perf_test_buf = *buf;
        }
    }

    hcc_printf("adapt alloc - %p\r\n", *buf);
    // 不需要memset成0，外面有赋值
    *user_param = TD_NULL;
    return EXT_ERR_SUCCESS;
}

td_void hcc_adapt_test_free(hcc_queue_type queue_id, td_u8 *buf, td_u8 *user_param)
{
    uapi_unused(queue_id);
    uapi_unused(user_param);
    hcc_printf("adapt free - %p\r\n", buf);
    if (buf != g_hcc_perf_test_buf) {
        osal_kfree(buf);
    }
}

static hcc_adapt_ops g_hcc_test_adapt = {
    .free = hcc_adapt_test_free,
    .alloc = hcc_adapt_test_alloc,
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    .start_subq = hcc_test_start_subq,
    .stop_subq = hcc_test_stop_subq,
#endif
    .rx_proc = hcc_test_rx_proc,
};

STATIC td_void hcc_test_fill_frame_content(td_u8 *buf, td_u32 data_size, TD_CONST hcc_transfer_param *param)
{
    td_u32 test_tx_data = hcc_test_get_test_data();
    uapi_unused(param);
    memset_s(buf + sizeof(hcc_header), data_size - sizeof(hcc_header),
             test_tx_data, data_size - sizeof(hcc_header));
}

#ifndef CONFIG_HCC_SUPPORT_NON_OS
STATIC int hcc_test_thread_wait_cond(TD_CONST td_void *param)
{
    uapi_unused(param);
    return (osal_atomic_read(&hcc_test_get_test_info()->hcc_test_start) == HCC_TEST_TASK_START) ||
        (osal_kthread_should_stop() != 0);
}

STATIC td_s32 hcc_test_thread(td_void *data)
{
    uapi_unused(data);
    for (;;) {
        osal_wait_interruptible(&hcc_test_get_test_info()->hcc_test_wq, hcc_test_thread_wait_cond, TD_NULL);
        if (osal_kthread_should_stop() != 0) {
            break;
        }
        hcc_test();
        osal_atomic_set(&hcc_test_get_test_info()->hcc_test_start, HCC_TEST_TASK_STOP);
    }
    hcc_printf("hcc test transfer thread done!\r\n");
    return EXT_ERR_SUCCESS;
}
#endif

STATIC td_void hcc_test_task_init(hcc_test_info *test_info)
{
    /* DEVICE测试初始化 */
    if (hcc_test_get_m2s_status() == TD_FALSE) {
        hcc_send_message(hcc_test_get_test_chan(), H2D_MSG_TEST_INIT, 0);
    }

#ifdef CONFIG_HCC_SUPPORT_NON_OS
    uapi_unused(test_info);
#else
    osal_atomic_set(&test_info->hcc_test_start, HCC_TEST_TASK_STOP);
    if (osal_wait_init(&test_info->hcc_test_wq) != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("hcc test task init failed\r\n");
        return;
    }

    test_info->hcc_test_thread_handler = osal_kthread_create(
        hcc_test_thread, TD_NULL, "hcc_test", HCC_TEST_TASK_STACK_SIZE);
    if (test_info->hcc_test_thread_handler == TD_NULL) {
        osal_wait_destroy(&test_info->hcc_test_wq);
        return;
    }
    osal_kthread_set_priority(test_info->hcc_test_thread_handler, HCC_TEST_TASK_PRIORITY);
#endif
}

#ifndef CONFIG_HCC_SUPPORT_NON_OS
STATIC td_void hcc_test_task_deinit(hcc_test_info *test_info)
{
    osal_kthread_destroy(test_info->hcc_test_thread_handler, TD_TRUE);
    osal_wait_destroy(&test_info->hcc_test_wq);
}
#endif

STATIC td_void hcc_test_print_time(td_u64 t1, td_u64 t2)
{
#ifdef HCC_PRINT_PERFORM
    td_u64 t = hcc_test_get_us();
    hcc_debug("T0:%llu, T1:%llu\r\n", hcc_calc_time_us(t1, t2), hcc_calc_time_us(t2, t));
#else
    uapi_unused(t1);
    uapi_unused(t2);
#endif
}

td_void hcc_test(td_void)
{
    td_u32 data_size = hcc_test_get_frame_size();
    hcc_transfer_param param = {
        .service_type = hcc_test_get_test_service(),
        .sub_type = HCC_TEST_SUBTYPE_DATA,
        .queue_id = hcc_test_get_test_queue_id(),
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
        .fc_flag = HCC_FC_NET | HCC_FC_WAIT,
#else
        .fc_flag = HCC_FC_NONE,
#endif
    };

    td_u8 *buf = TD_NULL;
    td_u8 *user_param = TD_NULL;
    td_u32 i;
    td_u32 ret;
    td_u8 chl = hcc_test_get_test_chan();
    td_u32 cnt = hcc_test_get_loop_cnt();
    hcc_debug("hcc_test start:%d, cnt:%d, exp:%d\r\n", param.queue_id, cnt, hcc_test_get_exp_rx_cnt());
    hcc_test_start();
    if (cnt == 0) {
        return;
    }

    for (i = 0; i < cnt;) {
        td_u64 t1, t2;
        t1 = hcc_test_get_us();
        if (hcc_test_is_test_stop()) {
            osal_udelay(10); // delay 10  us
        }

        ret = hcc_adapt_alloc_priv_buf(hcc_get_handler(chl), param.queue_id, data_size, &buf, &user_param);
        if (ret != EXT_ERR_SUCCESS) {
            hcc_printf_err_log("malloc user_param ERR\r\n");
            return;
        }
        hcc_printf("send cnt: %d - %d, %p, %p\r\n", i, data_size, buf, user_param);

        param.user_param = user_param;
        hcc_test_fill_frame_content(buf, data_size, &param);
        data_size &= ~(hcc_get_handler(chl)->bus->len_align - 1);
        t2 = hcc_test_get_us();
        if (hcc_tx_data(chl, buf, (td_u16)data_size, &param) == EXT_ERR_SUCCESS) {
            hcc_test_print_time(t1, t2);
            i++;
        } else {
            hcc_adapt_free_priv_buf(hcc_get_handler(chl), param.queue_id, buf, user_param);
        }
    }
}

td_void hcc_test_init(td_void)
{
    hcc_test_info *test_info = hcc_test_get_test_info();
    if (test_info->test_stat.test_init == TD_TRUE) {
        hcc_printf_err_log("hcc test service has been initiated\r\n");
        return;
    }
    hcc_service_init(hcc_test_get_test_chan(), HCC_ACTION_TYPE_TEST, &g_hcc_test_adapt);
    hcc_test_task_init(test_info);
    test_info->test_stat.test_init = TD_TRUE;
}

td_void hcc_test_deinit(td_void)
{
    hcc_test_info *test_info = hcc_test_get_test_info();
    hcc_service_deinit(hcc_test_get_test_chan(), HCC_ACTION_TYPE_TEST);
#ifndef CONFIG_HCC_SUPPORT_NON_OS
    hcc_test_task_deinit(test_info);
#endif
    test_info->test_stat.test_init = TD_FALSE;
}
#endif
