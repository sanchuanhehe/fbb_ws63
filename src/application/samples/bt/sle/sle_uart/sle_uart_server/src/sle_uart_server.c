/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Implements the service and connection logic of the SLE UART server.
 * @else
 * @brief 实现 SLE UART 服务端的服务与连接逻辑。
 * @endif
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
#include "sle_uart_server_adv.h"
#include "sle_uart_server.h"

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define UUID_INDEX 14
#define BT_INDEX_4 4
#define BT_INDEX_0 0

/* Advertising identifier. / 广播标识。 */
#define SLE_ADV_HANDLE_DEFAULT 1

/* SLE server application UUID. / SLE 服务端应用 UUID。 */
static char g_sle_uart_app_uuid[UUID_LEN_2] = {0x12, 0x34};

/* SLE connection ACB handle. / SLE 连接 ACB 句柄。 */
static uint16_t g_sle_conn_hdl = 0;

/* SLE server handle. / SLE 服务端句柄。 */
static uint8_t g_server_id = 0;

/* SLE service handle. / SLE 服务句柄。 */
static uint16_t g_service_handle = 0;

/* SLE notification property handle. / SLE 通知属性句柄。 */
static uint16_t g_property_handle = 0;

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

#define SLE_UART_SERVER_LOG "[sle uart server]"

static uint8_t g_sle_uart_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Callbacks supplied by the application. / 应用层传入的回调。 */
static ssaps_read_request_callback g_read_cb = NULL;
static ssaps_write_request_callback g_write_cb = NULL;

static bool g_connected = false;

/**
 * @if Eng
 * @brief Reports whether the SLE link is connected.
 * @else
 * @brief 返回 SLE 链路是否已连接。
 * @endif
 */
uint16_t sle_uart_server_is_connected(void)
{
    return g_connected ? 1 : 0;
}

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
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_uart_base, SLE_UUID_LEN);
    if (ret != EOK) {
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2;
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
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_mtu_changed_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_mtu_changed_cbk 的异步事件。
 * @endif
 */
static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("%s ssaps_mtu_changed_cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
                server_id, conn_id, mtu_size->mtu_size, status);
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
    osal_printk("%s start service cbk server_id:%d, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG, server_id, handle,
                status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c ssaps_add_service_cbk.
 * @else
 * @brief 处理分发给 \c ssaps_add_service_cbk 的异步事件。
 * @endif
 */
static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    osal_printk("%s add service cbk server_id:%x, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG, server_id, handle,
                status);
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
    unused(uuid);
    osal_printk("%s add property cbk server_id:%x, service_handle:%x, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
                server_id, service_handle, handle, status);
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
    unused(uuid);
    osal_printk("%s add descriptor cbk server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n",
                SLE_UART_SERVER_LOG, server_id, service_handle, property_handle, status);
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
    osal_printk("%s delete all service cbk server_id:%x, status:%x\r\n", SLE_UART_SERVER_LOG, server_id, status);
}

/**
 * @if Eng
 * @brief Sends UART data to the peer through an SLE notification.
 * @else
 * @brief 通过 SLE 通知向对端发送串口数据。
 * @endif
 */
errcode_t sle_uart_server_send_notification(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_t param = {0};
    uint8_t send_buf[len];

    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = send_buf;
    param.value_len = len;
    if (memcpy_s(send_buf, len, data, len) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_uart_server_connect_state_changed_cbk.
 * @else
 * @brief 处理分发给 \c sle_uart_server_connect_state_changed_cbk 的异步事件。
 * @endif
 */
static void sle_uart_server_connect_state_changed_cbk(uint16_t conn_id,
                                                      const sle_addr_t *addr,
                                                      sle_acb_state_t conn_state,
                                                      sle_pair_state_t pair_state,
                                                      sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_UART_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("%s addr:%02x:**:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG, addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4]);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_hdl = conn_id;
        g_connected = true;
        osal_printk("%s connected, conn_id=0x%02x\r\n", SLE_UART_SERVER_LOG, conn_id);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_conn_hdl = 0;
        g_connected = false;
        osal_printk("%s disconnected, re-start announce\r\n", SLE_UART_SERVER_LOG);

        /* Drain stale queued data. / 清空消息队列中的残留数据。 */
        uint8_t dummy[CONFIG_SLE_UART_MSGQ_ITEM_SIZE];
        uint32_t len = CONFIG_SLE_UART_MSGQ_ITEM_SIZE;
        while (osal_msg_queue_read_copy(g_sle_uart_server_msgq_id, dummy, &len, 0) == OSAL_SUCCESS) {
            len = CONFIG_SLE_UART_MSGQ_ITEM_SIZE;
        }

        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_uart_server_pair_complete_cbk.
 * @else
 * @brief 处理分发给 \c sle_uart_server_pair_complete_cbk 的异步事件。
 * @endif
 */
static void sle_uart_server_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete conn_id:%02x, status:%x\r\n", SLE_UART_SERVER_LOG, conn_id, status);
    osal_printk("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG, addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4]);

    /* Configure the MTU after pairing. / 配对完成后配置 MTU。 */
    ssap_exchange_info_t parameter = {0};
    parameter.mtu_size = CONFIG_SLE_UART_MTU_SIZE;
    parameter.version = 1;
    ssaps_set_info(g_server_id, &parameter);

    osal_printk("%s === bridge ready ===\r\n", SLE_UART_SERVER_LOG);
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_uart_server_ssaps_register_cbks.
 * @else
 * @brief 注册 \c sle_uart_server_ssaps_register_cbks 所需的回调函数。
 * @endif
 */
static errcode_t sle_uart_server_ssaps_register_cbks(void)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = g_read_cb;
    ssaps_cbk.write_request_cb = g_write_cb;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_register_callbacks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_uart_server_service_add.
 * @else
 * @brief 添加 \c sle_uart_server_service_add 配置的服务对象。
 * @endif
 */
static errcode_t sle_uart_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UART_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_uart_server_property_add.
 * @else
 * @brief 添加 \c sle_uart_server_property_add 配置的服务对象。
 * @endif
 */
static errcode_t sle_uart_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_UART_SRV_PROPERTIES;
    property.operate_indication = SLE_UART_SRV_OPERATION;
    sle_uuid_setu2(SLE_UART_SERVER_NTF_REPORT, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(uint8_t));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    property.value[0] = 0;

    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add property fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }

    descriptor.permissions = SLE_UART_SRV_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s add descriptor fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Adds the service object configured by \c sle_uart_server_add.
 * @else
 * @brief 添加 \c sle_uart_server_add 配置的服务对象。
 * @endif
 */
static errcode_t sle_uart_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("%s add service in\r\n", SLE_UART_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uart_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uart_app_uuid, sizeof(g_sle_uart_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uart_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uart_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service ok, server_id:%x, service_handle:%x, property_handle:%x\r\n", SLE_UART_SERVER_LOG,
                g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s add service out\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_uart_server_conn_register_cbks.
 * @else
 * @brief 注册 \c sle_uart_server_conn_register_cbks 所需的回调函数。
 * @endif
 */
static errcode_t sle_uart_server_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_uart_server_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_uart_server_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_connection_register_callbacks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_uart_server_init.
 * @else
 * @brief 初始化 \c sle_uart_server_init 对应的功能。
 * @endif
 */
errcode_t sle_uart_server_init(ssaps_read_request_callback read_cb, ssaps_write_request_callback write_cb)
{
    errcode_t ret;

    /* Store callbacks supplied by the application. / 保存应用层传入的回调。 */
    g_read_cb = read_cb;
    g_write_cb = write_cb;

    /* Enable the SLE subsystem. / 使能 SLE 子系统。 */
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("[SLE UART Server] sle enable fail!\r\n");
        return -1;
    }

    ret = sle_uart_server_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s announce register cbks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_uart_server_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s conn register cbks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_uart_server_ssaps_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps register cbks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_uart_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s server add fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s service added, ready for connection\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}
