/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: ble_mesh_config.h ble mesh config 接口描述
 */

/**
 * @defgroup bluetooth_bts_mesh_ota BLE MESH CONFIG API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_CONFIG_H
#define BLE_MESH_CONFIG_H

#include <stdint.h>

#include "ble_mesh_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of ble mesh feature state.
 * @else
 * @brief BLE Mesh特性状态。
 * @endif
 */
typedef enum {
    BLE_MESH_FEATURE_DISABLED  = 0x00,       /*!< @if Eng Feature is disabled
                                                  @else   特性去使能 @endif */
    BLE_MESH_FEATURE_ENABLED   = 0x01,       /*!< @if Eng Feature is enabled
                                                  @else   特性使能 @endif */
    BLE_MESH_FEATURE_UNSUPPORT = 0x02,       /*!< @if Eng Feature is not support
                                                  @else   特性不支持 @endif */
} ble_mesh_feat_state_t;

/**
 * @if Eng
 * @brief This API is used to set default ttl.
 * @par This API is used to set default ttl.
 * @attention
 * @param  [in] default_ttl  default ttl.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  设置默认TTL。
 * @par    设置默认TTL。
 * @attention
 * @param  [in] default_ttl  默认TTL。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_default_ttl(uint8_t default_ttl);

/**
 * @if Eng
 * @brief This API is used to get default ttl.
 * @par This API is used to get default ttl.
 * @attention
 * @retval default ttl
 * @par Dependency:
 * @else
 * @brief  获取默认TTL。
 * @par    获取默认TTL。
 * @attention
 * @retval 默认ttl
 * @par 依赖:
 * @endif
 */
uint8_t ble_mesh_get_default_ttl(void);

/**
 * @if Eng
 * @brief This API is used to set message transmission parameters.
 * @par This API is used to set message transmission parameters.
 * @attention
 * @param  [in] count  message transmit count, The value range is 0-7.
 * @param  [in] interval  message transmit interval.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  设置消息传输参数。
 * @par    设置消息传输参数。
 * @attention
 * @param  [in] count  消息传输次数，取值范围0-7。
 * @param  [in] interval  消息传输间隔。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_net_transmit(uint8_t count, uint16_t interval);

/**
 * @if Eng
 * @brief This API is used to get message transmission parameters.
 * @par This API is used to get message transmission parameters.
 * @attention
 * @param  [in,out] count  message transmit count.
 * @param  [in,out] interval  message transmit interval.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取消息传输参数。
 * @par    获取消息传输参数。
 * @attention
 * @param  [in,out] count  消息传输次数。
 * @param  [in,out] interval  消息传输间隔。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_get_net_transmit(uint8_t *count, uint16_t *interval);

/**
 * @if Eng
 * @brief This API is used to set relay parameters.
 * @par This API is used to set relay parameters.
 * @attention
 * @param  [in] state     relay feature state.
 * @param  [in] count     relay retransmit count.
 * @param  [in] interval  relay transmit interval.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  中继参数设置。
 * @par    中继参数设置。
 * @attention
 * @param  [in] state     中继使能状态。
 * @param  [in] count     中继消息重传次数。
 * @param  [in] interval  中继消息传输间隔。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_relay(ble_mesh_feat_state_t state, uint8_t count, uint16_t interval);

/**
 * @if Eng
 * @brief This API is used to set relay state.
 * @par This API is used to set relay state.
 * @attention
 * @param  [in] state     relay feature state.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  中继使能状态设置。
 * @par    中继使能状态设置。
 * @attention
 * @param  [in] state     中继使能状态。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_relay_state(ble_mesh_feat_state_t state);

/**
 * @if Eng
 * @brief This API is used to get relay feature state.
 * @par This API is used to get relay feature state.
 * @attention
 * @retval BLE_MESH_FEATURE_DISABLED relay enable.
           BLE_MESH_FEATURE_ENABLED relay disable.
           BLE_MESH_FEATURE_UNSUPPORT relay not support.
 * @par Dependency:
 * @else
 * @brief  中继状态获取。
 * @par    中继状态获取。
 * @attention
 * @retval BLE_MESH_FEATURE_DISABLED 中继使能。
           BLE_MESH_FEATURE_ENABLED 中继未使能。
           BLE_MESH_FEATURE_UNSUPPORT 中继不支持。
 * @par 依赖:
 * @endif
 */
ble_mesh_feat_state_t ble_mesh_get_relay_state(void);

/**
 * @if Eng
 * @brief This API is used to get relay message transmission parameters.
 * @par This API is used to get relay message transmission parameters.
 * @attention
 * @param  [in,out] count     relay message transmit count.
 * @param  [in,out] interval  relay message transmit interval.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取中继消息传输参数。
 * @par    获取中继消息传输参数。
 * @attention
 * @param  [in,out] count     中继消息传输次数。
 * @param  [in,out] interval  中继消息传输间隔。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_get_relay_transmit(uint8_t *count, uint16_t *interval);

/**
 * @if Eng
 * @brief This API is used to get mod sub list.
 * @par This API is used to get mod sub list.
 * @attention
 * @param  [in] mod_id  mod id.
 * @param  [in,out] count  sub count.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取mod组订阅列表。
 * @par    获取mod组订阅列表。
 * @attention
 * @param  [in] mod_id  模型id.
 * @param  [in,out] count  订阅地址个数.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_get_mod_sub_list_count(uint32_t mod_id, uint16_t *count);

/**
 * @if Eng
 * @brief This API is used to get mod sub list.
 * @par This API is used to get mod sub list.
 * @attention
 * @param  [in] mod_id  mod id.
 * @param  [in] list  group list.
 * @param  [in] count  sub count.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取mod组订阅列表。
 * @par    获取mod组订阅列表。
 * @attention
 * @param  [in] mod_id  模型id.
 * @param  [in] list  组订阅地址列表.
 * @param  [in] count  订阅地址个数.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_get_mod_sub_list(uint32_t mod_id, uint16_t *list, uint16_t *count);

/**
 * @if Eng
 * @brief This API is used to add a group subscription address based on the module ID.
 * @par This API is used to add a group subscription address based on the module ID.
 * @attention
 * @param  [in] sub_addr  group subscription address.
 * @param  [in] mod_id  module ID.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  根据model id添加组订阅地址。
 * @par    根据model id添加组订阅地址。
 * @attention
 * @param  [in] sub_addr  组订阅地址。
 * @param  [in] mod_id  模型id。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_add_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id);

/**
 * @if Eng
 * @brief This API is used to add a group subscription address for all modules under the specified element address.
 * @par This API is used to add a group subscription address for all modules under the specified element address.
 * @attention
 * @param  [in] sub_addr   group subscription address.
 * @param  [in] elem_addr  element address.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  为指定元素地址下的所有model添加组订阅地址。
 * @par    为指定元素地址下的所有model添加组订阅地址。
 * @attention
 * @param  [in] sub_addr   组订阅地址。
 * @param  [in] elem_addr  元素地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_add_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr);

/**
 * @if Eng
 * @brief This API is used to delete a group subscription address based on the module ID.
 * @par This API is used to delete a group subscription address based on the module ID.
 * @attention
 * @param  [in] sub_addr  group subscription address.
 * @param  [in] mod_id  module ID.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  根据model id删除组订阅地址。
 * @par    根据model id删除组订阅地址。
 * @attention
 * @param  [in] sub_addr  组订阅地址。
 * @param  [in] mod_id  模型id。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_del_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id);

/**
 * @if Eng
 * @brief This API is used to delete a group subscription address for all modules under the specified element address.
 * @par This API is used to delete a group subscription address for all modules under the specified element address.
 * @attention
 * @param  [in] sub_addr   group subscription address.
 * @param  [in] elem_addr  element address.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  删除指定元素地址下的所有model的组订阅地址。
 * @par    删除指定元素地址下的所有model的组订阅地址。
 * @attention
 * @param  [in] sub_addr   组订阅地址。
 * @param  [in] elem_addr  元素地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_del_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr);

/**
 * @if Eng
 * @brief This API is used to overwrite a group subscription address based on the module ID.
 * @par This API is used to overwrite a group subscription address based on the module ID.
 * @attention
 * @param  [in] sub_addr  group subscription address.
 * @param  [in] mod_id  module ID.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  根据model id重写组订阅地址。
 * @par    根据model id重写组订阅地址。
 * @attention
 * @param  [in] sub_addr  组订阅地址。
 * @param  [in] mod_id  模型id。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_overwrite_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id);

/**
 * @if Eng
 * @brief This API is used to overwrite a group subscription address
 *        for all modules under the specified element address.
 * @par This API is used to overwrite a group subscription address for all modules under the specified element address.
 * @attention
 * @param  [in] sub_addr   group subscription address.
 * @param  [in] elem_addr  element address.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  重写指定元素地址下的所有model的组订阅地址。
 * @par    重写指定元素地址下的所有model的组订阅地址。
 * @attention
 * @param  [in] sub_addr   组订阅地址。
 * @param  [in] elem_addr  元素地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_overwrite_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */