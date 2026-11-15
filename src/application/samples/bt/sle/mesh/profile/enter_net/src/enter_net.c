/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Create Date : 2026.05
 * Description: joiner.c 星闪Mesh 自动化入网Demo
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
#include "sle_mesh_sdk_security.h"
#include "sle_device_discovery.h"
#include "sle_mesh_sdk_ota.h"
#include "sle_c_frame.h"
#include "enter_net.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID SLE_ENTER_NET_JOINER_SAMPLE

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 网络层数 */
#define SLE_MESH_NET_LAYER 2

/* 线程调度优先级 */
#define SLE_MESH_INIT_DELAY 100

/* 实例类型(控制=0，受控=1) */
#define INSTANCE_TYPE 0
/* 实例描述字符串最大长度 */
#define DESCRIPTION_STR_MAX_LEN     50
/* Sample自定义灯服务UUID */
#define SUBSCRUBE_UUID 0x0609
/* 开关开关UUID */
#define LIGHT_PROPERTY_SWITCH_UUID 0x301
/* 开关属性最小值-关闭 */
#define LIGHT_PROPERTY_SWITCH_OFF 0
/* 开关属性最大值-打开 */
#define LIGHT_PROPERTY_SWITCH_ON  1
/* UUID第一位值 */
#define GLE_MESH_UUID_FIRST_PLACE  14
/* UUID第二位值 */
#define GLE_MESH_UUID_SECOND_PLACE 15

/* 灯控服务信息属性结构 */
typedef struct {
    uint16_t property_uuid; /* 灯控服务信息属性UUID */
    uint16_t value_type;    /* 灯控服务信息属性类型, 参考sle_mesh_property_value_type_t定义 */
    uint32_t value_min;     /* 灯控服务信息属性定义最小值 */
    uint32_t value_max;     /* 灯控服务信息属性定义最大值 */
} light_property_distribute_t;

/* 灯控服务信息，可根据需要扩招 */
static light_property_distribute_t g_light_property_distribute[] = {
    {LIGHT_PROPERTY_SWITCH_UUID, SLE_MESH_PROPERTY_VALUE_TYPE_ARRAY, LIGHT_PROPERTY_SWITCH_OFF,
        LIGHT_PROPERTY_SWITCH_ON},
};

/* 灯控服务信息注册句柄 */
static sle_mesh_service_instance_t *g_instance_info = NULL;
static timer_handle_t g_hard_timer = 0;                 /* 硬件定时器句柄 */

#ifdef CONFIG_SLE_MESH_DEV_MA_ROLE
#define LED_STATUS_ADD_STEP 1
#define LED_STATUS_DIVIDEND 2
/* MA设备名称 */
static uint8_t g_mesh_dev_name[] = "MA";
/* MA设备地址 */
static sle_addr_t g_mesh_dev_addr = {0, {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}};
/* 将节点角色配置为MA，参考sle_mesh_role_type_t中的定义 */
static sle_mesh_role_type_t g_mesh_role = SLE_MESH_ROLE_TYPE_MA;
demo_ctrl_data_t g_demo_cmd = { 0 };                 /* AT指令数据缓存 */

#elif CONFIG_SLE_MESH_DEV_MF_ROLE
/* MF设备名称 */
static uint8_t g_mesh_dev_name[] = "MF";
/* MF设备地址 */
static sle_addr_t g_mesh_dev_addr = {0, {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}};
/* 将节点角色配置为MF，参考sle_mesh_role_type_t中的定义 */
static sle_mesh_role_type_t g_mesh_role = SLE_MESH_ROLE_TYPE_MF;
#endif

/* 网络标识，统一网络的MA和MF必须配置为相同的NetId */
#define SLE_MESH_NETID 2
#define SLE_MESH_UUID_HIGH_BIT 8

/* 申请的节点实例ID */
static uint8_t g_instance = 0;

uint32_t sample_adapt_get_sys_time(void)
{
    uint32_t sys_time = 0;
    errcode_t ret = sle_c_frame_get_sys_time(&sys_time);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sample_adapt_get_sys_time failed ret 0x%x ", ret);
        return 0;
    }
    return sys_time;
}

/*********** led ctrl demo start *****************/
#if defined(CONFIG_SLE_MESH_DEV_MA_ROLE) && defined(CONFIG_SLE_MESH_SUPPORT_LIGHT_CTRL)
static uint8_t g_onoff_count = 0;
/* 服务组播ID */
#define SLE_MESH_SERVICE_MULTI_GROUP_ID     0xFFFF

/* MA发送控制数据 */
void nm_enroll_transform_uuid(uint16_t uuid_value, sle_uuid_t *uuid);
void sle_mesh_sample_service_send_data(demo_ctrl_data_t *demo_data)
{
    sle_mesh_property_t property = { 0 };
    nm_enroll_transform_uuid(LIGHT_PROPERTY_SWITCH_UUID, &property.property_uuid);
    property.value_len = sizeof(demo_ctrl_data_t);
    property.value_type = SLE_MESH_PROPERTY_VALUE_TYPE_ARRAY;
    property.value = (uint8_t *)demo_data;
    sle_mesh_property_change_multicast(SLE_MESH_SERVICE_MULTI_GROUP_ID, &property);
}

void sle_mesh_led_demo_task(void)
{
    demo_ctrl_data_t *demo_cmd = &g_demo_cmd;

    /* 单一灯控模式，默认slot=5000 */
    demo_cmd->exec_time = SAMPLE_SYNC_CLOCK_DEFAULT_SLOT;

    demo_ctrl_data_t demo_data = { 0 };
    demo_data.exec_time = MESH_SCENE_CLK_125_ADD(sample_adapt_get_sys_time(),
        demo_cmd->exec_time * MESH_SAMPLE_SLOT_TO_MS);             /* 计算MA时间基线，单位：ms */
    g_onoff_count = g_onoff_count == 0 ? 1 : 0;
    demo_data.onoff_status = g_onoff_count;
    sle_mesh_sample_service_send_data(&demo_data);
}
#endif

/***********  led ctrl demo end ******************/

static inline void transform_uuid_to_struct(uint16_t uuid_value, sle_uuid_t *uuid)
{
    uuid->len = sizeof(uint16_t);
    uuid->uuid[GLE_MESH_UUID_SECOND_PLACE] = (uint8_t)uuid_value;
    uuid->uuid[GLE_MESH_UUID_FIRST_PLACE] = (uint8_t)(uuid_value >> SLE_MESH_UUID_HIGH_BIT);
}

static void sle_mesh_del_property_list_value(uint8_t num)
{
    for (uint8_t i = 0; i < num; ++i) {
        if (g_instance_info->property_list[i].value != NULL) {
            // 已申请的value资源
            osal_vfree(g_instance_info->property_list[i].value);
        }
    }
}

/* 申请实例结果回调，开始配置实例属性 */
static void group_register_instance_cmd_cbk(uint8_t instance, uint8_t result)
{
    osal_printk("[Demo] register service, instance:%d, result:%u.\r\n",
        instance, result);
    if (g_instance_info) {
        uint8_t num = sizeof(g_light_property_distribute) / sizeof(g_light_property_distribute[0]);
        // 调用删除接口，删除已申请的value资源
        sle_mesh_del_property_list_value(num);
        osal_vfree(g_instance_info);
        g_instance_info = NULL;
    }

    if (result == SLE_MESH_SERVICE_SUCCESS) {
        g_instance = instance;
    }
    sle_mesh_net_param_t net_param = {0};
    // 如果获取参数成功，参数从nv中恢复成功，直接用恢复的参数启动，如果失败，使用设置的参数启动
    errcode_t ret = sle_mesh_get_net_param(&net_param);
    if (ret != ERRCODE_SLE_MESH_SUCCESS) {
#if defined (CONFIG_SLE_MESH_DEV_MA_ROLE) || defined (CONFIG_SLE_MESH_DEV_MF_ROLE)
        osal_printk("[Demo] get net param fail, start from setting param ret 0x%x\r\n", ret);
        net_param.role = g_mesh_role;
        (void)memcpy_s(net_param.name, sizeof(net_param.name), g_mesh_dev_name, strlen((char *)g_mesh_dev_name));
        net_param.net_id = SLE_MESH_NETID;
        net_param.net_max_hop = SLE_MESH_NET_LAYER;
        net_param.net_access_policy = SLE_MESH_NET_CENTRALIZED_CONTROL_ACCESS_POLICY; // 只支持集中控制入网
        net_param.net_topo_type = SLE_MESH_NET_TREE_TOPOLOGY_TYPE; // 只支持树形网络topo
        // 判断网络算法和节点的支持算法不一直就返回不支持
        net_param.net_auth_algo = SLE_MESH_AUTH_ALGO_AES_GCM;
        net_param.net_complete_algo = SLE_MESH_COMPLETE_PROTECT_ALGO_AES_GCM;
        // 网络只支持 同步广播
        net_param.net_con_mode = SLE_MESH_SYNCHRONOUS_BROADCAST_MODE;
        ret = sle_mesh_start(&net_param);
        if (ret != ERRCODE_SLE_MESH_SUCCESS) {
            osal_printk("[Demo] sle_mesh_start failed! ret = 0x%x.\r\n", ret);
            return;
        }
#endif
        return;
    } else {
        osal_printk("[Demo] get net param success, start from nv param\r\n");
        ret = sle_mesh_start(&net_param);
        if (ret != ERRCODE_SLE_MESH_SUCCESS) {
            osal_printk("[Demo] sle_mesh_start failed! ret = 0x%x.\r\n", ret);
            return;
        }
    }
    
    return;
}

void sample_hardware_timer_init(void)
{
    osal_printk("[Demo]:sample_hardware_timer_init");
    errcode_t ret = uapi_timer_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("[Demo]:hardware timer init ret:0x%x", ret);
    }

    ret = uapi_timer_adapter(UH_HW_TIMER_INDEX, UH_HW_TIMER_IRQN, TIMER_PRIO);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[Demo]:hardware timer adapter ret:0x%x", ret);
        return;
    }

    ret = uapi_timer_create(UH_HW_TIMER_INDEX, &g_hard_timer);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[Demo]:hardware timer create ret:0x%x", ret);
        return;
    }

    osal_printk("[Demo]:hardware timer init success");
}

uint16_t transform_uuid_to_uint(sle_uuid_t uuid)
{
    uint16_t uuid_value = 0;
    *((uint8_t *)&uuid_value + 1) = uuid.uuid[GLE_MESH_UUID_FIRST_PLACE];
    *((uint8_t *)&uuid_value) = uuid.uuid[GLE_MESH_UUID_SECOND_PLACE];
    return uuid_value;
}

static void sample_led_mode_all_on_off(uintptr_t onoff_status)
{
    if (onoff_status == 1) {
        sample_light_switch(true);
    } else {
        sample_light_switch(false);
    }
}

/* 时钟同步执行入口 */
void sample_demo_sync_clock_exec(demo_ctrl_data_t *ctrl_data)
{
    uint32_t cur_time = sample_adapt_get_sys_time();
    int net_time_gap = MESH_SCENE_CLK_125_DIFF(cur_time, ctrl_data->exec_time);
    uint32_t timer_time = net_time_gap * MESH_SCENE_SLE_SLOT_WIN;
    if (ctrl_data->exec_time == 0) {
        timer_time = 0;
    }
    if (timer_time <= SAMPLE_TIMER_IGNORED_TIME_US) {
        sample_led_mode_all_on_off(ctrl_data->onoff_status);
        return;
    }
    /* 等待时钟同步完成 */
    errcode_t ret = uapi_timer_start(g_hard_timer, timer_time, sample_led_mode_all_on_off, ctrl_data->onoff_status);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[Demo]:start hardware sync clock timer fail:%#x,us:%u.", ret, timer_time);
    }
}

/* 执行节点处理入口 */
void sample_light_switch_proc(uint8_t *data, uint16_t data_len)
{
    if (data_len == sizeof(demo_ctrl_data_t)) {
        uapi_timer_stop(g_hard_timer);
        sample_demo_sync_clock_exec((demo_ctrl_data_t *)data);
    }
}

/* 申请实例时下发的应用回调，当实例属性值更改时被调用 */
void group_instance_app_cbk(sle_uuid_t uuid, uint8_t instance, sle_mesh_property_t *property, uint16_t sequence_num)
{
    osal_printk("======  group_instance_app_cbk  ============");
    unused(instance);
    unused(sequence_num);

    if (uuid.len != sizeof(uint16_t) || property->property_uuid.len != sizeof(uint16_t)) {
        osal_printk("[Demo] instance app cbk error, uuid length is invalid");
        return;
    }
    uint16_t property_uuid_value = transform_uuid_to_uint(property->property_uuid);
    switch (property_uuid_value) {
        case LIGHT_PROPERTY_SWITCH_UUID: {
            uint32_t ucrr_time = sample_adapt_get_sys_time();
            demo_ctrl_data_t *value = (demo_ctrl_data_t *)property->value;
            osal_printk("[SERVICE][SEQ %05d] ucrr_time(%u) instance(0x%02x) property(%04x) change to %u\n",
                sequence_num, ucrr_time, instance, property_uuid_value, value->onoff_status);
            sample_light_switch_proc(property->value, property->value_len);
            break;
        }
        default: {
            return;
        }
    }
    return;
}

static errcode_t construct_instance_param(uint8_t num)
{
    g_instance_info->ctrl_type = INSTANCE_TYPE;
    const char *description = "this is a device";
    g_instance_info->uuid.len = sizeof(uint16_t);
    transform_uuid_to_struct(SUBSCRUBE_UUID, &g_instance_info->uuid);

    uint8_t description_len = strlen(description) + 1;
    if (description_len > DESCRIPTION_STR_MAX_LEN) {
        description_len = DESCRIPTION_STR_MAX_LEN;
    }
    (void)memcpy_s(g_instance_info->description, DESCRIPTION_STR_MAX_LEN, description, description_len);
    g_instance_info->ckb_fun.sub_cbk = group_instance_app_cbk;
    g_instance_info->ckb_fun.result_cbk = group_register_instance_cmd_cbk;
    g_instance_info->property_num = num;
    g_instance_info->property_list =
        (sle_mesh_property_t *)((uint8_t *)g_instance_info + sizeof(sle_mesh_service_instance_t));
    for (uint8_t i = 0; i < num; ++i) {
        transform_uuid_to_struct(g_light_property_distribute[i].property_uuid,
            &g_instance_info->property_list[i].property_uuid);
        g_instance_info->property_list[i].value_type = g_light_property_distribute[i].value_type;
        g_instance_info->property_list[i].value_len = sizeof(uint32_t);
        g_instance_info->property_list[i].value = (uint8_t *)osal_vmalloc(sizeof(uint32_t));
        if (g_instance_info->property_list[i].value == NULL) {
            // 调用删除接口，删除已申请的value资源
            sle_mesh_del_property_list_value(num);
            osal_vfree(g_instance_info);
            g_instance_info = NULL;
            return ERRCODE_SLE_MESH_FAIL;
        }
        (void)memcpy_s(g_instance_info->property_list[i].value, sizeof(uint32_t),
            &(g_light_property_distribute[i].value_min), sizeof(uint32_t));
        g_instance_info->property_list[i].value_min = g_light_property_distribute[i].value_min;
        g_instance_info->property_list[i].value_max = g_light_property_distribute[i].value_max;
    }
    return ERRCODE_SLE_MESH_SUCCESS;
}

static errcode_t sle_mesh_init_service_instance(void)
{
    uint8_t num = sizeof(g_light_property_distribute) / sizeof(g_light_property_distribute[0]);
    uint32_t instance_len = sizeof(sle_mesh_service_instance_t) + (sizeof(sle_mesh_property_t) * num);
    g_instance_info = (sle_mesh_service_instance_t *)osal_vmalloc(instance_len);
    if (g_instance_info == NULL) {
        osal_printk("[Demo] register service instance malloc failed!\r\n");
        return ERRCODE_SLE_MESH_FAIL;
    }

    (void)memset_s(g_instance_info, instance_len, 0, instance_len);
    if (construct_instance_param(num) != ERRCODE_SLE_MESH_SUCCESS) {
        return ERRCODE_SLE_MESH_FAIL;
    }

    if (sle_mesh_register_service_instance(g_instance_info) != ERRCODE_SLE_MESH_SUCCESS) {
        osal_printk("[Demo] register service instance failed!\r\n");
        sle_mesh_del_property_list_value(num);
        osal_vfree(g_instance_info);
        g_instance_info = NULL;
        return ERRCODE_SLE_MESH_FAIL;
    }

    return ERRCODE_SLE_MESH_SUCCESS;
}

#ifdef CONFIG_SLE_MESH_DEV_MA_ROLE
void device_enter_net_cbk(uint16_t mesh_id, uint8_t result)
{
    osal_printk("[Demo] Enter net %s\r\n", (result == SLE_MESH_ENROLL_SUCCESS) ? "success" : "failed !!!.");
    osal_printk("[Demo] Enter net remote device mesh id 0x%x\r\n", mesh_id);

#ifdef CONFIG_SLE_MESH_SUPPORT_OTA
    osal_printk("[Demo] Enter ota\r\n");
    /* OTA升级是在节点入网后触发的，所以若是多个板子验证，可通过计数 + 定时器逻辑确定OTA触发时机 */

    /* 设置升级包大小 */
    sle_mesh_image_t img_info = {
        .img_size = 1056336,
    };
    sle_mesh_ota_init_image_info(&img_info);

    /* 获取网内待升级节点信息 */
    sle_mesh_ota_get_all_firmware_info();
#endif
}
#endif

static void sle_mesh_access_cbk(uint8_t mac_addr[SLE_ADDR_LEN], uint16_t mesh_id,
    sle_mesh_node_state_t node_state, sle_mesh_access_reason_t access_reason)
{
    osal_printk("[SAMPLE]node %02x:**:**:**:%02x:%02x mesh_id 0x%x node_state %u access_reason 0x%x \r\n",
        mac_addr[SAMPLE_ADDR_IND_0], mac_addr[SAMPLE_ADDR_IND_4], mac_addr[SAMPLE_ADDR_IND_5],
        mesh_id, node_state, access_reason);
    
#ifdef CONFIG_SLE_MESH_DEV_MA_ROLE
    device_enter_net_cbk(mesh_id, node_state);
#endif
}

/* 星闪协议栈初始化结果回调 */
void sle_stack_enable_callback(errcode_t status)
{
    if (status != 0) {
        osal_printk("[Demo] sle enable fail:%u.\r\n", status);
        return;
    }
    // 进行sle mesh net cbk注册
    sle_mesh_network_callback_t sle_mesh_net_cbk = {
        .access_cbk = sle_mesh_access_cbk,
    };
    sle_mesh_network_register_callback(&sle_mesh_net_cbk);
    /* 初始化mesh */
    sle_mesh_init_param_t init_param = {
        .node_cap = SLE_MESH_NODE_CAP_ADDR_ASSIGN | SLE_MESH_NODE_CAP_ADDR_ASSIGN_AGENT |
            SLE_MESH_NODE_CAP_MESSAGE_CACHE | SLE_MESH_NODE_CAP_MESSAGE_FORWARDER |
            SLE_MESH_NODE_CAP_ROLE_TRANSITION,
        .addr_type = SLE_MESH_ADDR_TYPE_PROXY,
        .route_cap = SLE_MESH_ROUTE_CAP_PRIVATE_RIP,
        .auth_algo = SLE_MESH_AUTH_ALGO_AES_GCM,
        .complete_algo = SLE_MESH_COMPLETE_PROTECT_ALGO_AES_GCM,
        .key_nego_algo = SLE_MESH_KEY_NEGO_ALGO_ECDH,
        .key_derivate_algo = SLE_MESH_KEY_DERIVATE_ALGO_AES_CMAC,
        .node_con_mode = SLE_MESH_SYNCHRONOUS_BROADCAST_MODE,
    };

#if defined (CONFIG_SLE_MESH_DEV_MA_ROLE) || defined (CONFIG_SLE_MESH_DEV_MF_ROLE)
    sle_set_local_addr(&g_mesh_dev_addr);
    osal_msleep(SLE_MESH_INIT_DELAY);
#endif

    errcode_t ret = sle_mesh_init(&init_param);
    if (ret != ERRCODE_SLE_MESH_SUCCESS) {
        osal_printk("[Demo] sle mesh init fail:0x%x\r\n.", ret);
        return;
    }

    osal_msleep(SLE_MESH_INIT_DELAY);

    ret = sle_mesh_init_service_instance();
    if (ret != ERRCODE_SLE_MESH_SUCCESS) {
        osal_printk("[Demo] sle mesh service init fail:0x%x\r\n.", ret);
    }
}

int sle_enter_net_sample_init(void)
{
    /* 注册星闪协议栈回调 */
    sle_announce_seek_callbacks_t stack_cbk = {
        .sle_enable_cb = sle_stack_enable_callback,
    };
    sle_announce_seek_register_callbacks(&stack_cbk);

    /* 使能星闪 */
    enable_sle();
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
