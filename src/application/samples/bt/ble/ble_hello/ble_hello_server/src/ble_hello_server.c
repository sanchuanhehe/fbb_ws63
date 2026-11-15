/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello GATT server.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "ble_hello_server.h"
#include "ble_hello_server_adv.h"

#define BLE_HELLO_SERVER_LOG "[ble hello server]"
#define BLE_HELLO_UUID_LEN 2

static uint8_t g_server_id;
static uint16_t g_conn_id;
static uint16_t g_service_handle;
static uint16_t g_data_handle;
static uint16_t g_notify_handle;
static uint16_t g_notify_cccd_handle;
static bool g_connected;
static bool g_hello_notify_enabled;
static bool g_stack_reset_done;
static uint8_t g_property_value[BLE_HELLO_PROPERTY_MAX_LEN] = "device_status_ok";
static uint16_t g_property_value_len = sizeof("device_status_ok") - 1;
static const uint8_t DEFAULT_VALUE[] = "device_status_ok";
static const uint8_t HELLO_MESSAGE[] = "hello world";
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_CCCD_VALUE_LEN 2
#define BLE_CCCD_NOTIFY_ENABLED 1

typedef struct {
    uint16_t request_id;
    uint8_t status;
    uint8_t *value;
    uint16_t value_len;
} ble_hello_response_t;

static errcode_t ble_hello_send_value_notification(uint16_t handle,
                                                   const uint8_t *data,
                                                   uint16_t len,
                                                   const char *name);

static void ble_hello_uuid16(uint16_t value, bt_uuid_t *uuid)
{
    uuid->uuid_len = BLE_HELLO_UUID_LEN;
    uuid->uuid[0] = (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT);
    uuid->uuid[1] = (uint8_t)value;
}

static errcode_t ble_hello_send_response(uint8_t server_id, uint16_t conn_id, const ble_hello_response_t *response_data)
{
    gatts_send_rsp_t response = {0};
    response.request_id = response_data->request_id;
    response.status = response_data->status;
    response.offset = 0;
    response.value = response_data->value;
    response.value_len = response_data->value_len;
    return gatts_send_response(server_id, conn_id, &response);
}

static void ble_hello_read_request_cb(uint8_t server_id,
                                      uint16_t conn_id,
                                      gatts_req_read_cb_t *request,
                                      errcode_t status)
{
    (void)status;
    osal_printk("%s read request received, handle=0x%04x\r\n", BLE_HELLO_SERVER_LOG, request->handle);
    if (!request->need_rsp) {
        return;
    }

    if (request->handle != g_data_handle) {
        ble_hello_response_t response = {request->request_id, GATT_STATUS_INVALID_HANDLE, NULL, 0};
        (void)ble_hello_send_response(server_id, conn_id, &response);
        return;
    }

    ble_hello_response_t response = {request->request_id, GATT_STATUS_SUCCESS, g_property_value, g_property_value_len};
    if (ble_hello_send_response(server_id, conn_id, &response) == ERRCODE_BT_SUCCESS) {
        osal_printk("%s read response sent: value=%.*s\r\n", BLE_HELLO_SERVER_LOG, g_property_value_len,
                    g_property_value);
    }
}

static void ble_hello_handle_cccd_write(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *request)
{
    uint16_t cccd_value = 0;
    uint8_t response_status = GATT_STATUS_SUCCESS;

    if (request->length != BLE_CCCD_VALUE_LEN) {
        response_status = GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH;
    } else {
        cccd_value = (uint16_t)request->value[0] | ((uint16_t)request->value[1] << BLE_UUID_HIGH_BYTE_SHIFT);
        if (cccd_value != 0 && cccd_value != 1) {
            response_status = GATT_STATUS_VALUE_NOT_ALLOWED;
        }
    }

    if (request->need_rsp) {
        ble_hello_response_t response = {request->request_id, response_status, NULL, 0};
        (void)ble_hello_send_response(server_id, conn_id, &response);
    }
    if (response_status != GATT_STATUS_SUCCESS) {
        osal_printk("%s invalid CCCD write, status=0x%x\r\n", BLE_HELLO_SERVER_LOG, response_status);
        return;
    }

    g_hello_notify_enabled = (cccd_value == BLE_CCCD_NOTIFY_ENABLED);
    osal_printk("%s hello CCCD %s\r\n", BLE_HELLO_SERVER_LOG, g_hello_notify_enabled ? "enabled" : "disabled");
    if (g_hello_notify_enabled) {
        (void)ble_hello_server_send_notification(HELLO_MESSAGE, sizeof(HELLO_MESSAGE) - 1);
    }
}

static void ble_hello_write_request_cb(uint8_t server_id,
                                       uint16_t conn_id,
                                       gatts_req_write_cb_t *request,
                                       errcode_t status)
{
    uint8_t response_status = GATT_STATUS_SUCCESS;
    (void)status;

    osal_printk("%s write request received, handle=0x%04x, len=%u\r\n", BLE_HELLO_SERVER_LOG, request->handle,
                request->length);
    if (request->handle == g_notify_cccd_handle) {
        ble_hello_handle_cccd_write(server_id, conn_id, request);
        return;
    }
    if (request->handle != g_data_handle) {
        response_status = GATT_STATUS_INVALID_HANDLE;
    } else if (request->length == 0 || request->length >= BLE_HELLO_PROPERTY_MAX_LEN) {
        response_status = GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH;
    } else {
        (void)memset_s(g_property_value, sizeof(g_property_value), 0, sizeof(g_property_value));
        if (memcpy_s(g_property_value, sizeof(g_property_value), request->value, request->length) != EOK) {
            response_status = GATT_STATUS_UNLIKELY_ERROR;
        } else {
            g_property_value_len = request->length;
        }
    }

    if (request->need_rsp) {
        ble_hello_response_t response = {request->request_id, response_status, NULL, 0};
        (void)ble_hello_send_response(server_id, conn_id, &response);
    }
    if (response_status == GATT_STATUS_SUCCESS) {
        ble_hello_server_set_adv_default_state(g_property_value_len == sizeof(DEFAULT_VALUE) - 1 &&
                                               memcmp(g_property_value, DEFAULT_VALUE, sizeof(DEFAULT_VALUE) - 1) == 0);
        osal_printk("%s property updated: %.*s\r\n", BLE_HELLO_SERVER_LOG, g_property_value_len, g_property_value);
        osal_printk("%s write response sent: success\r\n", BLE_HELLO_SERVER_LOG);
    } else {
        osal_printk("%s write rejected, status=0x%x\r\n", BLE_HELLO_SERVER_LOG, response_status);
    }
}

static errcode_t ble_hello_add_data_characteristic(void)
{
    bt_uuid_t data_uuid = {0};
    gatts_add_chara_info_t characteristic = {0};
    gatts_add_character_result_t data_result = {0};

    ble_hello_uuid16(BLE_HELLO_DATA_UUID, &data_uuid);
    characteristic.chara_uuid = data_uuid;
    characteristic.properties = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE;
    characteristic.permissions =
        GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE | GATT_ATTRIBUTE_PERMISSION_AUTHORIZATION_NEED;
    characteristic.value = g_property_value;
    characteristic.value_len = g_property_value_len;
    errcode_t ret = gatts_add_characteristic_sync(g_server_id, g_service_handle, &characteristic, &data_result);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    g_data_handle = data_result.value_handle;
    return ERRCODE_BT_SUCCESS;
}

static errcode_t ble_hello_add_notify_characteristic(void)
{
    bt_uuid_t notify_uuid = {0};
    bt_uuid_t cccd_uuid = {0};
    gatts_add_chara_info_t characteristic = {0};
    gatts_add_character_result_t notify_result = {0};
    gatts_add_desc_info_t descriptor = {0};
    uint8_t cccd_value[BLE_CCCD_VALUE_LEN] = {0};

    ble_hello_uuid16(BLE_HELLO_NOTIFY_UUID, &notify_uuid);
    characteristic.chara_uuid = notify_uuid;
    characteristic.properties = GATT_CHARACTER_PROPERTY_BIT_NOTIFY;
    characteristic.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    characteristic.value = (uint8_t *)HELLO_MESSAGE;
    characteristic.value_len = sizeof(HELLO_MESSAGE) - 1;
    errcode_t ret = gatts_add_characteristic_sync(g_server_id, g_service_handle, &characteristic, &notify_result);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    g_notify_handle = notify_result.value_handle;

    ble_hello_uuid16(BLE_HELLO_CCCD_UUID, &cccd_uuid);
    descriptor.desc_uuid = cccd_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value = cccd_value;
    descriptor.value_len = sizeof(cccd_value);
    ret = gatts_add_descriptor_sync(g_server_id, g_service_handle, &descriptor, &g_notify_cccd_handle);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    return ERRCODE_BT_SUCCESS;
}

static errcode_t ble_hello_add_gatt_service(void)
{
    bt_uuid_t app_uuid = {0};
    bt_uuid_t service_uuid = {0};

    ble_hello_uuid16(BLE_HELLO_SERVICE_UUID, &app_uuid);
    errcode_t ret = gatts_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    ble_hello_uuid16(BLE_HELLO_SERVICE_UUID, &service_uuid);
    ret = gatts_add_service_sync(g_server_id, &service_uuid, true, &g_service_handle);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    ret = ble_hello_add_data_characteristic();
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    ret = ble_hello_add_notify_characteristic();
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    osal_printk("%s service ready: service=0x%04x data=0x%04x notify=0x%04x notify_cccd=0x%04x\r\n",
                BLE_HELLO_SERVER_LOG, g_service_handle, g_data_handle, g_notify_handle, g_notify_cccd_handle);
    return gatts_start_service(g_server_id, g_service_handle);
}

static void ble_hello_service_start_cb(uint8_t server_id, uint16_t handle, errcode_t status)
{
    if (server_id != g_server_id || handle != g_service_handle) {
        return;
    }
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s service start failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, status);
        return;
    }
    if (ble_hello_server_start_adv() == ERRCODE_BT_SUCCESS) {
        osal_printk("%s advertising started: ble_hello_server\r\n", BLE_HELLO_SERVER_LOG);
    } else {
        osal_printk("%s advertising start failed\r\n", BLE_HELLO_SERVER_LOG);
    }
}

static void ble_hello_conn_state_cb(uint16_t conn_id,
                                    bd_addr_t *addr,
                                    gap_ble_conn_state_t conn_state,
                                    gap_ble_pair_state_t pair_state,
                                    gap_ble_disc_reason_t reason)
{
    (void)addr;
    (void)pair_state;
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        g_conn_id = conn_id;
        g_connected = true;
        g_hello_notify_enabled = false;
        osal_printk("%s connected, conn_id=0x%04x\r\n", BLE_HELLO_SERVER_LOG, conn_id);
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        g_connected = false;
        g_hello_notify_enabled = false;
        osal_printk("%s disconnected, reason=0x%x, re-advertising\r\n", BLE_HELLO_SERVER_LOG, reason);
        (void)ble_hello_server_start_adv();
    }
}

static void ble_hello_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete, conn_id=0x%04x, status=0x%x\r\n", BLE_HELLO_SERVER_LOG, conn_id, status);
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s remove stale pair, ret=0x%x\r\n", BLE_HELLO_SERVER_LOG, gap_ble_remove_pair(addr));
    }
}

static void ble_hello_enable_cb(errcode_t status)
{
    gap_ble_sec_params_t security = {0};
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, status);
        return;
    }
    if (!g_stack_reset_done) {
        g_stack_reset_done = true;
        osal_printk("%s cycling BLE stack to clear retained GATT state\r\n", BLE_HELLO_SERVER_LOG);
        ret = disable_ble();
        if (ret == ERRCODE_BT_SUCCESS) {
            return;
        }
        osal_printk("%s stack reset request failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, ret);
    }
    ble_hello_server_set_adv_default_state(g_property_value_len == sizeof(DEFAULT_VALUE) - 1 &&
                                           memcmp(g_property_value, DEFAULT_VALUE, sizeof(DEFAULT_VALUE) - 1) == 0);
    security.bondable = 1;
    security.io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT;
    security.sc_enable = 0;
    security.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    ret = gap_ble_set_sec_param(&security);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s security config failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, ret);
        return;
    }
    ret = ble_hello_add_gatt_service();
    osal_printk("%s init %s, ret=0x%x\r\n", BLE_HELLO_SERVER_LOG, ret == ERRCODE_BT_SUCCESS ? "ok" : "failed", ret);
}

static void ble_hello_disable_cb(errcode_t status)
{
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s disable failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, status);
        return;
    }
    osal_printk("%s BLE stack reset complete, enabling BLE\r\n", BLE_HELLO_SERVER_LOG);
    ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable request failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, ret);
    }
}

static errcode_t ble_hello_register_callbacks(void)
{
    gap_ble_callbacks_t gap_callbacks = {0};
    gatts_callbacks_t gatt_callbacks = {0};
    errcode_t ret;

    gap_callbacks.ble_enable_cb = ble_hello_enable_cb;
    gap_callbacks.ble_disable_cb = ble_hello_disable_cb;
    gap_callbacks.conn_state_change_cb = ble_hello_conn_state_cb;
    gap_callbacks.pair_result_cb = ble_hello_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    gatt_callbacks.start_service_cb = ble_hello_service_start_cb;
    gatt_callbacks.read_request_cb = ble_hello_read_request_cb;
    gatt_callbacks.write_request_cb = ble_hello_write_request_cb;
    return gatts_register_callbacks(&gatt_callbacks);
}

static errcode_t ble_hello_send_value_notification(uint16_t handle, const uint8_t *data, uint16_t len, const char *name)
{
    gatts_ntf_ind_t notification = {0};
    errcode_t ret;
    if (!g_connected || data == NULL || len == 0) {
        return ERRCODE_BT_FAIL;
    }
    notification.attr_handle = handle;
    notification.value = (uint8_t *)data;
    notification.value_len = len;
    ret = gatts_notify_indicate(g_server_id, g_conn_id, &notification);
    if (ret == ERRCODE_BT_SUCCESS) {
        osal_printk("%s %s sent: %.*s\r\n", BLE_HELLO_SERVER_LOG, name, len, data);
    } else {
        osal_printk("%s %s failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, name, ret);
    }
    return ret;
}

errcode_t ble_hello_server_send_notification(const uint8_t *data, uint16_t len)
{
    if (!g_hello_notify_enabled) {
        return ERRCODE_BT_FAIL;
    }
    return ble_hello_send_value_notification(g_notify_handle, data, len, "notification");
}

errcode_t ble_hello_server_init(void)
{
    errcode_t ret = ble_hello_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s callback registration failed: 0x%x\r\n", BLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    if (ble_is_enable()) {
        g_stack_reset_done = true;
        osal_printk("%s BLE already enabled, resetting stack\r\n", BLE_HELLO_SERVER_LOG);
        return disable_ble();
    }
    osal_printk("%s enabling BLE\r\n", BLE_HELLO_SERVER_LOG);
    return enable_ble();
}
