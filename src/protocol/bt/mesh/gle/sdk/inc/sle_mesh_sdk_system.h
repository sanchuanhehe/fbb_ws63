/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle mesh system init & ota.
 */
#ifndef SLE_MESH_SDK_SYSTEM_H
#define SLE_MESH_SDK_SYSTEM_H

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
 * @brief  SLE mesh network device appearance information length.
 * @else
 * @brief  SLE Mesh 网络设备外观信息长度。
 * @endif
 */
#define SLE_MESH_DEVICE_APPEARANCE_LEN   3

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
 * @brief SLE mesh node routing protocol capability.
 * @else
 * @brief 星闪mesh节点路由协议能力。
 * @endif
 */
typedef enum {
    SLE_MESH_ROUTE_CAP_AODV = 1,        /*!< @if Eng Ad-hoc On-Demand Distance Vector routing capabilities.
                                            @else AODV路由能力 @endif */
    SLE_MESH_ROUTE_CAP_RPL = 2,         /*!< @if Eng Routing Protocol for Low-Power routing capabilities.
                                            @else RPL路由能力 @endif */
    SLE_MESH_ROUTE_CAP_RIP = 4,         /*!< @if Eng Routing Information Protocol routing capabilities.
                                            @else RIP路由能力 @endif */
    SLE_MESH_ROUTE_CAP_PRIVATE_RIP = 8, /*!< @if Eng Private Routing Information Protocol routing capabilities.
                                            @else 私有RIP路由能力 @endif */
} sle_mesh_route_capability_t;

/**
 * @if Eng
 * @brief SLE mesh network address allocation agent.
 * @else
 * @brief 星闪mesh网络地址分配代理。
 * @endif
 */
typedef enum {
    SLE_MESH_ADDR_TYPE_UNPROXY = 0,     /*!< @if Eng Non-proxy address.
                                            @else 非代理地址 @endif */
    SLE_MESH_ADDR_TYPE_PROXY = 1,       /*!< @if Eng Proxy address.
                                            @else 代理地址 @endif */
} sle_mesh_network_addr_type_t;

/**
 * @if Eng
 * @brief SLE mesh node network capability bitmap.
 * @else
 * @brief 星闪mesh节点能力位图。
 * @endif
 */
typedef enum {
    SLE_MESH_NODE_CAP_ADDR_ASSIGN = 1,          /*!< @if Eng Address allocation capability.
                                                    @else 地址分配能力 @endif */
    SLE_MESH_NODE_CAP_ADDR_ASSIGN_AGENT = 2,    /*!< @if Eng Address allocation agent capability.
                                                    @else 地址分配代理能力 @endif */
    SLE_MESH_NODE_CAP_MESSAGE_CACHE = 4,        /*!< @if Eng Network packet buffering capability.
                                                    @else 网络报文缓存能力 @endif */
    SLE_MESH_NODE_CAP_MESSAGE_FORWARDER = 8,    /*!< @if Eng Network packet forwarding capability.
                                                    @else 网络报文转发能力 @endif */
    SLE_MESH_NODE_CAP_ROLE_TRANSITION = 16,     /*!< @if Eng Network role transition (promotion & demotion) capability.
                                                    @else 网络角色转换（升降级）能力 @endif */
} sle_mesh_node_capability_t;

/**
 * @if Eng
 * @brief SLE mesh node connect capability bitmap.
 * @else
 * @brief 星闪mesh节点能力位图。
 * @endif
 */
typedef enum {
    SLE_MESH_CONNECT_CAPABILITY_LINK = 1,       /*!< @if Eng Node connectivity capability: link.
                                                    @else 节点连接能力：链路 @endif */
    SLE_MESH_CONNECT_CAPABILITY_BROADCAST = 2,  /*!< @if Eng Node connectivity capability: broadcast.
                                                    @else 节点连接能力：广播 @endif */
} sle_mesh_connect_capability_t;

/**
 * @if Eng
 * @brief SLE mesh node address allocation policy.
 * @else
 * @brief sle mesh 结点地址分配策略。
 * @endif
 */
typedef enum {
    SLE_MESH_NODE_DYNAMIC_ADDR_ALLOC = 0x1,     /*!< @if Eng Dynamic address allocation policy for mesh nodes..
                                                    @else 节点地址动态分配策略 @endif */
    SLE_MESH_NODE_STATIC_ADDR_ALLOC = 0x2,      /*!< @if Eng Static address allocation policy for mesh nodes.
                                                    @else 节点地址静态分配策略 @endif */
    SLE_MESH_NODE_SIMPLE_TREE_ADDR_ALLOC = 0x4, /*!< @if Eng Simple tree allocation policy for mesh node addresses.
                                                    @else 节点地址简单树分配策略 @endif */
    SLE_MESH_NODE_ADDR_ACCIGN_POLICY_BUTT,
} sle_mesh_node_addr_assign_policy_t;

/**
 * @if Eng
 * @brief SLE mesh communication mode between nodes.
 * @else
 * @brief sle mesh 结点间通信方式。
 * @endif
 */
typedef enum {
    SLE_MESH_ASYNCHRONOUS_LINK_MODE = 0x1,      /*!< @if Eng The neighboring node uses asynchronous links for communication.
                                                    @else 邻居节点使用异步链路通信 @endif */
    SLE_MESH_SYNCHRONOUS_BROADCAST_MODE = 0x2,  /*!< @if Eng Neighboring nodes use synchronous broadcast communication.
                                                    @else 邻居节点使用同步广播通信 @endif */
    SLE_MESH_CON_MODE_BUTT,
} sle_mesh_con_mode_t;

/**
 * @if Eng
 * @brief SLE mesh system initialization parameter.
 * @else
 * @brief SLE mesh初始化参数
 * @endif
 */
typedef struct {
    uint8_t node_cap;           /*!< @if Eng Node capability bitmap { @ref sle_mesh_node_capability_t }.
                                    @else 节点能力位图 { @ref sle_mesh_node_capability_t } @endif */
    uint8_t addr_type;          /*!< @if Eng Address type { @ref sle_mesh_network_addr_type_t }.
                                    @else 地址类型 { @ref sle_mesh_network_addr_type_t } @endif */
    uint16_t route_cap;         /*!< @if Eng Mesh node routing capability { @ref sle_mesh_route_capability_t }.
                                    @else 网络节点路由能力 { @ref sle_mesh_route_capability_t } @endif */
    uint8_t auth_algo;          /*!< @if Eng authentication and encryption algorithm type { @ref sle_mesh_auth_algo_t }.
                                    @else 认证加密算法 { @ref sle_mesh_auth_algo_t } @endif */
    uint8_t complete_algo;      /*!< @if Eng integrity protection algorithm type { @ref sle_mesh_complete_algo_t }.
                                    @else 完整性保护算法 { @ref sle_mesh_complete_algo_t } @endif */
    uint8_t key_nego_algo;      /*!< @if Eng Key negotiation algorithm type { @ref sle_mesh_key_nego_algo_t }.
                                    @else 密钥协商算法类型 { @ref sle_mesh_key_nego_algo_t } @endif */
    uint8_t key_derivate_algo;  /*!< @if Eng Key derivation algorithm type { @ref sle_mesh_key_derivation_algo_t }.
                                    @else 密钥派生算法类型 { @ref sle_mesh_key_derivation_algo_t } @endif */
    uint8_t node_con_mode;      /*!< @if Eng Network node connection modes { @ref sle_mesh_con_mode_t }.
                                    @else 网络内节点连接方式 { @ref sle_mesh_con_mode_t }。 @endif */
    uint8_t node_appearance[SLE_MESH_DEVICE_APPEARANCE_LEN];
                                /*!< @if Eng SLE mesh network device appearance.
                                    @else 网络设备外观。 @endif */
} sle_mesh_init_param_t;

/**
 * @if Eng
 * @brief SLE mesh initialization.
 * @par Description: SLE mesh initialization.
 * @param  [in] param initialization parameter information
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  星闪mesh初始化。
 * @par Description: 星闪mesh初始化。
 * @param  [in]  param  初始化参数信息。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_init(sle_mesh_init_param_t *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* END of SLE_MESH_SDK_SYSTEM_H */