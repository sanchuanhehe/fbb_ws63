/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH SECURITY API.
 */

/**
 * @defgroup bluetooth_bts_mesh_security BLE MESH SECURITY API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_SM_H
#define BLE_MESH_SM_H

#include <stdint.h>

#include "ble_mesh_errcode.h"
#include "ble_mesh_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Use this funtion to add app key to mesh device.
 * @par   Use this funtion to add app key to mesh device.
 * @attention NULL
 * @param  [in] net_info  mesh app key info.
 * @param  [in] status    execution status.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  给mesh设备添加应用密钥。
 * @par    给mesh设备添加应用密钥
 * @attention 无
 * @param  [in] net_info  mesh应用密钥信息。
 * @param  [in] status    执行状态。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_cfg_add_app_key(const ble_mesh_prov_net_info_t *net_info, uint8_t *status);

/**
 * @if Eng
 * @brief Use this funtion to bind app key to mesh model.
 * @par   Use this funtion to bind app key to mesh model.
 * @attention NULL
 * @param  [in] net_info  mesh app key info.
 * @param  [in] mod_id    mesh model id.
 * @param  [in] status    execution status.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  给mesh设备指定标准模型绑定应用密钥。
 * @par    给mesh设备指定标准模型绑定应用密钥。
 * @attention 无
 * @param  [in] net_info  mesh应用密钥信息。
 * @param  [in] mod_id    mesh模型ID。
 * @param  [in] status    执行状态。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_cfg_mod_bind_app_key(const ble_mesh_prov_net_info_t *net_info, uint16_t mod_id, uint8_t *status);

/**
 * @if Eng
 * @brief Use this funtion to bind app key to vnd mesh model.
 * @par   Use this funtion to bind app key to vnd mesh model.
 * @attention NULL
 * @param  [in] net_info  mesh app key info.
 * @param  [in] mod_id    mesh model id.
 * @param  [in] mod_id    mesh company id.
 * @param  [in] status    execution status.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  给mesh设备指定vnd模型绑定应用密钥。
 * @par    给mesh设备指定vnd模型绑定应用密钥。
 * @attention 无
 * @param  [in] net_info  mesh应用密钥信息。
 * @param  [in] mod_id    mesh模型ID。
 * @param  [in] mod_id   mesh厂商ID。
 * @param  [in] status    执行状态。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_cfg_vnd_mod_bind_app_key(const ble_mesh_prov_net_info_t *net_info, uint16_t mod_id,
                                            uint16_t cid, uint8_t *status);


/**
 * @if Eng
 * @brief Use this interface to add the device local app key.
 * @par Use this interface to add the device local app key.
 * @attention None.
 * @param [in] app_idx         Application Key Index.
 * @param [in] net_idx         Network Key Index.
 * @param [in] app_key         Application Key.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口设置设备的本地网络信息。
 * @par    使用此接口设置设备的本地网络信息。
 * @attention 无。
 * @param [in] app_idx         应用密钥索引.
 * @param [in] net_idx         网络密钥索引.
 * @param [in] app_key         应用密钥.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_cfg_add_local_app_key(uint16_t app_idx, uint16_t net_idx,
                                         const uint8_t app_key[BLE_MESH_APP_KEY_LEN]);

/**
 * @if Eng
 * @brief Use this interface to update the device key.
 * @par Use this interface to update the device key.
 * @attention None.
 * @param [in] dev_key        Device Key.
 * @param [in] active         true: Immediately Effective; false: Not Effective.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口更新设备密钥。
 * @par    调用此接口更新设备密钥。
 * @attention 无。
 * @param [in] dev_key        设备key.
 * @param [in] active         true: 立即生效; false: 暂不生效.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_cfg_update_dev_key(const uint8_t dev_key[BLE_MESH_DEV_KEY_LEN], bool active);
#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
