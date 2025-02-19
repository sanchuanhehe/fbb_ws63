/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "product.h"
#include "sle_common.h"
#include "sle_server.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "string.h"
#include "sle_server_adv.h"

/* sle device name */
#define NAME_MAX_LENGTH 9
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT 0x32
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT 0x32
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY 0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER 20
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX 251
/* 广播名称 */
static uint8_t g_sle_local_name[NAME_MAX_LENGTH] = "sle_test";

static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    errno_t ret;
    uint8_t index = 0;

    uint8_t *local_name = g_sle_local_name;
    uint8_t local_name_len = sizeof(g_sle_local_name) - 1;
    printf("[%s] local_name_len = %d\r\n", __FUNCTION__, local_name_len);
    printf("[%s] local_name: ", __FUNCTION__);
    for (uint8_t i = 0; i < local_name_len; i++) {
        printf("0x%02x ", local_name[i]);
    }
    printf("\r\n");
    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    ret = memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len);
    if (ret != EOK) {
        printf("[%s] memcpy fail\r\n", __FUNCTION__);
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

static uint16_t sle_set_adv_data(uint8_t *adv_data)
{
    size_t len = 0;
    uint16_t idx = 0;
    errno_t ret = 0;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_disc_level, len);
    if (ret != EOK) {
        printf("[%s] adv_disc_level memcpy fail\r\n", __FUNCTION__);
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_access_mode, len);
    if (ret != EOK) {
        printf("[%s] adv_access_mode memcpy fail\r\n", __FUNCTION__);
        return 0;
    }
    idx += len;

    return idx;
}

static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data)
{
    uint16_t idx = 0;
    errno_t ret;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    ret = memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN_MAX, &tx_power_level, scan_rsp_data_len);
    if (ret != EOK) {
        printf("[%s] scan response data memcpy fail\r\n", __FUNCTION__);
        return 0;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

static int sle_set_default_announce_param(void)
{
    errno_t ret;
    sle_announce_param_t param = {0};
    uint8_t index;
    unsigned char local_addr[SLE_ADDR_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.announce_tx_power = SLE_ADV_TX_POWER;
    param.own_addr.type = 0;
    ret = memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    if (ret != EOK) {
        printf("[%s] data memcpy fail\r\n", __FUNCTION__);
        return 0;
    }
    printf("[%s] sle_local addr: ", __FUNCTION__);
    for (index = 0; index < SLE_ADDR_LEN; index++) {
        printf("0x%02x ", param.own_addr.addr[index]);
    }
    printf("\r\n");
    return sle_set_announce_param(param.announce_handle, &param);
}

static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t data_index = 0;

    announce_data_len = sle_set_adv_data(announce_data);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    printf("[%s] data.announce_data_len = %d\r\n", __FUNCTION__, data.announce_data_len);
    printf("[%s] data.announce_data: ", __FUNCTION__);
    for (data_index = 0; data_index < data.announce_data_len; data_index++) {
        printf("0x%02x ", data.announce_data[data_index]);
    }
    printf("\r\n");

    seek_data_len = sle_set_scan_response_data(seek_rsp_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    printf("[%s] data.seek_rsp_data_len = %d\r\n", __FUNCTION__, data.seek_rsp_data_len);
    printf("[%s] data.seek_rsp_data: ", __FUNCTION__);
    for (data_index = 0; data_index < data.seek_rsp_data_len; data_index++) {
        printf("0x%02x ", data.seek_rsp_data[data_index]);
    }
    printf("\r\n");

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        printf("[%s] set announce data success.\r\n", __FUNCTION__);
    } else {
        printf("[%s] set adv param fail.\r\n", __FUNCTION__);
    }
    return ERRCODE_SLE_SUCCESS;
}

static void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    printf("[%s]  id:%02x, state:%x\r\n", __FUNCTION__, announce_id, status);
}

static void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    printf("[%s] id:%02x, state:%x\r\n", __FUNCTION__, announce_id, status);
}

static void sle_announce_terminal_cbk(uint32_t announce_id)
{
    printf("[%s] id:%02x\r\n", __FUNCTION__, announce_id);
}

errcode_t sle_announce_register_cbks(void)
{
    errcode_t ret = 0;
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    ret = sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("[%s] fail :%x\r\n", __FUNCTION__, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("[%s] fail :%x\r\n", __FUNCTION__, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}
