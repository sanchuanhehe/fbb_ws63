/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements the BLE UART bridge data receiving role.
 * @else
 * @brief 实现 BLE UART 透传数据接收端。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
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
#include "ble_uart_bridge.h"
#include "ble_uart_bridge_client.h"

/* UART bridge GATT identity and negotiated payload size. / UART 透传 GATT 标识与协商载荷长度。 */
#define BLE_UART_BRIDGE_CLIENT_LOG "[ble hello client]"
#define BLE_UART_BRIDGE_SERVICE_UUID 0x4444
#define BLE_UART_BRIDGE_DATA_UUID 0x4545
#define BLE_UART_BRIDGE_NOTIFY_UUID 0x4546
#define BLE_UART_BRIDGE_CCCD_UUID 0x2902
#define BLE_UART_BRIDGE_UUID_LEN 2
#define BLE_UART_BRIDGE_MTU 247

/* Scan and advertising-data decoding constants. / 扫描与广播数据解析常量。 */
#define BLE_UART_BRIDGE_SCAN_INTERVAL 0x30
#define BLE_UART_BRIDGE_AD_COMPLETE_NAME 0x09
#define BLE_UART_BRIDGE_AD_SERVICE_DATA16 0x16
#define BLE_UART_BRIDGE_STATE_DEFAULT 0x00
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_AD_HEADER_LEN 2
#define BLE_AD_SERVICE_DATA_FIELD_LEN 4
#define BLE_AD_SERVICE_DATA_STATE_OFFSET 4
#define BLE_AD_UUID_LOW_OFFSET 2
#define BLE_AD_UUID_HIGH_OFFSET 3

/* Target identity, GATT handles, and client state. / 目标标识、GATT 句柄与客户端状态。 */
static const uint8_t TARGET_NAME[] = "uart1_bridge";
static const uint8_t DEFAULT_VALUE[] = "uart_ready";
static const uint8_t NEW_VALUE[] = "uart_from_central";
static bt_uuid_t g_client_app_uuid = {BLE_UART_BRIDGE_UUID_LEN, {0x44, 0x44}};
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
static bool g_uart_write_pending;

/**
 * @if Eng
 * @brief Checks whether an SDK Bluetooth UUID equals a 16-bit value.
 * @else
 * @brief 检查 SDK 蓝牙 UUID 是否等于指定的 16 位数值。
 * @endif
 */
static bool ble_uart_bridge_uuid_is(const bt_uuid_t *uuid, uint16_t value)
{
    /* The SDK stores a 16-bit UUID in high-byte-first order. / SDK 的 16 位 UUID 按高字节在前保存。 */
    return uuid->uuid_len == BLE_UART_BRIDGE_UUID_LEN &&
           uuid->uuid[0] == (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT) && uuid->uuid[1] == (uint8_t)value;
}

/**
 * @if Eng
 * @brief Clears handles and flags associated with GATT discovery.
 * @else
 * @brief 清除 GATT 服务发现相关的句柄与状态标志。
 * @endif
 */
static void ble_uart_bridge_reset_discovery_state(void)
{
    /* Handles belong to one connection and must never be reused after reconnecting. / GATT 句柄仅属于当前连接，重连后不可复用。 */
    g_service_start_handle = 0;
    g_service_end_handle = 0;
    g_data_declare_handle = 0;
    g_data_handle = 0;
    g_notify_declare_handle = 0;
    g_notify_handle = 0;
    g_notify_cccd_handle = 0;
    /* Clear every one-shot gate so the new connection can run the full discovery pipeline. / 清除一次性门控，允许新连接完整发现。 */
    g_mtu_exchange_started = false;
    g_discovery_started = false;
    g_read_started = false;
    g_write_started = false;
    g_hello_cccd_started = false;
    g_cache_sync_write_started = false;
    g_uart_write_pending = false;
}

/**
 * @if Eng
 * @brief Parses advertising fields and identifies the UART bridge server.
 * @else
 * @brief 解析广播字段并识别 UART 透传服务端。
 * @endif
 */
static bool ble_uart_bridge_parse_adv(const uint8_t *data, uint8_t data_len, bool *default_state)
{
    uint16_t index = 0;
    const uint16_t target_len = sizeof(TARGET_NAME) - 1;
    bool name_matched = false;
    bool state_found = false;

    /* Advertising data is a sequence of length-prefixed AD structures. / 广播数据由一组带长度前缀的 AD 结构组成。 */
    while (index < data_len) {
        uint8_t field_len = data[index];
        uint16_t field_end;
        if (field_len == 0) {
            break;
        }
        field_end = (uint16_t)(index + field_len + 1);
        /* Validate the complete field before reading its type or payload. / 读取类型和载荷前先校验完整字段边界。 */
        if (field_end > data_len || field_len < 1) {
            break;
        }
        if (data[index + 1] == BLE_UART_BRIDGE_AD_COMPLETE_NAME && field_len == target_len + 1 &&
            memcmp(&data[index + BLE_AD_HEADER_LEN], TARGET_NAME, target_len) == 0) {
            name_matched = true;
        } else if (data[index + 1] == BLE_UART_BRIDGE_AD_SERVICE_DATA16 && field_len == BLE_AD_SERVICE_DATA_FIELD_LEN &&
                   data[index + BLE_AD_UUID_LOW_OFFSET] == (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID & 0xFF) &&
                   data[index + BLE_AD_UUID_HIGH_OFFSET] ==
                   (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT)) {
            /* Service data carries the server's retained-value state for cache synchronization. / 服务数据携带服务端保留值状态。 */
            *default_state = (data[index + BLE_AD_SERVICE_DATA_STATE_OFFSET] == BLE_UART_BRIDGE_STATE_DEFAULT);
            state_found = true;
        }
        index = field_end;
    }
    /* Requiring both fields avoids connecting to a same-name or same-UUID unrelated device. / 同时匹配名称和服务数据，避免误连。 */
    return name_matched && state_found;
}

/**
 * @if Eng
 * @brief Starts scanning for the UART bridge server.
 * @else
 * @brief 开始扫描 UART 透传服务端。
 * @endif
 */
static errcode_t ble_uart_bridge_start_scan(void)
{
    osal_printk("%s start scanning\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
    return gap_ble_start_scan();
}

/**
 * @if Eng
 * @brief Starts scanning after scan parameters are configured.
 * @else
 * @brief 在扫描参数配置完成后启动扫描。
 * @endif
 */
static void ble_uart_bridge_set_scan_param_cb(errcode_t status)
{
    if (status == ERRCODE_BT_SUCCESS) {
        (void)ble_uart_bridge_start_scan();
    } else {
        osal_printk("%s scan parameter failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
    }
}

/**
 * @if Eng
 * @brief Connects when a matching UART bridge advertisement is received.
 * @else
 * @brief 收到匹配的 UART 透传广播后发起连接。
 * @endif
 */
static void ble_uart_bridge_scan_result_cb(gap_scan_result_data_t *result)
{
    bool default_state = false;
    /* Ignore duplicate reports while connecting or connected. / 正在连接或已连接时忽略重复广播。 */
    if (g_connecting || g_connected || result == NULL || result->adv_data == NULL ||
        !ble_uart_bridge_parse_adv(result->adv_data, result->adv_len, &default_state)) {
        return;
    }

    /* Copy the address because the scan result storage is owned by the BLE callback. / 复制地址，避免持有 BLE 回调临时内存。 */
    if (memcpy_s(&g_peer_addr, sizeof(g_peer_addr), &result->addr, sizeof(result->addr)) != EOK) {
        return;
    }
    g_peer_default_state = default_state;
    g_connecting = true;
    osal_printk("%s found ble_uart_bridge_server, state=%s, connecting\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                g_peer_default_state ? "device_status_ok" : "retained");
    /* Stop scanning before connecting so later reports cannot start a second request. / 连接前停止扫描，防止再次发起连接。 */
    (void)gap_ble_stop_scan();
    if (gap_ble_connect_remote_device(&g_peer_addr) != ERRCODE_BT_SUCCESS) {
        /* Recover immediately when the connect request is rejected synchronously. / 连接请求同步失败时立即恢复扫描。 */
        g_connecting = false;
        (void)ble_uart_bridge_start_scan();
    }
}

/**
 * @if Eng
 * @brief Starts discovery of all GATT services on the connected peer.
 * @else
 * @brief 开始发现已连接对端的全部 GATT 服务。
 * @endif
 */
static errcode_t ble_uart_bridge_discover_service(uint16_t conn_id)
{
    /* A zero-length UUID requests enumeration of all services; filtering is done in the callback. / 空 UUID 表示枚举全部服务。 */
    bt_uuid_t uuid = {0};
    errcode_t ret = gattc_discovery_service(g_client_id, conn_id, &uuid);
    osal_printk("%s discover all services, ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
    return ret;
}

/**
 * @if Eng
 * @brief Starts MTU exchange for the active connection.
 * @else
 * @brief 为当前连接发起 MTU 交换。
 * @endif
 */
static errcode_t ble_uart_bridge_exchange_mtu(uint16_t conn_id)
{
    errcode_t ret;
    osal_printk("%s exchange MTU %u\r\n", BLE_UART_BRIDGE_CLIENT_LOG, BLE_UART_BRIDGE_MTU);
    /* Set the gate before the asynchronous request to tolerate an immediate callback. / 异步请求前设置门控，以兼容立即回调。 */
    g_mtu_exchange_started = true;
    ret = gattc_exchange_mtu_req(g_client_id, conn_id, BLE_UART_BRIDGE_MTU);
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
static void ble_uart_bridge_conn_state_cb(uint16_t conn_id,
                                          bd_addr_t *addr,
                                          gap_ble_conn_state_t conn_state,
                                          gap_ble_pair_state_t pair_state,
                                          gap_ble_disc_reason_t reason)
{
    errcode_t ret;
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        /* A new link starts with no valid peer handles. / 新连接建立时，旧的对端句柄全部失效。 */
        g_conn_id = conn_id;
        g_connected = true;
        g_connecting = false;
        ble_uart_bridge_reset_discovery_state();
        osal_printk("%s connected, conn_id=0x%04x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, conn_id);
        if (pair_state == GAP_BLE_PAIR_PAIRED) {
            /* Bonded peers can continue directly with MTU negotiation. / 已绑定设备可直接进入 MTU 协商。 */
            g_pairing_started = false;
            (void)ble_uart_bridge_exchange_mtu(conn_id);
        } else {
            /* Security is established before accessing the protected data characteristic. / 访问受保护特征前先完成配对。 */
            ret = gap_ble_pair_remote_device(addr);
            g_pairing_started = (ret == ERRCODE_BT_SUCCESS);
            osal_printk("%s pairing requested, ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        }
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        if (g_uart_write_pending) {
            /* Preserve queued UART bytes when the confirmed write loses its connection. / 确认写入断连时保留队列数据。 */
            g_uart_write_pending = false;
            ble_uart_bridge_ble_send_complete(ERRCODE_BT_FAIL);
        }
        if (g_pairing_started) {
            /* A link lost during pairing may leave an unusable bond; remove it before retrying. / 配对中断可能残留无效绑定。 */
            osal_printk("%s remove stale pair after pairing disconnect, ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                        gap_ble_remove_pair(addr));
        }
        g_pairing_started = false;
        g_connected = false;
        g_connecting = false;
        ble_uart_bridge_reset_discovery_state();
        /* Return to discovery automatically so the bridge recovers without rebooting. / 自动恢复扫描，使透传无需重启即可恢复。 */
        osal_printk("%s disconnected, reason=0x%x, restart scan\r\n", BLE_UART_BRIDGE_CLIENT_LOG, reason);
        (void)ble_uart_bridge_start_scan();
    }
}

/**
 * @if Eng
 * @brief Continues with MTU exchange after pairing completes.
 * @else
 * @brief 配对完成后继续执行 MTU 交换。
 * @endif
 */
static void ble_uart_bridge_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    g_pairing_started = false;
    osal_printk("%s pair complete, status=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
    if (status == ERRCODE_BT_SUCCESS) {
        /* Pairing completion is the next state transition in the setup pipeline. / 配对完成后进入 MTU 协商阶段。 */
        (void)ble_uart_bridge_exchange_mtu(conn_id);
        return;
    }
    /* Remove the failed bond before disconnecting so the next attempt pairs from a clean state. / 先删除失败绑定再断连。 */
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
static void ble_uart_bridge_mtu_changed_cb(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    (void)client_id;
    osal_printk("%s MTU changed: %u, status=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, mtu_size, status);
    /* The state gates make service discovery start exactly once for the current link. / 状态门控确保当前连接只发现一次服务。 */
    if (status == ERRCODE_BT_SUCCESS && g_connected && conn_id == g_conn_id && g_mtu_exchange_started &&
        !g_discovery_started) {
        g_discovery_started = true;
        (void)ble_uart_bridge_discover_service(conn_id);
    }
}

/**
 * @if Eng
 * @brief Records the handle range of the UART bridge service.
 * @else
 * @brief 记录 UART 透传服务的句柄范围。
 * @endif
 */
static void ble_uart_bridge_discovery_service_cb(uint8_t client_id,
                                                 uint16_t conn_id,
                                                 gattc_discovery_service_result_t *service,
                                                 errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    osal_printk("%s service candidate: uuid_len=%u uuid=%02x%02x handles=0x%04x-0x%04x status=0x%x\r\n",
                BLE_UART_BRIDGE_CLIENT_LOG, service->uuid.uuid_len, service->uuid.uuid[0], service->uuid.uuid[1],
                service->start_hdl, service->end_hdl, status);
    if (status != ERRCODE_BT_SUCCESS || !ble_uart_bridge_uuid_is(&service->uuid, BLE_UART_BRIDGE_SERVICE_UUID)) {
        return;
    }
    /* Save the service range because later characteristic discovery is scoped by its start handle. / 保存服务范围供特征发现使用。 */
    g_service_start_handle = service->start_hdl;
    g_service_end_handle = service->end_hdl;
    osal_printk("%s service discovered, handles=0x%04x-0x%04x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, service->start_hdl,
                service->end_hdl);
}

/**
 * @if Eng
 * @brief Starts characteristic discovery after all services are examined.
 * @else
 * @brief 完成全部服务检查后启动特征发现。
 * @endif
 */
static void ble_uart_bridge_discovery_service_complete_cb(uint8_t client_id,
                                                          uint16_t conn_id,
                                                          bt_uuid_t *uuid,
                                                          errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    errcode_t ret;

    (void)client_id;
    (void)uuid;
    if (status != ERRCODE_BT_SUCCESS || g_service_start_handle == 0 || g_service_end_handle == 0) {
        osal_printk("%s service discovery failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
        return;
    }
    /* A zero-length characteristic UUID enumerates every characteristic inside the service. / 空 UUID 表示枚举服务内全部特征。 */
    param.service_handle = g_service_start_handle;
    param.uuid.uuid_len = 0;
    ret = gattc_discovery_character(g_client_id, conn_id, &param);
    osal_printk("%s discover service characteristics, ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
}

/**
 * @if Eng
 * @brief Records discovered data and indication characteristic handles.
 * @else
 * @brief 记录发现的数据特征与指示特征句柄。
 * @endif
 */
static void ble_uart_bridge_discovery_character_cb(uint8_t client_id,
                                                   uint16_t conn_id,
                                                   gattc_discovery_character_result_t *character,
                                                   errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS) {
        return;
    }
    if (ble_uart_bridge_uuid_is(&character->uuid, BLE_UART_BRIDGE_DATA_UUID)) {
        /* The data value handle is used for client-to-server Write Commands. / 数据值句柄用于客户端向服务端发送写命令。 */
        g_data_declare_handle = character->declare_handle;
        g_data_handle = character->value_handle;
        osal_printk("%s data characteristic discovered, value=0x%04x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, g_data_handle);
    } else if (ble_uart_bridge_uuid_is(&character->uuid, BLE_UART_BRIDGE_NOTIFY_UUID)) {
        /* The indication value handle carries peripheral-to-central UART fragments. / 指示值句柄承载反向 UART 分片。 */
        g_notify_declare_handle = character->declare_handle;
        g_notify_handle = character->value_handle;
        osal_printk("%s notify characteristic discovered, value=0x%04x\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
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
static void ble_uart_bridge_discovery_character_complete_cb(uint8_t client_id,
                                                            uint16_t conn_id,
                                                            gattc_discovery_character_param_t *param,
                                                            errcode_t status)
{
    (void)client_id;
    (void)param;
    if (status != ERRCODE_BT_SUCCESS || g_data_declare_handle == 0 || g_notify_declare_handle == 0) {
        osal_printk("%s characteristic discovery failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
        return;
    }
    /* Descriptors following the indication declaration contain its CCCD. / 指示特征声明后的描述符中包含其 CCCD。 */
    (void)gattc_discovery_descriptor(g_client_id, conn_id, g_notify_declare_handle);
}

/**
 * @if Eng
 * @brief Records the indication CCCD handle.
 * @else
 * @brief 记录指示特征的 CCCD 句柄。
 * @endif
 */
static void ble_uart_bridge_discovery_descriptor_cb(uint8_t client_id,
                                                    uint16_t conn_id,
                                                    gattc_discovery_descriptor_result_t *descriptor,
                                                    errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    if (status == ERRCODE_BT_SUCCESS && ble_uart_bridge_uuid_is(&descriptor->uuid, BLE_UART_BRIDGE_CCCD_UUID)) {
        /* Store the CCCD handle separately from the characteristic value handle. / CCCD 句柄与特征值句柄分开保存。 */
        g_notify_cccd_handle = descriptor->descriptor_hdl;
        osal_printk("%s hello CCCD discovered, handle=0x%04x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, g_notify_cccd_handle);
    }
}

/**
 * @if Eng
 * @brief Writes the indication enable value to a CCCD.
 * @else
 * @brief 向 CCCD 写入指示使能值。
 * @endif
 */
static errcode_t ble_uart_bridge_enable_cccd(uint16_t conn_id, uint16_t handle, const char *name)
{
    /* CCCD is little-endian; 0x0002 enables indications rather than notifications. / CCCD 为小端序，0x0002 使能指示。 */
    uint8_t cccd_enable[2] = {2, 0};
    gattc_handle_value_t write_value = {0};
    write_value.handle = handle;
    write_value.data = cccd_enable;
    write_value.data_len = sizeof(cccd_enable);
    osal_printk("%s enabling %s CCCD\r\n", BLE_UART_BRIDGE_CLIENT_LOG, name);
    return gattc_write_req(g_client_id, conn_id, &write_value);
}

/**
 * @if Eng
 * @brief Synchronizes retained state before enabling UART data indications.
 * @else
 * @brief 在使能 UART 数据指示前同步保留状态。
 * @endif
 */
static void ble_uart_bridge_discovery_descriptor_complete_cb(uint8_t client_id,
                                                             uint16_t conn_id,
                                                             uint16_t characteristic_handle,
                                                             errcode_t status)
{
    gattc_handle_value_t write_value = {0};
    errcode_t ret;
    (void)client_id;
    (void)conn_id;
    if (status != ERRCODE_BT_SUCCESS || !g_connected || conn_id != g_conn_id || g_notify_cccd_handle == 0) {
        osal_printk("%s descriptor discovery failed, char=0x%04x, status=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                    characteristic_handle, status);
        return;
    }
    if (g_peer_default_state) {
        /*
         * Write the advertised default value first to synchronize a controller-side stale GATT cache;
         * only then enable indications. / 先写入广播声明的默认值以同步可能陈旧的 GATT 缓存，再使能指示。
         */
        write_value.handle = g_data_handle;
        write_value.data = (uint8_t *)DEFAULT_VALUE;
        write_value.data_len = sizeof(DEFAULT_VALUE) - 1;
        g_cache_sync_write_started = true;
        osal_printk("%s syncing data attribute: device_status_ok\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
        ret = gattc_write_req(g_client_id, conn_id, &write_value);
        if (ret != ERRCODE_BT_SUCCESS) {
            g_cache_sync_write_started = false;
            osal_printk("%s cache sync write request failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        }
        return;
    }
    /* A retained server value needs no cache repair, so subscribe immediately. / 服务端为保留值时无需修复缓存，直接订阅。 */
    g_hello_cccd_started = true;
    (void)ble_uart_bridge_enable_cccd(conn_id, g_notify_cccd_handle, "hello");
}

/**
 * @if Eng
 * @brief Queues received indication data for UART transmission.
 * @else
 * @brief 将收到的指示数据加入 UART 发送队列。
 * @endif
 */
static void ble_uart_bridge_notification_cb(uint8_t client_id,
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
    /* Queue data instead of writing UART in the BLE callback to keep the stack callback non-blocking. / 先入队，避免回调阻塞。 */
    ret = ble_uart_bridge_uart_enqueue(data->data, data->data_len);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s BLE notify UART queue failed: bytes=%u, ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                    data->data_len, ret);
    }
    if (!g_read_started) {
        /* The first indication proves subscription is active. / 首个指示用于确认订阅已生效。 */
        /* Continue with the sample's one-time value handshake. / 随后执行本案例的一次性数值握手。 */
        g_read_started = true;
        osal_printk("%s read request sent\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
        ret = gattc_read_req_by_handle(g_client_id, g_conn_id, g_data_handle);
        if (ret != ERRCODE_BT_SUCCESS) {
            g_read_started = false;
            osal_printk("%s read request failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        }
    }
}

/**
 * @if Eng
 * @brief Processes the initial data characteristic read and writes the peer state.
 * @else
 * @brief 处理数据特征初始读取结果并写入对端状态。
 * @endif
 */
static void ble_uart_bridge_read_cb(uint8_t client_id,
                                    uint16_t conn_id,
                                    gattc_handle_value_t *read_result,
                                    gatt_status_t status)
{
    gattc_handle_value_t write_value = {0};
    (void)client_id;
    if (status != GATT_STATUS_SUCCESS || read_result->handle != g_data_handle) {
        osal_printk("%s read failed, status=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
        return;
    }
    osal_printk("%s read result: %.*s\r\n", BLE_UART_BRIDGE_CLIENT_LOG, read_result->data_len, read_result->data);
    if (g_write_started) {
        /* Guard the demonstration write against repeated read callbacks. / 防止重复读取回调触发多次演示写入。 */
        return;
    }
    /* Keep this control write separate from UART payload Write Commands. / 此控制写请求与 UART 载荷写命令相互独立。 */
    g_write_started = true;
    write_value.handle = g_data_handle;
    write_value.data = (uint8_t *)NEW_VALUE;
    write_value.data_len = sizeof(NEW_VALUE) - 1;
    osal_printk("%s UART RX forwarded with BLE write: uart_from_central\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
    (void)gattc_write_req(g_client_id, conn_id, &write_value);
}

/**
 * @if Eng
 * @brief Enables indications after retained-state synchronization completes.
 * @else
 * @brief 保留状态同步完成后使能指示。
 * @endif
 */
static void ble_uart_bridge_cache_sync_write_complete(gatt_status_t status)
{
    g_cache_sync_write_started = false;
    osal_printk("%s cache sync write %s\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                status == GATT_STATUS_SUCCESS ? "success" : "failed");
    if ((status != GATT_STATUS_SUCCESS) || g_hello_cccd_started) {
        return;
    }

    /* Cache synchronization succeeded, so the indication channel can now be enabled safely. / 缓存同步成功后再安全使能指示。 */
    g_hello_cccd_started = true;
    if (ble_uart_bridge_enable_cccd(g_conn_id, g_notify_cccd_handle, "hello") != ERRCODE_BT_SUCCESS) {
        g_hello_cccd_started = false;
        osal_printk("%s hello CCCD write request failed\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
    }
}

/**
 * @if Eng
 * @brief Dispatches completion of data characteristic and CCCD write requests.
 * @else
 * @brief 分发数据特征与 CCCD 写请求的完成事件。
 * @endif
 */
static void ble_uart_bridge_write_cb(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    (void)client_id;
    if (handle == g_notify_cccd_handle) {
        /* CCCD writes and data writes share one completion callback; dispatch by handle. / CCCD 与数据写共用回调，按句柄分流。 */
        osal_printk("%s hello CCCD write %s\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
                    status == GATT_STATUS_SUCCESS ? "success" : "failed");
    } else if (handle == g_data_handle) {
        if (g_cache_sync_write_started) {
            ble_uart_bridge_cache_sync_write_complete(status);
            return;
        }
        if (g_uart_write_pending && conn_id == g_conn_id) {
            /* Release the next queued fragment only after the peer confirms this write. / 对端确认当前写入后才释放下一段队列数据。 */
            g_uart_write_pending = false;
            ble_uart_bridge_ble_send_complete(status == GATT_STATUS_SUCCESS ?
                                              ERRCODE_BT_SUCCESS : ERRCODE_BT_FAIL);
            return;
        }
        osal_printk("%s write cfm: %s\r\n", BLE_UART_BRIDGE_CLIENT_LOG,
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
static void ble_uart_bridge_enable_cb(errcode_t status)
{
    gap_ble_sec_params_t security = {0};
    gap_ble_scan_params_t scan_params = {0};
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
        return;
    }
    if (!g_stack_reset_done) {
        /* Cycle once to remove GATT state retained by an earlier sample in the shared stack. / 首次使能时重启协议栈清理旧状态。 */
        g_stack_reset_done = true;
        osal_printk("%s cycling BLE stack to clear retained GATT state\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
        ret = disable_ble();
        if (ret == ERRCODE_BT_SUCCESS) {
            return;
        }
        osal_printk("%s stack reset request failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
    }
    /* Use Just Works pairing because the boards have no input or display. / 单板无输入和显示能力，使用 Just Works 配对。 */
    security.bondable = 1;
    security.io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT;
    security.sc_enable = 0;
    security.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    ret = gap_ble_set_sec_param(&security);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s security config failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        return;
    }
    /* Register the GATT client only after security parameters are accepted. / 安全参数生效后再注册 GATT 客户端。 */
    ret = gattc_register_client(&g_client_app_uuid, &g_client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register client failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        return;
    }
    /* A window equal to the interval scans continuously during device discovery. / 扫描窗口等于周期，实现持续扫描。 */
    scan_params.scan_interval = BLE_UART_BRIDGE_SCAN_INTERVAL;
    scan_params.scan_window = BLE_UART_BRIDGE_SCAN_INTERVAL;
    scan_params.scan_type = GAP_BLE_SCAN_TYPE_ACTIVE;
    scan_params.scan_phy = GAP_BLE_PHY_1M;
    scan_params.scan_filter_policy = GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL;
    ret = gap_ble_set_scan_parameters(&scan_params);
    osal_printk("%s init ok, scan param ret=0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
}

/**
 * @if Eng
 * @brief Re-enables BLE after clearing retained GATT state.
 * @else
 * @brief 清理保留的 GATT 状态后重新使能 BLE。
 * @endif
 */
static void ble_uart_bridge_disable_cb(errcode_t status)
{
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s disable failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, status);
        return;
    }
    osal_printk("%s BLE stack reset complete, enabling BLE\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
    ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable request failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Registers GAP and GATT client callbacks used by this sample.
 * @else
 * @brief 注册本案例使用的 GAP 和 GATT 客户端回调。
 * @endif
 */
static errcode_t ble_uart_bridge_register_callbacks(void)
{
    gap_ble_callbacks_t gap_callbacks = {0};
    gattc_callbacks_t gatt_callbacks = {0};
    errcode_t ret;

    /* GAP callbacks drive stack lifecycle, scanning, connection, and pairing. / GAP 回调驱动协议栈、扫描、连接和配对。 */
    gap_callbacks.ble_enable_cb = ble_uart_bridge_enable_cb;
    gap_callbacks.ble_disable_cb = ble_uart_bridge_disable_cb;
    gap_callbacks.set_scan_param_cb = ble_uart_bridge_set_scan_param_cb;
    gap_callbacks.scan_result_cb = ble_uart_bridge_scan_result_cb;
    gap_callbacks.conn_state_change_cb = ble_uart_bridge_conn_state_cb;
    gap_callbacks.pair_result_cb = ble_uart_bridge_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    /* GATT callbacks advance discovery and carry attribute data after connection. / GATT 回调推进发现流程并传递属性数据。 */
    gatt_callbacks.discovery_svc_cb = ble_uart_bridge_discovery_service_cb;
    gatt_callbacks.discovery_svc_cmp_cb = ble_uart_bridge_discovery_service_complete_cb;
    gatt_callbacks.discovery_chara_cb = ble_uart_bridge_discovery_character_cb;
    gatt_callbacks.discovery_chara_cmp_cb = ble_uart_bridge_discovery_character_complete_cb;
    gatt_callbacks.discovery_desc_cb = ble_uart_bridge_discovery_descriptor_cb;
    gatt_callbacks.discovery_desc_cmp_cb = ble_uart_bridge_discovery_descriptor_complete_cb;
    gatt_callbacks.mtu_changed_cb = ble_uart_bridge_mtu_changed_cb;
    gatt_callbacks.notification_cb = ble_uart_bridge_notification_cb;
    gatt_callbacks.indication_cb = ble_uart_bridge_notification_cb;
    gatt_callbacks.read_cb = ble_uart_bridge_read_cb;
    gatt_callbacks.write_cb = ble_uart_bridge_write_cb;
    return gattc_register_callbacks(&gatt_callbacks);
}

/**
 * @if Eng
 * @brief Initializes scanning, connection, and GATT client callbacks.
 * @else
 * @brief 初始化扫描、连接和 GATT 客户端回调。
 * @endif
 */
errcode_t ble_uart_bridge_client_init(void)
{
    errcode_t ret;
    ret = ble_uart_bridge_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s callback registration failed: 0x%x\r\n", BLE_UART_BRIDGE_CLIENT_LOG, ret);
        return ret;
    }
    if (ble_is_enable()) {
        /* Reuse the disable callback to perform the same clean enable sequence. / 通过禁用回调复用完整的干净使能流程。 */
        g_stack_reset_done = true;
        osal_printk("%s BLE already enabled, resetting stack\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
        return disable_ble();
    }
    osal_printk("%s enabling BLE\r\n", BLE_UART_BRIDGE_CLIENT_LOG);
    return enable_ble();
}

/**
 * @if Eng
 * @brief Sends one queued UART fragment through a confirmed GATT write request.
 * @else
 * @brief 通过有确认的 GATT 写请求发送一段已排队的 UART 数据。
 * @endif
 */
errcode_t ble_uart_bridge_client_send_write(const uint8_t *data, uint16_t length)
{
    gattc_handle_value_t write_value = {0};
    errcode_t ret;

    if (!g_connected || g_data_handle == 0 || g_uart_write_pending || data == NULL || length == 0 ||
        length > BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN) {
        return ERRCODE_BT_FAIL;
    }
    /*
     * Mark the request before submission because the SDK may report completion immediately.
     * 提交前先标记请求，兼容 SDK 立即上报完成事件的情况。
     */
    g_uart_write_pending = true;
    write_value.handle = g_data_handle;
    write_value.data = (uint8_t *)data;
    write_value.data_len = length;
    ret = gattc_write_req(g_client_id, g_conn_id, &write_value);
    if (ret != ERRCODE_BT_SUCCESS) {
        /* A rejected request has no completion callback; let the worker retry the same bytes. / 请求被拒绝时不会回调，保留原数据重试。 */
        g_uart_write_pending = false;
    }
    return ret;
}
