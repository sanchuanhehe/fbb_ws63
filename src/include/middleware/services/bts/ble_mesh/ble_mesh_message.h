/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH MESSAGE API.
 */

/**
 * @defgroup bluetooth_bts_mesh_provision BLE MESH MESSAGE API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_MESSAGE_H
#define BLE_MESH_MESSAGE_H

#include <stdint.h>

#include "ble_mesh_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Mesh message context.
 * @else
 * @brief mesh消息上下文结构
 * @endif
 */
typedef struct ble_mesh_msg_header {
    uint16_t net_idx;                   /*!< @if Eng net index
                                             @else   网络索引 @endif */
    uint16_t app_idx;                   /*!< @if Eng app key index
                                             @else   app密钥索引 @endif */
    uint16_t src_addr;                  /*!< @if Eng source address
                                             @else   源地址 @endif */
    uint16_t dest_addr;                 /*!< @if Eng destination address
                                             @else   目的地址 @endif */
    uint8_t send_rel;                   /*!< @if Eng force sending reliably by using segment acknowledgment
                                             @else   强制分片发送 @endif */
    uint8_t send_ttl;                   /*!< @if Eng TTL
                                             @else   TTL @endif */
    int8_t  recv_rssi;                  /*!< @if Eng recv rssi
                                             @else   rssi值 @endif */
    uint8_t  recv_ttl;                  /*!< @if Eng Received TTL value
                                             @else   接收到的TTL值 @endif */
    uint32_t recv_op;                   /*!< @if Eng Received message code
                                             @else   接收到的消息码 @endif */
} ble_mesh_msg_header_t;

/**
 * @if Eng
 * @brief This API is a callback for processing after receiving a message.
 * @par This API is a callback for processing after receiving a message.
 * @attention.
 * @param  [in] ctx  message header.
 * @param  [in] data user data.
 * @param  [in] len  user data length.
 * @par Dependency:
 * @else
 * @brief  此接口为收到消息后的处理回调。
 * @par    此接口为收到消息后的处理回调。
 * @attention
 * @param  [in] ctx  消息头结构。
 * @param  [in] data 用户数据。
 * @param  [in] len  用户数据长度。
 * @par 依赖:
 * @endif
 */
typedef void (*ble_mesh_access_callback)(const ble_mesh_msg_header_t *ctx, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief This API is a callback for receive user-defined beacon data.
 * @par This API is a callback for receive user-defined beacon data.
 * @attention.
 * @param  [in] addr  MAC address of the beacon transmission device.
 * @param  [in] rssi  rssi.
 * @param  [in] data  user data.
 * @param  [in] len   user data length.
 * @par Dependency:
 * @else
 * @brief  此接口用于接收用户自定义beacon数据。
 * @par    此接口用于接收用户自定义beacon数据。
 * @attention
 * @param  [in] addr  beacon发送设备的mac地址。
 * @param  [in] rssi  rssi。
 * @param  [in] data  用户数据。
 * @param  [in] len   用户数据长度。
 * @par 依赖:
 * @see
 * @endif
 */
typedef void (*ble_mesh_beacon_recv_callback)(const bd_addr_t *addr, int8_t rssi, const uint8_t *data, uint8_t len);

/**
 * @if Eng
 * @brief This API is a callback for receive prov adv data.
 * @par This API is a callback for receive prov adv data.
 * @attention.
 * @param  [in] addr     MAC address of the prov adv transmission device.
 * @param  [in] rssi     rssi.
 * @param  [in] link_id  prov link id.
 * @param  [in] data     prov adv data.
 * @param  [in] len      prov adv length.
 * @retval true :continue process prov adv, false: no need to continue process prov adv
 * @par Dependency:
 * @else
 * @brief  此接口用于接收配网类型的广播数据。
 * @par    此接口用于接收配网类型的广播数据。
 * @attention
 * @param  [in] addr     配网广播发送设备的mac地址。
 * @param  [in] rssi     rssi.
 * @param  [in] link_id  链路ID。
 * @param  [in] data     配网数据。
 * @param  [in] len      配网数据长度。
 * @retval true : 需要继续处理配网广播，false：不需要继续处理配网广播
 * @par 依赖:
 * @see
 * @endif
 */
typedef bool (*ble_mesh_prov_adv_recv_callback)(const bd_addr_t *addr, int8_t rssi, uint32_t link_id,
                                                const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief This API is a callback for receive remote prov adv data.
 * @par This API is a callback for receive remote prov adv data.
 * @attention.
 * @param  [in] addr     Mesh address of remote prov client.
 * @param  [in] rssi     rssi.
 * @param  [in] link_id  prov link id, reserved.
 * @param  [in] data     prov adv data.
 * @param  [in] len      prov adv length.
 * @retval true :continue process remote prov adv, false: no need to continue process remote prov adv
 * @par Dependency:
 * @else
 * @brief  此接口用于接收远程配网类型的广播数据。
 * @par    此接口用于接收远程配网类型的广播数据。
 * @attention
 * @param  [in] src_addr 远程配网客户端的Mesh地址。
 * @param  [in] rssi     rssi.
 * @param  [in] link_id  链路ID，预留未使用。
 * @param  [in] data     配网数据。
 * @param  [in] len      配网数据长度。
 * @retval true : 需要继续处理配网广播，false：不需要继续处理配网广播
 * @par 依赖:
 * @see
 * @endif
 */
typedef bool (* ble_mesh_rmt_prov_adv_recv_callback)(uint16_t src_addr, int8_t rssi, uint32_t link_id,
                                                     const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief Struct of message  callback function.
 * @else
 * @brief mesh消息处理的回调函数接口定义。
 * @endif
 */
typedef struct ble_mesh_msg_callbacks {
    ble_mesh_access_callback            access_cb;           /*!< @if Eng Processing received message callback.
                                                                  @else   处理接收消息回调函数。 @endif */
    ble_mesh_beacon_recv_callback       beacon_recv_cb;      /*!< @if Eng Receive user-defined beacon data.
                                                                  @else   接收用户自定义beacon数据。 @endif */
    ble_mesh_prov_adv_recv_callback     pb_adv_recv_cb;      /*!< @if Eng Receive prov adv data.
                                                                  @else   接收配网广播数据。 @endif */
    ble_mesh_rmt_prov_adv_recv_callback rmt_pb_adv_recv_cb;   /*!< @if Eng Receive remote prov adv data.
                                                                   @else   接收远程配网广播数据。 @endif */
} ble_mesh_msg_callbacks_t;

/**
 * @if Eng
 * @brief This API is used to send mesh message.
 * @par This API is used to send mesh message.
 * @attention.
 * @param  [in] ctx  message header.
 * @param  [in] data user data.
 * @param  [in] len  user data length.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口发送mesh消息。
 * @par    调用此接口发送mesh消息。
 * @attention
 * @param  [in] ctx  消息头结构。
 * @param  [in] data 用户数据。
 * @param  [in] len  用户数据长度。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_msg_send(const ble_mesh_msg_header_t *ctx, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  This API is used to send mesh beacon packet.
 * @par  This API is used to send mesh beacon packet.
 * @param  [in] trans_count beacon transmit count
 * @param  [in] interval    beacon transmit interval, unit:ms
 * @param  [in] data        beacon payload
 * @param  [in] interval    beacon payload length
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  调用此接口发送mesh beacon报文。
 * @par 调用此接口发送mesh beacon报文。
 * @param  [in] trans_count beacon重传次数
 * @param  [in] interval    beacon发送周期，单位ms
 * @param  [in] data        beacon数据内容
 * @param  [in] interval    beacon数据长度
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_beacon_send(uint8_t trans_count, uint8_t interval, const uint8_t *data, uint8_t len);

/**
 * @if Eng
 * @brief This API is used to send prov adv message.
 * @par This API is used to send prov adv message.
 * @attention.
 * @param  [in] data user data.
 * @param  [in] len  user data length.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口发送配网消息。
 * @par    调用此接口发送配网消息。
 * @attention
 * @param  [in] data 用户数据。
 * @param  [in] len  用户数据长度。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_prov_adv_send(const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief On the remote provision server node, this API is called to send the provisioning PDU
 *        that was received from the device being provisioned.
 * @par On the remote provision server node, this API is called to send the provisioning PDU
 *        that was received from the device being provisioned.
 * @attention.
 * @param  [in] send_rel reliable send flag, true: reliable send, will wait peer response.
 * @param  [in] data     user data.
 * @param  [in] len      user data length.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  在远程配网服务节点，调用此接口发送收到的未入网节点的配网消息。
 * @par    在远程配网服务节点，调用此接口发送收到的未入网节点的配网消息。
 * @attention
 * @param  [in] send_rel 是否可靠发送, true 强制可靠发送，需等对端响应。
 * @param  [in] data     用户数据。
 * @param  [in] len      用户数据长度。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_rmt_prov_pdu_report(bool send_rel, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  This API is used to clear the queue of messages to be sent.
 * @par  This API is used to clear the queue of messages to be sent.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  调用此接口清除待发送的消息缓存队列。
 * @par 调用此接口清除待发送的消息缓存队列。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_clear_msg_buff(void);

/**
 * @if Eng
 * @brief  This API is used to get the number of messages to be sent.
 * @par  This API is used to get the number of messages to be sent.
 * @param  [in/out] count the number of messages.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  调用此接口获取待发送的消息缓存个数。
 * @par 调用此接口获取待发送的消息缓存个数。
 * @param  [in/out] count    消息缓存个数。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_get_msg_buff_count(uint8_t *count);

/**
 * @if Eng
 * @brief  Register the message callback.
 * @par Description: Register the message callback.
 * @param  [in] func callback function.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  注册消息模块回调函数。
 * @par  注册消息模块回调函数。
 * @param  [in] func 回调函数
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_msg_register_callbacks(const ble_mesh_msg_callbacks_t *func);

/**
 * @if Eng
 * @brief  Set whether to enable the broadcast fragment extension capability.
 * @par Set whether to enable the broadcast fragment extension capability.
 * @param  [in] flag true:Enabled, with a fragment length of 230 and support for one fragment size;
                     false: disabled, with a fragment length of 12.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  设置是否使能广播分片扩展能力。
 * @par 设置是否使能广播分片扩展能力。
 * @param  [in] flag true:使能，分片长度230，支持一片大小；false:去使能，分片长度12
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_set_extend_segment_adv(bool flag);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
