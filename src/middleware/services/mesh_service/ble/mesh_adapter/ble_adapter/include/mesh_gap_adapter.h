/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: mesh gap adapter
 *
 */
#ifndef MESH_GAP_ADAPTER_H__
#define MESH_GAP_ADAPTER_H__
#include <stdint.h>
#include "bts_def.h"
#include "bts_le_gap.h"

/* Advertising options */
enum {
    /* 未指定选项 */
    BLE_MESH_ADV_OPT_NONE = 0,

    /* 可连接广播 */
    BLE_MESH_ADV_OPT_CONNECTABLE = BIT(0),

    /* 如果设置，则当调用bt_le_adv_stop()或发生传入（从属）连接时，广播将停止。
     * 如果未设置，即使发生连接，也会保持广播.
     */
    BLE_MESH_ADV_OPT_ONE_TIME = BIT(1),
};

/* Scan phy type */
#define BLE_MESH_SCAN_PHY_1M 0x00
#define BLE_MESH_SCAN_PHY_2M 0x01
#define BLE_MESH_SCAN_PHY_LE_CODED 0x02

#define BLE_MESH_GAP_ADV_LEN_MAX 251
#define BLE_MESH_LEGACY_ADV_LEN_MAX 31
#define BLE_MESH_ADDR_LEN 6

typedef struct {
    uint8_t type;
    uint8_t val[BLE_MESH_ADDR_LEN];
} bt_mesh_addr_t;

/* bt_mesh_dev flags: the flags defined here represent BT controller state */
typedef void bt_mesh_scan_cb_t(const bt_mesh_addr_t *addr, int8_t rssi, uint8_t adv_type, struct os_mbuf *buf);

// mesh扫描回调函数
typedef errcode_t (*gap_ble_mesh_scan_result_callback)(gap_scan_result_data_t *scan_result_data);

// BLE mesh回调函数接口定义
typedef struct {
    gap_ble_mesh_scan_result_callback mesh_scan_func;       /*!< @if Eng Ble enable callback
                                                                @else   BLE启动回调函数 @endif */
} gap_ble_mesh_callback_t;

/*******************************************************************************
  功能描述		:  获取nimble注册的gap回调处理函数
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  gap回调处理函数
*******************************************************************************/
ble_gap_event_fn *ble_mesh_get_gap_event(void);

/*******************************************************************************
  功能描述		:  gap adapter去初始化
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  true 去初始化成功，false 去初始化失败
*******************************************************************************/
bool ble_mesh_gap_adapter_deinit(void);

/*******************************************************************************
  功能描述		:  gap adapter初始化
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  true 初始化成功，false初始化失败
*******************************************************************************/
bool ble_mesh_gap_adapter_init(void);

#endif    // MESH_GAP_ADAPTER_H__