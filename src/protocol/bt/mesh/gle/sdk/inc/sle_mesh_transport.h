/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 数据传输功能接口声明文件
 */

#ifndef SLE_MESH_TRANSPORT_H
#define SLE_MESH_TRANSPORT_H

#undef THIS_FILE_ID
#define THIS_FILE_ID _SLE_MESH_TRANSPORT_H_

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief Data transfer protocol enumeration value.
 * @else
 * @brief 数据传输协议枚举值。
 * @endif
 */
typedef enum {
    SLE_MESH_CLTP_TRANSPORT_PROTOCOL,   /*!< @if Eng Use the CLTP protocol for transmission.
                                           @else   使用cltp协议进行传输 @endif */
    SLE_MESH_LWCTP_TRANSPORT_PROTOCOL, /*!< @if Eng Use lwcltp protocol for transmission
                                           @else   使用lwcltp协议进行传输 @endif */
    SLE_MESH_TRANSPORT_PROTOCOL_BUTT,
} sle_mesh_transport_protocol_t;

/**
 * @if Eng
 * @brief Enumerated value of the control parameter.
 * @else
 * @brief 控制参数枚举值。
 * @endif
 */
typedef enum {
    SLE_MESH_TRANSPORT_SEND_MSG = 0,        /*!< @if Eng Send message
                                                 @else   发送消息 */
    SLE_MESH_TRANSPORT_START_SEND = 1,      /*!< @if Eng Start transmission
                                                 @else   开始传输 */
    SLE_MESH_TRANSPORT_START_SEND_RSP = 3,  /*!< @if Eng Start response
                                                 @else   启动响应 */
    SLE_MESH_TRANSPORT_END_SEND = 4,        /*!< @if Eng End transmission
                                                 @else   结束传输 */
    SLE_MESH_TRANSPORT_END_SEND_RSP = 8,    /*!< @if Eng End response
                                                 @else   结束响应 */
    SLE_MESH_TRANSPORT_RESET_LINK = 16,     /*!< @if Eng Reset connection
                                                 @else   重置链接 */
} sle_mesh_transport_control_parameters_t;

/**
 * @if Eng
 * @brief Struct of static data send param.
 * @else
 * @brief 数据传输参数结构体。
 * @endif
 */
typedef struct {
    sle_mesh_transport_protocol_t  trans_protocol;  /*!< @if Eng Data transmission protocol
                                                         @else   数据传输协议 @endif */
    uint16_t                       src_port;        /*!< @if Eng Data transmission source port, user-defined port range:
                                                                 [1, 41216], internal port range used by mesh:
                                                                 (41216, 65535]
                                                         @else   数据传输源端口，用户自定义port端口范围: [1, 41216),
                                                                 mesh 内部使用端口范围：[41216,65535] @endif */
    uint16_t                       dest_port;       /*!< @if Eng Destination port for data transmission, user-defined
                                                                 port range: [1, 41216], internal port range used by
                                                                 mesh: (41216, 65535]
                                                         @else   数据传输目的端口,用户自定义port端口范围: [1, 41216),
                                                                 mesh 内部使用端口范围：[41216,65535] @endif */
    uint16_t                       dest_mesh_id;    /*!< @if Eng Destination route address for data transmission
                                                         @else   数据发送目的路由地址 @endif */
    uint16_t                       data_len;        /*!< @if Eng Send data length
                                                         @else   发送数据长度 @endif */
    uint8_t                        ttl_val;         /*!< @if Eng Data transmission route hop count
                                                         @else   数据发送路由跳数 @endif */
    uint8_t                        data[0];         /*!< @if Eng Send data
                                                         @else   发送数据 @endif */
} sle_mesh_transport_par_t;

/**
 * @if Eng
 * @brief  Struct of static lwctp data send param.
 * @else
 * @brief  数据可靠传输参数结构体。
 * @endif
 */
typedef struct {
    sle_mesh_transport_control_parameters_t  trans_ctrl;      /*!< @if Eng Control parameters
                                                                   @else   控制参数 @endif */
    uint16_t                                 src_port;        /*!< @if Eng Data transmission source port
                                                                   @else   数据传输源端口 @endif */
    uint16_t                                 dest_port;       /*!< @if Eng Destination port for data transmission
                                                                   @else   数据传输目的端口 @endif */
    uint16_t                                 dest_mesh_id;    /*!< @if Eng Destination route address for data
                                                                           transmission
                                                                   @else   数据发送目的路由地址 @endif */
    uint16_t                                 data_len;        /*!< @if Eng Send data length
                                                                   @else   发送数据长度 @endif */
    uint8_t                                  ttl_val;         /*!< @if Eng Data transmission route hop count
                                                                   @else   数据发送路由跳数 @endif */
    uint8_t                                  data[0];         /*!< @if Eng Send data
                                                                   @else   发送数据 @endif */
} sle_mesh_transport_credible_t;

/**
 * @if Eng
 * @brief Callback invoked when transport received data.
 * @par Callback invoked when transport received data.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in]    src_port Data transport source port.
 * @param [in]    src_mesh_id Data transport peer device network ID.
 * @param [in]    data_len Received data length.
 * @param [inout] data Received data buffer pointer.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_transport_callback_t
 * @else
 * @brief  数传数据接收函数回调。
 * @par    数传数据接收函数回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in]    src_port 数据传输源端口。
 * @param [in]    src_mesh_id  数据发送方网络地址。
 * @param [in]    data_len 接收到的数据长度。
 * @param [inout] data  接收到的数据。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_transport_callback_t
 * @endif
 */
typedef void (*sle_mesh_transport_recv_callback)(uint16_t src_port, uint16_t src_mesh_id, uint16_t data_len,
    uint8_t *data);

/**
 * @if Eng
 * @brief  Data transport callback function.
 * @else
 * @brief  数据传输回调函数。
 * @endif
 */
typedef struct {
    sle_mesh_transport_recv_callback  recv_cbk;      /*!< @if Eng Data receive callback function.
                                                          @else   数据接收回调函数 @endif */
} sle_mesh_transport_callback_t;

/**
 * @if Eng
 * @brief Register data transport callback function.
 * @par Description:Register data transport callback function.
 * @attention Synchronous processing on any node.
 * @param [in] port Data transport port.
 * @param [in] cbk  Data transport callback function { @ref sle_mesh_transport_callback_t }。
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册数据传输回调函数。
 * @par Description: 注册数据传输回调函数。
 * @attention 任意节点同步执行。
 * @param [in] port 数据传输端口。
 * @param [in] cbk  数据传输回调函数 { @ref sle_mesh_transport_callback_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_transport_register_callback(uint16_t port, sle_mesh_transport_callback_t *cbk);

/**
 * @if Eng
 * @brief Data transmitted through data transport.
 * @par Description:Data transmitted through data transport.
 * @attention Synchronous processing on any node.
 * @param [in] par Data transport parameter { @ref sle_mesh_transport_par_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  通过数据传输发送数据。
 * @par Description: 通过数据传输发送数据。
 * @attention 任意节点同步执行。
 * @param [in] par 数据传输参数 { @ref sle_mesh_transport_par_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_transport_send(sle_mesh_transport_par_t *par);

/**
 * @if Eng
 * @brief Data transmitted through credible data transport channel.
 * @par Description:Data transmitted through credible data transport channel.
 * @attention Synchronous processing on any node.
 * @param [in] param Credible data transport parameter { @ref sle_mesh_transport_credible_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  通过可靠通道发送数据。
 * @par Description: 通过可靠通道发送数据。
 * @attention 任意节点同步执行。
 * @param [in] param 可靠数据传输参数 { @ref sle_mesh_transport_credible_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_transport_send_credible_data(sle_mesh_transport_credible_t *param);

/**
 * @if Eng
 * @brief Create credible data transport channel.
 * @par Description:Create credible data transport channel.
 * @attention Synchronous processing on any node.
 * @param [in] param Credible data transport parameter { @ref sle_mesh_transport_credible_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  创建可靠数据传输通道。
 * @par Description: 创建可靠数据传输通道。
 * @attention 任意节点同步执行。
 * @param [in] param 可靠数据传输参数 { @ref sle_mesh_transport_credible_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_transport_create_credible_channel(sle_mesh_transport_credible_t *param);

/**
 * @if Eng
 * @brief Destroy credible data transport channel.
 * @par Description:Destroy credible data transport channel.
 * @attention Synchronous processing on any node.
 * @param [in] param Credible data transport parameter { @ref sle_mesh_transport_credible_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  销毁可靠数据传输通道。
 * @par Description: 销毁可靠数据传输通道。
 * @attention 任意节点同步执行。
 * @param [in] param 可靠数据传输参数 { @ref sle_mesh_transport_credible_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_transport_destroy_credible_channel(sle_mesh_transport_credible_t *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of SLE_MESH_TRANSPORT_H */