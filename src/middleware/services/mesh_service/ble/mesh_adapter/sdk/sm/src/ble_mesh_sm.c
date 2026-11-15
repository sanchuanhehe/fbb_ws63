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
#include "mesh/access.h"
#include "mesh/main.h"

#include "ble_mesh_common.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_sm.h"

errcode_t ble_mesh_cfg_mod_bind_app_key(const ble_mesh_prov_net_info_t *net_info, uint16_t mod_id, uint8_t *status)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err = bt_mesh_cfg_mod_app_bind(
        net_info->net_idx,
        net_info->addr,
        net_info->elem_addr,
        net_info->key_app_idx,
        mod_id,
        status);
    if (err != 0) {
        BT_INFO("failed err %d\r\n", err);
        return ble_mesh_errcode_convert(err);
    }
    if (*status != 0x00) {
        BT_INFO("status 0x%02x\r\n", *status);
        return ERRCODE_BLE_MESH_STATUS_ERR;
    }
    
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_cfg_vnd_mod_bind_app_key(
    const ble_mesh_prov_net_info_t *net_info,
    uint16_t mod_id,
    uint16_t cid,
    uint8_t *status)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err =  bt_mesh_cfg_mod_app_bind_vnd(
        net_info->net_idx,
        net_info->addr,
        net_info->elem_addr,
        net_info->key_app_idx,
        mod_id,
        cid,
        status);
    if (err != 0) {
        BT_INFO(" failed err %d\r\n", err);
        return ble_mesh_errcode_convert(err);
    }
    if (*status != 0x00) {
        BT_INFO(" status 0x%02x\r\n", *status);
        return ERRCODE_BLE_MESH_STATUS_ERR;
    }
    
    return ERRCODE_BLE_MESH_SUCCESS;
}


errcode_t ble_mesh_cfg_add_app_key(const ble_mesh_prov_net_info_t *net_info, uint8_t *status)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    int err = bt_mesh_cfg_app_key_add(
        net_info->net_idx,
        net_info->addr,
        net_info->key_net_idx,
        net_info->key_app_idx,
        net_info->app_key,
        status);
    if (err) {
        BT_INFO("Unable to send App Key Add (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }

    if (*status) {
        BT_ERR("AppKeyAdd failed with status 0x%02x", status);
    } else {
        BT_INFO("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x", net_info->key_net_idx, net_info->key_app_idx);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_cfg_add_local_app_key(uint16_t app_idx, uint16_t net_idx,
                                         const uint8_t app_key[BLE_MESH_APP_KEY_LEN])
{
    uint8_t err = 0;

    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    err = bt_mesh_app_key_add(app_idx, net_idx, app_key);
    if (err) {
        BT_ERR("app key add fail, err = %u", err);
        return ble_mesh_errcode_convert(err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_cfg_update_dev_key(const uint8_t dev_key[BLE_MESH_DEV_KEY_LEN], bool active)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    bt_mesh_dev_key_cand(dev_key, BLE_MESH_DEV_KEY_LEN);

    if (active) {
        bt_mesh_dev_key_cand_activate();
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}