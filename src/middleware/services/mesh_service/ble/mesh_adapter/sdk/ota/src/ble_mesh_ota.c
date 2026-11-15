/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh model.
 *
 * History:
 * 2025-4-15, Create file.
 */
 
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "mesh/glue.h"
#include "dfu_server.h"
#include "dfu_bank.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_nv_op.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_ota.h"

static ble_mesh_ota_register_callbacks_t g_ota_cbks = {0};

errcode_t ble_mesh_ota_get_upgrade_flag(uint8_t *upg_flag)
{
    errcode_t err = 0;
    uint16_t len = 0;
    uint8_t ota_upg_flag = 0;
    if (upg_flag == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    err = ble_mesh_data_read(BTH_BLE_MESH_OTA_FLAG_NV_ID, sizeof(uint32_t), &len, &ota_upg_flag);
    if (err) {
        BT_ERR("Failed reading ota upg: 0x%x", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    *upg_flag = ota_upg_flag;
 
    return ERRCODE_BLE_MESH_SUCCESS;
}
 
errcode_t ble_mesh_ota_clear_upgrade_flag(void)
{
    uint8_t ota_upg = 0;
    errcode_t err = 0;
 
    err = ble_mesh_data_write(BTH_BLE_MESH_OTA_FLAG_NV_ID, &ota_upg, sizeof(ota_upg));
    if (err) {
        BT_ERR("Failed write ota upg: %d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_ota_add_firmware(const ble_mesh_dfu_firmware_info_t *fw_info)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (fw_info == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    int err = 0;
    struct bt_mesh_dfu_bank_t *bank = bt_mesh_dfu_bank_reserve();
    if (!bank) {
        BT_INFO("Failed to reserve bank.");
        return ERRCODE_BLE_MESH_RESOURCE_INSUFFICIENT;
    }
 
    err = bt_mesh_dfu_bank_fwid_set(bank, fw_info->fwid, fw_info->fwid_len);
    if (err != 0) {
        BT_ERR("fwid set failed:%d", err);
        return ble_mesh_errcode_convert(err);
    }
 
    err = bt_mesh_dfu_bank_info_set(bank, fw_info->size, fw_info->metadata, fw_info->metadata_len);
    if (err != 0) {
        BT_ERR("meta info set failed:%d", err);
        return ble_mesh_errcode_convert(err);
    }
    
    err = bt_mesh_dfu_bank_commit(bank);
    if (err != 0) {
        BT_INFO("Failed to commit bank: %d", err);
        bt_mesh_dfu_bank_release(bank);
        return ble_mesh_errcode_convert(err);
    }
 
    BT_INFO("bank added. Index: %u", bt_mesh_dfu_bank_img_idx_get(bank));
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_ota_del_all_firmware(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    bt_mesh_dfu_bank_del_all();
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_ota_register_callbacks(const ble_mesh_ota_register_callbacks_t *func)
{
    if (func == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    (void)memcpy_s(&g_ota_cbks, sizeof(ble_mesh_ota_register_callbacks_t),
                   func, sizeof(ble_mesh_ota_register_callbacks_t));
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_ota_set_target_fwid(const uint8_t *fwid, uint8_t fwid_len)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    if (fwid == NULL || fwid_len < sizeof(uint32_t)) {
        BT_ERR("para not invalid:%p len :%d", fwid, fwid_len);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    int err = bt_mesh_set_target_fwid(fwid, fwid_len);
    return ble_mesh_errcode_convert(err);
}

ble_mesh_firmware_update_phase_t ble_mesh_ota_get_firmware_update_status(void)
{
    return bt_mesh_get_dfu_status();
}

bool ble_mesh_ota_reboot_check(void)
{
    if (g_ota_cbks.upgrade_check_cb != NULL) {
        return g_ota_cbks.upgrade_check_cb();
    }
    return true;
}

bool ble_mesh_ota_metadata_check(const uint8_t *metadata, uint8_t len)
{
    if (g_ota_cbks.metadata_check_cb != NULL) {
        return g_ota_cbks.metadata_check_cb(metadata, len);
    }
    return true;
}

void ble_mesh_ota_upgrade_status_report(ble_mesh_ota_upgrade_status_t upgrade_status)
{
    if (g_ota_cbks.ota_upgrade_status_cb != NULL) {
        g_ota_cbks.ota_upgrade_status_cb(upgrade_status);
    }
}