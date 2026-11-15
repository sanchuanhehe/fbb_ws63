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
#include "device_property.h"
#include "time_client.h"
#include "time_server.h"
#include "scene_client.h"
#include "scene_server.h"
#include "scheduler_client.h"
#include "scheduler_server.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_SCENE_CLI)
/* AT+BM_SCC=scene_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_scene_scene_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scene_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_SCENE_GET,
    };

    bt_mesh_scene_client_status_t status = {0};

    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    status.scene_status.status_code = 0;
    int ret;
    ret = bt_mesh_scene_client_get_state(&common, &status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("scene get fail, ret is %d", ret);
        return ret;
    }

    BT_INFO_SHELL("scene_status data:status_code is %d,current_scene is %d",
        status.scene_status.status_code,
        status.scene_status.current_scene);
    if (status.scene_status.op_en == true) {
        BT_INFO_SHELL(
            "target_scene is %d,remain_time is %d", status.scene_status.target_scene, status.scene_status.remain_time);
    }

    return ret;
}

/* AT+BM_SCC=register_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_scene_register_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scene_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_SCENE_REGISTER_GET,
    };

    bt_mesh_scene_client_status_t status = {0};

    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    status.register_status.status_code = 0;
    int ret;
    ret = bt_mesh_scene_client_get_state(&common, &status);

    BT_INFO_SHELL("register_status data:status_code is %d,current_scene is %d",
        status.register_status.status_code,
        status.register_status.current_scene);
    for (int i = 0; i < sizeof(status.register_status.scenes) / sizeof(uint16_t); i++) {
        if (status.register_status.scenes[i] != 0) {
            BT_INFO_SHELL("scene_number is %d", status.register_status.scenes[i]);
        }
    }

    return ret;
}

/* AT+BMSCENE=scene_store,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx */
/* AT+BMSCENE=scene_store_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx */
uint32_t cmd_bt_mesh_scene_scene_store(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scene_cli_model(),
    };
    bt_mesh_time_scene_client_set_msg_t set = {0};
    bt_mesh_scene_client_status_t status = {0};
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "scene_number") != 0)) {  // 9:index
        BT_ERR("cmd err, %s", argv[9]);
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(set.scene_store.scene_number, argv[10]);  // 10:index
    status.register_status.status_code = 0x03;

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "scene_store") == 0) {
        BT_INFO_SHELL("cmd is scene_store");
        common.opcode = BLE_MESH_MODEL_OP_SCENE_STORE;
    } else {
        common.opcode = BLE_MESH_MODEL_OP_SCENE_STORE_UNACK;
    }
    ret = bt_mesh_scene_client_set_state(&common, &set, &status);
    if (status.register_status.status_code == SCENE_REG_FULL) {
        BT_WARN("Scene Register is full!");
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ret;
}

/* AT+BM_SCC=scene_recall,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx,tid,xxx */
/* AT+BM_SCC=scene_recall,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx,tid,xxx,tt_time,xxx,delay,xxx */
/* AT+BM_SCC=scene_recall_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx,tid,xxx */
/* AT+BM_SCC=scene_recall_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx,tid,xxx,tt_time,xxx,delay,xxx */
uint32_t cmd_bt_mesh_scene_scene_recall(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13,17:index
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scene_cli_model(),
    };
    bt_mesh_time_scene_client_set_msg_t set = {0};
    bt_mesh_scene_client_status_t status = {0};
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "scene_number") != 0)) {  // 9:index
        BT_ERR("cmd err, %s", argv[9]);            // 9:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(set.scene_recall.scene_number, argv[10]);  // 10: index
    SHELL_PARAM_UINT(set.scene_recall.tid, argv[12]);           // 12 : index

    if (argc == 17) {                                                                  // 17:size
        if ((strcmp(argv[13], "tt_time") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13,15:index
            BT_ERR("cmd prop_id err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.scene_recall.op_en = 1;
        SHELL_PARAM_UINT(set.scene_recall.trans_time, argv[14]);  // 14:index
        // trans_time类型限制，数据不会超过256，超过会进行截取
        BT_INFO_SHELL("trans_time is %d", set.scene_recall.trans_time);
        SHELL_PARAM_UINT(set.scene_recall.delay, argv[16]);  // 16:index
    }

    status.scene_status.status_code = 0x03;
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "scene_recall") == 0) {
        BT_INFO_SHELL("cmd is scene_recall");
        common.opcode = BLE_MESH_MODEL_OP_SCENE_RECALL;
    } else {
        common.opcode = BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK;
    }
    ret = bt_mesh_scene_client_set_state(&common, &set, &status);
    if (status.scene_status.status_code == SCENE_NOT_FOUND) {
        BT_WARN("Scene Number 0x%04x not exists", set.scene_recall.scene_number);
    }
    return ret;
}

/* AT+BM_SCC=scene_delete,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx */
/* AT+BM_SCC=scene_delete_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,scene_number,xxx */
uint32_t cmd_bt_mesh_scene_scene_delete(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scene_cli_model(),
    };
    bt_mesh_time_scene_client_set_msg_t set = {0};
    bt_mesh_scene_client_status_t status = {0};
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "scene_number") != 0)) {  // 9:index
        BT_ERR("cmd err, %s", argv[9]);            // 9:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(set.scene_delete.scene_number, argv[10]);  // 10:index
    status.scene_status.status_code = 0x03;

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "scene_delete") == 0) {
        BT_INFO_SHELL("cmd is scene_delete");
        common.opcode = BLE_MESH_MODEL_OP_SCENE_DELETE;
    } else {
        common.opcode = BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK;
    }
    ret = bt_mesh_scene_client_set_state(&common, &set, &status);

    return ret;
}

uint32_t cmd_ble_mesh_scene_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "scene_get") == 0) {
        ret = cmd_bt_mesh_scene_scene_get(argc, argv);
    } else if (strcmp(argv[0], "register_get") == 0) {
        ret = cmd_bt_mesh_scene_register_get(argc, argv);
    } else if ((strcmp(argv[0], "scene_store") == 0) || (strcmp(argv[0], "scene_store_unack")) == 0) {
        ret = cmd_bt_mesh_scene_scene_store(argc, argv);
    } else if ((strcmp(argv[0], "scene_recall") == 0) || (strcmp(argv[0], "scene_recall_unack")) == 0) {
        ret = cmd_bt_mesh_scene_scene_recall(argc, argv);
    } else if ((strcmp(argv[0], "scene_delete") == 0) || (strcmp(argv[0], "scene_delete_unack")) == 0) {
        ret = cmd_bt_mesh_scene_scene_delete(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_scene_client(const ble_mesh_scene_client_t *args)
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
    ASSIGN_PARA(argv, args->para14, argc);
    ASSIGN_PARA(argv, args->para15, argc);
    ASSIGN_PARA(argv, args->para16, argc);
    ASSIGN_PARA(argv, args->para17, argc);

    uint32_t ret = cmd_ble_mesh_scene_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_scene_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_scene_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scene_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_scene_client_t, para17)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_TIME_CLI)
/* AT+BMTIME=time_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_time_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t tai_seconds[TAI_SECONDS_LEN] = {0};
    struct bt_mesh_time_status_t time_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_TIME_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = bt_mesh_time_get(&common, (void *)&time_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_time_get fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("tai_seconds:0x%02x%02x%02x%02x%02x",
        time_status.tai_seconds[0],   // 0:index
        time_status.tai_seconds[1],   // 1:index
        time_status.tai_seconds[2],   // 2:index
        time_status.tai_seconds[3],   // 3:index
        time_status.tai_seconds[4]);  // 4:index
    if (memcmp(time_status.tai_seconds, tai_seconds, TAI_SECONDS_LEN) != 0) {
        BT_INFO_SHELL("subsecond:%u", time_status.subsecond);
        BT_INFO_SHELL("uncertainty:%u", time_status.uncertainty);
        BT_INFO_SHELL("authority:%u", time_status.time_authority);
        BT_INFO_SHELL("tai_utc_delta:%d", time_status.tai_utc_delta_curr);
        BT_INFO_SHELL("timezone_offset:%u", time_status.timezone_offset_curr);
    }

    return ret;
}

/* AT+BMTIME=time_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,tai_seconds,xxx,subsecond,xxx,
   uncertainty,xxx,time_authority,xxx,tai_utc_delta_curr,xxx,timezone_offset_curr,xxx */
int cmd_bt_mesh_time_set(int argc, const char *argv[])
{
    if (argc != 21) {  // 21:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
    };
    struct bt_mesh_time_status_t time_status = {0};
    struct bt_mesh_time_set_t set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    uint16_t authority = 0;
    uint16_t tai_utc_delta_curr = 0;

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "tai_seconds") != 0) || (strcmp(argv[11], "subsecond") != 0) ||       // 9, 11:argv index
        (strcmp(argv[13], "uncertainty") != 0) || (strcmp(argv[15], "time_authority") != 0) || // 13, 15:argv index
        (strcmp(argv[17], "tai_utc_delta_curr") != 0) ||    // 17:argv index
        (strcmp(argv[19], "timezone_offset_curr") != 0)) {  // 19:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (bt_mesh_at_str_to_data(argv[10], TAI_SECONDS_LEN, set.tai_seconds) != ERRCODE_BLE_MESH_SUCCESS) {  // 10:index
        BT_ERR("can't set tai_seconds to %s", argv[10]);
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0, 0xFF);  // 11,12:index, 0,0xFF:range
    SHELL_PARAM_UINT(set.subsecond, argv[12]);                // 12:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], 0, 0xFF);  // 13,14:index, 0,0xFF:range
    SHELL_PARAM_UINT(set.uncertainty, argv[14]);              // 14:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[15], argv[16], 0, 1);     // 15,16:index, 0,1:range
    SHELL_PARAM_UINT(authority, argv[16]);                    // 16:index
    set.time_authority = authority;
    SHELL_PARAM_RANGE_CHECK_IN(argv[17], argv[18], 0, 0x7FFF);  // 17,18:index, 0,0x7FFF:range
    SHELL_PARAM_UINT(tai_utc_delta_curr, argv[18]);             // 18:index
    set.tai_utc_delta_curr = tai_utc_delta_curr;
    SHELL_PARAM_RANGE_CHECK_IN(argv[19], argv[20], 0, 0xFF);  // 19,20:index, 0,0xFF:range
    SHELL_PARAM_UINT(set.timezone_offset_curr, argv[20]);     // 20:index

    common.opcode = BLE_MESH_MODEL_OP_TIME_SET;
    ret = bt_mesh_time_set(&common, &set, &time_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_time_set fail, ret = %d", ret);
        return ret;
    }
    return ret;
}

/* AT+BMTIME=timezone_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_timezone_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_timezone_status_t timezone_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_TIME_ZONE_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = bt_mesh_timezone_get(&common, (void *)&timezone_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_timezone_get fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("timezone_offset_curr:%u", timezone_status.timezone_offset_curr);
    BT_INFO_SHELL("timezone_offset_new:%u", timezone_status.timezone_offset_new);
    BT_INFO_SHELL("tai_zone_change:0x%02x%02x%02x%02x%02x",
        timezone_status.tai_zone_change[0],   // 0:index
        timezone_status.tai_zone_change[1],   // 1:index
        timezone_status.tai_zone_change[2],   // 2:index
        timezone_status.tai_zone_change[3],   // 3:index
        timezone_status.tai_zone_change[4]);  // 4:index

    return ret;
}

/* AT+BMTIME=timezone_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,timezone_offset_new,xxx,tai_zone_change,xxx */
int cmd_bt_mesh_timezone_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
    };
    struct bt_mesh_timezone_status_t timezone_status = {0};
    struct bt_mesh_timezone_set_t set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    uint8_t res = ERRCODE_BLE_MESH_SUCCESS;

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    // 9,11:argv index
    if ((strcmp(argv[9], "timezone_offset_new") != 0) || (strcmp(argv[11], "tai_zone_change") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0xFF);  // 9,10:index, 1,0xFF:range
    SHELL_PARAM_UINT(set.timezone_offset_new, argv[10]);     // 10:index

    res = bt_mesh_at_str_to_data(argv[12], TAI_OF_ZONE_CHANGE_LEN, set.tai_zone_change);  // 12:index
    if (res != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("ble mesh at tai_zone_change error");
        return ERRCODE_BLE_MESH_FAIL;
    }

    common.opcode = BLE_MESH_MODEL_OP_TIME_ZONE_SET;
    ret = bt_mesh_timezone_set(&common, (void *)&set, (void *)&timezone_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_timezone_set fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("timezone_offset_curr:%u", timezone_status.timezone_offset_curr);
    BT_INFO_SHELL("timezone_offset_new:%u", timezone_status.timezone_offset_new);
    BT_INFO_SHELL("tai_zone_change:0x%02x%02x%02x%02x%02x",
        timezone_status.tai_zone_change[0],   // 0:index
        timezone_status.tai_zone_change[1],   // 1:index
        timezone_status.tai_zone_change[2],   // 2:index
        timezone_status.tai_zone_change[3],   // 3:index
        timezone_status.tai_zone_change[4]);  // 4:index

    return ret;
}

/* AT+BMTIME=tai_utc_delta_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_tai_utc_delta_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_tai_utc_delta_status_t tai_utc_delta_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = bt_mesh_tai_utc_delta_get(&common, (void *)&tai_utc_delta_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_tai_utc_delta_get fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("tai_utc_delta_curr:%u", tai_utc_delta_status.tai_utc_delta_curr);
    BT_INFO_SHELL("tai_utc_delta_new:%u", tai_utc_delta_status.tai_utc_delta_new);
    BT_INFO_SHELL("tai_delta_change:0x%02x%02x%02x%02x%02x",
        tai_utc_delta_status.tai_delta_change[0],   // 0:index
        tai_utc_delta_status.tai_delta_change[1],   // 1:index
        tai_utc_delta_status.tai_delta_change[2],   // 2:index
        tai_utc_delta_status.tai_delta_change[3],   // 3:index
        tai_utc_delta_status.tai_delta_change[4]);  // 4:index

    return ret;
}

/* AT+BMTIME=tai_utc_delta_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,tai_utc_delta_new,xxx,tai_delta_change,xxx */
int cmd_bt_mesh_tai_utc_delta_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
    };
    struct bt_mesh_tai_utc_delta_status_t tai_utc_delta_status = {0};
    struct bt_mesh_tai_utc_delta_set_t set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;
    uint8_t res = ERRCODE_BLE_MESH_SUCCESS;
    uint16_t tai_utc_delta_new = 0;

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    // 9,11:argv index
    if ((strcmp(argv[9], "tai_utc_delta_new") != 0) || (strcmp(argv[11], "tai_delta_change") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0x7FFF);  // 9,10:index, 1,0x7FFF:range
    SHELL_PARAM_UINT(tai_utc_delta_new, argv[10]);             // 10:index
    set.tai_utc_delta_new = tai_utc_delta_new;

    res = bt_mesh_at_str_to_data(argv[12], TAI_OF_DELTA_CHANGE_LEN, set.tai_delta_change);  // 12:index
    if (res != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("ble mesh at tai_delta_change error");
        return ERRCODE_BLE_MESH_FAIL;
    }

    common.opcode = BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET;
    ret = bt_mesh_tai_utc_delta_set(&common, (void *)&set, (void *)&tai_utc_delta_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_timezone_set fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("tai_utc_delta_curr:%u", tai_utc_delta_status.tai_utc_delta_curr);
    BT_INFO_SHELL("tai_utc_delta_new:%u", tai_utc_delta_status.tai_utc_delta_new);
    BT_INFO_SHELL("tai_delta_change:0x%02x%02x%02x%02x%02x",
        tai_utc_delta_status.tai_delta_change[0],   // 0:index
        tai_utc_delta_status.tai_delta_change[1],   // 1:index
        tai_utc_delta_status.tai_delta_change[2],   // 2:index
        tai_utc_delta_status.tai_delta_change[3],   // 3:index
        tai_utc_delta_status.tai_delta_change[4]);  // 4:index

    return ret;
}

/* AT+BMTIME=time_role_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_time_role_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_time_role_status_t time_role_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_TIME_ROLE_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = bt_mesh_time_role_get(&common, (void *)&time_role_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_time_role_get fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("time_role:%u", time_role_status.time_role);

    return ret;
}

/* AT+BMTIME=time_role_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,time_role,xxx */
int cmd_bt_mesh_time_role_set(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_time_cli_model(),
    };
    struct bt_mesh_time_role_status_t time_role_status = {0};
    struct bt_mesh_time_role_set_t set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    // 9:argv index
    if (strcmp(argv[9], "time_role") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 3);  // 9,10:index, 0,3:range
    SHELL_PARAM_UINT(set.time_role, argv[10]);            // 10:index

    common.opcode = BLE_MESH_MODEL_OP_TIME_ROLE_SET;
    ret = bt_mesh_time_role_set(&common, (void *)&set, (void *)&time_role_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_timezone_set fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("time_role:%u", time_role_status.time_role);

    return ret;
}

uint32_t cmd_ble_mesh_time_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "time_get") == 0) {
        /* AT+BMTIME=time_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_time_get(argc, argv);
    } else if (strcmp(argv[0], "time_set") == 0) {
        /* AT+BMTIME=time_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,tai_seconds,xxx,subsecond,xxx,
            uncertainty,xxx,time_authority,xxx,tai_utc_delta_curr,xxx,timezone_offset_curr,xxx */
        ret = cmd_bt_mesh_time_set(argc, argv);
    } else if (strcmp(argv[0], "timezone_get") == 0) {
        /* AT+BMTIME=timezone_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_timezone_get(argc, argv);
    } else if (strcmp(argv[0], "timezone_set") == 0) {
        /* AT+BMTIME=timezone_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,timezone_offset_new,xxx,tai_zone_change,xxx */
        ret = cmd_bt_mesh_timezone_set(argc, argv);
    } else if (strcmp(argv[0], "tai_utc_delta_get") == 0) {
        /* AT+BMTIME=tai_utc_delta_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_tai_utc_delta_get(argc, argv);
    } else if (strcmp(argv[0], "tai_utc_delta_set") == 0) {
        /* AT+BMTIME=tai_utc_delta_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,tai_utc_delta_new,xxx,tai_delta_change,xxx */
        ret = cmd_bt_mesh_tai_utc_delta_set(argc, argv);
    } else if (strcmp(argv[0], "time_role_get") == 0) {
        /* AT+BMTIME=time_role_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_time_role_get(argc, argv);
    } else if (strcmp(argv[0], "time_role_set") == 0) {
        /* AT+BMTIME=time_role_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,time_role,xxx */
        ret = cmd_bt_mesh_time_role_set(argc, argv);
    } else {
        BT_INFO_SHELL("set cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_time_client(const ble_mesh_time_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_time_client args[1]:%s, args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[21] = {NULL};
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
    ASSIGN_PARA(argv, args->para18, argc);
    ASSIGN_PARA(argv, args->para19, argc);
    ASSIGN_PARA(argv, args->para20, argc);
    ASSIGN_PARA(argv, args->para21, argc);

    uint32_t ret = cmd_ble_mesh_time_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_time_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_time_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_time_client_t, para20)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_time_client_t, para21)}};
#endif

#if MYNEWT_VAL(BLE_MESH_SCHEDULER_CLI)
/* AT+BMSCHEDULER=scheduler_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_scheduler_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_scheduler_status_t scheduler_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scheduler_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_SCHEDULER_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = bt_mesh_scheduler_get(&common, (void *)&scheduler_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_scheduler_get fail, ret = %d", ret);
        return ret;
    }

    BT_INFO_SHELL("schedules:%u", scheduler_status.schedules);

    return ret;
}

/* AT+BMSCHEDULER=scheduler_action_get,app,xxx,net,xxx,dst,xxx,ttl,xxx,index,xxx */
int cmd_bt_mesh_scheduler_action_get(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t index = 0;
    struct bt_mesh_scheduler_act_t scheduler_act_status = {0};
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scheduler_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    // 9:argv index
    if (strcmp(argv[9], "index") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0xF);  // 9,10:index, 1,0xF:range
    SHELL_PARAM_UINT(index, argv[10]);                      // 10:index

    int ret = bt_mesh_scheduler_act_get(&common, (void *)&scheduler_act_status, index);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("bt_mesh_scheduler_act_get fail, ret = %d", ret);
        return ret;
    }

    char buffer[BT_MESH_MON_LEN] = {0};
    char week_buff[BT_MESH_WEEK_LEN] = {0};
    BT_INFO_SHELL("index:%u", scheduler_act_status.index);
    BT_INFO_SHELL("scheduler time:");
    BT_INFO_SHELL("year:%u", scheduler_act_status.year + DELAY_YEAR);
    convert_month_bit_mask(scheduler_act_status.month, buffer, BT_MESH_MON_LEN);
    BT_INFO_SHELL("month:%s", buffer);
    BT_INFO_SHELL("day:%02u", scheduler_act_status.day);
    BT_INFO_SHELL(
        "time:%02u:%02u:%02u", scheduler_act_status.hour, scheduler_act_status.minute, scheduler_act_status.second);
    convert_week_bit_mask(scheduler_act_status.day_of_week, week_buff, BT_MESH_WEEK_LEN);
    BT_INFO("day_of_week:%s", week_buff);
    BT_INFO_SHELL("action:%u", scheduler_act_status.action);
    BT_INFO_SHELL("trans_time:%u", scheduler_act_status.trans_time);
    BT_INFO_SHELL("scene_number:%u", scheduler_act_status.scene_number);

    return ret;
}

int bt_mesh_scheduler_time_param_proc(struct bt_mesh_scheduler_act_t *set, char *str)
{
    char *input = str;
    char *token = NULL;
    const char *sep = "/";
    int i = 0;
    uint16_t year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

    token = strtok(input, sep);
    while (token != NULL) {
        if (i == 0) { // 0:index
            SHELL_PARAM_RANGE_CHECK_IN("year", token, 0, 0x64);
            SHELL_PARAM_UINT(year, token);
            set->year = year;
        }
        if (i == 1) { // 1:index
            SHELL_PARAM_RANGE_CHECK_IN("month", token, 0, 0x0FFF);
            SHELL_PARAM_UINT(month, token);
            set->month = month;
        }
        if (i == 2) { // 2:index
            SHELL_PARAM_RANGE_CHECK_IN("day", token, 0, 0x1F);
            SHELL_PARAM_UINT(day, token);
            set->day = day;
        }
        if (i == 3) { // 3:index
            SHELL_PARAM_RANGE_CHECK_IN("hour", token, 0, 0x19);
            SHELL_PARAM_UINT(hour, token);
            set->hour = hour;
        }
        if (i == 4) { // 4:index
            SHELL_PARAM_RANGE_CHECK_IN("minute", token, 0, 0x3F);
            SHELL_PARAM_UINT(minute, token);
            set->minute = minute;
        }
        if (i == 5) { // 5:index
            SHELL_PARAM_RANGE_CHECK_IN("second", token, 0, 0x3F);
            SHELL_PARAM_UINT(second, token);
            set->second = second;
        }
        token = strtok(NULL, sep);
        i++;
    }
    
    return ERRCODE_BLE_MESH_SUCCESS;
}

int bt_mesh_scheduler_param_proc(struct bt_mesh_scheduler_act_t *set, const char *argv[])
{
    uint16_t index = 0, day_of_week = 0, action = 0, trans_time = 0;
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0x0F); // 9,10:index
    SHELL_PARAM_UINT(index, argv[10]); // 10:index
    set->index = index;
    ret = bt_mesh_scheduler_time_param_proc(set, (char *)argv[12]); // 12:index
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], 0x0, 0x7F); // 13,14:index
    SHELL_PARAM_UINT(day_of_week, argv[14]); // 14:index
    set->day_of_week = day_of_week;
    SHELL_PARAM_UINT(action, argv[16]); // 16:index
    if (action > SCHEDULE_ACT_SCENE_RECALL && action != 0xF) {
        BT_ERR("param error action = %u", action);
        return ERRCODE_BLE_MESH_FAIL;
    }
    set->action = action;
    SHELL_PARAM_RANGE_CHECK_IN(argv[17], argv[18], 0, 0xFF); // 17,18:index
    SHELL_PARAM_UINT(trans_time, argv[18]); // 18:index
    set->trans_time = trans_time;
    SHELL_PARAM_RANGE_CHECK_IN(argv[19], argv[20], 0, 0xFFFF); // 19,20:index
    SHELL_PARAM_UINT(set->scene_number, argv[20]); // 20:index

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMTIME=time_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,index,xxx,year,xxx,month,xxx,day,xxx,hour,xxx,
    minute,xxx,second,xxx,day_of_week,xxx,action,xxx,trans_time,xxx,scene_number,xxx */
uint32_t cmd_bt_mesh_scheduler_action_set(int argc, const char *argv[])
{
    if (argc != 21) {  // 21:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_scheduler_cli_model(),
    };
    struct bt_mesh_scheduler_act_t scheduler_act_status = {0};
    struct bt_mesh_scheduler_act_t set = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "idx") != 0) || (strcmp(argv[11], "time") != 0) || // 9,11:argv index
        (strcmp(argv[13], "week") != 0) || (strcmp(argv[15], "act") != 0) || // 13,15:argv index
        (strcmp(argv[17], "tt") != 0) || (strcmp(argv[19], "scene_num") != 0)) { // 17,19:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    ret = bt_mesh_scheduler_param_proc(&set, argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("param err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "sch_act_set") == 0) { // 0:argv index
        common.opcode = BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET;
        ret = bt_mesh_scheduler_act_set(&common, (void *)&set, (void *)&scheduler_act_status);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK;
        ret = bt_mesh_scheduler_act_set(&common, (void *)&set, NULL);
    }

    return ret;
}

uint32_t cmd_ble_mesh_scheduler_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "scheduler_get") == 0) {
        /* AT+BMSCHEDULER=scheduler_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_scheduler_get(argc, argv);
    } else if (strcmp(argv[0], "scheduler_action_get") == 0) {
        /* AT+BMSCHEDULER=scheduler_action_get,app,xxx,net,xxx,dst,xxx,ttl,xxx,index,xxx */
        ret = cmd_bt_mesh_scheduler_action_get(argc, argv);
    } else if ((strcmp(argv[0], "sch_act_set") == 0) || (strcmp(argv[0], "sch_act_set_unack") == 0)) {
        /* AT+BMSCHEDULER=sch_act_set,app,0,net,0,dst,3,ttl,7,idx,0,time,25-111-6-18-30-52,
            week,3,act,1,tt,2,scene_num,1 */
        /* AT+BMSCHEDULER=sch_act_set_unack,app,0,net,0,dst,3,ttl,7,idx,1,time,25-111-6-18-30-52,
            week,3,act,1,tt,2,scene_num,2 */
        ret = cmd_bt_mesh_scheduler_action_set(argc, argv);
    } else {
        BT_INFO_SHELL("set cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_scheduler_client(const ble_mesh_scheduler_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_scheduler_client args[1]:%s, args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[21] = {NULL};
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
    ASSIGN_PARA(argv, args->para18, argc);
    ASSIGN_PARA(argv, args->para19, argc);
    ASSIGN_PARA(argv, args->para20, argc);
    ASSIGN_PARA(argv, args->para21, argc);

    uint32_t ret = cmd_ble_mesh_scheduler_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_scheduler_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_scheduler_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_scheduler_client_t, para20)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_scheduler_client_t, para21)}};
#endif

const at_cmd_entry_t at_bt_mesh_time_scenes_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_SCENE_CLI)
    {
        "BMSCENE",  // scene 场景模型 AT+BM_SCC
        0,
        0,
        ble_mesh_scene_client,                   // 参数类型
        NULL,                                    // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_scene_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_TIME_CLI)
    {
        "BMTIME",  // time 时间模型 AT+BM_TIME
        0,
        0,
        ble_mesh_time_client,                   // 参数类型
        NULL,                                   // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_time_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_SCHEDULER_CLI)
    {
        "BMSCHEDULER",  // 调度器模型 AT+BM_TIME
        0,
        0,
        ble_mesh_scheduler_client,                   // 参数类型
        NULL,                                        // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_scheduler_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_time_scenes_parse_table) / sizeof(at_bt_mesh_time_scenes_parse_table[0]))

void mesh_at_time_scenes_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_time_scenes_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
