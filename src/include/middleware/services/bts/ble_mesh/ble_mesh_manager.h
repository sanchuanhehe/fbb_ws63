/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH MANAGER API.
 */

/**
 * @defgroup bluetooth_bts_mesh_manager BLE MESH MANAGER API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_MANAGER_H__
#define BLE_MESH_MANAGER_H__

#include <stdint.h>

#include "ble_mesh_errcode.h"
#include "ble_mesh_common.h"
#include "ble_mesh_prov.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Use this funtion to enable ble mesh stack.
 * @par   Use this funtion to enable ble mesh stack.
 * @attention NULL
 * @param  [in] provision provision flow process { @ref ble_mesh_prov_t }.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency: BLE Stack enable.
 * @else
 * @brief  用于使能MESH协议栈。
 * @par    用于使能MESH协议栈。
 * @attention 无
 * @param  [in] provision 配网流程信息 { @ref ble_mesh_prov_t }。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:BLE 协议栈使能。
 * @endif
 */
errcode_t ble_mesh_enable(uint8_t uuid[BLE_MESH_UUID_LEN], const ble_mesh_prov_t *prov, ble_mesh_prov_bearer_t bearers);

/**
 * @if Eng
 * @brief Use this funtion to disable ble mesh stack.
 * @par   Use this funtion to disable ble mesh stack.
 * @attention NULL
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency: BLE Stack enable.
 * @li  bts_def.h
 * @else
 * @brief  用于去使能MESH协议栈。
 * @par    用于去使能MESH协议栈。
 * @attention 无
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:BLE 协议栈使能。
 * @li  bts_def.h
 * @endif
 */
errcode_t ble_mesh_disable(void);

/**
 * @if Eng
 * @brief Use this funtion to check ble mesh stack status.
 * @par   Use this funtion to check ble mesh stack status.
 * @attention NULL
 * @retval true  The mesh protocol stack has been enabled.
 * @retval false  The mesh protocol stack is not enabled.
 * @par Dependency: BLE Stack enable.
 * @li  bts_def.h
 * @else
 * @brief  用于判断Mesh协议栈是否已经使能。
 * @par    用于判断Mesh协议栈是否已经使能。
 * @attention 无
 * @retval true  mesh协议栈已经使能。
 * @retval false mesh协议栈未使能。
 * @par 依赖:BLE 协议栈使能。
 * @li  bts_def.h
 * @endif
 */
bool ble_mesh_is_enable(void);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
