/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: HCC TEST
 * Author: CompanyName
 */

#include "hcc_comm.h"
#include "hcc_if.h"
#include "hcc_dfx.h"
#include "hcc.h"
#include "hcc_bus.h"
#include "soc_osal.h"
#include "hcc_channel.h"
#include "hcc_adapt.h"
#include "hcc_service.h"
#include "securec.h"
#include "common_def.h"
#include "hcc_ipc_adapt.h"
#include "hcc_test.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_TEST_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

#ifdef CONFIG_HCC_SUPPORT_TEST
#ifdef CONFIG_HCC_SUPPORT_TEST_SYSCH
STATIC hcc_test_info g_test_info = {
    .test_para.loop_cnt = 0,
    .test_para.frame_size = 0,
    .test_para.expect_rx_cnt = 0,
    .test_para.test_queue_id = (hcc_queue_type)SYSCH_TEST_QUEUE,
    .test_para.channel_id = HCC_CHANNEL_AP,
    .test_para.test_data = 0x01, // 测试数据  0x5a
    .test_para.test_service = HCC_ACTION_TYPE_TEST,
    .test_para.master_to_slave = TD_FALSE,

    .test_stat = {
        .test_init = TD_FALSE,
        .test_stop = TD_FALSE,
    },
};
#else
STATIC hcc_test_info g_test_info = {
    .test_para.loop_cnt = 0,
    .test_para.frame_size = 0,
    .test_para.expect_rx_cnt = 0,
    .test_para.test_queue_id = HCC_TEST_QUEUE,
    .test_para.channel_id = HCC_CHANNEL_AP,
    .test_para.test_data = 0x01, // 测试数据  0x5a
    .test_para.test_service = HCC_ACTION_TYPE_TEST,
    .test_para.master_to_slave = TD_FALSE,

    .test_stat = {
        .test_init = TD_FALSE,
        .test_stop = TD_FALSE,
    },
};
#endif

hcc_test_info *hcc_test_get_test_info(td_void)
{
    return &g_test_info;
}

td_void hcc_test_set_trans_rate(td_u32 rate)
{
    g_test_info.test_data.trans_rate = rate;
}

td_void hcc_test_set_trans_time_us(td_u64 us)
{
    g_test_info.test_data.trans_time = us;
}

td_u64 hcc_test_get_start_time(td_void)
{
    return g_test_info.start_time;
}

td_void hcc_test_set_start_time(td_u64 us)
{
    hcc_printf("set start_time us: %llu us\r\n", us);
    g_test_info.start_time = us;
}

td_u64 hcc_test_get_stop_time(td_void)
{
    return g_test_info.stop_time;
}

td_void hcc_test_set_stop_time(td_u64 us)
{
    hcc_printf("set stop_time us: %llu us\r\n", us);
    g_test_info.stop_time = us;
}

td_u64 hcc_test_get_trans_time_us(td_void)
{
    return g_test_info.test_data.trans_time;
}

td_u32 hcc_test_get_trans_rate(td_void)
{
    return g_test_info.test_data.trans_rate;
}

td_u32 hcc_test_get_trans_bits(td_void)
{
    return g_test_info.test_data.trans_bits;
}

td_void hcc_test_set_trans_bits(td_u32 bits)
{
    g_test_info.test_data.trans_bits = bits;
}

td_u32 hcc_test_get_exp_rx_cnt(td_void)
{
    return g_test_info.test_para.expect_rx_cnt;
}

td_bool hcc_test_get_m2s_status(td_void)
{
    return g_test_info.test_para.master_to_slave;
}

td_u32 hcc_test_get_loop_cnt(td_void)
{
    return g_test_info.test_para.loop_cnt;
}

td_u16 hcc_test_get_frame_size(td_void)
{
    return g_test_info.test_para.frame_size;
}

td_u32 hcc_test_get_test_service(td_void)
{
    return g_test_info.test_para.test_service;
}

td_bool hcc_test_is_test_stop(td_void)
{
    return g_test_info.test_stat.test_stop;
}

td_void hcc_test_set_test_stop(td_bool stop)
{
    g_test_info.test_stat.test_stop = stop;
}

td_void hcc_test_set_test_queue_id(td_u32 queue_id)
{
    g_test_info.test_para.test_queue_id = (hcc_queue_type)queue_id;
}

hcc_queue_type hcc_test_get_test_queue_id(td_void)
{
    return g_test_info.test_para.test_queue_id;
}

STATIC td_void hcc_test_clear_assemble(td_u32 mode)
{
#ifndef CONFIG_HCC_SUPPORT_IPC_HOST
    td_u8 i;
    hcc_handler *hcc = hcc_get_handler(hcc_test_get_test_chan());
    hcc_queue_cfg *q_cfg = hcc->que_cfg;
#endif
    if (mode >= HCC_TRANS_MODE_BUTT) {
        return;
    }
    hcc_debug("mode:%u\r\n", mode);
#ifdef CONFIG_HCC_SUPPORT_IPC_HOST
    if (mode == HCC_IPC_TRANS_POSTMEM) {
        hcc_ipc_clear_premem();
    }
#else
    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (q_cfg[i].queue_ctrl.service_type == HCC_ACTION_TYPE_TEST) {
            q_cfg[i].queue_ctrl.transfer_mode = mode;
        }
    }
#endif
}

td_u8 hcc_test_get_test_chan(td_void)
{
    return g_test_info.test_para.channel_id;
}

td_u32 hcc_test_get_test_data(td_void)
{
    return g_test_info.test_para.test_data++;
}

STATIC td_void hcc_test_set_test_chan(td_u8 channel_id)
{
    g_test_info.test_para.channel_id = channel_id;
}

STATIC td_void hcc_test_set_exp_rx_cnt(td_u32 cnt)
{
    g_test_info.test_para.expect_rx_cnt = cnt;
}

STATIC td_void hcc_test_set_loop_cnt(td_u32 cnt)
{
    g_test_info.test_para.loop_cnt = cnt;
}

STATIC td_void hcc_test_set_frame_size(td_u16 size)
{
    g_test_info.test_para.frame_size = size;
}

STATIC td_void hcc_test_set_test_data(td_u32 data)
{
    g_test_info.test_para.test_data = data;
}

STATIC td_void hcc_test_msg(hcc_tx_msg_type msg)
{
    hcc_send_message(hcc_test_get_test_chan(), msg, 0);
}

STATIC td_void hcc_test_cmd_init_all_param(td_u32 data1, td_u32 data2, td_u32 data3)
{
    td_u32 trans_bits = 0;
    hcc_test_set_loop_cnt(data1);
    hcc_test_set_frame_size((td_u16)data2);
    hcc_test_set_exp_rx_cnt(data3);
    hcc_test_reset_rx_stat();
    trans_bits = hcc_test_get_frame_size() * 8; /* 8 : 位数 */
    trans_bits *= (hcc_test_get_loop_cnt() > 0 ? hcc_test_get_loop_cnt() : hcc_test_get_exp_rx_cnt());
    hcc_test_set_trans_bits(trans_bits);
    hcc_test_set_trans_time_us(0);
    hcc_test_set_trans_rate(0);
}

STATIC ext_errno hcc_test_command_proc(td_u32 command)
{
    switch (command) {
        case HCC_TEST_CMD_TEST_INIT:
            hcc_test_init();
            break;
        case HCC_TEST_CMD_M2S:
            hcc_test_get_test_info()->test_para.master_to_slave = TD_TRUE;
            break;
        case HCC_TEST_CMD_START_TEST:
            hcc_debug("HCC TEST 1 DIR\r\n");
            hcc_test();
            break;

        case HCC_TEST_CMD_PRINT_RESULT:
            hcc_test_print_test_result();
            break;

        case HCC_TEST_GET_CREDIT:
            hcc_test_get_credit();
            break;

        default:
            hcc_printf_err_log("hcc test cmd[%d]\r\n", command);
            return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}

STATIC ext_errno hcc_test_command_proc_p1(td_u32 command, td_u32 data)
{
    switch (command) {
        case HCC_TEST_CMD_ENABLE_RX_THREAD:
            hcc_enable_rx_thread(hcc_test_get_test_chan(), (td_bool)data);
            break;

        case HCC_TEST_CMD_ENABLE_TX_THREAD:
            hcc_enable_tx_thread(hcc_test_get_test_chan(), (td_bool)data);
            break;

        case HCC_TEST_CMD_SET_MODE:
            hcc_test_clear_assemble(data);
            break;
        case HCC_TEST_CMD_SET_QUEUE:
            hcc_test_set_test_queue_id(data);
            break;
        case HCC_TEST_CMD_TEST_MSG:
            hcc_test_msg((hcc_tx_msg_type)data);
            break;

        case HCC_TEST_PRINT_BUS_INFO:
            hcc_bus_dfx_statics_print((td_u8)data);
            break;

        case HCC_TEST_UPDATE_CREDIT:
            hcc_bus_update_credit(hcc_get_handler(hcc_test_get_test_chan())->bus, data);
            break;

        case HCC_TEST_CMD_SET_TX_DATA:
            hcc_test_set_test_data(data);
            break;

        case HCC_TEST_CMD_SET_TX_CHANNEL:
            hcc_test_set_test_chan((td_u8)data);
            break;
#ifdef CONFIG_HCC_SUPPORT_REG_OPT
        case HCC_TEST_CMD_TEST_REG_OPT:
            hcc_test_register_opt(data);
            break;
#endif
        default:
            hcc_printf_err_log("hcc test cmd p1\r\n");
            return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}

STATIC ext_errno hcc_test_command_proc_p2(td_u32 command, td_u32 data1, td_u32 data2)
{
    switch (command) {
        case HCC_TEST_PRINT_QUEUE_INFO:
            hcc_dfx_queue_info_print(hcc_test_get_test_chan(), data1, data2);
            break;
        case HCC_TEST_PRINT_SERVICE_INFO:
            hcc_dfx_service_info_print(data1, (hcc_service_type)data2);
            break;
        default:
            hcc_printf_err_log("hcc test cmd p2\r\n");
            return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}

STATIC ext_errno hcc_test_command_proc_p3(td_u32 command, td_u32 data1, td_u32 data2, td_u32 data3)
{
    switch (command) {
#ifdef CONFIG_HCC_SUPPORT_REG_OPT
        case HCC_TEST_CMD_SET_REG_ADDR:
            hcc_test_set_register_addr(data1, data2, data3);
            break;
#endif
        case HCC_TEST_CMD_INIT_ALL_PARAM:
            hcc_test_cmd_init_all_param(data1, data2, data3);
            break;

        case HCC_TEST_CMD_INIT_ALL_PARAM_AND_START_TEST:
            hcc_debug("hcc test start:\r\n");
            hcc_test_cmd_init_all_param(data1, data2, data3);
            /* 设置完参数启动测试流程 */
            hcc_test_start_test_after_init_paras();
            /* 收到测试停止后计算速率并打印 */
            break;

        default:
            hcc_printf_err_log("hcc test cmd p3\r\n");
            return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}

hcc_test_proc g_hcc_test_proc = TD_NULL;
td_void hcc_test_cmd_register(hcc_test_proc test_proc)
{
    g_hcc_test_proc = test_proc;
}

td_s32 hcc_test_proc_local_cmd(td_u32 *argv, td_u32 argc)
{
    ext_errno ret = EXT_ERR_FAILURE;
    switch (argc) {
        case 1:
            ret = hcc_test_command_proc(argv[0]);
            break;

        case 2: /* 2: 参数数量 */
            ret = hcc_test_command_proc_p1(argv[0], argv[1]);
            break;

        case 3: /* 3: 参数数量 */
            ret = hcc_test_command_proc_p2(argv[0], argv[1], argv[2]); /* 2 : index of parameters */
            break;

        case 4: /* 4: 参数数量 */
            /* 2 : index of parameters 3 : index of parameters */
            ret = hcc_test_command_proc_p3(argv[0], argv[1], argv[2], argv[3]);
            break;

        default:
            hcc_printf_err_log("hcc test local argc err\r\n");
            break;
    }
    if (g_hcc_test_proc != TD_NULL) {
        return g_hcc_test_proc(argv, argc);
    }
    return ret;
}
#endif
