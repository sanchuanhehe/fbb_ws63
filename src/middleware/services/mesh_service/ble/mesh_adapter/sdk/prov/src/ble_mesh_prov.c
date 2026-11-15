/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh prov.
 *
 * History:
 * 2025-4-15, Create file.
 */
#include <stdbool.h>
#include <stdint.h>

#include "securec.h"
#include "mesh/mesh.h"
#include "mesh/main.h"
#include "provisioner.h"
#include "access.h"
#include "beacon.h"
#include "osal_addr.h"
#include "prov.h"

#include "bts_def.h"
#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_errcode_inner.h"
#include "remote_prov_client.h"
#include "remote_prov_server.h"
#include "ble_mesh_prov.h"

static void link_close(bt_mesh_prov_bearer_t bearer);
static void prov_reset(void);
static int output_number(bt_mesh_output_action_t action, uint32_t number);
static int output_string(const char *str);
static int input(bt_mesh_input_action_t act, uint8_t size);
static void recv_unprovisioned_beacon(uint8_t uuid[BLE_MESH_UUID_LEN],
                                      bt_mesh_prov_oob_info_t oob_info,
                                      uint32_t *uri_hash);
static void provisioner_complete(uint16_t net_idx, uint16_t addr);
static void recv_rmt_unprovisioned_beacon(
    uint8_t uuid[16], bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash, int8_t rssi);
static void node_added(uint16_t net_idx, uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t addr, uint8_t num_elem);

static ble_mesh_prov_t g_mesh_prov = { 0 };
static struct bt_mesh_prov provision = {
    .unprovisioned_beacon = recv_unprovisioned_beacon,
    .complete = provisioner_complete,
    .link_close = link_close,
    .reset = prov_reset,
    .node_added = node_added,
    .input = input,
    .output_number = output_number,
    .output_string = output_string,
};

static bt_mesh_rmt_prov_t rmt_provision = {
    .rmt_unprovisioned_beacon_recv = recv_rmt_unprovisioned_beacon,
};

static uint8_t g_uuid[BLE_MESH_UUID_LEN] = { 0 };
static uint8_t static_auth[BLE_MESH_STATIC_AUTH_LEN] = { 0 };
static bd_addr_t g_mesh_addr_net = {0};
static bool cfg_load = false;
static ble_mesh_prov_bearer_t g_prov_bearers;

static void link_close(bt_mesh_prov_bearer_t bearer)
{
    if (g_mesh_prov.link_close != NULL) {
        g_mesh_prov.link_close(bearer);
    }
    return;
}

static void prov_reset(void)
{
    if (g_mesh_prov.reset != NULL) {
        g_mesh_prov.reset();
    }
    
    return;
}

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
    switch (action) {
        case BT_MESH_BLINK:
            BT_INFO("OOB blink Number: %u", number);
            break;
        case BT_MESH_BEEP:
            BT_INFO("OOB beep Number: %u", number);
            break;
        case BT_MESH_VIBRATE:
            BT_INFO("OOB vibrate Number: %u", number);
            break;
        case BT_MESH_DISPLAY_NUMBER:
            BT_INFO("OOB display Number: %u", number);
            break;
        default:
            BT_ERR("Unknown Output action %u (number %u) requested!", action, number);
            return -EINVAL;
    }

    return 0;
}

static int output_string(const char *str)
{
    BT_INFO("OOB String: %s", str);
    return 0;
}

static int input(bt_mesh_input_action_t act, uint8_t size)
{
    switch (act) {
        case BT_MESH_ENTER_NUMBER:
            BT_INFO("Enter a number (max %u digits) with: Input-num <num>", size);
            break;
        case BT_MESH_ENTER_STRING:
            BT_INFO("Enter a string (max %u chars) with: Input-str <str>", size);
            break;
        case BT_MESH_TWIST:
            BT_INFO("\"Twist\" a number (max %u digits) with: Input-num <num>", size);
            break;
        case BT_MESH_PUSH:
            BT_INFO("\"Push\" a number (max %u digits) with: Input-num <num>", size);
            break;
        default:
            BT_ERR("Unknown Input action %u (size %u) requested!", act, size);
            return -EINVAL;
    }

    return 0;
}

static void recv_unprovisioned_beacon(uint8_t uuid[16], bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash)
{
    if (g_mesh_prov.unprovisioned_beacon_recv != NULL) {
        g_mesh_prov.unprovisioned_beacon_recv(uuid, oob_info, uri_hash);
    }
    return;
}

static void recv_rmt_unprovisioned_beacon(uint8_t uuid[16], bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash,
                                          int8_t rssi)
{
    if (g_mesh_prov.rmt_unprovisioned_beacon_recv != NULL) {
        g_mesh_prov.rmt_unprovisioned_beacon_recv(uuid, oob_info, uri_hash, rssi);
    }
    return;
}

static void provisioner_complete(uint16_t net_idx, uint16_t addr)
{
    if (g_mesh_prov.provisioned_complete != NULL) {
        g_mesh_prov.provisioned_complete(net_idx, addr);
    }
    return;
}

static void node_added(uint16_t net_idx, uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t addr, uint8_t num_elem)
{
    if (g_mesh_prov.node_added != NULL) {
        g_mesh_prov.node_added(net_idx, uuid, addr, num_elem);
    }
    
    return;
}

errcode_t ble_mesh_prov_disable(void)
{
    int err;
    memset_s(bt_mesh.flags, sizeof(bt_mesh.flags), 0, sizeof(bt_mesh.flags));
    bt_mesh_prov_reset_state();
    err = bt_mesh_prov_disable(g_prov_bearers);
    if (err != 0) {
        BT_INFO("Failed to disable mesh node (err %d)", err);
    }
    cfg_load = false;
    return err;
}

errcode_t ble_mesh_prov_enable(ble_mesh_prov_bearer_t bearers)
{
    int err;
    #if (MYNEWT_VAL(BLE_MESH_SETTINGS_EXT))
    if (!cfg_load) {
        bt_mesh_settings_conf_load();
        cfg_load = true;
    }
    #endif

    err = bt_mesh_prov_enable(bearers);
    g_prov_bearers = bearers;
    if (err != 0 && err != -EALREADY) { // 已经配网了，prov使能不返回失败
        BT_INFO("Failed to enable mesh prov (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_prov_deinit(void)
{
    if (provision.uri) {
        osal_kfree((void *)provision.uri);
        provision.uri = NULL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_prov_init(uint8_t uuid[BLE_MESH_UUID_LEN], const ble_mesh_prov_t *mesh_prov)
{
    memset_s(&g_mesh_prov, sizeof(ble_mesh_prov_t), 0, sizeof(ble_mesh_prov_t));

    (void)memcpy_s(g_uuid, BLE_MESH_UUID_LEN, uuid, BLE_MESH_UUID_LEN);
    g_mesh_prov.unprovisioned_beacon_recv = mesh_prov->unprovisioned_beacon_recv;
    g_mesh_prov.link_close = mesh_prov->link_close;
    g_mesh_prov.provisioned_complete = mesh_prov->provisioned_complete;
    g_mesh_prov.reset = mesh_prov->reset;
    g_mesh_prov.rmt_unprovisioned_beacon_recv = mesh_prov->rmt_unprovisioned_beacon_recv;
    g_mesh_prov.node_added = mesh_prov->node_added;

    provision.uuid = g_uuid;
    if (mesh_prov->uri) {
        char *uri_hash = (char *)osal_kmalloc(strlen(mesh_prov->uri) + 1, OSAL_GFP_ATOMIC);  // 1:结束符
        if (uri_hash) {
            if (strcpy_s(uri_hash, strlen(mesh_prov->uri) + 1, mesh_prov->uri) == EOK) {  // 1:结束符
                provision.uri = uri_hash;
            } else {
                osal_kfree(uri_hash);
            }
        }
    }

    if (mesh_prov->static_value_len != 0) {
        provision.static_val = static_auth;
        provision.static_val_len = mesh_prov->static_value_len;
        if (memcpy_s(static_auth, BLE_MESH_STATIC_AUTH_LEN,
                     mesh_prov->static_auth_value, mesh_prov->static_value_len) != EOK) {
            BT_ERR("Failed to cpy static auth value, value len:%d", mesh_prov->static_value_len);
        }
    } else {
        provision.static_val = NULL;
        provision.static_val_len = 0;
    }
    bt_mesh_rmt_prov_register(&rmt_provision);

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_provisioner_disable(void)
{
    int err;
    err =  bt_mesh_provisioner_deinit();
    if (err != 0) {
        BT_ERR("bt_mesh_provisioner_deinit (err %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_provisioner_enable(uint16_t addr, uint16_t net_idx, uint8_t flags, uint32_t iv_index)
{
    int err;
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    err = bt_mesh_provisioner_enable(addr, net_idx, flags, iv_index);
    if (err != 0) {
        BT_INFO("Failed to enable mesh node (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_pb_adv_open(const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t addr)
{
    int err;
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    err = bt_mesh_pb_adv_open(uuid, net_idx, addr, 0);
    if (err != 0) {
        BT_INFO("Failed to adv open (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

struct bt_mesh_prov *ble_mesh_get_provision(void)
{
    return &provision;
}

errcode_t ble_mesh_set_prov_addr(const bd_addr_t *addr)
{
    if (addr == NULL) {
        BT_INFO("addr is null");
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    g_mesh_addr_net.type = addr->type;
    memcpy_s(g_mesh_addr_net.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
    return ERRCODE_BLE_MESH_SUCCESS;
}

// 获取待配网的设备地址
void ble_mesh_get_prov_addr(bd_addr_t *addr)
{
    if (addr == NULL) {
        BT_INFO("addr is null");
        return;
    }
    addr->type = g_mesh_addr_net.type;
    (void)memcpy_s(addr->addr, BD_ADDR_LEN, g_mesh_addr_net.addr, BD_ADDR_LEN);
    return;
}

errcode_t ble_mesh_pb_start_rmt_scan(uint16_t net_idx, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN],
                                     uint8_t items_limit, uint8_t timeout)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err = bt_mesh_rmt_prov_start_scan(net_idx, addr, uuid, items_limit, timeout);
    if (err != 0) {
        BT_INFO("Failed to scan open (err %d)", err);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_pb_start_rmt_ex_scan(
    uint16_t net_idx, uint16_t addr, const uint8_t *uuid, uint8_t timeout, ble_mesh_rmt_prov_ex_scan_t *ad_filter)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err = bt_mesh_rmt_prov_start_ex_scan(net_idx, addr, uuid, timeout, ad_filter);
    if (err != 0) {
        BT_INFO("Failed to scan open (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_pb_rmt_adv_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t rmt_addr, uint16_t prov_addr, uint8_t nppi)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err = bt_mesh_rmt_prov_adv_open(uuid, net_idx, rmt_addr, prov_addr, nppi);
    if (err != 0) {
        BT_INFO("Failed to adv open (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_static_oob(const uint8_t *static_val, uint8_t size)
{
    int err = bt_mesh_auth_method_set_static(static_val, size);
    return ble_mesh_errcode_convert(err);
}

errcode_t ble_mesh_set_remote_public_key(const uint8_t *key)
{
    int err = bt_mesh_prov_remote_pub_key_set(key);
    return ble_mesh_errcode_convert(err);
}

errcode_t ble_mesh_set_input_action(bt_mesh_input_action_t action, uint8_t size)
{
    int err = bt_mesh_auth_method_set_input(action, size);
    return ble_mesh_errcode_convert(err);
}

errcode_t ble_mesh_set_output_action(bt_mesh_output_action_t action, uint8_t size)
{
    int err = bt_mesh_auth_method_set_output(action, size);
    return ble_mesh_errcode_convert(err);
}

errcode_t ble_mesh_node_reset(void)
{
    bt_mesh_reset();
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_prov_start_unprov_beacon_send(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    bt_mesh_beacon_set_unprov_beacon_flag(true);
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_prov_stop_unprov_beacon_send(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    bt_mesh_beacon_set_unprov_beacon_flag(false);
    return ERRCODE_BLE_MESH_SUCCESS;
}

bool ble_mesh_device_is_provisioned(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return false;
    }
    return bt_mesh_is_provisioned();
}

uint16_t ble_mesh_get_primary_addr(void)
{
    return bt_mesh_primary_addr();
}

errcode_t ble_mesh_set_prov_link_id(uint32_t link_id, bool is_filter)
{
    bt_mesh_prov_adv_link_set(link_id, is_filter);
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_local_net_info(ble_mesh_local_net_info_t *net_info)
{
    int err = 0;

    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);

    err = bt_mesh_provision(net_info->net_key, net_info->net_idx, net_info->flags, net_info->iv_idx,
                            net_info->addr, net_info->dev_key);
    if (err) {
        BT_ERR("bt_mesh_provision() failed (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_prov_type(ble_mesh_prov_type_t prov_type)
{
    if (prov_type == BLE_MESH_PROV_TYPE_SIG_MESH) {
        bt_mesh_pb_set_sig_mesh_prov(true);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    if (prov_type == BLE_MESH_PROV_TYPE_CUSTOM) {
        bt_mesh_pb_set_sig_mesh_prov(false);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    return ERRCODE_BLE_MESH_PARAM_ERR;
}

errcode_t ble_mesh_pb_stop_rmt_scan_local(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    bt_mesh_stop_rmt_scan();
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_pb_start_rmt_scan_local(uint16_t net_index, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN],
                                           uint8_t items_limit, uint8_t timeout)
{
    int ret = 0;
    uint8_t invalid_uuid[BLE_MESH_UUID_LEN] = {0};
    struct bt_mesh_msg_ctx ctx = {0};

    bt_mesh_rmt_prov_scan_start_t scan_start = {0};
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    scan_start.timeout = timeout;
    scan_start.scan_items_limit = items_limit;
    // 全0 UUID则不进行指定UUID扫描
    if (uuid != NULL && memcmp(uuid, invalid_uuid, BLE_MESH_UUID_LEN) != 0) {
        scan_start.uuid_len = BLE_MESH_UUID_LEN;
        (void)memcpy_s(scan_start.uuid, BLE_MESH_UUID_LEN, uuid, BLE_MESH_UUID_LEN);
    }
    
    ctx.addr = addr;
    ctx.net_idx = net_index;
    ctx.app_idx = BT_MESH_KEY_DEV;
    ret = bt_mesh_rmt_prov_scan_start(&ctx, &scan_start);
    if (ret != 0) {
        return ble_mesh_errcode_convert(ret);
    }
    
    return ERRCODE_BLE_MESH_SUCCESS;
}