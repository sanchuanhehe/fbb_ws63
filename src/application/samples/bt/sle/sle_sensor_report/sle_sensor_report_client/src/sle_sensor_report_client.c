/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * @if Eng
 * @brief Implements scanning, connection, and data reception for the SLE sensor report client.
 * @else
 * @brief 实现 SLE 传感器上报客户端的扫描、连接和数据接收流程。
 * @endif
 *
 * History: \n
 * 2024-06-01, Create file. \n
 */

#include "common_def.h"
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_errcode.h"
#include "sle_sensor_report_client.h"
#include "string.h"

#define SENSOR_CLIENT_LOG "[sensor client]"

#define SLE_MTU_SIZE_DEFAULT 520
#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_WAIT_SLE_CORE_READY_MS 5000
#define SLE_SENSOR_SERVER_NAME "sensor_server"

/* Global client state. / 客户端全局状态。 */
static uint16_t g_conn_id = 0;
static bool g_connected = false;
static sle_addr_t g_remote_addr = {0};
static uint16_t g_alarm_property_handle = 0;

/* Application callbacks. / 应用层回调。 */
static ssapc_notification_callback g_saved_notification_cb = NULL;
static ssapc_indication_callback g_saved_indication_cb = NULL;

/* Scanning callbacks. / 扫描回调。 */

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_seek_enable_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_seek_enable_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_seek_enable_cbk(errcode_t status)
{
    osal_printk("%s seek enable cbk, status: 0x%x\r\n", SENSOR_CLIENT_LOG, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_seek_disable_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_seek_disable_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_seek_disable_cbk(errcode_t status)
{
    unused(status);
    /* Connect after scanning stops. / 扫描停止后发起连接。 */
    (void)sle_remove_paired_remote_device(&g_remote_addr);
    errcode_t ret = sle_connect_remote_device(&g_remote_addr);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_connect_remote_device fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_seek_result_info_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_seek_result_info_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL || seek_result_data->data == NULL) {
        return;
    }

    /* Match the "sensor_server" advertising name. / 匹配 "sensor_server" 广播名称。 */
    if (strstr((const char *)seek_result_data->data, SLE_SENSOR_SERVER_NAME) != NULL) {
        osal_printk("%s found sensor_server, stop seek.\r\n", SENSOR_CLIENT_LOG);
        (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        (void)sle_stop_seek();
    }
}

static errcode_t sle_sensor_report_seek_cbk_register(void);
static errcode_t sle_sensor_report_connect_cbk_register(void);
static errcode_t sle_sensor_report_ssapc_cbk_register(void);

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_sle_enable_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_sle_enable_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_sle_enable_cbk(errcode_t status)
{
    if (status != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle enable fail: 0x%x\r\n", SENSOR_CLIENT_LOG, status);
        return;
    }
    /* Re-register SSAPC callbacks because enable_sle() may clear them. / enable_sle() 可能清空 SSAPC
     * 回调表，因此需要重新注册。 */
    (void)sle_sensor_report_seek_cbk_register();
    (void)sle_sensor_report_connect_cbk_register();
    (void)sle_sensor_report_ssapc_cbk_register();
    (void)sle_sensor_report_client_start_scan();
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_sensor_report_seek_cbk_register.
 * @else
 * @brief 注册 \c sle_sensor_report_seek_cbk_register 所需的回调函数。
 * @endif
 */
static errcode_t sle_sensor_report_seek_cbk_register(void)
{
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.sle_enable_cb = sle_sensor_sle_enable_cbk;
    seek_cbks.seek_enable_cb = sle_sensor_seek_enable_cbk;
    seek_cbks.seek_result_cb = sle_sensor_seek_result_info_cbk;
    seek_cbks.seek_disable_cb = sle_sensor_seek_disable_cbk;

    errcode_t ret = sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register seek callbacks fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* Connection callbacks. / 连接回调。 */

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_connect_state_changed_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_connect_state_changed_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_connect_state_changed_cbk(uint16_t conn_id,
                                                 const sle_addr_t *addr,
                                                 sle_acb_state_t conn_state,
                                                 sle_pair_state_t pair_state,
                                                 sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    unused(disc_reason);

    switch (conn_state) {
        case SLE_ACB_STATE_CONNECTED:
            g_conn_id = conn_id;
            g_connected = true;
            osal_printk("%s connected, conn_id: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id);

            /* Pair when the peer is not paired. / 对端未配对时发起配对。 */
            if (pair_state == SLE_PAIR_NONE) {
                (void)sle_pair_remote_device(&g_remote_addr);
            }
            break;

        case SLE_ACB_STATE_DISCONNECTED:
            osal_printk("%s disconnected, conn_id: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id);
            g_conn_id = 0;
            g_connected = false;
            /* Restart scanning. / 重新开始扫描。 */
            (void)sle_sensor_report_client_start_scan();
            break;

        default:
            break;
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_pair_complete_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_pair_complete_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(addr);

    if (status != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s pair failed, conn_id: 0x%x, status: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id, status);
        return;
    }

    osal_printk("%s pair complete, conn_id: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id);

    /* Start MTU exchange. / 发起 MTU 交换。 */
    ssap_exchange_info_t info = {.mtu_size = SLE_MTU_SIZE_DEFAULT, .version = 1};
    errcode_t ret = ssapc_exchange_info_req(0, g_conn_id, &info);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssapc_exchange_info_req fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_sensor_report_connect_cbk_register.
 * @else
 * @brief 注册 \c sle_sensor_report_connect_cbk_register 所需的回调函数。
 * @endif
 */
static errcode_t sle_sensor_report_connect_cbk_register(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_sensor_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_sensor_pair_complete_cbk;

    errcode_t ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* SSAPC callbacks. / SSAPC 回调。 */

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_exchange_info_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_exchange_info_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_exchange_info_cbk(uint8_t client_id,
                                         uint16_t conn_id,
                                         ssap_exchange_info_t *param,
                                         errcode_t status)
{
    unused(client_id);
    unused(param);

    osal_printk("%s exchange info cbk, conn_id: 0x%x, status: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id, status);

    if (status != ERRCODE_SLE_SUCCESS) {
        return;
    }

    /* Start service discovery. / 发起服务发现。 */
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;

    errcode_t ret = ssapc_find_structure(0, g_conn_id, &find_param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssapc_find_structure fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_find_structure_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_find_structure_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_find_structure_cbk(uint8_t client_id,
                                          uint16_t conn_id,
                                          ssapc_find_service_result_t *service,
                                          errcode_t status)
{
    unused(client_id);
    osal_printk("%s find structure cbk, conn_id: 0x%x, start: 0x%x, end: 0x%x, status: 0x%x\r\n", SENSOR_CLIENT_LOG,
                conn_id, (service != NULL) ? service->start_hdl : 0, (service != NULL) ? service->end_hdl : 0, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_find_property_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_find_property_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_find_property_cbk(uint8_t client_id,
                                         uint16_t conn_id,
                                         ssapc_find_property_result_t *property,
                                         errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (property != NULL) {
        osal_printk("%s find property, handle: 0x%x, operate: 0x%x\r\n", SENSOR_CLIENT_LOG, property->handle,
                    property->operate_indication);
        if (property->operate_indication & SSAP_OPERATE_INDICATION_BIT_INDICATE) {
            g_alarm_property_handle = property->handle;
        }
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_find_structure_cmp_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_find_structure_cmp_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_find_structure_cmp_cbk(uint8_t client_id,
                                              uint16_t conn_id,
                                              ssapc_find_structure_result_t *structure_result,
                                              errcode_t status)
{
    unused(client_id);
    unused(structure_result);
    osal_printk("%s find structure complete, conn_id: 0x%x, status: 0x%x\r\n", SENSOR_CLIENT_LOG, conn_id, status);
    /* Write 0x0002 to the alarm CCCD to enable indications. / 向告警 CCCD 写入 0x0002 以使能指示。 */
    if (g_alarm_property_handle != 0) {
        uint8_t cccd_val[2] = {0x02, 0x00};
        ssapc_write_param_t wparam = {0};
        wparam.handle = g_alarm_property_handle + 1; /* CCCD follows the alarm property. / CCCD 位于告警属性之后。 */
        wparam.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
        wparam.data = cccd_val;
        wparam.data_len = sizeof(cccd_val);

        (void)ssapc_write_cmd(0, g_conn_id, &wparam);
        osal_printk("%s service discovery done, waiting for sensor data...\r\n", SENSOR_CLIENT_LOG);
    }
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_sensor_report_ssapc_cbk_register.
 * @else
 * @brief 注册 \c sle_sensor_report_ssapc_cbk_register 所需的回调函数。
 * @endif
 */
static errcode_t sle_sensor_report_ssapc_cbk_register(void)
{
    ssapc_callbacks_t ssapc_cbk = {0};
    ssapc_cbk.exchange_info_cb = sle_sensor_exchange_info_cbk;
    ssapc_cbk.find_structure_cb = sle_sensor_find_structure_cbk;
    ssapc_cbk.ssapc_find_property_cbk = sle_sensor_find_property_cbk;
    ssapc_cbk.find_structure_cmp_cb = sle_sensor_find_structure_cmp_cbk;
    ssapc_cbk.notification_cb = g_saved_notification_cb;
    ssapc_cbk.indication_cb = g_saved_indication_cb;

    errcode_t ret = ssapc_register_callbacks(&ssapc_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register ssapc callbacks fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* Public APIs. / 公共接口。 */

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_sensor_report_client_init.
 * @else
 * @brief 初始化 \c sle_sensor_report_client_init 对应的功能。
 * @endif
 */
void sle_sensor_report_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    /* Store application callbacks. / 保存应用层回调。 */
    g_saved_notification_cb = notification_cb;
    g_saved_indication_cb = indication_cb;

    /* Wait for the SLE core. / 等待 SLE 核心就绪。 */
    osal_msleep(SLE_WAIT_SLE_CORE_READY_MS);

    /* Register protocol callbacks. / 注册协议回调。 */
    (void)sle_sensor_report_seek_cbk_register();
    (void)sle_sensor_report_connect_cbk_register();
    (void)sle_sensor_report_ssapc_cbk_register();

    /* Enable SLE; the enable callback starts scanning. / 使能 SLE，并由使能回调启动扫描。 */
    (void)enable_sle();
}

/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_sensor_report_client_is_connected(void)
{
    return (uint16_t)g_connected;
}

/**
 * @if Eng
 * @brief Starts scanning for the target SLE server.
 * @else
 * @brief 开始扫描目标 SLE 服务端。
 * @endif
 */
void sle_sensor_report_client_start_scan(void)
{
    sle_seek_param_t seek_param = {0};
    seek_param.own_addr_type = 0;
    seek_param.filter_duplicates = 0;
    seek_param.seek_filter_policy = SLE_SEEK_FILTER_ALLOW_ALL;
    seek_param.seek_phys = SLE_SEEK_PHY_1M;
    seek_param.seek_type[0] = SLE_SEEK_ACTIVE;
    seek_param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    seek_param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;

    errcode_t ret = sle_set_seek_param(&seek_param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_set_seek_param fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
        return;
    }

    ret = sle_start_seek();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_start_seek fail: 0x%x\r\n", SENSOR_CLIENT_LOG, ret);
        return;
    }
    osal_printk("%s start seek.\r\n", SENSOR_CLIENT_LOG);
}
