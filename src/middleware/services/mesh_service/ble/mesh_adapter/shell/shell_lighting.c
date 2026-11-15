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
#include "lightness_client.h"
#include "light_ctl_client.h"
#include "device_property.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHT_CTL_CLIENT)

/* AT+BMLCCM=light_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_light_ctl_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_GET,
    };

    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_light_ctl_status get_value = {0};
    int ret = bt_mesh_light_ctl_client_get_state(&common, &get_value);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("Acknownledgement from LIGHT_CTL_SRV");
    BT_INFO_SHELL("Present CTL Lightness = %u", get_value.present_ctl_lightness);
    BT_INFO_SHELL("Present CTL Temperature = %u", get_value.present_ctl_temperature);
    BT_INFO_SHELL("op_en = %d", get_value.op_en);
    if (get_value.op_en) {
        BT_INFO_SHELL("Target CTL Lightness = %u", get_value.target_ctl_lightness);
        BT_INFO_SHELL("Target CTL Temperature = %u", get_value.target_ctl_temperature);
        BT_INFO_SHELL("Remaining Time = %u", get_value.remain_time);
    }

    return ret;
}

int cmd_bt_mesh_light_ctl_set_check(int argc, const char *argv[])
{
    if (argc != 17 && argc != 21) {  // 17:argc size;21:argc size
        BT_ERR("cmd_ble_mesh_onoff_get_proc:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "light") != 0) || (strcmp(argv[11], "temperature") != 0) ||  // 9:argv index;11:argv index
        (strcmp(argv[13], "delta_uv") != 0) || (strcmp(argv[15], "tid") != 0)) {      // 13:argv index;15:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    // light 0x0000–0xFFFE
    // temperature 0x0320–0x4E20
    // delta_uv -0x7FFF-0x7FFF
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0x0000, 0xFFFF);    // 9,10:index, 0x0000,0xFFFF:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0x0320, 0x4E20);   // 11,12:index, 0x0320,0x4E20:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], -0x8000, 0x7FFF);  // 13,14:index, -0x8000,0x7FFF:range

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMLCCM=light_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,light,xxx,temperature,xxx,delta_uv,xxx,tid,xxx */
/* AT+BMLCCM=light_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,light,xxx,temperature,xxx,delta_uv,xxx,tid,xxx */
uint32_t cmd_bt_mesh_light_ctl_set(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
    };
    if (cmd_bt_mesh_light_ctl_set_check(argc, argv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct bt_mesh_light_ctl_set set = {0};
    int ret;
    SHELL_PARAM_UINT(set.ctl_lightness, argv[10]);                                // 10 : index
    SHELL_PARAM_UINT(set.ctl_temperature, argv[12]);                              // 12 : index
    set.ctl_delta_uv = atoi(argv[14]);                                            // 14:argv index
    SHELL_PARAM_UINT(set.tid, argv[16]);                                          // 16 : index
    if (argc == 21) {                                                             // 21:size
        if ((strcmp(argv[17], "tt") != 0) || (strcmp(argv[19], "delay") != 0)) {  // 17,19:index
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[18]);  // 18:index
        if (check_dtt(set.trans_time) != 0) {
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_UINT(set.delay, argv[20]);       // 20:index
    }

    if (strcmp(argv[0], "light_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_SET;
        struct bt_mesh_light_ctl_status get_value = {0};
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, &get_value);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("Present CTL Lightness = %u", get_value.present_ctl_lightness);
        BT_INFO_SHELL("Present CTL Temperature = %u", get_value.present_ctl_temperature);
        BT_INFO_SHELL("op_en = %d", get_value.op_en);
        if (get_value.op_en) {
            BT_INFO_SHELL("Target CTL Lightness = %u", get_value.target_ctl_lightness);
            BT_INFO_SHELL("Target CTL Temperature = %u", get_value.target_ctl_temperature);
            BT_INFO_SHELL("Remaining Time = %u", get_value.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK;
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, NULL);
    }
    return ret;
}

/* AT+BMLCCM=light_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_light_ctl_temp_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET,
    };

    if (argc != 9) {  // 9:size
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
    struct bt_mesh_light_ctl_temperature_status get_value = {0};
    int ret = bt_mesh_light_ctl_client_get_state(&common, &get_value);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("Acknownledgement from LIGHT_CTL_TEMP_SRV");
    BT_INFO_SHELL("Present CTL Temperature = %u", get_value.present_ctl_temperature);
    BT_INFO_SHELL("present_ctl_delta_uv = %d", get_value.present_ctl_delta_uv);
    BT_INFO_SHELL("op_en = %d", get_value.op_en);
    if (get_value.op_en) {
        BT_INFO_SHELL("Target CTL temperature = %u", get_value.target_ctl_temperature);
        BT_INFO_SHELL("Target CTL delta_uv = %d", get_value.target_ctl_delta_uv);
        BT_INFO_SHELL("Remaining Time = %u", get_value.remain_time);
    }

    return ret;
}

uint32_t cmd_bt_mesh_light_ctl_temp_set_check(int argc, const char *argv[])
{
    if (argc != 15 && argc != 19) {  // 15,19:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "temperature") != 0) || (strcmp(argv[11], "delta_uv") != 0) ||  // 9,11:index
        (strcmp(argv[13], "tid") != 0)) {                                                // 13:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}
/* AT+BMLCCM=temp_set,app,0,net,0,dst,xxx,ttl,7,temperature,800,delta_uv,300,tid,xxx,tt,10,delay,10 */
/* AT+BMLCCM=temp_set_unack,app,0,net,0,dst,xxx,ttl,7,temperature,800,delta_uv,300,tid,xxx,tt,10,delay,10 */
int cmd_bt_mesh_light_ctl_temp_set(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
    };

    if (cmd_bt_mesh_light_ctl_temp_set_check(argc, argv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_light_ctl_temperature_set set = {0};
    int ret;
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0x0320, 0x4E20);              // 9,10:index, -0x320, 0x420:range
    SHELL_PARAM_UINT(set.ctl_temperature, argv[10]);                              // 10:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], -0x8000, 0x7FFF);              // 11,12:index, -0x8000, 0x7FFF:range
    set.ctl_delta_uv = atoi(argv[12]);                                            // 12:index
    SHELL_PARAM_UINT(set.tid, argv[14]);                                          // 14:index
    if (argc == 19) {                                                             // 19:size
        if ((strcmp(argv[15], "tt") != 0) || (strcmp(argv[17], "delay") != 0)) {  // 15,17:index
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[16]);  // 16:index
        if (check_dtt(set.trans_time) != 0) {
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_UINT(set.delay, argv[18]);       // 18:index
    }

    if (strcmp(argv[0], "temp_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET;
        struct bt_mesh_light_ctl_temperature_status get_value = {0};
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, &get_value);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("present_ctl_delta_uv = %d", get_value.present_ctl_delta_uv);
        BT_INFO_SHELL("op_en = %d", get_value.op_en);
        if (get_value.op_en) {
            BT_INFO_SHELL("Target CTL temperature = %u", get_value.target_ctl_temperature);
            BT_INFO_SHELL("Target CTL delta_uv = %d", get_value.target_ctl_delta_uv);
            BT_INFO_SHELL("Remaining Time = %u", get_value.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK;
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, NULL);
    }
    return ret;
}

/* AT+BMLCCM=temp_range_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_light_ctl_temp_range_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET,
    };

    if (argc != 9) {  // 9:size
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

    struct bt_mesh_light_ctl_temperature_range_status get_value = {0};
    int ret = bt_mesh_light_ctl_client_get_state(&common, &get_value);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("Acknownledgement from LIGHT_CTL_TEMP_RANGE_SRV");
    BT_INFO_SHELL("status_code = %u", get_value.status_code);
    BT_INFO_SHELL("range_min = %u", get_value.range_min);
    BT_INFO_SHELL("range_max = %u", get_value.range_max);

    return ret;
}

/* AT+BMLCCM=temp_range_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,min,xxx,max,xxx */
/* AT+BMLCCM=temp_range_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,min,xxx,max,xxx */
int cmd_bt_mesh_light_ctl_temp_range_set(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
    };

    if (argc != 13) {  // 13:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "min") != 0) || (strcmp(argv[11], "max") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_light_ctl_temperature_range_set set = {0};
    int ret;

    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0x0320, 0x4E20);   // 9,10:index, 0x0320,0x4E20:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0x0320, 0x4E20);  // 11,12:index, 0x0320,0x4E20:range

    SHELL_PARAM_UINT(set.range_min, argv[10]);  // 10:index
    SHELL_PARAM_UINT(set.range_max, argv[12]);  // 12:index

    if (set.range_max < set.range_min) {
        BT_ERR("can't set min value greater than the max value");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "temp_range_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET;
        struct bt_mesh_light_ctl_temperature_range_status get_value = {0};
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, &get_value);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK;
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, NULL);
    }

    return ret;
}

/* AT+BMLCCM=temp_default_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_light_ctl_temp_default_get(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET,
    };

    if (argc != 9) {  // 9:argc size
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
    
    struct bt_mesh_light_ctl_default_status get_value = {0};
    int ret;
    ret = bt_mesh_light_ctl_client_get_state(&common, &get_value);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("Acknownledgement from LIGHT_CTL_TEMP_DEFAULT_SRV");
    BT_INFO_SHELL("lightness = %u", get_value.lightness);
    BT_INFO_SHELL("temperature = %u", get_value.temperature);
    BT_INFO_SHELL("delta_uv = %d", get_value.delta_uv);

    return ret;
}

/* AT+BMLCCM=temp_default_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness,xxx,temperature,xxx,delta_uv,xxx */
/* AT+BMLCCM=temp_default_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness,xxx,temperature,xxx,delta_uv,xxx */
int cmd_bt_mesh_light_ctl_temp_default_set(int argc, const char *argv[])
{
    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_ctl_cli_model(),
    };

    if (argc != 15) {  // 15:argc size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[9], "lightness") != 0) || (strcmp(argv[11], "temperature") != 0) ||  // 9:argv index;11:argv index
        (strcmp(argv[13], "delta_uv") != 0)) {                                            // 13:argv index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_light_ctl_default_set set = {0};
    int ret;

    SHELL_PARAM_UINT(set.lightness, argv[10]);                        // 10:index
    SHELL_PARAM_UINT(set.temperature, argv[12]);                      // 12:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0x0001, 0xFFFF); // 9,10:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0x0320, 0x4E20); // 11,12:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], -0x8000, 0x7FFF);  // 13,14:index, -0x8000,0x7FFF:range
    set.delta_uv = atoi(argv[14]);                                    // 14:argv index

    if (strcmp(argv[0], "temp_default_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {  // 0:argv index
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET;
        struct bt_mesh_light_ctl_default_status get_value = {0};
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, &get_value);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK;
        ret = bt_mesh_light_ctl_client_set_state(&common, &set, NULL);
    }

    return ret;
}

uint32_t cmd_ble_mesh_light_ctl_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "light_get") == 0) {
        ret = cmd_bt_mesh_light_ctl_get(argc, argv);
    } else if (strcmp(argv[0], "light_set") == 0 || strcmp(argv[0], "light_set_unack") == 0) {
        ret = cmd_bt_mesh_light_ctl_set(argc, argv);
    } else if (strcmp(argv[0], "temp_get") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_get(argc, argv);
    } else if (strcmp(argv[0], "temp_set") == 0 || strcmp(argv[0], "temp_set_unack") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_set(argc, argv);
    } else if (strcmp(argv[0], "temp_range_get") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_range_get(argc, argv);
    } else if (strcmp(argv[0], "temp_range_set") == 0 || strcmp(argv[0], "temp_range_set_unack") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_range_set(argc, argv);
    } else if (strcmp(argv[0], "temp_default_get") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_default_get(argc, argv);
    } else if (strcmp(argv[0], "temp_default_set") == 0 || strcmp(argv[0], "temp_default_set_unack") == 0) {
        ret = cmd_bt_mesh_light_ctl_temp_default_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_light_ctl_client(const ble_mesh_light_ctl_client_t *args)
{
    BT_DBG("[debug] at_bt_mesh_light_ctl_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
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
    uint32_t ret = cmd_ble_mesh_light_ctl_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_light_ctl_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_light_ctl_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para20)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_light_ctl_client_t, para21)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHTNESS_CLIENT)
/* AT+BMLIGHTNESS=lightness_actual_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_lightness_actual_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct light_lightness_actual_status actual_status = {0};
    int ret = bt_mesh_lightness_get(&common, (void *)&actual_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("light_actual:%u", actual_status.actual);
    if (actual_status.op_en) {
        BT_INFO_SHELL("target_actual:%u, remain_time:%u", actual_status.target_actual, actual_status.remain_time);
    }

    return ret;
}
/* AT+BMLIGHTNESS=lightness_actual_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx */
/* AT+BMLIGHTNESS=lightness_actual_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BMLIGHTNESS=lightness_actual_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx */
/* AT+BMLIGHTNESS=lightness_actual_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx,tt,xxx,
delay,xxx */
uint32_t cmd_bt_mesh_lightness_actual_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "lightness_actual") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9:argv index;11:argv size
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct light_lightness_set set = {0};
    SHELL_PARAM_UINT(set.actual, argv[10]);  // 10:argv index
    SHELL_PARAM_UINT(set.tid, argv[12]);     // 12:argv index

    if (argc == 17) {                                                             // 17:argc size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13:argv index,15:argv index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:argv index
        if (check_dtt(set.trans_time) != 0) {
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_UINT(set.delay, argv[16]);       // 16:argv index
    }

    struct light_lightness_actual_status actual_status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (strcmp(argv[0], "lightness_actual_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET;
        ret = bt_mesh_lightness_set(&common, &set, (void *)&actual_status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("light_actual:%u", actual_status.actual);
        if (actual_status.op_en) {
            BT_INFO_SHELL("target_actual:%u, remain_time:%u", actual_status.target_actual, actual_status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK;
        ret = bt_mesh_lightness_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BMLIGHTNESS=lightness_linear_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_lightness_linear_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:argc size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct light_lightness_linear_status linear_status = {0};
    int ret = bt_mesh_lightness_get(&common, (void *)&linear_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("light_linear:%u", linear_status.linear);
    if (linear_status.op_en) {
        BT_INFO_SHELL("target_linear:%u, remain_time:%u", linear_status.target_linear, linear_status.remain_time);
    }

    return ret;
}
/* AT+BMLIGHTNESS=lightness_linear_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx */
/* AT+BMLIGHTNESS=lightness_linear_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx,tt,xxx,delay,xxx */
/* AT+BMLIGHTNESS=lightness_linear_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx */
/* AT+BMLIGHTNESS=lightness_linear_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx,tt,xxx,
delay,xxx */
uint32_t cmd_bt_mesh_lightness_linear_set(int argc, const char *argv[])
{
    if (argc != 13 && argc != 17) {  // 13:argc size;17:argc size;
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "lightness_linear") != 0) || (strcmp(argv[11], "tid") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct light_lightness_set set = {0};
    SHELL_PARAM_UINT(set.linear, argv[10]);  // 10:index
    SHELL_PARAM_UINT(set.tid, argv[12]);     // 12:index

    if (argc == 17) {                                                             // 17:size
        if ((strcmp(argv[13], "tt") != 0) || (strcmp(argv[15], "delay") != 0)) {  // 13,15:index
            BT_ERR("cmd option err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        set.op_en = 1;
        SHELL_PARAM_UINT(set.trans_time, argv[14]);  // 14:index
        if (check_dtt(set.trans_time) != 0) {
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_UINT(set.delay, argv[16]);  // 16:index
    }

    struct light_lightness_linear_status linear_status = {0};
    int ret = ERRCODE_BLE_MESH_SUCCESS;

    if (strcmp(argv[0], "lightness_linear_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET;
        ret = bt_mesh_lightness_set(&common, (void *)&set, (void *)&linear_status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("light_linear:%u", linear_status.linear);
        if (linear_status.op_en) {
            BT_INFO_SHELL("target_linear:%u, remain_time:%u", linear_status.target_linear, linear_status.remain_time);
        }
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK;
        ret = bt_mesh_lightness_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BMLIGHTNESS=default_lightness_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_default_lightness_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t light_default = 0;
    int ret = bt_mesh_lightness_get(&common, &light_default);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("light_default %u", light_default);

    return ret;
}
/* AT+BMLIGHTNESS=default_lightness_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_lightness,xxx */
/* AT+BMLIGHTNESS=default_lightness_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_lightness,xxx */
uint32_t cmd_bt_mesh_default_lightness_set(int argc, const char *argv[])
{
    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[9], "default_lightness") != 0)) {  // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct light_lightness_set set = {0};
    SHELL_PARAM_UINT(set.def, argv[10]);  // 10:index

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    
    // 单播才支持使用有响应，否则使用无响应
    if (strcmp(argv[0], "default_lightness_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        uint16_t light_default = 0;
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET;
        ret = bt_mesh_lightness_set(&common, (void *)&set, &light_default);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("light_default %u", light_default);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK;
        ret = bt_mesh_lightness_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BMLIGHTNESS=lightness_range_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_lightness_range_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:szie
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    
    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }
    struct light_lightness_range_status range_status = {0};
    int ret = bt_mesh_lightness_get(&common, (void *)&range_status);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("status_code:%u, range [%u,%u]",
        range_status.status_code,
        range_status.light_range_min,
        range_status.light_range_max);

    return ret;
}
/* AT+BMLIGHTNESS=lightness_range_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_min_level,xxx,lightness_max_level,xxx */
/* AT+BMLIGHTNESS=lightness_range_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_min_level,xxx,
lightness_max_level,xxx */
int cmd_bt_mesh_lightness_range_set(int argc, const char *argv[])
{
    if (argc != 13) {  // 13:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd ctx err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    // 9,11:index, 0:compare result the same
    if ((strcmp(argv[9], "lightness_min_level") != 0) || (strcmp(argv[11], "lightness_max_level") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct light_lightness_set set = {0};
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 1, 0xFFFF);   // 9,10:index, 1,0xFFFF:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 1, 0xFFFF);  // 11,12:index, 1,0xFFFF:range
    SHELL_PARAM_UINT(set.light_range_min, argv[10]);            // 10:index
    SHELL_PARAM_UINT(set.light_range_max, argv[12]);            // 12:index
    if (set.light_range_min > set.light_range_max) {
        BT_ERR("value err. light_range_min should be less than light_range_max.");
        return ERRCODE_BLE_MESH_FAIL;
    }

    int ret = ERRCODE_BLE_MESH_SUCCESS;
    if (strcmp(argv[0], "lightness_range_set") == 0 && BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        struct light_lightness_range_status range_status = {0};
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET;
        ret = bt_mesh_lightness_set(&common, (void *)&set, (void *)&range_status);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO_SHELL("fail, ret = %d", ret);
            return ret;
        }
        BT_INFO_SHELL("status_code:%u, range [%u,%u]",
            range_status.status_code,
            range_status.light_range_min,
            range_status.light_range_max);
    } else {
        common.opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK;
        ret = bt_mesh_lightness_set(&common, (void *)&set, NULL);
    }

    return ret;
}
/* AT+BMLIGHTNESS=last_lightness_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
uint32_t cmd_bt_mesh_last_lightness_get(int argc, const char *argv[])
{
    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_client_common_param_t common = {
        .model = bt_mesh_get_light_lightness_cli_model(),
        .opcode = BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET,
    };
    if (cmd_bt_mesh_ctx_info_set(&common.ctx, argv) != ERRCODE_BLE_MESH_SUCCESS) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!BT_MESH_ADDR_IS_UNICAST(common.ctx.addr)) {
        BT_INFO_SHELL("fail, should use unicast addr to query");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t light_last = 0;
    int ret = bt_mesh_lightness_get(&common, &light_last);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("fail, ret = %d", ret);
        return ret;
    }
    BT_INFO_SHELL("light_last %u", light_last);

    return ret;
}

uint32_t cmd_ble_mesh_lightness_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc <= 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "lightness_actual_get") == 0) {
        /* AT+BM_LNC=lightness_actual_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_lightness_actual_get(argc, argv);
    } else if (strcmp(argv[0], "lightness_actual_set") == 0 || strcmp(argv[0], "lightness_actual_set_unack") == 0) {
        /* AT+BM_LNC=lightness_actual_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx */
        /* AT+BM_LNC=lightness_actual_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx,tt,xxx,delay,xxx
         */
        /* AT+BM_LNC=lightness_actual_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx */
        /* AT+BM_LNC=lightness_actual_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_actual,xxx,tid,xxx,tt,xxx,
        delay,xxx */
        ret = cmd_bt_mesh_lightness_actual_set(argc, argv);
    } else if (strcmp(argv[0], "lightness_linear_get") == 0) {
        /* AT+BM_LNC=lightness_linear_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_lightness_linear_get(argc, argv);
    } else if (strcmp(argv[0], "lightness_linear_set") == 0 || strcmp(argv[0], "lightness_linear_set_unack") == 0) {
        /* AT+BM_LNC=lightness_linear_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx */
        /* AT+BM_LNC=lightness_linear_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx,tt,xxx,
        delay,xxx */
        /* AT+BM_LNC=lightness_linear_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx */
        /* AT+BM_LNC=lightness_linear_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_linear,xxx,tid,xxx,tt,xxx,
        delay,xxx */
        ret = cmd_bt_mesh_lightness_linear_set(argc, argv);
    } else if (strcmp(argv[0], "default_lightness_get") == 0) {
        /* AT+BM_LC=default_lightness_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_default_lightness_get(argc, argv);
    } else if (strcmp(argv[0], "default_lightness_set") == 0 || strcmp(argv[0], "default_lightness_set_unack") == 0) {
        /* AT+BM_LC=default_lightness_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_lightness,xxx */
        /* AT+BM_LC=default_lightness_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,default_lightness,xxx */
        ret = cmd_bt_mesh_default_lightness_set(argc, argv);
    } else if (strcmp(argv[0], "lightness_range_get") == 0) {
        /* AT+BM_LNC=lightness_range_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_lightness_range_get(argc, argv);
    } else if (strcmp(argv[0], "lightness_range_set") == 0 || strcmp(argv[0], "lightness_range_set_unack") == 0) {
        /* AT+BM_LNC=lightness_range_set,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_min_level,xxx,lightness_max_level,
        xxx */
        /* AT+BM_LNC=lightness_range_set_unack,app,xxx,net,xxx,dst,xxx,ttl,xxx,lightness_min_level,xxx,
        lightness_max_level,xxx */
        ret = cmd_bt_mesh_lightness_range_set(argc, argv);
    } else if (strcmp(argv[0], "last_lightness_get") == 0) {
        /* AT+BM_LC=last_lightness_get,app,xxx,net,xxx,dst,xxx,ttl,xxx */
        ret = cmd_bt_mesh_last_lightness_get(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

at_ret_t at_bt_mesh_lightness_client(const ble_mesh_lightness_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_lightness_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
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
    uint32_t ret = cmd_ble_mesh_lightness_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_lightness_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_lightness_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_lightness_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_lightness_client_t, para17)},
};
#endif

const at_cmd_entry_t at_bt_mesh_lighting_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHT_CTL_CLIENT)
    {
        "BMLCCM",  // BM_LCCM 色温 客户端控制
        0,
        0,
        ble_mesh_light_ctl_client,                   // 参数类型
        NULL,                                        // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_light_ctl_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHTNESS_CLIENT)
    {
        "BMLIGHTNESS",  // BM_LNC 亮度控制
        0,
        0,
        ble_mesh_lightness_client,                   // 参数类型
        NULL,                                        // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_lightness_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_lighting_parse_table) / sizeof(at_bt_mesh_lighting_parse_table[0]))

void mesh_at_lighting_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_lighting_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
