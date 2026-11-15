/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements the BLE UART bridge GATT server.
 * @else
 * @brief 实现 BLE UART 透传 GATT 服务端。
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
#include "bts_gatt_server.h"
#include "ble_uart_bridge.h"
#include "ble_uart_bridge_server.h"
#include "ble_uart_bridge_server_adv.h"

/* UART bridge service identity. / UART 透传服务标识。 */
#define BLE_UART_BRIDGE_SERVER_LOG "[ble hello server]"
#define BLE_UART_BRIDGE_UUID_LEN 2

/* GATT handles, connection state, and data values. / GATT 句柄、连接状态与数据值。 */
static uint8_t g_server_id;
static uint16_t g_conn_id;
static uint16_t g_service_handle;
static uint16_t g_data_handle;
static uint16_t g_notify_handle;
static uint16_t g_notify_cccd_handle;
static bool g_connected;
static bool g_hello_notify_enabled;
static bool g_stack_reset_done;
static const bd_addr_t g_ble_uart_bridge_addr = {
    .addr = {0x44, 0x44, 0x55, 0x41, 0x52, 0x63},
    .type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS,
};
static uint8_t g_property_value[BLE_UART_BRIDGE_PROPERTY_MAX_LEN] = "uart_ready";
static uint16_t g_property_value_len = sizeof("uart_ready") - 1;
/* The SDK response path requires a nonzero copy span even when an ATT Write Response has no value. */
static uint8_t g_empty_response_value;
static const uint8_t DEFAULT_VALUE[] = "uart_ready";
static const uint8_t HELLO_MESSAGE[] = "uart_from_peripheral";
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_CCCD_VALUE_LEN 2
#define BLE_CCCD_INDICATE_ENABLED 2

/**
 * @if Eng
 * @brief Describes the payload used to send one GATT response.
 * @else
 * @brief 描述发送一条 GATT 响应所需的数据。
 * @endif
 */
typedef struct {
    uint16_t request_id;
    uint8_t status;
    uint8_t *value;
    uint16_t value_len;
} ble_uart_bridge_response_t;

/**
 * @if Eng
 * @brief Declares the helper used to send an indication value.
 * @else
 * @brief 声明用于发送指示值的内部辅助接口。
 * @endif
 */
static errcode_t ble_uart_bridge_send_value_notification(uint16_t handle,
                                                         const uint8_t *data,
                                                         uint16_t len,
                                                         const char *name);

/**
 * @if Eng
 * @brief Converts a 16-bit value to the SDK Bluetooth UUID representation.
 * @else
 * @brief 将 16 位数值转换为 SDK 蓝牙 UUID 表示形式。
 * @endif
 */
static void ble_uart_bridge_uuid16(uint16_t value, bt_uuid_t *uuid)
{
    /* The SDK represents 16-bit UUID bytes in high-byte-first order. / SDK 的 16 位 UUID 字节按高字节在前表示。 */
    uuid->uuid_len = BLE_UART_BRIDGE_UUID_LEN;
    uuid->uuid[0] = (uint8_t)(value >> BLE_UUID_HIGH_BYTE_SHIFT);
    uuid->uuid[1] = (uint8_t)value;
}

/**
 * @if Eng
 * @brief Sends a GATT response for a read or write request.
 * @else
 * @brief 为 GATT 读写请求发送响应。
 * @endif
 */
static errcode_t ble_uart_bridge_send_response(uint8_t server_id,
                                               uint16_t conn_id,
                                               const ble_uart_bridge_response_t *response_data)
{
    gatts_send_rsp_t response = {0};
    /* This sample always returns the complete value, so the response starts at offset zero. / 本案例始终返回完整值，偏移为零。 */
    response.request_id = response_data->request_id;
    response.status = response_data->status;
    response.offset = 0;
    response.value = response_data->value;
    response.value_len = response_data->value_len;
    if (response.value_len == 0) {
        /* The SDK copies before request dispatch and rejects zero-sized memcpy_s. / SDK 分发请求前先复制，拒绝零长度 memcpy_s。 */
        response.value = &g_empty_response_value;
        response.value_len = sizeof(g_empty_response_value);
    }
    return gatts_send_response(server_id, conn_id, &response);
}

/**
 * @if Eng
 * @brief Handles reads of the UART data characteristic.
 * @else
 * @brief 处理 UART 数据特征的读取请求。
 * @endif
 */
static void ble_uart_bridge_read_request_cb(uint8_t server_id,
                                            uint16_t conn_id,
                                            gatts_req_read_cb_t *request,
                                            errcode_t status)
{
    (void)status;
    osal_printk("%s read request received, handle=0x%04x\r\n", BLE_UART_BRIDGE_SERVER_LOG, request->handle);
    if (!request->need_rsp) {
        /* The stack does not expect an ATT response for this request type. / 当前请求类型无需返回 ATT 响应。 */
        return;
    }

    if (request->handle != g_data_handle) {
        /* Never expose the cached UART value through an unrelated handle. / 不允许通过无关句柄读取缓存的 UART 数据。 */
        ble_uart_bridge_response_t response = {request->request_id, GATT_STATUS_INVALID_HANDLE, NULL, 0};
        (void)ble_uart_bridge_send_response(server_id, conn_id, &response);
        return;
    }

    /* Return the latest accepted data-characteristic value. / 返回最近一次成功接收的数据特征值。 */
    ble_uart_bridge_response_t response = {request->request_id, GATT_STATUS_SUCCESS, g_property_value,
                                           g_property_value_len};
    if (ble_uart_bridge_send_response(server_id, conn_id, &response) == ERRCODE_BT_SUCCESS) {
        osal_printk("%s read response sent: value=%.*s\r\n", BLE_UART_BRIDGE_SERVER_LOG, g_property_value_len,
                    g_property_value);
    }
}

/**
 * @if Eng
 * @brief Validates a CCCD write and updates the indication state.
 * @else
 * @brief 校验 CCCD 写请求并更新指示使能状态。
 * @endif
 */
static void ble_uart_bridge_handle_cccd_write(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *request)
{
    uint16_t cccd_value = 0;
    uint8_t response_status = GATT_STATUS_SUCCESS;

    if (request->length != BLE_CCCD_VALUE_LEN) {
        response_status = GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH;
    } else {
        /* CCCD is a little-endian 16-bit bitmask. / CCCD 是小端序的 16 位位图。 */
        cccd_value = (uint16_t)request->value[0] | ((uint16_t)request->value[1] << BLE_UUID_HIGH_BYTE_SHIFT);
        /* Accept only disabled (0x0000) or indication enabled (0x0002). / 仅接受禁用或使能指示。 */
        if (cccd_value != 0 && cccd_value != BLE_CCCD_INDICATE_ENABLED) {
            response_status = GATT_STATUS_VALUE_NOT_ALLOWED;
        }
    }

    if (request->need_rsp) {
        /* A CCCD Write Request must be acknowledged before changing local state. / CCCD 写请求需先应答再更新本地状态。 */
        ble_uart_bridge_response_t response = {request->request_id, response_status, NULL, 0};
        (void)ble_uart_bridge_send_response(server_id, conn_id, &response);
    }
    if (response_status != GATT_STATUS_SUCCESS) {
        osal_printk("%s invalid CCCD write, status=0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, response_status);
        return;
    }

    /* Update the subscription gate only after value validation succeeds. / 仅在校验成功后更新订阅门控。 */
    g_hello_notify_enabled = (cccd_value == BLE_CCCD_INDICATE_ENABLED);
    osal_printk("%s UART RX indication CCCD %s\r\n", BLE_UART_BRIDGE_SERVER_LOG,
                g_hello_notify_enabled ? "enabled" : "disabled");
    if (g_hello_notify_enabled) {
        /* The initial indication confirms the data path and starts the client handshake. / 首个指示用于确认通道并启动客户端握手。 */
        (void)ble_uart_bridge_server_send_notification(HELLO_MESSAGE, sizeof(HELLO_MESSAGE) - 1);
    }
}

/**
 * @if Eng
 * @brief Queues data characteristic writes for UART transmission.
 * @else
 * @brief 将数据特征写入内容加入 UART 发送队列。
 * @endif
 */
static void ble_uart_bridge_write_request_cb(uint8_t server_id,
                                             uint16_t conn_id,
                                             gatts_req_write_cb_t *request,
                                             errcode_t status)
{
    uint8_t response_status = GATT_STATUS_SUCCESS;
    errcode_t response_ret;

    if (status != ERRCODE_BT_SUCCESS) {
        /* Preserve the ATT request metadata when the stack reports a callback error. / 协议栈回调异常时保留 ATT 请求元数据。 */
        osal_printk("%s write callback failed: ret=0x%x, request_id=%u, need_rsp=%u, authorize=%u, prepare=%u\r\n",
                    BLE_UART_BRIDGE_SERVER_LOG, status, request->request_id, request->need_rsp,
                    request->need_authorize, request->is_prep);
    }

    osal_printk("%s write request received, handle=0x%04x, len=%u\r\n", BLE_UART_BRIDGE_SERVER_LOG, request->handle,
                request->length);
    if (request->handle == g_notify_cccd_handle) {
        /* CCCD writes configure indications and are not UART payload. / CCCD 写用于配置指示，不属于 UART 载荷。 */
        ble_uart_bridge_handle_cccd_write(server_id, conn_id, request);
        return;
    }
    if (request->handle != g_data_handle) {
        response_status = GATT_STATUS_INVALID_HANDLE;
    } else if (request->length == 0 || request->length > BLE_UART_BRIDGE_PROPERTY_MAX_LEN) {
        response_status = GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH;
    } else if (ble_uart_bridge_uart_enqueue(request->value, request->length) != ERRCODE_BT_SUCCESS) {
        /* Report backpressure to Write Requests when the UART queue has no room. / UART 队列空间不足时向写请求返回资源不足。 */
        response_status = GATT_STATUS_INSUFFICIENT_RESOURCES;
    } else {
        /* Update the readable cache only after the complete fragment is queued for UART. / 完整分片成功入队后再更新可读缓存。 */
        (void)memset_s(g_property_value, sizeof(g_property_value), 0, sizeof(g_property_value));
        if (memcpy_s(g_property_value, sizeof(g_property_value), request->value, request->length) != EOK) {
            response_status = GATT_STATUS_UNLIKELY_ERROR;
        } else {
            g_property_value_len = request->length;
        }
    }

    if (request->need_rsp) {
        /* Write Commands skip this response, while Write Requests receive the mapped status. / 写命令无响应，写请求返回映射状态。 */
        ble_uart_bridge_response_t response = {request->request_id, response_status, NULL, 0};
        response_ret = ble_uart_bridge_send_response(server_id, conn_id, &response);
        if (response_ret != ERRCODE_BT_SUCCESS) {
            /* A failed ATT response stalls the client's single-flight Write Request queue. / ATT 应答失败会阻塞客户端的单请求发送队列。 */
            osal_printk("%s write response failed: ret=0x%x, request_id=%u, offset=%u, authorize=%u, prepare=%u\r\n",
                        BLE_UART_BRIDGE_SERVER_LOG, response_ret, request->request_id, request->offset,
                        request->need_authorize, request->is_prep);
        }
    }
    if (response_status == GATT_STATUS_SUCCESS) {
        /* Mirror the cached-value state into future advertisements for reconnect recovery. / 将缓存状态同步到后续广播以支持重连恢复。 */
        ble_uart_bridge_server_set_adv_default_state(
            g_property_value_len == sizeof(DEFAULT_VALUE) - 1 &&
            memcmp(g_property_value, DEFAULT_VALUE, sizeof(DEFAULT_VALUE) - 1) == 0);
        osal_printk("%s BLE write queued to UART TX: bytes=%u\r\n", BLE_UART_BRIDGE_SERVER_LOG,
                    g_property_value_len);
    } else {
        osal_printk("%s write rejected, status=0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, response_status);
    }
}

/**
 * @if Eng
 * @brief Adds the readable and writable UART data characteristic.
 * @else
 * @brief 添加支持读写的 UART 数据特征。
 * @endif
 */
static errcode_t ble_uart_bridge_add_data_characteristic(void)
{
    bt_uuid_t data_uuid = {0};
    gatts_add_chara_info_t characteristic = {0};
    gatts_add_character_result_t data_result = {0};

    ble_uart_bridge_uuid16(BLE_UART_BRIDGE_DATA_UUID, &data_uuid);
    characteristic.chara_uuid = data_uuid;
    /* Support both control Write Requests and high-throughput UART Write Commands. / 同时支持控制写请求和高吞吐 UART 写命令。 */
    characteristic.properties = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE |
                                GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
    /* Authorization forces protected access after the configured pairing procedure. / 授权权限要求在配置的配对流程后访问。 */
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

/**
 * @if Eng
 * @brief Adds the UART indication characteristic and its CCCD.
 * @else
 * @brief 添加 UART 指示特征及其 CCCD。
 * @endif
 */
static errcode_t ble_uart_bridge_add_notify_characteristic(void)
{
    bt_uuid_t notify_uuid = {0};
    bt_uuid_t cccd_uuid = {0};
    gatts_add_chara_info_t characteristic = {0};
    gatts_add_character_result_t notify_result = {0};
    gatts_add_desc_info_t descriptor = {0};
    uint8_t cccd_value[BLE_CCCD_VALUE_LEN] = {0};

    ble_uart_bridge_uuid16(BLE_UART_BRIDGE_NOTIFY_UUID, &notify_uuid);
    characteristic.chara_uuid = notify_uuid;
    /* Indications provide an explicit confirmation used to advance the UART RX ring safely. / 指示确认用于安全推进 UART 接收队列。 */
    characteristic.properties = GATT_CHARACTER_PROPERTY_BIT_INDICATE;
    characteristic.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    characteristic.value = (uint8_t *)HELLO_MESSAGE;
    characteristic.value_len = sizeof(HELLO_MESSAGE) - 1;
    errcode_t ret = gatts_add_characteristic_sync(g_server_id, g_service_handle, &characteristic, &notify_result);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    g_notify_handle = notify_result.value_handle;

    /* CCCD lets the client explicitly enable or disable indication delivery. / CCCD 允许客户端显式控制指示发送。 */
    ble_uart_bridge_uuid16(BLE_UART_BRIDGE_CCCD_UUID, &cccd_uuid);
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

/**
 * @if Eng
 * @brief Registers and starts the UART bridge GATT service.
 * @else
 * @brief 注册并启动 UART 透传 GATT 服务。
 * @endif
 */
static errcode_t ble_uart_bridge_add_gatt_service(void)
{
    bt_uuid_t app_uuid = {0};
    bt_uuid_t service_uuid = {0};

    /* Register the application before creating attributes owned by its server ID. / 创建属性前先注册应用并获取服务端 ID。 */
    ble_uart_bridge_uuid16(BLE_UART_BRIDGE_SERVICE_UUID, &app_uuid);
    errcode_t ret = gatts_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    ble_uart_bridge_uuid16(BLE_UART_BRIDGE_SERVICE_UUID, &service_uuid);
    ret = gatts_add_service_sync(g_server_id, &service_uuid, true, &g_service_handle);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    /* Build the service in dependency order: service, data value, indication value, then CCCD. / 按依赖顺序构建服务。 */
    ret = ble_uart_bridge_add_data_characteristic();
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    ret = ble_uart_bridge_add_notify_characteristic();
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    osal_printk("%s service ready: service=0x%04x data=0x%04x notify=0x%04x notify_cccd=0x%04x\r\n",
                BLE_UART_BRIDGE_SERVER_LOG, g_service_handle, g_data_handle, g_notify_handle, g_notify_cccd_handle);
    return gatts_start_service(g_server_id, g_service_handle);
}

/**
 * @if Eng
 * @brief Starts advertising after the GATT service becomes available.
 * @else
 * @brief 在 GATT 服务就绪后启动广播。
 * @endif
 */
static void ble_uart_bridge_service_start_cb(uint8_t server_id, uint16_t handle, errcode_t status)
{
    if (server_id != g_server_id || handle != g_service_handle) {
        return;
    }
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s service start failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, status);
        return;
    }
    /* Advertising starts only after every characteristic and descriptor is active. / 全部特征和描述符生效后才开始广播。 */
    if (ble_uart_bridge_server_start_adv() == ERRCODE_BT_SUCCESS) {
        osal_printk("%s advertising started: ble_uart_bridge_server\r\n", BLE_UART_BRIDGE_SERVER_LOG);
    } else {
        osal_printk("%s advertising start failed\r\n", BLE_UART_BRIDGE_SERVER_LOG);
    }
}

/**
 * @if Eng
 * @brief Tracks connection state and restarts advertising after disconnection.
 * @else
 * @brief 跟踪连接状态并在断开后重新启动广播。
 * @endif
 */
static void ble_uart_bridge_conn_state_cb(uint16_t conn_id,
                                          bd_addr_t *addr,
                                          gap_ble_conn_state_t conn_state,
                                          gap_ble_pair_state_t pair_state,
                                          gap_ble_disc_reason_t reason)
{
    (void)addr;
    (void)pair_state;
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        /* CCCD subscription is connection-specific and must start disabled on every link. / CCCD 订阅属于单次连接，重连后需重置。 */
        g_conn_id = conn_id;
        g_connected = true;
        g_hello_notify_enabled = false;
        osal_printk("%s connected, conn_id=0x%04x\r\n", BLE_UART_BRIDGE_SERVER_LOG, conn_id);
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        /* Fail the in-flight indication without consuming its bytes. / 将在途指示标记失败但不消费数据。 */
        /* The worker can retry the same bytes after reconnecting. / 重连后任务可重试相同数据。 */
        ble_uart_bridge_ble_send_complete(ERRCODE_BT_FAIL);
        g_connected = false;
        g_hello_notify_enabled = false;
        /* Resume discoverability automatically after the active link is released. / 当前连接释放后自动恢复可发现状态。 */
        osal_printk("%s disconnected, reason=0x%x, re-advertising\r\n", BLE_UART_BRIDGE_SERVER_LOG, reason);
        (void)ble_uart_bridge_server_start_adv();
    }
}

/**
 * @if Eng
 * @brief Reports completion of one confirmed UART-data indication.
 * @else
 * @brief 上报一包 UART 数据指示的确认结果。
 * @endif
 */
static void ble_uart_bridge_indication_confirm_cb(uint8_t server_id, uint16_t conn_id, errcode_t status)
{
    /* Ignore confirmations belonging to another server instance or an obsolete connection. / 忽略其他实例或旧连接的确认。 */
    if (server_id == g_server_id && conn_id == g_conn_id) {
        ble_uart_bridge_ble_send_complete(status);
    }
}

/**
 * @if Eng
 * @brief Handles pairing completion and removes stale pairing information on failure.
 * @else
 * @brief 处理配对完成事件，并在失败时删除陈旧配对信息。
 * @endif
 */
static void ble_uart_bridge_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete, conn_id=0x%04x, status=0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, conn_id, status);
    if (status != ERRCODE_BT_SUCCESS) {
        /* A failed bond is removed so the next connection can negotiate fresh keys. / 删除失败绑定，便于下次重新协商密钥。 */
        osal_printk("%s remove stale pair, ret=0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, gap_ble_remove_pair(addr));
    }
}

/**
 * @if Eng
 * @brief Configures security, the local address, and the GATT service after BLE is enabled.
 * @else
 * @brief BLE 使能后配置安全参数、本地地址与 GATT 服务。
 * @endif
 */
static void ble_uart_bridge_enable_cb(errcode_t status)
{
    gap_ble_sec_params_t security = {0};
    errcode_t ret;
    if (status != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, status);
        return;
    }
    if (!g_stack_reset_done) {
        /* Cycle once to clear attributes retained by another sample in the shared BLE stack. / 首次使能时重启协议栈清理旧属性。 */
        g_stack_reset_done = true;
        osal_printk("%s cycling BLE stack to clear retained GATT state\r\n", BLE_UART_BRIDGE_SERVER_LOG);
        ret = disable_ble();
        if (ret == ERRCODE_BT_SUCCESS) {
            return;
        }
        osal_printk("%s stack reset request failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret);
    }
    /* Encode the retained cache state before advertising begins. / 开始广播前编码保留缓存状态。 */
    ble_uart_bridge_server_set_adv_default_state(g_property_value_len == sizeof(DEFAULT_VALUE) - 1 &&
                                                 memcmp(g_property_value, DEFAULT_VALUE, sizeof(DEFAULT_VALUE) - 1) ==
                                                     0);
    /* Use Just Works pairing because the board exposes no input or display. / 单板无输入和显示能力，使用 Just Works 配对。 */
    security.bondable = 1;
    security.io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT;
    security.sc_enable = 0;
    security.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    ret = gap_ble_set_sec_param(&security);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s security config failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret);
        return;
    }
    /* A fixed sample address makes reconnect behavior deterministic during development. / 固定案例地址便于调试重连行为。 */
    ret = gap_ble_set_local_addr(&g_ble_uart_bridge_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s local address config failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret);
        return;
    }
    ret = ble_uart_bridge_add_gatt_service();
    osal_printk("%s init %s, ret=0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret == ERRCODE_BT_SUCCESS ? "ok" : "failed",
                ret);
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
        osal_printk("%s disable failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, status);
        return;
    }
    osal_printk("%s BLE stack reset complete, enabling BLE\r\n", BLE_UART_BRIDGE_SERVER_LOG);
    ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable request failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Registers GAP and GATT server callbacks used by this sample.
 * @else
 * @brief 注册本案例使用的 GAP 和 GATT 服务端回调。
 * @endif
 */
static errcode_t ble_uart_bridge_register_callbacks(void)
{
    gap_ble_callbacks_t gap_callbacks = {0};
    gatts_callbacks_t gatt_callbacks = {0};
    errcode_t ret;

    /* GAP callbacks own stack lifecycle, connection state, and pairing. / GAP 回调管理协议栈、连接和配对。 */
    gap_callbacks.ble_enable_cb = ble_uart_bridge_enable_cb;
    gap_callbacks.ble_disable_cb = ble_uart_bridge_disable_cb;
    gap_callbacks.conn_state_change_cb = ble_uart_bridge_conn_state_cb;
    gap_callbacks.pair_result_cb = ble_uart_bridge_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    /* GATT callbacks serve attributes and close the indication flow-control loop. / GATT 回调处理属性并闭合指示流控。 */
    gatt_callbacks.start_service_cb = ble_uart_bridge_service_start_cb;
    gatt_callbacks.read_request_cb = ble_uart_bridge_read_request_cb;
    gatt_callbacks.write_request_cb = ble_uart_bridge_write_request_cb;
    gatt_callbacks.indicate_confirm_cb = ble_uart_bridge_indication_confirm_cb;
    return gatts_register_callbacks(&gatt_callbacks);
}

/**
 * @if Eng
 * @brief Sends an indication using the specified characteristic handle.
 * @else
 * @brief 使用指定特征句柄发送指示。
 * @endif
 */
static errcode_t ble_uart_bridge_send_value_notification(uint16_t handle,
                                                         const uint8_t *data,
                                                         uint16_t len,
                                                         const char *name)
{
    gatts_ntf_ind_t notification = {0};
    errcode_t ret;
    /* Reject data until the link and configured payload bounds are valid. / 连接或载荷边界无效时拒绝发送。 */
    if (!g_connected || data == NULL || len == 0 || len > BLE_UART_BRIDGE_BLE_PAYLOAD_MAX_LEN) {
        return ERRCODE_BT_FAIL;
    }
    notification.attr_handle = handle;
    notification.value = (uint8_t *)data;
    notification.value_len = len;
    /* The characteristic property selects indication semantics for this API call. / 该接口依据特征属性执行指示发送。 */
    ret = gatts_notify_indicate(g_server_id, g_conn_id, &notification);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s %s failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, name, ret);
    }
    return ret;
}

/**
 * @if Eng
 * @brief Sends one UART data fragment to the connected data receiving role.
 * @else
 * @brief 向已连接的数据接收端发送一段 UART 数据。
 * @endif
 */
errcode_t ble_uart_bridge_server_send_notification(const uint8_t *data, uint16_t len)
{
    /* Do not enqueue an ATT indication until the peer has enabled its CCCD. / 对端未使能 CCCD 时不发送 ATT 指示。 */
    if (!g_hello_notify_enabled) {
        return ERRCODE_BT_FAIL;
    }
    return ble_uart_bridge_send_value_notification(g_notify_handle, data, len, "notification");
}

/**
 * @if Eng
 * @brief Initializes GATT server callbacks and enables the BLE stack.
 * @else
 * @brief 初始化 GATT 服务端回调并使能 BLE 协议栈。
 * @endif
 */
errcode_t ble_uart_bridge_server_init(void)
{
    errcode_t ret = ble_uart_bridge_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s callback registration failed: 0x%x\r\n", BLE_UART_BRIDGE_SERVER_LOG, ret);
        return ret;
    }
    if (ble_is_enable()) {
        /* Reuse the disable callback to enter the same clean enable sequence. / 通过禁用回调复用干净使能流程。 */
        g_stack_reset_done = true;
        osal_printk("%s BLE already enabled, resetting stack\r\n", BLE_UART_BRIDGE_SERVER_LOG);
        return disable_ble();
    }
    osal_printk("%s enabling BLE\r\n", BLE_UART_BRIDGE_SERVER_LOG);
    return enable_ble();
}
