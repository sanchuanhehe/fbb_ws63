/* *
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle mesh dfx interface.
 */
#ifndef SLE_MESH_SDK_DFX_H
#define SLE_MESH_SDK_DFX_H

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
 * @brief DFX statistics modules.
 * @else
 * @brief DFX统计模块。
 * @endif
 */
typedef enum {
    SLE_MESH_DFX_OTA = 0x00,        /*!< @if Eng OTA.
                                         @else OTA. @endif */
    SLE_MESH_DFX_SERVICE = 0x01,    /*!< @if Eng Service management.
                                         @else 服务管理。 @endif */
    SLE_MESH_DFX_NM = 0x02,         /*!< @if Eng Network management.
                                         @else 网络管理。 @endif */
    SLE_MESH_DFX_MODULE_BUTT
} sle_mesh_dfx_module_t;

/**
 * @if Eng
 * @brief OTA module statistics items.
 * @else
 * @brief OTA模块统计项。
 * @endif
 */
typedef enum {
    DFX_OTA_ABNORMAL_STOP = 0x00,       /*!< @if Eng OTA Abnormal Termination.
                                         @else OTA异常终止. @endif */
    DFX_OTA_HIGH_PACKET_LOSS = 0x01,    /*!< @if Statistics on Excessive Packet Loss (More Than 100 Packets).
                                         @else 丢包数过高统计（超过100包）。 @endif */
    DFX_OTA_BUTT,
} sle_mesh_dfx_ota_item_t;

/**
 * @if Eng
 * @brief Service management module statistics items.
 * @else
 * @brief 服务管理模块统计项。
 * @endif
 */
typedef enum {
    DFX_SERVICE_SUBCRIBE_MALLOC_ERROR = 0x00,           /*!< @if Eng Failed to apply for service subscription memory.
                                                             @else 服务订阅内存申请失败. @endif */
    DFX_SERVICE_PROPERTY_CHANGE_MALLOC_ERROR = 0x01,    /*!< @if Eng Failed to allocate memory for attribute change.
                                                             @else 属性变更内存申请失败. @endif */
    DFX_SERVICE_REPORT_MALLOC_ERROR = 0x02,             /*!< @if Eng Failed to allocate memory for service reporting.
                                                             @else 服务信息上报内存申请失败. @endif */
    DFX_SERVICE_SUBCRIBE_INVALID_PARAM = 0x03,          /*!< @if Eng Invalid subscription parameters.
                                                             @else 订阅参数非法. @endif */
    DFX_SERVICE_PROPERTY_CHANGE_INVALID_PARAM = 0x04,   /*!< @if Eng Invalid property change parameter.
                                                             @else 属性变更参数非法. @endif */
    DFX_SERVICE_UNICASAT_INVALID_PARAM = 0x05,          /*!< @if Eng Unicast address is invalid.
                                                             @else 单播地址非法. @endif */
    DFX_SERVICE_MULITCAST_INVALID_PARAM = 0x06,         /*!< @if Eng Invalid multicast address.
                                                             @else 组播地址非法. @endif */
    DFX_SERVICE_UNICASAT_PUBLISH_INVALID_PARAM = 0x07,  /*!< @if Eng Invalid unicast advertisement parameters.
                                                             @else 单播发布参数非法. @endif */
    DFX_SERVICE_MULITCAST_PUBLISH_INVALID_PARAM = 0x08, /*!< @if Eng Invalid multicast advertisement parameters.
                                                             @else 组播发布参数非法. @endif */
    DFX_SERVICE_BUTT,
} sle_mesh_dfx_service_item_t;

/**
 * @if Eng
 * @brief Network management module statistics items.
 * @else
 * @brief 网络管理模块统计项。
 * @endif
 */
typedef enum {
    DFX_NM_ACCESS_POINT_LOSS = 0x00,    /*!< @if Eng Access point connection lost.
                                             @else 接入点连接断开. @endif */
    DFX_NM_ACCESS_NET_SUCCESS = 0x01,   /*!< @if Eng Network access successful.
                                             @else 网络接入成功. @endif */
    DFX_NM_BUTT,
} sle_mesh_dfx_nm_item_t;

/**
 * @if Eng
 * @brief Callback interface for reporting DFX information query results.
 * @par Callback interface for reporting DFX information query results.
 * @attention 1. This callback function runs in the SLE mesh thread and must not be blocked or perform long operations.
 * @attention 2. The memory for pointers is allocated and freed by the SLE mesh automatically;
 *               Do not free them in the callback.
 * @param [in] module   Module ID { @ref sle_mesh_dfx_module_t }.
 * @param [in] nums     Number of DFX point statistics entries.
 * @param [in] records  DFX point statistics records.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  DFX打点信息查询结果回调上报接口。
 * @par DFX打点信息查询结果回调上报接口。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] module   模块ID { @ref sle_mesh_dfx_module_t }。
 * @param [in] nums     打点统计下个数。
 * @param [in] records  打点统计记录。
 * @retval 无返回值。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
typedef void (*sle_mesh_query_dfx_callback)(sle_mesh_dfx_module_t module, uint8_t nums, uint16_t *records);

/**
 * @if Eng
 * @brief DFX callback structure.
 * @else
 * @brief DFX回调结构。
 * @endif
 */
typedef struct {
    sle_mesh_query_dfx_callback dfx_query_cbk;  /*!< @if Eng Callback for DFX querying information.
                                                     @else  DFX 打点信息查询回调 @endif */
} sle_mesh_dfx_callback_t;

/**
 * @if Eng
 * @brief Obtain DFX information about a specified node.
 * @par Obtain DFX information about a specified node.
 * @attention Asynchronous processing on admin node,
 *          reported via the dfx_query_cbk callback in { @ref sle_mesh_dfx_callback_t }.
 * @param [in] mesh_id Target device network id.
 * @param [in] module  module ID. { @ref sle_mesh_dfx_module_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询指定节点的DFX打点信息。
 * @par 查询指定节点的DFX打点信息。
 * @attention 管理节点异步执行，通过 { @ref sle_mesh_dfx_callback_t } 中的 dfx_query_cbk 回调上报。
 * @param [in] mesh_id 目标设备网络地址。
 * @param [in] module  查询的模块ID { @ref sle_mesh_dfx_module_t }。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_query_dfx_info(uint16_t mesh_id, sle_mesh_dfx_module_t module);

/**
 * @if Eng
 * @brief Register the DFX query result callback function.
 * @par Register the DFX query result callback function.
 * @attention Synchronous processing on admin node.
 * @param [in] func Callback function.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册DFX查询结果回调函数。
 * @par 注册DFX查询结果回调函数。
 * @attention 管理节点下发。
 * @param [in] func 回调函数。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_dfx_register_callback(const sle_mesh_dfx_callback_t *func);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* END of SLE_MESH_SDK_DFX_H */