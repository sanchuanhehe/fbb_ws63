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
#include "sle_hello_server_adv.h"
#include "sle_hello_server.h"

#define SLE_HELLO_MTU_SIZE 520
#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define UUID_INDEX 14
#define BT_INDEX_4 4
#define BT_INDEX_0 0

/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1

/* sle server app uuid for test */
static char g_sle_hello_app_uuid[UUID_LEN_2] = {0x12, 0x34};

/* server notify property value for test */
static char g_sle_hello_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

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

#define SLE_HELLO_SERVER_LOG "[sle hello server]"

/* hello world message */
static uint8_t g_hello_msg[] = "hello world";

static uint8_t g_sle_hello_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                     0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint16_t sle_hello_server_is_connected(void)
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
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_hello_base, SLE_UUID_LEN);
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
    osal_printk("%s ssaps_mtu_changed_cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n", SLE_HELLO_SERVER_LOG,
                server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("%s start service cbk server_id:%d, handle:%x, status:%x\r\n", SLE_HELLO_SERVER_LOG, server_id, handle,
                status);
}

static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add service cbk server_id:%x, handle:%x, status:%x\r\n", SLE_HELLO_SERVER_LOG, server_id, handle,
                status);
}

static void ssaps_add_property_cbk(uint8_t server_id,
                                   sle_uuid_t *uuid,
                                   uint16_t service_handle,
                                   uint16_t handle,
                                   errcode_t status)
{
    unused(uuid);
    osal_printk("%s add property cbk server_id:%x, service_handle:%x, handle:%x, status:%x\r\n", SLE_HELLO_SERVER_LOG,
                server_id, service_handle, handle, status);
}

static void ssaps_add_descriptor_cbk(uint8_t server_id,
                                     sle_uuid_t *uuid,
                                     uint16_t service_handle,
                                     uint16_t property_handle,
                                     errcode_t status)
{
    unused(uuid);
    osal_printk("%s add descriptor cbk server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n",
                SLE_HELLO_SERVER_LOG, server_id, service_handle, property_handle, status);
}

static void sle_hello_read_request_cb(uint8_t server_id,
                                      uint16_t conn_id,
                                      ssaps_req_read_cb_t *read_cb_para,
                                      errcode_t status)
{
    unused(status);
    osal_printk("%s read request received, handle=0x%04x, type=0x%x\r\n", SLE_HELLO_SERVER_LOG, read_cb_para->handle,
                read_cb_para->type);

    if (read_cb_para->need_rsp) {
        ssaps_send_rsp_t rsp = {0};
        rsp.request_id = read_cb_para->request_id;
        rsp.status = ERRCODE_SLE_SUCCESS;
        rsp.value = (uint8_t *)g_sle_hello_property_value;
        rsp.value_len = sizeof(g_sle_hello_property_value);
        ssaps_send_response(server_id, conn_id, &rsp);
        osal_printk("%s read response sent\r\n", SLE_HELLO_SERVER_LOG);
    }
}

static void sle_hello_write_request_cb(uint8_t server_id,
                                       uint16_t conn_id,
                                       ssaps_req_write_cb_t *write_cb_para,
                                       errcode_t status)
{
    unused(status);
    osal_printk("%s write request received, handle=0x%04x, length=%d\r\n", SLE_HELLO_SERVER_LOG, write_cb_para->handle,
                write_cb_para->length);

    /* validate data length */
    if (write_cb_para->length > sizeof(g_sle_hello_property_value)) {
        osal_printk("%s write data too large, rejected\r\n", SLE_HELLO_SERVER_LOG);
        return;
    }

    /* update local property value */
    if (memcpy_s(g_sle_hello_property_value, sizeof(g_sle_hello_property_value), write_cb_para->value,
                 write_cb_para->length) != EOK) {
        osal_printk("%s write memcpy failed\r\n", SLE_HELLO_SERVER_LOG);
        return;
    }

    osal_printk("%s property value updated\r\n", SLE_HELLO_SERVER_LOG);

    if (write_cb_para->need_rsp) {
        ssaps_send_rsp_t rsp = {0};
        rsp.request_id = write_cb_para->request_id;
        rsp.status = ERRCODE_SLE_SUCCESS;
        ssaps_send_response(server_id, conn_id, &rsp);
        osal_printk("%s write response sent\r\n", SLE_HELLO_SERVER_LOG);
    }
}

static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("%s delete all service cbk server_id:%x, status:%x\r\n", SLE_HELLO_SERVER_LOG, server_id, status);
}

static errcode_t sle_hello_ssaps_register_cbks(void)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = sle_hello_read_request_cb;
    ssaps_cbk.write_request_cb = sle_hello_write_request_cb;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_register_callbacks fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_hello_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_HELLO_SERVICE_UUID, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add service fail, ret:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_hello_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_HELLO_TEST_PROPERTIES;
    property.operate_indication = SLE_HELLO_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_HELLO_NTF_REPORT_UUID, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_hello_property_value));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_hello_property_value), g_sle_hello_property_value,
                 sizeof(g_sle_hello_property_value)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add property fail, ret:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_HELLO_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add descriptor fail, ret:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_hello_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("%s add service in\r\n", SLE_HELLO_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_hello_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_hello_app_uuid, sizeof(g_sle_hello_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_hello_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_hello_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service ok, server_id:%x, service_handle:%x, property_handle:%x\r\n", SLE_HELLO_SERVER_LOG,
                g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start service fail, ret:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service out\r\n", SLE_HELLO_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_hello_server_send_data(const uint8_t *data, uint16_t len)
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

static void sle_hello_connect_state_changed_cbk(uint16_t conn_id,
                                                const sle_addr_t *addr,
                                                sle_acb_state_t conn_state,
                                                sle_pair_state_t pair_state,
                                                sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_HELLO_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("%s addr:%02x:**:**:**:%02x:%02x\r\n", SLE_HELLO_SERVER_LOG, addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4]);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_hdl = conn_id;
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_HELLO_SERVER_LOG, conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s disconnected, re-start announce\r\n", SLE_HELLO_SERVER_LOG);
        g_sle_conn_hdl = 0;
        (void)sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_hello_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete conn_id:%02x, status:%x\r\n", SLE_HELLO_SERVER_LOG, conn_id, status);
    osal_printk("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_HELLO_SERVER_LOG, addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4]);

    /* 配对完成后设置MTU */
    ssap_exchange_info_t parameter = {0};
    parameter.mtu_size = SLE_HELLO_MTU_SIZE;
    parameter.version = 1;
    ssaps_set_info(g_server_id, &parameter);

    /* 发送 hello world */
    osal_printk("%s sending hello world...\r\n", SLE_HELLO_SERVER_LOG);
    sle_hello_server_send_data(g_hello_msg, sizeof(g_hello_msg) - 1);
    osal_printk("%s hello world sent.\r\n", SLE_HELLO_SERVER_LOG);
}

static errcode_t sle_hello_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_hello_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_hello_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_connection_register_callbacks fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_hello_server_init(void)
{
    errcode_t ret;

    /* 使能SLE */
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("[SLE Hello Server] sle enable fail!\r\n");
        return -1;
    }

    ret = sle_hello_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_hello_announce_register_cbks fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_hello_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_hello_conn_register_cbks fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_hello_ssaps_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_hello_ssaps_register_cbks fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_hello_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_hello_server_add fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_hello_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_hello_server_adv_init fail:%x\r\n", SLE_HELLO_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s init ok\r\n", SLE_HELLO_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}
