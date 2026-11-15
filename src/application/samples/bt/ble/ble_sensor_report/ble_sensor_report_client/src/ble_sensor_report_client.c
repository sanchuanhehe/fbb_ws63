/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements the BLE sensor report data collector.
 * @else
 * @brief 实现 BLE 传感器上报数据采集端。
 * @endif
 *
 * History: \n
 * 2026-07-23, Create file. \n
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
#include "ble_sensor_report_client.h"

#define BLE_SENSOR_REPORT_CLIENT_LOG "[ble hello client]"
#define BLE_SENSOR_REPORT_SERVICE_UUID 0x3333
#define BLE_SENSOR_REPORT_DATA_UUID 0x3434
#define BLE_SENSOR_REPORT_NOTIFY_UUID 0x3435
#define BLE_SENSOR_REPORT_CCCD_UUID 0x2902
#define BLE_SENSOR_REPORT_UUID_LEN 2
#define BLE_SENSOR_REPORT_MTU 247
#define BLE_SENSOR_REPORT_SCAN_INTERVAL 0x30
#define BLE_SENSOR_REPORT_AD_COMPLETE_NAME 0x09
#define BLE_SENSOR_REPORT_AD_SERVICE_DATA16 0x16
#define BLE_SENSOR_REPORT_STATE_DEFAULT 0x00
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_AD_HEADER_LEN 2
#define BLE_AD_SERVICE_DATA_FIELD_LEN 4
#define BLE_AD_SERVICE_DATA_STATE_OFFSET 4
#define BLE_AD_UUID_LOW_OFFSET 2
#define BLE_AD_UUID_HIGH_OFFSET 3
#define BLE_SENSOR_REPORT_MAX_PAYLOAD_LEN 80
#define BLE_SENSOR_REPORT_DECIMAL_BASE 10
#define BLE_SENSOR_REPORT_MIN_TEMPERATURE_TENTHS_C (-400)
#define BLE_SENSOR_REPORT_MAX_TEMPERATURE_TENTHS_C 850
#define BLE_SENSOR_REPORT_MAX_HUMIDITY_TENTHS_PERCENT 1000
#define BLE_SENSOR_REPORT_MIN_PRESSURE_TENTHS_HPA 3000
#define BLE_SENSOR_REPORT_MAX_PRESSURE_TENTHS_HPA 11000

/* Target identity, control values, and client state. / 目标标识、控制值与客户端状态。 */
static const uint8_t TARGET_NAME[] = "sensor_node";
static const uint8_t DEFAULT_VALUE[] = "sensor_ready";
static const uint8_t NEW_VALUE[] = "interval=1000";
static bt_uuid_t g_client_app_uuid = {BLE_SENSOR_REPORT_UUID_LEN, {0x33, 0x33}};
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
static bool g_peer_default_state;
static bool g_connected;
static bool g_connecting;
static bool g_pairing_started;
static bool g_mtu_exchange_started;
static bool g_discovery_started;
static bool g_read_started;
static bool g_write_started;
static bool g_hello_cccd_started;
static bool g_stack_reset_done;
static bool g_cache_sync_write_started;

/**
 * @if Eng
 * @brief Matches and consumes one literal token from a report string.
 * @else
 * @brief 从报文字符串中匹配并消费一个固定标记。
 * @endif
 */
static bool ble_sensor_report_match_literal(const char **cursor, const char *literal)
{
    size_t length = strlen(literal);
    if (strncmp(*cursor, literal, length) != 0) {
        return false;
    }
    *cursor += length;
    return true;
}

/**
 * @if Eng
 * @brief Parses an unsigned decimal integer with overflow protection.
 * @else
 * @brief 解析无符号十进制整数并检查溢出。
 * @endif
 */
static bool ble_sensor_report_parse_unsigned(const char **cursor, uint32_t *value)
{
    uint32_t result = 0;
    bool has_digit = false;

    while (**cursor >= '0' && **cursor <= '9') {
        uint32_t digit = (uint32_t)(**cursor - '0');
        if (result > (UINT32_MAX - digit) / BLE_SENSOR_REPORT_DECIMAL_BASE) {
            return false;
        }
        result = result * BLE_SENSOR_REPORT_DECIMAL_BASE + digit;
        (*cursor)++;
        has_digit = true;
    }
    if (!has_digit) {
        return false;
    }
    *value = result;
    return true;
}

/**
 * @if Eng
 * @brief Parses a value containing exactly one decimal digit.
 * @else
 * @brief 解析恰好包含一位小数的数值。
 * @endif
 */
static bool ble_sensor_report_parse_tenths(const char **cursor, bool allow_negative, int32_t *value)
{
    bool negative = false;
    uint32_t integer;
    uint32_t decimal;

    if (allow_negative && **cursor == '-') {
        negative = true;
        (*cursor)++;
    }
    if (!ble_sensor_report_parse_unsigned(cursor, &integer) || **cursor != '.') {
        return false;
    }
    (*cursor)++;
    if (**cursor < '0' || **cursor > '9') {
        return false;
    }
    decimal = (uint32_t)(**cursor - '0');
    (*cursor)++;
    if (integer > (uint32_t)INT32_MAX / BLE_SENSOR_REPORT_DECIMAL_BASE ||
        (integer == (uint32_t)INT32_MAX / BLE_SENSOR_REPORT_DECIMAL_BASE &&
         decimal > (uint32_t)INT32_MAX % BLE_SENSOR_REPORT_DECIMAL_BASE)) {
        return false;
    }
    int32_t result = (int32_t)(integer * BLE_SENSOR_REPORT_DECIMAL_BASE + decimal);
    *value = negative ? -result : result;
    return true;
}

/**
 * @if Eng
 * @brief Validates the complete sensor report format and physical ranges.
 * @else
 * @brief 校验完整的传感器报文格式与物理量范围。
 * @endif
 */
static bool ble_sensor_report_validate_report(const uint8_t *data, uint16_t length)
{
    char report[BLE_SENSOR_REPORT_MAX_PAYLOAD_LEN] = {0};
    uint32_t sequence = 0;
    int32_t temperature_tenths = 0;
    int32_t humidity_tenths = 0;
    int32_t pressure_tenths = 0;
    const char *cursor = report;

    if (data == NULL || length == 0 || length >= sizeof(report) ||
        memcpy_s(report, sizeof(report), data, length) != EOK) {
        return false;
    }
    if (!ble_sensor_report_match_literal(&cursor, "seq=") ||
        !ble_sensor_report_parse_unsigned(&cursor, &sequence) ||
        !ble_sensor_report_match_literal(&cursor, ",temp=") ||
        !ble_sensor_report_parse_tenths(&cursor, true, &temperature_tenths) ||
        !ble_sensor_report_match_literal(&cursor, ",hum=") ||
        !ble_sensor_report_parse_tenths(&cursor, false, &humidity_tenths) ||
        !ble_sensor_report_match_literal(&cursor, ",press=") ||
        !ble_sensor_report_parse_tenths(&cursor, false, &pressure_tenths) || *cursor != '\0') {
        return false;
    }
    return sequence > 0 && temperature_tenths >= BLE_SENSOR_REPORT_MIN_TEMPERATURE_TENTHS_C &&
           temperature_tenths <= BLE_SENSOR_REPORT_MAX_TEMPERATURE_TENTHS_C &&
           humidity_tenths >= 0 && humidity_tenths <= BLE_SENSOR_REPORT_MAX_HUMIDITY_TENTHS_PERCENT &&
           pressure_tenths >= BLE_SENSOR_REPORT_MIN_PRESSURE_TENTHS_HPA &&
           pressure_tenths <= BLE_SENSOR_REPORT_MAX_PRESSURE_TENTHS_HPA;
}

/**
 * @if Eng
 * @brief Converts a 16-bit value to the SDK Bluetooth UUID representation.
 * @else
 * @brief 将 16 位数值转换为 SDK 蓝牙 UUID 表示形式。
 * @endif
 */
static void ble_sensor_report_uuid16(uint16_t value, bt_uuid_t *uuid)
{
    uuid->uuid_len = BLE_SENSOR_REPORT_UUID_LEN;
    uuid->uuid[0] = (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT);
    uuid->uuid[1] = (uint8_t)value;
}

/**
 * @if Eng
 * @brief Checks whether an SDK Bluetooth UUID equals a 16-bit value.
 * @else
 * @brief 检查 SDK 蓝牙 UUID 是否等于指定的 16 位数值。
 * @endif
 */
static bool ble_sensor_report_uuid_is(const bt_uuid_t *uuid, uint16_t value)
{
    return uuid->uuid_len == BLE_SENSOR_REPORT_UUID_LEN &&
           uuid->uuid[0] == (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT) && uuid->uuid[1] == (uint8_t)value;
}

/**
 * @if Eng
 * @brief Clears handles and flags associated with GATT discovery.
 * @else
 * @brief 清除 GATT 服务发现相关的句柄与状态标志。
 * @endif
 */
static void ble_sensor_report_reset_discovery_state(void)
{
    g_service_start_handle = 0;
    g_service_end_handle = 0;
    g_data_declare_handle = 0;
    g_data_handle = 0;
    g_notify_declare_handle = 0;
    g_notify_handle = 0;
    g_notify_cccd_handle = 0;
    g_mtu_exchange_started = false;
    g_discovery_started = false;
    g_read_started = false;
    g_write_started = false;
    g_hello_cccd_started = false;
    g_cache_sync_write_started = false;
}

/**
 * @if Eng
 * @brief Parses advertising fields and identifies the target sensor node.
 * @else
 * @brief 解析广播字段并识别目标传感器节点。
 * @endif
 */
static bool ble_sensor_report_parse_adv(const uint8_t *data, uint8_t data_len, bool *default_state)
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
        if (data[index + 1] == BLE_SENSOR_REPORT_AD_COMPLETE_NAME && field_len == target_len + 1 &&
            memcmp(&data[index + BLE_AD_HEADER_LEN], TARGET_NAME, target_len) == 0) {
            name_matched = true;
        } else if (data[index + 1] == BLE_SENSOR_REPORT_AD_SERVICE_DATA16 &&
                   field_len == BLE_AD_SERVICE_DATA_FIELD_LEN &&
                   data[index + BLE_AD_UUID_LOW_OFFSET] == (uint8_t)(BLE_SENSOR_REPORT_SERVICE_UUID & 0xFF) &&
                   data[index + BLE_AD_UUID_HIGH_OFFSET] ==
                   (uint8_t)(BLE_SENSOR_REPORT_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT)) {
            *default_state = (data[index + BLE_AD_SERVICE_DATA_STATE_OFFSET] == BLE_SENSOR_REPORT_STATE_DEFAULT);
            state_found = true;
        }
        index = field_end;
    }
    return name_matched && state_found;
}

/**
 * @if Eng
 * @brief Starts scanning for the target sensor node.
 * @else
 * @brief 开始扫描目标传感器节点。
 * @endif
 */
static errcode_t ble_sensor_report_start_scan(void)
{
    osal_printk("%s start scanning\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    return gap_ble_start_scan();
}

/**
 * @if Eng
 * @brief Starts scanning after scan parameters are configured.
 * @else
 * @brief 在扫描参数配置完成后启动扫描。
 * @endif
 */
static void ble_sensor_report_set_scan_param_cb(errcode_t status)
{
    if (status == ERRCODE_BT_SUCCESS) {
        (void)ble_sensor_report_start_scan();
    } else {
        osal_printk("%s scan parameter failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
    }
}

/**
 * @if Eng
 * @brief Connects when a matching sensor node advertisement is received.
 * @else
 * @brief 收到匹配的传感器节点广播后发起连接。
 * @endif
 */
static void ble_sensor_report_scan_result_cb(gap_scan_result_data_t *result)
{
    bool default_state = false;
    if (g_connecting || g_connected || result == NULL || result->adv_data == NULL ||
        !ble_sensor_report_parse_adv(result->adv_data, result->adv_len, &default_state)) {
        return;
    }

    if (memcpy_s(&g_peer_addr, sizeof(g_peer_addr), &result->addr, sizeof(result->addr)) != EOK) {
        return;
    }
    g_peer_default_state = default_state;
    g_connecting = true;
    osal_printk("%s found ble_sensor_report_server, state=%s, connecting\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                g_peer_default_state ? "device_status_ok" : "retained");
    (void)gap_ble_stop_scan();
    if (gap_ble_connect_remote_device(&g_peer_addr) != ERRCODE_BT_SUCCESS) {
        g_connecting = false;
        (void)ble_sensor_report_start_scan();
    }
}

/**
 * @if Eng
 * @brief Starts discovery of the sensor report GATT service.
 * @else
 * @brief 启动传感器上报 GATT 服务发现。
 * @endif
 */
static errcode_t ble_sensor_report_discover_service(uint16_t conn_id)
{
    bt_uuid_t uuid = {0};
    ble_sensor_report_uuid16(BLE_SENSOR_REPORT_SERVICE_UUID, &uuid);
    osal_printk("%s discover service 0x3333\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    return gattc_discovery_service(g_client_id, conn_id, &uuid);
}

/**
 * @if Eng
 * @brief Starts MTU exchange for the active connection.
 * @else
 * @brief 为当前连接发起 MTU 交换。
 * @endif
 */
static errcode_t ble_sensor_report_exchange_mtu(uint16_t conn_id)
{
    errcode_t ret;
    osal_printk("%s exchange MTU %u\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, BLE_SENSOR_REPORT_MTU);
    g_mtu_exchange_started = true;
    ret = gattc_exchange_mtu_req(g_client_id, conn_id, BLE_SENSOR_REPORT_MTU);
    if (ret != ERRCODE_BT_SUCCESS) {
        g_mtu_exchange_started = false;
    }
    return ret;
}

/**
 * @if Eng
 * @brief Handles connection changes, pairing, and scan recovery.
 * @else
 * @brief 处理连接变化、配对和扫描恢复。
 * @endif
 */
static void ble_sensor_report_conn_state_cb(uint16_t conn_id,
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
        ble_sensor_report_reset_discovery_state();
        osal_printk("%s connected, conn_id=0x%04x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, conn_id);
        if (pair_state == GAP_BLE_PAIR_PAIRED) {
            g_pairing_started = false;
            (void)ble_sensor_report_exchange_mtu(conn_id);
        } else {
            ret = gap_ble_pair_remote_device(addr);
            g_pairing_started = (ret == ERRCODE_BT_SUCCESS);
            osal_printk("%s pairing requested, ret=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        }
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        if (g_pairing_started) {
            osal_printk("%s remove stale pair after pairing disconnect, ret=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                        gap_ble_remove_pair(addr));
        }
        g_pairing_started = false;
        g_connected = false;
        g_connecting = false;
        ble_sensor_report_reset_discovery_state();
        osal_printk("%s disconnected, reason=0x%x, restart scan\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, reason);
        (void)ble_sensor_report_start_scan();
    }
}

/**
 * @if Eng
 * @brief Continues with MTU exchange after pairing completes.
 * @else
 * @brief 配对完成后继续执行 MTU 交换。
 * @endif
 */
static void ble_sensor_report_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    g_pairing_started = false;
    osal_printk("%s pair complete, status=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
    if (status == ERRCODE_BT_SUCCESS) {
        (void)ble_sensor_report_exchange_mtu(conn_id);
        return;
    }
    (void)gap_ble_remove_pair(addr);
    (void)gap_ble_disconnect_remote_device(addr);
}

/**
 * @if Eng
 * @brief Starts service discovery after MTU exchange completes.
 * @else
 * @brief MTU 交换完成后启动服务发现。
 * @endif
 */
static void ble_sensor_report_mtu_changed_cb(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    (void)client_id;
    osal_printk("%s MTU changed: %u, status=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, mtu_size, status);
    if (status == ERRCODE_BT_SUCCESS && g_connected && conn_id == g_conn_id && g_mtu_exchange_started &&
        !g_discovery_started) {
        g_discovery_started = true;
        (void)ble_sensor_report_discover_service(conn_id);
    }
}

/**
 * @if Eng
 * @brief Saves service handles and starts characteristic discovery.
 * @else
 * @brief 保存服务句柄并启动特征发现。
 * @endif
 */
static void ble_sensor_report_discovery_service_cb(uint8_t client_id,
                                                   uint16_t conn_id,
                                                   gattc_discovery_service_result_t *service,
                                                   errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    (void)client_id;
    if (status != ERRCODE_BT_SUCCESS || !ble_sensor_report_uuid_is(&service->uuid, BLE_SENSOR_REPORT_SERVICE_UUID)) {
        return;
    }
    g_service_start_handle = service->start_hdl;
    g_service_end_handle = service->end_hdl;
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = 0;
    osal_printk("%s service discovered, handles=0x%04x-0x%04x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, service->start_hdl,
                service->end_hdl);
    (void)gattc_discovery_character(g_client_id, conn_id, &param);
}

/**
 * @if Eng
 * @brief Records discovered data and notification characteristic handles.
 * @else
 * @brief 记录发现的数据特征与通知特征句柄。
 * @endif
 */
static void ble_sensor_report_discovery_character_cb(uint8_t client_id,
                                                     uint16_t conn_id,
                                                     gattc_discovery_character_result_t *character,
                                                     errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS) {
        return;
    }
    if (ble_sensor_report_uuid_is(&character->uuid, BLE_SENSOR_REPORT_DATA_UUID)) {
        g_data_declare_handle = character->declare_handle;
        g_data_handle = character->value_handle;
        osal_printk("%s data characteristic discovered, value=0x%04x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, g_data_handle);
    } else if (ble_sensor_report_uuid_is(&character->uuid, BLE_SENSOR_REPORT_NOTIFY_UUID)) {
        g_notify_declare_handle = character->declare_handle;
        g_notify_handle = character->value_handle;
        osal_printk("%s notify characteristic discovered, value=0x%04x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                    g_notify_handle);
    }
}

/**
 * @if Eng
 * @brief Starts descriptor discovery after all characteristics are found.
 * @else
 * @brief 特征发现完成后启动描述符发现。
 * @endif
 */
static void ble_sensor_report_discovery_character_complete_cb(uint8_t client_id,
                                                              uint16_t conn_id,
                                                              gattc_discovery_character_param_t *param,
                                                              errcode_t status)
{
    (void)client_id;
    (void)param;
    if (status != ERRCODE_BT_SUCCESS || g_data_declare_handle == 0 || g_notify_declare_handle == 0) {
        osal_printk("%s characteristic discovery failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
        return;
    }
    (void)gattc_discovery_descriptor(g_client_id, conn_id, g_notify_declare_handle);
}

/**
 * @if Eng
 * @brief Records the notification CCCD handle.
 * @else
 * @brief 记录通知 CCCD 句柄。
 * @endif
 */
static void ble_sensor_report_discovery_descriptor_cb(uint8_t client_id,
                                                      uint16_t conn_id,
                                                      gattc_discovery_descriptor_result_t *descriptor,
                                                      errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status == ERRCODE_BT_SUCCESS && ble_sensor_report_uuid_is(&descriptor->uuid, BLE_SENSOR_REPORT_CCCD_UUID)) {
        g_notify_cccd_handle = descriptor->descriptor_hdl;
        osal_printk("%s hello CCCD discovered, handle=0x%04x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, g_notify_cccd_handle);
    }
}

/**
 * @if Eng
 * @brief Writes the notification enable value to a CCCD.
 * @else
 * @brief 向 CCCD 写入通知使能值。
 * @endif
 */
static errcode_t ble_sensor_report_enable_cccd(uint16_t conn_id, uint16_t handle, const char *name)
{
    uint8_t cccd_enable[2] = {1, 0};
    gattc_handle_value_t write_value = {0};
    write_value.handle = handle;
    write_value.data = cccd_enable;
    write_value.data_len = sizeof(cccd_enable);
    osal_printk("%s enabling %s CCCD\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, name);
    return gattc_write_req(g_client_id, conn_id, &write_value);
}

/**
 * @if Eng
 * @brief Synchronizes control state before enabling notifications.
 * @else
 * @brief 在使能通知前同步控制特征状态。
 * @endif
 */
static void ble_sensor_report_discovery_descriptor_complete_cb(uint8_t client_id,
                                                               uint16_t conn_id,
                                                               uint16_t characteristic_handle,
                                                               errcode_t status)
{
    gattc_handle_value_t write_value = {0};
    errcode_t ret;
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS || !g_connected || conn_id != g_conn_id || g_notify_cccd_handle == 0) {
        osal_printk("%s descriptor discovery failed, char=0x%04x, status=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                    characteristic_handle, status);
        return;
    }
    if (g_peer_default_state) {
        write_value.handle = g_data_handle;
        write_value.data = (uint8_t *)DEFAULT_VALUE;
        write_value.data_len = sizeof(DEFAULT_VALUE) - 1;
        g_cache_sync_write_started = true;
        osal_printk("%s syncing data attribute: device_status_ok\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
        ret = gattc_write_req(g_client_id, conn_id, &write_value);
        if (ret != ERRCODE_BT_SUCCESS) {
            g_cache_sync_write_started = false;
            osal_printk("%s cache sync write request failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        }
        return;
    }
    g_hello_cccd_started = true;
    (void)ble_sensor_report_enable_cccd(conn_id, g_notify_cccd_handle, "hello");
}

/**
 * @if Eng
 * @brief Parses and validates a received sensor report notification.
 * @else
 * @brief 解析并校验收到的传感器上报通知。
 * @endif
 */
static void ble_sensor_report_notification_cb(uint8_t client_id,
                                              uint16_t conn_id,
                                              gattc_handle_value_t *data,
                                              errcode_t status)
{
    errcode_t ret;
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS || data->handle != g_notify_handle) {
        return;
    }
    osal_printk("%s sensor report received: %.*s\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, data->data_len, data->data);
    if (ble_sensor_report_validate_report(data->data, data->data_len)) {
        osal_printk("%s report validation passed\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    } else {
        osal_printk("%s report validation failed\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    }
    if (!g_read_started) {
        g_read_started = true;
        osal_printk("%s read request sent\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
        ret = gattc_read_req_by_handle(g_client_id, g_conn_id, g_data_handle);
        if (ret != ERRCODE_BT_SUCCESS) {
            g_read_started = false;
            osal_printk("%s read request failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        }
    }
}

/**
 * @if Eng
 * @brief Processes the control value and writes the desired report interval.
 * @else
 * @brief 处理控制特征读取结果并写入期望的上报周期。
 * @endif
 */
static void ble_sensor_report_read_cb(uint8_t client_id,
                                      uint16_t conn_id,
                                      gattc_handle_value_t *read_result,
                                      gatt_status_t status)
{
    gattc_handle_value_t write_value = {0};
    (void)client_id;
    if (status != GATT_STATUS_SUCCESS || read_result->handle != g_data_handle) {
        osal_printk("%s read failed, status=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
        return;
    }
    osal_printk("%s read result: %.*s\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, read_result->data_len, read_result->data);
    if (g_write_started) {
        return;
    }
    g_write_started = true;
    write_value.handle = g_data_handle;
    write_value.data = (uint8_t *)NEW_VALUE;
    write_value.data_len = sizeof(NEW_VALUE) - 1;
    osal_printk("%s report interval write request sent: interval=1000\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    (void)gattc_write_req(g_client_id, conn_id, &write_value);
}

/**
 * @if Eng
 * @brief Enables notifications after control state synchronization completes.
 * @else
 * @brief 控制状态同步完成后使能通知。
 * @endif
 */
static void ble_sensor_report_cache_sync_write_complete(gatt_status_t status)
{
    g_cache_sync_write_started = false;
    osal_printk("%s cache sync write %s\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                status == GATT_STATUS_SUCCESS ? "success" : "failed");
    if ((status != GATT_STATUS_SUCCESS) || g_hello_cccd_started) {
        return;
    }

    g_hello_cccd_started = true;
    if (ble_sensor_report_enable_cccd(g_conn_id, g_notify_cccd_handle, "hello") != ERRCODE_BT_SUCCESS) {
        g_hello_cccd_started = false;
        osal_printk("%s hello CCCD write request failed\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    }
}

/**
 * @if Eng
 * @brief Dispatches completion of control and CCCD write requests.
 * @else
 * @brief 分发控制特征与 CCCD 写请求的完成事件。
 * @endif
 */
static void ble_sensor_report_write_cb(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    (void)client_id;
    (void)conn_id;
    if (handle == g_notify_cccd_handle) {
        osal_printk("%s hello CCCD write %s\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                    status == GATT_STATUS_SUCCESS ? "success" : "failed");
    } else if (handle == g_data_handle) {
        if (g_cache_sync_write_started) {
            ble_sensor_report_cache_sync_write_complete(status);
            return;
        }
        osal_printk("%s write cfm: %s\r\n", BLE_SENSOR_REPORT_CLIENT_LOG,
                    status == GATT_STATUS_SUCCESS ? "success" : "failed");
    }
}

/**
 * @if Eng
 * @brief Configures security, registers the GATT client, and sets scan parameters.
 * @else
 * @brief 配置安全参数、注册 GATT 客户端并设置扫描参数。
 * @endif
 */
static void ble_sensor_report_enable_cb(errcode_t status)
{
    gap_ble_sec_params_t security = {0};
    gap_ble_scan_params_t scan_params = {0};
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
        return;
    }
    if (!g_stack_reset_done) {
        g_stack_reset_done = true;
        osal_printk("%s cycling BLE stack to clear retained GATT state\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
        ret = disable_ble();
        if (ret == ERRCODE_BT_SUCCESS) {
            return;
        }
        osal_printk("%s stack reset request failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
    }
    security.bondable = 1;
    security.io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT;
    security.sc_enable = 0;
    security.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    ret = gap_ble_set_sec_param(&security);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s security config failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        return;
    }
    ret = gattc_register_client(&g_client_app_uuid, &g_client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register client failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        return;
    }
    scan_params.scan_interval = BLE_SENSOR_REPORT_SCAN_INTERVAL;
    scan_params.scan_window = BLE_SENSOR_REPORT_SCAN_INTERVAL;
    scan_params.scan_type = GAP_BLE_SCAN_TYPE_ACTIVE;
    scan_params.scan_phy = GAP_BLE_PHY_1M;
    scan_params.scan_filter_policy = GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL;
    ret = gap_ble_set_scan_parameters(&scan_params);
    osal_printk("%s init ok, scan param ret=0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
}

/**
 * @if Eng
 * @brief Re-enables BLE after clearing retained GATT state.
 * @else
 * @brief 清理保留的 GATT 状态后重新使能 BLE。
 * @endif
 */
static void ble_sensor_report_disable_cb(errcode_t status)
{
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s disable failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, status);
        return;
    }
    osal_printk("%s BLE stack reset complete, enabling BLE\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable request failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Registers GAP and GATT client callbacks used by this sample.
 * @else
 * @brief 注册本案例使用的 GAP 和 GATT 客户端回调。
 * @endif
 */
static errcode_t ble_sensor_report_register_callbacks(void)
{
    gap_ble_callbacks_t gap_callbacks = {0};
    gattc_callbacks_t gatt_callbacks = {0};
    errcode_t ret;

    gap_callbacks.ble_enable_cb = ble_sensor_report_enable_cb;
    gap_callbacks.ble_disable_cb = ble_sensor_report_disable_cb;
    gap_callbacks.set_scan_param_cb = ble_sensor_report_set_scan_param_cb;
    gap_callbacks.scan_result_cb = ble_sensor_report_scan_result_cb;
    gap_callbacks.conn_state_change_cb = ble_sensor_report_conn_state_cb;
    gap_callbacks.pair_result_cb = ble_sensor_report_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    gatt_callbacks.discovery_svc_cb = ble_sensor_report_discovery_service_cb;
    gatt_callbacks.discovery_chara_cb = ble_sensor_report_discovery_character_cb;
    gatt_callbacks.discovery_chara_cmp_cb = ble_sensor_report_discovery_character_complete_cb;
    gatt_callbacks.discovery_desc_cb = ble_sensor_report_discovery_descriptor_cb;
    gatt_callbacks.discovery_desc_cmp_cb = ble_sensor_report_discovery_descriptor_complete_cb;
    gatt_callbacks.mtu_changed_cb = ble_sensor_report_mtu_changed_cb;
    gatt_callbacks.notification_cb = ble_sensor_report_notification_cb;
    gatt_callbacks.read_cb = ble_sensor_report_read_cb;
    gatt_callbacks.write_cb = ble_sensor_report_write_cb;
    return gattc_register_callbacks(&gatt_callbacks);
}

/**
 * @if Eng
 * @brief Initializes scanning, connection, and GATT client callbacks.
 * @else
 * @brief 初始化扫描、连接和 GATT 客户端回调。
 * @endif
 */
errcode_t ble_sensor_report_client_init(void)
{
    errcode_t ret;
    ret = ble_sensor_report_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s callback registration failed: 0x%x\r\n", BLE_SENSOR_REPORT_CLIENT_LOG, ret);
        return ret;
    }
    if (ble_is_enable()) {
        g_stack_reset_done = true;
        osal_printk("%s BLE already enabled, resetting stack\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
        return disable_ble();
    }
    osal_printk("%s enabling BLE\r\n", BLE_SENSOR_REPORT_CLIENT_LOG);
    return enable_ble();
}
