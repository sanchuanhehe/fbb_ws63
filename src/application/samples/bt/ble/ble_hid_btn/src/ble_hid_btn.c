/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE HID Button — HID-over-GATT keyboard service (Boot mode).
 *              Registers Service 0x1812 with standard HID characteristics.
 *              Reference: HID over GATT Profile v1.0, §3.3.
 */

#include <stdint.h>
#include <string.h>
#include "securec.h"
#include "common_def.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "errcode.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "../inc/ble_hid_btn.h"
#include "../inc/ble_hid_adv.h"

#define BLE_HID_TAG "[ble_hid_btn]"

/* ---- UUIDs ---- */
#define BLE_UUID_HID_SERVICE 0x1812       /* Human Interface Device */
#define BLE_UUID_PROTOCOL_MODE 0x2A4E     /* Boot (0) / Report (1) */
#define BLE_UUID_REPORT_MAP 0x2A4B        /* HID Report Descriptor */
#define BLE_UUID_BOOT_KB_INPUT 0x2A22     /* Boot Keyboard Input Report */
#define BLE_UUID_BOOT_KB_OUTPUT 0x2A32    /* Boot Keyboard Output Report */
#define BLE_UUID_HID_INFORMATION 0x2A4A   /* HID version, country, flags */
#define BLE_UUID_HID_CONTROL_POINT 0x2A4C /* Suspend / Exit Suspend */
#define BLE_UUID_CCCD 0x2902              /* Client Characteristic Config */

#define UUID16_LEN 2
#define UUID16_HIGH_BYTE_SHIFT 8
#define HID_SERVER_ID 1

/* ---- HID Information (4 bytes) ---- */
/* bcdHID=1.11, bCountryCode=0, Flags=RemoteWake(1)+NormallyConnectable(2)=3 */
static const uint8_t HID_INFO[] = {0x11, 0x01, 0x00, 0x03};

/* ---- Protocol Mode (1 byte, default Boot) ---- */
static uint8_t g_protocol_mode = 0x00;

/* ---- Standard Keyboard Boot Report Descriptor (63 bytes, USB HID 1.11) ---- */
static const uint8_t REPORT_MAP[] = {
    0x05, 0x01, /* Usage Page (Generic Desktop)              */
    0x09, 0x06, /* Usage (Keyboard)                          */
    0xA1, 0x01, /* Collection (Application)                  */
    0x05, 0x07, /*   Usage Page (Keyboard/Keypad)            */
    0x19, 0xE0, /*   Usage Minimum (224)                    */
    0x29, 0xE7, /*   Usage Maximum (231)                    */
    0x15, 0x00, /*   Logical Minimum (0)                    */
    0x25, 0x01, /*   Logical Maximum (1)                    */
    0x75, 0x01, /*   Report Size (1)                        */
    0x95, 0x08, /*   Report Count (8)                       */
    0x81, 0x02, /*   Input (Data,Var,Abs) — 8 modifier bits */
    0x95, 0x01, /*   Report Count (1)                       */
    0x75, 0x08, /*   Report Size (8)                        */
    0x81, 0x03, /*   Input (Cnst,Var,Abs) — reserved byte   */
    0x95, 0x06, /*   Report Count (6)                       */
    0x75, 0x08, /*   Report Size (8)                        */
    0x15, 0x00, /*   Logical Minimum (0)                    */
    0x25, 0x65, /*   Logical Maximum (101)                  */
    0x05, 0x07, /*   Usage Page (Keyboard/Keypad)            */
    0x19, 0x00, /*   Usage Minimum (0)                      */
    0x29, 0x65, /*   Usage Maximum (101)                    */
    0x81, 0x00, /*   Input (Data,Ary,Abs) — 6 keycodes      */
    /* Output report (LEDs) */
    0x05, 0x01, /*   Usage Page (Generic Desktop)           */
    0x09, 0x06, /*   Usage (Keyboard)                       */
    0xA1, 0x01, /*   Collection (Application)               */
    0x05, 0x07, /*     Usage Page (Keyboard/Keypad)         */
    0x19, 0x01, /*     Usage Minimum (1)                    */
    0x29, 0x05, /*     Usage Maximum (5)                    */
    0x75, 0x01, /*     Report Size (1)                      */
    0x95, 0x05, /*     Report Count (5)                     */
    0x91, 0x02, /*     Output (Data,Var,Abs) — 5 LED bits   */
    0x95, 0x03, /*     Report Count (3)                     */
    0x91, 0x03, /*     Output (Cnst,Var,Abs)                */
    0xC0,       /*   End Collection                         */
    0xC0        /* End Collection                           */
};

/* ---- Internal state ---- */
static uint16_t g_conn_id = 0;
static uint8_t g_server_id = 0;
static uint16_t g_hid_srvc_handle = 0;
static uint16_t g_input_val_handle = 0; /* Boot KB Input value handle */
static bool g_connected = false;

typedef struct {
    uint16_t uuid;
    uint8_t properties;
    uint8_t permissions;
    const uint8_t *value;
    uint16_t value_len;
} ble_hid_characteristic_t;

/* ---- Helpers ---- */
static void uuid16_to_bt(uint16_t u16, bt_uuid_t *out)
{
    out->uuid_len = UUID16_LEN;
    out->uuid[0] = (uint8_t)(u16 >> UUID16_HIGH_BYTE_SHIFT);
    out->uuid[1] = (uint8_t)(u16);
}

/* =============================================================
 *  GATT callbacks
 * ============================================================= */

static void cbk_service_start(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("%s start service: srv=%d hdl=%d status=%d\r\n", BLE_HID_TAG, server_id, handle, status);
}

static void cbk_read_request(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *para, errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    osal_printk("%s read req: hdl=%d offset=%d status=%d\r\n", BLE_HID_TAG, para->handle, para->offset, status);
}

static void cbk_write_request(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *para, errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    osal_printk("%s write req: hdl=%d len=%d status=%d\r\n", BLE_HID_TAG, para->handle, para->length, status);
    /* Handle Protocol Mode write */
    if (para->length == 1 && para->value != NULL) {
        g_protocol_mode = para->value[0];
        osal_printk("%s protocol mode set to %d\r\n", BLE_HID_TAG, g_protocol_mode);
    }
}

static void cbk_mtu_changed(uint8_t server_id, uint16_t conn_id, uint16_t mtu, errcode_t status)
{
    osal_printk("%s mtu: srv=%d conn=%d mtu=%d st=%d\r\n", BLE_HID_TAG, server_id, conn_id, mtu, status);
}

/* ---- GAP callbacks ---- */

static void cbk_adv_enable(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv enable: id=%d st=%d\r\n", BLE_HID_TAG, adv_id, status);
}

static void cbk_conn_state_change(uint16_t conn_id,
                                  bd_addr_t *addr,
                                  gap_ble_conn_state_t conn_state,
                                  gap_ble_pair_state_t pair_state,
                                  gap_ble_disc_reason_t disc_reason)
{
    unused(addr);
    osal_printk("%s conn: id=%d state=%d pair=%d disc=%d\r\n", BLE_HID_TAG, conn_id, conn_state, pair_state,
                disc_reason);
    g_conn_id = conn_id;
    g_connected = (conn_state == GAP_BLE_STATE_CONNECTED);
    if (!g_connected) {
        osal_printk("%s PC disconnected, re-advertising...\r\n", BLE_HID_TAG);
        ble_hid_adv_restart();
    }
}

/* =============================================================
 *  Service registration
 * ============================================================= */

static errcode_t register_callbacks(void)
{
    errcode_t ret;

    gap_ble_callbacks_t gap_cb = {0};
    gap_cb.start_adv_cb = cbk_adv_enable;
    gap_cb.conn_state_change_cb = cbk_conn_state_change;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s gap cbk fail: %d\r\n", BLE_HID_TAG, ret);
        return ret;
    }

    gatts_callbacks_t svc_cb = {0};
    svc_cb.start_service_cb = cbk_service_start;
    svc_cb.read_request_cb = cbk_read_request;
    svc_cb.write_request_cb = cbk_write_request;
    svc_cb.mtu_changed_cb = cbk_mtu_changed;
    ret = gatts_register_callbacks(&svc_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s svc cbk fail: %d\r\n", BLE_HID_TAG, ret);
    }
    return ret;
}

/**
 * Add a characteristic without CCCD.
 */
static errcode_t add_chara(const ble_hid_characteristic_t *characteristic, uint16_t *out_val_handle)
{
    gatts_add_chara_info_t ch = {0};
    gatts_add_character_result_t res = {0};
    const uint8_t *value = characteristic->value;
    uuid16_to_bt(characteristic->uuid, &ch.chara_uuid);
    ch.properties = characteristic->properties;
    ch.permissions = characteristic->permissions;
    ch.value_len = characteristic->value_len;
    ch.value = (uint8_t *)value; /* const cast — stack won't mutate */
    errcode_t ret = gatts_add_characteristic_sync(g_server_id, g_hid_srvc_handle, &ch, &res);
    if (out_val_handle) {
        *out_val_handle = res.value_handle;
    }
    osal_printk("%s chara uuid=0x%04X val_hdl=%d ret=%d\r\n", BLE_HID_TAG, characteristic->uuid, res.value_handle, ret);
    return ret;
}

/**
 * Add a characteristic with a CCCD descriptor (for Notify-capable chars).
 */
static errcode_t add_chara_with_cccd(const ble_hid_characteristic_t *characteristic,
                                     uint16_t *out_val_handle,
                                     uint16_t *out_cccd_handle)
{
    errcode_t ret = add_chara(characteristic, out_val_handle);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    uint8_t cccd_val[2] = {0x00, 0x00};
    gatts_add_desc_info_t desc = {0};
    uuid16_to_bt(BLE_UUID_CCCD, &desc.desc_uuid);
    desc.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    desc.value_len = sizeof(cccd_val);
    desc.value = cccd_val;

    uint16_t dummy;
    ret =
        gatts_add_descriptor_sync(g_server_id, g_hid_srvc_handle, &desc, (out_cccd_handle ? out_cccd_handle : &dummy));
    return ret;
}

/**
 * Build the HID service tree.
 */
static errcode_t add_hid_metadata(void)
{
    errcode_t ret = add_chara(&(ble_hid_characteristic_t) {BLE_UUID_HID_INFORMATION,
                                                          GATT_CHARACTER_PROPERTY_BIT_READ,
                                                          GATT_ATTRIBUTE_PERMISSION_READ, HID_INFO, sizeof(HID_INFO)},
                              NULL);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    uint8_t control_point = 0;
    return add_chara(&(ble_hid_characteristic_t) {BLE_UUID_HID_CONTROL_POINT,
                                                 GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
                                                 GATT_ATTRIBUTE_PERMISSION_WRITE, &control_point,
                                                 sizeof(control_point)},
                     NULL);
}

static errcode_t add_hid_reports(void)
{
    uint8_t zero_report[sizeof(hid_kb_report_t)] = {0};
    errcode_t ret = add_chara_with_cccd(
        &(ble_hid_characteristic_t) {BLE_UUID_BOOT_KB_INPUT,
                                    GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY,
                                    GATT_ATTRIBUTE_PERMISSION_READ, zero_report, sizeof(zero_report)},
        &g_input_val_handle, NULL);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    return add_chara(
        &(ble_hid_characteristic_t) {
            BLE_UUID_BOOT_KB_OUTPUT,
            GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE |
                GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
            GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE,
            zero_report,
            1,
        },
        NULL);
}

static errcode_t add_hid_protocol(void)
{
    errcode_t ret = add_chara(
        &(ble_hid_characteristic_t) {
            BLE_UUID_PROTOCOL_MODE,
            GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
            GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE,
            &g_protocol_mode,
            sizeof(g_protocol_mode),
        },
        NULL);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    return add_chara(&(ble_hid_characteristic_t) {BLE_UUID_REPORT_MAP, GATT_CHARACTER_PROPERTY_BIT_READ,
                                                 GATT_ATTRIBUTE_PERMISSION_READ, REPORT_MAP, sizeof(REPORT_MAP)},
                     NULL);
}

static errcode_t build_hid_service(void)
{
    bt_uuid_t service_uuid;
    uuid16_to_bt(BLE_UUID_HID_SERVICE, &service_uuid);
    errcode_t ret = gatts_add_service_sync(HID_SERVER_ID, &service_uuid, true, &g_hid_srvc_handle);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s add svc fail: %d\r\n", BLE_HID_TAG, ret);
        return ret;
    }
    osal_printk("%s svc hdl=%d\r\n", BLE_HID_TAG, g_hid_srvc_handle);
    if (((ret = add_hid_protocol()) != ERRCODE_BT_SUCCESS) || ((ret = add_hid_reports()) != ERRCODE_BT_SUCCESS) ||
        ((ret = add_hid_metadata()) != ERRCODE_BT_SUCCESS)) {
        return ret;
    }
    gatts_start_service(g_server_id, g_hid_srvc_handle);
    return ERRCODE_BT_SUCCESS;
}

/* =============================================================
 *  Public API
 * ============================================================= */

errcode_t ble_hid_btn_init(void)
{
    osal_printk("%s init begin\r\n", BLE_HID_TAG);

    osal_printk("%s registering callbacks\r\n", BLE_HID_TAG);
    if (register_callbacks() != ERRCODE_BT_SUCCESS) {
        return ERRCODE_FAIL;
    }

    osal_printk("%s enabling BLE\r\n", BLE_HID_TAG);
    enable_ble();

    osal_printk("%s set name '%s'\r\n", BLE_HID_TAG, CONFIG_BLE_HID_DEVICE_NAME);
    gap_ble_set_local_name((const uint8_t *)CONFIG_BLE_HID_DEVICE_NAME, sizeof(CONFIG_BLE_HID_DEVICE_NAME) - 1);

    /* Register GATT server */
    bt_uuid_t app_uuid = {UUID16_LEN, {0x00, 0x00}};
    osal_printk("%s reg server\r\n", BLE_HID_TAG);
    errcode_t ret = gatts_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_BT_SUCCESS || g_server_id == 0) {
        osal_printk("%s reg server fail: %d\r\n", BLE_HID_TAG, ret);
        return ERRCODE_FAIL;
    }
    osal_printk("%s server_id=%d\r\n", BLE_HID_TAG, g_server_id);

    /* Build HID service tree */
    osal_printk("%s building service\r\n", BLE_HID_TAG);
    ret = build_hid_service();
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    osal_printk("%s init done\r\n", BLE_HID_TAG);
    return ERRCODE_SUCC;
}

errcode_t ble_hid_btn_send_report(const hid_kb_report_t *report)
{
    if (!g_connected || report == NULL) {
        return ERRCODE_FAIL;
    }

    gatts_ntf_ind_t param = {0};
    param.attr_handle = g_input_val_handle;
    param.value = (uint8_t *)report;
    param.value_len = sizeof(hid_kb_report_t);
    gatts_notify_indicate(g_server_id, g_conn_id, &param);
    return ERRCODE_SUCC;
}

bool ble_hid_btn_is_connected(void)
{
    return g_connected;
}
