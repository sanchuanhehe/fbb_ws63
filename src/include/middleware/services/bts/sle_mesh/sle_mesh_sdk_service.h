/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle_mesh_sdk_service.h
 */

#ifndef SLE_MESH_SDK_SERVICE_H
#define SLE_MESH_SDK_SERVICE_H

#include "errcode.h"
#include "sle_common.h"
#include "sle_ssap_stru.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  Maximum length of service instance description string.
 * @else
 * @brief  服务实例描述最大长度。
 * @endif
 */
#define SLE_MESH_SERVICE_DESCRIPT_LEN                 50

/**
 * @if Eng
 * @brief Service attribute value type.
 * @else
 * @brief 服务属性值类型。
 * @endif
 */
typedef enum {
    SLE_MESH_PROPERTY_VALUE_TYPE_UINT32 = 0,    /*!< @if Eng Service attribute value type: unsigned int.
                                                  @else 服务属性值类型：unsigned int。 @endif */
    SLE_MESH_PROPERTY_VALUE_TYPE_ARRAY = 1,     /*!< @if Eng Service attribute value type: array.
                                                  @else 服务属性值类型：数组。 @endif */
} sle_mesh_property_value_type_t;

/**
 * @if Eng
 * @brief SLE mesh service registration or deletion result.
 * @else
 * @brief 星闪mesh服务注册或删除结果。
 * @endif
 */
typedef enum {
    SLE_MESH_SERVICE_SUCCESS = 0,               /*!< @if Eng Service registration or deletion succeeded.
                                                    @else 服务注册成功 @endif */
    SLE_MESH_SERVICE_FAILED = 1,                /*!< @if Eng Service registration or deletion failed.
                                                    @else 服务注册失败 @endif */
} sle_mesh_service_result_t;

/**
 * @if Eng
 * @brief Service control type.
 * @else
 * @brief 服务类型。
 * @endif
 */
typedef enum {
    SLE_MESH_SERVICE_TYPE_CONTROL = 0,      /*!< @if Eng Control service.
                                                @else 控制类服务。 @endif */
    SLE_MESH_SERVICE_TYPE_CONTROLLED = 1,   /*!< @if Eng Controlled service.
                                                @else 受控制类服务。 @endif */
} sle_mesh_service_type_t;

/**
 * @if Eng
 * @brief Service publish and subscribe.
 * @else
 * @brief 服务发布订阅。
 * @endif
 */
typedef struct {
    uint16_t mesh_id;                       /*!< @if Eng Publisher or subscriber network address.
                                                @else 发布方或订阅方网络地址。 @endif */
    sle_uuid_t service_uuid;                /*!< @if Eng Unique identifier for publishing or subscribing to a service.
                                                @else 发布服务或订阅服务的唯一标识。 @endif */
    uint8_t service_instance;               /*!< @if Eng Service instance ID.
                                                @else 服务实例ID。 @endif */
} sle_mesh_publish_subscribe_t;

/**
 * @if Eng
 * @brief Struct of Service attributes.
 * @else
 * @brief 服务属性。
 * @endif
 */
typedef struct {
    sle_uuid_t property_uuid;                   /*!< @if Eng Unique identifier of the service attribute.
                                                    @else 服务属性唯一标识。 @endif */
    sle_mesh_property_value_type_t value_type;  /*!< @if Eng Service attribute value type
                                                    { @ref sle_mesh_property_value_type_t }.
                                                   @else 服务属性值类型 { @ref sle_mesh_property_value_type_t }。 @endif */
    uint8_t *value;                             /*!< @if Eng Service attribute value.
                                                    @else 服务属性值。 @endif */
    uint16_t value_len;                         /*!< @if Eng Service attribute value length.
                                                    @else 服务属性值长度。 @endif */
    uint32_t value_min;                         /*!< @if Eng Lower threshold for service attribute value.
                                                    @else 服务属性值下限阈值。 @endif */
    uint32_t value_max;                         /*!< @if Eng Upper threshold for service attribute value.
                                                    @else 服务属性值上限阈值。 @endif */
} sle_mesh_property_t;

/**
 * @if Eng
 * @brief Callback invoked when service subscription changed.
 * @par Callback invoked when service subscription changed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] service      Service unique service identifier.
 * @param [in] instance     Service instance ID.
 * @param [in] property     Service unique identifier of the service attribute.
 * @param [in] value        Service attribute value.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  服务订阅回调函数。
 * @par    服务订阅回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] service        服务唯一标识。
 * @param [in] instance       服务实例ID。
 * @param [in] property       服务属性唯一标识。
 * @param [in] sequence_num   服务回调序列号。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @endif
 */
typedef void(*sle_mesh_subscribe_callback)(sle_uuid_t service,
    uint8_t instance, sle_mesh_property_t *property, uint16_t sequence_num);

/**
 * @if Eng
 * @brief Callback invoked when service subscription changed.
 * @par Callback invoked when service subscription changed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] instance  Service instance ID.
 * @param [in] service   Service instance creation result { @ref sle_mesh_service_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  服务实例注册回调函数。
 * @par    服务实例注册回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] instance   服务实例ID。
 * @param [in] result     服务实例创建结果 { @ref sle_mesh_service_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @endif
 */
typedef void(*sle_mesh_service_register_callback)(uint8_t instance, uint8_t result);

/**
 * @if Eng
 * @brief Struct of SLE mesh service management callback, supporting multiple service instances.
 * @else
 * @brief 服务管理回调，支持多个服务实例。
 * @endif
 */
typedef struct {
    sle_mesh_subscribe_callback sub_cbk;             /*!< @if Eng Service subscription callback.
                                                    @else 服务订阅回调。 @endif */
    sle_mesh_service_register_callback result_cbk;   /*!< @if Eng Service registration result reported.
                                                    @else 服务注册结果上报。 @endif */
} sle_mesh_service_callback_t;

/**
 * @if Eng
 * @brief Struct of Service attributes.
 * @else
 * @brief 服务实例。
 * @endif
 */
typedef struct sle_mesh_instance_info {
    sle_uuid_t uuid;                                    /*!< @if Eng Unique identifier of the service.
                                                            @else 服务唯一标识。 @endif */
    uint8_t description[SLE_MESH_SERVICE_DESCRIPT_LEN + 1]; /*!< @if Eng Service instance description.
                                                            @else 服务实例描述。 @endif */
    sle_mesh_service_callback_t ckb_fun;                /*!< @if Eng Service instance callback.
                                                            @else 服务实例回调。 @endif */
    uint8_t ctrl_type;                                  /*!< @if Eng { @ref sle_mesh_service_type_t }.
                                                            @else 服务控制类型 { @ref sle_mesh_service_type_t }。 @endif */
    uint8_t property_num;                               /*!< @if Eng Total number of attributes included in the service.
                                                            @else 服务包含的属性总数。 @endif */
    sle_mesh_property_t *property_list;                 /*!< @if Eng List of attributes included in the service.
                                                            @else 服务包含的属性列表。 @endif */
} sle_mesh_service_instance_t;

/**
 * @if Eng
 * @brief Struct of Service attributes.
 * @else
 * @brief 服务订阅信息。
 * @endif
 */
typedef struct {
    uint16_t mesh_id;           /*!< @if Eng Network address of the service subscriber.
                                    @else 服务订阅方的网络地址。 @endif */
    uint8_t instance;           /*!< @if Eng Local service instance associated with the service subscriber.
                                    @else 服务订阅方关联的本地服务实例。 @endif */
    sle_uuid_t service_uuid;    /*!< @if Eng Unique identifier of the local service associated with the subscriber.
                                    @else 服务订阅方关联的本地服务唯一标识。 @endif */
} sle_mesh_subscribe_t;

typedef struct sle_mesh_instance_query_info {
    uint8_t instance;                                       /*!< @if Eng Id of the service instance.
                                                                 @else   服务实例ID。 @endif */
    uint8_t property_num;                                   /*!< @if Eng Total number of attributes included in the service.
                                                                 @else   服务包含的属性总数。 @endif */
    sle_mesh_property_t *property_list;                   /*!< @if Eng List of attributes included in the service.
                                                                 @else   服务包含的属性列表。 @endif */
} sle_mesh_instance_query_info_t;

/**
 * @if Eng
 * @brief Struct of Service instance query result.
 * @else
 * @brief 服务实例查询结果。
 * @endif
 */
typedef struct sle_mesh_service_query_info {
    sle_uuid_t uuid;                                        /*!< @if Eng Unique identifier of the service.
                                                                 @else   服务唯一标识。 @endif */
    uint8_t inst_num;                                       /*!< @if Eng Total number of instance in the service.
                                                                 @else   服务包含的实例总数。 @endif */
    sle_mesh_instance_query_info_t *inst_list;            /*!< @if Eng List of attributes included in the service.
                                                                 @else   服务包含的属性列表。 @endif */
} sle_mesh_service_query_info_t;

/**
 * @if Eng
 * @brief Callback function for adding a subscription group.
 * @par This function is invoked when a new subscription group is added.
 * @attention 1. This callback function operates on the SLE mesh thread and must not block or wait for extended periods.
 * @attention 2. Pointers are allocated and deallocated by SLE mesh; do not release them within the callback.
 * @param [in] group_name   The name of the subscription group.
 * @param [in] group_id     The ID of the subscription group.
 * @param [in] result       The result of adding the subscription group.
 * @retval void This function does not return a value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  订阅组添加回调函数。
 * @par    订阅组添加回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] group_name   订阅组名称。
 * @param [in] group_id     订阅组ID。
 * @param [in] result       订阅组添加结果。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_management_callback_t
 * @endif
 */
typedef void (*sle_mesh_group_add_callback)(const char *group_name, uint16_t group_id, uint8_t result);

/**
 * @if Eng
 * @brief Callback function for remove a subscription group.
 * @par This function is invoked when a subscription group is removed.
 * @attention 1. This callback function operates on the SLE mesh thread and must not block or wait for extended periods.
 * @attention 2. Pointers are allocated and deallocated by SLE mesh; do not release them within the callback.
 * @param [in] group_id     The ID of the subscription group.
 * @param [in] result       The result of remove the subscription group.
 * @retval void This function does not return a value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  订阅组删除回调函数。
 * @par    订阅组删除回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] group_id     订阅组ID。
 * @param [in] result       订阅组删除结果。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_management_callback_t
 * @endif
 */
typedef void (*sle_mesh_group_rmv_callback)(uint16_t group_id, uint8_t result);

/**
 * @if Eng
 * @brief  Callback function for adding group address subscription to a specified service instance.
 * @par    Callback function for adding group address subscription to a specified service instance.
 * @attention 1. This callback function operates on the SLE mesh thread and must not block or wait for extended periods.
 * @attention 2. Pointers are allocated and deallocated by SLE mesh; do not release them within the callback.
 * @param [in] group_id     Subscription group ID.
 * @param [in] inst         Service instance.
 * @param [in] result       Result of the group address subscription.
 * @retval void This function does not return a value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  指定服务实例添加组地址订阅回调函数。
 * @par    指定服务实例添加组地址订阅回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] group_id     订阅组ID。
 * @param [in] inst         服务实例。
 * @param [in] result       组地址订阅结果。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_management_callback_t
 * @endif
 */
typedef void (*sle_mesh_group_subscribe_add_callback)(uint16_t group_id, const sle_mesh_subscribe_t *inst,
                                                      uint8_t result);

/**
 * @if Eng
 * @brief  Callback function for remove group address subscription to a specified service instance.
 * @par    Callback function for remove group address subscription to a specified service instance.
 * @attention 1. This callback function operates on the SLE mesh thread and must not block or wait for extended periods.
 * @attention 2. Pointers are allocated and deallocated by SLE mesh; do not release them within the callback.
 * @param [in] group_id     Subscription group ID.
 * @param [in] inst         Service instance.
 * @param [in] result       Result of the group address subscription.
 * @retval void This function does not return a value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief  指定服务实例删除组地址订阅回调函数。
 * @par    指定服务实例删除组地址订阅回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] group_id     订阅组ID。
 * @param [in] inst         服务实例。
 * @param [in] result       组地址订阅结果。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_management_callback_t
 * @endif
 */
typedef void (*sle_mesh_group_subscribe_rmv_callback)(uint16_t group_id, const sle_mesh_subscribe_t *inst,
                                                      uint8_t result);

/**
 * @if Eng
 * @brief  Callback function for service query results.
 * @par    Callback function for service query results.
 * @attention 1. This callback function operates on the SLE mesh thread and must not block or wait for extended periods.
 * @attention 2. Pointers are allocated and deallocated by SLE mesh; do not release them within the callback.
 * @param [in] mesh_id           The mesh addr of the node.
 * @param [in] num               The number of service entries returned.
 * @param [in] services          A pointer to an array of service information structures.
 * @param [in] result            The result or error code from the query operation.
 * @retval void This function does not return a value.
 * @par Dependency:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_callback_t
 * @else
 * @brief 服务查询结果回调函数。
 * @par    服务查询结果回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] mesh_id           节点mesh地址.
 * @param [in] num               服务个数.
 * @param [in] services          服务列表.
 * @param [in] result            查询结果.
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_service.h
 * @see sle_mesh_service_management_callback_t
 * @endif
 */
typedef void (*sle_mesh_service_query_callback)(uint16_t mesh_id,
                                                uint16_t num,
                                                const sle_mesh_service_query_info_t *services,
                                                uint8_t result);

/**
 * @if Eng
 * @brief Callback for subscribe group address of service instance.
 * @else
 * @brief 服务订阅组相关回调。
 * @endif
 */
typedef struct {
    sle_mesh_group_add_callback group_add_cbk;                  /*!< @if Eng Callback for add the group address.
                                                                     @else 组地址添加回调。 @endif */
    sle_mesh_group_rmv_callback group_rmv_cbk;                 /*!< @if Eng Callback for remove the group address.
                                                                    @else 组地址删除回调。 @endif */
    sle_mesh_group_subscribe_add_callback subscribe_add_cbk;   /*!< @if Eng Callback for the service instance
                                                                            add subscribe group address.
                                                                    @else 服务实例组地址订阅回调。 @endif */
    sle_mesh_group_subscribe_rmv_callback subscribe_rmv_cbk;   /*!< @if Eng Callback for the service instance
                                                                            remove subscribe group address.
                                                                    @else 服务实例取消组地址订阅回调。 @endif */
    sle_mesh_service_query_callback service_query_cbk;         /*!< @if Eng Callback for the service query result.
                                                                    @else 服务查询结果回调 @endif */
} sle_mesh_service_management_callback_t;

/**
 * @if Eng
 * @brief Register service instance.
 * @par Description: Register service instance.
 * @param [in] service  Service instance information { @ref sle_mesh_service_instance_t }.
 * @attention Synchronous processing on any service node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册服务实例。
 * @par Description: 注册服务实例。
 * @param [in] service  服务实例信息 { @ref sle_mesh_service_instance_t }。
 * @attention 任意服务节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_register_service_instance(sle_mesh_service_instance_t *service);

/**
 * @if Eng
 * @brief Create a service subscription group.
 * @par Description: Create a service subscription group.
 * @param [in] group_name   Subscription group name, with a maximum length of { @ref SLE_MESH_SERVICE_DESCRIPT_LEN }.
 * @param [in] cfg_mesh_id  Configure the network address of the subscribed node.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  创建服务订阅组。
 * @par Description: 创建服务订阅组。
 * @param [in] group_name   订阅组名称，名称最大长度 { @ref SLE_MESH_SERVICE_DESCRIPT_LEN }。
 * @param [in] cfg_mesh_id  配置订阅的节点网络地址。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_create_subscribe_group(char *group_name, uint16_t cfg_mesh_id);

/**
 * @if Eng
 * @brief Release service subscription group.
 * @par Description: Release service subscription group.
 * @param [in] group_id   Subscription group ID.
 * @param [in] cfg_mesh_id  Configure the network address of the subscribed node.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  释放服务订阅组。
 * @par Description: 释放服务订阅组。
 * @param [in] group_id     订阅组ID。
 * @param [in] cfg_mesh_id  配置订阅的节点网络地址。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_release_subscribe_group(uint16_t group_id, uint16_t cfg_mesh_id);

/**
 * @if Eng
 * @brief Add members to the subscription group.
 * @par Description: Add members to the subscription group.
 * @param [in] group_id     Service subscription group ID.
 * @param [in] subscribe    Subscription information.
 * @param [in] cfg_mesh_id  Configure the network address of the subscribed node.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  向订阅组添加成员。
 * @par Description: 向订阅组添加成员。
 * @param [in] group_id     服务订阅组ID。
 * @param [in] subscribe    订阅信息。
 * @param [in] cfg_mesh_id  配置订阅的节点网络地址。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_add_subscribe_member(uint16_t group_id, sle_mesh_subscribe_t *subscribe, uint16_t cfg_mesh_id);

/**
 * @if Eng
 * @brief Delete members to the subscription group.
 * @par Description: Delete members to the subscription group.
 * @param [in] group_id     Service subscription group ID.
 * @param [in] subscribe    Subscription information.
 * @param [in] cfg_mesh_id  Configure the network address of the subscribed node.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  从订阅组删除成员。
 * @par Description: 从订阅组删除成员。
 * @param [in] group_id     服务订阅组ID。
 * @param [in] subscribe    订阅信息。
 * @param [in] cfg_mesh_id  配置订阅的节点网络地址。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_del_subscribe_member(uint16_t group_id, sle_mesh_subscribe_t *subscribe, uint16_t cfg_mesh_id);

/**
 * @if Eng
 * @brief Delete members to the subscription group.
 * @par Description: Delete members to the subscription group.
 * @param [in] service    Unique identifier of the service instance.
 * @param [in] instance   Service instance ID.
 * @param [in] property   Service instance attribute value to be written.
 * @attention Synchronous processing on the node where the service is located, modifying the instance attributes
 *  of this node and the corresponding node instances related to the publish relationship.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  服务实例属性值变更。
 * @par Description: 服务实例属性值变更。
 * @param [in] service    服务实例唯一标识。
 * @param [in] instance   服务实例ID。
 * @param [in] property   待写入的服务实例属性值。
 * @attention 服务所在节点执行，变更本节点实例以及发布关系对应节点实例属性值。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_service_property_write(sle_uuid_t service, uint8_t instance, sle_mesh_property_t *property);

/**
 * @if Eng
 * @brief Service instance attribute value change unicast message.
 * @par Description: Service instance attribute value change unicast message.
 * @param [in] mesh_id   Target device network address.
 * @param [in] instance  Service instance ID.
 * @param [in] service   Unique service identifier.
 * @param [in] property  Service attributes to be changed.
 * @attention Synchronous processing on admin node, and update the attribute values of the
 *  corresponding node instances in the publish relationship.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  服务实例属性值变更单播消息。
 * @par Description: 服务实例属性值变更单播消息。
 * @param [in] mesh_id   目标设备网络地址。
 * @param [in] instance  服务实例ID。
 * @param [in] service   服务唯一标识。
 * @param [in] property  待变更的服务属性。
 * @attention 管理节点同步执行，变更目标节点实例以及发布关系对应节点实例属性值。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_property_change_unicast(uint16_t mesh_id, sle_uuid_t service, uint8_t instance,
    sle_mesh_property_t *property);

/**
 * @if Eng
 * @brief Service instance attribute value change multicast message.
 * @par Description: Service instance attribute value change multicast message.
 * @param [in] group_id  Subscription group ID.
 * @param [in] property  Service attributes to be changed.
 * @attention Synchronous processing on admin node, modifying the attribute values of the node instances corresponding
 *  to the subscription group and its associated publishing relationships.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  服务实例属性值变更多播消息。
 * @par Description: 服务实例属性值变更多播消息。
 * @param [in] group_id  订阅组ID。
 * @param [in] property  待变更的服务属性。
 * @attention 管理节点同步执行，变更订阅了该订阅组的属性以及发布关系对应节点实例属性值。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_property_change_multicast(uint16_t group_id, sle_mesh_property_t *property);

/**
 * @if Eng
 * @brief Configure unicast service publishing and subscription relationships.
 * @par Description: Configure unicast service publishing and subscription relationships.
 * @param [in] sub  Service subscribe information.
 * @param [in] pub  Service publish information.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  配置单播服务发布、订阅关系。
 * @par Description: 配置单播服务发布、订阅关系。
 * @param [in] sub  服务订阅信息。
 * @param [in] pub  服务发布信息。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_publish_config_unicast(sle_mesh_publish_subscribe_t *sub, sle_mesh_publish_subscribe_t *pub);

/**
 * @if Eng
 * @brief Configure multicast service publishing and subscription relationships.
 * @par Description: Configure multicast service publishing and subscription relationships.
 * @param [in] group_id  Service publish or subscribe group ID.
 * @param [in] publish   Service publish information.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  配置组播服务发布、订阅关系。
 * @par Description: 配置组播服务发布、订阅关系。
 * @param [in] group_id  服务发布、订阅组ID。
 * @param [in] publish   服务发布信息。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_publish_config_multicast(uint16_t group_id, sle_mesh_publish_subscribe_t *pub);

/**
 * @if Eng
 * @brief Query service information.
 * @par Description: Query service information.
 * @param [in] mesh_id  Query the network address of the target device.
 * @attention Synchronous processing on admin node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  查询服务信息。
 * @par Description: 查询服务信息。
 * @param [in] mesh_id  查询目标设备的网络地址。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_sdk_service_query(uint16_t mesh_id);

/**
 * @if Eng
 * @brief Register callbacks for service subscription groups.
 * @par Description: Register callbacks for service subscription groups.
 * @param [in] cbk Callback function. { @ref sle_mesh_service_management_callback_t }
 * @attention Management node executes synchronously.
 * @retval Execution result error code { @ref errcode_sle_mesh_t }.
 * @par Dependencies:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册服务订阅组相关回调。
 * @par Description: 注册服务订阅组相关回调。
 * @param [in] cbk  回调函数。{ @ref sle_mesh_service_management_callback_t }
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_sdk_service_callback_register(const sle_mesh_service_management_callback_t *cbk);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SLE_MESH_SDK_SERVICE_H */
