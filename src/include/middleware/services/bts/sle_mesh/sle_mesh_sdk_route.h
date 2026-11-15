/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle_mesh_sdk_route.h sdk路由管理模块业务处理接口
 */
#ifndef SLE_MESH_SDK_ROUTE_H
#define SLE_MESH_SDK_ROUTE_H

#include "errcode.h"
#include "sle_mesh_errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief state of route.
 * @else
 * @brief 路由状态信息定义。
 * @endif
 */
typedef enum {
    SLE_MESH_ROUTE_STATE_ACTIVE,    /*!< @if Eng active state of route
                                         @else   路由激活状态。@endif */
    SLE_MESH_ROUTE_STATE_INACT,     /*!< @if Eng inactive state of route
                                         @else   路由老化状态。@endif */
    SLE_MESH_ROUTE_STATE_LOSS,      /*!< @if Eng loss state of route
                                         @else   路由断开状态。@endif */
    SLE_MESH_ROUTE_STATE_BUTT       /*!< @if Eng butt state of route
                                         @else   路由状态边界。@endif */
} sle_mesh_route_state_t;

/**
 * @if Eng
 * @brief SLE mesh node routing table.
 * @else
 * @brief 节点路由表。
 * @endif
 */
typedef struct {
    uint16_t dest_addr;                 /*!< @if Eng Destination address of route.
                                            @else   路由目的地址。@endif */
    uint16_t next_hop;                  /*!< @if Eng Next hop of route.
                                            @else   路由下一跳地址。 @endif */
    uint8_t state;                      /*!< @if Eng State of route { @ref sle_mesh_route_state_t }.
                                            @else   路由状态。{ @ref sle_mesh_route_state_t }。 @endif */
    uint8_t reserve;                    /*!< @if Eng Reserved field.
                                            @else 保留字段。 @endif */
} sle_mesh_route_table_t;


/**
 * @if Eng
 * @brief Initialize SLE mesh static route.
 * @par Description: Initialize SLE mesh static route.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  初始化星闪Mesh静态路由。
 * @par Description: 初始化星闪Mesh静态路由。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_static_route_init(void);

/**
 * @if Eng
 * @brief Deinitialize SLE mesh static route.
 * @par Description: Deinitialize SLE mesh static route.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  去初始化星闪Mesh静态路由。
 * @par Description: 去初始化星闪Mesh静态路由。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_static_route_deinit(void);

/**
 * @if Eng
 * @brief Add static routing items.
 * @par Description: Add static routing items.
 * @param [in] mesh_id  Destination device network address.
 * @param [in] next_hop Next-hop device network address.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  添加静态路由表项。
 * @par Description: 添加静态路由表项。
 * @param [in] mesh_id  目的设备网络地址。
 * @param [in] next_hop 下一跳设备网络地址。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_add_static_route(uint16_t mesh_id, uint16_t next_hop);

/**
 * @if Eng
 * @brief Delete static routing items.
 * @par Description: Delete static routing items.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  删除所有静态路由表项。
 * @par Description: 删除所有静态路由表项。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_del_all_static_route(void);

/**
 * @if Eng
 * @brief Delete single static routing items.
 * @par Description: Delete single static routing items.
 * @param [in] mesh_id  Destination device network address.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  删除单个静态路由表项。
 * @par Description: 删除单个静态路由表项。
 * @param [in] mesh_id  目的设备网络地址。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_del_static_route(uint16_t mesh_id);

/**
 * @if Eng
 * @brief Modify static routing items.
 * @par Description: Modify static routing items.
 * @param [in] mesh_id  Destination device network address.
 * @param [in] next_hop Next-hop device network address.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  修改静态路由表项。
 * @par Description: 修改静态路由表项。
 * @param [in] mesh_id  目的设备网络地址。
 * @param [in] next_hop 下一跳设备网络地址。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_modify_static_route(uint16_t mesh_id, uint16_t next_hop);

/**
 * @if Eng
 * @brief Query static routing table.
 * @par Description: Query static routing table.
 * @param [in]    mesh_id      Destination device network address.
 * @param [inout] route_table  Result of routing table { @ref sle_mesh_route_table_t }.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询节点静态路由表。
 * @par Description: 查询节点静态路由表。
 * @param [in]    dest_addr   目的设备网络地址。
 * @param [inout] route_table 路由表查询结果 { @ref sle_mesh_route_table_t }。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_query_static_route(uint16_t dest_addr, sle_mesh_route_table_t *route_table);

/**
 * @if Eng
 * @brief Get the number of routing tables.
 * @par Description: Get the number of routing tables.
 * @param [inout] route_count  Result of routing tables number.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取路由表数量。
 * @par Description: 获取路由表数量。
 * @param [inout] route_count  路由表数量查询结果。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_static_route_count(uint32_t *route_count);

/**
 * @if Eng
 * @brief Update the status of static routes.
 * @par Description: Update the status of static routes.
 * @param [in] mesh_id Address of the static routing table entry
 * @param [in] new_state New state of the routing table entry { @ref sle_mesh_route_state_t }
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  更新静态路由状态。
 * @par Description: 更新静态路由状态。
 * @param [in] mesh_id   静态路由表项目的地址
 * @param [in] new_state 路由表项新状态 { @ref sle_mesh_route_state_t }。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_static_route_update_state(uint16_t mesh_id, uint8_t new_state);

/**
 * @if Eng
 * @brief Set the max hop for dynamic routes.
 * @par Description: Set the max hop for dynamic routes.
 * @param [in] max_hop  Maximum number of hops for dynamic routing,value range: [1, 15].
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置动态路由最大跳数。
 * @par Description: 设置动态路由最大跳数。
 * @param [in] max_hop  动态路由最大跳数，取值范围: [1, 15]。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_dync_route_max_hop(uint8_t max_hop);

/**
 * @if Eng
 * @brief Get the max hop for dynamic routes.
 * @par Description: Get the max hop for dynamic routes.
 * @param [inout] max_hop  Query result of the maximum number of hops for dynamic routing.
 * @attention Synchronous processing on admin or forwarding node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取动态路由最大跳数。
 * @par Description: 获取动态路由最大跳数。
 * @param [inout] max_hop  动态路由最大跳数查询结果。
 * @attention 管理、转发节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_dync_route_max_hop(uint8_t *max_hop);

/**
 * @if Eng
 * @brief set Aging time of dynamic routes.
 * @par Description: set Aging time of dynamic routes.
 * @param [inout] threshold_val  Aging time of dynamic routes,The unit is threshold_val x 5 seconds,value range: [1,15].
 * @attention Run the following commands on each node separately.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  设置动态路由老化门限时间。
 * @par Description: 设置动态路由老化门限时间。
 * @param [inout] threshold_val  动态路由老化门限值，单位threshold_val*5秒，范围[1,15] 。
 * @attention 所有节点单独执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_set_route_inact_threshold(uint8_t threshold_val);

/**
 * @if Eng
 * @brief get Aging time of dynamic routes.
 * @par Description: get Aging time of dynamic routes.
 * @param [inout] threshold_val  Aging time of dynamic routes,The unit is threshold_val x 5 seconds,value range: [1,15].
 * @attention Run the following commands on each node separately.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  获取动态路由老化门限时间。
 * @par Description: 获取动态路由老化门限时间。
 * @param [inout] threshold_val  动态路由老化门限值，单位threshold_val*5秒，范围[1,15] 。
 * @attention 所有节点单独执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_get_route_inact_threshold(uint8_t *threshold_val);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif