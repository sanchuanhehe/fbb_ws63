/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SLE mesh scene management.
 */
#ifndef SLE_MESH_SDK_SCENE_H
#define SLE_MESH_SDK_SCENE_H

#include "sle_ssap_stru.h"
#include "sle_mesh_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  SLE mesh scene management reserved field length,unit:byte.
 * @else
 * @brief  SLE Mesh 场景管理保留字段长度，单位:byte。
 * @endif
 */
#define SLE_MESH_SCENE_RESERVED_LEN     3

/**
 * @if Eng
 * @brief SLE mesh scene trigger mode.
 * @else
 * @brief 星闪mesh场景触发模式。
 * @endif
 */
typedef enum {
    SLE_MESH_TRIGGER_MODE_GT = 0,   /*!< @if Eng Upper threshold (triggered when greater than).
                                        @else 上限阈值（大于时触发） @endif */
    SLE_MESH_TRIGGER_MODE_LT = 1,   /*!< @if Eng Lower threshold (triggered when less than).
                                        @else 下限阈值（小于时触发） @endif */
    SLE_MESH_TRIGGER_MODE_EQ = 2,   /*!< @if Eng Set value (triggered when equal to).
                                        @else 设定值（等于时触发） @endif */
    SLE_MESH_TRIGGER_MODE_GTE = 3,  /*!< @if Eng Upper threshold (triggered when greater than or equal to).
                                        @else 上临界值（大于等于时触发） @endif */
    SLE_MESH_TRIGGER_MODE_LTE = 4,  /*!< @if Eng Lower threshold (triggered when less than or equal to).
                                        @else 下临界值（小于等于时触发） @endif */
} sle_mesh_scene_trigger_mode_t;

/**
 * @if Eng
 * @brief SLE mesh scene type bitmap.
 * @else
 * @brief 星闪mesh场景类型位图。
 * @endif
 */
typedef enum {
    SLE_MESH_SCENE_TRIGGER_REQ = 1,     /*!< @if Eng Scene trigger.
                                            @else 场景触发 @endif */
    SLE_MESH_SCENE_TRIGGER_RSP = 2,     /*!< @if Eng Scene response.
                                            @else 场景响应 @endif */
} sle_mesh_scene_type_t;

/**
 * @if Eng
 * @brief SLE mesh scene registration or deletion result.
 * @else
 * @brief 星闪mesh场景注册或删除结果。
 * @endif
 */
typedef enum {
    SLE_MESH_SCENE_SUCCESS = 0,                     /*!< @if Eng Scene registration or deletion succeeded.
                                                        @else 场景注册/删除成功 @endif */
    SLE_MESH_SCENE_FAILED = 1,                      /*!< @if Eng Scene registration or deletion failed.
                                                        @else 场景注册/删除失败 @endif */
    SLE_MESH_SCENE_TIMEOUT = 2,                     /*!< @if Eng Scene registration or deletion timeout.
                                                        @else 场景注册/删除超时 @endif */
} sle_mesh_scene_result_t;

/**
 * @if Eng
 * @brief SLE mesh scene type.
 * @else
 * @brief 星闪mesh场景信息。
 * @endif
 */
typedef struct {
    uint8_t scene_type;             /*!< @if Eng Scene type bitmap { @ref sle_mesh_scene_type_t }.
                                        @else 场景类型位图 { @ref sle_mesh_scene_type_t }。 @endif */
    uint8_t service_instance;       /*!< @if Eng Service management instance identifier.
                                        @else 服务管理实例ID。 @endif */
    uint16_t scene_id;              /*!< @if Eng Scene identifier.
                                        @else 场景ID。 @endif */
    uint16_t mesh_id;               /*!< @if Eng SLE mesh network address.
                                        @else 星闪mesh网内地址。 @endif */
    uint16_t reserved;              /*!< @if Eng Reserved field.
                                        @else 保留字段。 @endif */
    sle_uuid_t service_uuid;        /*!< @if Eng Service universally unique identifier.
                                        @else 服务uuid。 @endif */
    sle_uuid_t property_uuid;       /*!< @if Eng Service property universally unique identifier.
                                        @else 服务属性uuid。 @endif */
} sle_mesh_scene_info_t;

/**
 * @if Eng
 * @brief SLE mesh scene trigger.
 * @else
 * @brief 星闪mesh场景触发。
 * @endif
 */
typedef struct {
    sle_mesh_scene_info_t scene_info;               /*!< @if Eng Scene information { @ref sle_mesh_scene_info_t }.
                                                        @else 场景信息 { @ref sle_mesh_scene_info_t } @endif */
    uint8_t trigger_mode;                           /*!< @if Eng Scene trigger mode
                                                            { @ref sle_mesh_scene_trigger_mode_t }.
                                                        @else 场景触发模式
                                                            { @ref sle_mesh_scene_trigger_mode_t }。 @endif */
    uint8_t reserved[SLE_MESH_SCENE_RESERVED_LEN];  /*!< @if Eng Reserved field.
                                                        @else 保留字段。 @endif */
    uint32_t trigger_threshold;                     /*!< @if Eng Scene trigger threshold.
                                                        @else 场景触发阈值。 @endif */
} sle_mesh_scene_trigger_t;

/**
 * @if Eng
 * @brief SLE mesh scene respond.
 * @else
 * @brief 星闪mesh场景响应。
 * @endif
 */
typedef struct {
    sle_mesh_scene_info_t scene_info;             /*!< @if Eng Scene information { @ref sle_mesh_scene_info_t }.
                                                        @else 场景信息 { @ref sle_mesh_scene_info_t } @endif */
    uint32_t expected;                            /*!< @if Eng Scene response expected change value.
                                                        @else 场景响应预期更改值。 @endif */
} sle_mesh_scene_respond_t;

/**
 * @if Eng
 * @brief SLE mesh scene trigger params.
 * @else
 * @brief mesh场景触发入参。
 * @endif
 */
typedef struct {
    uint16_t scene_id;              /*!< @if Eng Scene ID to be triggered
                                         @else 需要触发的场景id @endif */
    uint32_t scene_trigger_time;    /*!< @if Eng From the current moment, trigger the scene after
                                                 a period of time equal to scene_trigger_time.Unit: millisecond
                                         @else 从当前时刻开始，过 scene_trigger_time 时间以后触发场景,单位毫秒 @endif */
} sle_mesh_scene_active_par_t;

/**
 * @if Eng
 * @brief Callback invoked when scene registration completed.
 * @par Callback invoked when scene registration completed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] scene_info Scene information { @ref sle_mesh_scene_info_t }.
 * @param [in] result     Scene or scene trigger or scene response registration result { @ref sle_mesh_scene_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @else
 * @brief  场景注册结果回调。
 * @par    场景注册结果回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] scene_info   场景信息 { @ref sle_mesh_scene_info_t }。
 * @param [in] result       场景、触发/响应注册结果 { @ref sle_mesh_scene_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @endif
 */
typedef void (*sle_mesh_scene_register_callback)(sle_mesh_scene_info_t *scene_info, uint8_t result);

/**
 * @if Eng
 * @brief Callback invoked when scene deletion completed.
 * @par Callback invoked when scene deletion completed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] scene_info Scene information { @ref sle_mesh_scene_info_t }.
 * @param [in] result     Scene trigger or scene response deletion result { @ref sle_mesh_scene_result_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @else
 * @brief  场景删除结果回调。
 * @par    场景删除结果回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] scene_info   场景信息 { @ref sle_mesh_scene_info_t }。
 * @param [in] result       场景触发/响应删除结果 { @ref sle_mesh_scene_result_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @endif
 */
typedef void (*sle_mesh_scene_delete_callback)(sle_mesh_scene_info_t *scene_info, uint8_t result);

/**
 * @if Eng
 * @brief Callback invoked when scene trigger.
 * @par Callback invoked when scene trigger.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] trigger_info Scene trigger information { @ref sle_mesh_scene_trigger_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @else
 * @brief  场景触发回调。
 * @par    场景触发回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] trigger_info   场景触发信息 { @ref sle_mesh_scene_trigger_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @endif
 */
typedef void (*sle_mesh_scene_trigger_callback)(sle_mesh_scene_trigger_t *trigger_info);

/**
 * @if Eng
 * @brief Callback invoked when scene respond.
 * @par Callback invoked when scene respond.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] trigger_info Scene respond information { @ref sle_mesh_scene_respond_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @else
 * @brief  场景响应回调。
 * @par    场景响应回调。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] trigger_info   场景响应信息 { @ref sle_mesh_scene_respond_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_scene.h
 * @see sle_mesh_scene_callback_t
 * @endif
 */
typedef void (*sle_mesh_scene_respond_callback)(sle_mesh_scene_respond_t *respond_info);

/**
 * @if Eng
 * @brief Struct of SLE mesh scene management callback function.
 * @else
 * @brief 星闪mesh 场景管理回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_mesh_scene_register_callback register_cbk;  /*!< @if Eng Scene registration result callback
                                                        { @ref sle_mesh_scene_register_callback}.
                                                        @else 场景注册结果回调
                                                        { @ref sle_mesh_scene_register_callback}。 @endif */
    sle_mesh_scene_delete_callback delete_cbk;      /*!< @if Eng Scene deletion result callback
                                                        { @ref sle_mesh_scene_delete_callback}.
                                                        @else 场景删除回调
                                                        { @ref sle_mesh_scene_delete_callback}。 @endif */
    sle_mesh_scene_trigger_callback trigger_cbk;    /*!< @if Eng Scene trigger callback
                                                        { @ref sle_mesh_scene_trigger_callback}.
                                                        @else 场景触发回调
                                                        { @ref sle_mesh_scene_trigger_callback}。 @endif */
    sle_mesh_scene_respond_callback respond_cbk;    /*!< @if Eng Scene response callback
                                                        { @ref sle_mesh_scene_respond_callback}.
                                                        @else 场景响应回调
                                                        { @ref sle_mesh_scene_respond_callback}。 @endif */
} sle_mesh_scene_callback_t;

/**
 * @if Eng
 * @brief Register scene management callback.
 * @par Description: Register scene management callback.
 * @param  [in]  func  Scene callback { @ref sle_mesh_scene_callback_t }.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 注册场景管理回调。
 * @par Description: 注册场景管理回调。
 * @param  [in]  func  场景回调 { @ref sle_mesh_scene_callback_t }。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_callback_register(sle_mesh_scene_callback_t *func);

/**
 * @if Eng
 * @brief Registration scene trigger.
 * @par Description: Registration scene trigger.
 * @param  [in]  trigger_info  Scene trigger information.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 注册场景触发。
 * @par Description: 注册场景触发。
 * @param  [in]  trigger_info  场景触发信息。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_trigger_register(sle_mesh_scene_trigger_t *trigger_info);

/**
 * @if Eng
 * @brief Registration scene response.
 * @par Description: Registration scene response.
 * @param  [in]  respond_info  Scene response information.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 注册场景响应。
 * @par Description: 注册场景响应。
 * @param  [in]  respond_info  场景响应信息。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_respond_register(sle_mesh_scene_respond_t *respond_info);

/**
 * @if Eng
 * @brief Actively triggering scene messages.
 * @par Description: Actively triggering scene messages.
 * @param  [in] scene_id Scenario ID.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 主动触发场景。
 * @par Description: 主动触发场景。
 * @param  [in] scene_id 场景ID。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_trigger_active(sle_mesh_scene_active_par_t *trigger_par);

/**
 * @if Eng
 * @brief SLE mesh add default scene trigger.
 * @par Description: SLE mesh add default scene trigger.
 * @param  [in] scene_resp Information of scene trigger.
 * @attention Admin nodes and terminal nodes need to be added simultaneously.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 增加缺省场景触发。
 * @par Description: 增加缺省场景触发。
 * @param  [in]  scene_resp  场景触发信息。
 * @attention 管理节点和端节点需要同时添加。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_add_default_trigger(sle_mesh_scene_trigger_t *scene_trigger);

/**
 * @if Eng
 * @brief SLE mesh add default scene response.
 * @par Description: SLE mesh add default scene response.
 * @param  [in] scene_resp Information of scene response.
 * @attention Admin nodes and terminal nodes need to be added simultaneously.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 增加缺省场景响应。
 * @par Description: 增加缺省场景响应。
 * @param  [in]  scene_resp  场景响应信息。
 * @attention 管理节点和端节点需要同时添加。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_add_default_respond(sle_mesh_scene_respond_t *scene_resp);

/**
 * @if Eng
 * @brief SLE mesh delete scene.
 * @par Description: SLE mesh delete scene.
 * @param  [in] scene_info Scenario information to be deleted.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 删除场景。
 * @par Description: 删除场景。
 * @param  [in] scene_info 待删除场景信息。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_delete(sle_mesh_scene_info_t *scene_info);

/**
 * @if Eng
 * @brief Query scene triggered table.
 * @par Description: Query scene triggered table.
 * @param  [in]     trigger_table Scene triggered table { @ref sle_mesh_scene_trigger_t }.
 * @param  [inout]  table_size    Scene triggered table size;return the actual size after the query completed.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 查询场景触发表。
 * @par Description: 查询场景触发表。
 * @param  [in]     trigger_table  场景触发表 { @ref sle_mesh_scene_trigger_t }.
 * @param  [inout]  table_size     场景触发表大小，查询完成后返回实际大小。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_query_trigger(sle_mesh_scene_trigger_t *trigger_table, uint8_t *table_size);

/**
 * @if Eng
 * @brief Query scene response table.
 * @par Description: Query scene response table.
 * @param  [in]     resp_table Scene response table { @ref sle_mesh_scene_respond_t }.
 * @param  [inout]  table_size  Scene response table size;return the actual size after the query completed.
 * @attention Aasynchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 查询场景响应表。
 * @par Description: 查询场景响应表。
 * @param  [in]     resp_table 场景响应表 { @ref sle_mesh_scene_respond_t }.
 * @param  [inout]  table_size 场景响应表大小，查询完成后返回实际大小。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_query_respond(sle_mesh_scene_respond_t *resp_table, uint8_t *table_size);

/**
 * @if Eng
 * @brief SLE mesh scene initialization.
 * @par Description: SLE mesh scene initialization.
 * @param  [out] scene_id Apply for scene ID.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 场景初始化。
 * @par Description: 场景初始化。
 * @param  [out]  scene_id  申请场景ID。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_init(uint16_t *scene_id);

/**
 * @if Eng
 * @brief SLE mesh scene deinitialization.
 * @par Description: SLE mesh scene deinitialization.
 * @param  [in] scene_id Apply for scene ID.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 场景去初始化。
 * @par Description: 场景去初始化。
 * @param  [in]  scene_id  申请场景ID。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_scene_deinit(const uint16_t scene_id);

#ifdef __cplusplus
}
#endif
#endif /* END of SLE_MESH_SDK_SCENE_H */
