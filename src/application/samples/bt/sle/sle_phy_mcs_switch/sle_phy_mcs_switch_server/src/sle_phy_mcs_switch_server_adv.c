/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE PHY/MCS dynamic switch Server announcement implementation. \n
 * @else
 * @brief SLE PHY/MCS 动态切换 Server 广播实现。 \n
 * @endif
 */
#include "securec.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_phy_mcs_switch_server_adv.h"

#define SLE_ADV_ELEMENT_HEADER_LEN 2U
#define SLE_ANNOUNCE_TX_POWER_DBM 18
#define SLE_PHY_MCS_SERVER_LOG "[sle phy mcs server]"
#define SLE_PHY_MCS_ADV_DATA_MAX_LEN 251
#define SLE_PHY_MCS_ADV_INTERVAL 0xC8
#define SLE_PHY_MCS_CONN_INTERVAL 50
#define SLE_PHY_MCS_CONN_TIMEOUT 500
#define SLE_PHY_MCS_ADV_TX_POWER 10
#define SLE_PHY_MCS_LOCAL_NAME "sle_phy_mcs_server"

enum {
    SLE_PHY_MCS_ADV_CHANNEL_MAP_DEFAULT = 0x07,
    SLE_PHY_MCS_DATA_DISCOVERY_LEVEL = 0x01,
    SLE_PHY_MCS_DATA_ACCESS_MODE = 0x02,
    SLE_PHY_MCS_DATA_COMPLETE_LOCAL_NAME = 0x0B,
    SLE_PHY_MCS_DATA_TX_POWER_LEVEL = 0x0C,
};

/**
 * @if Eng
 * @brief Append the complete Server local-name field.
 * @else
 * @brief 追加 Server 完整本地名称字段。
 * @endif
 */
static uint16_t sle_phy_mcs_set_local_name(uint8_t *data, uint16_t max_len)
{
    const uint8_t local_name[] = SLE_PHY_MCS_LOCAL_NAME;
    uint8_t local_name_len = sizeof(local_name) - 1;

    if (max_len < (uint16_t)(local_name_len + 2U)) {
        return 0;
    }
    data[0] = local_name_len + 1U;
    data[1] = SLE_PHY_MCS_DATA_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&data[SLE_ADV_ELEMENT_HEADER_LEN], max_len - SLE_ADV_ELEMENT_HEADER_LEN, local_name, local_name_len) !=
        EOK) {
        return 0;
    }
    return (uint16_t)(local_name_len + SLE_ADV_ELEMENT_HEADER_LEN);
}

/**
 * @if Eng
 * @brief Build the primary announcement payload.
 * @else
 * @brief 构造主广播数据。
 * @endif
 */
static uint16_t sle_phy_mcs_set_announce_data(uint8_t *data)
{
    sle_phy_mcs_adv_common_value_t discovery = {
        .length = sizeof(discovery) - 1U,
        .type = SLE_PHY_MCS_DATA_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    sle_phy_mcs_adv_common_value_t access_mode = {
        .length = sizeof(access_mode) - 1U,
        .type = SLE_PHY_MCS_DATA_ACCESS_MODE,
        .value = 0,
    };
    uint16_t index = 0;

    if (memcpy_s(&data[index], SLE_PHY_MCS_ADV_DATA_MAX_LEN - index, &discovery, sizeof(discovery)) != EOK) {
        return 0;
    }
    index += sizeof(discovery);
    if (memcpy_s(&data[index], SLE_PHY_MCS_ADV_DATA_MAX_LEN - index, &access_mode, sizeof(access_mode)) != EOK) {
        return 0;
    }
    return (uint16_t)(index + sizeof(access_mode));
}

/**
 * @if Eng
 * @brief Build the seek-response payload.
 * @else
 * @brief 构造扫描响应数据。
 * @endif
 */
static uint16_t sle_phy_mcs_set_seek_response(uint8_t *data)
{
    sle_phy_mcs_adv_common_value_t tx_power = {
        .length = sizeof(tx_power) - 1U,
        .type = SLE_PHY_MCS_DATA_TX_POWER_LEVEL,
        .value = SLE_PHY_MCS_ADV_TX_POWER,
    };
    uint16_t index = 0;
    uint16_t name_len;

    if (memcpy_s(data, SLE_PHY_MCS_ADV_DATA_MAX_LEN, &tx_power, sizeof(tx_power)) != EOK) {
        return 0;
    }
    index += sizeof(tx_power);
    name_len = sle_phy_mcs_set_local_name(&data[index], SLE_PHY_MCS_ADV_DATA_MAX_LEN - index);
    return (name_len == 0) ? 0 : (uint16_t)(index + name_len);
}

/**
 * @if Eng
 * @brief Configure connectable announcement parameters.
 * @else
 * @brief 配置可连接广播参数。
 * @endif
 */
static errcode_t sle_phy_mcs_set_announce_param(void)
{
    sle_announce_param_t param = {0};
    const uint8_t local_addr[SLE_ADDR_LEN] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36};

    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_PHY_MCS_ADV_HANDLE;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_PHY_MCS_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_PHY_MCS_ADV_INTERVAL;
    param.announce_interval_max = SLE_PHY_MCS_ADV_INTERVAL;
    param.conn_interval_min = SLE_PHY_MCS_CONN_INTERVAL;
    param.conn_interval_max = SLE_PHY_MCS_CONN_INTERVAL;
    param.conn_max_latency = 0;
    param.conn_supervision_timeout = SLE_PHY_MCS_CONN_TIMEOUT;
    param.announce_tx_power = SLE_ANNOUNCE_TX_POWER_DBM;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

/**
 * @if Eng
 * @brief Submit announcement payloads to the SLE stack.
 * @else
 * @brief 向 SLE 协议栈提交广播数据。
 * @endif
 */
static errcode_t sle_phy_mcs_set_announce_payload(void)
{
    sle_announce_data_t payload = {0};
    uint8_t announce_data[SLE_PHY_MCS_ADV_DATA_MAX_LEN] = {0};
    uint8_t seek_rsp_data[SLE_PHY_MCS_ADV_DATA_MAX_LEN] = {0};

    payload.announce_data_len = sle_phy_mcs_set_announce_data(announce_data);
    payload.seek_rsp_data_len = sle_phy_mcs_set_seek_response(seek_rsp_data);
    if ((payload.announce_data_len == 0) || (payload.seek_rsp_data_len == 0)) {
        return ERRCODE_SLE_FAIL;
    }
    payload.announce_data = announce_data;
    payload.seek_rsp_data = seek_rsp_data;
    return sle_set_announce_data(SLE_PHY_MCS_ADV_HANDLE, &payload);
}

/**
 * @if Eng
 * @brief Report the announcement-enable result.
 * @else
 * @brief 输出广播使能结果。
 * @endif
 */
static void sle_phy_mcs_announce_enable_cb(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce enabled, id=%u, status=0x%x\r\n", SLE_PHY_MCS_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Report the announcement-disable result.
 * @else
 * @brief 输出广播停止结果。
 * @endif
 */
static void sle_phy_mcs_announce_disable_cb(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce disabled, id=%u, status=0x%x\r\n", SLE_PHY_MCS_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Register announcement callbacks.
 * @else
 * @brief 注册广播回调。
 * @endif
 */
errcode_t sle_phy_mcs_switch_announce_register_callbacks(void)
{
    sle_announce_seek_callbacks_t callbacks = {0};
    callbacks.announce_enable_cb = sle_phy_mcs_announce_enable_cb;
    callbacks.announce_disable_cb = sle_phy_mcs_announce_disable_cb;
    return sle_announce_seek_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Configure and start Server announcement.
 * @else
 * @brief 配置并启动 Server 广播。
 * @endif
 */
errcode_t sle_phy_mcs_switch_server_announce_start(void)
{
    errcode_t ret = sle_phy_mcs_set_announce_param();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce param failed: 0x%x\r\n", SLE_PHY_MCS_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_phy_mcs_set_announce_payload();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce data failed: 0x%x\r\n", SLE_PHY_MCS_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_start_announce(SLE_PHY_MCS_ADV_HANDLE);
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start announce, name=%s\r\n", SLE_PHY_MCS_SERVER_LOG, SLE_PHY_MCS_LOCAL_NAME);
    }
    return ret;
}
