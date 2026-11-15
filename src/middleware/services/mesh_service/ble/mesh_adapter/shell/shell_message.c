/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: message shell source
 *
 * History:
 * 2025-11-1, Create file.
 */

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "mesh/main.h"
#include "mesh/access.h"
#include "access.h"
#include "bts_def.h"

#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_message.h"

#include "at.h"
#include "shell_utils.h"
#include "shell.h"

#define STR_PARA_LENGTH 40
#define MESSAGE_DATA_PARA_LENGTH 400

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
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
} ble_mesh_message_local_t;

static void bt_mesh_access_recv_cbk(const ble_mesh_msg_header_t *ctx, const uint8_t *data, uint16_t len)
{
    char data_str[2 * 31 + 1] = {0}; // 2:len 1 end char, 31 print max length
    if (data == NULL || len == 0) {
        return;
    }

    uint16_t cpy_len = len > 31 ? 31 : len; // 31 print max length
    ble_mesh_bin2hex(data, cpy_len, data_str, sizeof(data_str));
    BT_INFO_SHELL("opcode: 0x%08x, src_addr: %d, dest_addr: %d, recv_rssi: %d, recv_ttl: %d\n",
        ctx->recv_op, ctx->src_addr, ctx->dest_addr, ctx->recv_rssi, ctx->recv_ttl);
    BT_INFO_SHELL("data len:%d data: %s", len, data_str);
}

static void bt_mesh_beacon_recv_cbk(const bd_addr_t *addr, int8_t rssi, const uint8_t *data, uint8_t len)
{
    char data_str[2 * 31 + 1] = {0}; // 2:len 1 end char, 31 print max length
    if (data == NULL || len == 0) {
        return;
    }

    uint8_t cpy_len = len > 31 ? 31 : len; // 31 print max length
    ble_mesh_bin2hex(data, cpy_len, data_str, sizeof(data_str));
    BT_INFO_SHELL("addr: %02x:%02x:%02x:%02x:%02x:%02x, rssi: %d", addr->addr[0], addr->addr[1],  // 0，1 mac地址索引
                  addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5], rssi);  // 2，3，4，5 mac地址索引
    BT_INFO_SHELL("data len:%d data: %s", len, data_str);
}

static bool bt_mesh_prov_adv_recv_cbk(const bd_addr_t *addr, int8_t rssi, uint32_t link_id,
                                      const uint8_t *data, uint16_t len)
{
    char data_str[2 * 31 + 1] = {0}; // 2:len 1 end char, 31 print max length
    if (data == NULL || len == 0) {
        return true;
    }

    uint16_t cpy_len = len > 31 ? 31 : len; // 31 print max length
    ble_mesh_bin2hex(data, cpy_len, data_str, sizeof(data_str));
    BT_INFO_SHELL("addr: %02x:%02x:%02x:%02x:%02x:%02x, rssi: %d link id: 0x%08x",
                  addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], // 0 1 2 3 mac地址索引
                  addr->addr[4], addr->addr[5], rssi, link_id);  // 4 5 mac地址索引
    BT_INFO_SHELL("data len:%d data: %s", len, data_str);
    return true;
}

static bool bt_mesh_remote_prov_adv_recv_cbk(uint16_t src_addr, int8_t rssi, uint32_t link_id,
                                             const uint8_t *data, uint16_t len)
{
    char data_str[2 * 31 + 1] = {0}; // 2:len 1 end char, 31 print max length
    if (data == NULL || len == 0) {
        return true;
    }
    
    uint16_t cpy_len = len > 31 ? 31 : len; // 31 print max length
    ble_mesh_bin2hex(data, cpy_len, data_str, sizeof(data_str));
    BT_INFO_SHELL("addr: %04x, rssi: %d link id: 0x%08x", src_addr, rssi, link_id);
    BT_INFO_SHELL("data len:%d data: %s", len, data_str);
    return true;
}
/* AT+MESSAGE=register_access_cbk */
int cmd_cfg_register_access_cbk(int argc, const char *argv[])
{
    int ret = 0;
    if (argc != 1) {  // 1:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    ble_mesh_msg_callbacks_t cb = {0};
    cb.access_cb = bt_mesh_access_recv_cbk;
    cb.beacon_recv_cb = bt_mesh_beacon_recv_cbk;
    cb.pb_adv_recv_cb = bt_mesh_prov_adv_recv_cbk;
    cb.rmt_pb_adv_recv_cb = bt_mesh_remote_prov_adv_recv_cbk;
    ret = ble_mesh_msg_register_callbacks(&cb);
    return ret;
}

#if CONFIG_BLE_MESH_PTS_MODE
/* AT+MESSAGE=send_beacon,xxx,key_refresh,xxx */
int cmd_cfg_send_beacon(int argc, const char *argv[])
{
    int ret = 0;
    if (argc != 4) {  // 4:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[2], "key_refresh") != 0)) {  // 2:key refresh
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint8_t beacon_type = 0;
    uint8_t key_refresh;
    SHELL_PARAM_UINT(beacon_type, argv[1]);  // 1:index
    SHELL_PARAM_UINT(key_refresh, argv[3]);  // 3:index

    // beacon_type 0:secure beacon 1:priv beacon
    if (beacon_type == 0) {  // 0:secure beacon
        ret = bt_mesh_cmd_secure_beacon_send(key_refresh);
    } else {
        ret = bt_mesh_cmd_priv_beacon_send(key_refresh);
    }
    BT_INFO_SHELL("send %s beacon key_refresh:%d ",
        beacon_type == 0 ? "secure" : "priv",
        key_refresh,
        ret == 0 ? "succ" : "fail");

    return ret;
}
#endif

/* AT+MESSAGE=msg_send,src,xxx,dst,XXX,net_idx,XXX,app_idx,xxx(default: 65533),send_rel,xxx,send_ttl,xxx,data,xxx */
static int cmd_cfg_cli_msg_send(int argc, const char *argv[])
{
    ble_mesh_msg_header_t msg = {0};
    int err = 0;
    uint8_t data[256] = {0}; // 256: size

    if (argc != 15) {  // 15:size
        BT_ERR("cmd_cfg_cli_mod_app_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "src") != 0) ||            // 1:index
        (strcmp(argv[3], "dst") != 0) ||            // 3:index
        (strcmp(argv[5], "net_idx") != 0) ||        // 5:index
        (strcmp(argv[7], "app_idx") != 0) ||        // 7:index
        (strcmp(argv[9], "send_rel") != 0) ||       // 9:index
        (strcmp(argv[11], "send_ttl") != 0) ||      // 11:index
        (strcmp(argv[13], "data") != 0)) {          // 13:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(msg.src_addr, argv[2]);   // 2:index
    SHELL_PARAM_UINT(msg.dest_addr, argv[4]);  // 4:index
    SHELL_PARAM_UINT(msg.net_idx, argv[6]);    // 6:index
    SHELL_PARAM_UINT(msg.app_idx, argv[8]);    // 8:index
    SHELL_PARAM_UINT(msg.send_rel, argv[10]);  // 10:index
    SHELL_PARAM_UINT(msg.send_ttl, argv[12]);  // 12:index

    BT_INFO("src_addr: 0x%04x", msg.src_addr);
    BT_INFO("dest_addr: 0x%04x", msg.dest_addr);
    BT_INFO("net_idx: 0x%04x", msg.net_idx);
    BT_INFO("app_idx: 0x%04x", msg.app_idx);
    BT_INFO("send_rel: 0x%02x", msg.send_rel);
    BT_INFO("send_ttl: 0x%02x", msg.send_ttl);

    uint32_t hexlen = strlen(argv[14]);  // 14:argv index
    if ((hexlen / 2) > 256 || (hexlen % 2 != 0)) { // 2: divisor, 256: size
        BT_ERR("cmd data len err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint32_t len = ble_mesh_hex2bin(argv[14], data, hexlen / 2);  // 12:argv index

    err = ble_mesh_msg_send(&msg, data, len);
    if (err) {
        BT_INFO_SHELL("Unable to send msg (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return err;
}

/* AT+BMMESSAGE=beacon_send,trans_count,xxx,interval,XXX,data,xxx */
static int cmd_cfg_cli_beacon_send(int argc, const char *argv[])
{
    int err = 0;
    uint8_t data[256] = {0}; // 256: size
    uint8_t trans_count = 0;
    uint8_t interval = 0;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_mod_app_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "trans_count") != 0) ||            // 1:index
        (strcmp(argv[3], "interval") != 0) ||            // 3:index
        (strcmp(argv[5], "data") != 0)) {      // 5:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(trans_count, argv[2], 0, 7);   // 2:index 0, 7:range
    SHELL_PARAM_UINT(trans_count, argv[2]);  // 4:index
    SHELL_PARAM_UINT(interval, argv[4]);  // 4:index

    BT_INFO("trans count: 0x%02x", trans_count);
    BT_INFO("interval: 0x%02x", interval);

    uint32_t hexlen = strlen(argv[6]);  // 14:argv index
    if ((hexlen / 2) > 256 || (hexlen % 2 != 0)) { // 2: divisor, 256: size
        BT_ERR("cmd data len err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint32_t len = ble_mesh_hex2bin(argv[6], data, hexlen / 2);  // 12:argv index

    err = ble_mesh_beacon_send(trans_count, interval, data, len);
    if (err) {
        BT_INFO_SHELL("Unable to send beacon (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return err;
}

static int cmd_cfg_cli_msg_clear(int argc, const char *argv[])
{
    uint8_t count = 0;
    ble_mesh_get_msg_buff_count(&count);
    BT_INFO_SHELL("before clear msg num:%d", count);
    ble_mesh_clear_msg_buff();
    ble_mesh_get_msg_buff_count(&count);
    BT_INFO_SHELL("after clear msg num:%d", count);
    return 0;
}

static int cmd_cfg_cli_msg_num(int argc, const char *argv[])
{
    uint8_t count = 0;
    ble_mesh_get_msg_buff_count(&count);
    BT_INFO_SHELL("msg num:%d", count);
    return 0;
}

static uint32_t cmd_ble_mesh_unprov_adv_send(int argc, const char *argv[])
{
    errcode_t err = 0;
    uint8_t data[256] = {0}; // 256: size

    if (argc != 3) {  // 3:size
        BT_ERR("cmd_cfg_cli_mod_app_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "data") != 0) {      // 1:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint32_t hexlen = strlen(argv[2]);  // 2:argv index
    if ((hexlen / 2) > 256 || (hexlen % 2 != 0)) { // 2: divisor, 256: size
        BT_ERR("cmd data len err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint32_t len = ble_mesh_hex2bin(argv[2], data, hexlen / 2);  // 12:argv index
    err = ble_mesh_prov_adv_send(data, len);
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("ble_mesh_prov_adv_send failed:0x%x", err);
        return err;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static uint32_t cmd_ble_mesh_rmt_prov_pdu_report(int argc, const char *argv[])
{
    errcode_t err = 0;
    uint8_t prov_pdu[256] = {0}; // 256: size

    if (argc != 3) {  // 3:size
        BT_ERR("cmd_cfg_cli_mod_app_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "prov_pdu") != 0) {      // 1:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint32_t hexlen = strlen(argv[2]);  // 2:argv index
    if ((hexlen / 2) > 256 || (hexlen % 2 != 0)) { // 2: divisor, 256: size
        BT_ERR("cmd data len err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint32_t len = ble_mesh_hex2bin(argv[2], prov_pdu, hexlen / 2);  // 12:argv index
    err = ble_mesh_rmt_prov_pdu_report(true, prov_pdu, len);
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("ble_mesh_prov_adv_send failed:0x%x", err);
        return err;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static uint32_t cmd_ble_mesh_set_extend_segment_adv(int argc, const char *argv[])
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

    err = ble_mesh_set_extend_segment_adv(flag == 1);
    if (err) {
        BT_INFO_SHELL("set ext seg %s failed (err 0x%x)", flag == 1 ? "enable" : "disable", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static at_cmd msg_cmd_tbl[] = {
    {"register_msg_cbk", cmd_cfg_register_access_cbk},
    {"msg_send", cmd_cfg_cli_msg_send},
    {"beacon_send", cmd_cfg_cli_beacon_send},
    {"prov_adv_send", cmd_ble_mesh_unprov_adv_send},
    {"rmt_prov_pdu_report", cmd_ble_mesh_rmt_prov_pdu_report},
    {"msg_clear", cmd_cfg_cli_msg_clear},
    {"msg_num", cmd_cfg_cli_msg_num},
    {"set_ext_seg", cmd_ble_mesh_set_extend_segment_adv},
#if CONFIG_BLE_MESH_PTS_MODE
    {"secure_send_beacon", cmd_cfg_send_beacon},
#endif
};

static uint32_t cmd_ble_mesh_message_test_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    int cmd_tbl_num = (sizeof(msg_cmd_tbl) / sizeof(at_cmd));
    for (int i = 0; i < cmd_tbl_num; i++) {
        if (strcmp(argv[0], msg_cmd_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return msg_cmd_tbl[i].func(argc, argv);
        }
    }

    BT_INFO_SHELL("invalid cmd:%s", argv[0]);

    return ERRCODE_BLE_MESH_FAIL;
}

static at_ret_t at_bt_mesh_message_local(const ble_mesh_message_local_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_message_local args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
        args->para1,
        args->para2,
        args->para3,
        args->para4);
    char *argv[15] = {NULL}; // 15 para num
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

    uint32_t ret = cmd_ble_mesh_message_test_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_cfg_test_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_message_local[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = MESSAGE_DATA_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = STR_PARA_LENGTH,
        .offset = offsetof(ble_mesh_message_local_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = MESSAGE_DATA_PARA_LENGTH,
        .last = true,
        .offset = offsetof(ble_mesh_message_local_t, para15)},
};

static const at_cmd_entry_t at_bt_mesh_message_parse_table[] = {
    {
        "BMMESSAGE",  // 显示mesh 状态
        0,
        0,
        ble_mesh_message_local,                      // 参数类型
        NULL,  // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_message_local,       // 有参数时调用的函数
        NULL,
        NULL,
    },
};

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_message_parse_table) / sizeof(at_bt_mesh_message_parse_table[0]))

void mesh_at_message_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_message_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}