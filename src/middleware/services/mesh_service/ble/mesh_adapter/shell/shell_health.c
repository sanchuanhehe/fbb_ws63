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
#include "health_client.h"
#include "mesh/mesh.h"
#include "mesh/main.h"
#include "mesh/access.h"
#include "bts_def.h"
#include "model_opcode.h"
#include "client_common.h"
#include "device_property.h"
#include "mesh/cfg_cli.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_model_op.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"

#if MYNEWT_VAL(BLE_MESH_HEALTH_CLI)
void show_faults(uint8_t test_id, uint16_t cid, uint8_t *faults, size_t fault_count)
{
    size_t i;

    if (!fault_count) {
        BT_INFO_SHELL("Health Test ID 0x%02x Company ID 0x%04x: no faults\n", test_id, cid);
        return;
    }

    BT_INFO_SHELL("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n", test_id, cid, fault_count);

    for (i = 0; i < fault_count; i++) {
        BT_INFO_SHELL("\t0x%02x\n", faults[i]);
    }
}

static void health_get_common(uint16_t *addr, uint16_t *app_idx)
{
    struct bt_mesh_model *mod_cli = bt_mesh_get_health_cli_model();
    if (mod_cli == NULL) {
        return;
    }

    struct bt_mesh_model_pub *pub_cli = mod_cli->pub;
    if (pub_cli == NULL) {
        BT_ERR("pub_cli is NULL");
        return;
    }

    *addr = pub_cli->addr;
    if (addr == BT_MESH_ADDR_UNASSIGNED) {
        BT_INFO_SHELL("pub addr is UNASSIGNED,pls set the pub addr");
        return;
    }

    *app_idx = pub_cli->key;
}

/* AT+BM_HC=fault_get,cid,xxx,dst,xxx */
uint32_t cmd_bt_mesh_health_fault_get(int argc, const char *argv[])
{
    if (argc != 5 && argc != 3) {  // 3,5:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t faults[32];  // 32:size
    size_t fault_count;
    uint8_t test_id = 0;
    uint16_t cid;
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    if (strcmp(argv[1], "cid") != 0) {   // 3:index
        BT_ERR("cmd err, %s", argv[1]);  // 3:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(cid, argv[2]);  // 2:index
    fault_count = sizeof(faults);
    BT_INFO_SHELL("cid is 0x%04x,fault_count = %d", cid, fault_count);
    SHELL_PARAM_UINT(addr, argv[4]);  // 4:index

    ret = bt_mesh_health_fault_get(addr, app_idx, cid, &test_id, faults, &fault_count);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to send Health Fault Get (err %d)", ret);
    } else {
        show_faults(test_id, cid, faults, fault_count);
    }

    return ret;
}

/* AT+BM_HC=fault_clear,cid,xxx,dst,xxx */
/* AT+BM_HC=fault_clear_unack,cid,xxx,dst,xxx */
uint32_t cmd_bt_mesh_health_fault_clear(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t faults[32];  // 32:size
    size_t fault_count;
    uint8_t test_id = 0;
    uint16_t cid;
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    if ((strcmp(argv[1], "cid") != 0) || (strcmp(argv[3], "dst") != 0)) {  // 3:index
        BT_ERR("cmd err, %s,%s", argv[1], argv[3]);                            // 3:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(cid, argv[2]);      // 2:index
    SHELL_PARAM_UINT(addr, argv[4]);  // 4:index
    fault_count = sizeof(faults);
    BT_INFO_SHELL("cid is 0x%04x,fault_count = %d", cid, fault_count);
    if ((strcmp(argv[0], "fault_clear") == 0)) {
        BT_INFO_SHELL("cmd is   fault_clear");
        ret = bt_mesh_health_fault_clear(addr, app_idx, cid, &test_id, faults, &fault_count);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_ERR("Failed to send Health Fault Clear (err %d)\n", ret);
        } else {
            show_faults(test_id, cid, faults, fault_count);
        }
    } else if (strcmp(argv[0], "fault_clear_unack") == 0) {
        BT_INFO_SHELL("cmd is   fault_clear_unack");
        ret = bt_mesh_health_fault_clear(addr, app_idx, cid, NULL, NULL, NULL);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_ERR("Health Fault Clear Unacknowledged failed (err %d)\n", ret);
        }
    }
    return ret;
}

/* AT+BM_HC=fault_test,cid,xxx,dst,xxx,test_id,xxx */
/* AT+BM_HC=fault_test_unack,cid,xxx,dst,xxx,test_id,xxx */
uint32_t cmd_bt_mesh_health_fault_test(int argc, const char *argv[])
{
    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t faults[32];  // 32:size
    size_t fault_count;
    uint8_t test_id;
    uint16_t cid;
    int ret;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    if ((strcmp(argv[1], "cid") != 0) || (strcmp(argv[3], "dst") != 0) ||     // 3:index
        (strcmp(argv[5], "test_id") != 0)) {  // 5:index
        BT_ERR("cmd err, %s,%s", argv[1], argv[3]);                            // 3:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(cid, argv[2]);      // 2:index
    SHELL_PARAM_UINT(addr, argv[4]);  // 4:index
    SHELL_PARAM_UINT(test_id, argv[6]);  // 6:index
    fault_count = sizeof(faults);
    BT_INFO_SHELL("cid is 0x%04x,testid is 0x%04x,fault_count = %d", cid, test_id, fault_count);
    if (addr == ble_mesh_get_primary_addr()) {     // 设置本端状态
        ble_mesh_fault_test(NULL, test_id, cid);
        return ERRCODE_BLE_MESH_SUCCESS;
    }
    if ((strcmp(argv[0], "fault_test") == 0)) {
        BT_INFO_SHELL("cmd is   fault_test");
        ret = bt_mesh_health_fault_test(addr, app_idx, cid, test_id, faults, &fault_count);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_ERR("Failed to send Health Fault Clear (err %d)\n", ret);
        } else {
            show_faults(test_id, cid, faults, fault_count);
        }
    } else {
        BT_INFO_SHELL("cmd is   fault_test_unack");
        ret = bt_mesh_health_fault_test(addr, app_idx, cid, test_id, NULL, NULL);
        if (ret != ERRCODE_BLE_MESH_SUCCESS) {
            BT_ERR("Health Fault Test Unacknowledged failed (err %d)", ret);
        }
    }
    return ret;
}

/* AT+BM_HC=period_get,dst,xxx */
uint32_t cmd_bt_mesh_health_period_get(int argc, const char *argv[])
{
    if (argc != 3) { // 3: index
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t divisor;
    int ret;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    SHELL_PARAM_UINT(addr, argv[2]); // 2 : index
    ret = bt_mesh_health_period_get(addr, app_idx, &divisor);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to send Health Period Get (err %d)", ret);
    } else {
        BT_INFO("Health FastPeriodDivisor: %u", divisor);
    }

    return ret;
}

/* AT+BM_HC=period_set,divisor,xxx,dst,xxx */
/* AT+BM_HC=period_set_unack,divisor,xxx,dst,xxx */
uint32_t cmd_bt_mesh_health_period_set(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t divisor, updated_divisor;
    int ret;

    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    SHELL_PARAM_UINT(divisor, argv[2]); // 2 : index
    SHELL_PARAM_UINT(addr, argv[4]); // 4 : index
    updated_divisor = 0;
    if ((strcmp(argv[0], "period_set")) == 0) {
        BT_INFO_SHELL("cmd is   period_set");
        ret = bt_mesh_health_period_set(addr, app_idx, divisor, &updated_divisor);
        if (ret == ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO("Health FastPeriodDivisor: %u", updated_divisor);
        }
    } else {
        BT_INFO_SHELL("cmd is   period_set_unack");
        ret = bt_mesh_health_period_set(addr, app_idx, divisor, NULL);
    }
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to send Health Period Set (err %d)", ret);
    }
    return ret;
}

/* AT+BM_HC=attention_get,dst,xxx */
uint32_t cmd_bt_mesh_health_attention_get(int argc, const char *argv[])
{
    if (argc != 3) { // 3: index
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t attention;
    int ret;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);
    SHELL_PARAM_UINT(addr, argv[2]); // 2 : index

    ret = bt_mesh_health_attention_get(addr, app_idx, &attention);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to send Health Attention Get (err %d)", ret);
    } else {
        BT_INFO("Health Attention Timer: %u\n", attention);
    }

    return ret;
}

/* AT+BM_HC=attention_set,attention,xxx,dst,xxx */
/* AT+BM_HC=attention_set_unack,attention,xxx,dst,xxx */
uint32_t cmd_bt_mesh_health_attention_set(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t attention, updated_attention;
    int ret;
    uint16_t addr = BT_MESH_ADDR_UNASSIGNED;
    uint16_t app_idx = 0;
    health_get_common(&addr, &app_idx);

    SHELL_PARAM_UINT(attention, argv[2]); // 2 : index
    SHELL_PARAM_UINT(addr, argv[4]); // 4 : index
    if ((strcmp(argv[0], "attention_set")) == 0) {
        BT_INFO_SHELL("cmd is attention_set");
        ret = bt_mesh_health_attention_set(addr, app_idx, attention, &updated_attention);
        if (ret == ERRCODE_BLE_MESH_SUCCESS) {
            BT_INFO("Health Attention Timer: %u\n", updated_attention);
        }
    } else {
        BT_INFO_SHELL("cmd is attention_set_unack");
        ret = bt_mesh_health_attention_set(addr, app_idx, attention, NULL);
    }
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to send Health Period Set (err %d)", ret);
    }

    return ret;
}

uint32_t cmd_ble_mesh_health_cli_op_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (argc < 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "fault_get") == 0) {
        ret = cmd_bt_mesh_health_fault_get(argc, argv);
    } else if (strcmp(argv[0], "fault_clear") == 0 || (strcmp(argv[0], "fault_clear_unack")) == 0) {
        ret = cmd_bt_mesh_health_fault_clear(argc, argv);
    } else if ((strcmp(argv[0], "fault_test") == 0) || (strcmp(argv[0], "fault_test_unack")) == 0) {
        ret = cmd_bt_mesh_health_fault_test(argc, argv);
    } else if (strcmp(argv[0], "period_get") == 0) {
        ret = cmd_bt_mesh_health_period_get(argc, argv);
    } else if ((strcmp(argv[0], "period_set") == 0) || (strcmp(argv[0], "period_set_unack")) == 0) {
        ret = cmd_bt_mesh_health_period_set(argc, argv);
    } else if (strcmp(argv[0], "attention_get") == 0) {
        ret = cmd_bt_mesh_health_attention_get(argc, argv);
    } else if ((strcmp(argv[0], "attention_set") == 0) || (strcmp(argv[0], "attention_set_unack")) == 0) {
        ret = cmd_bt_mesh_health_attention_set(argc, argv);
    } else {
        BT_INFO_SHELL("cmd err %s", argv[0]);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ret;
}

// health model 模型 -- -参数类型如何构建待确定
at_ret_t at_bt_mesh_health_model(const ble_mesh_health_model_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_health_model args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[23] = {NULL};
    int argc = 0;
    // 进行参数处理
    if (args->para1 != NULL) {
        argv[argc++] = args->para1;
    }
    if (args->para2 != NULL) {
        argv[argc++] = args->para2;
    }
    if (args->para3 != NULL) {
        argv[argc++] = args->para3;
    }
    if (args->para4 != NULL) {
        argv[argc++] = args->para4;
    }
    if (args->para5 != NULL) {
        argv[argc++] = args->para5;
    }
    if (args->para6 != NULL) {
        argv[argc++] = args->para6;
    }
    if (args->para7 != NULL) {
        argv[argc++] = args->para7;
    }
    uint32_t ret = cmd_ble_mesh_health_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_health_cli_op_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_health_model[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_health_model_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_health_model_t, para7)},
};
#endif

const at_cmd_entry_t at_bt_mesh_health_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_HEALTH_CLI)
    {
        "BMHEALTH",  // health 模型 AT+BM_HC
        0,
        0,
        ble_mesh_health_model,                   // 参数类型
        NULL,                                    // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_health_model,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_health_parse_table) / sizeof(at_bt_mesh_health_parse_table[0]))

void mesh_at_health_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_health_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
