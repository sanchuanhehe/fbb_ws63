/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * @if Eng
 * @brief Implements data generation and reporting for the SLE sensor report server.
 * @else
 * @brief 实现 SLE 传感器上报服务端的数据生成与上报流程。
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
#include "sle_ssap_server.h"
#include "sle_errcode.h"
#include "sle_sensor_report_server.h"
#include "sle_sensor_report_server_adv.h"
#include "stdlib.h"

#define SENSOR_SERVER_LOG "[sensor server]"

/* Application UUID (16-bit). / 应用 UUID（16 位）。 */
static char g_sensor_app_uuid[2] = {0x12, 0x34};

/* SLE 128-bit base UUID. / SLE 128 位基础 UUID。 */
static uint8_t g_sensor_base_uuid[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                       0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define UUID_INDEX 14
#define SENSOR_TEMP_SCALE 100
#define SENSOR_TEMP_BASE 2500
#define SENSOR_TEMP_MAX 8500
#define SENSOR_TEMP_MIN 2000
#define SENSOR_DRIFT_RESET_COUNT 40
#define SENSOR_HUMIDITY_MIN 45
#define SENSOR_HUMIDITY_MAX 75
#define SENSOR_LIGHT_MIN 500
#define SENSOR_LIGHT_MAX 2000
#define SENSOR_COUNT 3
#define USEC_PER_MSEC 1000

/* UUID helpers based on hello and UART samples. / 参考 hello 和 UART 案例的 UUID 辅助函数。 */

/**
 * @if Eng
 * @brief Encodes a 16-bit value in little-endian order.
 * @else
 * @brief 按小端序编码 16 位数值。
 * @endif
 */
static void encode2byte_little(uint8_t *ptr, uint16_t data)
{
    *(uint8_t *)(ptr + 1) = (uint8_t)(data >> 0x8);
    *(uint8_t *)ptr = (uint8_t)data;
}

/**
 * @if Eng
 * @brief Initializes an SLE UUID with the sample base UUID.
 * @else
 * @brief 使用案例基础 UUID 初始化 SLE UUID。
 * @endif
 */
static void sle_uuid_set_base(sle_uuid_t *out)
{
    errcode_t ret;
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sensor_base_uuid, SLE_UUID_LEN);
    if (ret != EOK) {
        out->len = 0;
        return;
    }
    out->len = UUID_16BIT_LEN;
}

/**
 * @if Eng
 * @brief Builds a 16-bit service UUID from the sample base UUID.
 * @else
 * @brief 基于案例基础 UUID 构造 16 位服务 UUID。
 * @endif
 */
static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_16BIT_LEN;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

/* Global SSAP server state. / SSAP 服务端全局状态。 */
static uint8_t g_server_id = 0;
static uint16_t g_service_handle = 0;
static uint16_t g_data_property_handle = 0;  /* Periodic data property. / 常规数据属性。 */
static uint16_t g_alarm_property_handle = 0; /* Alarm data property. / 告警数据属性。 */
static uint16_t g_sle_conn_hdl = 0;
static bool g_connected = false;

/* Reporting timer. / 上报定时器。 */
static osal_timer g_sensor_report_timer = {0};

/* Counter used to generate simulated data. / 用于生成模拟数据的调用计数。 */
static uint32_t g_sensor_call_count = 0;

/* Simulated data generation. / 模拟数据生成。 */

/* A 16-point sine approximation scaled by 100. / 放大 100 倍的 16 点正弦近似表。 */
static const int16_t SINE_TABLE[16] = {0, 195, 383, 500, 500, 383, 195, 0, 0, -195, -383, -500, -500, -383, -195, 0};

/**
 * @if Eng
 * @brief Generates the simulated value used by \c get_simulated_temperature.
 * @else
 * @brief 生成 \c get_simulated_temperature 所需的模拟数值。
 * @endif
 */
static int16_t get_simulated_temperature(void)
{
    g_sensor_call_count++;
    /* Combine baseline, drift, and sine components. / 组合温度基准、漂移和简谐波分量。 */
    int16_t drift = (int16_t)(g_sensor_call_count * SENSOR_TEMP_SCALE);
    int16_t sine = SINE_TABLE[g_sensor_call_count & 0xF];
    int16_t noise = (int16_t)(rand() % 31 - 15); /* +/-0.15 C jitter. / 正负 0.15 摄氏度抖动。 */
    int16_t temp = SENSOR_TEMP_BASE + drift + sine + noise;
    /* Reset drift at the upper limit to repeat alarm cycles. / 达到上限后重置漂移，以重复告警周期。 */
    if (temp > SENSOR_TEMP_MAX) {
        g_sensor_call_count = SENSOR_DRIFT_RESET_COUNT;
        temp = SENSOR_TEMP_MAX;
    }
    if (temp < SENSOR_TEMP_MIN) {
        temp = SENSOR_TEMP_MIN;
    }
    return temp;
}

/**
 * @if Eng
 * @brief Generates the simulated value used by \c get_simulated_humidity.
 * @else
 * @brief 生成 \c get_simulated_humidity 所需的模拟数值。
 * @endif
 */
static uint8_t get_simulated_humidity(void)
{
    int16_t val = 60 + (rand() % 11 - 5); /* Simulate 60% +/- 5%. / 模拟 60% 上下浮动 5%。 */
    if (val < SENSOR_HUMIDITY_MIN) {
        val = SENSOR_HUMIDITY_MIN;
    }
    if (val > SENSOR_HUMIDITY_MAX) {
        val = SENSOR_HUMIDITY_MAX;
    }
    return (uint8_t)val;
}

/**
 * @if Eng
 * @brief Generates the simulated value used by \c get_simulated_light.
 * @else
 * @brief 生成 \c get_simulated_light 所需的模拟数值。
 * @endif
 */
static uint16_t get_simulated_light(void)
{
    int32_t val = 1200 + (rand() % 401 - 200); /* Simulate 1200 +/- 200 lux. / 模拟 1200 上下浮动 200 lux。 */
    if (val < SENSOR_LIGHT_MIN) {
        val = SENSOR_LIGHT_MIN;
    }
    if (val > SENSOR_LIGHT_MAX) {
        val = SENSOR_LIGHT_MAX;
    }
    return (uint16_t)val;
}

/* Timer callback for packing and sending data. / 打包并发送数据的定时器回调。 */

/**
 * @if Eng
 * @brief Generates and reports one periodic sensor data frame.
 * @else
 * @brief 生成并上报一帧周期性传感器数据。
 * @endif
 */
static void sensor_report_timer_cb(unsigned long arg)
{
    unused(arg);

    if (!g_connected) {
        return;
    }

    sensor_data_frame_t frame;
    (void)memset_s(&frame, sizeof(frame), 0, sizeof(frame));

    /* Generate simulated measurements. / 生成模拟测量数据。 */
    frame.temperature = get_simulated_temperature();
    frame.humidity = get_simulated_humidity();
    frame.light = get_simulated_light();
    frame.sensor_count = SENSOR_COUNT;

    /* Capture the timestamp. / 获取时间戳。 */
    osal_timeval tv;
    osal_gettimeofday(&tv);
    frame.timestamp = (uint32_t)(tv.tv_sec * USEC_PER_MSEC + tv.tv_usec / USEC_PER_MSEC);

    /* Select a property according to the alarm threshold. / 根据告警阈值选择属性通道。 */
    uint16_t prop_handle;
    bool is_alarm = (frame.temperature > TEMP_ALARM_HIGH || frame.temperature < TEMP_ALARM_LOW);

    if (is_alarm) {
        frame.frame_type = SENSOR_FRAME_TYPE_ALARM;
        prop_handle = g_alarm_property_handle;
        osal_printk("%s ** ALARM ** temp=%d.%02dC, using IND Indicate\r\n", SENSOR_SERVER_LOG,
                    frame.temperature / SENSOR_TEMP_SCALE,
                    (frame.temperature >= 0) ? (frame.temperature % SENSOR_TEMP_SCALE)
                                             : (-frame.temperature % SENSOR_TEMP_SCALE));
    } else {
        frame.frame_type = SENSOR_FRAME_TYPE_PERIODIC;
        prop_handle = g_data_property_handle;
    }

    uint8_t send_buf[sizeof(sensor_data_frame_t)];
    (void)memcpy_s(send_buf, sizeof(send_buf), &frame, sizeof(frame));

    ssaps_ntf_ind_t param = {0};
    param.handle = prop_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = send_buf;
    param.value_len = sizeof(send_buf);

    (void)ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);

    /* Restart the one-shot timer for periodic reporting. / 重启单次定时器以实现周期上报。 */
    (void)osal_timer_start(&g_sensor_report_timer);
}

/* SSAPS callbacks. / SSAPS 回调。 */

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_add_service_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_add_service_cbk 的异步事件。
 * @endif
 */
static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(server_id);
    unused(uuid);
    unused(handle);
    osal_printk("%s add service cbk, status: 0x%x\r\n", SENSOR_SERVER_LOG, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_add_property_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_add_property_cbk 的异步事件。
 * @endif
 */
static void ssaps_add_property_cbk(uint8_t server_id,
                                   sle_uuid_t *uuid,
                                   uint16_t service_handle,
                                   uint16_t handle,
                                   errcode_t status)
{
    unused(server_id);
    unused(uuid);
    unused(service_handle);
    osal_printk("%s add property cbk, handle: 0x%x, status: 0x%x\r\n", SENSOR_SERVER_LOG, handle, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_add_descriptor_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_add_descriptor_cbk 的异步事件。
 * @endif
 */
static void ssaps_add_descriptor_cbk(uint8_t server_id,
                                     sle_uuid_t *uuid,
                                     uint16_t service_handle,
                                     uint16_t property_handle,
                                     errcode_t status)
{
    unused(server_id);
    unused(uuid);
    unused(service_handle);
    osal_printk("%s add descriptor cbk, property_handle: 0x%x, status: 0x%x\r\n", SENSOR_SERVER_LOG, property_handle,
                status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_start_service_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_start_service_cbk 的异步事件。
 * @endif
 */
static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    unused(server_id);
    osal_printk("%s start service cbk, handle: 0x%x, status: 0x%x\r\n", SENSOR_SERVER_LOG, handle, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_delete_all_service_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_delete_all_service_cbk 的异步事件。
 * @endif
 */
static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("%s delete all service cbk, server_id: %u, status: 0x%x\r\n", SENSOR_SERVER_LOG, server_id, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_mtu_changed_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_mtu_changed_cbk 的异步事件。
 * @endif
 */
static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *info, errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    osal_printk("%s mtu changed cbk, mtu: %u, status: 0x%x\r\n", SENSOR_SERVER_LOG, (info != NULL) ? info->mtu_size : 0,
                status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_read_request_cb.
 * @else
 * @brief 处理分发给 \c ssaps_read_request_cb 的异步事件。
 * @endif
 */
static void ssaps_read_request_cb(uint8_t server_id,
                                  uint16_t conn_id,
                                  ssaps_req_read_cb_t *read_cb_para,
                                  errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
    /* Client reads are unused in this scenario. / 本场景不处理客户端读请求。 */
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_write_request_cb.
 * @else
 * @brief 处理分发给 \c ssaps_write_request_cb 的异步事件。
 * @endif
 */
static void ssaps_write_request_cb(uint8_t server_id,
                                   uint16_t conn_id,
                                   ssaps_req_write_cb_t *write_cb_para,
                                   errcode_t status)
{
    unused(conn_id);
    unused(status);

    /* The stack handles CCCD writes. / 协议栈负责处理 CCCD 写入。 */
    if (write_cb_para != NULL && write_cb_para->need_rsp &&
        write_cb_para->type != SSAP_DESCRIPTOR_CLIENT_CONFIGURATION) {
        ssaps_send_rsp_t rsp = {0};
        rsp.request_id = write_cb_para->request_id;
        rsp.status = ERRCODE_SLE_SUCCESS;
        (void)ssaps_send_response(server_id, conn_id, &rsp);
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_indicate_cfm_cb.
 * @else
 * @brief 处理分发给 \c ssaps_indicate_cfm_cb 的异步事件。
 * @endif
 */
static void ssaps_indicate_cfm_cb(uint8_t server_id,
                                  uint16_t conn_id,
                                  sle_indication_cfm_result_t cfm_result,
                                  errcode_t status)
{
    unused(server_id);
    osal_printk("%s indicate cfm cbk, conn_id: %u, result: %u, status: 0x%x\r\n", SENSOR_SERVER_LOG, conn_id,
                cfm_result, status);
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_sensor_report_ssaps_register_cbks.
 * @else
 * @brief 注册 \c sle_sensor_report_ssaps_register_cbks 所需的回调函数。
 * @endif
 */
static errcode_t sle_sensor_report_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cb;
    ssaps_cbk.write_request_cb = ssaps_write_request_cb;
    ssaps_cbk.indicate_cfm_cb = ssaps_indicate_cfm_cb;

    errcode_t ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register ssaps callbacks fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* SSAP service registration. / SSAP 服务注册。 */

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_sensor_report_add_service.
 * @else
 * @brief 添加 \c sle_sensor_report_add_service 配置的服务对象。
 * @endif
 */
static errcode_t sle_sensor_report_add_service(void)
{
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SENSOR_SERVICE_UUID, &service_uuid);
    errcode_t ret = ssaps_add_service_sync(g_server_id, &service_uuid, true, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add service fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_sensor_report_add_data_property.
 * @else
 * @brief 添加 \c sle_sensor_report_add_data_property 配置的服务对象。
 * @endif
 */
static errcode_t sle_sensor_report_add_data_property(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SENSOR_DATA_PROPERTY_PERMISSIONS;
    property.operate_indication = SENSOR_DATA_PROPERTY_OP_INDICATION;
    sle_uuid_setu2(SENSOR_DATA_PROPERTY_UUID, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(sensor_data_frame_t));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    property.value_len = sizeof(sensor_data_frame_t);

    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_data_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add data property fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }

    descriptor.permissions = SSAP_PERMISSION_READ;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_data_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add data descriptor fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_sensor_report_add_alarm_property.
 * @else
 * @brief 添加 \c sle_sensor_report_add_alarm_property 配置的服务对象。
 * @endif
 */
static errcode_t sle_sensor_report_add_alarm_property(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};

    property.permissions = SENSOR_PROPERTY_PERMISSIONS;
    property.operate_indication = SENSOR_ALARM_PROPERTY_OP_INDICATION;
    sle_uuid_setu2(SENSOR_ALARM_PROPERTY_UUID, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(sensor_data_frame_t));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    property.value_len = sizeof(sensor_data_frame_t);

    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_alarm_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add alarm property fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }

    /* Initialize the CCCD to 0x0002 to enable indications. / 将 CCCD 初值设为 0x0002 以使能指示。 */
    uint8_t ind_value[] = {0x02, 0x00};
    descriptor.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.value = ind_value;
    descriptor.value_len = sizeof(ind_value);

    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_alarm_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add alarm CCCD fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_sensor_report_server_add.
 * @else
 * @brief 添加 \c sle_sensor_report_server_add 配置的服务对象。
 * @endif
 */
static errcode_t sle_sensor_report_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    app_uuid.len = sizeof(g_sensor_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sensor_app_uuid, sizeof(g_sensor_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_sensor_report_add_service() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_sensor_report_add_data_property() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_sensor_report_add_alarm_property() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service ok, server_id:%x, svc_hdl:%x, data_hdl:%x, alarm_hdl:%x\r\n", SENSOR_SERVER_LOG,
                g_server_id, g_service_handle, g_data_property_handle, g_alarm_property_handle);

    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start service fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s service added successfully.\r\n", SENSOR_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/* Connection callbacks. / 连接回调。 */

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_report_connect_state_changed_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_report_connect_state_changed_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_report_connect_state_changed_cbk(uint16_t conn_id,
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
            g_sle_conn_hdl = conn_id;
            g_connected = true;
            osal_printk("%s connected, conn_id: 0x%x\r\n", SENSOR_SERVER_LOG, conn_id);
            break;

        case SLE_ACB_STATE_DISCONNECTED:
            osal_printk("%s disconnected, conn_id: 0x%x\r\n", SENSOR_SERVER_LOG, conn_id);
            /* Stop reporting. / 停止上报。 */
            (void)osal_timer_stop(&g_sensor_report_timer);
            g_sle_conn_hdl = 0;
            g_connected = false;
            /* Restart advertising. / 重新启动广播。 */
            (void)sle_start_announce(1);
            break;

        default:
            break;
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_report_pair_complete_cbk.
 * @else
 * @brief 处理分发给 \c sle_sensor_report_pair_complete_cbk 的异步事件。
 * @endif
 */
static void sle_sensor_report_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(addr);

    if (status != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s pair failed, conn_id: 0x%x, status: 0x%x\r\n", SENSOR_SERVER_LOG, conn_id, status);
        return;
    }

    osal_printk("%s pair complete, conn_id: 0x%x\r\n", SENSOR_SERVER_LOG, conn_id);

    /* Configure an MTU of 520 bytes. / 配置 520 字节 MTU。 */
    ssap_exchange_info_t info = {.mtu_size = 520, .version = 1};
    (void)ssaps_set_info(g_server_id, &info);

    /* Start the one-second timer. / 启动 1 秒定时器。 */
    if (g_sensor_report_timer.timer == NULL) {
        g_sensor_report_timer.handler = sensor_report_timer_cb;
        g_sensor_report_timer.data = 0;
        g_sensor_report_timer.interval = SENSOR_REPORT_INTERVAL_MS;
        int timer_ret = osal_timer_init(&g_sensor_report_timer);
        if (timer_ret != 0) {
            osal_printk("%s osal_timer_init fail: %d\r\n", SENSOR_SERVER_LOG, timer_ret);
            return;
        }
    }
    int timer_ret = osal_timer_start(&g_sensor_report_timer);
    if (timer_ret != 0) {
        osal_printk("%s osal_timer_start fail: %d\r\n", SENSOR_SERVER_LOG, timer_ret);
        return;
    }
    osal_printk("%s 1s periodic timer started.\r\n", SENSOR_SERVER_LOG);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_report_read_rssi_cb.
 * @else
 * @brief 处理分发给 \c sle_sensor_report_read_rssi_cb 的异步事件。
 * @endif
 */
static void sle_sensor_report_read_rssi_cb(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    unused(conn_id);
    unused(rssi);
    unused(status);
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_sensor_report_conn_register_cbks.
 * @else
 * @brief 注册 \c sle_sensor_report_conn_register_cbks 所需的回调函数。
 * @endif
 */
static errcode_t sle_sensor_report_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_sensor_report_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_sensor_report_pair_complete_cbk;
    conn_cbks.read_rssi_cb = sle_sensor_report_read_rssi_cb;

    errcode_t ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* Public APIs. / 公共接口。 */

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_sensor_report_server_init.
 * @else
 * @brief 初始化 \c sle_sensor_report_server_init 对应的功能。
 * @endif
 */
errcode_t sle_sensor_report_server_init(void)
{
    errcode_t ret;

    ret = enable_sle();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s enable_sle fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    ret = sle_sensor_report_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register announce cbks fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    ret = sle_sensor_report_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register conn cbks fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    ret = sle_sensor_report_ssaps_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register ssaps cbks fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    ret = sle_sensor_report_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s server add fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    ret = sle_sensor_report_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s adv init fail: 0x%x\r\n", SENSOR_SERVER_LOG, ret);
        return ret;
    }

    osal_printk("%s init complete.\r\n", SENSOR_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_sensor_report_server_is_connected(void)
{
    return (uint16_t)g_connected;
}
