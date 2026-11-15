/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH HEARTBEAT API.
 */

/**
 * @defgroup bluetooth_bts_mesh_provision BLE MESH HEARTBEAT API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_HEARTBEAT_H
#define BLE_MESH_HEARTBEAT_H

#include <stdint.h>

#include "ble_mesh_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Heartbeat Subscription parameters.
 * @else
 * @brief 心跳订阅参数
 * @endif
 */
typedef struct ble_mesh_heartbeat_sub {
    uint32_t period;                    /*!< @if Eng Subscription period in seconds.
                                             @else   订阅周期(秒) @endif */
    uint32_t remaining;                 /*!< @if Eng Remaining subscription time in seconds.
                                             @else   剩余订阅时间(秒) @endif */
    uint16_t src;                       /*!< @if Source address to receive Heartbeats from.
                                             @else   接收心跳消息的源地址 @endif */
    uint16_t dst;                       /*!< @if Destination address to received Heartbeats on.
                                             @else   接收心跳消息的目的地址。 @endif */
    uint16_t count;                     /*!< @if The number of received Heartbeat messages so far.
                                             @else   接收到的心跳消息数量 @endif */
    uint8_t min_hops;                   /*!< @if Eng Minimum hops in received messages, ie the shortest registered
                                             @else   接收到的消息中的最小跳数 @endif */
    uint8_t max_hops;                   /*!< @if Eng Maximum hops in received messages, ie the longest registered
                                             @else   接收到的消息中的最大跳数 @endif */
} ble_mesh_heartbeat_sub_t;

/**
 * @if Eng
 * @brief Heartbeat publish parameters.
 * @else
 * @brief 心跳发布参数
 * @endif
 */
typedef struct ble_mesh_heartbeat_pub {
    uint16_t dst;                       /*!< @if Destination address to publish Heartbeats on.
                                             @else   心跳消息发布的目的地址。 @endif */
    uint16_t count;                     /*!< @if The number of Heartbeat messages to published.
                                                 0x00:Periodic Heartbeat messages are not published
                                                 0x01–0x11:Number of Heartbeat messages, 2(n-1), that remain to be sent
                                                 0xff:Periodic Heartbeat messages are published indefinitely
                                             @else   心跳发布消息数量
                                                 0x00:不进行心跳消息发布
                                                 0x01–0x11:待发布的心跳消息数为2(n-1)
                                                 0xff:周期性心跳消息无限期发布 @endif */
    uint8_t ttl;                        /*!< @if Time to live value of Heartbeat messages to published.
                                             @else   心跳发布消息数量TTL @endif */
    uint16_t feature;                   /*!< @if Bitmap of features that trigger a Heartbeat publication if they change.
                                             @else   触发心跳发布的特性位图 @endif */
    uint16_t net_idx;                   /*!< @if Network index used for publishing.
                                             @else   心跳发布的网络索引 @endif */
    uint32_t period;                    /*!< @if Publication period, The value is represented as 2(n-1) seconds.
                                             @else   心跳发布周期,该值表示为2(n-1)秒 @endif */
} ble_mesh_heartbeat_pub_t;

/**
 * @if Eng
 * @brief This API is a callback for receiving a heartbeat message.
 * @par This API is a callback for receiving a heartbeat message.
 * @attention.
 * @param  [in] sub      Heartbeat message parameters.
 * @param  [in] hops     hops.
 * @param  [in] feature  Bit field of currently active features of the node.
 * @retval
 * @par Dependency:
 * @else
 * @brief  此接口用于接收心跳消息回调。
 * @par    此接口用于接收心跳消息回调。
 * @attention
 * @param  [in] sub      心跳消息参数。
 * @param  [in] hops     跳数。
 * @param  [in] feature  节点当前激活功能的位字段。
 * @retval
 * @par 依赖:
 * @endif
 */
typedef void (*ble_mesh_heartbeat_recv_callback)(const ble_mesh_heartbeat_sub_t *sub, uint8_t hops, uint16_t feature);

/**
 * @if Eng
 * @brief Struct of heartbeat callback function.
 * @else
 * @brief  心跳的回调函数接口定义。
 * @endif
 */
typedef struct ble_mesh_heartbeat_callbacks {
    ble_mesh_heartbeat_recv_callback heartbeat_recv_cb;    /*!< @if Eng heartbeat info callback.
                                                            @else   接收心跳信息回调函数。 @endif */
} ble_mesh_heartbeat_callbacks_t;

/**
 * @if Eng
 * @brief  Register the heartbeat callback.
 * @par Description: Register the heartbeat callback.
 * @param  [in] func callback function.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  心跳回调函数。
 * @par Description: 心跳回调函数。
 * @param  [in] func 回调函数
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_heartbeat_callbacks(const ble_mesh_heartbeat_callbacks_t *func);

/**
 * @if Eng
 * @brief This API is used to set hb pub parameters.
 * @par This API is used to set hb pub parameters.
 * @attention
 * @param  [in] pub param @ref ble_mesh_hb_pub_t.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  设置心跳参数。
 * @par    设置心跳参数。
 * @attention
 * @param  [in] pub 参数结构体 @ref ble_mesh_hb_pub_t.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_heartbeat_pub(const ble_mesh_heartbeat_pub_t *pub);

/**
 * @if Eng
 * @brief This API is used to get heartbeart pub parameters.
 * @par This API is used to set heartbeart pub parameters.
 * @attention
 * @param  [in] pub param @ref ble_mesh_heartbeat_pub_t.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  获取心跳参数。
 * @par    获取心跳参数。
 * @attention
 * @param  [in] pub 参数结构体 @ref ble_mesh_heartbeat_pub_t.
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_get_heartbeat_pub(ble_mesh_heartbeat_pub_t *pub);

/**
 * @if Eng
 * @brief This API is used to get hb pub addr.
 * @par This API is used to get hb pub addr.
 * @attention
 * @param  [in] NULL.
 * @retval hb pub addr.
 * @par Dependency:
 * @else
 * @brief  获取心跳地址。
 * @par    获取心跳地址。
 * @attention
 * @param  [in] 无。
 * @retval 心跳地址。
 * @par 依赖:
 * @endif
 */
uint16_t ble_mesh_get_heartbeat_pub_addr(void);

/**
 * @if Eng
 * @brief This API is used to start hb.
 * @par This API is used to strart hb.
 * @attention
 * @param  [in] NULL.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  心跳启动。
 * @par    心跳启动。
 * @attention
 * @param  [in] 无。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_start_heartbeat(void);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
