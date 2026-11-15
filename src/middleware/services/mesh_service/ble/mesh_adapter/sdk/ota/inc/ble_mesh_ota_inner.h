/*
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: ble_mesh_ota.h ble mesh OTA 内部接口描述
 */

#ifndef BLE_MESH_OTA_INNER_H
#define BLE_MESH_OTA_INNER_H

#include <stdint.h>

/*******************************************************************************
  功能描述		:  用于检测当前是否允许重启
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  true可以重启，false不允许重启
*******************************************************************************/
bool ble_mesh_ota_reboot_check(void);

/*******************************************************************************
  功能描述    :  DFU 服务调用进行用户注册的metadata校验的处理
  输入参数    :  metadata 数据
                len 数据长度
  输出参数    :  None
  返 回 值    :  true metadata校验合法，false metadata校验非法
*******************************************************************************/
bool ble_mesh_ota_metadata_check(const uint8_t *metadata, uint8_t len);
#endif /* end of BLE_MESH_OTA_INNER_H */