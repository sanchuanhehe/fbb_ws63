/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle_mesh_sdk_network.h sdk网络管理模块业务处理接口
 */
#ifndef SLE_MESH_SDK_NETWORK_H
#define SLE_MESH_SDK_NETWORK_H

#include "errcode.h"
#include "sle_common.h"
#include "sle_mesh_sdk_equip.h"
#include "sle_ssap_stru.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  Maximum duration for role switching timeout, in minutes.
 * @else
 * @brief  角色切换超时最大时长，单位：分钟。
 * @endif
 */
#define SLE_MESH_ROLE_SWITCH_MAX_TIME           60

/**
 * @if Eng
 * @brief SLE mesh node status.
 * @else
 * @brief 星闪mesh节点状态。
 * @endif
 */
typedef enum {
    SLE_MESH_NODE_STATE_INIT,               /*!< @if Eng Initial state.
                                                @else 初始态。 @endif */
    SLE_MESH_NODE_STATE_UNCONNECTED,        /*!< @if Eng Disconnected state.
                                                @else 未连接态。 @endif */
    SLE_MESH_NODE_STATE_CONNECTING,         /*!< @if Eng Connecting state.
                                                @else 正在连接。 @endif */
    SLE_MESH_NODE_STATE_CONNECTED,          /*!< @if Eng Connected state.
                                                @else 已连接。 @endif */
    SLE_MESH_NODE_STATE_ONLINE,             /*!< @if Eng Activated online state.
                                                @else 已在网激活。 @endif */
    SLE_MESH_NODE_STATE_OFFLINE,            /*!< @if Eng Offline state.
                                                @else 离线状态。 @endif */
    SLE_MESH_NODE_STATE_DISCONNECTING,      /*!< @if Eng disconnecting state.
                                                @else 连接正在断开状态。 @endif */
    SLE_MESH_NODE_STATE_DISCONNECTED,       /*!< @if Eng Disconnected state.
                                                @else 连接断开。 @endif */
    SLE_MESH_NODE_STATE_BUTT,               /*!< @if Eng node state butt.
                                                @else 结点状态边界。 @endif */
} sle_mesh_node_state_t;

/**
 * @if Eng
 * @brief SLE mesh address assign type bitmap.
 * @else
 * @brief 星闪地址分配类型位图。
 * @endif
 */
typedef enum {
    SLE_MESH_ADDRESS_ASSIGN_DYNC = 1,       /*!< @if Eng Dynamic address allocation.
                                                @else 动态地址分配。 @endif */
    SLE_MESH_ADDRESS_ASSIGN_STATIC = 2,     /*!< @if Eng Static address allocation.
                                                @else 静态地址分配。 @endif */
    SLE_MESH_ADDRESS_ASSIGN_TREE = 4,       /*!< @if Eng Tree-shaped address allocation.
                                                @else 树形地址分配。 @endif */
} sle_mesh_address_assign_t;

/**
 * @if Eng
 * @brief Device discovery results.
 * @else
 * @brief 设备发现结果。
 * @endif
 */
typedef struct {
    uint8_t addr[SLE_ADDR_LEN];                 /*!< @if Eng SLE device media access control address.
                                                   @else 设备MAC地址。 @endif */
    uint8_t role;                               /*!< @if Eng SLE mesh network role { @ref sle_mesh_role_type_t }.
                                                   @else 设备网络角色 { @ref sle_mesh_role_type_t }。 @endif */
    int8_t rssi;                                /*!< @if Eng Equipment signal quality.
                                                   @else 设备信号质量。 @endif */
} sle_mesh_discovery_result_t;

/**
 * @if Eng
 * @brief SLE mesh network node information.
 * @else
 * @brief 星闪mesh网络节点信息。
 * @endif
 */
typedef struct {
    uint16_t mesh_id;                       /*!< @if Eng SLE mesh network address.
                                                @else 网络地址。 @endif */
    uint16_t net_id;                        /*!< @if Eng SLE mesh network ID.
                                                @else 网络ID。 @endif */
    uint8_t addr[SLE_ADDR_LEN];             /*!< @if Eng SLE device media access control address.
                                                @else 设备MAC地址。 @endif */
    sle_uuid_t uuid;                        /*!< @if Eng SLE device appearance UUID.
                                                @else 设备外观UUID。 @endif */
    uint8_t role;                           /*!< @if Eng SLE mesh network role { @ref sle_mesh_role_type_t }.
                                                @else 设备网络角色 { @ref sle_mesh_role_type_t }。 @endif */
    uint8_t state;                          /* !< @if Eng SLE mesh node status { @ref sle_mesh_node_state_t }.
                                                @else 网络节点状态 { @ref sle_mesh_node_state_t }。 @endif */
    uint8_t connect_cap;                    /* !< @if Eng SLE mesh node connect capability bitmap
                                                    { @ref sle_mesh_connect_capability_t }.
                                                @else 节点连接能力位图 { @ref sle_mesh_connect_capability_t }。 @endif */
    uint32_t node_cap;                      /*!< @if Eng Node capability bitmap { @ref sle_mesh_node_capability_t }.
                                                @else 节点能力位图 { @ref sle_mesh_node_capability_t } @endif */
    uint8_t name[SLE_MESH_DEVICE_NAME_LEN]; /*!< @if Eng SLE mesh network device name.
                                                @else 网络设备名称。 @endif */
    uint8_t address_assign;                 /*!< @if Eng SLE mesh address assign { @ref sle_mesh_address_assign_t }.
                                                @else 地址分配方式 { @ref sle_mesh_address_assign_t }。 @endif */
    uint8_t neighbor_count;                 /*!< @if Eng Number of neighbors.
                                                @else 邻居数量。 @endif */
    uint16_t *neighbor_list;                /*!< @if Eng List of neighbor network addresses.
                                                @else 邻居网络地址列表。 @endif */
    uint8_t group_count;                    /*!< @if Eng Number of group information.
                                                @else 组信息数量。 @endif */
    uint16_t *group_list;                   /*!< @if Eng List of group information.
                                                @else 组信息列表。 @endif */
} sle_mesh_node_info_t;

/**
 * @if Eng
 * @brief Callback invoked when device discovery result reporting.
 * @par Callback invoked when device discovery result reporting.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] device_num    Number of devices found in the discovery result.
 * @param [in] result       Device discovery result { @ref sle_mesh_discovery_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @else
 * @brief  设备发现结果上报回调。
 * @par    设备发现结果上报回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] device_num  设备发现结果设备数。
 * @param [in] result      设备发现结果 { @ref sle_mesh_discovery_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @endif
 */
typedef void (*sle_mesh_discovery_callback)(uint16_t device_num, sle_mesh_discovery_result_t *result);

/**
 * @if Eng
 * @brief Callback invoked when SLE mesh device information query results reporting.
 * @par Callback invoked when SLE mesh device information query results reporting.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @attention 3.The device information will reported in batches and multiple cycles.
 * @param [in] device_num Query node info device number.
 * @param [in] node_info  SLE mesh network node info { @ref sle_mesh_node_info_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @else
 * @brief  网内设备信息查询结果上报。
 * @par    网内设备信息查询结果上报。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @attention  3. 数据分批次、多次循环上报。
 * @param [in] device_num  设备数量。
 * @param [in] node_info   节点信息 { @ref sle_mesh_node_info_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @endif
 */
typedef void (*sle_mesh_query_node_callback)(uint16_t device_num, sle_mesh_node_info_t *node_info);

/**
 * @if Eng
 * @brief SLE mesh access result.
 * @else
 * @brief SLE mesh 入网结果。
 * @endif
 */
typedef enum {
    SLE_MESH_ENROLL_SUCCESS = 0,                 /*!< @if Eng SLE mesh network access success.
                                                     @else 入网成功。 @endif */
    SLE_MESH_ENROLL_NO_RESOURCE = 1,             /*!< @if Eng Insufficient network ID resources.
                                                     @else 网络地址资源不足。 @endif */
    SLE_MESH_ENROLL_ENROLLING = 2,               /*!< @if Eng Device enrolling to the network.
                                                     @else 正在入网。 @endif */
    SLE_MESH_ENROLL_ALREADY_ONLINE = 3,          /*!< @if Eng Device already online.
                                                     @else 设备已在网。 @endif */
    SLE_MESH_ENROLL_INTERNAL_ERROR = 4,          /*!< @if Eng Network internal error.
                                                     @else 内部错误。 @endif */
    SLE_MESH_ENROLL_MESH_ID_CONFLICT = 5,        /*!< @if Eng SLE mesh network address conflict.
                                                     @else 网络地址冲突。 @endif */
    SLE_MESH_ENROLL_MESH_UNAUTH = 6,             /*!< @if Eng The device is not authenticated.
                                                     @else 设备未鉴权。 @endif */
    SLE_MESH_ENROLL_MESH_TIMEOUT = 7,            /*!< @if Eng The network access processing times out.
                                                     @else 入网处理超时。 @endif */
    SLE_MESH_ENROLL_OFFLINE = 8,                 /*!< @if Eng Device already offline.
                                                     @else 设备离网。 @endif */
    SLE_MESH_ENROLL_SAME_ADDR_DENIED = 9,        /*!< @if Eng Forbidding the nodes with the same MAC address
                                                     @else 结点入网过程发现相同mac地址结点禁止入网。 @endif */
    SLE_MESH_ENROLL_CREATE_LINK_TIMEOUT = 0xA,   /*!< @if Eng Create link timed out during node network access
                                                     @else  结点入网过程中连接超时。 @endif */
    SLE_MESH_ENROLL_SECURITY_AUTH_TIMEOUT = 0xB, /*!< @if Eng Security verification timed out
                                                     @else  结点入网过程中安全校验超时。 @endif */
    SLE_MESH_ENROLL_CREATE_LINK_FAIL  = 0xC,     /*!< @if Eng Create link fail during node network access
                                                      @else  结点入网过程中建连失败。 @endif */
} sle_mesh_access_reason_t;

/**
 * @if Eng
 * @brief Callback invoked when SLE mesh device access net.
 * @par Callback invoked when SLE mesh device access net.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [out] mac_addr          SLE mesh node mac address.
 * @param [out] mesh_id           SLE mesh assign network address.
 * @param [out] node_state        SLE mesh node state { @ref sle_mesh_node_state_t }.
 * @param [out] reason            SLE mesh access reason { @ref sle_mesh_access_reason_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @else
 * @brief  设备网络状态回调。
 * @par    设备网络状态回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [out] mac_addr        SLE mesh 入网结点mac地址.
 * @param [out] mesh_id         星闪mesh分配的网络地址。
 * @param [out] node_state      设备状态 { @ref sle_mesh_node_state_t }。
 * @param [out] reason          设备接入原因 { @ref sle_mesh_access_reason_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_network.h
 * @see sle_mesh_network_callback_t
 * @endif
 */
typedef void (*sle_mesh_access_callback)(uint8_t mac_addr[SLE_ADDR_LEN], uint16_t mesh_id,
    sle_mesh_node_state_t node_state, sle_mesh_access_reason_t reason);

/**
 * @if Eng
 * @brief Struct of SLE mesh network management callback function.
 * @else
 * @brief 星闪mesh网络管理回调。
 * @endif
 */
typedef struct {
    sle_mesh_discovery_callback disc_cbk;       /*!< @if Eng Callback for reporting device discovery results.
                                                    @else  设备发现结果上报回调。 @endif */
    sle_mesh_query_node_callback query_cbk;     /*!< @if Eng Query results are reported in batches.
                                                    @else 查询结果上报，分批次上报。 @endif */
    sle_mesh_access_callback access_cbk;        /*!< @if Eng SLE mesh device net state callback.
                                                    @else  设备网络状态回调。 @endif */
} sle_mesh_network_callback_t;

/**
 * @if Eng
 * @brief Register network management callback function.
 * @par Description: Register network management callback function.
 * @attention Synchronous processing on admin or forwarding node.
 * @param [in] func Callback function.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册网络管理回调函数。
 * @par Description: 注册网络管理回调函数。
 * @attention 管理、转发节点同步请求。
 * @param [in] func 回调函数。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_network_register_callback(sle_mesh_network_callback_t *func);

/**
 * @if Eng
 * @brief Get the total number of devices within the network.
 * @par Description: Get the total number of devices within the network.
 * @attention Synchronous processing on admin node.
 * @param [inout] node_count Total number of devices.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取网内设备总数。
 * @par Description: 获取网内设备总数。
 * @attention 管理节点同步执行。
 * @param [inout] node_count 设备总数。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_node_num(uint16_t *node_count);

/**
 * @if Eng
 * @brief Obtain information about all nodes on the network.
 * @par Description: Obtain information about all nodes on the network.
 * @attention Asynchronous processing on admin node,
 *          reported via the query_cbk callback in { @ref sle_mesh_network_callback_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取全网节点信息。
 * @par Description: 获取全网节点信息。
 * @attention 管理节点异步执行，通过 { @ref sle_mesh_network_callback_t } 中的 query_cbk 回调上报。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_all_node_info(void);

/**
 * @if Eng
 * @brief Enable the reporting of device discovery results.
 * @par Description: Enable the reporting of device discovery results.
 * @attention Asynchronous processing on admin or forwarding node,
 *  which reports the query result through the disc_cbk callback { @ref sle_mesh_network_callback_t }.
 * @param [in] net_id Network ID of the discovered device.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  使能设备发现结果上报。
 * @par Description: 使能设备发现结果上报。
 * @attention 管理、转发节点异步请求，通过 { @ref sle_mesh_network_callback_t } disc_cbk 回调函数上报查询结果。
 * @param [in] net_id 设备发现的网络ID。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_enable_discovery_report(uint16_t net_id);

/**
 * @if Eng
 * @brief Disable the reporting of device discovery results.
 * @par Description: Disable the reporting of device discovery results.
 * @attention Asynchronous processing on admin or forwarding node.
 * @param [in] net_id Network ID of the discovered device.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  禁用设备发现结果上报。
 * @par Description: 禁用设备发现结果上报。
 * @attention 管理、转发节点异步请求。
 * @param [in] net_id 设备发现的网络ID。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_disable_discovery_report(uint16_t net_id);

/**
 * @if Eng
 * @brief Query the device discovery results.
 * @par Description: Query the device discovery results.
 * @attention Synchronous processing on admin or forwarding node.
 * @param [inout] device_number  Number of device discovery results.
 * @param [inout] result         Device information list of discovery results.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询设备发现结果。
 * @par Description: 查询设备发现结果。
 * @attention 管理、转发节点同步请求。
 * @param [inout] device_number  设备发现结果，设备数量。
 * @param [inout] result         设备发现结果，设备信息列表。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_discovery_result(uint16_t *device_number, sle_mesh_discovery_result_t *result);

/**
 * @if Eng
 * @brief Obtain the reason for the device going offline.
 * @par Description: Obtain the reason for the device going offline.
 * @attention Asynchronous processing on admin node.
 * @param [in]    mesh_id Target device network id.
 * @param [inout] reason Target device offline reason.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取设备离网原因。
 * @par Description: 获取设备离网原因。
 * @attention 管理节点异步执行。
 * @param [in]    mesh_id 目标设备网络地址。
 * @param [inout] reason 目标设备离网原因。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_offline_reason(uint16_t mesh_id, uint8_t *reason);

/**
 * @if Eng
 * @brief Set the timeout for node role switching.
 * @par Description: Set the timeout for node role switching.
 * @attention Synchronous processing on admin node.
 * @param [inout] time Timeout interval for node role switching, in minutes. The value range is [1, 60].
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置节点角色切换超时时间。
 * @par Description: 设置节点角色切换超时时间。
 * @attention 管理节点同步执行。
 * @param [inout] time 节点角色切换超时时间，单位：分钟，取值范围：[1, 60]。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_role_switch_time(uint8_t time);

/**
 * @if Eng
 * @brief Delete all nodes of the entire network.
 * @par Delete all nodes of the entire network.
 * @attention 1、Execute on admin node.
 *            2. Please proceed with caution. After this API is executed,
 *               all Mesh nodes within the network will disconnect from the network.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  删除全网所有节点。
 * @par 删除全网所有节点。
 * @attention 1、在MA上执行。
 *            2、请谨慎执行，执行此指令后，网络内所有Mesh节点将离网。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_delete_all_node(void);

/**
 * @if Eng
 * @brief Delete the specified node.
 * @par Delete the specified node.
 * @attention 1、Execute on admin node.
 *            2. Please proceed with caution. After this API is executed,
 *               the specified node will be disconnected from the network.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  删除指定节点。
 * @par 删除指定节点。
 * @attention 1、在MA上执行。
 *            2、请谨慎执行，执行此指令后，指定的节点将离网。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_delete_node(uint8_t addr[SLE_ADDR_LEN], uint8_t addr_len);

/**
 * @if Eng
 * @brief Get the maximum number of network layers.
 * @par Description: Get the maximum number of network layers.
 * @param [in] max_layer  Maximum number of layers for dynamic routing,value range: [1, 15].
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取网络最大层数。
 * @par Description: 获取网络最大层数。
 * @param [in] max_layer  网络最大层数。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_network_layers(uint8_t *max_layer);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif