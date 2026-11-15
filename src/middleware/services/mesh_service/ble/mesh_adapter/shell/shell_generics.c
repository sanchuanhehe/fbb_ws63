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
#include "generic_client.h"
#include "device_property.h"
#include "mesh/cfg_cli.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"

#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONOFF_CLIENT)
/* AT+BMGOCM=set,app,0,net,0,dst,65535,ttl,10 */
/* AT+BMGOCM=set_unack,app,0,net,0,dst,65535,ttl,10 */
uint32_t cmd_ble_mesh_onoff_get_proc(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_onoff_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_ONOFF_GET,
    };
    struct gen_onoff_status status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc != 9) {  // 9: argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    ret = bt_mesh_gen_onoff_get(&common, &status);
    BT_INFO_SHELL("onoff:%u", status.onoff);
    if (status.op_en) {
        BT_INFO_SHELL("target:%u, remain_time:%u", status.target_onoff, status.remain_time);
    }

    return ret;
}

int cmd_ble_mesh_onoff_set_proc(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_onoff_cli_model(),
        .ctx =
            {
                .send_ttl = BT_MESH_TTL_DEFAULT,
            },
    };
    struct gen_onoff_status status = {0};
    struct gen_onoff_set set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc != 13 && argc != 17) {  // 13:argc szie;17:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "onoff") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9,11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 1);  // 9,10:index, 0,1:range
    SHELL_PARAM_UINT(set.onoff, argv[10]);                // 10:index
    SHELL_PARAM_UINT(set.tid, argv[12]);                  // 12:index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13,15:argv index
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }

        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:index
        if (check_dtt(set.trans_time) != 0) {
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:index
    }

    if (strcmp(argv[0], "set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_ONOFF_SET;
        ret = bt_mesh_gen_onoff_set(&common, (void *)&set, (void *)&status);
        BT_INFO_SHELL("onoff:%u", status.onoff);
        if (status.op_en) {
            BT_INFO_SHELL("target:%u, remain_time:%u", status.target_onoff, status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK;
        ret = bt_mesh_gen_onoff_set(&common, (void *)&set, NULL);
    }

    return ret;
}

uint32_t cmd_ble_mesh_onoff_cli_op_func(int argc, const char *argv[])
{
    BT_INFO(" ");
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "get") == 0) {
        /* AT+BM_GOCM=get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_ble_mesh_onoff_get_proc(argc, argv);
    } else if ((strcmp(argv[0], "set") == 0) || (strcmp(argv[0], "set_unack") == 0)) {
        ret = cmd_ble_mesh_onoff_set_proc(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_onoff_client(const ble_mesh_onoff_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_onoff_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
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
    ASSIGN_PARA(argv, args->para17, argc);
    uint32_t ret = cmd_ble_mesh_onoff_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_onoff_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_onoff_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_onoff_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_onoff_client_t, para17)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_POWER_LEVEL_CLIENT)
/* AT+BM_PLC=power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_power_level_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_power_level_status status = {0};
    int ret = bt_mesh_gen_power_level_get(&common, (void *)&status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("power_level:%u", status.power_level);
    if (status.op_en) {
        BT_INFO_SHELL("target_power_level:%u, remain_time:%u", status.target_power_level, status.remain_time);
    }

    return ret;
}
/* AT+BM_PLC=power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx */
/* AT+BM_PLC=power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BM_PLC=power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx */
/* AT+BM_PLC=power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx,tt,xxx,delay,xxx */
uint32_t cmd_bt_mesh_power_level_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size;
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "power_level") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9:argv index;11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_power_level_set set = {0};
    SHELL_PARAM_UINT(set.power_level, argv[10]);  // 10:argv index
    SHELL_PARAM_UINT(set.tid, argv[12]);          // 12: argv index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13:argv index;15:argv index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14: argv index
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:argv index
    }

    struct gen_power_level_status status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (strcmp(argv[0], "power_level_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, (void *)&status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("power_level:%u", status.power_level);
        if (status.op_en) {
            BT_INFO_SHELL("target_power_level:%u, remain_time:%u", status.target_power_level, status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BM_PLC=default_power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_default_power_level_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t default_power_level = 0;
    int ret = bt_mesh_gen_power_level_get(&common, (void *)&default_power_level);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("default_power_level %u", default_power_level);

    return ret;
}
/* AT+BM_PLC=default_power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_power_level,xxx */
/* AT+BM_PLC=default_power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_power_level,xxx */
uint32_t cmd_bt_mesh_default_power_level_set(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "default_power_level") != 0)) {  // 9:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_power_level_set set = {0};
    SHELL_PARAM_UINT(set.power_def_level, argv[10]);  // 10:argv index

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "default_power_level_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        uint16_t default_power_level = 0;
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, (void *)&default_power_level);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("default_power_level %u", default_power_level);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BM_PLC=power_level_range_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_power_level_range_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_power_level_range_status range_status = {0};
    int ret = bt_mesh_gen_power_level_get(&common, (void *)&range_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("status_code %u, range [%u,%u]",
        range_status.status_code,
        range_status.min_power_level,
        range_status.max_power_level);

    return ret;
}
/* AT+BM_PLC=power_level_range_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_min_level,xxx,power_max_level,xxx */
/* AT+BM_PLC=power_level_range_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_min_level,xxx,power_max_level,xxx */
uint32_t cmd_bt_mesh_power_level_range_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "power_min_level") != 0) ||   // 9:argv index
        (strcmp(argv[11], "power_max_level") != 0)) {  // 11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_power_level_set set = {0};
    SHELL_PARAM_UINT(set.power_min_level, argv[10]);  // 10:argv index
    SHELL_PARAM_UINT(set.power_max_level, argv[12]);  // 12:argv index

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "power_level_range_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        struct gen_power_level_range_status range_status = {0};
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, (void *)&range_status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("status_code %u, range [%u,%u]",
            range_status.status_code,
            range_status.min_power_level,
            range_status.max_power_level);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK;
        ret = bt_mesh_gen_power_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BM_PLC=last_power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_last_power_level_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_level_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t last_power_level = 0;
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    int ret = bt_mesh_gen_power_level_get(&common, (void *)&last_power_level);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("last_power_level %u", last_power_level);

    return ret;
}

uint32_t cmd_ble_mesh_power_level_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "power_level_get") == 0) {
        /* AT+BM_PLC=power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_power_level_get(argc, argv);
    } else if (strcmp(argv[0], "power_level_set") == 0 || strcmp(argv[0], "power_level_set_unack") == 0) {
        /* AT+BM_PLC=power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx */
        /* AT+BM_PLC=power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        /* AT+BM_PLC=power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx */
        /* AT+BM_PLC=power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        ret = cmd_bt_mesh_power_level_set(argc, argv);
    } else if (strcmp(argv[0], "default_power_level_get") == 0) {
        /* AT+BM_PLC=default_power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_default_power_level_get(argc, argv);
    } else if (strcmp(argv[0], "default_power_level_set") == 0 ||
               strcmp(argv[0], "default_power_level_set_unack") == 0) {
        /* AT+BM_PLC=default_power_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_power_level,xxx */
        /* AT+BM_PLC=default_power_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_power_level,xxx */
        ret = cmd_bt_mesh_default_power_level_set(argc, argv);
    } else if (strcmp(argv[0], "power_level_range_get") == 0) {
        /* AT+BM_PLC=power_level_range_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_power_level_range_get(argc, argv);
    } else if (strcmp(argv[0], "power_level_range_set") == 0 || strcmp(argv[0], "power_level_range_set_unack") == 0) {
        /* AT+BM_PLC=power_level_range_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_min_level,xxx,power_max_level,xxx */
        /* AT+BM_PLC=power_level_range_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,power_min_level,xxx,power_max_level,xxx
         */
        ret = cmd_bt_mesh_power_level_range_set(argc, argv);
    } else if (strcmp(argv[0], "last_power_level_get") == 0) {
        /* AT+BM_PLC=last_power_level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_last_power_level_get(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_power_level_client(const ble_mesh_power_level_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_set_level_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[17] = {NULL};
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
    ASSIGN_PARA(argv, args->para17, argc);
    uint32_t ret = cmd_ble_mesh_power_level_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_power_level_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_power_level_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_power_level_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_power_level_client_t, para17)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LEVEL_CLIENT)
/* AT+BM_LC=level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_level_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_level_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_LEVEL_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    struct gen_level_status status = {0};

    int ret = bt_mesh_gen_level_get(&common, (void *)&status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    
    BT_INFO_SHELL("level:%d", status.level);
    if (status.op_en) {
        BT_INFO_SHELL("target_level:%d, remain_time:%u", status.target_level, status.remain_time);
    }

    return ret;
}
/* AT+BMSETLEVEL=level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx */
/* AT+BMSETLEVEL=level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BMSETLEVEL=level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx */
/* AT+BMSETLEVEL=level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx,tt,xxx,delay,xxx */
int cmd_bt_mesh_level_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "level") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9,11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct gen_level_set set = {0};
    SHELL_PARAM_RANGE_CHECK_IN(argv[0], argv[10], -0x8000, 0x7FFF);  // 0 10: index, -0x8000 0x7FFF: range
    set.level = atoi(argv[10]);                                      // 10:argv index
    SHELL_PARAM_UINT(set.tid, argv[12]);                             // 12 : index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13,15:argv index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:index
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:index
    }

    struct gen_level_status status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    
    if (strcmp(argv[0], "level_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_LEVEL_SET;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, (void *)&status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("level:%d", status.level);
        if (status.op_en) {
            BT_INFO_SHELL("target_level:%d, remain_time:%u", status.target_level, status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BM_LC=delta_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx */
/* AT+BM_LC=delta_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BM_LC=delta_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx */
/* AT+BM_LC=delta_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx,tt,xxx,delay,xxx */
uint32_t cmd_bt_mesh_delta_level_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "delta_level") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9:argv index;11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_level_set set = {0};
    set.delta_level = atoi(argv[10]);     // 10:argv index
    SHELL_PARAM_UINT(set.tid, argv[12]);  // 12:argv index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13:argv index;15:argv index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:argv index
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:argv index
    }

    struct gen_level_status status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (strcmp(argv[0], "delta_level_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_DELTA_SET;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, (void *)&status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("level:%d", status.level);
        if (status.op_en) {
            BT_INFO_SHELL("target_level:%d, remain_time:%u", status.target_level, status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BM_LC=move_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx */
/* AT+BM_LC=move_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BM_LC=move_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx */
/* AT+BM_LC=move_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx,tt,xxx,delay,xxx */
int cmd_bt_mesh_move_level_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_level_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "move_level") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9,11:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_level_set set = {0};
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], -0x8000, 0x7FFF);  // 9,10:argv index, -0x8000,0x7FFF:range
    set.move_level = atoi(argv[10]);                                 // 10:argv index
    SHELL_PARAM_UINT(set.tid, argv[12]);                             // 12:argv index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13:argv index;15:argv index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:argv index
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:argv index
    }

    struct gen_level_status status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (strcmp(argv[0], "move_level_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_MOVE_SET;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, (void *)&status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("level:%d", status.level);
        if (status.op_en) {
            BT_INFO_SHELL("target_level:%d, remain_time:%u", status.target_level, status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK;
        ret = bt_mesh_gen_level_set(&common, (void *)&set, NULL);
    }

    return ret;
}

uint32_t cmd_ble_mesh_level_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "level_get") == 0) {
        /* AT+BM_LC=level_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_level_get(argc, argv);
    } else if (strcmp(argv[0], "level_set") == 0 || strcmp(argv[0], "level_set_unack") == 0) {
        /* AT+BM_LC=level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx */
        /* AT+BM_LC=level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx,tt,xxx,delay,xxx */
        /* AT+BM_LC=level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx */
        /* AT+BM_LC=level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,level,xxx,tid,xxx,tt,xxx,delay,xxx */
        ret = cmd_bt_mesh_level_set(argc, argv);
    } else if (strcmp(argv[0], "delta_level_set") == 0 || strcmp(argv[0], "delta_level_set_unack") == 0) {
        /* AT+BM_LC=delta_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx */
        /* AT+BM_LC=delta_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        /* AT+BM_LC=delta_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx */
        /* AT+BM_LC=delta_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,delta_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        ret = cmd_bt_mesh_delta_level_set(argc, argv);
    } else if (strcmp(argv[0], "move_level_set") == 0 || strcmp(argv[0], "move_level_set_unack") == 0) {
        /* AT+BM_LC=move_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx */
        /* AT+BM_LC=move_level_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        /* AT+BM_LC=move_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx */
        /* AT+BM_LC=move_level_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,move_level,xxx,tid,xxx,tt,xxx,delay,xxx */
        ret = cmd_bt_mesh_move_level_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_set_level_client(const ble_mesh_level_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_set_level_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[17] = {NULL};
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
    ASSIGN_PARA(argv, args->para17, argc);
    uint32_t ret = cmd_ble_mesh_level_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_level_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_level_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_level_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_level_client_t, para17)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_DEF_TRANS_TIME_CLIENT)
/* AT+BM_DTTC=dtt_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_dtt_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_dtt_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint8_t dtt = 0;
    int ret = bt_mesh_gen_dtt_get(&common, (void *)&dtt);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("dtt %d", dtt);

    return ret;
}
/* AT+BM_DTTC=dtt_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,dtt,xxx */
/* AT+BM_DTTC=dtt_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,dtt,xxx */
uint32_t cmd_bt_mesh_dtt_set(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_dtt_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "dtt") != 0)) {  // 9:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    uint8_t dtt;
    SHELL_PARAM_UINT(dtt, argv[10]);  // 10:index
    if (check_dtt(dtt) != 0) {
        BT_ERR("dtt value err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "dtt_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET;
        ret = bt_mesh_gen_dtt_set(&common, (void *)&dtt, (void *)&dtt);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("dtt %d", dtt);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK;
        ret = bt_mesh_gen_dtt_set(&common, (void *)&dtt, NULL);
    }

    return ret;
}

uint32_t cmd_ble_mesh_dtt_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "dtt_get") == 0) {
        /* AT+BM_DTTC=dtt_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_dtt_get(argc, argv);
    } else if (strcmp(argv[0], "dtt_set") == 0 || strcmp(argv[0], "dtt_set_unack") == 0) {
        /* AT+BM_DTTC=dtt_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,dtt,xxx */
        /* AT+BM_DTTC=dtt_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,dtt,xxx */
        ret = cmd_bt_mesh_dtt_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_set_def_trans_timer(const ble_mesh_def_trans_time_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[11] = {NULL};
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
    uint32_t ret = cmd_ble_mesh_dtt_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_def_trans_time[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_def_trans_time_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_def_trans_time_t, para11)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONPOWERUP_CLIENT)
/* AT+BM_OPC=onpowerup_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_onpowerup_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_onoff_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint8_t onpowerup = 0;
    int ret = bt_mesh_gen_power_onoff_get(&common, (void *)&onpowerup);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("onpowerup %u", onpowerup);

    return ret;
}
/* AT+BM_OPC=onpowerup_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,onpowerup,xxx */
/* AT+BM_OPC=onpowerup_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,onpowerup,xxx */
uint32_t cmd_bt_mesh_onpowerup_set(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_power_onoff_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "onpowerup") != 0)) {  // 9:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    uint8_t onpowerup;
    SHELL_PARAM_UINT(onpowerup, argv[10]);  // 10:argv index

    if (strcmp(argv[0], "onpowerup_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET;
        ret = bt_mesh_gen_power_onoff_set(&common, &onpowerup, &onpowerup);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("onpowerup %u", onpowerup);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK;
        ret = bt_mesh_gen_power_onoff_set(&common, &onpowerup, NULL);
    }

    return ret;
}
uint32_t cmd_ble_mesh_onpowerup_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "onpowerup_get") == 0) {
        /* AT+BM_OPC=onpowerup_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_onpowerup_get(argc, argv);
    } else if (strcmp(argv[0], "onpowerup_set") == 0 || strcmp(argv[0], "onpowerup_set_unack") == 0) {
        /* AT+BM_OPC=onpowerup_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,onpowerup,xxx */
        /* AT+BM_OPC=onpowerup_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,onpowerup,xxx */
        ret = cmd_bt_mesh_onpowerup_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_onpower_state(const ble_mesh_on_power_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[11] = {NULL};
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
    uint32_t ret = cmd_ble_mesh_onpowerup_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_onpowerup_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_on_power_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_on_power_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_on_power_client_t, para11)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_BATTERY_CLIENT)
/* AT+BM_BC=battery_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_battery_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_battery_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_BATTERY_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct gen_battery_status status = {0};
    int ret = bt_mesh_gen_battery_get(&common, (void *)&status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("battery_level:%u, time_to_discharge:%u, time_to_charge:%u, flags:%u",
        status.battery_level,
        status.time_to_discharge,
        status.time_to_charge,
        status.flags);

    return ret;
}
uint32_t cmd_ble_mesh_battery_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "battery_get") == 0) {
        /* AT+BM_BC=battery_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_battery_get(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_battery_model(const ble_mesh_battery_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[9] = {NULL};
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
    uint32_t ret = cmd_ble_mesh_battery_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_battery_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_battery_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_battery_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_battery_client_t, para9)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LOCATION_CLIENT)
/* AT+BM_LCC=location_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_location_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_location_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct gen_location_status status = {0};
    int ret = bt_mesh_gen_location_get(&common, (void *)&status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("global_latitude:%u, global_longitude:%u, global_altitude:%u",
        status.global_latitude,
        status.global_longitude,
        status.global_altitude);

    return ret;
}
uint32_t cmd_ble_mesh_location_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "location_get") == 0) {
        /* AT+BM_LCC=location_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_location_get(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_location_model(const ble_mesh_location_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_location_model args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[9] = {NULL};
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
    uint32_t ret = cmd_ble_mesh_location_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_location_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_location_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_location_client_t, para9)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_PROP_CLIENT)
int user_properties_status_proc(struct bt_mesh_model *model, struct gen_user_properties_status *status)
{
    BT_INFO_SHELL("user_property_ids: 0x%s, id_nums: %d",
        bt_hex(status->user_prop_ids->om_data, status->user_prop_ids->om_len),
        status->user_prop_ids->om_len / 2);  // 2:divisor
    BT_INFO_SHELL("user operation");

    return ERRCODE_BLE_MESH_SUCCESS;
}
/* AT+BM_PC=user_properties_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_user_properties_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct gen_user_properties_param user_properties_param = {
        .status = user_properties_status_proc,
    };
    BT_INFO_SHELL("cmd_bt_mesh_user_properties_get");
    int ret = bt_mesh_gen_properties_get(&common, (void *)&user_properties_param);
    BT_INFO_SHELL("ret = %d", ret);

    return ret;
}

int manu_properties_status_proc(struct bt_mesh_model *model, struct gen_manu_properties_status *status)
{
    BT_INFO_SHELL("manu_property_ids: 0x%s, id_nums: %d",
        bt_hex(status->manu_prop_ids->om_data, status->manu_prop_ids->om_len),
        status->manu_prop_ids->om_len / 2);  // 2：divisor
    BT_INFO_SHELL("user operation");

    return ERRCODE_BLE_MESH_SUCCESS;
}
/* AT+BM_PC=manu_properties_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_manu_properties_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct gen_manu_properties_param manu_properties_param = {
        .status = manu_properties_status_proc,
    };
    BT_INFO_SHELL("cmd_bt_mesh_manu_properties_get");
    int ret = bt_mesh_gen_properties_get(&common, (void *)&manu_properties_param);
    BT_INFO_SHELL("ret = %d", ret);

    return ret;
}

int user_property_status_proc(struct bt_mesh_model *model, struct gen_user_property_status *status)
{
    BT_INFO_SHELL("user_prop_id: %u, user_access: %u, user_prop_val: %s",
        status->user_prop_id,
        status->user_access,
        bt_hex(status->user_prop_val->om_data, status->user_prop_val->om_len));
    BT_INFO_SHELL("user operation");

    return ERRCODE_BLE_MESH_SUCCESS;
}
/* AT+BM_PC=user_property_get,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx */
uint32_t cmd_bt_mesh_user_property_get(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx_info err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "prop_id") != 0)) {  // 9:index
        BT_ERR("cmd prop_id err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t prop_id;
    SHELL_PARAM_UINT(prop_id, argv[10]);  // 10:index

    struct gen_user_property_param user_property_param = {
        .status = user_property_status_proc,
    };

    BT_INFO_SHELL("cmd_bt_mesh_user_property_get");
    int ret = bt_mesh_gen_property_get(&common, prop_id, (void *)&user_property_param);
    BT_INFO_SHELL("ret = %d", ret);

    return ret;
}

uint8_t C2X(char a)
{
    if ('0' <= a && a <= '9') {
        return (uint8_t)a - ('0' - 0);
    } else if ('a' <= a && a <= 'f') {
        return (uint8_t)a - ('a' - 0xa);
    } else if ('A' <= a && a <= 'F') {
        return (uint8_t)a - ('A' - 0xa);
    } else {
        return -1;
    }
}

#define HEX_LEN 2
void fill_val(const char *a, int len, struct gen_user_property_set *val, int val_len)
{
    uint8_t *b = osal_kmalloc(sizeof(uint8_t) * val_len, 0);
    if (b == NULL) {
        BT_ERR("cmd err, malloc err");
        return;
    }

    int from = 0;
    int to = 0;
    int tlen = len;
    if ((len % HEX_LEN) != 0) {
        b[to++] = C2X(a[from++]);
        from = 1;
        tlen = len - 1;
    }

    while (from < tlen) {
        b[to] = (C2X(a[from]) << 4) | C2X(a[from + 1]);  // 4：size
        to = to + 1;
        from = from + HEX_LEN;
    }
    net_buf_simple_add_mem(val->user_property_value, b, val_len);
    osal_kfree(b);
}

/* AT+BM_PC=user_property_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_value,xxx */
/* AT+BM_PC=user_property_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_value,xxx */
uint32_t cmd_bt_mesh_user_property_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "prop_id") != 0) || (strcmp(argv[11], "prop_value") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_user_property_set user_property_set = {0};
    SHELL_PARAM_UINT(user_property_set.user_property_id, argv[10]);  // 10:index
    uint8_t len = bt_mesh_get_dev_prop_len(user_property_set.user_property_id);
    if (strlen(argv[12]) / HEX_LEN != len) {  // 12:index
        BT_ERR("Invaild user_property_value len, should be %d bytes", len);
        return ERRCODE_BLE_MESH_FAIL;
    }

    user_property_set.user_property_value = NET_BUF_SIMPLE(len);
    if (!user_property_set.user_property_value) {
        BT_ERR("alloc user_property_value memory err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    fill_val(argv[12], strlen(argv[12]), &user_property_set, len);  // 12:index

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    struct gen_user_property_param user_property_param = {
        .status = user_property_status_proc,
    };

    if (strcmp(argv[0], "user_property_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET;
        BT_INFO_SHELL("cmd_bt_mesh_user_property_set");
        ret = bt_mesh_gen_property_set(&common, (void *)&user_property_set, (void *)&user_property_param);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK;
        ret = bt_mesh_gen_property_set(&common, (void *)&user_property_set, NULL);
    }

    BT_INFO_SHELL("ret = %d", ret);
    os_mbuf_free_chain(user_property_set.user_property_value);

    return ret;
}
int manu_property_status_proc(struct bt_mesh_model *model, struct gen_manu_property_status *status)
{
    BT_INFO_SHELL("manu_prop_id: %u, manu_access: %u, manu_prop_val: %s",
        status->manu_prop_id,
        status->manu_access,
        bt_hex(status->manu_prop_val->om_data, status->manu_prop_val->om_len));
    BT_INFO_SHELL("user operation");

    return ERRCODE_BLE_MESH_SUCCESS;
}
/* AT+BM_PC=manu_property_get,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx */
uint32_t cmd_bt_mesh_manu_property_get(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "prop_id") != 0)) {  // 9:index
        BT_ERR("cmd prop_id err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t prop_id;
    SHELL_PARAM_UINT(prop_id, argv[10]);  // 10:index

    struct gen_manu_property_param manu_property_param = {
        .status = manu_property_status_proc,
    };
    BT_INFO_SHELL("cmd_bt_mesh_manu_property_get");
    int ret = bt_mesh_gen_property_get(&common, prop_id, (void *)&manu_property_param);
    BT_INFO_SHELL("ret = %d", ret);

    return ret;
}
/* AT+BM_PC=manu_property_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_access,xxx */
/* AT+BM_PC=manu_property_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_access,xxx */
int cmd_bt_mesh_manu_property_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_gen_prop_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "prop_id") != 0) || (strcmp(argv[11], "prop_access") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct gen_manu_property_set manu_property_set = {0};
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0, 1);            // 11,12:index, 0,1:range
    SHELL_PARAM_UINT(manu_property_set.manu_property_id, argv[10]);  // 10:index
    SHELL_PARAM_UINT(manu_property_set.manu_user_access, argv[12]);  // 12:index

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    struct gen_manu_property_param manu_property_param = {
        .status = manu_property_status_proc,
    };

    if (strcmp(argv[0], "manu_property_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET;
        ret = bt_mesh_gen_property_set(&common, (void *)&manu_property_set, (void *)&manu_property_param);
        BT_INFO_SHELL("manu_property_set ret %d", ret);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK;
        ret = bt_mesh_gen_property_set(&common, (void *)&manu_property_set, NULL);
        BT_INFO_SHELL("manu_property_set_unack ret %d", ret);
    }
    return ret;
}

uint32_t cmd_ble_mesh_property_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "user_properties_get") == 0) {
        /* AT+BM_PC=user_properties_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_user_properties_get(argc, argv);
    } else if (strcmp(argv[0], "manu_properties_get") == 0) {
        /* AT+BM_PC=manu_properties_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_manu_properties_get(argc, argv);
    } else if (strcmp(argv[0], "user_property_get") == 0) {
        /* AT+BM_PC=user_property_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_user_property_get(argc, argv);
    } else if ((strcmp(argv[0], "user_property_set") == 0) || (strcmp(argv[0], "user_property_set_unack") == 0)) {
        /* AT+BM_PC=user_property_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_value,xxx */
        /* AT+BM_PC=user_property_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_value,xxx */
        ret = cmd_bt_mesh_user_property_set(argc, argv);
    } else if (strcmp(argv[0], "manu_property_get") == 0) {
        /* AT+BM_PC=manu_property_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_manu_property_get(argc, argv);
    } else if ((strcmp(argv[0], "manu_property_set") == 0) || (strcmp(argv[0], "manu_property_set_unack") == 0)) {
        /* AT+BM_PC=manu_property_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_access,xxx */
        /* AT+BM_PC=manu_property_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,prop_id,xxx,prop_access,xxx */
        ret = cmd_bt_mesh_manu_property_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_property_info(const ble_mesh_property_info_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[17] = {NULL};
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
    uint32_t ret = cmd_ble_mesh_property_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_property_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_property_info[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_property_info_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_property_info_t, para13)},
};
#endif

const at_cmd_entry_t at_bt_mesh_generics_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONOFF_CLIENT)
    {
        "BMGOCM",  // BM_GOCM onoff 客户端控制
        0,
        0,
        ble_mesh_onoff_client,                   // 参数类型
        NULL,                                    // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_onoff_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_POWER_LEVEL_CLIENT)
    {
        "BMPOWERLEVEL",  // BM_PLC 设置电源级别 客户端控制
        0,
        0,
        ble_mesh_power_level_client,                   // 参数类型
        NULL,                                          // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_power_level_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LEVEL_CLIENT)
    {
        "BMSETLEVEL",  // BM_LC 设置级别状态 客户端控制
        0,
        0,
        ble_mesh_level_client,                       // 参数类型
        NULL,                                        // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_set_level_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_DEF_TRANS_TIME_CLIENT)
    {
        "BMDTTC",  // BM_DTTC 设置数据传输时间
        0,
        0,
        ble_mesh_def_trans_time,                        // 参数类型
        NULL,                                           // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_set_def_trans_timer,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONPOWERUP_CLIENT)
    {
        "BMOPC",  // BM_OPC 获取上电状态
        0,
        0,
        ble_mesh_on_power_client,                 // 参数类型
        NULL,                                     // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_onpower_state,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_BATTERY_CLIENT)
    {
        "BMBATTERY",  // BM_BC 获取battery模型状态
        0,
        0,
        ble_mesh_battery_client,                  // 参数类型
        NULL,                                     // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_battery_model,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LOCATION_CLIENT)
    {
        "BMLOCATION",  // BM_LCC 获取location模型状态
        0,
        0,
        ble_mesh_location_client,                  // 参数类型
        NULL,                                      // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_location_model,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_PROP_CLIENT)
    {
        "BMPROPERTY",  // BM_PC PROPERTY 属性值信息
        0,
        0,
        ble_mesh_property_info,                   // 参数类型
        NULL,                                     // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_property_info,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_generics_parse_table) / sizeof(at_bt_mesh_generics_parse_table[0]))

void mesh_at_generics_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_generics_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
