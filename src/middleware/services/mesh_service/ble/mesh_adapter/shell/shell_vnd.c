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
#include "vnd_client.h"
#include "ble_mesh_vnd_attr.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

static int cmd_ble_mesh_parse_set_attr_value(uint16_t attr_type, const char *val_str, bt_mesh_vnd_data_union_t *data)
{
    switch (attr_type) {
        case MESH_VENDOR_ONOFF_TYPE:
            SHELL_PARAM_UINT(data->onoff, val_str);
            break;
        case MESH_VENDOR_UNIX_TIME_TYPE:
            char_to_digit_array(data->unix_time, val_str, sizeof(data->unix_time));
            break;
        case MESH_VENDOR_POWER_PERCENT_TYPE:
            SHELL_PARAM_UINT(data->power_percent, val_str);
            break;
        default:
            BT_INFO_SHELL("cmd attr is invalid");
            return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_ble_mesh_vnd_opcode(
    bt_mesh_client_common_param_t *common, const bt_mesh_vnd_op_t *param, bt_mesh_vnd_status_t *status)
{
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    switch (common->opcode) {
        case BT_MESH_OP_VND_ATTR_SET:
            ret = bt_mesh_vnd_cli_set(common, (void *)param, (void *)status);
            BT_INFO_SHELL("status op_en:%d", status->op_en);
            break;
        case BT_MESH_OP_VND_ATTR_SET_UNACK:
            ret = bt_mesh_vnd_cli_set(common, (void *)param, NULL);
            break;
        case BT_MESH_OP_VND_ATTR_GET:
            ret = bt_mesh_vnd_cli_get(common, (void *)param, (void *)status);
            BT_INFO_SHELL("status op_en:%d", status->op_en);
            break;
        default:
            BT_INFO_SHELL("cmd opcode is invalid");
            ret = ERRCODE_BLE_MESH_FAIL;
            break;
    }

    return ret;
}

/* AT+VNDOP=get,app,0,net,0,dst,65535,ttl,10,op,xxx,attr,xxx */
/* AT+VNDOP=set,app,0,net,0,dst,65535,ttl,10,op,xxx,attr,xxx,val,xxx */
static uint32_t cmd_ble_mesh_vnd_model_op_func(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_vnd_get_cli_model(),
        .ctx =
            {
                .send_ttl = BT_MESH_TTL_DEFAULT,
            },
    };
    uint8_t vnd_opcode = 0;
    bt_mesh_vnd_status_t status = {0};
    bt_mesh_vnd_op_t param = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc != 13 && argc != 15) {  // 13:argc szie;15:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "op") != 0) || (strcmp(argv[11], "attr") != 0)) {  // 9,11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(vnd_opcode, argv[10]);  // 10:opcode

    if ((strcmp(argv[0], "get") == 0) && vnd_opcode != 0xD0) {
        BT_ERR("invalid get vnd opcode");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[0], "set") == 0) && (vnd_opcode != 0xD1 && vnd_opcode != 0xD2)) {
        BT_ERR("invalid set vnd opcode");
        return ERRCODE_BLE_MESH_FAIL;
    }
    common.opcode = BT_MESH_MODEL_OP_3(vnd_opcode, HW_CID);

    SHELL_PARAM_UINT(param.attr_type, argv[12]);  // 12:attr
    bt_mesh_vnd_data_union_t *data = (bt_mesh_vnd_data_union_t *)&param.data;

    // 只有 set 操作且有值时才解析
    if (strcmp(argv[0], "set") == 0) {
        if (argc != 15) {  // 15:argc size
            BT_ERR("SET command requires 'val' parameter");
            return ERRCODE_BLE_MESH_FAIL;
        }
        // 根据attr设置attr value
        ret = cmd_ble_mesh_parse_set_attr_value(param.attr_type, argv[14], data);  // 14:attr
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            return ret;
        }
    }
    // 根据opcode拼包
    ret = cmd_ble_mesh_vnd_opcode(&common, &param, &status);

    return ret;
}

at_ret_t at_bt_mesh_vnd_model(const ble_at_vnd_op_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_vnd_model args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[23] = {NULL};
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
    ASSIGN_PARA(argv, args->para14, argc);
    ASSIGN_PARA(argv, args->para15, argc);
    ASSIGN_PARA(argv, args->para16, argc);
    uint32_t ret = cmd_ble_mesh_vnd_model_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_vnd_model_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t g_ble_mesh_vnd_model_params[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_at_vnd_op_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_at_vnd_op_t, para16)},
};

const at_cmd_entry_t at_bt_mesh_vnd_parse_table[] = {
    {
        "VNDOP",  // 模型 AT+VNDOP vendor model operation
        0,
        0,
        g_ble_mesh_vnd_model_params,          // 参数类型
        NULL,                                 // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_vnd_model,  // 有参数时调用的函数
        NULL,
        NULL,
    },
};

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_vnd_parse_table) / sizeof(at_bt_mesh_vnd_parse_table[0]))

void mesh_at_vnd_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_vnd_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
