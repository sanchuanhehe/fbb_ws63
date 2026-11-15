/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Create Date : 2026.05
 * Description: one_touch_start.c 星闪Mesh 自动化入网Demo
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "errcode.h"
#include "securec.h"
#include "app_init.h"
#include "osal_task.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_mesh_sdk_network.h"
#include "sle_mesh_sdk_scene.h"
#include "sle_mesh_sdk_service.h"
#include "sle_mesh_sdk_equip.h"
#include "sle_mesh_sdk_system.h"
#include "sle_mesh_sdk_ota.h"
#include "sle_mesh_sdk_dfx.h"
#include "sle_device_discovery.h"
#include "enter_net.h"
#include "one_touch_start.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID SLE_TOUCH_SAMPLE

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 灯控demo */
#ifdef CONFIG_SAMPLE_SUPPORT_SLE_MESH_ONE_TOUCH_START
#define SAMPLE_DEMO_TASK_SEM_MAX_CNT                1           /* AT指令信号量 */
#define SLE_MESH_SERVICE_MULTI_GROUP_ID     0xFFFF  /* 服务组播ID */

/* 灯控demo */
#endif /* END of CONFIG_SAMPLE_SUPPORT_SLE_MESH_ONE_TOUCH_START */

/* 线程StackSize */
#define SLE_MESH_STACK_SIZE 0x2000
/* 线程调度优先级 */
#define SLE_INIT_THREAD_TASK_PRI 25
#define SLE_MESH_LED_CONTROL_DELAY 3000

#if defined(CONFIG_SLE_MESH_DEMO_SUPPORT) && defined(CONFIG_SLE_MESH_DEV_MA_ROLE)
/* 白名单信息 */
static sle_addr_t white_list[] = {
    {0, {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}},
    {0, {0x11, 0x11, 0x22, 0x33, 0x44, 0x55}},
    {0, {0x22, 0x11, 0x22, 0x33, 0x44, 0x55}},
};
#endif

#ifdef CONFIG_SLE_MESH_DEMO_SUPPORT
void net_start_cbk(uint16_t net_id, uint8_t result)
{
    osal_printk("[Demo] ACTION start %s\r\n", (result == SLE_MESH_START_SUCCESS) ? "success" : "failed !!! Need to start MESH NET.");
    osal_printk("[Demo] ACTION start net id %u\r\n", net_id);

#ifdef CONFIG_SLE_MESH_DEV_MA_ROLE
    /* 配置白名单 */
    errcode_t ret = sle_mesh_add_white_list(sizeof(white_list) / sizeof(white_list[0]), white_list);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Demo] add white list failed!\r\n");
    }
#endif
}

void device_access_net_cbk(uint16_t mesh_id, uint8_t result)
{
    osal_printk("[Demo] Access net %s\r\n", (result == SLE_MESH_ENROLL_SUCCESS) ? "success" : "failed !!!.");
    osal_printk("[Demo] Access mesh id 0x%x\r\n", mesh_id);
}

static void ota_state_callback(uint16_t mesh_id, sle_mesh_ota_state_t *ota_state)
{
    osal_printk("[Demo] OTA STATE, mesh_id: 0x%x, state: %d", mesh_id, ota_state->state);
}

#if defined(CONFIG_SLE_MESH_DEV_MA_ROLE) && defined(CONFIG_SLE_MESH_SUPPORT_OTA)
static void ota_firmware_callback(uint16_t device_num, sle_mesh_ota_firmware_t *device_frimware)
{
    // 固件个数为空，则固件信息获取结束，启动升级
    if (device_frimware == NULL) {
        sle_mesh_ota_start();
    } else {
        osal_printk("[Demo] device_num: %d, mesh id:0x%x, version:%s\r\n",
                    device_num, device_frimware->mesh_id, device_frimware->version);
        sle_mesh_ota_add_dev(1, &device_frimware->mesh_id);
    }
}
#endif
static bool ota_upgrade_request(sle_mesh_ota_firmware_t *device_frimware)
{
    osal_printk("[Demo][OTA]:upgrade request, version:%s\0\r\n", device_frimware->version);
    /* 默认允许升级 */
    return true;
}
#endif

static void dfx_query_cbk(sle_mesh_dfx_module_t module, uint8_t nums, uint16_t *record)
{
    // 打印打点的模型信息
    osal_printk("[Demo] dfx module: %#x, nums:%#x\r\n", module, nums);
    if (nums != 0 && record != NULL) {
        for (uint8_t i = 0; i < nums; ++i) {
            osal_printk("[Demo] item:0x%x, num:%u\r\n", i, record[i]);
        }
    }
}

static void sample_uuid_print(const sle_uuid_t *uuid)
{
    osal_printk("uuid len:%u uuid: ", uuid->len);
    for (uint8_t i = 0; i < SLE_UUID_LEN; ++i) {
        osal_printk("%02x", uuid->uuid[i]);
    }
    osal_printk("\r\n");
}

/* 创建订阅组回调函数 */
static void sample_create_group_cbk(const char *group_name, uint16_t group_id, uint8_t result)
{
    if (result != 0) {
        osal_printk(" create group is failed, result = %u \r\n", result);
        return;
    }
    osal_printk(" create group is success, group_name = %s group_id:%#x\r\n", group_name, group_id);
    return;
}

/* 释放订阅组回调函数 */
static void sample_release_group_cbk(uint16_t group_id, uint8_t result)
{
    if (result != 0) {
        osal_printk(" release group:%#x is failed, result = %u \r\n", group_id, result);
        return;
    }

    /* 删除订阅组信息 */
    osal_printk(" release group is success, group_id:%#x\r\n", group_id);
    return;
}

/* 增加订阅组实例回调函数 */
static void sample_add_group_instance_cbk(uint16_t group_id, const sle_mesh_subscribe_t *inst, uint8_t result)
{
    if (result != 0) {
        osal_printk(" add group:%#x insance is failed, result = %u \r\n", group_id, result);
        return;
    }
    
    if (inst == NULL) {
        osal_printk(" add group insance null ptr \r\n");
        return;
    }

    osal_printk(" add group instance is success, group_id:%#x mesh_id:%#x instance:%#x\r\n",
                group_id, inst->mesh_id, inst->instance);
    osal_printk(" add group instance is success, uuid:");
    sample_uuid_print(&inst->service_uuid);
    return;
}

/* 删除订阅组实例回调函数 */
static void sample_del_group_instance_cbk(uint16_t group_id, const sle_mesh_subscribe_t *inst, uint8_t result)
{
    if (result != 0) {
        osal_printk(" del group:%#x insance is failed, result = %u \r\n", group_id, result);
        return;
    }
    
    if (inst == NULL) {
        osal_printk(" del group insance null ptr \r\n");
        return;
    }

    osal_printk(" del group instance is success, group_id:%#x mesh_id:%#x instance:%#x\r\n",
                group_id, inst->mesh_id, inst->instance);
    osal_printk(" del group instance is success, uuid:");
    sample_uuid_print(&inst->service_uuid);
    return;
}

/* 删除订阅组实例回调函数 */
static void sample_service_query_cbk(uint16_t mesh_id,
                                     uint16_t num,
                                     const sle_mesh_service_query_info_t *services,
                                     uint8_t result)
{
    if (result != 0) {
        osal_printk(" query service info is failed, mesh id:%04x result = %u \r\n", mesh_id, result);
        return;
    }
    osal_printk(" query service info is success, mesh id = %#x \r\n", mesh_id);
    
    for (uint16_t i = 0; i < num; i++) {
        const sle_mesh_service_query_info_t *service = &services[i];
        osal_printk("service uuid: ");
        sample_uuid_print(&service->uuid);
        osal_printk("instance_count = %u \r\n", service->inst_num);
        for (uint8_t j = 0; j < service->inst_num; j++) {
            const sle_mesh_instance_query_info_t *inst = &service->inst_list[j];
            osal_printk("instance = 0x%02x \r\n", inst->instance);
            osal_printk("------------------------------------------------------- \r\n");
            for (uint8_t k = 0; k < inst->property_num; k++) {
                sle_mesh_property_t *prop = &inst->property_list[k];
                osal_printk("property uuid: ");
                sample_uuid_print(&prop->property_uuid);
                if (prop->value_type == SLE_MESH_PROPERTY_VALUE_TYPE_UINT32) {
                    osal_printk("  type = %u, value = %u, min = %u, max = %u \r\n",
                                prop->value_type, *((uint32_t *)prop->value), prop->value_min, prop->value_max);
                } else if (prop->value_type == SLE_MESH_PROPERTY_VALUE_TYPE_ARRAY) {
                    osal_printk("  type = %u, value = %d, min = %u, max = %u\r\n",
                                prop->value_type, *prop->value, prop->value_min, prop->value_max);
                }
            }
            osal_printk("------------------------------------------------------- \r\n");
        }
    }
    return;
}

int sle_sample_init(void)
{
#ifdef CONFIG_SLE_MESH_DEMO_SUPPORT
    osal_printk("[Demo] register cbk.\r\n");
    /* 注册mesh回调 */
    sle_mesh_equip_callback_t action_cbk = {
        .action_cbk = net_start_cbk,
    };
    sle_mesh_equip_register_callback(&action_cbk);

    /* 注册OTA回调 */
    sle_mesh_ota_callback_t cbk = {
        .request_cbk = ota_upgrade_request,
        .state_cbk = ota_state_callback,
#if defined(CONFIG_SLE_MESH_DEV_MA_ROLE) && defined(CONFIG_SLE_MESH_SUPPORT_OTA)
        .firmware_cbk = ota_firmware_callback,
#endif
    };
    sle_mesh_ota_register_callback(&cbk);
#endif
    sle_mesh_dfx_callback_t dfx_cbk = {
        .dfx_query_cbk = dfx_query_cbk,
    };

    sle_mesh_dfx_register_callback(&dfx_cbk);

    // 服务相关回调注册
    sle_mesh_service_management_callback_t service_call_back = {0};
    service_call_back.group_add_cbk = sample_create_group_cbk;
    service_call_back.group_rmv_cbk = sample_release_group_cbk;
    service_call_back.subscribe_add_cbk = sample_add_group_instance_cbk;
    service_call_back.subscribe_rmv_cbk = sample_del_group_instance_cbk;
    service_call_back.service_query_cbk = sample_service_query_cbk;

    sle_mesh_sdk_service_callback_register(&service_call_back);

    /* 调用入网原子能力接口 */
    sle_enter_net_sample_init();
    osal_msleep(SLE_MESH_LED_CONTROL_DELAY);

    /* 启动灯控demo任务 */
    led_init();

    sample_hardware_timer_init();
#ifdef CONFIG_SLE_MESH_DEV_MA_ROLE
    // 开始灯控
    while (true) {
        osal_printk("[Demo] this device run as Mesh MA.\r\n");
#ifdef CONFIG_SLE_MESH_SUPPORT_LIGHT_CTRL
        sle_mesh_led_demo_task();
#endif   // CONFIG_SLE_MESH_SUPPORT_LIGHT_CTRL
        osal_msleep(SLE_MESH_LED_CONTROL_DELAY);
    }
#elif CONFIG_SLE_MESH_DEV_MF_ROLE
        osal_printk("[Demo] this device run as Mesh MF.\r\n");
        osal_msleep(SLE_MESH_LED_CONTROL_DELAY);
#else
        osal_printk("Not Demo Version.\r\n");
#endif
    return 0;
}

void sle_mesh_sample_init(void)
{
    osal_printk("[Demo] demo start!!!\r\n");
    osal_kthread_lock();
    osal_task *task_handle1 = osal_kthread_create((osal_kthread_handler)sle_sample_init, 0, "sle_mesh", SLE_MESH_STACK_SIZE);
    if (task_handle1 != NULL) {
        osal_kthread_set_priority(task_handle1, SLE_INIT_THREAD_TASK_PRI);
        osal_kfree(task_handle1);
    }
    osal_kthread_unlock();
}

#ifdef CONFIG_SAMPLE_SUPPORT_SLE_MESH_ONE_TOUCH_START
app_run(sle_mesh_sample_init);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
