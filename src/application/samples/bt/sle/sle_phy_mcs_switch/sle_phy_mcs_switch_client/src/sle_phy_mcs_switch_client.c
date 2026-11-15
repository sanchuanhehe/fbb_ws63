/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE PHY/MCS dynamic switch Client implementation. \n
 * @else
 * @brief SLE PHY/MCS 动态切换 Client 实现。 \n
 * @endif
 */
#include <stdbool.h>
#include <string.h>
#include "common_def.h"
#include "securec.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_phy_mcs_switch_client.h"

#define SLE_PHY_MCS_CLIENT_LOG          "[sle phy mcs client]"
#define SLE_PHY_MCS_SERVER_NAME         "sle_phy_mcs_server"
#define SLE_PHY_MCS_SEEK_INTERVAL       100
#define SLE_PHY_MCS_SEEK_WINDOW         100
#define SLE_PHY_MCS_CORE_READY_DELAY_MS 5000
#define SLE_PHY_MCS_NAME_TYPE           0x0B

static sle_addr_t g_remote_addr = {0};
static bool g_target_found = false;

/**
 * @if Eng
 * @brief Parse announcement fields and match the PHY/MCS Server name.
 * @else
 * @brief 解析广播字段并匹配 PHY/MCS Server 名称。
 * @endif
 */
static bool sle_phy_mcs_has_server_name(const uint8_t *data, uint8_t data_len)
{
    const uint8_t server_name[] = SLE_PHY_MCS_SERVER_NAME;
    uint16_t index = 0;

    while (index < data_len) {
        uint8_t field_len = data[index];
        uint16_t next = index + field_len + 1U;
        if ((field_len < 1U) || (next > data_len)) {
            return false;
        }
        if ((data[index + 1U] == SLE_PHY_MCS_NAME_TYPE) &&
            ((field_len - 1U) == (sizeof(server_name) - 1U)) &&
            (memcmp(&data[index + 2U], server_name, sizeof(server_name) - 1U) == 0)) {
            return true;
        }
        index = next;
    }
    return false;
}

/**
 * @if Eng
 * @brief Configure active seeking and start Server discovery.
 * @else
 * @brief 配置主动扫描并启动 Server 发现。
 * @endif
 */
static errcode_t sle_phy_mcs_start_seek(void)
{
    sle_seek_param_t param = {0};
    errcode_t ret;

    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_PHY_MCS_SEEK_INTERVAL;
    param.seek_window[0] = SLE_PHY_MCS_SEEK_WINDOW;
    ret = sle_set_seek_param(&param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = sle_start_seek();
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start seek\r\n", SLE_PHY_MCS_CLIENT_LOG);
    }
    return ret;
}

/**
 * @if Eng
 * @brief Start seeking after the SLE stack is enabled.
 * @else
 * @brief SLE 协议栈使能后启动扫描。
 * @endif
 */
static void sle_phy_mcs_enable_cb(errcode_t status)
{
    osal_printk("%s SLE enabled, status=0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, status);
    if (status == ERRCODE_SLE_SUCCESS) {
        (void)sle_phy_mcs_start_seek();
    }
}

/**
 * @if Eng
 * @brief Report the asynchronous seek-enable result.
 * @else
 * @brief 输出异步扫描使能结果。
 * @endif
 */
static void sle_phy_mcs_seek_enable_cb(errcode_t status)
{
    osal_printk("%s seek enabled, status=0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, status);
}

/**
 * @if Eng
 * @brief Select the named Server and stop seeking.
 * @else
 * @brief 选择指定名称的 Server 并停止扫描。
 * @endif
 */
static void sle_phy_mcs_seek_result_cb(sle_seek_result_info_t *result)
{
    if ((result == NULL) || (result->data == NULL) || g_target_found) {
        return;
    }
    if (!sle_phy_mcs_has_server_name(result->data, result->data_length)) {
        return;
    }
    if (memcpy_s(&g_remote_addr, sizeof(g_remote_addr), &result->addr, sizeof(result->addr)) != EOK) {
        return;
    }
    g_target_found = true;
    osal_printk("%s found %s, stop seek\r\n", SLE_PHY_MCS_CLIENT_LOG, SLE_PHY_MCS_SERVER_NAME);
    (void)sle_stop_seek();
}

/**
 * @if Eng
 * @brief Connect to the selected Server after seeking stops.
 * @else
 * @brief 扫描停止后连接已选中的 Server。
 * @endif
 */
static void sle_phy_mcs_seek_disable_cb(errcode_t status)
{
    errcode_t ret;
    osal_printk("%s seek disabled, status=0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, status);
    if ((status != ERRCODE_SLE_SUCCESS) || !g_target_found) {
        return;
    }
    (void)sle_remove_paired_remote_device(&g_remote_addr);
    ret = sle_connect_remote_device(&g_remote_addr);
    osal_printk("%s connect request sent, status=0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, ret);
}

/**
 * @if Eng
 * @brief Report connection changes and restart seeking after disconnection.
 * @else
 * @brief 输出连接状态，并在断链后重新扫描。
 * @endif
 */
static void sle_phy_mcs_state_changed_cb(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_PHY_MCS_CLIENT_LOG, conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s disconnected, reason=0x%x, restart seek\r\n",
            SLE_PHY_MCS_CLIENT_LOG, disc_reason);
        g_target_found = false;
        (void)sle_remove_paired_remote_device(&g_remote_addr);
        (void)sle_phy_mcs_start_seek();
    }
}

/**
 * @if Eng
 * @brief Report the PHY and MCS values applied by the controller.
 * @else
 * @brief 输出控制器实际应用的 PHY 和 MCS 参数。
 * @endif
 */
static void sle_phy_mcs_set_phy_cb(uint16_t conn_id, errcode_t status, const sle_set_phy_t *param)
{
    if (param == NULL) {
        return;
    }
    osal_printk("%s PHY changed: conn_id=0x%02x, status=0x%x, tx_phy=%uM, rx_phy=%uM\r\n",
        SLE_PHY_MCS_CLIENT_LOG, conn_id, status,
        (uint8_t)(1U << param->tx_phy), (uint8_t)(1U << param->rx_phy));
}

/**
 * @if Eng
 * @brief Register SLE enable and discovery callbacks.
 * @else
 * @brief 注册 SLE 使能和设备发现回调。
 * @endif
 */
static errcode_t sle_phy_mcs_register_seek_callbacks(void)
{
    sle_announce_seek_callbacks_t callbacks = {0};
    callbacks.sle_enable_cb = sle_phy_mcs_enable_cb;
    callbacks.seek_enable_cb = sle_phy_mcs_seek_enable_cb;
    callbacks.seek_result_cb = sle_phy_mcs_seek_result_cb;
    callbacks.seek_disable_cb = sle_phy_mcs_seek_disable_cb;
    return sle_announce_seek_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Register connection-state and PHY-update callbacks.
 * @else
 * @brief 注册连接状态和 PHY 更新回调。
 * @endif
 */
static errcode_t sle_phy_mcs_register_connection_callbacks(void)
{
    sle_connection_callbacks_t callbacks = {0};
    callbacks.connect_state_changed_cb = sle_phy_mcs_state_changed_cb;
    callbacks.set_phy_cb = sle_phy_mcs_set_phy_cb;
    return sle_connection_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Initialize Client callbacks and enable SLE.
 * @else
 * @brief 初始化 Client 回调并使能 SLE。
 * @endif
 */
errcode_t sle_phy_mcs_switch_client_init(void)
{
    errcode_t ret;

    (void)osal_msleep(SLE_PHY_MCS_CORE_READY_DELAY_MS);
    ret = sle_phy_mcs_register_seek_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register seek callbacks failed: 0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, ret);
        return ret;
    }
    ret = sle_phy_mcs_register_connection_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks failed: 0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, ret);
        return ret;
    }
    ret = enable_sle();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s enable SLE failed: 0x%x\r\n", SLE_PHY_MCS_CLIENT_LOG, ret);
    }
    return ret;
}
