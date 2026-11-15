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
#include "ble_mesh_message.h"
#include "ble_mesh_errcode.h"
#include "model_opcode.h"
#include "client_common.h"
#include "device_property.h"
#if (defined CONFIG_SAMPLE_SUPPORT_BLE_MESH_PROV) || (defined CONFIG_SAMPLE_SUPPORT_BLE_MESH_DEVICE)
#include "ble_mesh_sample.h"
#endif
#include "mesh/cfg_cli.h"
#include "at.h"
#include "shell_cmd.h"
#include "shell_utils.h"
#include "shell.h"
#include "ble_mesh_common.h"
#include "ble_mesh_sm.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_message.h"
#include "ble_mesh_model.h"

#if MYNEWT_VAL(BLE_MESH_CONSOLE_CFG_CLIENT)
#define COMP_DATA_MAX_LEN 32
#define COMP_DATA_MIN_LEN 4
#define SIG_MODEL_DATA_LEN 2
#define VENDOR_MODEL_DATA_LEN 4

int cmd_cfg_get_addr_and_net_index(const char *argv[], uint16_t *dst, uint16_t *net_index)
{
    if ((strcmp(argv[1], "addr") != 0) || (strcmp(argv[3], "net_index") != 0)) {  // 3:index
        BT_ERR("cmd err %s %s", argv[1], argv[3]);                                // 3:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(*dst, argv[2]);        // 2:index
    SHELL_PARAM_UINT(*net_index, argv[4]);  // 4:index

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGCLT=node_identity_set,addr,XXX,net_index,XXX,key_net_idx,XXX,identity,XXX */
int cmd_cfg_cli_node_identity_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t new_identity, identify_status;

    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "identity") != 0)) {  // 5,7:size
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0xFF);  // 7,8:index 0,0xFF:range

    SHELL_PARAM_UINT(key_net_idx, argv[6]);   // 6:index
    SHELL_PARAM_UINT(new_identity, argv[8]);  // 8:index

    err = bt_mesh_cfg_node_identity_set(net_idx, dst, key_net_idx, new_identity, &status, &identify_status);
    if (err) {
        BT_INFO_SHELL("Unable to send identity set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status || (new_identity != identify_status)) {
        BT_INFO_SHELL("identity set failed with status 0x%02x, identify: 0x%02x", status, identify_status);
    } else {
        BT_INFO_SHELL("identity set with NetKey Index 0x%03x, identify: 0x%02x", key_net_idx, identify_status);
    }

    return status;
}

/* AT+BMCFGCLT=node_identity_get,addr,XXX,net_index,XXX,key_net_idx,XXX */
int cmd_cfg_cli_node_identity_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t identify_status;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_app_key_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    err = bt_mesh_cfg_node_identity_get(net_idx, dst, key_net_idx, &status, &identify_status);
    if (err) {
        BT_INFO_SHELL("Unable to send node identity get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status != 0) {
        BT_INFO_SHELL("node identity get failed with status 0x%02x", status);
        return status;
    }

    BT_INFO_SHELL("node identity state of NetKey Index 0x%03x: 0x%02x", key_net_idx, identify_status);
    return status;
}

/* AT+BMCFGCLT=node_reset,addr,XXX,net_index,XXX */
int cmd_cfg_cli_node_reset(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    bool reset = false;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_node_reset:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_node_reset(net_idx, dst, &reset);
    if (err) {
        BT_INFO_SHELL("Unable to send Remote Node Reset (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
        struct bt_mesh_cdb_node *node = bt_mesh_cdb_node_get(dst);
        if (node) {
            bt_mesh_cdb_node_del(node, true);
        }
    }
    BT_INFO_SHELL("remote node reset complete status %d", reset);

    return err;
}

static void cmd_cfg_cli_comp_data_print(ble_mesh_net_buff_t *comp)
{
    BT_INFO_SHELL("\tCID      0x%04x", ble_mesh_net_buf_pull_le16(comp));
    BT_INFO_SHELL("\tPID      0x%04x", ble_mesh_net_buf_pull_le16(comp));
    BT_INFO_SHELL("\tVID      0x%04x", ble_mesh_net_buf_pull_le16(comp));
    BT_INFO_SHELL("\tCRPL     0x%04x", ble_mesh_net_buf_pull_le16(comp));
    BT_INFO_SHELL("\tFeatures 0x%04x", ble_mesh_net_buf_pull_le16(comp));
    while (ble_mesh_net_buf_left_len(comp) > COMP_DATA_MIN_LEN) {
        uint8_t sig, vnd;
        uint16_t loc;
        int i;
        loc = ble_mesh_net_buf_pull_le16(comp);
        sig = ble_mesh_net_buf_pull_u8(comp);
        vnd = ble_mesh_net_buf_pull_u8(comp);
        BT_INFO_SHELL("\tElement @ %d:", loc);
        if (ble_mesh_net_buf_left_len(comp) < ((sig * SIG_MODEL_DATA_LEN) + (vnd * VENDOR_MODEL_DATA_LEN))) {
            BT_INFO_SHELL("\t\t...truncated data!");
            break;
        }
        if (sig) {
            BT_INFO_SHELL("\t\tSIG Models:");
        } else {
            BT_INFO_SHELL("\t\tNo SIG Models");
        }
        for (i = 0; i < sig; i++) {
            uint16_t mod_id = ble_mesh_net_buf_pull_le16(comp);
            BT_INFO_SHELL("\t\t\t%d", mod_id);
        }
        if (vnd) {
            BT_INFO_SHELL("\t\tVendor Models:");
        } else {
            BT_INFO_SHELL("\t\tNo Vendor Models");
        }
        for (i = 0; i < vnd; i++) {
            uint16_t cid = ble_mesh_net_buf_pull_le16(comp);
            uint16_t mod_id = ble_mesh_net_buf_pull_le16(comp);

            BT_INFO_SHELL("\t\t\tCompany %d: %d", cid, mod_id);
        }
    }
}

/* AT+BMCFGCLT=comp_data_get,addr,XXX,net_index,XXX,page,XXX */
static int cmd_cfg_cli_comp_data_get(int argc, const char *argv[])
{
    uint8_t status, page = 0;
    int err = 0;
    uint16_t dst;
    uint16_t net_idx;
    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_comp_data_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "page") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
    ble_mesh_prov_net_info_t net_info = {0};
    net_info.net_idx = net_idx;
    net_info.addr = dst;

    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 0xFF);  // 5,6:index 0,0xFF:range
    SHELL_PARAM_UINT(page, argv[6]);                        // 6:index
    BT_INFO("dst:%d net_idx:%d page: 0x%02x", dst, net_idx, page);
    err = ble_mesh_mod_get_composition(&net_info, cmd_cfg_cli_comp_data_print, &status);
    if (err) {
        BT_INFO_SHELL("Getting composition failed (err %d)", err);
        return err;
    }
    if (status != 0x00) {
        BT_INFO_SHELL("Got non-success status %d", status);
        return err;
    }
    BT_INFO_SHELL("Got Composition Data for %d:", dst);
    return err;
}


static int cmd_cfg_cli_comp_vid_set(int argc, const char *argv[])
{
    bool ret = true;
    uint16_t vid = 0;

    if (argc != 2) {  // 2:size
        BT_ERR("cmd_cfg_cli_comp_vid_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    if (strlen(argv[1]) > sizeof(vid) * 2 || strlen(argv[1]) % 2 != 0) { // 1:index, 2: 2 hex chars
        BT_ERR("cmd_cfg_cli_comp_vid_set:para err %s", argv[1]);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    int32_t vid_len = ble_mesh_hex2bin(argv[1], (uint8_t *)&vid, sizeof(vid)); // 1:index
    if (vid_len < 0) {
        BT_ERR("cmd_cfg_cli_comp_vid_set:para must hex str: %s", argv[1]);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    vid =  (vid << 8) | (vid >> 8); // 8 bits
    BT_INFO("vid: 0x%04x", vid);
    ble_mesh_mod_update_vid(vid);
    
    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CC=beacon_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_beacon_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_beacon_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_beacon_get(net_idx, dst, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Beacon Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Beacon state is 0x%02x", status);

    return err;
}

/* AT+BMCFGCLT=gatt_proxy_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_gatt_proxy_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;

    if (argc != 5) {  // 5:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_gatt_proxy_get(net_idx, dst, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send gatt proxy Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("gatt proxy state is 0x%02x", status);

    return err;
}

/* AT+BMCFGCLT=gatt_proxy_set,addr,XXX,net_index,XXX,value,XXX */
int cmd_cfg_cli_gatt_proxy_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    uint8_t val;

    if (argc != 7) {  // 7:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 1);  // 5,6:index 0,1:range

    SHELL_PARAM_UINT(val, argv[6]);  // 6:index

    err = bt_mesh_cfg_gatt_proxy_set(net_idx, dst, val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send gatt proxy Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("gatt proxy state is 0x%02x", status);

    return err;
}

/* AT+BMCFGCLT=mod_sub_del_all,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX */
int cmd_cfg_cli_mod_sub_del_all(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id;
    uint8_t status;

    if (argc != 9) {  // 9:index
        BT_ERR("cmd_cfg_cli_mod_sub_del:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    err = bt_mesh_cfg_mod_sub_del_all(net_idx, dst, elem_addr, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Delete(All) (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Delete(All) failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription deltion(All) was successful");
    }

    return status;
}

/* AT+BM_CC=beacon_set,addr,XXX,net_index,XXX,value,XXX */
int cmd_cfg_cli_beacon_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    uint8_t val;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_beacon_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(val, argv[6]);  // 6:index

    err = bt_mesh_cfg_beacon_set(net_idx, dst, val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Beacon Get/Set message (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Beacon state is 0x%02x", status);

    return err;
}

/* AT+BM_CC=ttl_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_ttl_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t ttl;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_ttl_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_ttl_get(net_idx, dst, &ttl);
    if (err) {
        BT_INFO_SHELL("Unable to send Default TTL Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("TTL is 0x%02x", ttl);

    return err;
}

/* AT+BM_CC=ttl_set,addr,XXX,net_index,XXX,value,XXX */
int cmd_cfg_cli_ttl_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t ttl;
    uint8_t val;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_ttl_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(val, argv[6]);  // 6:index
    err = bt_mesh_cfg_ttl_set(net_idx, dst, val, &ttl);
    if (err) {
        BT_INFO_SHELL("Unable to send Default TTL Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("TTL is 0x%02x", ttl);

    return err;
}

/* AT+BM_CC=friend_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_friend_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t frnd;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_friend_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_friend_get(net_idx, dst, &frnd);
    if (err) {
        BT_INFO_SHELL("Unable to send Friend Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Friend is set to 0x%02x", frnd);

    return err;
}

/* AT+BM_CC=friend_set,addr,XXX,net_index,XXX,value,XXX */
int cmd_cfg_cli_friend_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t frnd;
    uint8_t val;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_friend_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "value") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 1);  // 5,6:index 0,1:range

    SHELL_PARAM_UINT(val, argv[6]);  // 6:index

    err = bt_mesh_cfg_friend_set(net_idx, dst, val, &frnd);
    if (err) {
        BT_INFO_SHELL("Unable to send Friend Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Friend is set to 0x%02x", frnd);

    return err;
}

/* AT+BM_CC=net_transmit_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_net_transmit_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t transmit;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_net_transmit_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_net_transmit_get(net_idx, dst, &transmit);
    if (err) {
        BT_INFO_SHELL("Unable to send network transmit Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Transmit 0x%02x (count %u interval %ums)",
        transmit,
        BT_MESH_TRANSMIT_COUNT(transmit),
        BT_MESH_TRANSMIT_INT(transmit));

    return err;
}

/* AT+BM_CC=net_transmit_set,addr,XXX,net_index,XXX,count,XXX,interval,XXX */
int cmd_cfg_cli_net_transmit_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t transmit, count;
    uint8_t new_transmit;
    uint16_t interval;
 
    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_net_transmit_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    if (strcmp(argv[5], "count") != 0 || strcmp(argv[7], "interval") != 0) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 7);  // 5,6:index, 0,7:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0x140);  // 7 8: index, 0 0x140: range
 
    SHELL_PARAM_UINT(count, argv[6]);     // 6:index
    SHELL_PARAM_UINT(interval, argv[8]);  // 8:index
 
    new_transmit = BT_MESH_TRANSMIT(count, interval);
    err = bt_mesh_cfg_net_transmit_set(net_idx, dst, new_transmit, &transmit);
    if (err) {
        BT_INFO_SHELL("Unable to send network transmit Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    BT_INFO_SHELL("new_transmit %u transmit %u", new_transmit, transmit);
 
    BT_INFO_SHELL("Transmit 0x%02x (count %u interval %ums)",
        transmit,
        BT_MESH_TRANSMIT_COUNT(transmit),
        BT_MESH_TRANSMIT_INT(transmit));
 
    return err;
}

/* AT+BM_CC=relay_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_relay_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t relay, transmit;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_relay_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_relay_get(net_idx, dst, &relay, &transmit);
    if (err) {
        BT_INFO_SHELL("Unable to send Relay Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Relay is 0x%02x, Transmit 0x%02x (count %u interval %ums)",
        relay,
        transmit,
        BT_MESH_TRANSMIT_COUNT(transmit),
        BT_MESH_TRANSMIT_INT(transmit));

    return err;
}

/* AT+BM_CC=relay_set,addr,XXX,net_index,XXX,value,XXX,count,XXX,interval,XXX */
int cmd_cfg_cli_relay_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t relay, count;
    uint8_t new_transmit, val, transmit;
    uint16_t interval;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_relay_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "value") != 0) || (strcmp(argv[7], "count") != 0) ||  // 5,7:index
        (strcmp(argv[9], "interval") != 0)) { // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 1);  // 5 6: index, 0 1: range
    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 7);  // 7 8: index, 0 1: range
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0x140);  // 9 10: index, 0 0x140: range
 
    SHELL_PARAM_UINT(val, argv[6]);        // 6:index
    SHELL_PARAM_UINT(count, argv[8]);      // 8:index
    SHELL_PARAM_UINT(interval, argv[10]);  // 10:index

    new_transmit = BT_MESH_TRANSMIT(count, interval);
    err = bt_mesh_cfg_relay_set(net_idx, dst, val, new_transmit, &relay, &transmit);
    if (err) {
        BT_INFO_SHELL("Unable to send Relay Get/Set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Relay is 0x%02x, Transmit 0x%02x (count %u interval %ums)",
        relay,
        transmit,
        BT_MESH_TRANSMIT_COUNT(transmit),
        BT_MESH_TRANSMIT_INT(transmit));

    return err;
}

/* AT+BM_CC=net_key_add,addr,XXX,net_index,XXX,key_net_idx,XXX,key,XXX */
int cmd_cfg_cli_net_key_add(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t key_val[16];  // 16:size

    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_net_key_add:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "key") != 0)) {  // 5,7:size
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    if (strlen(argv[8]) != 32) {                                // 8:index,32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[6]));  // 6:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[8];         // 8:index
    arg_str_to_key(str, key_val, 16);  // 16:size

    err = bt_mesh_cfg_net_key_add(net_idx, dst, key_net_idx, key_val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send NetKey Add (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("NetKeyAdd failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("NetKey added with NetKey Index 0x%03x", key_net_idx);
    }

    return status;
}

/* AT+BM_CC=net_key_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_net_key_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    size_t key_cnt;
    uint16_t key_val[16];  // 16:size

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_net_key_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    key_cnt = ARRAY_SIZE(key_val);
    err = bt_mesh_cfg_net_key_get(net_idx, dst, key_val, &key_cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send NetKey Get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("NetKeys known by 0x%04x:", dst);

    for (int i = 0; i < key_cnt; i++) {
        BT_INFO_SHELL("\t0x%03x", key_val[i]);
    }

    return err;
}

/* AT+BMCFGCLT=net_key_update,addr,XXX,net_index,XXX,key_net_idx,XXX,key,XXX */
int cmd_cfg_cli_net_key_update(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t key_val[16];  // 16:size

    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "key") != 0)) {  // 5,7:size
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    if (strlen(argv[8]) != 32) {                                // 8:index,32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[8]));  // 6:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[8];         // 8:index
    arg_str_to_key(str, key_val, 16);  // 16:size

    err = bt_mesh_cfg_net_key_update(net_idx, dst, key_net_idx, key_val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send NetKey Update (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("NetKeyUpdate failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("NetKey Update with NetKey Index 0x%03x", key_net_idx);
    }

    return status;
}

/* AT+BM_CC=net_key_del,addr,XXX,net_index,XXX,key_net_idx,XXX */
int cmd_cfg_cli_net_key_del(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_net_key_del:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    err = bt_mesh_cfg_net_key_del(net_idx, dst, key_net_idx, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send NetKeyDel (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("NetKeyDel failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("NetKey 0x%03x deleted", key_net_idx);
    }

    return status;
}

/* AT+BM_CC=app_key_add,addr,XXX,net_index,XXX,key_net_idx,XXX,key_app_idx,XXX,key,XXX */
int cmd_cfg_cli_app_key_add(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx, key_app_idx;
    uint8_t status;
    uint8_t key_val[16];

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_app_key_add:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "key_app_idx") != 0) ||  // 5,7:index
        (strcmp(argv[9], "key") != 0)) {                                                   // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index
    SHELL_PARAM_UINT(key_app_idx, argv[8]);  // 8:index

    if (strlen(argv[10]) != 32) {                                // 10:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];        // 10:index
    arg_str_to_key(str, key_val, 16);  // 16:size

    err = bt_mesh_cfg_app_key_add(net_idx, dst, key_net_idx, key_app_idx, key_val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send App Key Add (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("AppKeyAdd failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
    }

    return status;
}

/* AT+BM_CC=app_key_get,addr,XXX,net_index,XXX,key_net_idx,XXX */
int cmd_cfg_cli_app_key_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint16_t key_val[16];  // 16:size
    size_t cnt = 16;       // 16:size

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_app_key_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    cnt = ARRAY_SIZE(key_val);

    err = bt_mesh_cfg_app_key_get(net_idx, dst, key_net_idx, &status, key_val, &cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send AppKeyGet (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("AppKeyGet failed with status 0x%02x", status);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    BT_INFO_SHELL("AppKeys for NetKey 0x%03x known by 0x%04x:", net_idx, dst);
    for (int i = 0; i < cnt; i++) {
        BT_INFO_SHELL("\t0x%04x", key_val[i]);
    }

    return status;
}

/* AT+BMCFGCLT=app_key_update,addr,XXX,net_index,XXX,key_net_idx,XXX,key_app_idx,XXX,key,XXX */
int cmd_cfg_cli_app_key_update(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx, key_app_idx;
    uint8_t status;
    uint8_t key_val[16];

    if (argc != 11) {  // 11:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "key_app_idx") != 0) ||  // 5,7:index
        (strcmp(argv[9], "key") != 0)) {                                                   // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index
    SHELL_PARAM_UINT(key_app_idx, argv[8]);  // 8:index

    if (strlen(argv[10]) != 32) {                                // 10:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];        // 10:index
    arg_str_to_key(str, key_val, 16);  // 16:size

    err = bt_mesh_cfg_app_key_update(net_idx, dst, key_net_idx, key_app_idx, key_val, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send App Key update (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("AppKey update failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey updated, NetKeyIndex 0x%04x AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
    }

    return status;
}

/* AT+BM_CC=app_key_del,addr,XXX,net_index,XXX,key_net_idx,XXX,key_app_idx,XXX */
int cmd_cfg_cli_app_key_del(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx, key_app_idx;
    uint8_t status;

    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_app_key_del:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "key_app_idx") != 0)) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index
    SHELL_PARAM_UINT(key_app_idx, argv[8]);  // 8:index

    err = bt_mesh_cfg_app_key_del(net_idx, dst, key_net_idx, key_app_idx, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send App Key del(err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("AppKeyDel failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey deleted, NetKeyIndex 0x%04x AppKeyIndex 0x%04x", key_net_idx, key_app_idx);
    }

    return status;
}

/* AT+BM_CC=mod_app_bind,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,mod_app_idx,XXX */
int cmd_cfg_cli_mod_app_bind(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_app_idx, mod_id;
    uint8_t status;
    ble_mesh_prov_net_info_t net_info = {0};
    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_app_bind:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "mod_app_idx") != 0)) {                                    // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);     // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);        // 8:index
    SHELL_PARAM_UINT(mod_app_idx, argv[10]);  // 10:index

    net_info.net_idx = net_idx;
    net_info.addr = dst;
    net_info.elem_addr = elem_addr;
    net_info.key_app_idx = mod_app_idx;

    err = ble_mesh_cfg_mod_bind_app_key(&net_info, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Bind (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Bind failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey successfully bound");
    }

    return status;
}

/* AT+BM_CC=mod_app_unbind,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,mod_app_idx,XXX */
int cmd_cfg_cli_mod_app_unbind(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_app_idx, mod_id;
    uint8_t status;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_app_unbind:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "mod_app_idx") != 0)) {                                    // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);     // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);        // 8:index
    SHELL_PARAM_UINT(mod_app_idx, argv[10]);  // 10:index

    err = bt_mesh_cfg_mod_app_unbind(net_idx, dst, elem_addr, mod_app_idx, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Unbind (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Unbind failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey successfully unbound");
    }

    return status;
}

/* AT+BM_CC=mod_app_bind_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,mod_app_idx,XXX,cid,XXX */
int cmd_cfg_cli_mod_app_bind_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_app_idx, mod_id, cid;
    uint8_t status;
    ble_mesh_prov_net_info_t net_info = {0};
    if (argc != 13) {  // 13:index
        BT_ERR("cmd_cfg_cli_mod_app_bind_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "mod_app_idx") != 0) || (strcmp(argv[11], "cid") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);     // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);        // 8:index
    SHELL_PARAM_UINT(mod_app_idx, argv[10]);  // 10:index
    SHELL_PARAM_UINT(cid, argv[12]);          // 12:index

    net_info.net_idx = net_idx;
    net_info.addr = dst;
    net_info.elem_addr = elem_addr;
    net_info.key_app_idx = mod_app_idx;

    err = ble_mesh_cfg_vnd_mod_bind_app_key(&net_info, mod_id, cid, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Bind (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Bind failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey successfully bound");
    }

    return status;
}

/* AT+BM_CC=mod_app_unbind_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,mod_app_idx,XXX,cid,XXX */
int cmd_cfg_cli_mod_app_unbind_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_app_idx, mod_id, cid;
    uint8_t status;

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_app_unbind_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "mod_app_idx") != 0) || (strcmp(argv[11], "cid") != 0)) {  // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);     // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);        // 8:index
    SHELL_PARAM_UINT(mod_app_idx, argv[10]);  // 10:index
    SHELL_PARAM_UINT(cid, argv[12]);          // 12:index

    err = bt_mesh_cfg_mod_app_unbind_vnd(net_idx, dst, elem_addr, mod_app_idx, mod_id, cid, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Unbind (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Unbind failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("AppKey successfully unbound");
    }

    return status;
}

/* AT+BM_CC=mod_app_get,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id */
int cmd_cfg_cli_mod_app_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id;
    uint8_t status;
    uint16_t apps[16];  // 16:size
    size_t cnt = 16;    // 16:size
    int err, i;

    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_mod_app_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    err = bt_mesh_cfg_mod_app_get(net_idx, dst, elem_addr, mod_id, &status, apps, &cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Get failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Apps bound to Element 0x%04x, Model 0x%04x", elem_addr, mod_id);

        if (!cnt) {
            BT_INFO_SHELL("\tNone.");
        }

        for (i = 0; i < cnt; i++) {
            BT_INFO_SHELL("\t0x%04x", apps[i]);
        }
    }

    return status;
}

/* AT+BM_CC=mod_app_get,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,cid,XXX */
int cmd_cfg_cli_mod_app_get_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, cid;
    uint8_t status;
    uint16_t apps[16];  // 16:size
    size_t cnt = 16;    // 16:size
    uint32_t err, i;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_app_get_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "cid") != 0)) {                                            // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[10]);       // 10:index

    err = bt_mesh_cfg_mod_app_get_vnd(net_idx, dst, elem_addr, mod_id, cid, &status, apps, &cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send Model App Get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model App Get failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Apps bound to Element 0x%04x, Model 0x%04x, cid 0x%04x", elem_addr, mod_id, cid);

        if (!cnt) {
            BT_INFO_SHELL("\tNone.");
        }

        for (i = 0; i < cnt; i++) {
            BT_INFO_SHELL("\t0x%04x", apps[i]);
        }
    }

    return status;
}

/* AT+BM_CC=mod_pub_get,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX */
int cmd_cfg_cli_mod_pub_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id;
    struct bt_mesh_cfg_mod_pub pub = {0};
    uint8_t status;

    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_mod_pub_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5.7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    err = bt_mesh_cfg_mod_pub_get(net_idx, dst, elem_addr, mod_id, &pub, &status);
    if (err) {
        BT_INFO_SHELL("Model Publication Get failed (err %d)", err);
        return err;
    }

    if (status) {
        BT_INFO_SHELL("Model Publication Get failed (status 0x%02x)", status);
        return status;
    }

    BT_INFO_SHELL("Model Publication for Element 0x%04x, Model 0x%04x:\n"
                  "\tPublish Address:                0x%04x\n"
                  "\tAppKeyIndex:                    0x%04x\n"
                  "\tCredential Flag:                %u\n"
                  "\tPublishTTL:                     %u\n"
                  "\tPublishPeriod:                  0x%02x\n"
                  "\tPublishRetransmitCount:         %u\n"
                  "\tPublishRetransmitInterval:      %ums\n",
        dst,
        mod_id,
        pub.addr,
        pub.app_idx,
        pub.cred_flag,
        pub.ttl,
        pub.period,
        BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
        BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

    return status;
}

/* AT+BM_CC=mod_pub_get_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,cid,XXX */
int cmd_cfg_cli_mod_pub_get_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, cid;
    struct bt_mesh_cfg_mod_pub pub = {0};
    uint8_t status;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_pub_get_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "cid") != 0)) {                                            // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[10]);       // 10:index

    err = bt_mesh_cfg_mod_pub_get_vnd(net_idx, dst, elem_addr, mod_id, cid, &pub, &status);
    if (err) {
        BT_INFO_SHELL("Model Publication Get failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Publication Get failed (status 0x%02x)", status);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    BT_INFO_SHELL("Model Publication for Element 0x%04x, Model 0x%04x:\n"
                  "\tPublish Address:                0x%04x\n"
                  "\tAppKeyIndex:                    0x%04x\n"
                  "\tCredential Flag:                %u\n"
                  "\tPublishTTL:                     %u\n"
                  "\tPublishPeriod:                  0x%02x\n"
                  "\tPublishRetransmitCount:         %u\n"
                  "\tPublishRetransmitInterval:      %ums\n",
        dst,
        mod_id,
        pub.addr,
        pub.app_idx,
        pub.cred_flag,
        pub.ttl,
        pub.period,
        BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
        BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

    return status;
}

/* AT+BMCFGCLT=mod_pub_set,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,pub_addr,XXX,pub_app_idx,XXX,
    pub_cred_flag,XXX,pub_ttl,XXX,pub_period,XXX,pub_transmit,XXX,lable[optional],XXX */
static bool cmd_cfg_cli_mod_pub_set_cmd_check(int argc, const char *argv[])
{
    if (argc != 21 && argc != 23) {  // 21,23:size
        BT_INFO_SHELL("argc err %u", argc);
        return false;
    }
    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5,7:index
        BT_ERR("para name error, elem_addr or mod_id");
        return false;
    }
    if ((strcmp(argv[9], "pub_addr") != 0) || (strcmp(argv[11], "pub_app_idx") != 0) ||      // 9,11:index
        (strcmp(argv[13], "pub_cred_flag") != 0) || (strcmp(argv[15], "pub_ttl") != 0) ||    // 13,15:index
        (strcmp(argv[17], "pub_period") != 0) || (strcmp(argv[19], "pub_transmit") != 0)) {  // 17,19:index
        BT_ERR("para name error");
        return false;
    }
    return true;
}

int cmd_cfg_cli_mod_pub_set(int argc, const char *argv[])
{
    uint16_t dst, net_idx, elem_addr, mod_id;
    struct bt_mesh_cfg_mod_pub pub = {0};
    uint8_t status = 0;
    uint8_t lable[16] = {0}; // 16:size
    
    if (!cmd_cfg_cli_mod_pub_set_cmd_check(argc, argv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], 0, 1);     // 13,14:index, 0,1:ragne
    SHELL_PARAM_RANGE_CHECK_IN(argv[15], argv[16], 0, 0x7F);  // 15,16:index, 0,0x7F:ragne
    SHELL_PARAM_RANGE_CHECK_IN(argv[17], argv[18], 0, 0xFF);  // 17,18:index, 0,0xFF:ragne
    SHELL_PARAM_RANGE_CHECK_IN(argv[19], argv[20], 0, 0xFF);  // 19,20:index, 0,0xFF:ragne

    SHELL_PARAM_UINT(pub.addr, argv[10]);       // 10:index
    SHELL_PARAM_UINT(pub.app_idx, argv[12]);    // 12:index
    SHELL_PARAM_UINT(pub.cred_flag, argv[14]);  // 14:index
    SHELL_PARAM_UINT(pub.ttl, argv[16]);        // 16:index
    SHELL_PARAM_UINT(pub.period, argv[18]);     // 18:index
    SHELL_PARAM_UINT(pub.transmit, argv[20]);   // 20:index

    if (argc == 23) {                          // 23:size
        if (strcmp(argv[21], "lable") != 0) {  // 21:index
            return ERRCODE_BLE_MESH_FAIL;
        }

        if (strlen(argv[22]) != 32) {                                        // 22:index;32:len,32char,16byte
            BT_INFO_SHELL("invaild lable(uuid) len: %u", strlen(argv[22]));  // 22:index
            return ERRCODE_BLE_MESH_FAIL;
        }

        arg_str_to_key(argv[22], lable, 16);  // 22:index, 16:size
        pub.uuid = lable;
    }
    int err = bt_mesh_cfg_mod_pub_set(net_idx, dst, elem_addr, mod_id, &pub, &status);
    if (err != 0) {
        BT_INFO_SHELL("Model Publication Set failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("Model Publication Set %s (status 0x%02x)", status ? "failed" : "succ", status);
    return status;
}

/* AT+BM_CC=mod_pub_set_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,pub_addr,XXX,pub_app_idx,XXX,
    pub_cred_flag,XXX,pub_ttl,XXX,pub_period,XXX,pub_transmit,XXX,cid,XXX */
int cmd_cfg_cli_mod_pub_set_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, cid;
    struct bt_mesh_cfg_mod_pub pub = {0};
    uint8_t status;

    if (argc != 23) {  // 23:size
        BT_ERR("cmd_cfg_cli_mod_pub_set_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    if ((strcmp(argv[9], "pub_addr") != 0) || (strcmp(argv[11], "pub_app_idx") != 0) ||      // 9,11:index
        (strcmp(argv[13], "pub_cred_flag") != 0) || (strcmp(argv[15], "pub_ttl") != 0) ||    // 13,15:index
        (strcmp(argv[17], "pub_period") != 0) || (strcmp(argv[19], "pub_transmit") != 0) ||  // 17,19:index
        (strcmp(argv[21], "cid") != 0)) {                                                    // 21:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(pub.addr, argv[10]);       // 10:index
    SHELL_PARAM_UINT(pub.app_idx, argv[12]);    // 12:index
    SHELL_PARAM_UINT(pub.cred_flag, argv[14]);  // 14:index
    SHELL_PARAM_UINT(pub.ttl, argv[16]);        // 16:index
    SHELL_PARAM_UINT(pub.period, argv[18]);     // 18:index
    SHELL_PARAM_UINT(pub.transmit, argv[20]);   // 20:index
    SHELL_PARAM_UINT(cid, argv[22]);            // 22:index

    err = bt_mesh_cfg_mod_pub_set_vnd(net_idx, dst, elem_addr, mod_id, cid, &pub, &status);
    if (err) {
        BT_INFO_SHELL("Model Publication Set failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Publication Set failed (status 0x%02x)", status);
    } else {
        BT_INFO_SHELL("Model Publication successfully set");
    }

    return status;
}

/* AT+BM_CC=mod_sub_add,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX */
int cmd_cfg_cli_mod_sub_add(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_add:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7index
        (strcmp(argv[9], "sub_addr") != 0)) {                                       // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index

    err = bt_mesh_cfg_mod_sub_add(net_idx, dst, elem_addr, sub_addr, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Add (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Add failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_add_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_add_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_add:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_addr") != 0) || (strcmp(argv[11], "cid") != 0)) {     // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    err = bt_mesh_cfg_mod_sub_add_vnd(net_idx, dst, elem_addr, sub_addr, mod_id, cid, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Add (err %d)", err);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Add failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_del,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX */
int cmd_cfg_cli_mod_sub_del(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;

    if (argc != 11) {  // 11:index
        BT_ERR("cmd_cfg_cli_mod_sub_del:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_addr") != 0)) {                                       // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index

    err = bt_mesh_cfg_mod_sub_del(net_idx, dst, elem_addr, sub_addr, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Delete (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Delete failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription deltion was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_del_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_del_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_del_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_addr") != 0) || (strcmp(argv[11], "cid") != 0)) {     // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    err = bt_mesh_cfg_mod_sub_del_vnd(net_idx, dst, elem_addr, sub_addr, mod_id, cid, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Delete (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Delete failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription deltion was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_overwrite,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX */
int cmd_cfg_cli_mod_sub_overwrite(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_overwrite:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_addr") != 0)) {                                       // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index

    err = bt_mesh_cfg_mod_sub_overwrite(net_idx, dst, elem_addr, sub_addr, mod_id, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Overwrite (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Overwrite failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription overwrite was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_overwrite_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,sub_addr,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_overwrite_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_overwrite_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_addr") != 0) || (strcmp(argv[11], "cid") != 0)) {     // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(sub_addr, argv[10]);  // 10:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    err = bt_mesh_cfg_mod_sub_overwrite_vnd(net_idx, dst, elem_addr, sub_addr, mod_id, cid, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Overwrite (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Overwrite failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model subscription overwrite was successful");
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_add,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX */
int cmd_cfg_cli_mod_sub_va_add(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;
    uint8_t lable[16];  // 16:size

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_add:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0)) {                                          // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    if (strlen(argv[10]) != 32) {                                // 10:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_add(net_idx, dst, elem_addr, lable, mod_id, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Add (err %d)", err);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Add failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x subscribed (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_add_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_va_add_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;
    uint8_t lable[16];  // 16:index

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_add_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0) || (strcmp(argv[11], "cid") != 0)) {        // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    if (strlen(argv[10]) != 32) {                                // 10:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_add_vnd(net_idx, dst, elem_addr, lable, mod_id, cid, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Add (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Add failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x subscribed (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_del,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX */
int cmd_cfg_cli_mod_sub_va_del(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;
    uint8_t lable[16];  // 16:size

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_del:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0)) {                                          // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    if (strlen(argv[10]) != 32) {                                // 10:index;32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_del(net_idx, dst, elem_addr, lable, mod_id, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Delete (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Delete failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x unsubscribed (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_del_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_va_del_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;
    uint8_t lable[16];  // 16:size

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_del_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0) || (strcmp(argv[11], "cid") != 0)) {        // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    if (strlen(argv[10]) != 32) {                                // 10:index,32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_del_vnd(net_idx, dst, elem_addr, lable, mod_id, cid, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Delete (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Delete failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x unsubscribed (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_overwrite,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX */
int cmd_cfg_cli_mod_sub_va_overwrite(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr;
    uint8_t status;
    uint8_t lable[16];  // 16:size

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_overwrite:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0)) {                                          // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index

    if (strlen(argv[10]) != 32) {                                // 10:index,32:size
        BT_INFO_SHELL("invaild key len: %u", strlen(argv[10]));  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_overwrite(net_idx, dst, elem_addr, lable, mod_id, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Overwrite (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Overwrite failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x Sub VA Overwrite (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_va_overwrite_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,lable,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_va_overwrite_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, sub_addr, cid;
    uint8_t status;
    uint8_t lable[16];  // 16:size

    if (argc != 13) {  // 13:size
        BT_ERR("cmd_cfg_cli_mod_sub_va_overwrite_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "lable") != 0) || (strcmp(argv[11], "cid") != 0)) {        // 9,11:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[12]);       // 12:index

    if ((strlen(argv[10]) != 32) || (dst == 1)) {                               // 10:index,32:size,1:addr
        BT_INFO_SHELL("invaild key len: %u, addr: %u", strlen(argv[10]), dst);  // 10:index
        return ERRCODE_BLE_MESH_FAIL;
    }

    const char *str = argv[10];      // 10:index
    arg_str_to_key(str, lable, 16);  // 16:size

    err = bt_mesh_cfg_mod_sub_va_overwrite_vnd(net_idx, dst, elem_addr, lable, mod_id, cid, &sub_addr, &status);
    if (err) {
        BT_INFO_SHELL("Unable to send Mod Sub VA Overwrite (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Mod Sub VA Overwrite failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("0x%04x Sub VA Overwrite (va 0x%04x)", elem_addr, sub_addr);
    }

    return status;
}

/* AT+BM_CC=mod_sub_get,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX */
int cmd_cfg_cli_mod_sub_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id;
    uint8_t status;
    uint16_t subs[16];  // 16:size
    size_t cnt = 16;    // 16:size
    uint32_t err, i;

    if (argc != 9) {  // 9:size
        BT_ERR("cmd_cfg_cli_mod_sub_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0)) {  // 5,7:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:size

    err = bt_mesh_cfg_mod_sub_get(net_idx, dst, elem_addr, mod_id, &status, subs, &cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Get failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model Subscriptions for Element 0x%04x, Model 0x%04x", elem_addr, mod_id);

        if (!cnt) {
            BT_INFO_SHELL("\tNone.");
        }

        for (i = 0; i < cnt; i++) {
            BT_INFO_SHELL("\t0x%04x", subs[i]);
        }
    }

    return status;
}

/* AT+BM_CC=mod_sub_get_vnd,addr,XXX,net_index,XXX,elem_addr,XXX,mod_id,XXX,cid,XXX */
int cmd_cfg_cli_mod_sub_get_vnd(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t elem_addr, mod_id, cid;
    uint8_t status;
    uint16_t subs[16];  // 16:size
    size_t cnt = 16;    // 16:size
    uint32_t err, i;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_mod_sub_get_vnd:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "elem_addr") != 0) || (strcmp(argv[7], "mod_id") != 0) ||  // 5,7:index
        (strcmp(argv[9], "cid") != 0)) {                                            // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(elem_addr, argv[6]);  // 6:index
    SHELL_PARAM_UINT(mod_id, argv[8]);     // 8:index
    SHELL_PARAM_UINT(cid, argv[10]);       // 10:index

    err = bt_mesh_cfg_mod_sub_get_vnd(net_idx, dst, elem_addr, mod_id, cid, &status, subs, &cnt);
    if (err) {
        BT_INFO_SHELL("Unable to send Model Subscription Get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Model Subscription Get failed with status 0x%02x", status);
    } else {
        BT_INFO_SHELL("Model Subscriptions for Element 0x%04x, Model 0x%04x, cid 0x%04x", elem_addr, mod_id, cid);

        if (!cnt) {
            BT_INFO_SHELL("\tNone.");
        }

        for (i = 0; i < cnt; i++) {
            BT_INFO_SHELL("\t0x%04x", subs[i]);
        }
    }

    return status;
}

static void hb_sub_print(struct bt_mesh_cfg_hb_sub *sub)
{
    BT_INFO_SHELL("Heartbeat Subscription:\n"
                  "\tSource:      %u\n"
                  "\tDestination: %u\n"
                  "\tPeriodLog:   %u\n"
                  "\tCountLog:    %u\n"
                  "\tMinHops:     %u\n"
                  "\tMaxHops:     %u\n",
        sub->src,
        sub->dst,
        sub->period,
        sub->count,
        sub->min,
        sub->max);
}

/* AT+BM_CC=hb_sub_set,addr,XXX,net_index,XXX,sub_src,XXX,sub_dst,XXX,sub_period,XXX */
int cmd_cfg_cli_hb_sub_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    struct bt_mesh_cfg_hb_sub sub;

    if (argc != 11) {  // 11:size
        BT_ERR("cmd_cfg_cli_hb_sub_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "sub_src") != 0) || (strcmp(argv[7], "sub_dst") != 0) ||  // 5,7:index
        (strcmp(argv[9], "sub_period") != 0)) {                                    // 9:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[5], argv[6], 0, 0xFFFF);  // 5,6:index, 0,0xFFFF:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0xFFFF);  // 7,8:index, 0,0xFFFF:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0x11);   // 9,10:index, 0,0x11:range

    sub.src = atoi(argv[6]);      // 6:index
    sub.dst = atoi(argv[8]);      // 8:index
    sub.period = atoi(argv[10]);  // 10:index

    err = bt_mesh_cfg_hb_sub_set(net_idx, dst, &sub, &status);
    if (err) {
        BT_INFO_SHELL("Heartbeat Subscription Set failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Heartbeat Subscription Set failed (status 0x%02x)", status);
    } else {
        hb_sub_print(&sub);
    }

    return status;
}

/* AT+BM_CC=hb_sub_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_hb_sub_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    struct bt_mesh_cfg_hb_sub sub;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_hb_sub_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_hb_sub_get(net_idx, dst, &sub, &status);
    if (err) {
        BT_INFO_SHELL("Heartbeat Subscription Get failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Heartbeat Subscription Get failed (status 0x%02x)", status);
    } else {
        hb_sub_print(&sub);
    }

    return status;
}

/* AT+BM_CC=cmd_cfg_cli_hb_pub_set,addr,XXX,net_index,XXX,pub_dst,XXX,pub_count,XXX,pub_period,XXX,pub_ttl,XXX,
pub_feat,XXX,pub_net_idx,XXX */
int cmd_cfg_cli_hb_pub_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    struct bt_mesh_cfg_hb_pub pub;

    if (argc != 17) {  // 17:size
        BT_ERR("cmd_cfg_cli_hb_pub_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "pub_dst") != 0) || (strcmp(argv[7], "pub_count") != 0) ||       // 5,7:index
        (strcmp(argv[9], "pub_period") != 0) || (strcmp(argv[11], "pub_ttl") != 0) ||     // 9,11:index
        (strcmp(argv[13], "pub_feat") != 0) || (strcmp(argv[15], "pub_net_idx") != 0)) {  // 13,15:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0xFF);      // 7,8:index, 0,0xFF:range
    SHELL_PARAM_RANGE_CHECK_OUT(argv[7], argv[8], 0x12, 0xFE);  // 7,8:index, 0,0xFE:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[9], argv[10], 0, 0x11);     // 9,10:index, 0,0x11:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[11], argv[12], 0, 0x7F);    // 11,12:index, 0,0x7F:range
    SHELL_PARAM_RANGE_CHECK_IN(argv[13], argv[14], 0, 0xF);     // 13,14:index, 0,0xF:range

    SHELL_PARAM_UINT(pub.dst, argv[6]);       // 6:index
    pub.count = atoi(argv[8]);                // 8:index
    pub.period = atoi(argv[10]);              // 10:index
    pub.ttl = atoi(argv[12]);                 // 12:index
    pub.feat = atoi(argv[14]);                // 14:index
    SHELL_PARAM_UINT(pub.net_idx, argv[16]);  // 16:index

    err = bt_mesh_cfg_hb_pub_set(net_idx, dst, &pub, &status);
    if (err) {
        BT_INFO_SHELL("Heartbeat Publication Set failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Heartbeat Publication Set failed (status 0x%02x)", status);
    } else {
        BT_INFO_SHELL("Heartbeat publication successfully set");
    }

    return status;
}

/* AT+BM_CC=hb_pub_get,addr,XXX,net_index,XXX */
int cmd_cfg_cli_hb_pub_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    struct bt_mesh_cfg_hb_pub pub;

    if (argc != 5) {  // 5:size
        BT_ERR("cmd_cfg_cli_hb_pub_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_cfg_hb_pub_get(net_idx, dst, &pub, &status);
    if (err) {
        BT_INFO_SHELL("Heartbeat Publication Get failed (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("Heartbeat Publication Get failed (status 0x%02x)", status);
        return status;
    }

    BT_INFO_SHELL("Heartbeat publication:");
    BT_INFO_SHELL("\tdst %u count %u period %u", pub.dst, pub.count, pub.period);
    BT_INFO_SHELL("\tttl %u feat %u net_idx %u", pub.ttl, pub.feat, pub.net_idx);

    return status;
}

/* AT+BM_CC=cli_timeout_get */
int cmd_cfg_cli_timeout_get(int argc, const char *argv[])
{
    int32_t timeout;

    timeout = bt_mesh_cfg_cli_timeout_get();
    if (timeout == K_FOREVER) {
        BT_INFO_SHELL("Message timeout: forever");
    } else {
        BT_INFO_SHELL("Message timeout: %u seconds", timeout / 1000);  // 1000: size
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BM_CC=cli_timeout_set,timeout,XXX */
int cmd_cfg_cli_timeout_set(int argc, const char *argv[])
{
    int32_t timeout;

    if (argc != 3) {  // 3:size
        BT_ERR("cmd_cfg_cli_timeout_set:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "timeout") != 0) {
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_INT(timeout, argv[2]);                  // 2 : index
    if (timeout < 0 || timeout > (INT32_MAX / 1000)) {  // 1000:size
        timeout = K_FOREVER;
    } else {
        timeout = timeout * 1000;  // 1000:size
    }

    bt_mesh_cfg_cli_timeout_set(timeout);
    if (timeout == K_FOREVER) {
        BT_INFO_SHELL("Message timeout: forever");
    } else {
        BT_INFO_SHELL("Message timeout: %u seconds", timeout / 1000);  // 1000:size
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

/* AT+BMCFGCLT=lpn_timeout_get,addr,XXX,net_index,XXX,unicast_addr,XXX */
int cmd_cfg_cli_lpn_timeout_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint8_t status;
    uint16_t unicast_addr;
    int32_t timeout;

    if (argc != 7) {  // 7:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "unicast_addr") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(unicast_addr, argv[6]);  // 6:index

    err = bt_mesh_cfg_lpn_timeout_get(net_idx, dst, unicast_addr, &timeout);
    if (err) {
        BT_INFO_SHELL("Unable to send LPN timeout get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("LPN timeout get for NetKey 0x%03x, timeout: %i", net_idx, timeout);
    return status;
}


/* AT+BMCFGCLT=krp_set,addr,XXX,net_index,XXX,key_net_idx,XXX,transition,XXX */
int cmd_cfg_cli_krp_set(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t transition, phase;

    if (argc != 9) {  // 9:size
        BT_ERR("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if ((strcmp(argv[5], "key_net_idx") != 0) || (strcmp(argv[7], "transition") != 0)) {  // 5,7:size
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[7], argv[8], 0, 0x03);  // 7,8:index 0,0x03:range

    SHELL_PARAM_UINT(key_net_idx, argv[6]);   // 6:index
    SHELL_PARAM_UINT(transition, argv[8]);  // 8:index

    err = bt_mesh_cfg_krp_set(net_idx, dst, key_net_idx, transition, &status, &phase);
    if (err) {
        BT_INFO_SHELL("Unable to send krp set (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("krp set failed with status 0x%02x, phase: 0x%02x", status, phase);
    } else {
        BT_INFO_SHELL("krp set with NetKey Index 0x%04x, phase: 0x%02x", key_net_idx, phase);
    }

    return status;
}

/* AT+BMCFGCLT=krp_get,addr,XXX,net_index,XXX,key_net_idx,XXX */
int cmd_cfg_cli_krp_get(int argc, const char *argv[])
{
    uint16_t dst;
    uint16_t net_idx;
    uint16_t key_net_idx;
    uint8_t status;
    uint8_t phase;

    if (argc != 7) {  // 7:size
        BT_ERR("cmd_cfg_cli_app_key_get:argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int err = cmd_cfg_get_addr_and_net_index(argv, &dst, &net_idx);
    if (err) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[5], "key_net_idx") != 0) {  // 5:index
        BT_ERR("cmd err");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(key_net_idx, argv[6]);  // 6:index

    err = bt_mesh_cfg_krp_get(net_idx, dst, key_net_idx, &status, &phase);
    if (err) {
        BT_INFO_SHELL("Unable to send node krp get (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (status) {
        BT_INFO_SHELL("node krp get failed with status 0x%02x", status);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    BT_INFO_SHELL("node krp state of NetKey Index 0x%04x: 0x%02x", key_net_idx, phase);
    return status;
}

at_cmd cfg_cli_cmd_tbl[] = {
    {"node_reset", cmd_cfg_cli_node_reset},
    {"comp_data_get", cmd_cfg_cli_comp_data_get},
    {"comp_vid_set", cmd_cfg_cli_comp_vid_set},
    {"beacon_get", cmd_cfg_cli_beacon_get},
    {"beacon_set", cmd_cfg_cli_beacon_set},
    {"ttl_get", cmd_cfg_cli_ttl_get},
    {"ttl_set", cmd_cfg_cli_ttl_set},
    {"friend_get", cmd_cfg_cli_friend_get},
    {"friend_set", cmd_cfg_cli_friend_set},
    {"net_transmit_get", cmd_cfg_cli_net_transmit_get},
    {"net_transmit_set", cmd_cfg_cli_net_transmit_set},
    {"relay_get", cmd_cfg_cli_relay_get},
    {"relay_set", cmd_cfg_cli_relay_set},
    {"net_key_add", cmd_cfg_cli_net_key_add},
    {"net_key_get", cmd_cfg_cli_net_key_get},
    {"net_key_update", cmd_cfg_cli_net_key_update},
    {"net_key_del", cmd_cfg_cli_net_key_del},
    {"app_key_add", cmd_cfg_cli_app_key_add},
    {"app_key_get", cmd_cfg_cli_app_key_get},
    {"app_key_update", cmd_cfg_cli_app_key_update},
    {"app_key_del", cmd_cfg_cli_app_key_del},
    {"mod_app_bind", cmd_cfg_cli_mod_app_bind},
    {"mod_app_unbind", cmd_cfg_cli_mod_app_unbind},
    {"mod_app_bind_vnd", cmd_cfg_cli_mod_app_bind_vnd},
    {"mod_app_unbind_vnd", cmd_cfg_cli_mod_app_unbind_vnd},
    {"mod_app_get", cmd_cfg_cli_mod_app_get},
    {"mod_app_get_vnd", cmd_cfg_cli_mod_app_get_vnd},
    {"mod_pub_get", cmd_cfg_cli_mod_pub_get},
    {"mod_pub_get_vnd", cmd_cfg_cli_mod_pub_get_vnd},
    {"mod_pub_set", cmd_cfg_cli_mod_pub_set},
    {"mod_pub_set_vnd", cmd_cfg_cli_mod_pub_set_vnd},
    {"mod_sub_add", cmd_cfg_cli_mod_sub_add},
    {"mod_sub_add_vnd", cmd_cfg_cli_mod_sub_add_vnd},
    {"mod_sub_del", cmd_cfg_cli_mod_sub_del},
    {"mod_sub_del_all", cmd_cfg_cli_mod_sub_del_all},
    {"mod_sub_del_vnd", cmd_cfg_cli_mod_sub_del_vnd},
    {"mod_sub_overwrite", cmd_cfg_cli_mod_sub_overwrite},
    {"mod_sub_overwrite_vnd", cmd_cfg_cli_mod_sub_overwrite_vnd},
    {"mod_sub_va_add", cmd_cfg_cli_mod_sub_va_add},
    {"mod_sub_va_add_vnd", cmd_cfg_cli_mod_sub_va_add_vnd},
    {"mod_sub_va_del", cmd_cfg_cli_mod_sub_va_del},
    {"mod_sub_va_del_vnd", cmd_cfg_cli_mod_sub_va_del_vnd},
    {"mod_sub_va_overwrite", cmd_cfg_cli_mod_sub_va_overwrite},
    {"mod_sub_va_overwrite_vnd", cmd_cfg_cli_mod_sub_va_overwrite_vnd},
    {"mod_sub_get", cmd_cfg_cli_mod_sub_get},
    {"mod_sub_get_vnd", cmd_cfg_cli_mod_sub_get_vnd},
    {"hb_sub_set", cmd_cfg_cli_hb_sub_set},
    {"hb_sub_get", cmd_cfg_cli_hb_sub_get},
    {"hb_pub_set", cmd_cfg_cli_hb_pub_set},
    {"hb_pub_get", cmd_cfg_cli_hb_pub_get},
    {"cli_timeout_get", cmd_cfg_cli_timeout_get},
    {"cli_timeout_set", cmd_cfg_cli_timeout_set},
    {"node_identity_set", cmd_cfg_cli_node_identity_set},
    {"node_identity_get", cmd_cfg_cli_node_identity_get},
    {"gatt_proxy_get", cmd_cfg_cli_gatt_proxy_get},
    {"gatt_proxy_set", cmd_cfg_cli_gatt_proxy_set},
    {"lpn_timeout_get", cmd_cfg_cli_lpn_timeout_get},
    {"krp_set", cmd_cfg_cli_krp_set},
    {"krp_get", cmd_cfg_cli_krp_get},
};

uint32_t cmd_ble_mesh_cfg_cli_op_func(int argc, const char *argv[])
{
    if (argc < 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    int cmd_tbl_num = (sizeof(cfg_cli_cmd_tbl) / sizeof(at_cmd));
    for (int i = 0; i < cmd_tbl_num; i++) {
        if (strcmp(argv[0], cfg_cli_cmd_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return cfg_cli_cmd_tbl[i].func(argc, argv);
        }
    }

    BT_INFO_SHELL("invalid cmd:%s", argv[0]);

    return ERRCODE_BLE_MESH_FAIL;
}

// 配置客户端接口，AT+BM_CC --- 参数类型如何构建待确定
at_ret_t at_bt_mesh_config_client(const ble_mesh_config_client_t *args)
{
    // 判断是否要删除多余的逗号
    BT_INFO("[debug] at_bt_mesh_config_client args[1]:%s,  args[2]:%s, args[3]:%s args[4]:%s\r\n",
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
    uint32_t ret = cmd_ble_mesh_cfg_cli_op_func(argc, (const char **)argv);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("error ret = 0x%x \r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_para_parse_syntax_t ble_mesh_config_client[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para10)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para11)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para12)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para13)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para14)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para15)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para16)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para17)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para18)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para19)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para20)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para21)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_config_client_t, para22)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_config_client_t, para23)},
};
#endif

const at_cmd_entry_t at_bt_mesh_cfg_parse_table[] = {
#if MYNEWT_VAL(BLE_MESH_CONSOLE_CFG_CLIENT)
    {
        "BMCFGCLT",  // 配置客户端 AT+BM_CC
        0,
        0,
        ble_mesh_config_client,                   // 参数类型
        NULL,                                     // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_config_client,  // 有参数时调用的函数
        NULL,
        NULL,
    },
#endif
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_cfg_parse_table) / sizeof(at_bt_mesh_cfg_parse_table[0]))

void mesh_at_cfg_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_cfg_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
