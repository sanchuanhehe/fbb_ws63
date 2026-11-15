/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell source
 *
 * History:
 * 2025-5-26, Create file.
 */

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "mesh/main.h"
#include "mesh/access.h"
#include "bts_def.h"
#include "model_opcode.h"
#include "device_property.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_hex.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_prov.h"

#define BT_MESH_PUB_KEY_LEN 64
#define BT_MESH_AUTH_SET_PID_LEN 8
#define BT_MESH_AUTH_SET_AUTH_LEN 32
#if MYNEWT_VAL(BLE_MESH_PROVISIONER)
typedef struct {
    uint32_t para_map;
    uint32_t addr_type;
    const char *addr;
} ble_at_addr_t;

at_ret_t at_bt_mesh_fix_mac_net(const ble_at_addr_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_fix_mac_net args[1]:%d,  args[2]:%s\r\n", args->addr_type, args->addr);
    bd_addr_t net_addr = {0};
    net_addr.type = args->addr_type;
    uint8_t ret = bt_mesh_at_str_to_data(args->addr, BD_ADDR_LEN, net_addr.addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        BT_ERR("ble mesh at link common addr error type:%u\n", net_addr.type);
        return AT_RET_CMD_PARA_ERROR;
    }
    ble_mesh_set_prov_addr(&net_addr);
    return AT_RET_OK;
}

const at_para_parse_syntax_t g_ble_mesh_at_addr_params[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(ble_at_addr_t, addr_type)},
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 14,
        .last = true,
        .offset = offsetof(ble_at_addr_t, addr)},
};
#endif

void __attribute__((weak)) ble_mesh_sample_set_uuid_value(uint8_t *uuid, uint8_t len)
{
    BT_INFO_SHELL("not implementation func %s", __FUNCTION__); // 1 is uuid para index
    return;
}

uint32_t __attribute__((weak)) ble_mesh_sample_set_auth_value(const char *pid, const char *auth)
{
    BT_INFO_SHELL("not implementation func %s", __FUNCTION__); // 1 is uuid para index
    return 0;
}

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
    char                 *para10;
    char                 *para11;
    char                 *para12;
    char                 *para13;
} ble_mesh_prov_info_t;

const at_para_parse_syntax_t ble_mesh_prov_info[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_prov_info_t, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_prov_info_t, para13)
    },
};

static uint32_t cmd_ble_mesh_prov_uuid_set(int argc, const char *argv[])
{
    if (argc != 3) { // 3 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "uuid") != 0) { // 1 is uuid para index
        BT_INFO_SHELL("argv err %s, not uuid", argv[1]); // 1 is uuid para index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strlen(argv[2]) > 32) { // 2 is uuid value index, 32 is uuid len
        BT_INFO_SHELL("argv err uuid para too long %d", strlen(argv[2])); // 2 is uuid value index
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t uuid[16] = {0};  // 16:size
    if (ble_mesh_hex2bin(argv[2], uuid, 16) < 0) {  // 2:index; 16:size
        BT_INFO_SHELL("wrong params");
        return ERRCODE_BLE_MESH_FAIL;
    }

    ble_mesh_sample_set_uuid_value(uuid, sizeof(uuid));
    return ERRCODE_BLE_MESH_SUCCESS;
}

static uint32_t cmd_ble_mesh_prov_auth_set(int argc, const char *argv[])
{
    if (argc != 5) { // 5 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "pid") != 0 || strcmp(argv[3], "auth") != 0) { // 1 and 3 are pid and auth para index
        BT_INFO_SHELL("argv err %s, %s not uuid or auth", argv[1], argv[3]); // 1 and 3 are pid and auth para index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strlen(argv[2]) != BT_MESH_AUTH_SET_PID_LEN) { // 2 is pid para
        BT_INFO_SHELL("pid len err: current %zu chars (max 16)", strlen(argv[2])); // 2 is pid para
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (strlen(argv[4]) != BT_MESH_AUTH_SET_AUTH_LEN) { // 4 is auth para
        BT_INFO_SHELL("auth len err: current %zu chars (max 32)", strlen(argv[4])); // 4 is auth para
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ble_mesh_sample_set_auth_value(argv[2], argv[4]); // 2 and 34 are pid and auth para
}

uint32_t __attribute__((weak)) ble_mesh_sample_set_static_oob_value(const uint8_t *static_val, uint8_t size)
{
    BT_INFO_SHELL("not implementation func %s", __FUNCTION__); // 1 is uuid para index
    return 0;
}

/* AT+BMPROV=static_oob,xxx */
static uint32_t cmd_ble_mesh_static_oob_set(int argc, const char *argv[])
{
    size_t len;
    uint8_t static_oob_auth[32]; // 32: size
    int err = 0;

    if (argc != 2 && argc != 3) {  // 2,3 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (strlen(argv[1]) != 32) { // 2: index, 32: size
        BT_INFO_SHELL("value len err: current %zu chars (max 32)", strlen(argv[1])); // 1: index
        return ERRCODE_BLE_MESH_FAIL;
    }
    len = ble_mesh_hex2bin(argv[1], static_oob_auth, 16); // 16: size
    if (len < 1) {
        BT_INFO_SHELL("Unable to parse input string argument");
        return -EINVAL;
    }

    if (argc == 3) { // 3:size
        if (strlen(argv[2]) != 32) { // 2: index, 32: size
            BT_INFO_SHELL("value len err: current %zu chars (max 32)", strlen(argv[2])); // 2: index
            return ERRCODE_BLE_MESH_FAIL;
        }
        len += ble_mesh_hex2bin(argv[2], static_oob_auth + 16, 16); // 2: index, 16: offset, 32: size
        if (len < 17) { // 17:size
            BT_INFO_SHELL("Unable to parse input string argument");
            return -EINVAL;
        }
    }

    err = ble_mesh_sample_set_static_oob_value(static_oob_auth, len);
    if (err) {
        BT_INFO_SHELL("Setting static OOB value set failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=unprov_beacon,flag, xx */
static uint32_t cmd_ble_mesh_unprov_beacon_send_flag(int argc, const char *argv[])
{
    int err = 0;
    uint8_t flag = 0;

    if (argc != 3) {  // 3 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "flag") != 0) { // 1: index
        BT_INFO_SHELL("unknown para:%s", argv[1]); // 1: index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 1);  // 1,2:index 0,1:range
    SHELL_PARAM_UINT(flag, argv[2]);  // 8:index
    if (flag == 0) {
        err = ble_mesh_prov_stop_unprov_beacon_send();
    } else {
        err = ble_mesh_prov_start_unprov_beacon_send();
    }
  
    if (err) {
        BT_INFO_SHELL("unprov beacon %s failed (err 0x%x)", flag ? "start" : "stop", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=remote_pub_key,xxx,xxx,xxx,xxx */
#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
static uint32_t cmd_ble_mesh_remote_pub_key_set(int argc, const char *argv[])
{
    size_t len = 0;
    uint8_t rm_pub_key[BT_MESH_PUB_KEY_LEN] = {0};
    int err = 0;

    if (argc != 5) {  // 5 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    for (int i = 0; i < 4; i++) { // 0,4: index
        if (strlen(argv[i+1]) != 32) { // 32: size
            BT_INFO_SHELL("key len err: current %zu chars (max 32)", strlen(argv[i+1]));
            return ERRCODE_BLE_MESH_FAIL;
        }
        len += ble_mesh_hex2bin(argv[i + 1], rm_pub_key + (16 * i), 16);  // 16: offset, 32: size
        if (len < ((16 * i) + 1)) {                        // 16:size
            BT_INFO_SHELL("Unable to parse input string argument %d", i + 1);
            return -EINVAL;
        }
    }

    err = ble_mesh_sample_set_remote_pub_key(rm_pub_key, BT_MESH_PUB_KEY_LEN);
    if (err) {
        BT_INFO_SHELL("Setting static OOB authentication failed (err %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=input_action,xxx,size,xxx */
static uint32_t cmd_ble_mesh_input_action_set(int argc, const char *argv[])
{
    int err = 0;
    uint8_t action, size;

    if (argc != 4) {  // 4 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[2], "size") != 0)) {  // 2:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(action, argv[1]);  // 1:index
    SHELL_PARAM_UINT(size, argv[3]);    // 3:index

    err = ble_mesh_sample_set_input_action(action, size);
    if (err) {
        BT_INFO_SHELL("Setting static OOB authentication failed (err %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=input_number,xxx */
static uint32_t cmd_ble_mesh_input_number_set(int argc, const char *argv[])
{
    int err = 0;
    uint32_t number;

    if (argc != 2) {  // 2 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(number, argv[1]);  // 1:index

    err = bt_mesh_input_number(number);
    if (err) {
        BT_INFO_SHELL("Setting input number failed (err %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=input_string,xxx */
static uint32_t cmd_ble_mesh_input_string_set(int argc, const char *argv[])
{
    int err = 0;

    if (argc != 2) {  // 2 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_input_string(argv[1]);
    if (err) {
        BT_INFO_SHELL("Setting input number failed (err %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=output_action,xxx,size,xxx */
static uint32_t cmd_ble_mesh_output_action_set(int argc, const char *argv[])
{
    int err = 0;
    uint8_t action, size;

    if (argc != 4) {  // 4 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[2], "size") != 0)) {  // 2:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(action, argv[1]);  // 1:index
    SHELL_PARAM_UINT(size, argv[3]);    // 3:index

    err = ble_mesh_sample_set_output_action(action, size);
    if (err) {
        BT_INFO_SHELL("Setting static OOB authentication failed (err %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

static uint32_t cmd_ble_mesh_link_id_set(int argc, const char *argv[])
{
    int err = 0;
    uint32_t link_id = 0;
    uint8_t filter = 0;
    bool is_filter = true;
    if (argc != 5) {  // 5 is para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "id") != 0) || (strcmp(argv[3], "filter") != 0)) {  // 1 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(link_id, argv[2]);  // 2:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 1);        // 3,4:index, 0,1:range
    SHELL_PARAM_UINT(filter, argv[4]);  // 4:index
    is_filter = (filter != 0);
    
    err = ble_mesh_set_prov_link_id(link_id, is_filter);
    if (err) {
        BT_INFO_SHELL("Setting static OOB authentication failed (err %d)", err);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int bt_mesh_net_info_param_get(ble_mesh_local_net_info_t *net_info, const char *argv[])
{
    SHELL_PARAM_UINT(net_info->net_idx, argv[2]);                   // 2:index
    if (strlen(argv[4]) != 32) {                                    // 4:index;32:size
        BT_INFO_SHELL("invaild net key len: %u", strlen(argv[4]));  // 4:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    arg_str_to_key(argv[4], net_info->net_key, 16);                  // 4:index 16:size
    SHELL_PARAM_UINT(net_info->flags, argv[6]);                      // 6:index
    SHELL_PARAM_UINT(net_info->iv_idx, argv[8]);                     // 8:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 1, 0x7FFF);        // 9,10:index, 1,0x7FFF:range
    SHELL_PARAM_UINT(net_info->addr, argv[10]);                      // 10:index
    if (strlen(argv[12]) != 32) {                                    // 12:index;32:size
        BT_INFO_SHELL("invaild dev key len: %u", strlen(argv[12]));  // 12:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    arg_str_to_key(argv[12], net_info->dev_key, 16);  // 12:index 16:size

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=set_net_info,net_idx,xxx,net_key,xxx,flags,xxx,iv_idx,xxx,addr,xxx,dev_key,xxx */
int cmd_cfg_set_net_info(int argc, const char *argv[])
{
    ble_mesh_local_net_info_t net_info = {0};
    int ret = 0;
    errcode_t err;

    if (argc != 13) {  // 13:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "net_idx") != 0 || strcmp(argv[3], "net_key") != 0 ||  // 1 3:index
        strcmp(argv[5], "flags") != 0 || strcmp(argv[7], "iv_idx") != 0 ||     // 5 7:index
        strcmp(argv[9], "addr") != 0 || strcmp(argv[11], "dev_key") != 0) {    // 9 11:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    ret = bt_mesh_net_info_param_get(&net_info, argv);
    if (ret) {
        BT_INFO_SHELL("param get fail");
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = ble_mesh_set_local_net_info(&net_info);
    if (err) {
        BT_INFO_SHELL("set local net info fail");
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMPROV=set_prov_type,type,1 */
static int cmd_cfg_set_prov_type(int argc, const char *argv[])
{
    errcode_t err = ERRCODE_BLE_MESH_SUCCESS;
    uint8_t type = 0;

    if (argc != 3) {  // 3: para num
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "type") != 0) {    // 1:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 1, 2);        // 1,2:index, 0,1:range
    SHELL_PARAM_UINT(type, argv[2]); // 2 index

    err = ble_mesh_set_prov_type(type);
    if (err) {
        BT_INFO_SHELL("set prov type fail:0x%x", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static at_cmd prov_cmd_tbl[] = {
    {"uuid_set", cmd_ble_mesh_prov_uuid_set},
    {"auth_set", cmd_ble_mesh_prov_auth_set},
    {"static_oob", cmd_ble_mesh_static_oob_set},
    {"unprov_beacon", cmd_ble_mesh_unprov_beacon_send_flag},
    {"link_id_set", cmd_ble_mesh_link_id_set},
    {"set_net_info", cmd_cfg_set_net_info},
    {"set_prov_type", cmd_cfg_set_prov_type},
#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
    {"remote_pub_key", cmd_ble_mesh_remote_pub_key_set},
    {"input_action", cmd_ble_mesh_input_action_set},
    {"input_number", cmd_ble_mesh_input_number_set},
    {"input_string", cmd_ble_mesh_input_string_set},
    {"output_action", cmd_ble_mesh_output_action_set},
#endif
};

uint32_t cmd_ble_mesh_prov_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    int prov_cmd_tbl_num = (sizeof(prov_cmd_tbl) / sizeof(at_cmd));
    for (int i = 0; i < prov_cmd_tbl_num; i++) {
        if (strcmp(argv[0], prov_cmd_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return prov_cmd_tbl[i].func(argc, argv);
        }
    }

    BT_INFO_SHELL("invalid cmd:%s", argv[0]);

    return ERRCODE_BLE_MESH_FAIL;
}

at_ret_t at_bt_mesh_prov_info(const ble_mesh_prov_info_t *args)
{
    // 判断是否要删除多余的逗号
    BT_INFO_SHELL("[debug] args[1]:%s,  args[2]:%s, args[3]:%s\r\n",
        args->para1,
        args->para2,
        args->para3);
    char *argv[13] = {NULL}; // 13 配网器指令的参数个数
    int argc = 0;
    // 进行参数处理
    ASSIGN_PARA(argv, args->para1, argc);
    ASSIGN_PARA(argv, args->para2, argc);
    ASSIGN_PARA(argv, args->para3, argc);
    ASSIGN_PARA(argv, args->para4, argc);
    ASSIGN_PARA(argv, args->para5, argc);
    ASSIGN_PARA(argv, args->para6, argc);
    ASSIGN_PARA(argv, args->para7, argc);
    ASSIGN_PARA(argv, args->para8, argc);
    ASSIGN_PARA(argv, args->para9, argc);
    ASSIGN_PARA(argv, args->para10, argc);
    ASSIGN_PARA(argv, args->para11, argc);
    ASSIGN_PARA(argv, args->para12, argc);
    ASSIGN_PARA(argv, args->para13, argc);

    uint32_t ret = cmd_ble_mesh_prov_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_prov_device_op_func error ret = %u %u\r\n", ret, argc);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_cmd_entry_t at_bt_mesh_prov_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_PROVISIONER)
    {
        "BMPROVNET",  // AT+BMPROVNET mesh指定mac配网
        0,
        0,
        g_ble_mesh_at_addr_params,              // 参数类型
        NULL,                                   // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_fix_mac_net,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
    {
        "BMPROV",  // AT+BMPROV mesh指定待入网设备的信息
        0,
        0,
        ble_mesh_prov_info,
        NULL,
        (at_set_func_t)at_bt_mesh_prov_info,
        NULL,
        NULL,
    }
};

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_prov_parse_table) / sizeof(at_bt_mesh_prov_parse_table[0]))

void mesh_at_prov_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_prov_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
