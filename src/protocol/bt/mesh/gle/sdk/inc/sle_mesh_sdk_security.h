/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle mesh security.
 */
#ifndef SLE_MESH_SDK_SECURITY_H
#define SLE_MESH_SDK_SECURITY_H

#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  SLE mesh network key length.
 * @else
 * @brief  SLE Mesh 网络密钥长度。
 * @endif
 */
#define SLE_MESH_NETWORK_KEY_LEN    16

/**
 * @if Eng
 * @brief SLE mesh authentication and encryption algorithm type.
 * @else
 * @brief 认证加密算法类型。
 * @endif
 */
typedef enum {
    SLE_MESH_AUTH_ALGO_SM4_GCM = 1,             /*!< @if Eng National algorithm sm4 gcm mode
                                                    @else 国密算法SM4 GCM模式 @endif */
    SLE_MESH_AUTH_ALGO_AES_GCM = 2              /*!< @if Eng Algorithm aes gcm mode
                                                    @else 算法AES GCM模式 @endif */
} sle_mesh_auth_algo_t;

/**
 * @if Eng
 * @brief SLE mesh integrity protection algorithm type.
 * @else
 * @brief 完整性保护算法类型。
 * @endif
 */
typedef enum {
    SLE_MESH_COMPLETE_PROTECT_ALGO_SM4_GCM = 1,  /*!< @if Eng National algorithm sm4 gcm mode.
                                                    @else 国密算法SM4 GCM模式 @endif */
    SLE_MESH_COMPLETE_PROTECT_ALGO_AES_GCM = 2   /*!< @if Eng Algorithm aes gcm mode.
                                                    @else AES算法GCM模式 @endif */
} sle_mesh_complete_algo_t;

/**
 * @if Eng
 * @brief SLE mesh key negotiation algorithm type.
 * @else
 * @brief 密钥协商算法类型。
 * @endif
 */
typedef enum {
    SLE_MESH_KEY_NEGO_ALGO_SM2 = 1,             /*!< @if Eng National key negotiation type SM2.
                                                    @else 国密密钥协商算法SM2 @endif */
    SLE_MESH_KEY_NEGO_ALGO_ECDH = 2             /*!< @if Eng Algorithm ecdh P-256.
                                                    @else ECDH P-256密钥协商算法 @endif */
} sle_mesh_key_nego_algo_t;

/**
 * @if Eng
 * @brief SLE mesh key derivation algorithm type.
 * @else
 * @brief 密钥派生算法类型。
 * @endif
 */
typedef enum {
    SLE_MESH_KEY_DERIVATE_ALGO_SM3_HMAC = 1,    /*!< @if Eng National key derivation type SM3-HMAC.
                                                    @else 国密密钥派生算法SM3-HMAC @endif */
    SLE_MESH_KEY_DERIVATE_ALGO_AES_CMAC = 2     /*!< @if Eng key derivation algorithm type AES-CMAC.
                                                    @else AES-CMAC密钥派生算法 @endif */
} sle_mesh_key_derivation_algo_t;

/**
 * @if Eng
 * @brief Set the periodic update interval for the network key.
 * @par Description: Set the periodic update interval for the network key.
 * @attention Synchronous processing on admin node.
 * @param [inout] update_time  Network key update interval, in minutes.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置网络密钥定时更新周期。
 * @par Description: 设置网络密钥定时更新周期。
 * @attention 管理同步执行。
 * @param [inout] update_time  更新网络密钥周期，单位：分钟。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_netkey_periodic_update(uint32_t update_time);

/**
 * @if Eng
 * @brief Proactively update the network key.
 * @par Description: Proactively update the network key.
 * @attention Synchronous processing on admin node.
 * @param [in] key       Network key.
 * @param [in] key_len   Network key length, fixed at 16 bytes.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  更新网络密钥。
 * @par Description: 更新网络密钥。
 * @attention 管理同步执行。
 * @param [in] key       网络密钥。
 * @param [in] key_len   网络密钥长度，固定16Byte。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_update_netkey(uint8_t *key, uint8_t key_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* END of SLE_MESH_SDK_SECURITY_H */