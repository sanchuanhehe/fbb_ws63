/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Hello Server Source. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */
#include "common_def.h"
#include "securec.h"
#include "soc_osal.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "nv.h"
#include "sle_device_config_protocol.h"
#include "sle_device_config_server_adv.h"
#include "sle_device_config_server.h"

#define SLE_DEVICE_CONFIG_MTU_SIZE 520
#define UUID_LEN_2 2
#define UUID_INDEX 14
#define BT_INDEX_4 4
#define BT_INDEX_5 5
#define BT_INDEX_0 0

/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1

/* sle server app uuid for test */
static char g_sle_device_config_app_uuid[UUID_LEN_2] = {0x12, 0x34};

#define SLE_DEVICE_CONFIG_NV_ID 0x20A1

static sle_device_config_t g_device_config = {
    .magic = SLE_DEVICE_CONFIG_MAGIC,
    .report_interval_ms = 1000,
    .alarm_threshold_decicelsius = 800,
    .mode = 0,
    .version = SLE_DEVICE_CONFIG_VERSION,
};

/* sle connect acb handle */
static uint16_t g_sle_conn_hdl = 0;

/* sle server handle */
static uint8_t g_server_id = 0;

/* sle service handle */
static uint16_t g_service_handle = 0;

/* sle ntf property handle */
static uint16_t g_property_handle = 0;

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

#define SLE_DEVICE_CONFIG_SERVER_LOG "[sle device config server]"

static uint8_t g_sle_device_config_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                             0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint16_t sle_device_config_server_is_connected(void)
{
    return (g_sle_conn_hdl != 0) ? 1 : 0;
}

static void encode2byte_little(uint8_t *ptr, uint16_t data)
{
    *(uint8_t *)(ptr + 1) = (uint8_t)(data >> 0x8);
    *(uint8_t *)ptr = (uint8_t)data;
}

static void sle_uuid_set_base(sle_uuid_t *out)
{
    errcode_t ret;
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_device_config_base, SLE_UUID_LEN);
    if (ret != EOK) {
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("%s ssaps_mtu_changed_cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
                SLE_DEVICE_CONFIG_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("%s start service cbk server_id:%d, handle:%x, status:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, server_id,
                handle, status);
}

static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add service cbk server_id:%x, handle:%x, status:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, server_id,
                handle, status);
}

static void ssaps_add_property_cbk(uint8_t server_id,
                                   sle_uuid_t *uuid,
                                   uint16_t service_handle,
                                   uint16_t handle,
                                   errcode_t status)
{
    unused(uuid);
    osal_printk("%s add property cbk server_id:%x, service_handle:%x, handle:%x, status:%x\r\n",
                SLE_DEVICE_CONFIG_SERVER_LOG, server_id, service_handle, handle, status);
}

static void ssaps_add_descriptor_cbk(uint8_t server_id,
                                     sle_uuid_t *uuid,
                                     uint16_t service_handle,
                                     uint16_t property_handle,
                                     errcode_t status)
{
    unused(uuid);
    osal_printk("%s add descriptor cbk server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n",
                SLE_DEVICE_CONFIG_SERVER_LOG, server_id, service_handle, property_handle, status);
}

static void sle_device_config_read_request_cb(uint8_t server_id,
                                              uint16_t conn_id,
                                              ssaps_req_read_cb_t *read_cb_para,
                                              errcode_t status)
{
    unused(status);
    osal_printk("%s read request received, handle=0x%04x, type=0x%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                read_cb_para->handle, read_cb_para->type);

    if (read_cb_para->need_rsp) {
        ssaps_send_rsp_t rsp = {0};
        rsp.request_id = read_cb_para->request_id;
        rsp.status = ERRCODE_SLE_SUCCESS;
        rsp.value = (uint8_t *)&g_device_config;
        rsp.value_len = sizeof(g_device_config);
        ssaps_send_response(server_id, conn_id, &rsp);
        osal_printk("%s read response: interval=%u, threshold=%d, mode=%u\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                    g_device_config.report_interval_ms, g_device_config.alarm_threshold_decicelsius,
                    g_device_config.mode);
    }
}

static bool sle_device_config_is_valid(const sle_device_config_t *config)
{
    return (config->magic == SLE_DEVICE_CONFIG_MAGIC) && (config->version == SLE_DEVICE_CONFIG_VERSION) &&
           (config->report_interval_ms >= SLE_DEVICE_CONFIG_INTERVAL_MIN_MS) &&
           (config->report_interval_ms <= SLE_DEVICE_CONFIG_INTERVAL_MAX_MS) &&
           (config->alarm_threshold_decicelsius >= SLE_DEVICE_CONFIG_THRESHOLD_MIN) &&
           (config->alarm_threshold_decicelsius <= SLE_DEVICE_CONFIG_THRESHOLD_MAX) &&
           (config->mode <= SLE_DEVICE_CONFIG_MODE_MAX);
}

static void sle_device_config_write_request_cb(uint8_t server_id,
                                               uint16_t conn_id,
                                               ssaps_req_write_cb_t *write_cb_para,
                                               errcode_t status)
{
    unused(status);
    osal_printk("%s write request received, handle=0x%04x, length=%d\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                write_cb_para->handle, write_cb_para->length);

    uint8_t response_status = (uint8_t)ERRCODE_SLE_SUCCESS;
    sle_device_config_t candidate = {0};
    if ((write_cb_para->length != sizeof(candidate)) ||
        (memcpy_s(&candidate, sizeof(candidate), write_cb_para->value, write_cb_para->length) != EOK)) {
        response_status = (uint8_t)ERRCODE_SSAP_INCORRECT_DATA_TYPE;
        osal_printk("%s rejected: invalid payload length=%u\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, write_cb_para->length);
    } else if (!sle_device_config_is_valid(&candidate)) {
        response_status = (uint8_t)ERRCODE_SSAP_VALUE_OUT_OF_RANGE;
        osal_printk("%s rejected: interval=%u, threshold=%d, mode=%u\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                    candidate.report_interval_ms, candidate.alarm_threshold_decicelsius, candidate.mode);
    } else {
        g_device_config = candidate;
        errcode_t nv_ret =
            uapi_nv_write(SLE_DEVICE_CONFIG_NV_ID, (const uint8_t *)&g_device_config, sizeof(g_device_config));
        if (nv_ret != ERRCODE_SUCC) {
            response_status = (uint8_t)ERRCODE_SSAP_INSUFFICIENT_RESOURCES;
            osal_printk("%s NV write failed:0x%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, nv_ret);
        } else {
            osal_printk("%s config saved: interval=%u, threshold=%d, mode=%u\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                        g_device_config.report_interval_ms, g_device_config.alarm_threshold_decicelsius,
                        g_device_config.mode);
        }
    }

    if (write_cb_para->need_rsp) {
        ssaps_send_rsp_t rsp = {0};
        rsp.request_id = write_cb_para->request_id;
        rsp.status = response_status;
        ssaps_send_response(server_id, conn_id, &rsp);
        osal_printk("%s write response status=0x%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, response_status);
    }
}

static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("%s delete all service cbk server_id:%x, status:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, server_id,
                status);
}

static errcode_t sle_device_config_ssaps_register_cbks(void)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = sle_device_config_read_request_cb;
    ssaps_cbk.write_request_cb = sle_device_config_write_request_cb;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_register_callbacks fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_device_config_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_DEVICE_CONFIG_SERVICE_UUID, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add service fail, ret:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_device_config_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_DEVICE_CONFIG_TEST_PROPERTIES;
    property.operate_indication = SLE_DEVICE_CONFIG_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_DEVICE_CONFIG_NTF_REPORT_UUID, &property.uuid);
    property.value_len = sizeof(g_device_config);
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_device_config));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_device_config), &g_device_config, sizeof(g_device_config)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add property fail, ret:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_DEVICE_CONFIG_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add descriptor fail, ret:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_device_config_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("%s add service in\r\n", SLE_DEVICE_CONFIG_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_device_config_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_device_config_app_uuid, sizeof(g_sle_device_config_app_uuid)) !=
        EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_device_config_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_device_config_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service ok, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                SLE_DEVICE_CONFIG_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start service fail, ret:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service out\r\n", SLE_DEVICE_CONFIG_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_device_config_server_send_data(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_t param = {0};
    uint8_t send_buf[len];

    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = send_buf;
    param.value_len = len;
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}

static void sle_device_config_connect_state_changed_cbk(uint16_t conn_id,
                                                        const sle_addr_t *addr,
                                                        sle_acb_state_t conn_state,
                                                        sle_pair_state_t pair_state,
                                                        sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_DEVICE_CONFIG_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("%s addr:%02x:**:**:**:%02x:%02x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_hdl = conn_id;
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s disconnected, re-start announce\r\n", SLE_DEVICE_CONFIG_SERVER_LOG);
        g_sle_conn_hdl = 0;
        (void)sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_device_config_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete conn_id:%02x, status:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, conn_id, status);
    osal_printk("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);

    /* 配对完成后设置MTU */
    ssap_exchange_info_t parameter = {0};
    parameter.mtu_size = SLE_DEVICE_CONFIG_MTU_SIZE;
    parameter.version = 1;
    ssaps_set_info(g_server_id, &parameter);
}

static errcode_t sle_device_config_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_device_config_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_device_config_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_connection_register_callbacks fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_device_config_server_init(void)
{
    errcode_t ret;
    uint16_t actual_len = 0;
    sle_device_config_t saved_config = {0};

    ret = uapi_nv_read(SLE_DEVICE_CONFIG_NV_ID, sizeof(saved_config), &actual_len, (uint8_t *)&saved_config);
    if ((ret == ERRCODE_SUCC) && (actual_len == sizeof(saved_config)) && sle_device_config_is_valid(&saved_config)) {
        g_device_config = saved_config;
        osal_printk("%s config loaded from NV: interval=%u, threshold=%d, mode=%u\r\n", SLE_DEVICE_CONFIG_SERVER_LOG,
                    g_device_config.report_interval_ms, g_device_config.alarm_threshold_decicelsius,
                    g_device_config.mode);
    } else {
        osal_printk("%s using default config\r\n", SLE_DEVICE_CONFIG_SERVER_LOG);
    }

    /* 使能SLE */
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("[SLE Hello Server] sle enable fail!\r\n");
        return -1;
    }

    ret = sle_device_config_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_device_config_announce_register_cbks fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_device_config_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_device_config_conn_register_cbks fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_device_config_ssaps_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_device_config_ssaps_register_cbks fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_device_config_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_device_config_server_add fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_device_config_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_device_config_server_adv_init fail:%x\r\n", SLE_DEVICE_CONFIG_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s init ok\r\n", SLE_DEVICE_CONFIG_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}
