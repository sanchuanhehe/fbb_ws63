/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: HCC TEST
 * Author: CompanyName
 */
#include "hcc_test.h"
#include "hcc_cfg_comm.h"
#include "hcc_if.h"
#include "hcc.h"
#include "hcc_comm.h"
#include "hcc_bus.h"
#include "hcc_service.h"
#include "hcc_flow_ctrl.h"
#include "hcc_channel.h"
#include "common_def.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_TEST_SLAVE_ROM_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

STATIC td_u32 hcc_test_loop_proc(td_u8 *data)
{
    uapi_unused(data);
    hcc_printf("slave rx: hcc test msg\r\n");
    hcc_printf("HCC TEST 2 DIR\r\n");
    hcc_test();
    return EXT_SUCCESS;
}

STATIC td_u32 hcc_test_msg_rx_proc(td_u8 *data)
{
    uapi_unused(data);
    hcc_printf("slave rx: hcc test msg\r\n");
    hcc_send_message(hcc_test_get_test_chan(), D2H_MSG_TEST_STOP, 0);
    return EXT_SUCCESS;
}

STATIC td_u32 hcc_test_msg_init_module(td_u8 *data)
{
    uapi_unused(data);
    hcc_printf("slave rx: hcc test init\r\n");
    hcc_test_init();
    return EXT_SUCCESS;
}

td_void hcc_test_get_credit(td_void)
{
    return;
}

td_void hcc_test_msg_init(td_void)
{
    hcc_message_register(hcc_test_get_test_chan(), 0, H2D_MSG_TEST_LOOP,
        hcc_test_loop_proc, (td_u8 *)hcc_get_handler(hcc_test_get_test_chan()));
    hcc_message_register(hcc_test_get_test_chan(), 0, H2D_MSG_TEST,
        hcc_test_msg_rx_proc, (td_u8 *)hcc_get_handler(hcc_test_get_test_chan()));
    hcc_message_register(hcc_test_get_test_chan(), 0, H2D_MSG_TEST_INIT,
        hcc_test_msg_init_module, (td_u8 *)hcc_get_handler(hcc_test_get_test_chan()));
}

STATIC td_u32 g_post_cnt = 0;
STATIC td_void hcc_test_reset_post_stat(td_void)
{
    g_post_cnt = 0;
}

td_void hcc_test_reset_rx_stat(td_void)
{
    hcc_test_reset_post_stat();
}

td_void hcc_test_print_test_result(td_void)
{
    hcc_debug("g_post_cnt: %d\r\n", g_post_cnt);
    hcc_debug("exp rx cnt: %d\n", hcc_test_get_exp_rx_cnt());
}

td_u32 hcc_test_rx_proc(hcc_queue_type queue_id, td_u8 sub_type, td_u8 *buf, td_u32 len, td_u8 *user_param)
{
    td_u8 argc;
    td_u32 *argv;
    if (sub_type == HCC_TEST_SUBTYPE_COMMAND) {
        argc = *(buf + sizeof(hcc_header));
        if (len < (sizeof(hcc_header) + HCC_TEST_REMOTE_CMD_ARGC_LEN + argc * sizeof(td_u32))) {
            hcc_debug("cmd len err:%d - arc len: %d\r\n", len, argc);
        } else {
            argv = (td_u32 *)(buf + sizeof(hcc_header) + HCC_TEST_REMOTE_CMD_ARGC_LEN);
            hcc_test_proc_local_cmd(argv, argc);
        }
    } else if (sub_type == HCC_TEST_SUBTYPE_DATA) {
        if (++g_post_cnt == hcc_test_get_exp_rx_cnt()) {
            hcc_send_message(hcc_test_get_test_chan(), D2H_MSG_TEST_STOP, 0);
        }
    }

    hcc_adapt_test_free(queue_id, buf, user_param);
    return EXT_ERR_SUCCESS;
}

td_void hcc_test_start(td_void)
{
    hcc_send_message(hcc_test_get_test_chan(), D2H_MSG_TEST_START, 0);
}

td_void hcc_test_start_test_after_init_paras(td_void)
{
#ifdef CONFIG_HCC_SUPPORT_NON_OS
    hcc_test();
#else
    osal_atomic_set(&hcc_test_get_test_info()->hcc_test_start, HCC_TEST_TASK_START);
    osal_wait_wakeup(&hcc_test_get_test_info()->hcc_test_wq);
#endif
}
