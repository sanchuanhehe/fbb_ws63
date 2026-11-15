/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging Server implementation. \n
 * @else
 * @brief SLE RSSI 测距 Server 实现。 \n
 * @endif
 */
#include "common_def.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_rssi_ranging_server_adv.h"
#include "sle_rssi_ranging_server.h"

#define SLE_RSSI_SERVER_LOG "[sle rssi server]"

/**
 * @if Eng
 * @brief Report connection changes and resume announcing after disconnection.
 * @param [in] conn_id SLE connection ID.
 * @param [in] addr Peer address.
 * @param [in] conn_state Current connection state.
 * @param [in] pair_state Current pairing state.
 * @param [in] disc_reason Disconnection reason.
 * @else
 * @brief 输出连接状态，并在断链后恢复广播。
 * @param [in] conn_id SLE 连接 ID。
 * @param [in] addr 对端地址。
 * @param [in] conn_state 当前连接状态。
 * @param [in] pair_state 当前配对状态。
 * @param [in] disc_reason 断链原因。
 * @endif
 */
static void sle_rssi_server_state_changed_cb(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_RSSI_SERVER_LOG, conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s disconnected, reason=0x%x, restart announce\r\n", SLE_RSSI_SERVER_LOG, disc_reason);
        (void)sle_start_announce(SLE_RSSI_RANGING_ADV_HANDLE);
    }
}

/**
 * @if Eng
 * @brief Register the Server connection-state callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 注册 Server 连接状态回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_server_register_connection_callbacks(void)
{
    sle_connection_callbacks_t callbacks = {0};
    callbacks.connect_state_changed_cb = sle_rssi_server_state_changed_cb;
    return sle_connection_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Enable SLE, register callbacks and start Server announcement.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 使能 SLE、注册回调并启动 Server 广播。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_server_init(void)
{
    errcode_t ret = enable_sle();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s enable SLE failed: 0x%x\r\n", SLE_RSSI_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_rssi_ranging_announce_register_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register announce callbacks failed: 0x%x\r\n", SLE_RSSI_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_rssi_server_register_connection_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks failed: 0x%x\r\n", SLE_RSSI_SERVER_LOG, ret);
        return ret;
    }
    return sle_rssi_ranging_server_announce_start();
}
