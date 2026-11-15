/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging Server announcement implementation. \n
 * @else
 * @brief SLE RSSI 测距 Server 广播实现。 \n
 * @endif
 */
#include "securec.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_rssi_ranging_server_adv.h"

#define SLE_ADV_ELEMENT_HEADER_LEN 2U
#define SLE_ADV_TX_POWER_DBM 18
#define SLE_RSSI_SERVER_LOG "[sle rssi server]"
#define SLE_RSSI_ADV_DATA_MAX_LEN 251
#define SLE_RSSI_ADV_INTERVAL 0xC8
#define SLE_RSSI_CONN_INTERVAL 50
#define SLE_RSSI_CONN_TIMEOUT 500
#define SLE_RSSI_ADV_TX_POWER 10
#define SLE_RSSI_LOCAL_NAME "sle_rssi_server"

enum {
    SLE_RSSI_ADV_CHANNEL_MAP_DEFAULT = 0x07,
    SLE_RSSI_DATA_DISCOVERY_LEVEL = 0x01,
    SLE_RSSI_DATA_ACCESS_MODE = 0x02,
    SLE_RSSI_DATA_COMPLETE_LOCAL_NAME = 0x0B,
    SLE_RSSI_DATA_TX_POWER_LEVEL = 0x0C,
};

/**
 * @if Eng
 * @brief Append the complete Server local-name field.
 * @param [out] data Output buffer.
 * @param [in] max_len Available buffer length.
 * @return Encoded field length, or zero on failure.
 * @else
 * @brief 追加 Server 完整本地名称字段。
 * @param [out] data 输出缓冲区。
 * @param [in] max_len 可用缓冲区长度。
 * @return 编码后的字段长度，失败时返回 0。
 * @endif
 */
static uint16_t sle_rssi_set_local_name(uint8_t *data, uint16_t max_len)
{
    const uint8_t local_name[] = SLE_RSSI_LOCAL_NAME;
    uint8_t local_name_len = sizeof(local_name) - 1U;

    if (max_len < (uint16_t)(local_name_len + 2U)) {
        return 0;
    }
    data[0] = local_name_len + 1U;
    data[1] = SLE_RSSI_DATA_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&data[SLE_ADV_ELEMENT_HEADER_LEN], max_len - SLE_ADV_ELEMENT_HEADER_LEN, local_name, local_name_len) !=
        EOK) {
        return 0;
    }
    return (uint16_t)(local_name_len + SLE_ADV_ELEMENT_HEADER_LEN);
}

/**
 * @if Eng
 * @brief Build the primary announcement payload.
 * @param [out] data Output buffer.
 * @return Payload length, or zero on failure.
 * @else
 * @brief 构造主广播数据。
 * @param [out] data 输出缓冲区。
 * @return 广播数据长度，失败时返回 0。
 * @endif
 */
static uint16_t sle_rssi_set_announce_data(uint8_t *data)
{
    sle_rssi_ranging_adv_common_value_t discovery = {
        .length = sizeof(discovery) - 1U,
        .type = SLE_RSSI_DATA_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    sle_rssi_ranging_adv_common_value_t access_mode = {
        .length = sizeof(access_mode) - 1U,
        .type = SLE_RSSI_DATA_ACCESS_MODE,
        .value = 0,
    };
    uint16_t index = 0;

    if (memcpy_s(&data[index], SLE_RSSI_ADV_DATA_MAX_LEN - index, &discovery, sizeof(discovery)) != EOK) {
        return 0;
    }
    index += sizeof(discovery);
    if (memcpy_s(&data[index], SLE_RSSI_ADV_DATA_MAX_LEN - index, &access_mode, sizeof(access_mode)) != EOK) {
        return 0;
    }
    return (uint16_t)(index + sizeof(access_mode));
}

/**
 * @if Eng
 * @brief Build the seek-response payload containing TX power and local name.
 * @param [out] data Output buffer.
 * @return Payload length, or zero on failure.
 * @else
 * @brief 构造包含发射功率和本地名称的扫描响应数据。
 * @param [out] data 输出缓冲区。
 * @return 扫描响应数据长度，失败时返回 0。
 * @endif
 */
static uint16_t sle_rssi_set_seek_response(uint8_t *data)
{
    sle_rssi_ranging_adv_common_value_t tx_power = {
        .length = sizeof(tx_power) - 1U,
        .type = SLE_RSSI_DATA_TX_POWER_LEVEL,
        .value = SLE_RSSI_ADV_TX_POWER,
    };
    uint16_t index = 0;
    uint16_t name_len;

    if (memcpy_s(data, SLE_RSSI_ADV_DATA_MAX_LEN, &tx_power, sizeof(tx_power)) != EOK) {
        return 0;
    }
    index += sizeof(tx_power);
    name_len = sle_rssi_set_local_name(&data[index], SLE_RSSI_ADV_DATA_MAX_LEN - index);
    return (name_len == 0) ? 0 : (uint16_t)(index + name_len);
}

/**
 * @if Eng
 * @brief Configure connectable and scannable announcement parameters.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 配置可连接、可扫描的广播参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_set_announce_param(void)
{
    sle_announce_param_t param = {0};
    const uint8_t local_addr[SLE_ADDR_LEN] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_RSSI_RANGING_ADV_HANDLE;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_RSSI_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_RSSI_ADV_INTERVAL;
    param.announce_interval_max = SLE_RSSI_ADV_INTERVAL;
    param.conn_interval_min = SLE_RSSI_CONN_INTERVAL;
    param.conn_interval_max = SLE_RSSI_CONN_INTERVAL;
    param.conn_max_latency = 0;
    param.conn_supervision_timeout = SLE_RSSI_CONN_TIMEOUT;
    param.announce_tx_power = SLE_ADV_TX_POWER_DBM;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

/**
 * @if Eng
 * @brief Submit announcement and seek-response payloads to the SLE stack.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 将广播和扫描响应数据提交给 SLE 协议栈。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_set_announce_payload(void)
{
    sle_announce_data_t payload = {0};
    uint8_t announce_data[SLE_RSSI_ADV_DATA_MAX_LEN] = {0};
    uint8_t seek_rsp_data[SLE_RSSI_ADV_DATA_MAX_LEN] = {0};

    payload.announce_data_len = sle_rssi_set_announce_data(announce_data);
    payload.seek_rsp_data_len = sle_rssi_set_seek_response(seek_rsp_data);
    if ((payload.announce_data_len == 0) || (payload.seek_rsp_data_len == 0)) {
        return ERRCODE_SLE_FAIL;
    }
    payload.announce_data = announce_data;
    payload.seek_rsp_data = seek_rsp_data;
    return sle_set_announce_data(SLE_RSSI_RANGING_ADV_HANDLE, &payload);
}

/**
 * @if Eng
 * @brief Report the asynchronous announcement-enable result.
 * @param [in] announce_id Announcement handle.
 * @param [in] status Enable result.
 * @else
 * @brief 输出异步广播使能结果。
 * @param [in] announce_id 广播句柄。
 * @param [in] status 使能结果。
 * @endif
 */
static void sle_rssi_announce_enable_cb(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce enabled, id=%u, status=0x%x\r\n", SLE_RSSI_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Report the asynchronous announcement-disable result.
 * @param [in] announce_id Announcement handle.
 * @param [in] status Disable result.
 * @else
 * @brief 输出异步广播停止结果。
 * @param [in] announce_id 广播句柄。
 * @param [in] status 停止结果。
 * @endif
 */
static void sle_rssi_announce_disable_cb(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce disabled, id=%u, status=0x%x\r\n", SLE_RSSI_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Register announcement state callbacks.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 注册广播状态回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_announce_register_callbacks(void)
{
    sle_announce_seek_callbacks_t callbacks = {0};
    callbacks.announce_enable_cb = sle_rssi_announce_enable_cb;
    callbacks.announce_disable_cb = sle_rssi_announce_disable_cb;
    return sle_announce_seek_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Configure the payload and start Server announcement.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 配置广播数据并启动 Server 广播。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_server_announce_start(void)
{
    errcode_t ret = sle_rssi_set_announce_param();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce param failed: 0x%x\r\n", SLE_RSSI_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_rssi_set_announce_payload();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce data failed: 0x%x\r\n", SLE_RSSI_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_start_announce(SLE_RSSI_RANGING_ADV_HANDLE);
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start announce, name=%s\r\n", SLE_RSSI_SERVER_LOG, SLE_RSSI_LOCAL_NAME);
    }
    return ret;
}
