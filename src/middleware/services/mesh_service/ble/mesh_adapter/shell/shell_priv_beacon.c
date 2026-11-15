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
#include "client_common.h"
#include "priv_beacon_cli.h"
#include "device_property.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_PRIV_BEACONS)

#define PRIV_BEACON_RANDINT_MIN 0
#define PRIV_BEACON_RANDINT_MAX 255

typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
} ble_mesh_priv_beacon_client_t;

/* AT+BM_CC=priv_beacon_get,addr,XXX,net_index,XXX */
int cmd_priv_beacon_status_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    struct bt_mesh_priv_beacon_t status = {0};

    if (argc != 5) { // 5:size
        BT_ERR("cmd_priv_beacon_status_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_priv_beacon_cli_get(net_idx, dst, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Private Beacon Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Private Beacon state: %u, %u", status.enabled, status.rand_interval);

    return err;
}

/* AT+BMPRIVBEACONCLT=priv_beacon_set,addr,XXX,net_index,XXX,value,XXX,randInt[option],XXX */
int cmd_priv_beacon_status_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    struct bt_mesh_priv_beacon_t val = {0};
    struct bt_mesh_priv_beacon_t status = {0};

    if (argc != 7 && argc != 9) { // 7,9:size
        BT_ERR("cmd_priv_beacon_status_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) { // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(val.enabled, argv[6]); // 6:index
    if (argc == 9) {                        // 9: size
        if (strcmp(argv[7], "randInt") != 0) {  // 7:index
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        val.op_en = 1;
        SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], // 7,8:index
            PRIV_BEACON_RANDINT_MIN, PRIV_BEACON_RANDINT_MAX);
        SHELL_PARAM_UINT(val.rand_interval, argv[8]); // 8:index
    }
    err = bt_mesh_priv_beacon_cli_set(net_idx, dst, val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Priv Beacon Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Priv Beacon state is 0x%02x, rand_interval is 0x%02x", status.enabled, status.rand_interval);

    return err;
}

/* AT+BM_CC=gatt_proxy_get,addr,XXX,net_index,XXX */
int cmd_priv_beacon_gatt_proxy_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;

    if (argc != 5) { // 5:size
        BT_ERR("cmd_priv_beacon_gatt_proxy_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(net_idx, argv[4]); // 4:index

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_priv_beacon_cli_gatt_proxy_get(net_idx, dst, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Gatt Proxy Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Gatt Proxy state: %u", status);

    return err;
}

/* AT+BM_CC=gatt_proxy_set,addr,XXX,net_index,XXX,value,XXX */
int cmd_priv_beacon_gatt_proxy_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t val;
    uint8_t status;

    if (argc != 7) { // 7:index
        BT_ERR("cmd_priv_beacon_gatt_proxy_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) { // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(val, argv[6]);  // 6:index

    err = bt_mesh_priv_beacon_cli_gatt_proxy_set(net_idx, dst, val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Priv Beacon Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (status != 0) {
        BT_INFO_SHELL("Gatt Prox set fail status is 0x%02x", status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("Gatt Proxy is 0x%02x", val);

    return err;
}

/* AT+BM_CC=priv_beacon_get,addr,XXX,net_index,XXX,key_net_idx,XXX */
int cmd_priv_beacon_node_identity_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    struct bt_mesh_priv_node_id_t status = {0};

    if (argc != 7) { // 7:size
        BT_ERR("cmd_priv_beacon_node_identity_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0) { // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]); // 6:index

    err = bt_mesh_priv_beacon_cli_node_id_get(net_idx, dst, key_net_idx, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Node Identity Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Node Identity net_idx is 0x%02x, identity is 0x%02x", status.net_idx, status.state);

    return err;
}

/* AT+BM_CC=node_identity_set,addr,XXX,net_index,XXX,key_net_idx,XXX,state,XXX */
int cmd_priv_beacon_node_identity_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    struct bt_mesh_priv_node_id_t val = {0};
    struct bt_mesh_priv_node_id_t status = {0};

    if (argc != 9) { // 9:size
        BT_ERR("cmd_priv_beacon_node_identity_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0 || strcmp(argv[7], "identity") != 0) { // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(val.net_idx, argv[6]); // 6:index
    SHELL_PARAM_UINT(val.state, argv[8]); // 8:index

    err = bt_mesh_priv_beacon_cli_node_id_set(net_idx, dst, val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Node Identity Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status.status != 0) {
        BT_INFO_SHELL("Node Identity set fail net_idx is 0x%02x, identity is 0x%02x, status is 0x%02x",
                      status.net_idx, status.state, status.status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("Node Identity net_idx is 0x%02x, identity is 0x%02x", status.net_idx, status.state);

    return err;
}

uint32_t cmd_ble_mesh_cfg_priv_beacon_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "priv_beacon_get") == 0) { // 0:index
        ret = cmd_priv_beacon_status_get(argc, argv);
    } else if (strcmp(argv[0], "priv_beacon_set") == 0) { // 0:index
        ret = cmd_priv_beacon_status_set(argc, argv);
    } else if (strcmp(argv[0], "gatt_proxy_get") == 0) { // 0:index
        ret = cmd_priv_beacon_gatt_proxy_get(argc, argv);
    } else if (strcmp(argv[0], "gatt_proxy_set") == 0) { // 0:index
        ret = cmd_priv_beacon_gatt_proxy_set(argc, argv);
    } else if (strcmp(argv[0], "node_identity_get") == 0) { // 0:index
        ret = cmd_priv_beacon_node_identity_get(argc, argv);
    } else if (strcmp(argv[0], "node_identity_set") == 0) { // 0:index
        ret = cmd_priv_beacon_node_identity_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]); // 0:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_priv_beacon_client(const ble_mesh_config_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_config_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[10] = {NULL}; // 10:index
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
    uint32_t ret = cmd_ble_mesh_cfg_priv_beacon_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_cfg_priv_beacon_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_priv_beacon_client_[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_priv_beacon_client_t, para10)},
};

const at_cmd_entry_t at_bt_mesh_priv_beacon_table[] = {
    {
        "BMPRIVBEACONCLT",  // 配置客户端 AT+BM_CC
        0,
        0,
        ble_mesh_priv_beacon_client_,                  // 参数类型
        NULL,                                          // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_priv_beacon_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
};
#endif

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_priv_beacon_table) / sizeof(at_bt_mesh_priv_beacon_table[0]))

void mesh_at_priv_beacon_register(void)
{
    BT_INFO_SHELL("enter");
#if MYNEWT_VAL(BLE_MESH_PRIV_BEACONS)
    int ret = uapi_at_cmd_table_register(at_bt_mesh_priv_beacon_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
#else
    BT_INFO_SHELL("not support private beacon");
#endif
}
