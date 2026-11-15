/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: ble mesh manager source
 *
 * History:
 * 2025-6-21, Create file.
 */
#include <stdbool.h>
#include <stdint.h>

#include "mesh/glue.h"
#include "mesh/access.h"

#include "adapter.h"

#include "bts_def.h"
#include "ble_mesh_model_inner.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_prov_inner.h"
#include "bt_mesh_nv_task.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_manager.h"

static bool g_mesh_enabled = false;
static bool g_mesh_manager_inited = false;
static ble_mesh_prov_bearer_t g_mesh_bearers = 0;

static errcode_t ble_mesh_manager_init(uint8_t uuid[BLE_MESH_UUID_LEN], const ble_mesh_prov_t *prov)
{
    errcode_t err = ERRCODE_BLE_MESH_SUCCESS;

    if (g_mesh_manager_inited) {
        BT_WARN("mesh already initialized.");
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    BT_INFO("");
    err = ble_mesh_adapter_init();
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to ble mesh adapter init (err %d)", err);
        return ERRCODE_BLE_MESH_STATUS_ERR;
    }

    // 初始化model
    ble_mesh_model_init();

    // 初始化配网管理
    err = ble_mesh_prov_init(uuid, prov);
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to initialize mesh stack (err %d)", err);
        return err;
    }

    struct bt_mesh_comp *mesh_comp = ble_mesh_get_comp();
    struct bt_mesh_prov *provision = ble_mesh_get_provision();
    
    err = bt_mesh_init(BLE_ADDR_RANDOM, provision, mesh_comp);
    if (err != 0) {
        BT_ERR("Failed to initialize mesh stack (err %d)", err);
        return ble_mesh_errcode_convert(err);
    }

    bt_mesh_nv_write_task_init();
    g_mesh_manager_inited = true;
    return ERRCODE_BLE_MESH_SUCCESS;
}

static errcode_t ble_mesh_manager_deinit(void)
{
    errcode_t err = ERRCODE_BLE_MESH_SUCCESS;
    err = ble_mesh_adapter_deinit();
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO("Failed to deinit adapter (err %d)", err);
    }

    err = ble_mesh_prov_deinit();
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO("Failed to deinit mesh prov (err %d)", err);
    }

    err = bt_mesh_deinit();
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO("Failed to deinit mesh stack (err %d)", err);
    }

    err = bt_mesh_nv_write_task_deinit();
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_INFO("Failed to deinit nv write task : (err %d)", err);
    }

    g_mesh_manager_inited = false;
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_enable(uint8_t uuid[BLE_MESH_UUID_LEN], const ble_mesh_prov_t *prov, ble_mesh_prov_bearer_t bearers)
{
    errcode_t ret = ERRCODE_BLE_MESH_SUCCESS;

    if (g_mesh_enabled) {
        BT_INFO("mesh already enabled.");
        return ERRCODE_BLE_MESH_STATUS_ERR;
    }

    ret = ble_mesh_manager_init(uuid, prov);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to initialize mesh stack (err 0x%x)", ret);
        return ret;
    }

    ret = ble_mesh_prov_enable(bearers);
    if (ret != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("Failed to enable mesh node (err 0x%x)", ret);
        ble_mesh_manager_deinit();
        return ret;
    }
    g_mesh_enabled = true;
    g_mesh_bearers = bearers;
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_disable(void)
{
    errcode_t ret = ERRCODE_BLE_MESH_SUCCESS;
    if (!g_mesh_enabled) {
        BT_WARN("mesh already disabled.");
        return ERRCODE_BLE_MESH_SUCCESS;
    }
    ble_mesh_prov_disable();

    g_mesh_enabled = false;

    ble_mesh_manager_deinit();

    return ERRCODE_BLE_MESH_SUCCESS;
}

bool ble_mesh_is_enable(void)
{
    return g_mesh_enabled;
}