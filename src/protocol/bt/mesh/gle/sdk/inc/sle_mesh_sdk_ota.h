/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle mesh ota.
 */
#ifndef SLE_MESH_SDK_OTA_H
#define SLE_MESH_SDK_OTA_H

#include <stdbool.h>
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
 * @brief  SLE mesh firmware version length.
 * @else
 * @brief  SLE Mesh 固件版本号长度。
 * @endif
 */
#define SLE_MESH_VERSION_LEN        19

/**
 * @if Eng
 * @brief  SLE mesh number of devices supported for simultaneous upgrades.
 * @else
 * @brief  SLE Mesh 同时升级最大设备数。
 * @endif
 */
#define SLE_MESH_OTA_MAX_DEVICE_NUMBER      256

/**
 * @if Eng
 * @brief SLE mesh system initialization parameter.
 * @else
 * @brief SLE mesh升级状态值
 * @endif
 */
typedef enum {
    SLE_MESH_OTA_IDLE = 0,          /*!< @if Eng Upgrade idle.
                                      @else 升级空闲 @endif */
    SLE_MESH_OTA_WAITING = 1,       /*!< @if Eng Waiting for upgrade.
                                      @else 等待升级 @endif */
    SLE_MESH_OTA_GET_INFO = 2,      /*!< @if Eng Get upgrade information.
                                      @else 获取升级信息 @endif */
    SLE_MESH_OTA_ADD_GROUP = 3,     /*!< @if Eng Add upgrade group.
                                      @else 加入升级事件组 @endif */
    SLE_MESH_OTA_UPDATE_CFG = 4,    /*!< @if Eng Configure update information.
                                      @else 配置更新信息 @endif */
    SLE_MESH_OTA_UPGRADING = 5,     /*!< @if Eng Upgrading.
                                      @else 升级中 @endif */
    SLE_MESH_OTA_TERMINATED = 6,    /*!< @if Eng Upgrade terminated.
                                      @else 升级中止 @endif */
    SLE_MESH_OTA_COMPLETED = 7,     /*!< @if Eng Upgrade completed.
                                      @else 升级完成 @endif */
    SLE_MESH_OTA_FAILED = 8,        /*!< @if Eng Upgrade failed.
                                      @else 升级失败 @endif */
} sle_mesh_upgrade_state_t;

/**
 * @if Eng
 * @brief SLE mesh ota upgrade result cause value.
 * @else
 * @brief SLE mesh升级结果原因值。
 * @endif
 */
typedef enum {
    SLE_MESH_OTA_SUCCESS = 0,       /*!< @if Eng Upgrade successful.
                                      @else 升级成功 @endif */
    SLE_MESH_OTA_TIMEOUT = 1,       /*!< @if Eng Upgrade response timeout.
                                      @else 响应超时 @endif */
    SLE_MESH_OTA_NO_SPACE = 2,      /*!< @if Eng Storage space not enough.
                                      @else 存储空间不足 @endif */
    SLE_MESH_OTA_SIGN_VERIFY_FAIL = 3, /*!< @if Eng Upgrade image signature verification failed.
                                            @else 升级镜像签名校验失败 @endif */
    SLE_MESH_OTA_NOT_MATCH,         /*!< @if Eng Firmware mismatch.
                                      @else 固件不匹配 @endif */
    SLE_MESH_OTA_REFUSE,            /*!< @if Eng Upgrade request rejected.
                                      @else 升级请求被拒绝 @endif */
    SLE_MESH_OTA_DEVICE_BUSY,       /*!< @if Eng Target device is busy.
                                      @else 目标设备繁忙 @endif */    /*  */
    SLE_MESH_OTA_TRANS_TIMEOUT,     /*!< @if Eng Upgrade firmware transfer timed out.
                                      @else 固件传输超时 @endif */
} sle_mesh_ota_result_t;

/**
 * @if Eng
 * @brief SLE mesh ota upgrade result.
 * @else
 * @brief SLE mesh升级结果。
 * @endif
 */
typedef struct {
    uint8_t state;                          /*!< @if Eng Node upgrade status { @ref sle_mesh_upgrade_state_t }.
                                                @else 升级状态 { @ref sle_mesh_upgrade_state_t } @endif */
    uint8_t progress;                        /*!< @if Eng Upgrade progress,value range: [0, 100].
                                                @else 升级进度，取值范围：[0,100] @endif */
    uint8_t result;                         /*!< @if Eng Upgrade result { @ref sle_mesh_ota_result_t }.
                                                @else 升级结果 { @ref sle_mesh_ota_result_t } @endif */
    uint8_t resv;                           /*!< @if Eng Reserved field.
                                                @else 保留字段 @endif */
    char version[SLE_MESH_VERSION_LEN];  /*!< @if Eng Firmware version string.
                                                 @else 固件版本号 @endif */
    uint16_t resv_short;                    /*!< @if Eng Short type reserved field.
                                                @else short类型保留字段 @endif */
} sle_mesh_ota_state_t;

/**
 * @if Eng
 * @brief SLE mesh ota update state.
 * @else
 * @brief SLE mesh ota升级状态。
 * @endif
 */
typedef struct {
    uint16_t mesh_id;                           /*!< @if Eng SLE mesh network address.
                                                     @else 星闪网络地址 @endif */
    uint8_t state;                              /*!< @if Eng Node upgrade status { @ref sle_mesh_upgrade_state_t }.
                                                     @else 升级状态 { @ref sle_mesh_upgrade_state_t } @endif */
} sle_mesh_ota_update_state_t;

/**
 * @if Eng
 * @brief SLE mesh firmware information.
 * @else
 * @brief SLE mesh固件信息。
 * @endif
 */
typedef struct {
    uint16_t mesh_id;                           /*!< @if Eng SLE mesh network address.
                                                    @else 星闪网络地址 @endif */
    char version[SLE_MESH_VERSION_LEN];      /*!< @if Eng Firmware version string.
                                                    @else 固件版本号 @endif */
} sle_mesh_ota_firmware_t;

/**
 * @if Eng
 * @brief SLE mesh ota node param information.
 * @else
 * @brief SLE mesh ota 节点参数信息。
 * @endif
 */
typedef struct {
    char version[SLE_MESH_VERSION_LEN];         /*!< @if Eng Firmware version string.
                                                    @else 固件版本号 @endif */
    uint16_t mesh_id;                           /*!< @if Eng SLE mesh network address.
                                                    @else 星闪网络地址 @endif */
    uint8_t timeout_num;/*!< @if Eng SLE mesh network address.
                                                    @else 超时次数 @endif */
    uint8_t ack_id;/*!< @if Eng Upgrade state changed callback.
                                                                    @else 升级状态变化回调函数。 @endif */
    uint8_t hop;/*!< @if Eng Upgrade state changed callback.
                                                                    @else 升级状态变化回调函数。 @endif */
    uint8_t lost_state;/*!< @if Eng Upgrade state changed callback.
                                                                    @else 升级状态变化回调函数。 @endif */
} sle_mesh_ota_info_t;/*!< @if Eng Upgrade state changed callback.
                                                                    @else 升级状态变化回调函数。 @endif */

/**
 * @if Eng
 * @brief Callback invoked when ota state changed.
 * @par Callback invoked when ota state changed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] mesh_id    SLE mesh assign network address.
 * @param [in] ota_state  Upgrade status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @else
 * @brief  升级状态改变的回调函数。
 * @par    升级状态改变的回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] mesh_id    星闪mesh分配的网络地址。
 * @param [in] ota_state  升级状态 { @ref sle_mesh_ota_state_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @endif
 */
typedef void (*sle_mesh_ota_state_callback)(uint16_t mesh_id, sle_mesh_ota_state_t *ota_state);

/**
 * @if Eng
 * @brief Callback invoked when upgrade information query completed.
 * @par Callback invoked when upgrade information query completed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] device_num       Device number.
 * @param [in] device_firmware  Firmware information.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @else
 * @brief  升级信息查询结果上报的回调函数。
 * @par    升级信息查询结果上报的回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] device_num       升级查询结果，设备数。
 * @param [in] device_firmware  升级查询结果，固件信息。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @endif
 */
typedef void (*sle_mesh_ota_firmware_callback)(uint16_t device_num, sle_mesh_ota_firmware_t *device_firmware);

/**
 * @if Eng
 * @brief Callback invoked when target node firmware transfer completed.
 * @par Callback invoked when target node firmware transfer completed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] device_firmware  Firmware information.
 * @retval bool #true reboot and upgrade, #false no action.
 * @par Dependency:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @else
 * @brief  目标节点固件传输完成后，请求开始升级。
 * @par    目标节点固件传输完成后，请求开始升级。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] is_allow  是否允许设备重启升级。
 * @param [in] device_firmware  升级固件信息。
 * @retval bool #true 设备重启升级；#false 无动作。
 * @par 依赖:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @endif
 */
typedef bool (*sle_mesh_ota_request_callback)(sle_mesh_ota_firmware_t *device_firmware);

/**
 * @if Eng
 * @brief Callback invoked when overall ota state changed.
 * @par Callback invoked when overall ota state changed.
 * @attention 1.This function is called in SLE mesh context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE mesh automatically.
 * @param [in] device_num    Device num.
 * @param [in] ota_update_state  Upgrade status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @else
 * @brief  整体升级状态改变的回调函数。
 * @par    整体升级状态改变的回调函数。
 * @attention  1. 该回调函数运行于SLE mesh线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE mesh申请内存，也由SLE mesh释放，回调中不应释放。
 * @param [in] device_num    网内设备数。
 * @param [in] ota_update_state  升级状态 { @ref sle_mesh_ota_update_state_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_mesh_sdk_system.h
 * @see sle_mesh_ota_callback_t
 * @endif
 */
typedef void (*sle_mesh_ota_overall_state_callback)(uint16_t device_num, sle_mesh_ota_update_state_t *ota_update_state);

/**
 * @if Eng
 * @brief Struct of SLE mesh upgrade Over-The-Air callback function.
 * @else
 * @brief 星闪mesh OTA回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_mesh_ota_state_callback state_cbk;                       /*!< @if Eng Upgrade state changed callback.
                                                                    @else 升级状态变化回调函数。 @endif */
    sle_mesh_ota_firmware_callback firmware_cbk;                 /*!< @if Eng Reporting firmware information query results..
                                                                    @else 固件信息查询结果上报回调函数。 @endif */
    sle_mesh_ota_request_callback request_cbk;                   /*!< @if Eng Target node initiates upgrade request.
                                                                    @else 目标节点开始升级请求。 @endif */
    sle_mesh_ota_overall_state_callback ota_overall_state_cbk;   /*!< @if Eng Overall upgrade state.
                                                                    @else 总体升级状态上报回调函数。 @endif */
} sle_mesh_ota_callback_t;

/**
 * @if Eng
 * @brief Struct of SLE mesh upgrade firmware information.
 * @else
 * @brief OTA固件信息。
 * @endif
 */
typedef struct {
    uint8_t *img_path;                                      /*!< @if Eng Firmware location,flash address or path.
                                                                @else 升级固件位置，Flash地址或路径 @endif */
    uint32_t img_size;                                      /*!< @if Eng Size of the firmware to be upgraded, in bytes.
                                                                @else 升级固件大小，单位:Byte @endif */
    char version[SLE_MESH_VERSION_LEN];                  /*!< @if Eng Firmware version string.
                                                                @else 升级固件版本号 @endif */
} sle_mesh_image_t;


/**
 * @if Eng
 * @brief SLE mesh queries the firmware information of all nodes.
 * @par Description: SLE mesh queries the firmware information of all nodes.
 * @attention Asynchronous processing on manager node,which reports the query result through the firmware_cbk callback,
 *            { @ref sle_mesh_ota_callback_t }.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  星闪mesh查询全网节点固件信息。
 * @par Description: 星闪mesh查询全网节点固件信息。
 * @attention 管理节点执行，异步请求，通过 { @ref sle_mesh_ota_callback_t } firmware_cbk 回调函数上报查询结果。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_get_all_firmware_info(void);

/**
 * @if Eng
 * @brief Register upgrade management callback function.
 * @par Description: Register upgrade management callback function.
 * @param [in] func Callback function.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief  注册升级管理回调函数。
 * @par Description: 注册升级管理回调函数。
 * @param [in] func 回调函数。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_register_callback(sle_mesh_ota_callback_t *func);

/**
 * @if Eng
 * @brief SLE mesh add devices to be upgraded.
 * @par Description: SLE mesh add devices to be upgraded.
 * @param [in] device_num  Number of devices to be added for upgrade.
 * @param [in] device_list List of network addresses to be added for upgrade.
 * @attention Synchronous processing on manager node and supports batch addition.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 添加待升级设备。
 * @par Description: 添加待升级设备。
 * @param [in] device_num  待添加升级设备数。
 * @param [in] device_list 待添加升级网络地址列表。
 * @attention 管理节点同步执行，支持批量添加。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_add_dev(uint16_t device_num, uint16_t *device_list);

/**
 * @if Eng
 * @brief SLE mesh delete devices to be upgraded.
 * @par Description: SLE mesh delete devices to be upgraded.
 * @param [in] device_num  Number of devices to be deleted for upgrade.
 * @param [in] device_list List of network addresses to be deleted for upgrade.
 * @attention Synchronous processing on manager node and supports batch delete.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 删除待升级设备。
 * @par Description: 删除待升级设备。
 * @param [in] device_num  待删除升级设备数。
 * @param [in] device_list 待删除升级网络地址列表。
 * @attention 管理节点同步执行，支持批量删除。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_del_dev(uint16_t device_num, uint16_t *device_list);

/**
 * @if Eng
 * @brief SLE mesh initialize the firmware upgrade information.
 * @par Description: SLE mesh initialize the firmware upgrade information.
 * @param [in] img_info  Firmware upgrade information { @ref sle_mesh_image_t }.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 初始化升级固件信息。
 * @par Description: 初始化升级固件信息。
 * @param [in] img_info  升级固件信息 { @ref sle_mesh_image_t }。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_init_image_info(sle_mesh_image_t *img_info);

/**
 * @if Eng
 * @brief Start the upgrade.
 * @par Description: Start the upgrade.
 * @attention Synchronous processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 开始升级。
 * @par Description: 开始升级。
 * @attention 管理节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_start(void);

/**
 * @if Eng
 * @brief Stop the upgrade.
 * @par Description: Stop the upgrade.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 中止升级。
 * @par Description: 中止升级。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_stop(void);

/**
 * @if Eng
 * @brief Query the current node upgrade status.
 * @par Description: Query the current node upgrade status.
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 查询当前节点升级状态。
 * @par Description: 查询当前节点升级状态。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_query_status(sle_mesh_ota_state_t *ota_state);

/**
 * @if Eng
 * @brief Config whether the current node can be upgraded.
 * @par Description: Config whether the current node can be upgraded.
 * @param [in] is_allow  Whether to allow the upgrade. true:allow, false:not allow.
 * @param [in] reason    When upgrade not allowed,specify the reason. { @ref sle_mesh_ota_result_t }。
 * @attention Synchronous processing on any node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 当前节点是否升级。
 * @par Description: 当前节点是否升级。
 * @param [in] is_allow  是否允许升级，取值描述：true：允许，false：不允许。
 * @param [in] reason    不允许升级时，指明原因 { @ref sle_mesh_ota_result_t }。
 * @attention 任意节点同步执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ota_request_config(bool is_allow, sle_mesh_ota_result_t reason);

/**
 * @if Eng
 * @brief SLE mesh manager node prepare for upgrade.
 * @par Description: SLE mesh manager node prepare for upgrade.
 * @param [in] img_info  Firmware upgrade information { @ref sle_mesh_image_t }.
 * @attention Only processing on manager node.
 * @retval error code { @ref errcode_sle_mesh_t }.
 * @par Depends:
 * @li sle_mesh_errcode.h
 * @else
 * @brief 管理节点进行升级准备
 * @par Description: 管理节点进行升级准备
 * @param [in] img_info  升级固件信息 { @ref sle_mesh_image_t }。
 * @attention 仅管理节点执行。
 * @retval 执行结果错误码 { @ref errcode_sle_mesh_t }。
 * @par 依赖：
 * @li sle_mesh_errcode.h
 * @endif
 */
errcode_t sle_mesh_ma_prepare_ota(sle_mesh_image_t *img_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* END of SLE_MESH_SDK_OTA_H */