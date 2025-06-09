/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE UART Server Source. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 *
 * Code Comments: Written by Lamonce.
 * Last Modified: April 10, 2025 \n
 *
 * Introduction:
 * This file implements the server-side functionality for SLE UART
 * communications. It provides a complete GATT server implementation with service,
 * characteristic, and descriptor management. The server broadcasts its availability,
 * accepts client connections, handles pairing, and supports bidirectional data transmission
 * through both UUID-based and handle-based methods. Key features include connection state
 * management, callback registration for various events, and MTU negotiation.
 *
 * 简介：
 * 本文件实现了 SLE UART 通信的服务端功能。它提供了完整的 GATT 服务器
 * 实现，包括服务、特征和描述符管理。服务端广播自身可用性，接受客户端连接请求，处理配对过程，
 * 并通过基于 UUID 和基于句柄的方法支持双向数据传输。主要功能包括连接状态管理、各类事件的
 * 回调注册以及 MTU 协商。整个实现遵循星闪低功耗通信协议规范，确保高效、可靠的设备间通信。
 *
 * DISCLAIMER:
 * This code is provided for reference and learning purposes only.
 * No warranty of correctness, completeness, or suitability for any purpose is provided.
 * Before using in production environments, please review and test thoroughly.
 *
 * 免责声明:
 * 本文件中的代码及注释仅供学习和参考使用，不保证其在所有环境下的正确性和完整性。
 * 在实际项目中使用前，请根据具体需求进行适当的修改和测试。
 *
 */

#include "common_def.h"             // 常用函数定义
#include "securec.h"                // 安全函数库，用于替代标准 C 库中不安全的函数
#include "soc_osal.h"               // 硬件抽象层
#include "sle_errcode.h"            // 错误码定义
#include "sle_connection_manager.h" // 连接管理
#include "sle_device_discovery.h"   // 设备发现相关
#include "sle_uart_server_adv.h"    // 广播相关头文件
#include "sle_uart_server.h"        // Server 端头文件

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2  // 16 位 UUID 长度
#define UUID_INDEX 14 // UUID 最后两字节索引
#define BT_INDEX_4 4
#define BT_INDEX_0 0
#define UART_BUFF_LENGTH 0x100 // UART 缓冲区长度

/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1 // 设备公开 ID
/* sle server app uuid for test */
static char g_sle_uuid_app_uuid[UUID_LEN_2] = {0x12, 0x34}; // 服务端应用 UUID
/* server notify property uuid for test */
static char g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; // 特征值
/* sle connect acb handle */
static uint16_t g_sle_conn_hdl = 0; // 连接句柄
/* sle server handle */
static uint8_t g_server_id = 0; // 服务端 ID
/* sle service handle */
static uint16_t g_service_handle = 0; // 服务句柄
/* sle ntf property handle */
static uint16_t g_property_handle = 0; // 特征句柄
/* sle pair acb handle */
uint16_t g_sle_pair_hdl; // 配对句柄

#define UUID_16BIT_LEN 2   // 16 位 UUID 长度
#define UUID_128BIT_LEN 16 // 128 位 UUID 长度
#define sample_at_log_print(fmt, args...) osal_printk(fmt, ##args)
#define SLE_UART_SERVER_LOG "[sle uart server]"                      // 日志前缀
#define SLE_SERVER_INIT_DELAY_MS 1000                                // 延时 1 秒
static sle_uart_server_msg_queue g_sle_uart_server_msg_queue = NULL; // 消息队列

// 星闪标准服务标识 基础标识（Base UUID）：37BEA880-FC70-11EA-B720-000000000000
// 带上这个基础标识表示这个星闪服务
// Base UUID 后面6字节是媒体接入层标识（在某个网段内，分配给网络设备的用于网络通信寻址的唯一标识）
// 在用于产品开发时，厂商需要向 SparkLink 组织申请
static uint8_t g_sle_uart_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 获取连接句柄
uint16_t get_connect_id(void)
{
    return g_sle_conn_hdl;
}

/**
 * @brief 将16位整数（uint16_t）以小端字节序的方式存储到内存中
 *
 * @param _ptr 低地址指针
 * @param data 数据
 *
 * @attention 将16位整数 data 的低8位（最低有效字节）存储到 _ptr 指向的地址
 * @attention 将16位整数 data 的高8位（最高有效字节）存储到 _ptr+1 指向的地址
 */
static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

// 设置服务 UUID 的基础值
static void sle_uuid_set_base(sle_uuid_t *out)
{
    errcode_t ret;
    // 复制 UUID 的基础值
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_uart_base, SLE_UUID_LEN);
    if (ret != EOK)
    {
        sample_at_log_print("%s sle_uuid_set_base memcpy fail\n", SLE_UART_SERVER_LOG);
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2; // 设置 UUID 的长度为 2
}

// 设置长度为 2 的服务 UUID 的值
static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);                         // 设置 UUID 的基础值
    out->len = UUID_LEN_2;                          // 设置 UUID 的长度为 2
    encode2byte_little(&out->uuid[UUID_INDEX], u2); // 将 16 位整数以小端字节序存储到 UUID 末尾
}

// 输出 UUID 的值
static void sle_uart_uuid_print(sle_uuid_t *uuid)
{
    if (uuid == NULL)
    {
        sample_at_log_print("%s uuid_print,uuid is null\r\n", SLE_UART_SERVER_LOG);
        return;
    }

    // 检查 UUID 长度
    if (uuid->len == UUID_16BIT_LEN)
    {
        sample_at_log_print("%s uuid: %02x %02x.\n", SLE_UART_SERVER_LOG,
                            uuid->uuid[14], uuid->uuid[15]); /* 14 15: uuid index */
    }
    else if (uuid->len == UUID_128BIT_LEN)
    {
        sample_at_log_print("%s uuid: \n", SLE_UART_SERVER_LOG); /* 14 15: uuid index */
        sample_at_log_print("%s 0x%02x 0x%02x 0x%02x \n", SLE_UART_SERVER_LOG, uuid->uuid[0], uuid->uuid[1],
                            uuid->uuid[2], uuid->uuid[3]);
        sample_at_log_print("%s 0x%02x 0x%02x 0x%02x \n", SLE_UART_SERVER_LOG, uuid->uuid[4], uuid->uuid[5],
                            uuid->uuid[6], uuid->uuid[7]);
        sample_at_log_print("%s 0x%02x 0x%02x 0x%02x \n", SLE_UART_SERVER_LOG, uuid->uuid[8], uuid->uuid[9],
                            uuid->uuid[10], uuid->uuid[11]);
        sample_at_log_print("%s 0x%02x 0x%02x 0x%02x \n", SLE_UART_SERVER_LOG, uuid->uuid[12], uuid->uuid[13],
                            uuid->uuid[14], uuid->uuid[15]);
    }
}

// -------- ssapc 注册回调函数 --------

/**
 * @brief MTU 改变回调函数
 *
 * @param server_id 服务 ID
 * @param conn_id 连接 ID
 * @param mtu_size MTU 大小
 * @param status 状态码
 */
static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size,
                                  errcode_t status)
{
    sample_at_log_print("%s ssaps ssaps_mtu_changed_cbk callback server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
                        SLE_UART_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    if (g_sle_pair_hdl == 0)
    {
        g_sle_pair_hdl = conn_id + 1;
    }
}

/**
 * @brief 服务启动回调函数
 *
 * @param server_id 服务 ID
 * @param handle 服务句柄
 * @param status 状态码
 */
static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_at_log_print("%s start service cbk callback server_id:%d, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
                        server_id, handle, status);
}

/**
 * @brief ssaps 添加服务回调函数
 *
 * @param server_id 服务 ID
 * @param uuid 服务 UUID
 * @param handle 服务句柄
 * @param status 状态码
 */
static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    sample_at_log_print("%s add service cbk callback server_id:%x, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
                        server_id, handle, status);
    sle_uart_uuid_print(uuid);
}

/**
 * @brief 服务特征添加回调函数
 *
 * @param server_id 服务 ID
 * @param uuid 服务 UUID
 * @param service_handle 服务句柄
 * @param handle 特征句柄
 * @param status 状态码
 */
static void ssaps_add_property_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                   uint16_t handle, errcode_t status)
{
    sample_at_log_print("%s add property cbk callback server_id:%x, service_handle:%x,handle:%x, status:%x\r\n",
                        SLE_UART_SERVER_LOG, server_id, service_handle, handle, status);
    sle_uart_uuid_print(uuid);
}

/**
 * @brief 服务描述符添加回调函数
 *
 * @param server_id 服务 ID
 * @param uuid 服务 UUID
 * @param service_handle 服务句柄
 * @param property_handle 特征句柄
 * @param status 状态码
 */
static void ssaps_add_descriptor_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                     uint16_t property_handle, errcode_t status)
{
    sample_at_log_print("%s add descriptor cbk callback server_id:%x, service_handle:%x, property_handle:%x, \
        status:%x\r\n",
                        SLE_UART_SERVER_LOG, server_id, service_handle, property_handle, status);
    sle_uart_uuid_print(uuid);
}

/**
 * @brief 删除所有服务回调函数
 *
 * @param server_id 服务 ID
 * @param status 状态码
 */
static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    sample_at_log_print("%s delete all service callback server_id:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
                        server_id, status);
}

// ssaps 注册回调函数
static errcode_t sle_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
                                                                                              ssaps_write_callback)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};                              // 回调函数结构体
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;               // 添加服务回调函数
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;             // 添加特征回调函数
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;         // 添加描述符回调函数
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;           // 服务启动回调函数
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk; // 删除所有服务回调函数
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;               // MTU 改变回调函数
    ssaps_cbk.read_request_cb = ssaps_read_callback;                // 读请求回调函数
    ssaps_cbk.write_request_cb = ssaps_write_callback;              // 写请求回调函数
    ret = ssaps_register_callbacks(&ssaps_cbk);                     // 注册回调函数
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_ssaps_register_cbks,ssaps_register_callbacks fail :%x\r\n", SLE_UART_SERVER_LOG,
                            ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

// -------- ssapc 注册回调函数结束 ----

// 服务添加
static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};                                                  // 创建服务 UUID 结构体
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);                         // 设置服务 UUID
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle); // 添加一个ssap服务
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle uuid add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

// 添加特征
static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0}; // 创建特征信息结构体
    ssaps_desc_info_t descriptor = {0};   // 创建描述符信息结构体
    uint8_t ntf_value[] = {0x01, 0x0};    // 描述符数据

    property.permissions = SLE_UUID_TEST_PROPERTIES;                                                     // 特征权限，此 demo 设置为可读可写
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY; // 操作指示，数据值可被读取，通过通知方式传递给客户端
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);                                          // 设置特征 UUID

    // 分配内存给特征值（value 为指向特征值的指针）
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) // 检查内存分配是否成功
    {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
                 sizeof(g_sle_property_value)) != EOK) // 复制特征值
    {
        osal_vfree(property.value); // 当复制失败时，释放内存
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle); // 添加特征，并获取特征句柄
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle uart add property fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(property.value); // 当添加特征失败时，释放内存
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;                                                    // 特征权限，此 demo 设置为可读可写
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;                                                   // 描述符类型，属性说明描述符
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE; // 操作指示，数据值可被读取和写入，写入后产生反馈给客户端
    descriptor.value = ntf_value;                                                                         // 描述符数据
    descriptor.value_len = sizeof(ntf_value);                                                             // 描述符数据长度

    // 添加描述符
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle uart add descriptor fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(property.value);   // 若添加描述符失败，释放特征值内存
        osal_vfree(descriptor.value); // 释放描述符值内存
        return ERRCODE_SLE_FAIL;
    }
    // 添加特征成功后，释放特征值内存
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

// 添加服务
static errcode_t sle_uart_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0}; // 创建应用 UUID 结构体

    sample_at_log_print("%s sle uart add service in\r\n", SLE_UART_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uuid_app_uuid); // 设置应用 UUID 长度
    // 复制应用 UUID
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK)
    {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id); // 注册 ssap 服务端，参数:app_uuid:上层应用uuid，g_server_id:服务端ID

    // 添加服务
    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS)
    {
        ssaps_unregister_server(g_server_id); // 如果添加服务失败，注销服务端
        return ERRCODE_SLE_FAIL;
    }

    // 添加特征
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS)
    {
        ssaps_unregister_server(g_server_id); // 如果添加特征失败，注销服务端
        return ERRCODE_SLE_FAIL;
    }
    sample_at_log_print("%s sle uart add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                        SLE_UART_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);

    // 启动服务
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle uart add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    sample_at_log_print("%s sle uart add service out\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：report */
/**
 * @brief Server 端通过 UUID 向 Host（Client） 发送数据
 *
 * @param data 发送的数据
 * @param len 数据长度
 * @return errcode_t
 */
errcode_t sle_uart_server_send_report_by_uuid(const uint8_t *data, uint8_t len)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = {0};        // 创建通知/指示参数结构体
    param.type = SSAP_PROPERTY_TYPE_VALUE;      // 属性类型，特征值
    param.start_handle = g_service_handle;      // 起始句柄
    param.end_handle = g_property_handle;       // 结束句柄
    param.value_len = len;                      // 数据长度
    param.value = (uint8_t *)osal_vmalloc(len); // 动态分配内存给数据
    if (param.value == NULL)                    // 检查内存分配是否成功
    {
        sample_at_log_print("%s send report new fail\r\n", SLE_UART_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) // 复制数据到参数
    {
        sample_at_log_print("%s send input report memcpy fail\r\n", SLE_UART_SERVER_LOG);
        osal_vfree(param.value); // 当复制失败时，释放内存
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);                  // 设置 UUID
    ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_hdl, &param); // 发送通知/指示，具体发送状态取决于客户端特征配置描述符值
    if (ret != ERRCODE_SLE_SUCCESS)                                           // 检查发送是否成功
    {
        sample_at_log_print("%s sle_uart_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail :%x\r\n",
                            SLE_UART_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value); // 释放内存
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：report */
/**
 * @brief Server 端通过句柄向 Host（Client） 发送数据
 *
 * @param data 数据
 * @param len 数据长度
 * @return errcode_t
 */
errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_t param = {0};
    uint8_t receive_buf[UART_BUFF_LENGTH] = {0}; /* max receive length. */
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = receive_buf;
    param.value_len = len;
    if (memcpy_s(param.value, param.value_len, data, len) != EOK)
    {
        return ERRCODE_SLE_FAIL;
    }
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}

/**
 * @brief 连接状态改变回调函数
 *
 * @param conn_id 连接 ID
 * @param addr 设备地址
 * @param conn_state 连接状态
 * @param pair_state 配对状态
 * @param disc_reason 断开连接的原因
 */
static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                          sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    uint8_t sle_connect_state[] = "sle_dis_connect"; // 创建连接状态字符串，并初始化为 "sle_dis_connect"
    sample_at_log_print("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n",
                        SLE_UART_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    sample_at_log_print("%s connect state changed callback addr:%02x:**:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG,
                        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    if (conn_state == SLE_ACB_STATE_CONNECTED) // 已连接
    {
        g_sle_conn_hdl = conn_id; // 更新连接句柄
    }
    else if (conn_state == SLE_ACB_STATE_DISCONNECTED) // 未连接
    {
        g_sle_conn_hdl = 0;
        g_sle_pair_hdl = 0;
        if (g_sle_uart_server_msg_queue != NULL)
        {
            g_sle_uart_server_msg_queue(sle_connect_state, sizeof(sle_connect_state));
        }
    }
}

// 配对完成回调函数
static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    sample_at_log_print("%s pair complete conn_id:%02x, status:%x\r\n", SLE_UART_SERVER_LOG,
                        conn_id, status);
    sample_at_log_print("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG,
                        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    g_sle_pair_hdl = conn_id + 1;
    ssap_exchange_info_t parameter = {0};
    parameter.mtu_size = 520; // 设置 MTU 大小为 520 字节
    parameter.version = 1;
    ssaps_set_info(g_server_id, &parameter);
}

// 注册连接回调函数
static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk; // 连接状态改变回调
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;                 // 配对完成回调
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_conn_register_cbks,sle_connection_register_callbacks fail :%x\r\n",
                            SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

// 获取连接句柄
uint16_t sle_uart_client_is_connected(void)
{
    return g_sle_pair_hdl;
}

/* 初始化uuid server */
errcode_t sle_uart_server_init(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
                                                                                    ssaps_write_callback)
{
    errcode_t ret;

    /* 使能SLE */
    if (enable_sle() != ERRCODE_SUCC)
    {
        sample_at_log_print("[SLE Server] sle enbale fail !\r\n");
        return -1;
    }

    // 注册广播回调函数
    ret = sle_uart_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_init,sle_uart_announce_register_cbks fail :%x\r\n",
                            SLE_UART_SERVER_LOG, ret);
        return ret;
    }

    // 注册连接回调函数
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_init,sle_conn_register_cbks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }

    // 注册 ssaps 回调函数
    ret = sle_ssaps_register_cbks(ssaps_read_callback, ssaps_write_callback);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_init,sle_ssaps_register_cbks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }

    // 添加服务
    ret = sle_uart_server_add();
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_init,sle_uart_server_add fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }

    // 初始化广播
    ret = sle_uart_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_init,sle_uart_server_adv_init fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    sample_at_log_print("%s init ok\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

void sle_uart_server_register_msg(sle_uart_server_msg_queue sle_uart_server_msg)
{
    g_sle_uart_server_msg_queue = sle_uart_server_msg;
}