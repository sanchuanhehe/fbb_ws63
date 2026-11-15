/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH COMMON API.
 */

/**
 * @defgroup bluetooth_bts_mesh_common BLE MESH COMMON API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_COMMON_H__
#define BLE_MESH_COMMON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Length of app key.
 * @else
 * @brief  APP Key 长度。
 * @endif
 */
#define BLE_MESH_APP_KEY_LEN 16

/**
 * @if Eng
 * @brief  Length of uuid.
 * @else
 * @brief  UUID 长度。
 * @endif
 */
#define BLE_MESH_UUID_LEN 16

/**
 * @if Eng
 * @brief  Length of static auth value len.
 * @else
 * @brief  静态OOB鉴权数据长度。
 * @endif
 */
#define BLE_MESH_STATIC_AUTH_LEN 32

/**
 * @if Eng
 * @brief  Length of device key.
 * @else
 * @brief  device key 长度。
 * @endif
 */
#define BLE_MESH_DEV_KEY_LEN 16

/**
 * @if Eng
 * @brief  Length of net key.
 * @else
 * @brief  net key 长度。
 * @endif
 */
#define BLE_MESH_NET_KEY_LEN 16

/**
 * @if Eng
 * @brief Struct of prov info.
 * @else
 * @brief 配网信息
 * @endif
 */
typedef struct ble_mesh_prov_net_info {
    uint16_t net_idx;                       /*!< @if Eng net index.
                                                 @else   网络索引。@endif */
    uint16_t addr;                          /*!< @if Eng net addr.
                                                 @else   mesh设备地址。@endif */
    uint16_t elem_addr;                     /*!< @if Eng elelment addr.
                                                 @else   元素地址。@endif */
    uint16_t key_net_idx;                   /*!< @if Eng net key index.
                                                 @else   网络密钥索引。@endif */
    uint16_t key_app_idx;                   /*!< @if Eng app key index.
                                                 @else   应用密钥索引。@endif */
    uint8_t app_key[BLE_MESH_APP_KEY_LEN];  /*!< @if Eng app key.
                                                 @else   应用密钥。@endif */
} ble_mesh_prov_net_info_t;

typedef struct ble_mesh_net_buff {
    uint16_t len;     /*!< @if Eng data length.
                           @else   数据长度。@endif */
    uint16_t off;     /*!< @if Eng data offset.
                           @else   数据偏移。@endif */
    uint8_t *payload; /*!< @if Eng data payload.
                           @else   应用数据。@endif */
} ble_mesh_net_buff_t;

/**
 * @if Eng
 * @brief Use this funtion to pull 2-byte data in little-endian order from network packets.
 * @par   Use this funtion to pull 2-byte data in little-endian order from network packets.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval 2-byte data.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  从网络报文中按照小端序取2字节数据。
 * @par    从网络报文中按照小端序取2字节数据。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 2字节数据。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint16_t ble_mesh_net_buf_pull_le16(ble_mesh_net_buff_t *net_buff);

/**
 * @if Eng
 * @brief Use this funtion to pull 2-byte data in big-endian order from network packets.
 * @par   Use this funtion to pull 2-byte data in big-endian order from network packets.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval 2-byte data.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  从网络报文中按照大端序取2字节数据。
 * @par    从网络报文中按照大端序取2字节数据。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 2字节数据。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint16_t ble_mesh_net_buf_pull_be16(ble_mesh_net_buff_t *net_buff);

/**
 * @if Eng
 * @brief Use this funtion to pull 3-byte data in little-endian order from network packets.
 * @par   Use this funtion to pull 3-byte data in little-endian order from network packets.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval 3-byte data.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  从网络报文中按照小端序取3字节数据。
 * @par    从网络报文中按照小端序取3字节数据。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 3字节数据。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint32_t ble_mesh_net_buf_pull_le24(ble_mesh_net_buff_t *net_buff);

/**
 * @if Eng
 * @brief Use this funtion to pull 4-byte data in big-endian order from network packets.
 * @par   Use this funtion to pull 4-byte data in big-endian order from network packets.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval 4-byte data.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  从网络报文中按照大端序取4字节数据。
 * @par    从网络报文中按照大端序取4字节数据。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 4字节数据。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint32_t ble_mesh_net_buf_pull_be32(ble_mesh_net_buff_t *net_buff);

/**
 * @if Eng
 * @brief Use this funtion to pull 4-byte data in little-endian order from network packets.
 * @par   Use this funtion to pull 4-byte data in little-endian order from network packets.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval 4-byte data.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  从网络报文中按照小端序取4字节数据。
 * @par    从网络报文中按照小端序取4字节数据。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 4字节数据。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint32_t ble_mesh_net_buf_pull_le32(ble_mesh_net_buff_t *net_buff);

/**
 * @if Eng
 * @brief Use this funtion to get left data length.
 * @par   Use this funtion to get left data length.
 * @attention NULL
 * @param  [in] net_buff  net buff information.
 * @retval left data length
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  返回当前剩余数据长度。
 * @par    返回当前剩余数据长度。
 * @attention 无
 * @param  [in] net_buff  网络报文信息。
 * @retval 剩余数据长度。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
uint32_t ble_mesh_net_buf_left_len(ble_mesh_net_buff_t *net_buff);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
