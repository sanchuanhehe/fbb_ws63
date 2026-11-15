/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello advertising configuration.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "securec.h"
#include "bts_le_gap.h"
#include "ble_hello_server.h"
#include "ble_hello_server_adv.h"

#define BLE_HELLO_ADV_INTERVAL 0x30
#define BLE_HELLO_ADV_CHANNEL_MAP 0x07
#define BLE_HELLO_ADV_TYPE_CONN_UNDIR 0x00
#define BLE_HELLO_ADV_FILTER_ALLOW_ALL 0x00
#define BLE_HELLO_ADV_DURATION_FOREVER 0
#define BLE_AD_TYPE_FLAGS 0x01
#define BLE_AD_TYPE_COMPLETE_UUID16 0x03
#define BLE_AD_TYPE_COMPLETE_NAME 0x09
#define BLE_AD_TYPE_SERVICE_DATA16 0x16
#define BLE_AD_FLAGS_GENERAL 0x06
#define BLE_HELLO_STATE_DEFAULT 0x00
#define BLE_HELLO_STATE_RETAINED 0x01
#define BLE_UUID_HIGH_BYTE_SHIFT 8
#define BLE_AD_FLAGS_FIELD_LEN 2
#define BLE_AD_UUID16_FIELD_LEN 3
#define BLE_AD_SERVICE_DATA_FIELD_LEN 4
#define BLE_AD_ELEMENT_HEADER_LEN 2
#define BLE_AD_FIXED_PAYLOAD_LEN 5

static const uint8_t BLE_HELLO_NAME[] = "ble_hello_server";
static bool g_ble_hello_default_state = true;

void ble_hello_server_set_adv_default_state(bool is_default)
{
    g_ble_hello_default_state = is_default;
}

static uint16_t ble_hello_build_adv_data(uint8_t *data, uint16_t capacity)
{
    uint16_t index = 0;
    uint16_t name_len = (uint16_t)(sizeof(BLE_HELLO_NAME) - 1);

    if (capacity < (uint16_t)(BLE_AD_UUID16_FIELD_LEN + BLE_AD_SERVICE_DATA_FIELD_LEN + name_len +
                              BLE_AD_ELEMENT_HEADER_LEN + BLE_AD_FIXED_PAYLOAD_LEN)) {
        return 0;
    }

    data[index++] = BLE_AD_FLAGS_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_FLAGS;
    data[index++] = BLE_AD_FLAGS_GENERAL;

    data[index++] = BLE_AD_UUID16_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_COMPLETE_UUID16;
    data[index++] = (uint8_t)(BLE_HELLO_SERVICE_UUID & 0xFF);
    data[index++] = (uint8_t)(BLE_HELLO_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT);

    data[index++] = (uint8_t)(name_len + 1);
    data[index++] = BLE_AD_TYPE_COMPLETE_NAME;
    if (memcpy_s(&data[index], capacity - index, BLE_HELLO_NAME, name_len) != EOK) {
        return 0;
    }
    index = (uint16_t)(index + name_len);

    data[index++] = BLE_AD_SERVICE_DATA_FIELD_LEN;
    data[index++] = BLE_AD_TYPE_SERVICE_DATA16;
    data[index++] = (uint8_t)(BLE_HELLO_SERVICE_UUID & 0xFF);
    data[index++] = (uint8_t)(BLE_HELLO_SERVICE_UUID >> BLE_UUID_HIGH_BYTE_SHIFT);
    data[index++] = g_ble_hello_default_state ? BLE_HELLO_STATE_DEFAULT : BLE_HELLO_STATE_RETAINED;
    return index;
}

errcode_t ble_hello_server_start_adv(void)
{
    uint8_t adv_data[31] = {0};
    uint16_t adv_len = ble_hello_build_adv_data(adv_data, sizeof(adv_data));
    gap_ble_config_adv_data_t config = {0};
    gap_ble_adv_params_t param = {0};
    errcode_t ret;

    if (adv_len == 0) {
        return ERRCODE_BT_FAIL;
    }

    config.adv_data = adv_data;
    config.adv_length = adv_len;
    ret = gap_ble_set_adv_data(BLE_HELLO_ADV_ID, &config);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }

    param.min_interval = BLE_HELLO_ADV_INTERVAL;
    param.max_interval = BLE_HELLO_ADV_INTERVAL;
    param.duration = BLE_HELLO_ADV_DURATION_FOREVER;
    param.adv_type = BLE_HELLO_ADV_TYPE_CONN_UNDIR;
    param.channel_map = BLE_HELLO_ADV_CHANNEL_MAP;
    param.adv_filter_policy = BLE_HELLO_ADV_FILTER_ALLOW_ALL;
    param.peer_addr.type = 0;
    (void)memset_s(param.peer_addr.addr, sizeof(param.peer_addr.addr), 0, sizeof(param.peer_addr.addr));

    ret = gap_ble_set_adv_param(BLE_HELLO_ADV_ID, &param);
    if (ret != ERRCODE_BT_SUCCESS) {
        return ret;
    }
    return gap_ble_start_adv(BLE_HELLO_ADV_ID);
}
