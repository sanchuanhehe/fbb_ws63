/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH MODEL API.
 */

/**
 * @defgroup bluetooth_bts_mesh_model BLE MESH MODEL API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_MODEL_H__
#define BLE_MESH_MODEL_H__

#include <stdint.h>

#include "ble_mesh_errcode.h"
#include "ble_mesh_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Use this funtion to process the model information.
 * @par   Use this funtion to process the model information.
 * @attention NULL
 * @param  [in] comp  model infomation,
                      the comp content cannot be modified.
                      After the mesh protocol stack invokes this callback, the comp content is released.
 * @par Dependency:
 * @else
 * @brief 模型信息处理。
 * @par   模型信息处理。
 * @attention 无
 * @param  [in] comp  模型信息,注意不能修改comp内容，mesh协议栈调用完此回调后，会回收comp内容。
 * @par 依赖:
 * @endif
 */
typedef void (*ble_mesh_composition_callback)(ble_mesh_net_buff_t *comp);

/**
 * @if Eng
 * @brief Use this funtion to get the model information of a specified device.
 * @par   Use this funtion to get the model information of a specified device.
 * @attention NULL
 * @param  [in] net_info  mesh app key info.
 * @param  [in] comp      model infomation process func.
 * @param  [in] status    execution status.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取指定设备的模型信息。
 * @par    获取指定设备的模型信息。
 * @attention 无
 * @param  [in] net_info  mesh应用密钥信息。
 * @param  [in] comp      模型信息处理函数。
 * @param  [in] status    执行状态。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_mod_get_composition(const ble_mesh_prov_net_info_t *net_info,
                                       ble_mesh_composition_callback func, uint8_t *status);

/**
 * @if Eng
 * @brief Use this funtion to update the vid of the composition data.
 * @par   Use this funtion to update the vid of the composition data.
 * @attention NULL
 * @param  [in] vid  Product software version identifier.
 * @retval none
 * @par Dependency:
 * @else
 * @brief  此接口用于更新注册到composition中的版本信息。
 * @par    此接口用于更新注册到composition中的版本信息。
 * @attention 无
 * @param  [in] vid  产品软件版本。
 * @retval 无。
 * @par 依赖:
 * @endif
 */
void ble_mesh_mod_update_vid(uint16_t vid);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
