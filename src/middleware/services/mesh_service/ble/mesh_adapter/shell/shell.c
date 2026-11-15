/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell source
 *
 * History:
 * 2024-6-21, Create file.
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

#include "model_opcode.h"
#include "client_common.h"
#include "generic_client.h"
#include "device_property.h"
#include "mesh/cfg_cli.h"

#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_heartbeat.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_config.h"
#include "ble_mesh_manager.h"

#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"

uint8_t cmd_bt_mesh_cdb_node_print(struct bt_mesh_cdb_node *node, void *user_data);
#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
const at_para_parse_syntax_t ble_mesh_config_local[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para20)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para21)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_local_t, para22)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_config_local_t, para23)},
};
#endif

#if MYNEWT_VAL(BLE_MESH_STAT)
uint32_t cmd_ble_mesh_stat_func(int argc, const char *argv[])
{
    bt_mesh_stat_display();
    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

#if MYNEWT_VAL(BLE_MESH_IV_UPDATE_TEST)
/* AT+BMCFGLOCAL=iv_update */
uint32_t cmd_ble_mesh_iv_update_test_func(int argc, const char *argv[])
{
    bool ret = bt_mesh_iv_update();
    BT_INFO_SHELL("ret = %d", ret ? ERRCODE_BLE_MESH_SUCCESS : ERRCODE_BLE_MESH_FAIL);
    return ret ? ERRCODE_BLE_MESH_SUCCESS : ERRCODE_BLE_MESH_FAIL;
}

/* AT+BMCFGLOCAL=enable_iv_update,enable,xxx */
int cmd_ble_mesh_enable_iv_update_test_func(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "enable") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t enable;
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0x01); // 1 2: index, 0 0x01:range
    SHELL_PARAM_UINT(enable, argv[2]); // 2 : index
    bt_mesh_iv_update_test((bool)enable);
    BT_INFO_SHELL("set mesh_iv_update: %d", enable);

    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
/* AT+BM_CT=set_ttl,ttl,2 */
int cmd_cfg_set_ttl(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "ttl") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t ttl;
    SHELL_PARAM_UINT(ttl, argv[2]);  // 2 : index
    int ret = ble_mesh_set_default_ttl(ttl);
    BT_INFO_SHELL("set ttl: 0x%02x, ret: %d", ttl, ret);

    return ret;
}

/* AT+BM_CT=get_ttl */
int cmd_cfg_get_ttl(int argc, const char *argv[])
{
    uint8_t ttl = ble_mesh_get_default_ttl();
    BT_INFO_SHELL("get ttl: 0x%02x", ttl);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=set_net_xmit,count,3,interval,30 */
int cmd_cfg_set_net_xmit(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "count") != 0) || (strcmp(argv[3], "interval") != 0)) { // 1,3 : index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0x07);  // 1 2: index, 0 0x07:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 0x140);  // 3 4: index, 0 0x140:range

    uint8_t count = 0;
    uint16_t interval = 0;
    SHELL_PARAM_UINT(count, argv[2]);  // 2 : index
    SHELL_PARAM_UINT(interval, argv[4]);  // 4 : index

    errcode_t err = ble_mesh_set_net_transmit(count, interval);
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        return err;
    }
    BT_INFO_SHELL("set network count: %u", count);
    BT_INFO_SHELL("set network interval: %u", interval);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=get_net_xmit */
int cmd_cfg_get_net_xmit(int argc, const char *argv[])
{
    uint8_t count = 0;
    uint16_t interval = 0;

    ble_mesh_get_net_transmit(&count, &interval);
    BT_INFO_SHELL("get network count: %u", count);
    BT_INFO_SHELL("get network interval: %u", interval);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=set_relay,relay,1,count,4,interval,20 */
int cmd_cfg_set_relay(int argc, const char *argv[])
{
    if (argc != 7 && argc != 3) {  // 7 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "relay") != 0)) {         // 1 relay index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    errcode_t ret = ERRCODE_BLE_MESH_SUCCESS;
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0x01);  // 1 2: index, 0 0x01:range
    ble_mesh_feat_state_t relay = BLE_MESH_FEATURE_DISABLED;
    SHELL_PARAM_UINT(relay, argv[2]);  // 2 : index

    if (argc == 7) { // 7 para num
        if ((strcmp(argv[3], "count") != 0) ||     // 3 count index
            (strcmp(argv[5], "interval") != 0)) {  // 5 interval index
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 0x07);  // 3 4: index, 0 0x07:range
        SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 0x140);  // 5 6: index, 0 0x140:range
        
        uint8_t count = 0;
        uint16_t interval = 0;
        SHELL_PARAM_UINT(count, argv[4]);  // 4 : index
        SHELL_PARAM_UINT(interval, argv[6]);  // 6 : index
        ret = ble_mesh_set_relay(relay, count, interval);
        BT_INFO_SHELL("set relay feature: %u, count: %u, interval: %u ret: %d", relay, count, interval, ret);
    } else {
        ret = ble_mesh_set_relay_state(relay);
        BT_INFO_SHELL("set relay feature: %u ret: %d", relay, ret);
    }

    return ret;
}

/* AT+BM_CT=get_relay */
int cmd_cfg_get_relay(int argc, const char *argv[])
{
    ble_mesh_feat_state_t relay = ble_mesh_get_relay_state();
    BT_INFO_SHELL("get relay feature: %u", relay);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_relay_xmit */
int cmd_cfg_get_relay_xmit(int argc, const char *argv[])
{
    uint8_t count = 0;
    uint16_t interval = 0;

    errcode_t ret = ble_mesh_get_relay_transmit(&count, &interval);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("get relay failed, ret: 0x%x", ret);
        return ret;
    }
    BT_INFO_SHELL("get relay count: %u interval: %u", count, interval);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=set_proxy,proxy,0 */
int cmd_cfg_set_proxy(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "proxy") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    enum bt_mesh_feat_state proxy = atoi(argv[2]);  // 2 : index
    int ret = bt_mesh_gatt_proxy_set(proxy);
    BT_INFO_SHELL("set proxy feature: %u, ret: %d", proxy, ret);

    return ret;
}

/* AT+BM_CT=get_proxy */
int cmd_cfg_get_proxy(int argc, const char *argv[])
{
    enum bt_mesh_feat_state proxy = bt_mesh_gatt_proxy_get();
    BT_INFO_SHELL("get proxy feature: %u", proxy);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=set_priv_proxy,proxy,0 */
static int cmd_cfg_set_priv_proxy(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "proxy") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], BT_MESH_FEATURE_DISABLED, BT_MESH_FEATURE_NOT_SUPPORTED); // 1 2 index

    enum bt_mesh_feat_state proxy = atoi(argv[2]);  // 2 : index
    int ret = bt_mesh_priv_gatt_proxy_set(proxy);
    BT_INFO_SHELL("set priv_proxy feature: %u, ret: %d", proxy, ret);

    return ret;
}

/* AT+BMCFGLOCAL=get_priv_proxy */
static int cmd_cfg_get_priv_proxy(int argc, const char *argv[])
{
    enum bt_mesh_feat_state proxy = bt_mesh_priv_gatt_proxy_get();
    BT_INFO_SHELL("get priv_proxy feature: %u", proxy);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=set_node_id_state,net_idx,0,state,1 (1: enable,0: disable) */
int cmd_cfg_set_node_id_state(int argc, const char *argv[])
{
    uint16_t net_idx;
    enum bt_mesh_feat_state state;

    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "state") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], BT_MESH_FEATURE_DISABLED, BT_MESH_FEATURE_NOT_SUPPORTED); // 3 4 index
    SHELL_PARAM_UINT(state, argv[4]);    // 4:index
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    BT_INFO_SHELL("net_idx: %u, set state: %u", net_idx, state);
    uint8_t ret = bt_mesh_subnet_node_id_set(net_idx, state);
    BT_INFO_SHELL("ret: %u, node_id_state: %u", ret, state);

    return ret;
}

/* AT+BMCFGLOCAL=get_node_id_state,net_idx,0 */
int cmd_cfg_get_node_id_state(int argc, const char *argv[])
{
    uint16_t net_idx;
    enum bt_mesh_feat_state state;

    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index
    uint8_t ret = bt_mesh_subnet_node_id_get(net_idx, &state);
    BT_INFO_SHELL("net_idx: %u, node_id_state: %u, ret: %u", net_idx, state, ret);
    return ret;
}

/* AT+BMCFGLOCAL=set_priv_node_id_state,net_idx,0,state,1 (1: enable,0: disable) */
int cmd_cfg_set_priv_node_id_state(int argc, const char *argv[])
{
    uint16_t net_idx;
    enum bt_mesh_feat_state state;

    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "state") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], BT_MESH_FEATURE_DISABLED, BT_MESH_FEATURE_NOT_SUPPORTED); // 3 4:index

    SHELL_PARAM_UINT(state, argv[4]);    // 4:index
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    BT_INFO_SHELL("net_idx: %u, set state: %u", net_idx, state);
    uint8_t ret = bt_mesh_subnet_priv_node_id_set(net_idx, state);
    BT_INFO_SHELL("ret: %u, priv_node_id_state: %u", ret, state);

    return ret;
}

/* AT+BMCFGLOCAL=get_priv_node_id_state,net_idx,0 */
int cmd_cfg_get_priv_node_id_state(int argc, const char *argv[])
{
    uint16_t net_idx;
    enum bt_mesh_feat_state state;

    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index
    uint8_t ret = bt_mesh_subnet_priv_node_id_get(net_idx, &state);
    BT_INFO_SHELL("net_idx: %u, priv_node_id_state: %u, ret: %u", net_idx, state, ret);
    return ret;
}

#if MYNEWT_VAL(BLE_MESH_FRIEND)
/* AT+BM_CT=set_friend,friend,0 */
int cmd_cfg_set_friend(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "friend") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    enum bt_mesh_feat_state friend = atoi(argv[2]);  // 2 : index
    int ret = bt_mesh_friend_set(friend);
    BT_INFO_SHELL("set friend feature: %u, ret: %d", friend, ret);

    return ret;
}

/* AT+BM_CT=get_friend */
int cmd_cfg_get_friend(int argc, const char *argv[])
{
    enum bt_mesh_feat_state friend = bt_mesh_friend_get();
    BT_INFO_SHELL("get friend feature: %u", friend);

    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

/* AT+BM_CT=add_subnet,net_idx,1,key,****** */
int cmd_cfg_add_subnet(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "key") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2 : index
    uint8_t key[16] = {0};               // 16:size

    if (strlen(argv[4]) != 32) {                                // 4:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[4]));  // 4:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    const char *str = argv[4];     // 4:index
    arg_str_to_key(str, key, 16);  // 16:size

    uint8_t ret = bt_mesh_subnet_add(net_idx, key);
    BT_INFO_SHELL("add subnet net_idx: %u, ret: %u", net_idx, ret);

    return ret;
}

/* AT+BM_CT=update_subnet,net_idx,1,key,****** */
int cmd_cfg_update_subnet(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "key") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2 : index
    uint8_t key[16] = {0};               // 16:size

    if (strlen(argv[4]) != 32) {  // 4:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[4]));
        return ERRCODE_BLE_MESH_FAIL;
    }
    const char *str = argv[4];     // 4:index
    arg_str_to_key(str, key, 16);  // 16:size

    uint8_t ret = bt_mesh_subnet_update(net_idx, key);
    BT_INFO_SHELL("update subnet net_idx: %u, ret: %u", net_idx, ret);

    return ret;
}

/* AT+BM_CT=del_subnet,net_idx,1 */
int cmd_cfg_del_subnet(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    uint8_t ret = bt_mesh_subnet_del(net_idx);
    BT_INFO_SHELL("delete subnet net_idx: %u, ret: %u", net_idx, ret);

    return ret;
}

/* AT+BM_CT=subnet_exist,net_idx,1 */
int cmd_cfg_subnet_exist(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    bool ret = bt_mesh_subnet_exists(net_idx);
    BT_INFO_SHELL("subnet net_idx: %u, exist: %u", net_idx, ret);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=set_kr_phase,net_idx,1,phase,2 */
int cmd_cfg_set_kr_phase(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "phase") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t phase;
    uint16_t net_idx;
    SHELL_PARAM_UINT(phase, argv[4]);    // 4:index
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    BT_INFO_SHELL("net_idx: %u, set phase: %u", net_idx, phase);
    uint8_t ret = bt_mesh_subnet_kr_phase_set(net_idx, &phase);
    BT_INFO_SHELL("ret: %u, kr_phase: %u", ret, phase);

    return ret;
}

/* AT+BM_CT=get_kr_phase,net_idx,0 */
int cmd_cfg_get_kr_phase(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    uint8_t kr_phase = 0;
    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2:index

    uint8_t ret = bt_mesh_subnet_kr_phase_get(net_idx, &kr_phase);
    BT_INFO_SHELL("net_idx: %u, ret kr_phase: %u, ret: %u", net_idx, kr_phase, ret);

    return ret;
}

/* AT+BM_CT=set_node_id,net_idx,0,identity,1 */
int cmd_cfg_set_node_id(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0) || (strcmp(argv[3], "identity") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);                  // 2:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 2);  // 3,4:index 0,2:range
    enum bt_mesh_feat_state node_id = atoi(argv[4]);     // 4:size

    uint8_t ret = bt_mesh_subnet_node_id_set(net_idx, node_id);
    if (ret == 0) {
        BT_INFO_SHELL("set node identify: %u, net_idx: %u, ret: %u", node_id, net_idx, ret);
    } else {
        BT_INFO_SHELL("set node identify failed!: %u, net_idx: %u, ret: %u", node_id, net_idx, ret);
    }

    return ret;
}

/* AT+BM_CT=get_node_id,net_idx,0 */
int cmd_cfg_get_node_id(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "net_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t net_idx;
    SHELL_PARAM_UINT(net_idx, argv[2]);  // 2 : index
    enum bt_mesh_feat_state node_id = BT_MESH_FEATURE_ENABLED;
    uint8_t ret = bt_mesh_subnet_node_id_get(net_idx, &node_id);
    BT_INFO_SHELL("get node identify: %u, net_idx: %u, ret: %u", node_id, net_idx, ret);

    return ret;
}

/* AT+BM_CT=get_subnets,max,3,skip,0 */
int cmd_cfg_get_subnets(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "max") != 0) || (strcmp(argv[3], "skip") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t net_idxs[CONFIG_BT_MESH_SUBNET_COUNT] = {0};
    size_t max_num = atoi(argv[2]);  // 2:index
    off_t skip_num = atoi(argv[4]);  // 4:index

    ssize_t ret = bt_mesh_subnets_get(net_idxs, max_num, skip_num);
    BT_INFO_SHELL("ret size: %d", ret);
    for (int i = 0; i < ret && i < CONFIG_BT_MESH_SUBNET_COUNT; i++) {
        BT_INFO_SHELL("net_idx: %u", net_idxs[i]);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=add_app_key,app_idx,1,net_idx,0,key,****** */
int cmd_cfg_add_app_key(int argc, const char *argv[])
{
    if (argc != 7) {  // 7:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "app_idx") != 0) || (strcmp(argv[3], "net_idx") != 0) ||  // 3:index
        (strcmp(argv[5], "key") != 0)) {                                           // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t key[16] = {0};  // 16:size
    uint16_t net_idx;
    uint16_t app_idx;
    SHELL_PARAM_UINT(net_idx, argv[4]);  // 4:index
    SHELL_PARAM_UINT(app_idx, argv[2]);  // 2:index

    if (strlen(argv[6]) != 32) {  // 6:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[6]));
        return ERRCODE_BLE_MESH_FAIL;
    }
    const char *str = argv[6];     // 6:index
    arg_str_to_key(str, key, 16);  // 16:size

    errcode_t ret = ble_mesh_cfg_add_local_app_key(app_idx, net_idx, key);
    BT_INFO_SHELL("add app key, app_idx: %u, net_idx: %u, ret: %u", app_idx, net_idx, ret);

    return ret;
}

/* AT+BM_CT=update_app_key,app_idx,1,net_idx,0,key,****** */
int cmd_cfg_update_app_key(int argc, const char *argv[])
{
    if (argc != 7) {  // 7:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "app_idx") != 0) || (strcmp(argv[3], "net_idx") != 0) ||  // 3:index
        (strcmp(argv[5], "key") != 0)) {                                           // 3,5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t key[16] = {0};  // 16:size
    uint16_t net_idx;
    uint16_t app_idx;
    SHELL_PARAM_UINT(net_idx, argv[4]);  // 4:index
    SHELL_PARAM_UINT(app_idx, argv[2]);  // 2:index

    if (strlen(argv[6]) != 32) {                                // 6:index,32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[6]));  // 6:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    const char *str = argv[6];     // 6:index
    arg_str_to_key(str, key, 16);  // 16:size

    uint8_t ret = bt_mesh_app_key_update(app_idx, net_idx, key);
    BT_INFO_SHELL("update app key, app_idx: %u, net_idx: %u, ret: %u", app_idx, net_idx, ret);

    return ret;
}

/* AT+BM_CT=del_app_key,app_idx,1,net_idx,0 */
int cmd_cfg_del_app_key(int argc, const char *argv[])
{
    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "app_idx") != 0) || (strcmp(argv[3], "net_idx") != 0)) {  // 3:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t net_idx;
    uint16_t app_idx;
    SHELL_PARAM_UINT(net_idx, argv[4]);  // 4:index
    SHELL_PARAM_UINT(app_idx, argv[2]);  // 2:index

    uint8_t ret = bt_mesh_app_key_del(app_idx, net_idx);
    BT_INFO_SHELL("delete app key, app_idx: %u, net_idx: %u, ret: %u", app_idx, net_idx, ret);

    return ret;
}

/* AT+BM_CT=app_key_exist,app_idx,1 */
int cmd_cfg_app_key_exist(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "app_idx") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t app_idx;
    SHELL_PARAM_UINT(app_idx, argv[2]);  // 2:index

    bool ret = bt_mesh_app_key_exists(app_idx);
    BT_INFO_SHELL("app_idx:%d, app key exist: %d", app_idx, ret);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=get_app_keys,key_net_idx,1 */
int cmd_cfg_get_app_keys(int argc, const char *argv[])
{
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "key_net_idx") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0xfff);  // 1 2: index, 0 0xfff: range
    uint16_t key_net_idx;
    SHELL_PARAM_UINT(key_net_idx, argv[2]);  // 2 : index

    uint16_t app_idxs[CONFIG_BT_MESH_APP_KEY_COUNT];
    ssize_t ret = bt_mesh_app_keys_get(key_net_idx, app_idxs, ARRAY_SIZE(app_idxs), 0);
    if (ret < 0 || ret > ARRAY_SIZE(app_idxs)) {
        ret = ARRAY_SIZE(app_idxs);
    }
    BT_INFO_SHELL("ret size: %d", ret);
    for (int i = 0; i < ret; i++) {
        BT_INFO_SHELL("app_idxs: %u", app_idxs[i]);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_hb_pub */
int cmd_cfg_get_hb_pub(int argc, const char *argv[])
{
    struct bt_mesh_hb_pub pub = {0};

    ble_mesh_get_heartbeat_pub(&pub);

    BT_INFO_SHELL("pub.count:%u, pub.period:%u, pub.dst:%u, pub.ttl:%u, pub.feat:%u, pub.net_idx:%u",
        pub.count,
        pub.period,
        pub.dst,
        pub.ttl,
        pub.feat,
        pub.net_idx);

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_hb_sub */
int cmd_cfg_get_hb_sub(int argc, const char *argv[])
{
    struct bt_mesh_hb_sub sub = {0};

    bt_mesh_hb_sub_get(&sub);

    BT_INFO_SHELL("period:%u, remaining:%u, src:%u, dst:%u, count:%u, min_hops:%u, max_hops:%u",
        bt_mesh_hb_log(sub.period),
        bt_mesh_hb_log(sub.remaining),
        sub.src,
        sub.dst,
        sub.count,
        sub.min_hops,
        sub.max_hops);

    return ERRCODE_BLE_MESH_SUCCESS;
}

#ifndef CONFIG_BLE_MESH_PROVISIONER_ADV
/* AT+BMCFGLOCAL=node_reset */
int cmd_cfg_node_reset(int argc, const char *argv[])
{
    ble_mesh_node_reset();
    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

/* AT+BMCFGLOCAL=get_single_node */
int cmd_cfg_get_node_unicase_addr(int argc, const char *argv[])
{
    if (strcmp(argv[0], "get_single_node") != 0) {
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("The device node uincase addr: 0x%x", ble_mesh_get_primary_addr());
    return AT_RET_OK;
}
 
/* AT+BMCFGLOCAL=get_all_nodes */
int cmd_ble_mesh_prov_node_print(int argc, const char *argv[])
{
    if (strcmp(argv[0], "get_all_nodes") != 0) {
        return ERRCODE_BLE_MESH_FAIL;
    }
#if MYNEWT_VAL(BLE_MESH_CDB)
    bt_mesh_cdb_node_foreach(cmd_bt_mesh_cdb_node_print, NULL);
    return AT_RET_OK;
#endif
    return ERRCODE_BLE_MESH_FAIL;
}

/* AT+BMCFGLOCAL=mesh_net_test,enable,xxx */
int cmd_ble_mesh_net_test_func(int argc, const char *argv[])
{
#if CONFIG_BLE_MESH_PTS_MODE
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[1], "enable") != 0)) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t enable;
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0x01);  // 1 2: index, 0 0x01:range
    SHELL_PARAM_UINT(enable, argv[2]);                      // 2 : index
    atomic_set_bit_to(bt_mesh.flags, BT_MESH_NET_TEST, enable);
    BT_INFO_SHELL("set mesh net test: %d", enable);
    return ERRCODE_BLE_MESH_SUCCESS;
#endif
    return ERRCODE_BLE_MESH_FAIL;
}

static void heartbeat_recv_cbk(const ble_mesh_heartbeat_sub_t *sub, uint8_t hops, uint16_t feat)
{
    BT_INFO("Heartbeat subscribed : 0x%04x", feat);
}

/* AT+BMCFGLOCAL=register_heartbeat_cbk */
static int cmd_cfg_health_print_heartbeat(int argc, const char *argv[])
{
    ble_mesh_heartbeat_callbacks_t cb = {0};
    cb.heartbeat_recv_cb = heartbeat_recv_cbk;
    ble_mesh_heartbeat_callbacks(&cb);
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t __attribute__((weak)) ble_mesh_sample_enable(ble_mesh_prov_bearer_t bearers)
{
    BT_INFO_SHELL("not implementation func %s", __FUNCTION__); // 1 is uuid para index
    return ERRCODE_BLE_MESH_UNSUPPORTED;
}

/* AT+BMCFGCLT=ble_mesh_enable,xxx(option) */
static int cmd_ble_mesh_enable(int argc, const char *argv[])
{
    uint32_t bearers;
    if (argc == 2) {                        // 2:size
        SHELL_PARAM_INT(bearers, argv[1]);  // 1 : index
        if (bearers != BLE_MESH_PROV_ADV && bearers != BLE_MESH_PROV_GATT &&
            bearers != (BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT)) {
            BT_ERR("cmd err");
            return ERRCODE_BLE_MESH_FAIL;
        }
        return ble_mesh_sample_enable(bearers);
    }
    return ble_mesh_sample_enable(BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT);
}

/* AT+BMCFGCLT=ble_mesh_disable */
static int cmd_ble_mesh_disable(int argc, const char *argv[])
{
    errcode_t ret = ERRCODE_BLE_MESH_SUCCESS;
    ret = ble_mesh_disable();
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("Failed to disable mesh node (err %d)", ret);
        return ret;
    }

#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
    ble_mesh_provisioner_disable();
#endif

    return AT_RET_OK;
}

/* AT+BM_CT=set_hb_pub,dst,xxx,count,xxx,ttl,xxx,feat,xxx,net_id,xxx,period,xxx */
int cmd_cfg_set_hb_pub(int argc, const char *argv[])
{
    int ret;
    ble_mesh_heartbeat_pub_t pub;
    if (argc != 13) {  // 13:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "set_hb_pub") != 0 || strcmp(argv[1], "dst") != 0 ||                             // 0 1:param
        strcmp(argv[3], "count") != 0 || strcmp(argv[5], "ttl") != 0 || strcmp(argv[7], "feat") != 0 ||  // 3 5 7:param
        strcmp(argv[9], "net_idx") != 0 || strcmp(argv[11], "period") != 0) {                             // 9 11:param
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0xffff);   // 1 2:index,0 0xffff:range
    SHELL_PARAM_UINT(pub.dst, argv[2]);                        // 2:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 0xffff);   // 3 4:index,0 0xffff:range
    SHELL_PARAM_UINT(pub.count, argv[4]);                      // 4:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 0xff);     // 5 6:index,0 0xff:range
    SHELL_PARAM_UINT(pub.ttl, argv[6]);                        // 6:index
    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0xff);     // 7 8:index,0 0xff:range
    SHELL_PARAM_UINT(pub.feature, argv[8]);                    // 8:index
    SHELL_PARAM_UINT(pub.net_idx, argv[10]);                   // 10:index
    SHELL_PARAM_UINT(pub.period, argv[12]);                    // 12:index
    ret = ble_mesh_set_heartbeat_pub(&pub);
    BT_INFO_SHELL("hb pub set %s", ret == ERRCODE_BLE_MESH_SUCCESS ? "succ" : "fail");
    return ret;
}

/* AT+BM_CT=start_hb */
int cmd_cfg_start_hb(int argc, const char *argv[])
{
    uint16_t addr;
    if (argc != 1) {  // 1:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    ble_mesh_start_heartbeat();
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CT=get_mod_sub_list_count,mod_id,xxx */
int cmd_cfg_get_mod_sub_list_count(int argc, const char *argv[])
{
    int ret;
    uint32_t mod_id = 0;
    uint16_t count = 0;
    if (argc != 3) {  // 3:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (strcmp(argv[0], "get_mod_sub_list_count") != 0 || strcmp(argv[1], "mod_id") != 0) {  // 0 1:param
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(mod_id, argv[2]);                        // 2:index
    ret = ble_mesh_get_mod_sub_list_count(mod_id, &count);
    BT_INFO("get mod %d sub list count:%d %s", mod_id, count, ret == ERRCODE_BLE_MESH_SUCCESS ? "success" : "fail");
    return ret;
}

/* AT+BM_CT=get_mod_sub_list,mod_id,xxx */
int cmd_cfg_get_mod_sub_list(int argc, const char *argv[])
{
    int ret;
    uint32_t mod_id = 0;
    uint16_t count = CONFIG_BT_MESH_MODEL_GROUP_COUNT;
    uint16_t *list = NULL;
    if (argc != 3) {  // 3:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[0], "get_mod_sub_list") != 0 || strcmp(argv[1], "mod_id") != 0) {  // 0 1:param
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0, 0xffff);  // 2:index,0 0xffff:range
    SHELL_PARAM_UINT(mod_id, argv[2]);                       // 2:index
    ret = ble_mesh_get_mod_sub_list_count(mod_id, &count);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("get mod %d sub count failed:0x%x", mod_id, ret);
    }
    if (count == 0) {
        BT_INFO_SHELL("get mod %d sub list num:%d", mod_id, count);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    list = osal_kmalloc(sizeof(uint16_t) * count, 0);
    if (list == NULL) {
        BT_INFO_SHELL("get mod %d sub list malloc failed", mod_id);
        return ERRCODE_BLE_MESH_FAIL;
    }
    ret = ble_mesh_get_mod_sub_list(mod_id, list, &count);
    BT_INFO_SHELL("get mod %d sub list %s,num:%d", mod_id, ret == ERRCODE_BLE_MESH_SUCCESS ? "success" : "fail", count);
    for (uint16_t i = 0; i < count; i++) {
        BT_INFO_SHELL("sub list[%d] : 0x%x", i, list[i]);
    }
    osal_kfree(list);
    return ret;
}

/* AT+BMCFGLOCAL=update_dev_key,dev_key,xxx,active,xxx */
int cmd_cfg_update_dev_key(int argc, const char *argv[])
{
    errcode_t err;
    uint8_t dev_key[BLE_MESH_DEV_KEY_LEN] = {0};  // 16:size
    uint8_t active = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "dev_key") != 0 || strcmp(argv[3], "active") != 0) {  // 1 3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strlen(argv[2]) != 32) {                                    // 2:index;32:size
        BT_INFO_SHELL("invaild net key len: %u", strlen(argv[2]));  // 4:index
        return ERRCODE_BLE_MESH_FAIL;
    }
    arg_str_to_key(argv[2], dev_key, BLE_MESH_DEV_KEY_LEN);                // 2:index 16:size
    SHELL_PARAM_RANGE_CHECK_IN(argv[3], argv[4], 0, 1);  // 3,4:index, 0,1:range
    SHELL_PARAM_UINT(active, argv[4]);                   // 4:index

    if (active) {
        err = ble_mesh_cfg_update_dev_key(dev_key, true);
    } else {
        err = ble_mesh_cfg_update_dev_key(dev_key, false);
    }

    if (err) {
        BT_INFO_SHELL("update dev key fail");
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=add_mod_sub_by_model_id,sub_addr,xxx,model_id,xxx */
int cmd_cfg_add_mod_sub_by_model_id(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint32_t model_id = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "model_id") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(model_id, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_add_mod_sub_by_model_id(sub_addr, model_id);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("add mod sub by model id fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, model id = 0x%x", sub_addr, model_id);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=add_all_mod_sub,sub_addr,xxx,elem_addr,xxx */
int cmd_cfg_add_all_mod_sub(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint16_t elem_addr = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "elem_addr") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(elem_addr, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_add_mod_sub_by_elem(sub_addr, elem_addr);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("add all mod sub fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, elem addr = 0x%x", sub_addr, elem_addr);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=del_mod_sub_by_model_id,sub_addr,xxx,model_id,xxx */
int cmd_cfg_del_mod_sub_by_model_id(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint32_t model_id = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "model_id") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(model_id, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_del_mod_sub_by_model_id(sub_addr, model_id);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("del mod sub by model id fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, model id = 0x%x", sub_addr, model_id);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=del_all_mod_sub,sub_addr,xxx,elem_addr,xxx */
int cmd_cfg_del_all_mod_sub(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint16_t elem_addr = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "elem_addr") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(elem_addr, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_del_mod_sub_by_elem(sub_addr, elem_addr);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("del all mod sub fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, elem_addr = 0x%x", sub_addr, elem_addr);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=overwrite_mod_sub_by_model_id,sub_addr,xxx,model_id,xxx */
int cmd_cfg_overwrite_mod_sub_by_model_id(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint32_t model_id = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "model_id") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(model_id, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_overwrite_mod_sub_by_model_id(sub_addr, model_id);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("overwrite mod sub by model id fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, model id = 0x%x", sub_addr, model_id);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGLOCAL=overwrite_all_mod_sub,sub_addr,xxx,elem_addr,xxx */
int cmd_cfg_overwrite_all_mod_sub(int argc, const char *argv[])
{
    uint16_t sub_addr = 0;
    uint16_t elem_addr = 0;

    if (argc != 5) {  // 5:size
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "sub_addr") != 0 || strcmp(argv[3], "elem_addr") != 0) {  // 1,3:index
        BT_INFO_SHELL("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(sub_addr, argv[2]);  // 2:index
    SHELL_PARAM_UINT(elem_addr, argv[4]);  // 4:index

    errcode_t ret = ble_mesh_overwrite_mod_sub_by_elem(sub_addr, elem_addr);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO_SHELL("overwrite all mod sub fail, ret = %d", ret);
        BT_INFO_SHELL("sub addr = 0x%x, elem addr = 0x%x", sub_addr, elem_addr);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static at_cmd cfg_cmd_tbl[] = {
    {"set_beacon", cmd_cfg_set_beacon},
    {"get_beacon", cmd_cfg_get_beacon},
    {"set_priv_beacon", cmd_cfg_set_priv_beacon},
    {"get_priv_beacon", cmd_cfg_get_priv_beacon},
    {"set_priv_beacon_interval", cmd_cfg_set_priv_beacon_interval},
    {"get_priv_beacon_interval", cmd_cfg_get_priv_beacon_interval},
    {"set_ttl", cmd_cfg_set_ttl},
    {"get_ttl", cmd_cfg_get_ttl},
    {"set_net_xmit", cmd_cfg_set_net_xmit},
    {"get_net_xmit", cmd_cfg_get_net_xmit},
    {"set_relay", cmd_cfg_set_relay},
    {"get_relay", cmd_cfg_get_relay},
    {"get_relay_xmit", cmd_cfg_get_relay_xmit},
    {"set_proxy", cmd_cfg_set_proxy},
    {"get_proxy", cmd_cfg_get_proxy},
    {"set_priv_proxy", cmd_cfg_set_priv_proxy},
    {"get_priv_proxy", cmd_cfg_get_priv_proxy},
    {"set_node_id_state", cmd_cfg_set_node_id_state},
    {"get_node_id_state", cmd_cfg_get_node_id_state},
    {"set_priv_node_id_state", cmd_cfg_set_priv_node_id_state},
    {"get_priv_node_id_state", cmd_cfg_get_priv_node_id_state},
#if MYNEWT_VAL(BLE_MESH_FRIEND)
    {"set_friend", cmd_cfg_set_friend},
    {"get_friend", cmd_cfg_get_friend},
#endif
    {"add_subnet", cmd_cfg_add_subnet},
    {"update_subnet", cmd_cfg_update_subnet},
    {"del_subnet", cmd_cfg_del_subnet},
    {"subnet_exist", cmd_cfg_subnet_exist},
    {"set_kr_phase", cmd_cfg_set_kr_phase},
    {"get_kr_phase", cmd_cfg_get_kr_phase},
    {"set_node_id", cmd_cfg_set_node_id},
    {"get_node_id", cmd_cfg_get_node_id},
    {"get_subnets", cmd_cfg_get_subnets},
    {"add_app_key", cmd_cfg_add_app_key},
    {"update_app_key", cmd_cfg_update_app_key},
    {"del_app_key", cmd_cfg_del_app_key},
    {"app_key_exist", cmd_cfg_app_key_exist},
    {"get_app_keys", cmd_cfg_get_app_keys},
    {"get_hb_pub", cmd_cfg_get_hb_pub},
    {"get_hb_sub", cmd_cfg_get_hb_sub},
#ifndef CONFIG_BLE_MESH_PROVISIONER_ADV
    {"node_reset", cmd_cfg_node_reset},
#endif
#if MYNEWT_VAL(BLE_MESH_IV_UPDATE_TEST)
    {"iv_update", cmd_ble_mesh_iv_update_test_func},
    {"enable_iv_update", cmd_ble_mesh_enable_iv_update_test_func},
#endif
    {"get_all_nodes", cmd_ble_mesh_prov_node_print},
    {"get_single_node", cmd_cfg_get_node_unicase_addr},
    {"register_heartbeat_cbk", cmd_cfg_health_print_heartbeat},
    {"ble_mesh_enable", cmd_ble_mesh_enable},
    {"ble_mesh_disable", cmd_ble_mesh_disable},
#if CONFIG_BLE_MESH_PTS_MODE
    {"mesh_net_test", cmd_ble_mesh_net_test_func},
#endif
    {"set_hb_pub", cmd_cfg_set_hb_pub},
    {"start_hb", cmd_cfg_start_hb},
    {"get_mod_sub_list_count", cmd_cfg_get_mod_sub_list_count},
    {"get_mod_sub_list", cmd_cfg_get_mod_sub_list},
    {"update_dev_key", cmd_cfg_update_dev_key},
    {"add_mod_sub_by_model_id", cmd_cfg_add_mod_sub_by_model_id},
    {"add_all_mod_sub", cmd_cfg_add_all_mod_sub},
    {"del_mod_sub_by_model_id", cmd_cfg_del_mod_sub_by_model_id},
    {"del_all_mod_sub", cmd_cfg_del_all_mod_sub},
    {"overwrite_mod_sub_by_model_id", cmd_cfg_overwrite_mod_sub_by_model_id},
    {"overwrite_all_mod_sub", cmd_cfg_overwrite_all_mod_sub},
};

uint32_t cmd_ble_mesh_cfg_test_func(int argc, const char *argv[])
{
    uint32_t ret = ERRCODE_BLE_MESH_SUCCESS;

    int cfg_cmd_tbl_num = (sizeof(cfg_cmd_tbl) / sizeof(at_cmd));
    for (int i = 0; i < cfg_cmd_tbl_num; i++) {
        if (strcmp(argv[0], cfg_cmd_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return cfg_cmd_tbl[i].func(argc, argv);
        }
    }

    BT_INFO_SHELL("invalid cmd:%s", argv[0]);

    return ERRCODE_BLE_MESH_FAIL;
}
#endif /* MYNEWT_VAL(BLE_MESH_CFG_TEST) */

#if MYNEWT_VAL(BLE_MESH_STAT)
at_ret_t at_bt_mesh_display_state(void)
{
    char *argv = {0};
    BT_DBG("[debug] at_bt_mesh_display_state enter\r\n");
    cmd_ble_mesh_stat_func(0, (const char **)(&argv));
    return AT_RET_OK;
}
#endif

#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
// 配置本端，AT+BM_CT ---- 参数类型如何构建待确定
at_ret_t at_bt_mesh_config_local(const ble_mesh_config_local_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_config_local args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
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
    ASSIGN_PARA(argv, args->para18, argc);
    ASSIGN_PARA(argv, args->para19, argc);
    ASSIGN_PARA(argv, args->para20, argc);
    ASSIGN_PARA(argv, args->para21, argc);
    ASSIGN_PARA(argv, args->para22, argc);
    ASSIGN_PARA(argv, args->para23, argc);

    uint32_t ret = cmd_ble_mesh_cfg_test_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("cmd_ble_mesh_cfg_test_func error ret = %u \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}
#endif

const at_cmd_entry_t at_bt_mesh_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_STAT)
    {
        "BMSTAT",  // 显示mesh 状态
        0,
        0,
        NULL,                      // 参数类型
        at_bt_mesh_display_state,  // 无参数时调用的函数
        (at_set_func_t)NULL,       // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
    {
        "BMCFGLOCAL",  // 配置本端 AT+BM_CT
        0,
        0,
        ble_mesh_config_local,                   // 参数类型
        NULL,                                    // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_config_local,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#if MYNEWT_VAL(BLE_MESH_CDB)
uint8_t cmd_bt_mesh_cdb_node_print(struct bt_mesh_cdb_node *node, void *user_data)
{
    BT_INFO_SHELL(
        "node addr: 0x%04x net_idx: %d uuid: %s", node->addr, node->net_idx, bt_hex(node->uuid, sizeof(node->uuid)));
    return BT_MESH_CDB_ITER_CONTINUE;
}
#endif

#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_parse_table) / sizeof(at_bt_mesh_parse_table[0]))

static void mesh_at_common_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}

void mesh_at_register(void)
{
    BT_INFO_SHELL("enter");
    mesh_at_common_register();
    mesh_at_cfg_register();
    mesh_at_generics_register();
    mesh_at_lighting_register();
    mesh_at_prov_register();
    mesh_at_time_scenes_register();
    mesh_at_health_register();
    mesh_at_vnd_register();
    mesh_at_priv_beacon_register();
    mesh_at_ota_register();
    mesh_at_rmt_prov_register();
    mesh_at_message_register();
}
