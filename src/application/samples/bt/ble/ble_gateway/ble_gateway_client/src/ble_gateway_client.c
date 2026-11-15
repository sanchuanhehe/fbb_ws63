/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello GATT client.
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
#include "bts_gatt_client.h"
#include "ble_gateway_protocol.h"
#include "ble_gateway_client.h"
#include "ble_gateway_iotda.h"
#include "ble_gateway_wifi.h"

#define BLE_GATEWAY_CLIENT_LOG "[ble mqtt gateway]"
#define BLE_GATEWAY_SERVICE_UUID 0x3333
#define BLE_GATEWAY_DATA_UUID 0x3434
#define BLE_GATEWAY_NOTIFY_UUID 0x3435
#define BLE_GATEWAY_CCCD_UUID 0x2902
#define BLE_GATEWAY_UUID_LEN 2
#define BLE_GATEWAY_SCAN_INTERVAL 0x30
#define BLE_GATEWAY_AD_COMPLETE_NAME 0x09
#define BLE_GATEWAY_AD_SERVICE_DATA16 0x16
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_AD_HEADER_LEN 2
#define BLE_AD_SERVICE_DATA_FIELD_LEN 4
#define BLE_AD_UUID_LOW_OFFSET 2
#define BLE_AD_UUID_HIGH_OFFSET 3
#define BLE_GATEWAY_CONTROL_TASK_STACK_SIZE 0x1000
#define BLE_GATEWAY_CONTROL_TASK_PRIO 25
#define BLE_GATEWAY_CONTROL_POLL_MS 50
#define BLE_GATEWAY_SCAN_RESTART_TICKS 100
#define BLE_GATEWAY_DISCOVERY_DELAY_TICKS 10
#define BLE_GATEWAY_DISCOVERY_TIMEOUT_TICKS 60
#define BLE_GATEWAY_DISCOVERY_MAX_RETRIES 3
#define BLE_GATEWAY_FIXED_SERVICE_START_HANDLE 0x000E
#define BLE_GATEWAY_FIXED_SERVICE_END_HANDLE 0x0013
#define BLE_GATEWAY_FIXED_DATA_HANDLE 0x0010
#define BLE_GATEWAY_FIXED_NOTIFY_HANDLE 0x0012
#define BLE_GATEWAY_FIXED_NOTIFY_CCCD_HANDLE 0x0013

static const uint8_t TARGET_NAME[] = "sensor_node";
static bt_uuid_t g_client_app_uuid = {BLE_GATEWAY_UUID_LEN, {0x33, 0x33}};
static uint8_t g_client_id;
static uint16_t g_conn_id;
static uint16_t g_service_start_handle;
static uint16_t g_service_end_handle;
static uint16_t g_data_declare_handle;
static uint16_t g_data_handle;
static uint16_t g_notify_declare_handle;
static uint16_t g_notify_handle;
static uint16_t g_notify_cccd_handle;
static bd_addr_t g_peer_addr;
static bool g_connected;
static bool g_connecting;
static bool g_pairing_started;
static bool g_discovery_started;
static bool g_hello_cccd_started;
static bool g_stack_reset_done;
static bool g_control_task_started;
static volatile bool g_scan_restart_requested;
static bool g_scan_restart_stopped;
static volatile bool g_service_discovery_pending;
static uint8_t g_service_discovery_delay_ticks;
static uint8_t g_service_discovery_timeout_ticks;
static uint8_t g_service_discovery_retries;
static volatile bool g_command_write_pending;
static uint8_t g_command_value[BLE_GATEWAY_COMMAND_SIZE];
static uint32_t g_last_sequence;

static errcode_t ble_gateway_enable_cccd(uint16_t conn_id, uint16_t handle, const char *name);

static void ble_gateway_uuid16(uint16_t value, bt_uuid_t *uuid)
{
    uuid->uuid_len = BLE_GATEWAY_UUID_LEN;
    uuid->uuid[0] = (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT);
    uuid->uuid[1] = (uint8_t)value;
}

static bool ble_gateway_uuid_is(const bt_uuid_t *uuid, uint16_t value)
{
    return uuid->uuid_len == BLE_GATEWAY_UUID_LEN && uuid->uuid[0] == (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT) &&
           uuid->uuid[1] == (uint8_t)value;
}

static void ble_gateway_reset_discovery_state(void)
{
    g_service_start_handle = 0;
    g_service_end_handle = 0;
    g_data_declare_handle = 0;
    g_data_handle = 0;
    g_notify_declare_handle = 0;
    g_notify_handle = 0;
    g_notify_cccd_handle = 0;
    g_discovery_started = false;
    g_service_discovery_pending = false;
    g_service_discovery_delay_ticks = 0;
    g_service_discovery_timeout_ticks = 0;
    g_service_discovery_retries = 0;
    g_hello_cccd_started = false;
}

static bool ble_gateway_parse_adv(const uint8_t *data, uint8_t data_len)
{
    uint16_t index = 0;
    const uint16_t target_len = sizeof(TARGET_NAME) - 1;
    bool name_matched = false;
    bool state_found = false;

    while (index < data_len) {
        uint8_t field_len = data[index];
        uint16_t field_end;
        if (field_len == 0) {
            break;
        }
        field_end = (uint16_t)(index + field_len + 1);
        if (field_end > data_len || field_len < 1) {
            break;
        }
        if (data[index + 1] == BLE_GATEWAY_AD_COMPLETE_NAME && field_len == target_len + 1 &&
            memcmp(&data[index + BLE_AD_HEADER_LEN], TARGET_NAME, target_len) == 0) {
            name_matched = true;
        } else if (data[index + 1] == BLE_GATEWAY_AD_SERVICE_DATA16 && field_len == BLE_AD_SERVICE_DATA_FIELD_LEN &&
                   data[index + BLE_AD_UUID_LOW_OFFSET] == (uint8_t)(BLE_GATEWAY_SERVICE_UUID & 0xFF) &&
                   data[index + BLE_AD_UUID_HIGH_OFFSET] ==
                   (uint8_t)(BLE_GATEWAY_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT)) {
            state_found = true;
        }
        index = field_end;
    }
    /* Active scan data and scan response data can be delivered in separate callbacks. */
    return name_matched || state_found;
}

static errcode_t ble_gateway_start_scan(void)
{
    osal_printk("%s start scanning\r\n", BLE_GATEWAY_CLIENT_LOG);
    return gap_ble_start_scan();
}

static void ble_gateway_set_scan_param_cb(errcode_t status)
{
    if (status == ERRCODE_BT_SUCCESS) {
        (void)ble_gateway_start_scan();
    } else {
        osal_printk("%s scan parameter failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
    }
}

static void ble_gateway_scan_result_cb(gap_scan_result_data_t *result)
{
    bool matched = result != NULL && result->adv_data != NULL &&
                   ble_gateway_parse_adv(result->adv_data, result->adv_len);
    if (g_connecting || g_connected || !matched) {
        return;
    }

    if (memcpy_s(&g_peer_addr, sizeof(g_peer_addr), &result->addr, sizeof(result->addr)) != EOK) {
        return;
    }
    g_connecting = true;
    osal_printk("%s found environment sensor node, connecting\r\n", BLE_GATEWAY_CLIENT_LOG);
    (void)gap_ble_stop_scan();
    if (gap_ble_connect_remote_device(&g_peer_addr) != ERRCODE_BT_SUCCESS) {
        g_connecting = false;
        (void)ble_gateway_start_scan();
    }
}

static errcode_t ble_gateway_discover_service(uint16_t conn_id)
{
    bt_uuid_t uuid = {0};
    ble_gateway_uuid16(BLE_GATEWAY_SERVICE_UUID, &uuid);
    osal_printk("%s discover service 0x3333\r\n", BLE_GATEWAY_CLIENT_LOG);
    return gattc_discovery_service(g_client_id, conn_id, &uuid);
}

static void ble_gateway_update_scan_restart(uint16_t *scan_retry_ticks)
{
    if (!g_connecting && !g_connected && !g_scan_restart_requested) {
        (*scan_retry_ticks)++;
        if (*scan_retry_ticks >= BLE_GATEWAY_SCAN_RESTART_TICKS) {
            /* Wi-Fi scanning can pause BLE scanning; periodically restart it while no node is connected. */
            g_scan_restart_stopped = false;
            g_scan_restart_requested = true;
            *scan_retry_ticks = 0;
        }
    } else if (g_connecting || g_connected) {
        *scan_retry_ticks = 0;
    }
}

static void ble_gateway_process_scan_restart(void)
{
    if (!g_scan_restart_requested || g_connecting || g_connected) {
        return;
    }
    if (!g_scan_restart_stopped) {
        (void)gap_ble_stop_scan();
        g_scan_restart_stopped = true;
        return;
    }
    if (ble_gateway_start_scan() == ERRCODE_BT_SUCCESS) {
        g_scan_restart_requested = false;
        g_scan_restart_stopped = false;
    }
}

static void ble_gateway_process_discovery_start(void)
{
    errcode_t ret;
    if (!g_service_discovery_pending || !g_connected || g_discovery_started) {
        return;
    }
    if (g_service_discovery_delay_ticks > 0) {
        g_service_discovery_delay_ticks--;
        return;
    }
    /* Run discovery from task context after pairing callbacks have returned. */
    ret = ble_gateway_discover_service(g_conn_id);
    if (ret == ERRCODE_BT_SUCCESS) {
        g_discovery_started = true;
        g_service_discovery_pending = false;
        g_service_discovery_timeout_ticks = BLE_GATEWAY_DISCOVERY_TIMEOUT_TICKS;
    } else {
        osal_printk("%s service discovery start deferred: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
    }
}

static void ble_gateway_process_discovery_timeout(void)
{
    if (!g_discovery_started || g_service_start_handle != 0U) {
        return;
    }
    if (g_service_discovery_timeout_ticks > 0U) {
        g_service_discovery_timeout_ticks--;
        return;
    }
    g_service_discovery_retries++;
    if (g_service_discovery_retries < BLE_GATEWAY_DISCOVERY_MAX_RETRIES) {
        osal_printk("%s service discovery timeout, retrying\r\n", BLE_GATEWAY_CLIENT_LOG);
        g_discovery_started = false;
        g_service_discovery_delay_ticks = BLE_GATEWAY_DISCOVERY_DELAY_TICKS;
        g_service_discovery_pending = true;
        return;
    }
    /* Both roles belong to this sample, so their GATT registration order and handles are fixed. */
    g_service_start_handle = BLE_GATEWAY_FIXED_SERVICE_START_HANDLE;
    g_service_end_handle = BLE_GATEWAY_FIXED_SERVICE_END_HANDLE;
    g_data_handle = BLE_GATEWAY_FIXED_DATA_HANDLE;
    g_notify_handle = BLE_GATEWAY_FIXED_NOTIFY_HANDLE;
    g_notify_cccd_handle = BLE_GATEWAY_FIXED_NOTIFY_CCCD_HANDLE;
    g_service_discovery_pending = false;
    g_hello_cccd_started = true;
    osal_printk("%s discovery fallback to sample GATT handles\r\n", BLE_GATEWAY_CLIENT_LOG);
    (void)ble_gateway_enable_cccd(g_conn_id, g_notify_cccd_handle, "sensor report");
}

static int ble_gateway_control_task(void *arg)
{
    uint16_t scan_retry_ticks = 0;
    (void)arg;
    while (true) {
        ble_gateway_update_scan_restart(&scan_retry_ticks);
        ble_gateway_process_scan_restart();
        ble_gateway_process_discovery_start();
        ble_gateway_process_discovery_timeout();
        osal_msleep(BLE_GATEWAY_CONTROL_POLL_MS);
    }
    return 0;
}

static errcode_t ble_gateway_start_control_task(void)
{
    osal_task *task;
    if (g_control_task_started) {
        return ERRCODE_SUCC;
    }
    task = osal_kthread_create(ble_gateway_control_task, NULL, "ble_gateway_ctl", BLE_GATEWAY_CONTROL_TASK_STACK_SIZE);
    if (task == NULL) {
        return ERRCODE_FAIL;
    }
    osal_kthread_set_priority(task, BLE_GATEWAY_CONTROL_TASK_PRIO);
    osal_kfree(task);
    g_control_task_started = true;
    return ERRCODE_SUCC;
}

static void ble_gateway_conn_state_cb(uint16_t conn_id,
                                      bd_addr_t *addr,
                                      gap_ble_conn_state_t conn_state,
                                      gap_ble_pair_state_t pair_state,
                                      gap_ble_disc_reason_t reason)
{
    errcode_t ret;
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        g_conn_id = conn_id;
        g_connected = true;
        g_connecting = false;
        g_last_sequence = 0U;
        ble_gateway_iotda_node_state(true);
        ble_gateway_reset_discovery_state();
        osal_printk("%s connected, conn_id=0x%04x\r\n", BLE_GATEWAY_CLIENT_LOG, conn_id);
        if (pair_state == GAP_BLE_PAIR_PAIRED) {
            g_pairing_started = false;
            g_service_discovery_delay_ticks = BLE_GATEWAY_DISCOVERY_DELAY_TICKS;
            g_service_discovery_pending = true;
        } else {
            ret = gap_ble_pair_remote_device(addr);
            g_pairing_started = (ret == ERRCODE_BT_SUCCESS);
            osal_printk("%s pairing requested, ret=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        }
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        if (g_pairing_started) {
            osal_printk("%s remove stale pair after pairing disconnect, ret=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG,
                        gap_ble_remove_pair(addr));
        }
        g_pairing_started = false;
        g_connected = false;
        g_connecting = false;
        g_last_sequence = 0U;
        ble_gateway_iotda_node_state(false);
        if (g_command_write_pending) {
            g_command_write_pending = false;
            ble_gateway_iotda_command_result(false);
        }
        ble_gateway_reset_discovery_state();
        osal_printk("%s disconnected, reason=0x%x, restart scan\r\n", BLE_GATEWAY_CLIENT_LOG, reason);
        (void)ble_gateway_start_scan();
    }
}

static void ble_gateway_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    (void)conn_id;
    g_pairing_started = false;
    osal_printk("%s pair complete, status=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
    if (status == ERRCODE_BT_SUCCESS) {
        g_service_discovery_delay_ticks = BLE_GATEWAY_DISCOVERY_DELAY_TICKS;
        g_service_discovery_pending = true;
        return;
    }
    (void)gap_ble_remove_pair(addr);
    (void)gap_ble_disconnect_remote_device(addr);
}

static void ble_gateway_mtu_changed_cb(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    (void)client_id;
    osal_printk("%s MTU changed: %u, status=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, mtu_size, status);
    (void)conn_id;
}

static void ble_gateway_discovery_service_cb(uint8_t client_id,
                                             uint16_t conn_id,
                                             gattc_discovery_service_result_t *service,
                                             errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    (void)client_id;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s service discovery callback failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
        g_discovery_started = false;
        g_service_discovery_delay_ticks = BLE_GATEWAY_DISCOVERY_DELAY_TICKS;
        g_service_discovery_pending = true;
        return;
    }
    if (service == NULL || !ble_gateway_uuid_is(&service->uuid, BLE_GATEWAY_SERVICE_UUID)) {
        osal_printk("%s service discovery returned unexpected UUID\r\n", BLE_GATEWAY_CLIENT_LOG);
        return;
    }
    g_service_discovery_timeout_ticks = 0;
    g_service_discovery_retries = 0;
    g_service_start_handle = service->start_hdl;
    g_service_end_handle = service->end_hdl;
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = 0;
    osal_printk("%s service discovered, handles=0x%04x-0x%04x\r\n", BLE_GATEWAY_CLIENT_LOG, service->start_hdl,
                service->end_hdl);
    (void)gattc_discovery_character(g_client_id, conn_id, &param);
}

static void ble_gateway_discovery_character_cb(uint8_t client_id,
                                               uint16_t conn_id,
                                               gattc_discovery_character_result_t *character,
                                               errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS) {
        return;
    }
    if (ble_gateway_uuid_is(&character->uuid, BLE_GATEWAY_DATA_UUID)) {
        g_data_declare_handle = character->declare_handle;
        g_data_handle = character->value_handle;
        osal_printk("%s data characteristic discovered, value=0x%04x\r\n", BLE_GATEWAY_CLIENT_LOG, g_data_handle);
    } else if (ble_gateway_uuid_is(&character->uuid, BLE_GATEWAY_NOTIFY_UUID)) {
        g_notify_declare_handle = character->declare_handle;
        g_notify_handle = character->value_handle;
        osal_printk("%s notify characteristic discovered, value=0x%04x\r\n", BLE_GATEWAY_CLIENT_LOG, g_notify_handle);
    }
}

static void ble_gateway_discovery_character_complete_cb(uint8_t client_id,
                                                        uint16_t conn_id,
                                                        gattc_discovery_character_param_t *param,
                                                        errcode_t status)
{
    (void)client_id;
    (void)param;
    if (status != ERRCODE_BT_SUCCESS || g_data_declare_handle == 0 || g_notify_declare_handle == 0) {
        osal_printk("%s characteristic discovery failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
        return;
    }
    (void)gattc_discovery_descriptor(g_client_id, conn_id, g_notify_declare_handle);
}

static void ble_gateway_discovery_descriptor_cb(uint8_t client_id,
                                                uint16_t conn_id,
                                                gattc_discovery_descriptor_result_t *descriptor,
                                                errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status == ERRCODE_BT_SUCCESS && ble_gateway_uuid_is(&descriptor->uuid, BLE_GATEWAY_CCCD_UUID)) {
        g_notify_cccd_handle = descriptor->descriptor_hdl;
        osal_printk("%s hello CCCD discovered, handle=0x%04x\r\n", BLE_GATEWAY_CLIENT_LOG, g_notify_cccd_handle);
    }
}

static errcode_t ble_gateway_enable_cccd(uint16_t conn_id, uint16_t handle, const char *name)
{
    uint8_t cccd_enable[2] = {1, 0};
    gattc_handle_value_t write_value = {0};
    write_value.handle = handle;
    write_value.data = cccd_enable;
    write_value.data_len = sizeof(cccd_enable);
    osal_printk("%s enabling %s CCCD\r\n", BLE_GATEWAY_CLIENT_LOG, name);
    return gattc_write_req(g_client_id, conn_id, &write_value);
}

static void ble_gateway_discovery_descriptor_complete_cb(uint8_t client_id,
                                                         uint16_t conn_id,
                                                         uint16_t characteristic_handle,
                                                         errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS || !g_connected || conn_id != g_conn_id || g_notify_cccd_handle == 0) {
        osal_printk("%s descriptor discovery failed, char=0x%04x, status=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG,
                    characteristic_handle, status);
        return;
    }
    g_hello_cccd_started = true;
    (void)ble_gateway_enable_cccd(conn_id, g_notify_cccd_handle, "sensor report");
}

static void ble_gateway_notification_cb(uint8_t client_id,
                                        uint16_t conn_id,
                                        gattc_handle_value_t *data,
                                        errcode_t status)
{
    ble_gateway_report_t report = {0};
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS || data->handle != g_notify_handle) {
        return;
    }
    if (!ble_gateway_decode_report(data->data, data->data_len, &report)) {
        osal_printk("%s invalid binary sensor report, len=%u\r\n", BLE_GATEWAY_CLIENT_LOG, data->data_len);
        return;
    }
    if (g_last_sequence != 0U && report.sequence <= g_last_sequence) {
        osal_printk("%s stale or duplicate sensor report dropped: seq=%u last=%u\r\n", BLE_GATEWAY_CLIENT_LOG,
                    report.sequence, g_last_sequence);
        return;
    }
    g_last_sequence = report.sequence;
    osal_printk("%s BLE report queued: node=%u seq=%u\r\n", BLE_GATEWAY_CLIENT_LOG, report.node_id,
                report.sequence);
    ble_gateway_iotda_enqueue_report(&report);
}

static void ble_gateway_write_cb(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    (void)client_id;
    (void)conn_id;
    if (handle == g_notify_cccd_handle) {
        osal_printk("%s sensor report CCCD write %s\r\n", BLE_GATEWAY_CLIENT_LOG,
                    status == GATT_STATUS_SUCCESS ? "success" : "failed");
    } else if (handle == g_data_handle && g_command_write_pending) {
        g_command_write_pending = false;
        osal_printk("%s BLE interval command %s\r\n", BLE_GATEWAY_CLIENT_LOG,
                    status == GATT_STATUS_SUCCESS ? "success" : "failed");
        ble_gateway_iotda_command_result(status == GATT_STATUS_SUCCESS);
    }
}

static void ble_gateway_enable_cb(errcode_t status)
{
    gap_ble_sec_params_t security = {0};
    gap_ble_scan_params_t scan_params = {0};
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
        return;
    }
    if (!g_stack_reset_done) {
        g_stack_reset_done = true;
        osal_printk("%s cycling BLE stack to clear retained GATT state\r\n", BLE_GATEWAY_CLIENT_LOG);
        ret = disable_ble();
        if (ret == ERRCODE_BT_SUCCESS) {
            return;
        }
        osal_printk("%s stack reset request failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
    }
    security.bondable = 1;
    security.io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT;
    security.sc_enable = 0;
    security.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    ret = gap_ble_set_sec_param(&security);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s security config failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        return;
    }
    ret = gattc_register_client(&g_client_app_uuid, &g_client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register client failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        return;
    }
    scan_params.scan_interval = BLE_GATEWAY_SCAN_INTERVAL;
    scan_params.scan_window = BLE_GATEWAY_SCAN_INTERVAL;
    scan_params.scan_type = GAP_BLE_SCAN_TYPE_ACTIVE;
    scan_params.scan_phy = GAP_BLE_PHY_1M;
    scan_params.scan_filter_policy = GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL;
    ret = gap_ble_set_scan_parameters(&scan_params);
    osal_printk("%s init ok, scan param ret=0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
}

static void ble_gateway_disable_cb(errcode_t status)
{
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s disable failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, status);
        return;
    }
    osal_printk("%s BLE stack reset complete, enabling BLE\r\n", BLE_GATEWAY_CLIENT_LOG);
    ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable request failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
    }
}

static errcode_t ble_gateway_register_callbacks(void)
{
    gap_ble_callbacks_t gap_callbacks = {0};
    gattc_callbacks_t gatt_callbacks = {0};
    errcode_t ret;

    gap_callbacks.ble_enable_cb = ble_gateway_enable_cb;
    gap_callbacks.ble_disable_cb = ble_gateway_disable_cb;
    gap_callbacks.set_scan_param_cb = ble_gateway_set_scan_param_cb;
    gap_callbacks.scan_result_cb = ble_gateway_scan_result_cb;
    gap_callbacks.conn_state_change_cb = ble_gateway_conn_state_cb;
    gap_callbacks.pair_result_cb = ble_gateway_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    gatt_callbacks.discovery_svc_cb = ble_gateway_discovery_service_cb;
    gatt_callbacks.discovery_chara_cb = ble_gateway_discovery_character_cb;
    gatt_callbacks.discovery_chara_cmp_cb = ble_gateway_discovery_character_complete_cb;
    gatt_callbacks.discovery_desc_cb = ble_gateway_discovery_descriptor_cb;
    gatt_callbacks.discovery_desc_cmp_cb = ble_gateway_discovery_descriptor_complete_cb;
    gatt_callbacks.mtu_changed_cb = ble_gateway_mtu_changed_cb;
    gatt_callbacks.notification_cb = ble_gateway_notification_cb;
    gatt_callbacks.write_cb = ble_gateway_write_cb;
    return gattc_register_callbacks(&gatt_callbacks);
}

errcode_t ble_gateway_client_set_sample_interval(uint32_t interval_s)
{
    gattc_handle_value_t write_value = {0};
    errcode_t ret;

    if (!g_connected || g_data_handle == 0U || g_command_write_pending || interval_s < BLE_GATEWAY_MIN_INTERVAL_S ||
        interval_s > BLE_GATEWAY_MAX_INTERVAL_S) {
        return ERRCODE_FAIL;
    }
    ble_gateway_encode_interval_command(g_command_value, interval_s);
    write_value.handle = g_data_handle;
    write_value.data = g_command_value;
    write_value.data_len = sizeof(g_command_value);
    g_command_write_pending = true;
    ret = gattc_write_req(g_client_id, g_conn_id, &write_value);
    if (ret != ERRCODE_BT_SUCCESS) {
        g_command_write_pending = false;
    }
    return ret;
}

void ble_gateway_client_restart_scan(void)
{
    if (!g_connecting && !g_connected) {
        g_scan_restart_stopped = false;
        g_scan_restart_requested = true;
    }
}

errcode_t ble_gateway_client_init(void)
{
    errcode_t ret;
    ble_gateway_iotda_node_state(false);
    ret = ble_gateway_start_control_task();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s control task start failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        return ret;
    }
    ret = ble_gateway_wifi_start();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Wi-Fi task start failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        return ret;
    }
    ret = ble_gateway_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s callback registration failed: 0x%x\r\n", BLE_GATEWAY_CLIENT_LOG, ret);
        return ret;
    }
    if (ble_is_enable()) {
        g_stack_reset_done = true;
        osal_printk("%s BLE already enabled, resetting stack\r\n", BLE_GATEWAY_CLIENT_LOG);
        return disable_ble();
    }
    osal_printk("%s enabling BLE\r\n", BLE_GATEWAY_CLIENT_LOG);
    return enable_ble();
}
