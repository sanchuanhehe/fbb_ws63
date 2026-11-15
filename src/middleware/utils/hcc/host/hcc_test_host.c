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
#include "hcc_channel.h"
#include "soc_osal.h"
#include "hcc_service.h"
#include "hcc_bus.h"
#include "hcc_flow_ctrl.h"
#include "hcc_adapt.h"
#include "securec.h"
#include "common_def.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_TEST_HOST_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

#ifdef CONFIG_HCC_SUPPORT_TEST

#define HCC_TEST_US_IN_SECOND 1000000

/*
 * device -> host测试，开始时间由device开始测试前通知host记录，结束时间由Host接收完直接记录
 */
STATIC td_u32 hcc_test_start_test_loop(td_u8 *data)
{
    uapi_unused(data);
    hcc_debug("rx msg start test loop\r\n");
    /* schedule test task */
    osal_atomic_set(&hcc_test_get_test_info()->hcc_test_start, HCC_TEST_TASK_START);
    osal_wait_wakeup(&hcc_test_get_test_info()->hcc_test_wq);
    return EXT_ERR_SUCCESS;
}

/*
 * Host -> device测试，开始时间在测试开始时host记录，结束时间由device发送msg通知host再记录
 */
STATIC td_u32 hcc_test_stop_test_loop(td_u8 *data)
{
    uapi_unused(data);
    hcc_test_set_stop_time(hcc_get_timestamp());
    hcc_debug("\r\nrx msg stop test loop\r\n");
    if (hcc_test_get_loop_cnt() > 0 || hcc_test_get_exp_rx_cnt() > 0) {
        hcc_test_print_test_result();
    }
    return EXT_ERR_SUCCESS;
}

td_void hcc_test_get_credit(td_void)
{
    td_u32 credit = 0;
    td_s32 ret = hcc_bus_get_credit(hcc_get_handler(hcc_test_get_test_chan())->bus, &credit);
    if (ret != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("get credit failed\r\n");
        return;
    }
    hcc_debug("get credit: %d\r\n", credit);
}

STATIC td_u32 hcc_test_msg_rx_proc(td_u8 *data)
{
    uapi_unused(data);
    hcc_debug("hcc test msg\r\n");
    hcc_test();
    return EXT_SUCCESS;
}

td_void hcc_test_msg_init(td_void)
{
    hcc_message_register(hcc_test_get_test_chan(), 0, D2H_MSG_TEST,
        hcc_test_msg_rx_proc, (td_u8 *)hcc_get_handler(hcc_test_get_test_chan()));
    hcc_message_register(hcc_test_get_test_chan(), 0, D2H_MSG_TEST_START,
        hcc_test_start_test_loop, (td_u8 *)hcc_test_get_test_info());
    hcc_message_register(hcc_test_get_test_chan(), 0, D2H_MSG_TEST_STOP,
        hcc_test_stop_test_loop, (td_u8 *)hcc_test_get_test_info());
}

#ifdef CONFIG_HCC_SUPPORT_REG_OPT
#define HCC_TEST_REGISTER_READ 0
#define HCC_TEST_REGISTER_WRITE 1
#define HCC_TEST_REGISTER_ADDR 0x00007c00
#define HCC_TEST_REGISTER_VALUE 0xDDDDEEEE
td_u32 g_test_addr_base = HCC_TEST_REGISTER_ADDR;
td_u32 g_test_addr_offset = 0;
td_u32 g_test_addr_value = HCC_TEST_REGISTER_VALUE;
td_void hcc_test_register_opt(td_u32 cmd)
{
    td_u32 value = 0;
    td_u32 addr = g_test_addr_base + g_test_addr_offset;
    td_u64 start = 0;
    td_u64 stop = 0;
    switch (cmd) {
        case HCC_TEST_REGISTER_READ:
            start = hcc_get_timestamp();
            if (hcc_read_reg(hcc_test_get_test_chan(), addr, &value) != EXT_ERR_SUCCESS) {
                hcc_printf_err_log("read reg failed\r\n");
            } else {
                stop = hcc_get_timestamp();
                hcc_debug("read reg succes: 0x%x - 0x%x, time:%lluus\r\n", addr, value, stop - start);
            }
            break;

        case HCC_TEST_REGISTER_WRITE:
            start = hcc_get_timestamp();
            if (hcc_write_reg(hcc_test_get_test_chan(), addr, g_test_addr_value) !=  EXT_ERR_SUCCESS) {
                hcc_printf_err_log("write reg failed\r\n");
            } else {
                stop = hcc_get_timestamp();
                hcc_debug("write reg succes: 0x%x - 0x%x, time:%lluus\r\n", addr, g_test_addr_value, (stop - start));
            }
            break;

        default:
            hcc_printf_err_log("opt param err- %d\r\n", cmd);
            break;
    }
    g_test_addr_offset += sizeof(td_u32);
}

td_void hcc_test_set_register_addr(td_u32 addr, td_u32 offset, td_u32 value)
{
    g_test_addr_base = addr;
    g_test_addr_offset = offset;
    g_test_addr_value = value;
}
#endif

STATIC td_u32 g_post_cnt = 0;
td_void hcc_test_reset_rx_stat(td_void)
{
    g_post_cnt = 0;
}

STATIC td_u64 hcc_test_calc_trans_time_us(td_u64 start, td_u64 stop)
{
    return start > stop ? (start - stop) : (stop - start);
}

STATIC td_void hcc_test_calc_trans_rate(td_void)
{
    hcc_test_info *test_info = hcc_test_get_test_info();
    td_u64 trans_us = hcc_test_calc_trans_time_us(test_info->start_time, test_info->stop_time);
    td_u64 trans_bits = hcc_test_get_trans_bits();
    td_u32 trans_rate = 0;
    if (trans_us == 0) {
        return;
    }
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    trans_rate = div_u64((trans_bits * HCC_TEST_US_IN_SECOND), (td_u32)trans_us) >> 10; // >>10 == bits_2_Kbits
#else
    trans_rate = ((trans_bits * HCC_TEST_US_IN_SECOND) / trans_us) >> 10; // >>10 == bits_2_Kbits
#endif
    hcc_test_set_trans_time_us(trans_us);
    hcc_test_set_trans_rate(trans_rate);
}

td_void hcc_test_print_test_result(td_void)
{
    hcc_test_calc_trans_rate();
    hcc_debug("trans_bits: %u\r\n", hcc_test_get_trans_bits());
    hcc_debug("trans_time: %llu us\r\n", hcc_test_get_trans_time_us());
    hcc_debug("trans_rate: %u Kb/s\r\n", hcc_test_get_trans_rate());
    hcc_debug("g_post_cnt: %u\r\n", g_post_cnt);
    hcc_debug("start_timestamp: %llu\r\n", hcc_test_get_start_time());
    hcc_debug("stop_timestamp: %llu\r\n", hcc_test_get_stop_time());
}

td_u32 hcc_test_rx_proc(hcc_queue_type queue_id, td_u8 sub_type, td_u8 *buf, td_u32 len, td_u8 *user_param)
{
    if (sub_type == HCC_TEST_SUBTYPE_COMMAND) {
        td_u8 argc;
        td_u32 *argv;
        argc = *(buf + sizeof(hcc_header));
        if (len < (sizeof(hcc_header) + HCC_TEST_REMOTE_CMD_ARGC_LEN + argc * sizeof(td_u32))) {
            hcc_printf_err_log("cmd len err:%d - arc len: %d\r\n", len, argc);
        } else {
            argv = (td_u32 *)(buf + sizeof(hcc_header) + HCC_TEST_REMOTE_CMD_ARGC_LEN);
            hcc_test_proc_local_cmd(argv, argc);
        }
    } else if (sub_type == HCC_TEST_SUBTYPE_DATA) {
        if (++g_post_cnt == hcc_test_get_exp_rx_cnt()) {
            if (hcc_test_get_m2s_status() == TD_TRUE) {
                hcc_send_message(hcc_test_get_test_chan(), D2H_MSG_TEST_STOP, 0);
            } else {
                hcc_test_set_stop_time(hcc_get_timestamp());
                hcc_debug("test stop\r\n");
                hcc_test_print_test_result();
            }
        }
    }

    hcc_adapt_test_free(queue_id, buf, user_param);
    return EXT_ERR_SUCCESS;
}

td_void hcc_test_start(td_void)
{
    hcc_test_set_start_time(hcc_get_timestamp());
    hcc_test_set_stop_time(hcc_test_get_start_time()); /* stop的值与start值一致，测试结束再记录实际的停止测试时间 */
}

td_void hcc_test_start_test_after_init_paras(td_void)
{
    td_u32 cmd_param[] = {
        HCC_TEST_CMD_INIT_ALL_PARAM_AND_START_TEST,
        hcc_test_get_exp_rx_cnt(),
        hcc_test_get_frame_size(),
        hcc_test_get_loop_cnt()
    };
    /* 双向发送包数量一致 */
    if (hcc_test_get_m2s_status() == TD_TRUE) {
        hcc_send_message(hcc_test_get_test_chan(), D2H_MSG_TEST_START, 0);
        hcc_test_start_test_loop(0);
    } else {
        hcc_test_proc_remote_cmd(cmd_param, uapi_array_size(cmd_param));
    }
}

#if !(defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
#include "string.h"
STATIC td_u32 hcc_cmd_integer_check(TD_CONST td_char *val)
{
    size_t len;
    td_u32 i;
    td_char *buf = (td_char *)val;
    if (buf == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    len = strlen(buf);
    if ((*buf == '0') && (len != 1)) {
        return EXT_ERR_FAILURE;
    }
    for (i = 0; i < len; i++) {
        if ((*buf < '0') || (*buf > '9')) {
            return EXT_ERR_FAILURE;
        }
        buf++;
    }
    return EXT_ERR_SUCCESS;
}

/* here we provide hcc_test_atoi to avoid duplicated definition of atoi which was first defined in test_suite_log.c
  * for NONOS project. the whole hcc_test.c will not be included in release version */
STATIC int hcc_test_atoi(TD_CONST char *src)
{
    int s = 0;
    td_bool isMinus = TD_FALSE;

    while (*src == ' ') {
        src++;
    }

    if (*src == '+' || *src == '-') {
        if (*src == '-') {
            isMinus = TD_TRUE;
        }
        src++;
    }
    else if (*src < '0' || *src > '9') {
        s = EXT_ERR_FAILURE;
        return s;
    }

    while (*src != '\0' && *src >= '0' && *src <= '9') {
        s = s * 10 + *src - '0';
        src++;
    }
    return s * (isMinus ? -1 : 1);
}

STATIC td_s32 hcc_test_cmd_atoi(int argc, char *argv[], hcc_test_cmd_args *cmd_args)
{
    int i;

    if ((argc < 1) || (argc > (int)uapi_array_size(cmd_args->argv))) {
        hcc_printf_err_log("[ERROR] invalid argc\n");
        return EXT_ERR_FAILURE;
    }

    cmd_args->argc = argc;

    for (i = 0; i < argc; i++) {
        if (hcc_cmd_integer_check(argv[i]) != EXT_ERR_SUCCESS) {
            hcc_printf_err_log("invalid argv:[%d]\n", i);
            return EXT_ERR_FAILURE;
        }
        cmd_args->argv[i] = (td_u32)hcc_test_atoi(argv[i]);
    }

    return EXT_ERR_SUCCESS;
}

int hcc_test_cmd_proc(int argc, char *argv[])
{
    ext_errno ret = EXT_ERR_FAILURE;
    hcc_test_cmd_args cmd_args;

    ret = hcc_test_cmd_atoi(argc, argv, &cmd_args);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    if (cmd_args.argv[0] == HCC_TEST_CMD_REMOTE) {
        return hcc_test_proc_remote_cmd(&cmd_args.argv[1], cmd_args.argc - 1);
    } else {
        return hcc_test_proc_local_cmd(&cmd_args.argv[0], cmd_args.argc);
    }
}
#endif

td_s32 hcc_test_proc_remote_cmd(td_u32 *argv, td_u32 argc)
{
    hcc_transfer_param param = {
        .service_type = hcc_test_get_test_service(),
        .queue_id = hcc_test_get_test_queue_id(),
        .sub_type = HCC_TEST_SUBTYPE_COMMAND,
    };

    td_u8 *buf = TD_NULL;
    td_u8 *payload = TD_NULL;
    td_u8 *user_param = TD_NULL;
    td_u32 ret;
    td_u8 chl = hcc_test_get_test_chan();
    size_t data_size = osal_padding(sizeof(hcc_header) + HCC_TEST_REMOTE_CMD_ARGC_LEN + argc * sizeof(td_u32),
                                    hcc_get_channel_align_len(chl));

    if (argc < 1) {
        hcc_printf_err_log("[ERROR] remote cmd need more arguments\n");
        return EXT_ERR_FAILURE;
    }

    ret = hcc_adapt_alloc_priv_buf(hcc_get_handler(chl), param.queue_id, data_size, &buf, &user_param);
    if (ret != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("malloc user_param ERR\r\n");
        return EXT_ERR_FAILURE;
    }
    memset_s(buf, data_size, 0, data_size);
    param.user_param = user_param;

    /*
     * 4Bytes | 4Byte | 4Bytes  | 4Bytes  | 4Bytes  |
     *----------------------------------------------------------------------------------
     * header | argc  | argv[1] | argv[2] | argv[3] |
     *        |       | [para1] | [para2] | [para3] |
     *----------------------------------------------------------------------------------
     */
    payload = buf + sizeof(hcc_header);
    *((td_u32 *)payload) = argc;
    if (memcpy_s(payload + HCC_TEST_REMOTE_CMD_ARGC_LEN, data_size - sizeof(hcc_header) - HCC_TEST_REMOTE_CMD_ARGC_LEN,
                 argv, argc * sizeof(td_u32)) != EXT_ERR_SUCCESS) {
        hcc_adapt_test_free(param.queue_id, buf, user_param);
        hcc_printf_err_log("para cpy err\r\n");
        return EXT_ERR_FAILURE;
    }

    if (hcc_tx_data(chl, buf, data_size, &param) != EXT_ERR_SUCCESS) {
        hcc_adapt_test_free(param.queue_id, buf, user_param);
        return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}
#endif
