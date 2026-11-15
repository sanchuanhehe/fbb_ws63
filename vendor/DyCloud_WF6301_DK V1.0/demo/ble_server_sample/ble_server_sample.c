/*
 * Copyright (c) 2025 YunQiHui Network Technology (Shenzhen) Co., Ltd.
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Website: http://www.siotmate.com/
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "bts_def.h"
#include "securec.h"
#include "errcode.h"
#include "bts_le_gap.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "soc_osal.h"
#include "app_init.h"
#include "ble_server_adv.h"
#include "ble_server_sample.h"
#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "common_def.h"

/* 通用服务UUID */
#define BLE_UUID_GENERIC_SERVICE 0x1809 /* 通用服务 */
#define BLE_UUID_MEASUREMENT 0x2A1C     /* 测量值特征值 */
#define BLE_UUID_TYPE 0x2A1D            /* 类型特征值 */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902

/* 通用服务属性 */
#define MEASUREMENT_PROPERTIES (GATT_CHARACTER_PROPERTY_BIT_NOTIFY | GATT_CHARACTER_PROPERTY_BIT_READ)
#define TYPE_PROPERTIES (GATT_CHARACTER_PROPERTY_BIT_READ)

/* 通用服务变量 */
char g_uuid_app_uuid[] = {0x0, 0x0};
uint16_t g_measurement_att_hdl = 0;
uint16_t g_type_att_hdl = 0;
uint16_t g_conn_hdl = 0;
uint8_t g_server_id = 0;

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2

#define BLE_GENERIC_SERVER_TASK_PRIO 24
#define BLE_GENERIC_SERVER_STACK_SIZE 0x2000
#define BLE_GENERIC_UPDATE_TASK_PRIO 25
#define BLE_GENERIC_UPDATE_STACK_SIZE 0x2000

/* 宏定义 */
#define SIMPLE_TIMER_DEVICE 2         // 定时器设备ID
#define SIMPLE_IRQ_PRIORITY 2         // 中断优先级
#define SIMPLE_DELAY_US 1000000       // 定时器周期延迟，1s
#define SIMPLE_TASK_STACK_SIZE 0x1000 // 任务栈大小
#define SIMPLE_TASK_PRIORITY 24       // 任务优先级

/* 全局变量 */
static timer_handle_t g_timer_handle; // 定时器句柄

/* 函数声明 */
errcode_t ble_generic_server_notify_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len);
errcode_t ble_generic_server_notify_by_uuid(uint16_t uuid, const uint8_t *data, uint8_t len);

/* 将uint16_t UUID转换为bt_uuid_t结构 */
void stream_data_to_uuid(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    char uuid[] = {(uint8_t)(uuid_data >> OCTET_BIT_LEN), (uint8_t)uuid_data};
    out_uuid->uuid_len = UUID_LEN_2;
    if (memcpy_s(out_uuid->uuid, out_uuid->uuid_len, uuid, UUID_LEN_2) != EOK) {
        return;
    }
}

/* 比较两个服务UUID是否相同 */
errcode_t compare_service_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return ERRCODE_BT_FAIL;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}

/* 添加客户端特性配置描述符 */
static void ble_generic_server_add_descriptor_ccc(uint32_t server_id, uint32_t srvc_handle)
{
    bt_uuid_t ccc_uuid = {0};
    uint8_t ccc_data_val[] = {0x00, 0x00};

    osal_printk("[通用服务] 开始添加描述符\r\n");
    stream_data_to_uuid(BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_data_val);
    descriptor.value = ccc_data_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    osal_vfree(ccc_uuid.uuid);
}

/* 添加通用服务特征值和描述符 */
static void ble_generic_server_add_characters_and_descriptors(uint32_t server_id, uint32_t srvc_handle)
{
    bt_uuid_t measurement_uuid = {0};
    bt_uuid_t type_uuid = {0};

    // 设置初始测量值为0，单字节
    uint8_t measurement_value[1] = {0x00}; // 修改为1字节

    uint8_t type_value[] = {0x01}; /* 1: 通用类型 */

    osal_printk("[通用服务] 开始添加特征值\r\n");

    /* 添加测量值特征值 */
    stream_data_to_uuid(BLE_UUID_MEASUREMENT, &measurement_uuid);
    gatts_add_chara_info_t measurement_char;
    measurement_char.chara_uuid = measurement_uuid;
    measurement_char.properties = MEASUREMENT_PROPERTIES;
    measurement_char.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    measurement_char.value_len = sizeof(measurement_value);
    measurement_char.value = measurement_value;
    gatts_add_characteristic(server_id, srvc_handle, &measurement_char);

    /* 为测量值特征值添加CCC描述符 */
    ble_generic_server_add_descriptor_ccc(server_id, srvc_handle);

    /* 添加类型特征值 */
    stream_data_to_uuid(BLE_UUID_TYPE, &type_uuid);
    gatts_add_chara_info_t type_char;
    type_char.chara_uuid = type_uuid;
    type_char.properties = TYPE_PROPERTIES;
    type_char.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    type_char.value_len = sizeof(type_value);
    type_char.value = type_value;
    gatts_add_characteristic(server_id, srvc_handle, &type_char);
}

/* 服务添加回调 */
static void ble_generic_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    bt_uuid_t service_uuid = {0};
    osal_printk("[通用服务] 添加服务回调: 服务器ID: %d, 状态: %d, 服务句柄: %d, UUID长度: %d, UUID:", server_id, status,
                handle, uuid->uuid_len);
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");

    stream_data_to_uuid(BLE_UUID_GENERIC_SERVICE, &service_uuid);
    if (compare_service_uuid(uuid, &service_uuid) == ERRCODE_BT_SUCCESS) {
        ble_generic_server_add_characters_and_descriptors(server_id, handle);
        osal_printk("[通用服务] 启动服务\r\n");
        gatts_start_service(server_id, handle);
    } else {
        osal_printk("[通用服务] 未知的服务UUID\r\n");
        return;
    }
}

/* 特征值添加回调 */
static void ble_generic_server_characteristic_add_cbk(uint8_t server_id,
                                                      bt_uuid_t *uuid,
                                                      uint16_t service_handle,
                                                      gatts_add_character_result_t *result,
                                                      errcode_t status)
{
    int8_t i = 0;
    bt_uuid_t measurement_uuid = {0};
    bt_uuid_t type_uuid = {0};

    osal_printk(
        "[通用服务] 添加特征值回调: 服务器ID: %d, 状态: %d, 服务句柄: %d "
        "特征值句柄: %x, 特征值值句柄: %x, UUID长度: %d, UUID: ",
        server_id, status, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");

    stream_data_to_uuid(BLE_UUID_MEASUREMENT, &measurement_uuid);
    stream_data_to_uuid(BLE_UUID_TYPE, &type_uuid);

    if (compare_service_uuid(uuid, &measurement_uuid) == ERRCODE_BT_SUCCESS) {
        g_measurement_att_hdl = result->value_handle;
    } else if (compare_service_uuid(uuid, &type_uuid) == ERRCODE_BT_SUCCESS) {
        g_type_att_hdl = result->value_handle;
    }
}

/* 描述符添加回调 */
static void ble_generic_server_descriptor_add_cbk(uint8_t server_id,
                                                  bt_uuid_t *uuid,
                                                  uint16_t service_handle,
                                                  uint16_t handle,
                                                  errcode_t status)
{
    int8_t i = 0;
    osal_printk(
        "[通用服务] 添加描述符回调: 服务器ID: %d, 状态: %d, 服务句柄: %d, 描述符句柄: %x, "
        "UUID长度:%d, UUID: ",
        server_id, status, service_handle, handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
}

/* 启动服务回调 */
static void ble_generic_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[通用服务] 启动服务回调: 服务器ID: %d 状态: %d 服务句柄: %d\n", server_id, status, handle);
}

/* 写请求回调 */
static void ble_generic_server_receive_write_req_cbk(uint8_t server_id,
                                                     uint16_t conn_id,
                                                     gatts_req_write_cb_t *write_cb_para,
                                                     errcode_t status)
{
    osal_printk("[通用服务] 接收到写请求回调--服务器ID:%d 连接ID:%d\n", server_id, conn_id);
    osal_printk("请求ID:%d 属性句柄:%d 偏移量:%d 需要响应:%d 需要授权:%d 准备写入:%d\n", write_cb_para->request_id,
                write_cb_para->handle, write_cb_para->offset, write_cb_para->need_rsp, write_cb_para->need_authorize,
                write_cb_para->is_prep);
    osal_printk("数据长度:%d 数据:\n", write_cb_para->length);
    for (uint8_t i = 0; i < write_cb_para->length; i++) {
        osal_printk("%02x ", write_cb_para->value[i]);
    }
    osal_printk("\n");
    osal_printk("状态:%d\n", status);
}

/* 读请求回调 */
static void ble_generic_server_receive_read_req_cbk(uint8_t server_id,
                                                    uint16_t conn_id,
                                                    gatts_req_read_cb_t *read_cb_para,
                                                    errcode_t status)
{
    osal_printk("[通用服务] 接收到读请求--服务器ID:%d 连接ID:%d\n", server_id, conn_id);
    osal_printk("请求ID:%d 属性句柄:%d 偏移量:%d 需要响应:%d 需要授权:%d 长读取:%d\n", read_cb_para->request_id,
                read_cb_para->handle, read_cb_para->offset, read_cb_para->need_rsp, read_cb_para->need_authorize,
                read_cb_para->is_long);
    osal_printk("状态:%d\n", status);
}

/* 广播启用回调 */
static void ble_generic_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("广播启用 广播ID: %d, 状态:%d\n", adv_id, status);
}

/* 广播禁用回调 */
static void ble_generic_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("广播禁用 广播ID: %d, 状态:%d\n", adv_id, status);
}

/* 连接状态变化回调 */
void ble_generic_server_connect_change_cbk(uint16_t conn_id,
                                           bd_addr_t *addr,
                                           gap_ble_conn_state_t conn_state,
                                           gap_ble_pair_state_t pair_state,
                                           gap_ble_disc_reason_t disc_reason)
{
    osal_printk("连接状态变化 连接ID: %d, 状态: %d, 配对状态:%d, 地址 %x 断开原因 %x\n", conn_id, conn_state,
                pair_state, addr[0], disc_reason);
    g_conn_hdl = conn_id;

    if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        gap_ble_start_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    }
}

/* MTU变化回调 */
void ble_generic_server_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("MTU变化 服务器ID: %d, 连接ID: %d, MTU大小: %d, 状态:%d \n", server_id, conn_id, mtu_size, status);
}

/* BLE启用回调 */
void ble_generic_server_enable_cbk(errcode_t status)
{
    osal_printk("启用状态: %d\n", status);
}

/* 注册回调函数 */
static errcode_t ble_generic_server_register_callbacks(void)
{
    errcode_t ret;
    gap_ble_callbacks_t gap_cb = {0};
    gatts_callbacks_t service_cb = {0};

    gap_cb.start_adv_cb = ble_generic_server_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_generic_server_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_generic_server_connect_change_cbk;
    gap_cb.ble_enable_cb = ble_generic_server_enable_cbk;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("[通用服务] 注册GAP回调失败\r\n");
        return ERRCODE_BT_FAIL;
    }

    service_cb.add_service_cb = ble_generic_server_service_add_cbk;
    service_cb.add_characteristic_cb = ble_generic_server_characteristic_add_cbk;
    service_cb.add_descriptor_cb = ble_generic_server_descriptor_add_cbk;
    service_cb.start_service_cb = ble_generic_server_service_start_cbk;
    service_cb.read_request_cb = ble_generic_server_receive_read_req_cbk;
    service_cb.write_request_cb = ble_generic_server_receive_write_req_cbk;
    service_cb.mtu_changed_cb = ble_generic_server_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("[通用服务] 注册服务回调失败\r\n");
        return ERRCODE_BT_FAIL;
    }
    return ret;
}

/* 添加通用服务 */
uint8_t ble_generic_add_service(void)
{
    osal_printk("[通用服务] 添加服务开始\r\n");
    bt_uuid_t service_uuid = {0};
    stream_data_to_uuid(BLE_UUID_GENERIC_SERVICE, &service_uuid);
    gatts_add_service(g_server_id, &service_uuid, true);
    osal_printk("[通用服务] 添加服务结束\r\n");
    return ERRCODE_BT_SUCCESS;
}

/* 初始化通用服务器 */
errcode_t ble_generic_server_init(void)
{
    enable_ble();
    ble_generic_server_register_callbacks();
    bt_uuid_t app_uuid = {0};
    app_uuid.uuid_len = sizeof(g_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_uuid_app_uuid, sizeof(g_uuid_app_uuid)) != EOK) {
        return ERRCODE_BT_FAIL;
    }
    gatts_register_server(&app_uuid, &g_server_id);
    ble_generic_add_service();
    osal_printk("[通用服务] 初始化完成\r\n");
    ble_start_adv();
    return ERRCODE_BT_SUCCESS;
}

/* 更新测量值函数，发送单字节整数 */
errcode_t ble_generic_server_update_measurement(uint8_t value)
{
    uint8_t data[1]; // 使用1字节数据
    data[0] = value;

    return ble_generic_server_notify_by_handle(g_measurement_att_hdl, data, sizeof(data));
}

/* 通过句柄发送通知 */
errcode_t ble_generic_server_notify_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_t param = {0};
    uint16_t conn_id = g_conn_hdl;

    param.attr_handle = attr_handle;
    param.value = osal_vmalloc(len);
    param.value_len = len;

    if (param.value == NULL) {
        osal_printk("[通用服务][错误] 通知分配内存失败\r\n");
        return ERRCODE_BT_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[通用服务][错误] 通知内存复制失败\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate(g_server_id, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* 通过UUID发送通知 */
errcode_t ble_generic_server_notify_by_uuid(uint16_t uuid, const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    uint16_t conn_id = g_conn_hdl;

    param.start_handle = 0;
    param.end_handle = 0xffff;
    stream_data_to_uuid(uuid, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);

    if (param.value == NULL) {
        osal_printk("[通用服务][错误] 通知分配内存失败\r\n");
        return ERRCODE_BT_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[通用服务][错误] 通知内存复制失败\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(g_server_id, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* 定时器回调函数，发送递增整数 */
static void simple_timer_callback(uintptr_t param)
{
    unused(param);
    static uint8_t value = 1; // 从1开始递增

    osal_printk("发送值：%d\r\n", value);
    ble_generic_server_update_measurement(value);
    value++;
    if (value > 10) { // 循环发送1到10
        value = 1;
    }

    /* 重新启动定时器，实现持续触发 */
    uapi_timer_start(g_timer_handle, SIMPLE_DELAY_US, simple_timer_callback, 0);
}

/* 定时器任务函数 */
static void *simple_timer_task(const char *arg)
{
    unused(arg); // 避免未使用参数警告
    /* 初始化定时器模块 */
    uapi_timer_init();
    uapi_timer_adapter(SIMPLE_TIMER_DEVICE, TIMER_2_IRQN, SIMPLE_IRQ_PRIORITY);

    /* 创建定时器 */
    if (uapi_timer_create(SIMPLE_TIMER_DEVICE, &g_timer_handle) != 0) {
        osal_printk("Failed to create timer\n");
        return NULL;
    }

    /* 初次启动定时器 */
    if (uapi_timer_start(g_timer_handle, SIMPLE_DELAY_US, simple_timer_callback, 0) != 0) {
        osal_printk("Failed to start timer\n");
        uapi_timer_delete(g_timer_handle);
        return NULL;
    }

    /* 让任务持续运行，避免退出 */
    while (1) {
        osal_msleep(1000); // 休眠1秒 = 1000，避免忙等待占用过多资源
    }

    return NULL;
}

/* 入口函数 */
static void ble_generic_server_entry(void)
{
    osal_task *task_handle = NULL;
    osal_task *task_update_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ble_generic_server_init, 0, "ble_generic_server",
                                      BLE_GENERIC_SERVER_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_GENERIC_SERVER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    task_update_handle =
        osal_kthread_create((osal_kthread_handler)simple_timer_task, NULL, "SimpleTimerTask", SIMPLE_TASK_STACK_SIZE);
    if (task_update_handle != NULL) {
        osal_kthread_set_priority(task_update_handle, SIMPLE_TASK_PRIORITY);
    } else {
        osal_printk("Failed to create timer task\n");
    }
    osal_kthread_unlock();
}

app_run(ble_generic_server_entry);
