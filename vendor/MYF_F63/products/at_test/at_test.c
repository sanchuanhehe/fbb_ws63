/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: AT_TEST Sample Source. \n
 *
 * History: \n
 * 2023-07-06, Create file. \n
 */

/**
 * 本例程写了三条AT指令：
 *     1、AT+SLEADV=1   开启星闪广播；   AT+SLEADV=0   关闭星闪广播
 *        广播默认的mac地址在 sle_server_adv.c 的117行设置。
 *     2、AT+SLECONNBYMAC=000000000001   根据mac地址来连接对端设备
 *     3、AT+SLESEND=0,10,0123456789     向对端发送数据
 *          第一个参数是连接号，如果向第二台设备发送数据，值应为1，依次类推。
 *          第二个参数是发送的数据长度。
 *          第三个参数是发送的数据。
 */

#include "at_test.h"

const at_para_parse_syntax_t at_SLECONNBYMAC_syntax[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
     .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
     .entry.string.max_length = 12,
     .last = true,
     .offset = offsetof(AtSleConnByMac_t, para1)},
};

const at_para_parse_syntax_t at_SLESEND_syntax[] = {
    {.type = AT_SYNTAX_TYPE_INT,
     .attribute = AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_AT_MIN_VALUE,
     .entry.int_range.min_val = 0,
     .entry.int_range.max_val = 15,
     .offset = offsetof(AtSlesendT, para1)},
    {.type = AT_SYNTAX_TYPE_INT,
     .attribute = AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_AT_MIN_VALUE,
     .entry.int_range.min_val = 1,
     .entry.int_range.max_val = 1500,
     .offset = offsetof(AtSlesendT, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
     .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
     .entry.string.max_length = 1500,
     .last = true,
     .offset = offsetof(AtSlesendT, para3)},
};

const at_para_parse_syntax_t at_SLEADV_syntax[] = {
    {.type = AT_SYNTAX_TYPE_INT,
     .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
     .entry.int_range.max_val = 1,
     .entry.int_range.min_val = 0,
     .last = true,
     .offset = offsetof(AtSleadvT, para1)},
};

at_ret_t SLECONNBYMAC_my_set(AtSleConnByMac_t *args)
{
    int len = strlen((char *)args->para1);
    uint8_t mac[10];
    if (len != MAC_HEX_LEN)
        return AT_RET_SYNTAX_ERROR;
    sscanf((char *)args->para1, MY_EXT_AT_MACSTR, &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

    sle_addr_t device_addr;
    device_addr.type = 0;
    memcpy(device_addr.addr, mac, MAC_LEN);
    sle_connect_remote_device(&device_addr);
    return AT_RET_OK;
}

at_ret_t SLESEND_my_set(AtSlesendT *args)
{
    if (strlen((char *)args->para3) != (uint32_t)args->para2)
        return AT_RET_SYNTAX_ERROR;

    ssapc_write_param_t sle_uart_send_param;
    sle_uart_send_param.handle = 1;
    sle_uart_send_param.type = 0;
    sle_uart_send_param.data_len = args->para2;
    sle_uart_send_param.data = args->para3;

    // 向对端发送数据
    ssapc_write_req(0, args->para1, &sle_uart_send_param);
    return AT_RET_OK;
}

at_ret_t SLEADV_my_set(AtSleadvT *args)
{
    if (args->para1 == 1) {
        // 开启广播
        sle_uuid_server_adv_init();
    } 
    else {
        // 关闭广播
        sle_stop_announce(SLE_ADV_HANDLE_DEFAULT);
    }
    return AT_RET_OK;
}

const at_cmd_entry_t at_myf_parse_table[] = {
    {
        "SLECONNBYMAC",
        1,
        0,
        at_SLECONNBYMAC_syntax,
        NULL,
        (at_set_func_t)SLECONNBYMAC_my_set,
        NULL,
        NULL,
    },
    {
        "SLESEND",
        2,
        0,
        at_SLESEND_syntax,
        NULL,
        (at_set_func_t)SLESEND_my_set,
        NULL,
        NULL,
    },
    {
        "SLEADV",
        3,
        0,
        at_SLEADV_syntax,
        NULL,
        (at_set_func_t)SLEADV_my_set,
        NULL,
        NULL,
    },

};

static int at_test_task(const char *arg)
{
    unused(arg);

    enable_sle();
    at_uart_init();
    char mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
    sle_addr_t sle_addr;
    sle_addr.type = 0;
    memcpy(sle_addr.addr, mac, MAC_LEN);
    sle_set_local_addr(&sle_addr);

    sle_client_init();
    sle_uuid_server_init();

    uapi_at_register_cmd(at_myf_parse_table, AT_MY_FUNC_NUM);

    return 0;
}

static void at_test_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)at_test_task, 0, "AtTestTask", AT_TEST_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, AT_TEST_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the at_test_entry. */
app_run(at_test_entry);