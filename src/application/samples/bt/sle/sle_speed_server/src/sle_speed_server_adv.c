/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: sle adv config for sle uuid server.
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_speed_server.h"
#include "sle_speed_server_adv.h"

/* sle device name */
#define NAME_MAX_LENGTH 15
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT                 0xA
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT                 0xA
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT              0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT              0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT      0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY                      0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER  20
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX                      251

/*              SLE ADV/ScanRsp Struct
 * | AdvA |        AdvData or ScanRspData       |
 *        |          N * AD Structure           |
 *        | AD Type(1) | Length(1) | AD Data(n) |
*/
#define SLE_ADV_DATA_TYPE_DISCOVERY_LEN     1
#define SLE_ADV_DATA_TYPE_ACCESS_LEN        1
#define SLE_ADV_DATA_TYPE_UUID_LEN          2

uint8_t g_sle_announce_data[] = {
    /* SLE ADV TYPE DISCOVERY LEVEL */
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,                          /* type:支持的设备发现等级 */
    SLE_ADV_DATA_TYPE_DISCOVERY_LEN,                            /* length:1 Byte */
    SLE_ANNOUNCE_LEVEL_NORMAL,                                  /* value: 一般可发现 */
    /* SLE ADV UUID TYPE */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS,     /* type:支持的UUID列表 */
    SLE_ADV_DATA_TYPE_UUID_LEN,                                 /* length:2 Byte */
    0x0B, 0x06,                                                 /* value:具体的UUID列表 */
};

#define SLE_ADV_DATA_TYPE_TX_POWER_LEN      1
#define SLE_ADV_DATA_LOCAL_NAME_LEN         16
uint8_t g_sle_scan_rsp_data[] = {
    /* SLE ADV TX POWER LEVEL */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,                           /* type:设置的广播功率等级 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEN,                             /* length:1 Byte */
    SLE_ADV_TX_POWER,
    /* SLE ADV LOCAL NAME */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME,                      /* type:设备广播名称 */
    SLE_ADV_DATA_LOCAL_NAME_LEN,                                /* length:15 Byte */
    's', 'l', 'e', '_', 's', 'p', 'e', 'e', 'd', '_', 's', 'e', 'r', 'v', 'e', 'r'
};

static int sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.announce_tx_power = SLE_ADV_TX_POWER;
    param.own_addr.type = 0;
    memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, mac, SLE_ADDR_LEN);
    return sle_set_announce_param(param.announce_handle, &param);
}

static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;

    osal_printk("set adv data default\r\n");
    data.announce_data = g_sle_announce_data;
    data.announce_data_len = sizeof(g_sle_announce_data);

    data.seek_rsp_data = g_sle_scan_rsp_data;
    data.seek_rsp_data_len = sizeof(g_sle_scan_rsp_data);

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("[SLE DD SDK] set announce data success.");
    } else {
        osal_printk("[SLE DD SDK] set adv param fail.");
    }
    return ERRCODE_SLE_SUCCESS;
}

void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("sle announce enable id:0x%02x, state:0x%02x\r\n", announce_id, status);
}

void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("sle announce disable id:0x%02x, state:0x%02x\r\n", announce_id, status);
}

void sle_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("sle announce terminal id:0x%02x\r\n", announce_id);
}

void sle_enable_cbk(errcode_t status)
{
    osal_printk("sle enable status:0x%02x\r\n", status);
    sle_enable_server_cbk();
}

void sle_announce_register_cbks(void)
{
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    seek_cbks.sle_enable_cb = sle_enable_cbk;
    sle_announce_seek_register_callbacks(&seek_cbks);
}

errcode_t sle_uuid_server_adv_init(void)
{
    osal_printk("sle_uuid_server_adv_init in\r\n");

    sle_set_default_announce_param();
    sle_set_default_announce_data();
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    osal_printk("sle_uuid_server_adv_init out\r\n");
    return ERRCODE_SLE_SUCCESS;
}
