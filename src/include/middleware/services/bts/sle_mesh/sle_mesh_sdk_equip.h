/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle_mesh_sdk_equip.h sdk设备管理模块业务处理接口
 */
#ifndef SLE_MESH_SDK_EQUIP_H
#define SLE_MESH_SDK_EQUIP_H
#include "errcode.h"
#include "sle_common.h"
#include "sle_mesh_errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  SLE mesh network device name.
 * @else
 * @brief  SLE Mesh 网络设备名称。
 * @endif
 */
#define SLE_MESH_DEVICE_NAME_LEN         12

/**
 * @if Eng
 * @brief  SLE mesh network device appearance information length.
 * @else
 * @brief  SLE Mesh 网络设备外观信息长度。
 * @endif
 */
#define SLE_MESH_DEVICE_APPEARANCE_LEN   3

/**
 * @if Eng
 * @brief  SLE mesh network maximum network ID..
 * @else
 * @brief  SLE Mesh 最大网络ID。
 * @endif
 */
#define SLE_MESH_MAXIMUM_NET_ID          15

/**
 * @if Eng
 * @brief  SLE mesh network role type.
 * @else
 * @brief  SLE Mesh 网络角色。
 * @endif
 */
typedef enum {
    SLE_MESH_ROLE_TYPE_MT = 0,                  /*!< @if Eng SLE mesh network role type: Mesh Terminal.
                                                    @else mesh网络角色：终端类设备 @endif */
    SLE_MESH_ROLE_TYPE_MF = 1,                  /*!< @if Eng SLE mesh network role type: Mesh relay Forwarding.
                                                    @else mesh网络角色：中继转发类设备 @endif */
    SLE_MESH_ROLE_TYPE_MA = 2,                  /*!< @if Eng SLE mesh network role type: Mesh Administration.
                                                    @else mesh网络角色：管理类设备 @endif */
    SLE_MESH_ROLE_TYPE_INVALID = 3,             /*!< @if Eng SLE mesh network role type: invalid.
                                                    @else mesh网络角色：非法角色 @endif */
} sle_mesh_role_type_t;

/**
 * @if Eng
 * @brief SLE mesh node network access policy.
 * @else
 * @brief sle mesh 结点入网策略。
 * @endif
 */
typedef enum {
    SLE_MESH_NET_SELF_ACCESS_POLICY = 0x1,                 /*!< @if Eng SLE mesh node decides to join the network on its own.
                                                               @else 节点自主入网方式 @endif */
    SLE_MESH_NET_REQUEST_ACCESS_POLICY = 0x02,              /*!< @if Eng SLE node sends a request to join the network.
                                                                @else 节点请求入网方式 @endif */
    SLE_MESH_NET_CENTRALIZED_CONTROL_ACCESS_POLICY = 0x03,  /*!< @if Eng Centralized control of all nodes to access the network.
                                                                @else 节点集中控制入网方式 @endif */
    SLE_MESH_NET_ACCESS_POLICY_BUTT,
} sle_mesh_net_access_policy_t;

/**
 * @if Eng
 * @brief SLE mesh node networking topology structure.
 * @else
 * @brief sle mesh 结点组网拓扑结构类型。
 * @endif
 */
typedef enum {
    SLE_MESH_NET_STAR_TOPOLOGY_TYPE = 0x1,  /*!< @if Eng Form a star topology network structure.
                                                @else 星形拓扑网络类型 @endif */
    SLE_MESH_NET_TREE_TOPOLOGY_TYPE = 0x02, /*!< @if Eng Form a tree topology network structure.
                                                @else 树形拓扑网络类型 @endif */
    SLE_MESH_NET_NET_TOPOLOGY_TYPE = 0x03,  /*!< @if Eng Forming a net topology network structure.
                                                @else 网状拓扑网络类型 @endif */
    SLE_MESH_NET_TOPOLOGY_TYPE_BUTT,
} sle_mesh_net_topo_type_t;

/**
 * @if Eng
 * @brief  SLE mesh network access point lock.
 * @else
 * @brief  SLE Mesh 入网节点锁定。
 * @endif
 */
typedef enum {
    SLE_MESH_NETWORK_ACCESS_POINT_UNLOCK = 0,   /*!< @if Eng SLE mesh network access point unlock.
                                                    @else 入网节点未锁定。 @endif */
    SLE_MESH_NETWORK_ACCESS_POINT_LOCK = 1,     /*!< @if Eng SLE mesh network access point lock.
                                                    @else 入网节点锁定。 @endif */
} sle_mesh_access_lock_t;

/**
 * @if Eng
 * @brief  SLE mesh network access control policy..
 * @else
 * @brief  SLE Mesh 入网控制策略。
 * @endif
 */
typedef enum {
    SLE_MESH_ACCESS_CTRL_MA_AUTO = 0,           /*!< @if Eng Mesh admin control automatic network access.
                                                    @else 管理节点控制自动入网。 @endif */
    SLE_MESH_ACCESS_CTRL_REMOTE = 1,            /*!< @if Eng Remote authentication controls network access.
                                                    @else 远程鉴权控制入网。 @endif */
    SLE_MESH_ACCESS_CTRL_CUSTOM = 2,            /*!< @if Eng Customized strategy network access.
                                                    @else 定制入网控制策略。 @endif */
} sle_mesh_access_ctrl_type_t;

/**
 * @if Eng
 * @brief SLE mesh startup result.
 * @else
 * @brief SLE mesh 启动结果。
 * @endif
 */
typedef enum {
    SLE_MESH_START_SUCCESS = 0,             /*!< @if Eng SLE mesh start success.
                                                @else mesh启动成功。 @endif */
    SLE_MESH_START_FAILED = 1,              /*!< @if Eng SLE mesh start fail.
                                                @else mesh启动失败。 @endif */
} sle_mesh_startup_result_t;

/**
 * @if Eng
 * @brief SLE mesh address filter working mode.
 * @else
 * @brief 地址过滤器工作模式。
 * @endif
 */
typedef enum {
    SLE_MESH_FILTER_MODE_BYPASS = 0,                /*!< @if Eng Bypass mode, no address filtering.
                                                        @else 旁路模式，不进行地址过滤。 @endif */
    SLE_MESH_FILTER_MODE_WHITELIST = 1,             /*!< @if Eng Trustlist mode.
                                                        @else 白名单模式。 @endif */
    SLE_MESH_FILTER_MODE_BLACKLIST = 2,             /*!< @if Eng Blocklist mode.
                                                        @else 黑名单模式。 @endif */
} sle_mesh_filter_work_mode_t;

/**
 * @if Eng
 * @brief  SLE mesh initiates network parameters.
 * @else
 * @brief  SLE Mesh 启动网络参数。
 * @endif
 */
typedef struct {
    uint16_t net_id;                            /*!< @if Eng Mesh network ID, ranging from 0 to 65535
                                                    @else   mesh网络ID，取值0-65535。@endif */
    uint8_t role;                               /*!< @if Eng SLE Mesh network role type { @ref sle_mesh_role_type_t }.
                                                    @else mesh网络角色{ @ref sle_mesh_role_type_t }。 @endif */
    char name[SLE_MESH_DEVICE_NAME_LEN];        /*!< @if Eng SLE mesh network device name.
                                                    @else 网络设备名称。 @endif */
    uint8_t net_max_hop;                        /*!< @if Eng SLE mesh maximum number of network layers.
                                                    @else 网络最大层数。 @endif */
    uint8_t net_access_policy;                  /*!< @if Eng Policy for connecting a unconnected node to the network { @ref sle_mesh_net_access_policy_t }.
                                                    @else 未入网节点接入网络策略 { @ref sle_mesh_net_access_policy_t }。 @endif */
    uint8_t net_topo_type;                      /*!< @if Eng Network Topology { @ref sle_mesh_net_topo_type_t }.
                                                    @else 网络组网拓扑结构 { @ref sle_mesh_net_topo_type_t }。 @endif */
    uint8_t net_auth_algo;                      /*!< @if Eng Network authentication decryption algorithm { @ref sle_mesh_auth_algo_t }.
                                                    @else 网络认证解密算法 { @ref sle_mesh_auth_algo_t }。 @endif */
    uint8_t net_complete_algo;                 /*!< @if Eng Network packet complete protection algorithm { @ref sle_mesh_complete_algo_t }.
                                                    @else 网络数据包完成性保护算法 { @ref sle_mesh_complete_algo_t }。 @endif */
    uint8_t net_con_mode;                      /*!< @if Eng Network node connection modes { @ref sle_mesh_con_mode_t }.
                                                    @else 网络内节点连接方式 { @ref sle_mesh_con_mode_t }。 @endif */
} sle_mesh_net_param_t;

/**
 * @if Eng
 * @brief  SLE mesh network access policy.
 * @else
 * @brief  SLE Mesh 网络策略控制。
 * @endif
 */
typedef struct {
    uint8_t access_ctrl;                    /*!< @if Eng Network access control { @ref sle_mesh_access_ctrl_type_t }.
                                                @else 入网控制策略 { @ref sle_mesh_access_ctrl_type_t }。 @endif */
    uint8_t access_lock;                    /*!< @if Eng SLE mesh network access point locked (MF/MT)
                                                { @ref sle_mesh_access_lock_t}.
                                                @else  入网点锁定（MF/MT）{ @ref sle_mesh_access_lock_t}。 @endif */
} sle_mesh_policy_t;

/**
 * @if Eng
 * @brief  SLE mesh get network node capability.
 * @else
 * @brief  SLE Mesh 获取网络节点能力。
 * @endif
 */
typedef struct {
    uint8_t access_ctrl;                    /*!< @if Eng Network access control strategy
                                                { @ref sle_mesh_access_ctrl_type_t }.
                                                @else 入网控制策略{ @ref sle_mesh_access_ctrl_type_t }。@endif */
    uint32_t node_capability;               /*!< @if Eng SLE mesh node capability { @ref sle_mesh_node_capability_t }.
                                                @else  网络节点能力。 @endif */
    uint16_t route_capability;              /*!< @if Eng SLE mesh node route capability
                                                { @ref sle_mesh_route_capability_t }.
                                                @else 网络路由能力{ @ref sle_mesh_route_capability_t }。@endif */
    uint8_t addr_type;                      /*!< @if Eng SLE mesh node address type
                                                { @ref sle_mesh_network_addr_type_t }.
                                                @else 网络地址类型{ @ref sle_mesh_network_addr_type_t }。@endif */
} sle_mesh_capability_t;

/**
 * @if Eng
 * @brief Callback invoked when SLE mesh network startup completed.
 * @par Callback invoked when SLE mesh network startup completed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] net_id     SLE mesh network id.
 * @param [in] result     SLE mesh startup result { @ref sle_mesh_startup_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_equip.h
 * @see sle_mesh_equip_callback_t
 * @else
 * @brief  mesh网络启动结果回调。
 * @par    mesh网络启动结果回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] net_id     星闪网络ID。
 * @param [in] result     星闪mesh启动结果 { @ref sle_mesh_startup_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_equip.h
 * @see sle_mesh_equip_callback_t
 * @endif
 */
typedef void (*sle_mesh_action_callback)(uint16_t net_id, uint8_t result);

/**
 * @if Eng
 * @brief Callback invoked when read flash data completed during startup.
 * @par Callback invoked when read flash data completed during startup.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] result  Read flash data result { @ref sle_mesh_startup_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_equip.h
 * @see sle_mesh_equip_callback_t
 * @else
 * @brief  Flash启动结果回调。
 * @par    Flash启动结果回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] result  启动时读取flash数据结果 { @ref sle_mesh_startup_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_equip.h
 * @see sle_mesh_equip_callback_t
 * @endif
 */
typedef void(*sle_nv_cache_ready_callback)(uint8_t result);

/*
 * @if Eng
 * @brief Struct of SLE mesh equipment management callback..
 * @else
 * @brief 星闪mesh 设备管理回调。
 * @endif
 */
typedef struct {
    sle_mesh_action_callback action_cbk;        /*!< @if Eng SLE mesh network startup result callback.
                                                     @else mesh网络启动结果回调。 @endif */
    sle_nv_cache_ready_callback nv_cbk;              /*!< @if Eng Read flash data result callback.
                                                          @else  Flash启动结果回调。 @endif */
} sle_mesh_equip_callback_t;

/**
 * @if Eng
 * @brief Set node network policy.
 * @par Description: Set node network policy.
 * @param [in] node_cap   SLE mesh node network policy { @ref sle_mesh_policy_t }.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置网络策略。
 * @par Description: 设置网络策略。
 * @param [in] policy   节点网络策略 { @ref sle_mesh_policy_t }。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_policy(sle_mesh_policy_t *policy);

/**
 * @if Eng
 * @brief Get node network capabilities.
 * @par Description: Get node network capabilities.
 * @param [in] node_cap   SLE mesh node capaility { @ref sle_mesh_capability_t }.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取网络节点能力。
 * @par Description: 获取网络节点能力。
 * @param [in] node_cap   网络节点能力 { @ref sle_mesh_capability_t }。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_capability(sle_mesh_capability_t *node_cap);

/**
 * @if Eng
 * @brief Register equipment management callback function.
 * @par Description: Register equipment management callback function.
 * @param [in] func Callback function.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册mesh设备管理回调函数。
 * @par Description: 注册mesh设备管理回调函数。
 * @param [in] func 回调函数。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_equip_register_callback(sle_mesh_equip_callback_t *func);

/**
 * @if Eng
 * @brief Add the device list to the network access whitelist.
 * @par Description: Add the device list to the network access whitelist.
 * @param [in] device_num   Access whitelist device number.
 * @param [in] device_list  Access whitelist device address list.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  添加入网设备白名单。
 * @par Description: 添加入网设备白名单。
 * @param [in] device_num   入网白名单设备数。
 * @param [in] device_list  入网白名单设备地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_add_white_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Delete the device list from the network access whitelist.
 * @par Description: Delete the device list from the network access whitelist.
 * @param [in] device_num   Number of devices to be removed from the whitelist.
 * @param [in] device_list  List of device addresses to be removed from the whitelist.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  删除入网设备白名单。
 * @par Description: 删除入网设备白名单。
 * @param [in] device_num   待删除入网白名单设备数。
 * @param [in] device_list  待删除入网白名单设备地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_del_white_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Query the device list to the network access whitelist.
 * @par Description: Query the device list to the network access whitelist.
 * @param [in] device_num   Query result of the number of devices on the whitelist.
 * @param [in] device_list  Query result of the whitelisted device address list.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询入网设备白名单。
 * @par Description: 查询入网设备白名单。
 * @param [in] device_num   入网白名单设备数查询结果。
 * @param [in] device_list  入网白名单设备地址列表查询结果。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_query_white_list(uint16_t *device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Reset the device list to the network access whitelist.
 * @par Description: Reset the device list to the network access whitelist.
 * @param [in] device_num   Number of devices to be removed from the whitelist.
 * @param [in] device_list  List of device addresses to be removed from the whitelist.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  重置入网设备白名单。
 * @par Description: 重置入网设备白名单。
 * @param [in] device_num   入网白名单设备数。
 * @param [in] device_list  入网白名单设备地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_reset_white_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Add the device media access control address to the filter.
 * @par Description: Add the device media access control address to the filter.
 * @param [in] device_num   Number of devices added to filter.
 * @param [in] device_list  List of devices address added to filter.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  添加设备MAC地址过滤器。
 * @par Description: 添加设备MAC地址过滤器。
 * @param [in] device_num   待添加设备数。
 * @param [in] device_list  待添加设备MAC地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_add_filter_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Delete the device media access control address from filter.
 * @par Description: Delete the device media access control address from filter.
 * @param [in] device_num   Number of devices delete from filter.
 * @param [in] device_list  List of devices address delete from filter.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  添加设备MAC地址过滤器。
 * @par Description: 添加设备MAC地址过滤器。
 * @param [in] device_num   待添加设备数。
 * @param [in] device_list  待添加设备MAC地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_del_filter_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Query the list of device media access control address for filter devices.
 * @par Description: Query the list of device media access control address for filter devices.
 * @param [inout] device_num   Number of devices delete from filter.
 * @param [inout] device_list  List of devices address delete from filter.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询过滤器设备MAC地址列表。
 * @par Description: 查询过滤器设备MAC地址列表。
 * @param [inout] device_num   待添加设备数。
 * @param [inout] device_list  待添加设备MAC地址列表。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_query_filter_list(uint16_t *device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Reset the device media access control address for filter.
 * @par Description: Reset the device media access control address for filter.
 * @param [in] device_num   Number of devices reset for filter.
 * @param [in] device_list  List of devices address reset for filter.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  重置设备MAC地址过滤器。
 * @par Description: 重置设备MAC地址过滤器。
 * @param [in] device_num   待添加设备数。
 * @param [in] device_list  待添加设备MAC地址列表。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_reset_filter_list(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Set the MAC address filter working mode.
 * @par Description: Set the MAC address filter working mode.
 * @param [in] mode  Address filter working mode { @ref sle_mesh_filter_work_mode_t }.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置MAC地址过滤器工作模式。
 * @par Description: 设置MAC地址过滤器工作模式。
 * @param [in] mode   地址过滤器工作模式 { @ref sle_mesh_filter_work_mode_t }。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_filter_work_mode(uint8_t mode);

/**
 * @if Eng
 * @brief Invite new devices to join the network.
 * @par Description: Invite new devices to join the network.
 * @param [in] device_num   Number of devices to be invited.
 * @param [in] device_list  List of devices to be invited.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  邀请新设备入网。
 * @par Description: 邀请新设备入网。
 * @param [in] device_num   待邀请设备数。
 * @param [in] device_list  待邀请设备MAC地址列表。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_invite_join(uint16_t device_num, sle_addr_t *device_list);

/**
 * @if Eng
 * @brief Startup SLE mesh network.
 * @par Description: Startup SLE mesh network.
 * @param [in] net_param   SLE mesh network parameters.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  启动星闪mesh网络。
 * @par Description: 启动星闪mesh网络。
 * @param [in] net_param   星闪mesh网络参数。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_start(sle_mesh_net_param_t *net_param);

/**
 * @if Eng
 * @brief Get SLE mesh network parameter.
 * @par Description: Get SLE mesh network parameter.
 * @param [out] net_param   SLE mesh network parameters.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取星闪mesh网络参数。
 * @par Description: 获取星闪mesh网络参数。
 * @param [out] net_param   星闪mesh网络参数。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_net_param(sle_mesh_net_param_t *net_param);

/**
 * @if Eng
 * @brief Exit the SLE mesh network.
 * @par Description: Exit the SLE mesh network.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  退出星闪mesh网络。
 * @par Description: 退出星闪mesh网络。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif