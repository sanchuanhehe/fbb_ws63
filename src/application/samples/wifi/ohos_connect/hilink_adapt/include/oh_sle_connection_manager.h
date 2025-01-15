/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE Connection Manager module.
 */

/**
 * @defgroup oh_sle_connection_manager connection manager API
 * @ingroup  SLE
 * @{
 */

#ifndef OH_SLE_CONNECTION_MANAGER
#define OH_SLE_CONNECTION_MANAGER

#include <stdint.h>
#include "oh_sle_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of sle pairing state.
 * @else
 * @brief 星闪配对状态。
 * @endif
 */
typedef enum {
    OH_SLE_PAIR_NONE    = 0x01,    /*!< @if Eng Pair state of none
                                     @else   未配对状态 @endif */
    OH_SLE_PAIR_PAIRING = 0x02,    /*!< @if Eng Pair state of pairing
                                     @else   正在配对 @endif */
    OH_SLE_PAIR_PAIRED  = 0x03     /*!< @if Eng Pair state of paired
                                     @else   已完成配对 @endif */
} SlePairStateType;

/**
 * @if Eng
 * @brief Enum of sle pairing state.
 * @else
 * @brief 星闪断链原因。
 * @endif
 */
typedef enum {
    OH_SLE_DISCONNECT_BY_REMOTE = 0x10,    /*!< @if Eng disconnect by remote
                                             @else   远端断链 @endif */
    OH_SLE_DISCONNECT_BY_LOCAL  = 0x11,    /*!< @if Eng disconnect by local
                                             @else   本端断链 @endif */
} SleDiscReasonType;

/**
 * @if Eng
 * @brief Enum of sle ACB connection state.
 * @else
 * @brief SLE ACB连接状态。
 * @endif
 */
typedef enum {
    OH_SLE_ACB_STATE_NONE          = 0x00,   /*!< @if Eng SLE ACB connect state of none
                                               @else   SLE ACB 未连接状态 @endif */
    OH_SLE_ACB_STATE_CONNECTED     = 0x01,   /*!< @if Eng SLE ACB connect state of connected
                                               @else   SLE ACB 已连接 @endif */
    OH_SLE_ACB_STATE_DISCONNECTED  = 0x02,   /*!< @if Eng SLE ACB connect state of disconnected
                                               @else   SLE ACB 已断接 @endif */
} SleAcbStateType;

/**
 * @if Eng
 * @brief Enum of sle crytography algorithm.
 * @else
 * @brief 星闪加密算法类型。
 * @endif
 */
typedef enum {
    OH_SLE_CRYTO_ALGO_AC1     = 0x00,   /*!< @if Eng crytography algorithm ac1
                                          @else   AC1加密算法类型 @endif */
    OH_SLE_CRYTO_ALGO_AC2     = 0x01,   /*!< @if Eng crytography algorithm ac2
                                          @else   AC2加密算法类型@endif */
    OH_SLE_CRYTO_ALGO_EA1     = 0x02,   /*!< @if Eng crytography algorithm ea1
                                          @else   EA1加密算法类型 @endif */
    OH_SLE_CRYTO_ALGO_EA2     = 0x03,   /*!< @if Eng crytography algorithm ea2
                                          @else   EA2加密算法类型 @endif */
} SleCryptoAlgoType;

/**
 * @if Eng
 * @brief Enum of sle key derivation algorithm
 * @else
 * @brief 星闪秘钥分发算法类型。
 * @endif
 */
typedef enum {
    OH_SLE_KEY_DERIV_ALGO_HA1     = 0x00,   /*!< @if Eng key derivation algorithm ac1
                                              @else   HA1秘钥分发算法类型 @endif */
    OH_SLE_KEY_DERIV_ALGO_HA2     = 0x01,   /*!< @if Eng key derivation algorithm ac2
                                              @else   HA2秘钥分发算法类型 @endif */
} SleKeyDerivAlgoType;

/**
 * @if Eng
 * @brief Enum of sle integrity check indicator
 * @else
 * @brief 星闪完整性校验指示类型。
 * @endif
 */
typedef enum {
    OH_SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_ENABLE      = 0x00,   /*!< @if Eng Encryption and integrity check
                                                                           are enabled at the same time.
                                                                   @else   加密和完整性保护同时启动 @endif */
    OH_SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_ENABLE     = 0x01,   /*!< @if Eng Do not enable encryption, but enable
                                                                           integrity check.
                                                                   @else   不启动加密，启动完整性保护 @endif */
    OH_SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_DISABLE     = 0x02,   /*!< @if Eng Encryption is enabled, but integrity
                                                                           check is disabled.
                                                                   @else   启动加密，不启动完整性保护 @endif */
    OH_SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_DISABLE    = 0x03,   /*!< @if Eng Encryption and integrity check
                                                                           are not enabled.
                                                                   @else   不启动加密，不启动完整性保护 @endif */
} SleIntegrChkIndType;

/**
 * @if Eng
 * @brief Enum of sle logical link update parameters.
 * @else
 * @brief 星闪逻辑链路更新参数请求
 * @endif
 */
typedef struct {
    uint16_t intervalMin;        /*!< @if Eng minimum interval
                                       @else   链路调度最小间隔，单位slot @endif */
    uint16_t intervalMax;        /*!< @if Eng maximum interval
                                       @else   链路调度最大间隔，单位slot @endif */
    uint16_t maxLatency;         /*!< @if Eng maximum latency
                                       @else   延迟周期，单位slot @endif */
    uint16_t supervisionTimeout; /*!< @if Eng timeout
                                       @else   超时时间，单位10ms @endif */
} SleConnectionParamUpdateReq;

/**
 * @if Eng
 * @brief Enum of sle logical link update parameters.
 * @else
 * @brief 星闪逻辑链路更新参数
 * @endif
 */
typedef struct {
    uint16_t connId;             /*!< @if Eng connection ID
                                       @else   连接ID @endif */
    uint16_t intervalMin;        /*!< @if Eng minimum interval
                                       @else   链路调度最小间隔，单位slot @endif */
    uint16_t intervalMax;        /*!< @if Eng maximum interval
                                       @else   链路调度最大间隔，单位slot @endif */
    uint16_t maxLatency;         /*!< @if Eng maximum latency
                                       @else   延迟周期，单位slot @endif */
    uint16_t supervisionTimeout; /*!< @if Eng timeout
                                       @else   超时时间，单位10ms @endif */
} SleConnectionParamUpdate;

/**
 * @if Eng
 * @brief Enum of sle logical link update event parameters.
 * @else
 * @brief 星闪逻辑链路更新事件参数
 * @endif
 */
typedef struct {
    uint16_t interval;              /*!< @if Eng interval
                                         @else   链路调度间隔，单位slot @endif */
    uint16_t latency;               /*!< @if Eng latency
                                         @else   延迟周期，单位slot @endif */
    uint16_t supervision;           /*!< @if Eng timeout
                                         @else   超时时间，单位10ms @endif */
} SleConnectionParamUpdateEvt;

/**
 * @if Eng
 * @brief Enum of sle authentication result.
 * @else
 * @brief 星闪认证结果
 * @endif
 */
typedef struct {
    uint8_t linkKey[SLE_LINK_KEY_LEN];      /*!< @if Eng link key
                                                  @else   链路密钥 @endif */
    uint8_t cryptoAlgo;                     /*!< @if Eng encryption algorithm type { @ref SleCryptoAlgoType }
                                                  @else   加密算法类型 { @ref SleCryptoAlgoType } @endif */
    uint8_t keyDerivAlgo;                  /*!< @if Eng key distribution algorithm type { @ref SleKeyDerivAlgoType }
                                                  @else   秘钥分发算法类型 { @ref SleKeyDerivAlgoType } @endif */
    uint8_t integrChkInd;                  /*!< @if Eng integrity check indication { @ref SleIntegrChkIndType }
                                                  @else   完整性校验指示 { @ref SleIntegrChkIndType } @endif */
} SleAuthInfoEvt;

/**
 * @if Eng
 * @brief Callback invoked when connect state changed.
 * @par Callback invoked when connect state changed.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId    connection ID.
 * @param [in] addr       address.
 * @param [in] connState connection state { @ref SleAcbStateType }.
 * @param [in] pairState pairing state { @ref SlePairStateType }.
 * @param [in] discReason the reason of disconnect { @ref SleDiscReasonType }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  连接状态改变的回调函数。
 * @par    连接状态改变的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId    连接 ID。
 * @param [in] addr       地址。
 * @param [in] connState 连接状态 { @ref SleAcbStateType }。
 * @param [in] pairState 配对状态 { @ref SlePairStateType }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SleConnectStateChangedCallback)(uint16_t connId, const SleAddr *addr,
    SleAcbStateType connState, SlePairStateType pairState, SleDiscReasonType discReason);

/**
 * @if Eng
 * @brief Callback invoked when connect parameter updated.
 * @par Callback invoked when connect parameter updated.
 * @attention 1.This function is called in SLE service context, should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId    connection ID.
 * @param [in] addr       address.
 * @param [in] status     error code.
 * @param [in] param      connection param.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  连接参数更新的回调函数。
 * @par    连接参数更新的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId    连接 ID。
 * @param [in] addr       地址。
 * @param [in] status     执行结果错误码。
 * @param [in] param      连接参数。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SleConnectParamUpdateCallback)(uint16_t connId, ErrCodeType status,
    const SleConnectionParamUpdateEvt *param);

/**
 * @if Eng
 * @brief Callback invoked before the request for updating the connect parameter is complete.
 * @par Callback invoked before the request for updating the connect parameter is complete.
 * @attention 1.This function is called in SLE service context, should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId    connection ID.
 * @param [in] status     error code.
 * @param [in] param      connection param.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  连接参数更新请求完成前的回调函数。
 * @par    连接参数更新请求完成前的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId    连接 ID。
 * @param [in] status     执行结果错误码。
 * @param [in] param      连接参数。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SleConnectParamUpdateReqCallback)(uint16_t connId, ErrCodeType status,
    const SleConnectionParamUpdateReq *param);

/**
 * @if Eng
 * @brief Callback invoked when authentication complete.
 * @par Callback invoked when authentication complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId connection ID.
 * @param [in] addr    address.
 * @param [in] status  error code.
 * @param [in] evt     authentication event.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  认证完成的回调函数。
 * @par    认证完成的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId 连接 ID。
 * @param [in] addr    地址。
 * @param [in] status  执行结果错误码。
 * @param [in] evt     认证事件。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SleAuthCompleteCallback)(uint16_t connId, const SleAddr *addr, ErrCodeType status,
    const SleAuthInfoEvt* evt);

/**
 * @if Eng
 * @brief Callback invoked when pairing complete.
 * @par Callback invoked when pairing complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId connection ID.
 * @param [in] addr    address.
 * @param [in] status  error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  配对完成的回调函数。
 * @par    配对完成的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId 连接 ID。
 * @param [in] addr    地址。
 * @param [in] status  执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SlePairCompleteCallback)(uint16_t connId, const SleAddr *addr, ErrCodeType status);

/**
 * @if Eng
 * @brief Callback invoked when rssi read complete.
 * @par Callback invoked when rssi read complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] connId connection ID.
 * @param [in] rssi    rssi.
 * @param [in] status  error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @else
 * @brief  读取rssi的回调函数。
 * @par    读取rssi的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] connId 连接 ID。
 * @param [in] rssi    rssi。
 * @param [in] status  执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see SleConnectionCallbacks
 * @endif
 */
typedef void (*SleReadRssiCallback)(uint16_t connId, int8_t rssi, ErrCodeType status);
/**
 * @if Eng
 * @brief Struct of SLE connection manager callback function.
 * @else
 * @brief SLE连接管理回调函数接口定义。
 * @endif
 */
typedef struct {
    SleConnectStateChangedCallback connectStateChangedCb;         /*!< @if Eng Connect state changed callback.
                                                                            @else   连接状态改变回调函数。 @endif */
    SleConnectParamUpdateReqCallback connectParamUpdateReqCb;   /*!< @if Eng Connect param updated callback.
                                                                            @else   连接参数更新回调函数。 @endif */
    SleConnectParamUpdateCallback connectParamUpdateCb;           /*!< @if Eng Connect param updated callback.
                                                                            @else   连接参数更新回调函数。 @endif */
    SleAuthCompleteCallback authCompleteCb;                         /*!< @if Eng Authentication complete callback.
                                                                            @else   认证完成回调函数。 @endif */
    SlePairCompleteCallback pairCompleteCb;                         /*!< @if Eng Pairing complete callback.
                                                                            @else   配对完成回调函数。 @endif */
    SleReadRssiCallback readRssiCb;                                 /*!< @if Eng Read rssi callback.
                                                                            @else   读取rssi回调函数。 @endif */
} SleConnectionCallbacks;

/**
 * @if Eng
 * @brief  Send connect request to remote device.
 * @par Description: Send connect request to remote device.
 * @param [in] addr address.
 * @retval error code, connection state change result will be returned at { @ref SleConnectStateChangedCallback }.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送连接请求。
 * @par Description: 发送连接请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码， 连接状态改变结果将在 { @ref SleConnectStateChangedCallback }中返回。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleConnectRemoteDevice(const SleAddr *addr);

/**
 * @if Eng
 * @brief  Send disconnect request to remote device.
 * @par Description: Send disconnect request to remote device.
 * @param [in] addr address.
 * @retval error code, connection state change result will be returned at { @ref SleConnectStateChangedCallback }.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送断开连接请求。
 * @par Description: 发送断开连接请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码， 连接状态改变结果将在 { @ref SleConnectStateChangedCallback }中返回。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleDisconnectRemoteDevice(const SleAddr *addr);

/**
 * @if Eng
 * @brief  Send connection parameter update request to remote device.
 * @par Description: Send connection parameter update request to remote device.
 * @param [in] params connection parameter.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送更新连接参数请求。
 * @par Description: 发送更新连接参数请求。
 * @param [in] params 连接参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleUpdateConnectParam(SleConnectionParamUpdate *params);

/**
 * @if Eng
 * @brief  Send pairing request to remote device.
 * @par Description: Send pairing request to remote device.
 * @param [in] addr address.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送配对请求。
 * @par Description: 发送配对请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SlePairRemoteDevice(const SleAddr *addr);

/**
 * @if Eng
 * @brief  Remove pairing.
 * @par Description: Remove pairing.
 * @param [in] addr address.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  删除配对。
 * @par Description: 删除配对。
 * @param [in] addr 地址。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleRemovePairedRemoteDevice(const SleAddr *addr);

/**
 * @if Eng
 * @brief  Remove all pairing.
 * @par Description: Remove all pairing.
 * @param NULL
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  删除所有配对。
 * @par Description: 删除所有配对。
 * @param NULL
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleRemoveAllPairs(void);

/**
 * @if Eng
 * @brief  Get paired device number.
 * @par Description: Get paired device number.
 * @param [out] number device number.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对设备数量。
 * @par Description: 获取配对设备数量。
 * @param [out] number 设备数量。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleGetPairedDevicesNum(uint16_t *number);

/**
 * @if Eng
 * @brief  Get paired device.
 * @par Description: Get paired device.
 * @param [out]   addr   linked list of device address.
 * @param [inout] number device number.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对设备。
 * @par Description: 获取配对设备。
 * @param [out]   addr   设备地址链表。
 * @param [inout] number 设备数量。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleGetPairedDevices(SleAddr *addr, uint16_t *number);

/**
 * @if Eng
 * @brief  Get pair state.
 * @par Description: Get pair state.
 * @param [in]  addr  device address.
 * @param [out] state pair state { @ref SlePairStateType }.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对状态。
 * @par Description: 获取配对状态。
 * @param [in]  addr  设备地址。
 * @param [out] state 配对状态 { @ref SlePairStateType }。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleGetPairState(const SleAddr *addr, uint8_t *State);

/**
 * @if Eng
 * @brief  Read remote device rssi value.
 * @par Description: Read remote device rssi value.
 * @param [in]  connId connection ID.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  读取对端设备rssi值。
 * @par Description: 读取对端设备rssi值。
 * @param [in]  connId 连接 ID。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
ErrCodeType SleReadRemoteDeviceRssi(uint16_t connId);

/**
 * @if Eng
 * @brief  Register SLE connection manager callbacks.
 * @par Description: Register SLE connection manager callbacks.
 * @param [in] func Callback function.
 * @retval error code.
 * @else
 * @brief  注册SLE连接管理回调函数。
 * @par Description: 注册SLE连接管理回调函数。
 * @param [in] func 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
ErrCodeType SleConnectionRegisterCallbacks(SleConnectionCallbacks *func);

#ifdef __cplusplus
}
#endif
#endif /* OH_OH_SLE_CONNECTION_MANAGER */