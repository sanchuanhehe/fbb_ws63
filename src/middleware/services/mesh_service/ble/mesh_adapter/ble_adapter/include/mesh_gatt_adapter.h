/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: mesh gatt adapter
 *
 */
#ifndef MESH_GATT_ADAPTER_H__
#define MESH_GATT_ADAPTER_H__
#include <stdint.h>
#include "bts_def.h"

/*******************************************************************************
  功能描述		: mesh相关GATT服务去注册
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  int 0 success, other failed
*******************************************************************************/
int ble_mesh_gatts_unregister(void);

/*******************************************************************************
  功能描述		:  mesh gatt 服务注册
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  int 0 success, other failed
*******************************************************************************/
int ble_mesh_gatts_register(void);

/*******************************************************************************
  功能描述		:  gatt adapter 初始化
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  true 初始化成功，false初始化失败
*******************************************************************************/
bool ble_mesh_gatt_adapter_init(void);

/*******************************************************************************
  功能描述		:  gatt adapter去初始化函数
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  true 去初始化成功，false 去初始化失败
*******************************************************************************/
bool ble_mesh_gatt_adapter_deinit(void);
#endif    // MESH_GATT_ADAPTER_H__