/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements BLE UART bridge advertising data and startup.
 * @else
 * @brief 实现 BLE UART 透传广播数据构造与启动流程。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "securec.h"
#include "bts_le_gap.h"
#include "ble_uart_bridge_server.h"
#include "ble_uart_bridge_server_adv.h"

/* Connectable advertising parameters. / 可连接广播参数。 */
#define BLE_UART_BRIDGE_ADV_INTERVAL 0x30
#define BLE_UART_BRIDGE_ADV_CHANNEL_MAP 0x07
#define BLE_UART_BRIDGE_ADV_TYPE_CONN_UNDIR 0x00
#define BLE_UART_BRIDGE_ADV_FILTER_ALLOW_ALL 0x00
#define BLE_UART_BRIDGE_ADV_DURATION_FOREVER 0

/* Advertising-data field types and encoded lengths. / 广播数据字段类型与编码长度。 */
#define BLE_AD_TYPE_FLAGS 0x01
#define BLE_AD_TYPE_COMPLETE_UUID16 0x03
#define BLE_AD_TYPE_COMPLETE_NAME 0x09
#define BLE_AD_TYPE_SERVICE_DATA16 0x16
#define BLE_AD_FLAGS_GENERAL 0x06
#define BLE_UART_BRIDGE_STATE_DEFAULT 0x00
#define BLE_UART_BRIDGE_STATE_RETAINED 0x01
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_AD_FLAGS_FIELD_LEN 2
#define BLE_AD_UUID16_FIELD_LEN 3
#define BLE_AD_SERVICE_DATA_FIELD_LEN 4
#define BLE_AD_ELEMENT_HEADER_LEN 2
#define BLE_AD_FIXED_PAYLOAD_LEN 5

/* Advertising identity and retained-state flag. / 广播标识与保留状态标志。 */
static const uint8_t BLE_UART_BRIDGE_NAME[] = "uart1_bridge";
static bool g_ble_uart_bridge_default_state = true;

/**
 * @if Eng
 * @brief Updates the state encoded in subsequent advertising payloads.
 * @else
 * @brief 更新后续广播数据中编码的状态。
 * @endif
 */
void ble_uart_bridge_server_set_adv_default_state(bool is_default)
{
    g_ble_uart_bridge_default_state = is_default;
}

/**
 * @if Eng
 * @brief Builds flags, service UUID, device name, and service-state advertising fields.
 * @else
 * @brief 构造标志、服务 UUID、设备名与服务状态广播字段。
 * @endif
 */
static uint16_t ble_uart_bridge_build_adv_data(uint8_t *data, uint16_t capacity)
{
    uint16_t index = 0;
    uint16_t name_len = (uint16_t)(sizeof(BLE_UART_BRIDGE_NAME) - 1);

    /* Validate the complete encoded payload before writing the first byte. / 写入首字节前校验完整编码载荷空间。 */
    if (capacity < (uint16_t)(BLE_AD_UUID16_FIELD_LEN + BLE_AD_SERVICE_DATA_FIELD_LEN + name_len +
                              BLE_AD_ELEMENT_HEADER_LEN + BLE_AD_FIXED_PAYLOAD_LEN)) {
        return 0;
    }

    /* General-discoverable, BR/EDR-not-supported flags. / 通用可发现且不支持 BR/EDR 的标志字段。 */
    data[index++] = BLE_AD_FLAGS_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_FLAGS;
    data[index++] = BLE_AD_FLAGS_GENERAL;

    /* Complete 16-bit service UUID, encoded little-endian on air. / 完整 16 位服务 UUID，空口按小端序编码。 */
    data[index++] = BLE_AD_UUID16_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_COMPLETE_UUID16;
    data[index++] = (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID & 0xFF);
    data[index++] = (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT);

    /* The stable complete name is used as one of the client's identity checks. / 固定完整名称作为客户端身份校验之一。 */
    data[index++] = (uint8_t)(name_len + 1);
    data[index++] = BLE_AD_TYPE_COMPLETE_NAME;
    if (memcpy_s(&data[index], capacity - index, BLE_UART_BRIDGE_NAME, name_len) != EOK) {
        return 0;
    }
    index = (uint16_t)(index + name_len);

    /* Service data exposes cached-value state so the client can repair stale state before subscribing. */
    /* 服务数据发布缓存值状态，客户端可在订阅前修复陈旧状态。 */
    data[index++] = BLE_AD_SERVICE_DATA_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_SERVICE_DATA16;
    data[index++] = (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID & 0xFF);
    data[index++] = (uint8_t)(BLE_UART_BRIDGE_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT);
    data[index++] = g_ble_uart_bridge_default_state ? BLE_UART_BRIDGE_STATE_DEFAULT : BLE_UART_BRIDGE_STATE_RETAINED;
    return index;
}

/**
 * @if Eng
 * @brief Configures and starts connectable advertising.
 * @else
 * @brief 配置并启动可连接广播。
 * @endif
 */
errcode_t ble_uart_bridge_server_start_adv(void)
{
    uint8_t adv_data[31] = {0};
    uint16_t adv_len = ble_uart_bridge_build_adv_data(adv_data, sizeof(adv_data));
    gap_ble_config_adv_data_t config = {0};
    gap_ble_adv_params_t param = {0};
    errcode_t ret;

    if (adv_len == 0) {
        return ERRCODE_BT_FAIL;
    }

    /* Program the payload before parameters and start to avoid broadcasting stale bytes. / 先设置载荷，避免广播旧数据。 */
    config.adv_data = adv_data;
    config.adv_length = adv_len;
    ret = gap_ble_set_adv_data(BLE_UART_BRIDGE_ADV_ID, &config);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    /* Use a fixed interval on all three primary channels and advertise until connected. / 三个主广播信道持续等间隔广播。 */
    param.min_interval = BLE_UART_BRIDGE_ADV_INTERVAL;
    param.max_interval = BLE_UART_BRIDGE_ADV_INTERVAL;
    param.duration = BLE_UART_BRIDGE_ADV_DURATION_FOREVER;
    param.adv_type = BLE_UART_BRIDGE_ADV_TYPE_CONN_UNDIR;
    param.channel_map = BLE_UART_BRIDGE_ADV_CHANNEL_MAP;
    param.adv_filter_policy = BLE_UART_BRIDGE_ADV_FILTER_ALLOW_ALL;
    /* Undirected advertising does not target a peer, so clear the unused address. / 非定向广播无需目标地址，显式清零。 */
    param.peer_addr.type = 0;
    (void)memset_s(param.peer_addr.addr, sizeof(param.peer_addr.addr), 0, sizeof(param.peer_addr.addr));

    /* Parameters must be committed before issuing the start request. / 发起启动请求前先提交广播参数。 */
    ret = gap_ble_set_adv_param(BLE_UART_BRIDGE_ADV_ID, &param);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    return gap_ble_start_adv(BLE_UART_BRIDGE_ADV_ID);
}
