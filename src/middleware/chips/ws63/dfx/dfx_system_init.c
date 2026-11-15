/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx system init
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_cmd_connect.h"
#include "diag_cmd_filter.h"
#include "diag_cmd_password.h"
#include "diag_cmd_beat_heart.h"
#include "diag_cmd_get_mem_info.h"
#include "diag_cmd_get_task_info.h"
#include "diag_cmd_mem_read_write.h"
#include "diag_mocked_shell.h"
#include "diag_bt_sample_data.h"
#include "osal_task.h"
#include "osal_msgqueue.h"
#include "diag_ind_src.h"
#include "diag_filter.h"
#include "diag_msg.h"
#if (defined(CONFIG_SLE_MESH_DFX) && (CONFIG_SLE_MESH_DFX == 1))
#include "sle_ssap_stru.h"
#include "sle_mesh_sdk_equip.h"
#include "sle_mesh_sdk_network.h"
#endif
#ifdef SUPPORT_DIAG_V2_PROTOCOL
#include "diag_service.h"
#include "diag_cmd_dispatch.h"
#endif /* SUPPORT_DIAG_V2_PROTOCOL */
#include "diag_rom_api.h"
#include "diag_channel.h"
#include "diag_adapt_layer.h"
#include "diag_adapt_sdt.h"
#include "soc_log_uart_instance.h"
#include "sample_data_adapt.h"
#include "dfx_channel.h"
#if (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES)
#include "transmit.h"
#endif
#include "diag_adapt_psd.h"
#include "diag_sample_data.h"
#include "log_common.h"
#include "diag_dfx_cmd_init.h"
#ifdef CONFIG_MIDDLEWARE_NV_SUPPORT_HSO_ACCESS
#include "nv_adapt_zdiag.h"
#endif
#ifdef CONFIG_AT_SUPPORT_ZDIAG
#include "at_zdiag.h"
#endif

#if (defined(CONFIG_SLE_MESH_DFX) && (CONFIG_SLE_MESH_DFX == 1))
#define DIAG_SLE_MESH_NODE_QUERY 0x1001 /* 查询网络拓扑节点信息 */
errcode_t diag_cmd_node_info_read(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
#endif

static diag_cmd_reg_obj_t g_diag_default_cmd_tbl[] = {
#if (defined(CONFIG_SLE_MESH_DFX) && (CONFIG_SLE_MESH_DFX == 1))
    { DIAG_SLE_MESH_NODE_QUERY, DIAG_SLE_MESH_NODE_QUERY, diag_cmd_node_info_read },
#endif
    { DIAG_CMD_CONNECT_RANDOM, DIAG_CMD_PWD_CHANGE, diag_cmd_password },
    { DIAG_CMD_HOST_CONNECT, DIAG_CMD_HOST_DISCONNECT, diag_cmd_hso_connect_disconnect },
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    { DIAG_CMD_MSG_RPT_AIR, DIAG_CMD_MSG_RPT_USR, diag_cmd_filter_set },
    { DIAG_CMD_MSG_CFG_SET_AIR, DIAG_CMD_MSG_CFG_SET_LEVEL, diag_cmd_filter_set },
#else
    { DIAG_CMD_MSG_CFG_SET_SYS, DIAG_CMD_MSG_CFG_SET_LEVEL, diag_cmd_filter_set },
#endif
#if (CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES)
    { DIAG_CMD_HEART_BEAT, DIAG_CMD_HEART_BEAT, diag_cmd_beat_heart },
#endif
    { DIAG_CMD_GET_TASK_INFO, DIAG_CMD_GET_TASK_INFO, diag_cmd_get_task_info},
    { DIAG_CMD_GET_MEM_INFO, DIAG_CMD_GET_MEM_INFO, diag_cmd_get_mem_info},
#if (CONFIG_DFX_MEMORY_OPERATE == DFX_YES)
    { DIAG_CMD_MEM_MEM32, DIAG_CMD_MEM_W4, diag_cmd_mem_operate },
#endif
#if (CONFIG_DFX_BT_SAMPLE_DATA == DFX_YES)
    { DIAG_CMD_ID_SAMPLE, DIAG_CMD_ID_SAMPLE, diag_cmd_sample_data},
#endif
    { DIAG_CMD_ID_PSD_ENABLE, DIAG_CMD_ID_PSD_ENABLE, diag_cmd_psd_enable},
    { DIAG_CMD_ID_BGLE_SAMPLE, DIAG_CMD_ID_WLAN_PHY_SAMPLE, diag_cmd_wlan_module_sample_data},
#ifdef CONFIG_MIDDLEWARE_NV_SUPPORT_HSO_ACCESS
    {DIAG_CMD_NV_QRY, DIAG_CMD_NV_QRY, zdiag_adapt_nv_read},
    {DIAG_CMD_NV_WR, DIAG_CMD_NV_WR, zdiag_adapt_nv_write},
#endif
#ifdef CONFIG_AT_SUPPORT_ZDIAG
    {DIAG_CMD_SIMULATE_AT, DIAG_CMD_SIMULATE_AT, zdiag_at_proc},
#endif
};

#define DFX_MSG_STACK_SIZE          0x800
#define TASK_PRIORITY_DFX_MSG       (osPriority_t)(5)
#define DATA_INFO_SIZE 40
unsigned long g_dfx_osal_queue_id;
unsigned long dfx_get_osal_queue_id(void)
{
    return g_dfx_osal_queue_id;
}
static errcode_t register_default_diag_cmd(void)
{
    return uapi_diag_register_cmd(g_diag_default_cmd_tbl,
        sizeof(g_diag_default_cmd_tbl) / sizeof(g_diag_default_cmd_tbl[0]));
}

int32_t msg_process_proc(uint32_t msg_id, uint8_t *data, uint32_t size)
{
    switch (msg_id) {
        case DFX_MSG_ID_DIAG_PKT:
            diag_msg_proc((uint16_t)msg_id, data, size);
            break;
        case DFX_MSG_ID_SDT_MSG:
            zdiag_adapt_sdt_msg_dispatch(msg_id, data, size);
            break;
        case DFX_MSG_ID_BEAT_HEART:
#if (CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES)
            diag_beat_heart_process();
#endif
            break;
#if defined(CONFIG_DFX_SUPPORT_TRANSMIT_FILE) && (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES)
        case DFX_MSG_ID_TRANSMIT_FILE:
            transmit_msg_proc(msg_id, data, size);
            break;
#endif
        default:
            break;
    }
    return ERRCODE_SUCC;
}

static void cmd_shell_proc(uint8_t *data, uint32_t data_len)
{
    diag_debug_cmd_proc(data, data_len);
    dfx_log_debug("cmd shell: %s", data);
}

static void dfx_set_log_leve(bool enable)
{
    /* 断开hso, 不上报日志 */
    if (enable == false) {
        log_set_local_log_level(LOG_LEVEL_NONE);
    }
}

static int msg_process_thread(void *data)
{
    uint8_t msg_data[DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN];
    uint32_t msg_data_size = (uint32_t)sizeof(msg_data);
    uint32_t msg_id;
    int32_t ret;

    unused(data);

    while (true) {
        (void)memset_s(msg_data, sizeof(msg_data), 0, sizeof(msg_data));

        ret = osal_msg_queue_read_copy(g_dfx_osal_queue_id, msg_data, &msg_data_size, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != ERRCODE_SUCC) {
            PRINT("osal_msg_queue_read_copy, err ret[%d]", ret);
            break;
        }

        msg_id = *((uint32_t*)&msg_data[0]);

        msg_process_proc(msg_id, &msg_data[DFX_MSG_ID_LEN], DFX_MSG_MAX_SIZE);
    }

    return ERRCODE_SUCC;
}

static errcode_t thread_msg_event_init(void)
{
    errcode_t ret;
    ret = (uint32_t)osal_msg_queue_create("dfx_msg", DFX_QUEUE_MAX_SIZE, &g_dfx_osal_queue_id,
        0, DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    osal_task *task = osal_kthread_create(msg_process_thread, NULL, "dfx_msg", DFX_MSG_STACK_SIZE);
    if (task == NULL) {
        return ERRCODE_FAIL;
    }
    osal_kthread_lock();
    if (osal_kthread_set_priority(task, TASK_PRIORITY_DFX_MSG) != OSAL_SUCCESS) {
        PRINT("osal_kthread_set_priority excute failed!!! \r\n");
    }
    osal_kthread_unlock();
    return ERRCODE_SUCC;
}

errcode_t dfx_system_init(void)
{
    errcode_t ret;
    diag_rom_api_t rom_api;

#ifdef SUPPORT_DIAG_V2_PROTOCOL
    uapi_diag_service_init();
#endif
    ret = register_default_diag_cmd();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    zdiag_filter_init();
    zdiag_filter_register_notify_hook(dfx_set_log_leve);
    dfx_set_log_leve(false);

#if !defined(FORBIT_AUTO_LOG_REPORT) && !defined(CONFIG_DIAG_BEAT_HEART_SUPPORT)
    diag_auto_log_report_enable();
#else
#if CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES
    ret = diag_beat_heart_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
#endif

    ret = thread_msg_event_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = diag_register_channel();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL == DFX_YES)
    zdiag_mocked_shell_init();
    zdiag_mocked_shell_register_cmd_data_proc(cmd_shell_proc);
#endif /* CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL */

    rom_api.report_sys_msg = uapi_zdiag_report_sys_msg_instance;
    diag_rom_api_register(&rom_api);

#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
    uapi_transmit_init();
#endif

    ret = diag_dfx_cmd_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

#if (defined(CONFIG_SLE_MESH_DFX) && (CONFIG_SLE_MESH_DFX == 1))
#define DFX_NODE_DATA_MAGIC_NUM     0xBEE0 // 网络节点数据魔数字，节点数据开始标记位
#define DFX_NODE_DATA_VERSION       0x0000 // 网络节点数据版本号，上报数据结构变更时修改版本号

static uint16_t g_mesh_cmd_id = 0;
static uint16_t g_report_node_num = 0;
static uint16_t g_sn = 0;
static diag_option_t g_mesh_option = { 0 };

typedef enum {
    DFX_DATA_CONTINUE = 0,                  /* 单次查询数据未结束，继续上报 */
    DFX_DATA_END = 1,                       /* 单次查询数据结束 */
} dfx_data_flag_t;

#pragma pack(1)
typedef struct {
    uint16_t mesh_id;                       /*!< 网络地址。 */
    uint16_t net_id;                        /*!< 网络ID。 */
    sle_uuid_t uuid;                        /*!< 设备外观UUID */
    uint8_t addr[SLE_ADDR_LEN];             /*!< 设备MAC地址。 */
    uint8_t role;                           /*!< 设备网络角色 { @ref sle_mesh_role_type_t }。 */
    uint8_t state;                          /* 网络节点状态 { @ref sle_mesh_node_state_t }。 */
    uint8_t connect_cap;                    /* 节点连接能力位图 { @ref sle_mesh_connect_capability_t } */
    uint32_t node_cap;                      /* 节点能力位图 { @ref sle_mesh_node_capability_t } */
    uint8_t name[SLE_MESH_DEVICE_NAME_LEN]; /* 网络设备名称。 */
    uint8_t address_assign;                 /* 地址分配方式 { @ref sle_mesh_address_assign_t }。 */
} dfx_node_data_t;

typedef struct {
    uint8_t neighbor_count;                 /* 邻居数量。 */
    uint16_t neighbor_list[0];              /* 邻居网络地址列表（动态数组，拼接时申请Buffer）。 */
    uint8_t group_count;                    /* 组信息数量。 */
    uint16_t group_list[0];                 /* 组信息列表（动态数组，拼接时申请Buffer）。 */
} dfx_node_variable_lendth_data_t;

typedef struct {
    uint16_t dfx_magic_num;                 /* 节点信息魔术字，标识节点信息开始 */
    dfx_node_data_t node_data;              /* 节点信息 */
    dfx_node_variable_lendth_data_t node_variable_data; /* 节点可变数据信息 */
} dfx_node_info_t;

typedef struct dfx_query_node_data {
    uint16_t version;                       /* 数据上报版本号标识，首版本填0，后续变更字段时递增版本号。 */
    uint16_t sn;                            /* 单轮数据需分批次上报，sn用于标识分批次数据所属查询轮次。 */
    uint16_t is_last;                       /* 单轮数据上报是否结束 { @ref dfx_data_flag_t }。 */
    uint16_t num;                           /* 本数据包中包含的有效站点数。 */
    uint8_t nodes[0];                       /* 包含num个dfx_node_info_t，可变数据长度。 */
} dfx_query_node_data_t;
#pragma pack()

void diag_get_node_data(uint16_t device_num, sle_mesh_node_info_t *node_info, uint8_t *node_report)
{
    uint16_t total_num = 0;
    if ((sle_mesh_get_node_num(&total_num) != ERRCODE_SLE_MESH_SUCCESS) ||
        (total_num < (g_report_node_num + device_num))) {
        PRINT("node num error,total:%u,report num:%u,dev num:%u\n", total_num, g_report_node_num, device_num);
        return;
    }

    dfx_query_node_data_t *query_node_data = (dfx_query_node_data_t *)node_report;
    query_node_data->num = device_num;
    uint8_t *node_data = node_report + sizeof(dfx_query_node_data_t); // dfx_node_info_t
    for (uint16_t i = 0; i < device_num; i++) {
        // 单个节点内存大小
        uint16_t node_size = sizeof(dfx_node_info_t) +
            node_info[i].neighbor_count * sizeof(uint16_t) + node_info[i].group_count * sizeof(uint16_t);
        dfx_node_info_t *dfx_node_info = (dfx_node_info_t *)node_data;
        // 魔数
        dfx_node_info->dfx_magic_num = DFX_NODE_DATA_MAGIC_NUM;
        // 定长数据
        dfx_node_info->node_data.mesh_id = node_info[i].mesh_id;
        dfx_node_info->node_data.net_id = node_info[i].net_id;
        dfx_node_info->node_data.uuid = node_info[i].uuid;
        (void)memcpy_s(dfx_node_info->node_data.addr, SLE_ADDR_LEN, node_info[i].addr, SLE_ADDR_LEN);
        dfx_node_info->node_data.role = node_info[i].role;
        dfx_node_info->node_data.state = node_info[i].state;
        dfx_node_info->node_data.connect_cap = node_info[i].connect_cap;
        dfx_node_info->node_data.node_cap = node_info[i].node_cap;
        (void)memcpy_s(dfx_node_info->node_data.name, SLE_MESH_DEVICE_NAME_LEN,
            node_info[i].name, SLE_MESH_DEVICE_NAME_LEN);
        dfx_node_info->node_data.address_assign = node_info[i].address_assign;

        /* 邻居节点信息拼接，可变数据长度 */
        uint8_t *neighbor_num = (uint8_t *)&dfx_node_info->node_variable_data;
        (void)memcpy_s(neighbor_num, sizeof(uint8_t), &node_info[i].neighbor_count, sizeof(uint8_t));
        if (*neighbor_num != 0) {
            uint8_t *neighbor_list = neighbor_num + sizeof(uint8_t);
            (void)memcpy_s(neighbor_list, node_info[i].neighbor_count * sizeof(uint16_t),
                node_info[i].neighbor_list, node_info[i].neighbor_count * sizeof(uint16_t));
        }

        /* 组信息拼接，可变数据长度 */
        uint8_t *group_num = neighbor_num + sizeof(uint8_t) + *neighbor_num * sizeof(uint16_t);
        (void)memcpy_s(group_num, sizeof(uint8_t), &node_info[i].group_count, sizeof(uint8_t));
        if (*group_num != 0) {
            uint8_t *group_list = group_num + sizeof(uint8_t);
            (void)memcpy_s(group_list, node_info[i].group_count * sizeof(uint16_t),
                node_info[i].group_list, node_info[i].group_count * sizeof(uint16_t));
        }

        node_data += node_size;
    }
    g_report_node_num += device_num;
    if (g_report_node_num == total_num) {
        query_node_data->is_last = DFX_DATA_END;
        g_report_node_num = 0;
        g_sn++;
    }
}

void diag_cmd_report_node_info(uint16_t device_num, sle_mesh_node_info_t *node_info)
{
    uint16_t info_size = sizeof(dfx_query_node_data_t) + device_num * sizeof(dfx_node_info_t);
    for (uint8_t i = 0; i < device_num; i++) {
        info_size += node_info[i].neighbor_count * sizeof(uint16_t) + node_info[i].group_count * sizeof(uint16_t);
    }
    PRINT("info_size %d, device_num %d\r\n", info_size, device_num);
    uint8_t *node_report = dfx_malloc(0, info_size);
    if (node_report == NULL) {
        PRINT("node info report fail,alloc fail\r\n");
        return;
    }
    dfx_query_node_data_t *query_node_data = (dfx_query_node_data_t *)node_report;
    query_node_data->version = DFX_NODE_DATA_VERSION;
    query_node_data->sn = g_sn;
    query_node_data->is_last = DFX_DATA_CONTINUE;

    diag_get_node_data(device_num, node_info, node_report);

    errcode_t ret = uapi_diag_report_packet(g_mesh_cmd_id, &g_mesh_option, (uint8_t *)node_report, info_size, true);
    if (ret != EOK) {
        PRINT("node info report fail, ret=[0x%x]\r\n", ret);
    }
    dfx_free(0, node_report);
}

errcode_t diag_cmd_node_info_read(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    PRINT("diag_cmd_node_info_read enter \r\n");
    unused(cmd_param);
    unused(cmd_param_size);

    g_mesh_cmd_id = cmd_id;
    (void)memcpy_s(&g_mesh_option, sizeof(diag_option_t), option, sizeof(diag_option_t));

    /* 注册回调 */
    sle_mesh_network_callback_t net_cbk = {
        .query_cbk = diag_cmd_report_node_info,
    };

    errcode_t ret = sle_mesh_network_register_callback(&net_cbk);
    if (ret != ERRCODE_SLE_MESH_SUCCESS) {
        PRINT("[Error]:dfx register network callback failed.");
        return ERRCODE_FAIL;
    }

    /* 触发查询节点信息 */
    ret = sle_mesh_get_all_node_info();
    if (ret != ERRCODE_SLE_MESH_SUCCESS) {
        PRINT("[Error]:dfx query node info fail.");
        return ERRCODE_FAIL;
    }

    return ERRCODE_FAIL;
}
#endif