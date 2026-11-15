 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: ble mesh prov inner.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __BLE_MESH_PROV_INNER_H__
#define __BLE_MESH_PROV_INNER_H__
#include "mesh/main.h"
#include "bts_def.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  功能描述		:  获取注册的provision信息
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  provision信息
*******************************************************************************/
struct bt_mesh_prov *ble_mesh_get_provision(void);

/*******************************************************************************
  功能描述		:  配网模块初始化
  输入参数		:  uuid 设备UUID
                  prov 设备配网信息
  输出参数		:  None
  返 回 值		:  None
*******************************************************************************/
errcode_t ble_mesh_prov_init(uint8_t uuid[BLE_MESH_UUID_LEN], const ble_mesh_prov_t *prov);

/*******************************************************************************
  功能描述		:  配网模块去初始化
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
*******************************************************************************/
errcode_t ble_mesh_prov_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MESH_PROV_INNER_H__ */
