/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE HID Button — advertising setup using public bts_le_gap.h API.
 */

#include <stdint.h>
#include <string.h>
#include "securec.h"
#include "osal_debug.h"
#include "errcode.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "../inc/ble_hid_adv.h"

#define BLE_HID_ADV_TAG "[ble_hid_adv]"

#ifndef CONFIG_BLE_HID_DEVICE_NAME
#define CONFIG_BLE_HID_DEVICE_NAME "ble_hid_btn"
#endif

#define BLE_ADV_ID 0x01
#define FOREVER 0
#define BLE_ADV_DATA_MAX_LEN 31
#define BLE_AD_ELEMENT_HEADER_LEN 2
#define BLE_AD_TYPE_COMPLETE_LOCAL_NAME 0x09

/* ADV data (max 31 bytes):
 *  02 01 06 : Flags
 *  03 03 12 18 : Complete 16-bit Service UUIDs (HID 0x1812)
 *  04 19 C1 03 : Appearance (Keyboard 0x03C1) */
static uint8_t g_adv_data[] = {
    0x02, 0x01, 0x06, 0x03, 0x03, 0x12, 0x18, 0x04, 0x19, 0xC1, 0x03,
};

static uint8_t g_device_name[] = CONFIG_BLE_HID_DEVICE_NAME;

static void build_scan_rsp(uint8_t *buf, uint16_t buf_len, uint16_t *out_len)
{
    uint16_t idx = 0;
    uint16_t name_len = sizeof(g_device_name);
    if (name_len + BLE_AD_ELEMENT_HEADER_LEN > buf_len) {
        *out_len = 0;
        return;
    }
    buf[idx++] = (uint8_t)(name_len + 1);
    buf[idx++] = BLE_AD_TYPE_COMPLETE_LOCAL_NAME;
    (void)memcpy_s(&buf[idx], buf_len - idx, g_device_name, name_len);
    *out_len = idx + (uint16_t)name_len;
}

void ble_hid_adv_start(void)
{
    uint8_t scan_rsp[BLE_ADV_DATA_MAX_LEN] = {0};
    uint16_t rsp_len = 0;
    errcode_t ret;

    osal_printk("%s start\r\n", BLE_HID_ADV_TAG);

    build_scan_rsp(scan_rsp, sizeof(scan_rsp), &rsp_len);

    gap_ble_config_adv_data_t cfg = {0};
    cfg.adv_data = g_adv_data;
    cfg.adv_length = sizeof(g_adv_data);
    cfg.scan_rsp_data = scan_rsp;
    cfg.scan_rsp_length = rsp_len;
    ret = gap_ble_set_adv_data(BLE_ADV_ID, &cfg);
    if (ret) {
        osal_printk("%s set data fail %d\r\n", BLE_HID_ADV_TAG, ret);
        return;
    }

    gap_ble_adv_params_t param = {0};
    param.min_interval = 0x20; /* 20 ms */
    param.max_interval = 0x30; /* 30 ms */
    param.duration = FOREVER;
    param.adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    param.channel_map = 0x07; /* channels 37/38/39 */
    param.adv_filter_policy = GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    param.peer_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;
    (void)memset_s(param.peer_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);

    ret = gap_ble_set_adv_param(BLE_ADV_ID, &param);
    if (ret) {
        osal_printk("%s set param fail %d\r\n", BLE_HID_ADV_TAG, ret);
        return;
    }

    ret = gap_ble_start_adv(BLE_ADV_ID);
    osal_printk("%s started ret=%d\r\n", BLE_HID_ADV_TAG, ret);
}

void ble_hid_adv_restart(void)
{
    osal_printk("%s restart\r\n", BLE_HID_ADV_TAG);
    ble_hid_adv_start();
}
