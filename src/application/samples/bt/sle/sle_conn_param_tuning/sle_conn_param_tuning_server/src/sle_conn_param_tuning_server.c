/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE connection parameter tuning Server implementation. \n
 * @else
 * @brief SLE 连接参数调优 Server 实现。 \n
 * @endif
 */
#include "common_def.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_conn_param_tuning_server_adv.h"
#include "sle_conn_param_tuning_server.h"

#define SLE_CONN_PARAM_SERVER_LOG "[sle conn param server]"
#define SLE_CONN_INTERVAL_MIN 0x001E
#define SLE_CONN_INTERVAL_MAX 0x3E80
#define SLE_CONN_LATENCY_MAX 0x01F3
#define SLE_CONN_TIMEOUT_MIN 0x000A
#define SLE_CONN_TIMEOUT_MAX 0x0C80
#define SLE_CONN_TIMEOUT_SCALE 20U

/**
 * @if Eng
 * @brief Named target connection-parameter profile.
 * @else
 * @brief 带名称的目标连接参数档位。
 * @endif
 */
typedef struct {
    const char *name;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
} sle_conn_param_profile_t;

#if defined(CONFIG_SLE_CONN_PARAM_PROFILE_LOW_POWER)
static const sle_conn_param_profile_t PROFILE = {"low-power", 400, 49, 1200};
#elif defined(CONFIG_SLE_CONN_PARAM_PROFILE_LOW_LATENCY)
static const sle_conn_param_profile_t PROFILE = {"low-latency", 30, 0, 200};
#else
static const sle_conn_param_profile_t PROFILE = {"balanced", 50, 0, 500};
#endif

/**
 * @if Eng
 * @brief Validate the relationship and protocol range of requested connection parameters.
 * @else
 * @brief 校验请求连接参数之间的关系及协议取值范围。
 * @endif
 */
static errcode_t sle_conn_param_validate(const sle_connection_param_update_t *param)
{
    uint32_t timeout_scaled;
    uint32_t max_connection_gap;

    if ((param->interval_min < SLE_CONN_INTERVAL_MIN) || (param->interval_max > SLE_CONN_INTERVAL_MAX) ||
        (param->interval_min > param->interval_max) || (param->max_latency > SLE_CONN_LATENCY_MAX) ||
        (param->supervision_timeout < SLE_CONN_TIMEOUT_MIN) || (param->supervision_timeout > SLE_CONN_TIMEOUT_MAX)) {
        return ERRCODE_INVALID_PARAM;
    }

    /*
     * Interval uses 0.25 ms and timeout uses 10 ms:
     * timeout * 10 ms > 2 * (latency + 1) * interval * 0.25 ms.
     * interval 的单位为 0.25 ms，timeout 的单位为 10 ms，约束关系如下：
     * timeout * 10 ms > 2 * (latency + 1) * interval * 0.25 ms。
     */
    timeout_scaled = (uint32_t)param->supervision_timeout * SLE_CONN_TIMEOUT_SCALE;
    max_connection_gap = ((uint32_t)param->max_latency + 1U) * param->interval_max;
    return (timeout_scaled > max_connection_gap) ? ERRCODE_SUCC : ERRCODE_INVALID_PARAM;
}

/**
 * @if Eng
 * @brief Request the target connection parameters after a link is established.
 * @else
 * @brief 建链后请求切换到目标连接参数。
 * @endif
 */
static errcode_t sle_conn_param_request_update(uint16_t conn_id)
{
    sle_connection_param_update_t param = {
        .conn_id = conn_id,
        .interval_min = PROFILE.interval,
        .interval_max = PROFILE.interval,
        .max_latency = PROFILE.latency,
        .supervision_timeout = PROFILE.timeout,
    };
    errcode_t ret = sle_conn_param_validate(&param);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s invalid profile: %s\r\n", SLE_CONN_PARAM_SERVER_LOG, PROFILE.name);
        return ret;
    }
    osal_printk("%s update request: profile=%s, interval=%u (0.25ms), latency=%u, timeout=%u (10ms)\r\n",
                SLE_CONN_PARAM_SERVER_LOG, PROFILE.name, param.interval_min, param.max_latency,
                param.supervision_timeout);
    ret = sle_update_connect_param(&param);
    osal_printk("%s update request sent, status=0x%x\r\n", SLE_CONN_PARAM_SERVER_LOG, ret);
    return ret;
}

/**
 * @if Eng
 * @brief Trigger tuning on connection and resume announcement after disconnection.
 * @else
 * @brief 建链时触发参数调优，断链后恢复广播。
 * @endif
 */
static void sle_conn_param_state_changed_cb(uint16_t conn_id,
                                            const sle_addr_t *addr,
                                            sle_acb_state_t conn_state,
                                            sle_pair_state_t pair_state,
                                            sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_CONN_PARAM_SERVER_LOG, conn_id);
        (void)sle_conn_param_request_update(conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s disconnected, reason=0x%x, restart announce\r\n", SLE_CONN_PARAM_SERVER_LOG, disc_reason);
        (void)sle_start_announce(SLE_CONN_PARAM_ADV_HANDLE);
    }
}

/**
 * @if Eng
 * @brief Report the asynchronous parameter-update request result.
 * @else
 * @brief 输出异步连接参数更新请求结果。
 * @endif
 */
static void sle_conn_param_update_req_cb(uint16_t conn_id,
                                         errcode_t status,
                                         const sle_connection_param_update_req_t *param)
{
    if (param == NULL) {
        return;
    }
    osal_printk("%s peer update request: conn_id=0x%02x, status=0x%x, interval=%u-%u, latency=%u, timeout=%u\r\n",
                SLE_CONN_PARAM_SERVER_LOG, conn_id, status, param->interval_min, param->interval_max,
                param->max_latency, param->supervision_timeout);
}

/**
 * @if Eng
 * @brief Report the final connection parameters applied by the controller.
 * @else
 * @brief 输出控制器最终应用的连接参数。
 * @endif
 */
static void sle_conn_param_update_cb(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    if (param == NULL) {
        return;
    }
    osal_printk("%s update complete: conn_id=0x%02x, status=0x%x, interval=%u, latency=%u, timeout=%u\r\n",
                SLE_CONN_PARAM_SERVER_LOG, conn_id, status, param->interval, param->latency, param->supervision);
}

/**
 * @if Eng
 * @brief Register connection-state and parameter-update callbacks.
 * @else
 * @brief 注册连接状态和参数更新回调。
 * @endif
 */
static errcode_t sle_conn_param_register_connection_callbacks(void)
{
    sle_connection_callbacks_t callbacks = {0};
    callbacks.connect_state_changed_cb = sle_conn_param_state_changed_cb;
    callbacks.connect_param_update_req_cb = sle_conn_param_update_req_cb;
    callbacks.connect_param_update_cb = sle_conn_param_update_cb;
    return sle_connection_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Initialize SLE, callbacks and Server announcement.
 * @else
 * @brief 初始化 SLE、回调和 Server 广播。
 * @endif
 */
errcode_t sle_conn_param_tuning_server_init(void)
{
    errcode_t ret = enable_sle();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s enable SLE failed: 0x%x\r\n", SLE_CONN_PARAM_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_param_tuning_announce_register_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register announce callbacks failed: 0x%x\r\n", SLE_CONN_PARAM_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_param_register_connection_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks failed: 0x%x\r\n", SLE_CONN_PARAM_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s selected profile=%s\r\n", SLE_CONN_PARAM_SERVER_LOG, PROFILE.name);
    return sle_conn_param_tuning_server_announce_start();
}
