/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE C FRAME API module.
 */

/**
 * @defgroup sle_c_frame c frame API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_C_FRAME
#define SLE_C_FRAME

#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of sle create c frame net parameters.
 * @else
 * @brief 创建C帧网络参数。
 * @endif
 */
typedef struct {
    uint16_t duration;                     /*!< @if Eng Scheduling execution duration, in slots.
                                                @else   调度执行时长，slot为单位。 @endif */
    uint16_t interval;                     /*!< @if Eng System management frame spacing, 20 <= Interval <= 0xFFFF.
                                                @else   系统管理帧间隔, 20 <= Interval <= 0xFFFF。@endif */
    uint8_t adv_hdl;                       /*!< @if Eng Associated broadcast handle.
                                                @else   关联的广播句柄。@endif */
    uint8_t max_slot;                      /*!< @if Eng Max slot.
                                                @else   最大时隙个数。@endif */
    uint8_t g_retrans_num;                 /*!< @if Eng G retrans slot num.
                                                @else   G节点TX时隙个数。@endif */
    uint8_t preemption_num;                /*!< @if Eng Preemption slot num.
                                                @else   抢占时隙个数。@endif */
    uint8_t local_addr_type;               /*!< @if Eng Local addr type.
                                                @else   本机地址类型。@endif */
    uint8_t local_addr[SLE_ADDR_LEN];      /*!< @if Eng Local addr.
                                                @else   本机地址。@endif */
} sle_c_frame_g_create_param_t;

/**
 * @if Eng
 * @brief Enum of sle add c frame net parameters.
 * @else
 * @brief 加入C帧网络参数。
 * @endif
 */
typedef struct {
    uint16_t duration;                       /*!< @if Eng Scheduling execution duration, in slots.
                                                  @else   调度执行时长，slot为单位。 @endif */
    uint16_t interval;                       /*!< @if Eng System management frame spacing, 20 <= Interval <= 0xFFFF.
                                                  @else   系统管理帧间隔, 20 <= Interval <= 0xFFFF。@endif */
    uint8_t local_addr_type;                 /*!< @if Eng Local addr type.
                                                  @else   本机地址类型。@endif */
    uint8_t local_addr[SLE_ADDR_LEN];        /*!< @if Eng Local addr.
                                                  @else   本机地址。@endif */
    uint8_t peer_addr_type;                  /*!< @if Eng Peer addr type.
                                                  @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];         /*!< @if Eng Peer addr.
                                                  @else   远端地址。@endif */
    uint8_t join_req_data_len;               /*!< @if Eng Join req data len.
                                                  @else   入网请求数据长度。@endif */
    uint8_t *join_req_data;                  /*!< @if Eng Join req data.
                                                  @else   入网请求数据。@endif */
} sle_c_frame_t_add_g_req_t;

/**
 * @if Eng
 * @brief Enum of sle g add t to c frame net parameters.
 * @else
 * @brief G将T加入C帧网络参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                      /*!< @if Eng C frame id.
                                                   @else   C帧网络id。@endif */
    uint8_t peer_addr_type;                   /*!< @if Eng Peer addr type.
                                                   @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];          /*!< @if Eng Peer addr.
                                                   @else   远端地址。@endif */
} sle_c_frame_g_add_t_t;

/**
 * @if Eng
 * @brief Enum of sle g/t send data parameters.
 * @else
 * @brief G/T发送数据参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                      /*!< @if Eng C frame id.
                                                   @else   C帧网络id。@endif */
    uint8_t send_times;                       /*!< @if Eng Send times.
                                                   @else   发送次数。@endif */
    uint8_t dst_addr_type;                    /*!< @if Eng Dst addr type.
                                                   @else   目的地址类型。@endif */
    uint8_t dst_addr[SLE_ADDR_LEN];           /*!< @if Eng Dst addr.
                                                   @else   目的地址。@endif */
    uint8_t data_len;                         /*!< @if Eng Data len.
                                                   @else   发送数据长度。@endif */
    uint8_t *data;                            /*!< @if Eng Data.
                                                   @else   发送数据内容。@endif */
} sle_c_frame_send_data_t;

/**
 * @if Eng
 * @brief Enum of sle g/t delete channel parameters.
 * @else
 * @brief G/T删除链路参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                      /*!< @if Eng C frame id.
                                                   @else   C帧网络id。@endif */
    uint8_t peer_addr_type;                   /*!< @if Eng Peer addr type.
                                                   @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];          /*!< @if Eng Peer addr.
                                                   @else   远端地址。@endif */
} sle_c_frame_delete_channel_t;

/**
 * @if Eng
 * @brief Enum of sle set phy parameters.
 * @else
 * @brief 设置phy参数。
 * @endif
 */
typedef struct {
    uint8_t frame_format;            /*!< @if Eng Frame format.
                                          @else   帧格式，0:帧1, 1:帧2，T此值无效。@endif */
    uint8_t phy;                     /*!< @if Eng Phy.
                                          @else   带宽，0:1M带宽，1:2M带宽，2:4M带宽, T此值无效。@endif */
    uint8_t pilot;                   /*!< @if Eng Pilot.
                                          @else   导频密度，0:(4:1)，1:(8:1), 2:(16:1), 3:(无导频)。T此值无效。@endif */
    uint8_t mcs;                     /*!< @if Eng Mcs.
                                          @else   调制编码，帧4支持MCS0-5，帧2支持MCS6-12和MCS16-18，T此值无效。@endif */
} sle_c_frame_set_phy_param_t;

/**
 * @if Eng
 * @brief Enum of sle g recv add req result parameters.
 * @else
 * @brief 收到入网请求参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                       /*!< @if Eng C frame id.
                                                    @else   C帧网络id。@endif */
    uint8_t peer_addr_type;                    /*!< @if Eng Peer addr type.
                                                    @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];           /*!< @if Eng Peer addr.
                                                    @else   远端地址。@endif */
    uint8_t data_len;                          /*!< @if Eng Data len.
                                                    @else   入网请求数据长度。@endif */
    uint8_t *data;                             /*!< @if Eng Data.
                                                    @else   入网请求数据。@endif */
} sle_c_frame_g_recv_t_add_req_t;

/**
 * @if Eng
 * @brief Enum of sle add complete parameters.
 * @else
 * @brief 加入完成结果参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                      /*!< @if Eng C frame id.
                                                   @else   C帧网络id。@endif */
    uint8_t t_node_id;                        /*!< @if Eng T node id.
                                                   @else   分配的节点id。@endif */
    uint8_t peer_addr_type;                   /*!< @if Eng Peer addr type.
                                                   @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];          /*!< @if Eng Peer addr.
                                                   @else   远端地址。@endif */
} sle_c_frame_add_completed_result_t;

/**
 * @if Eng
 * @brief Enum of sle g/t recv data parameters.
 * @else
 * @brief g/t收到数据参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                     /*!< @if Eng C frame id.
                                                  @else   C帧网络id。@endif */
    uint8_t peer_addr_type;                  /*!< @if Eng Peer addr type.
                                                  @else   远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];         /*!< @if Eng Peer addr.
                                                  @else   远端地址。@endif */
    uint8_t data_len;                        /*!< @if Eng Data len.
                                                  @else   收到的数据长度。@endif */
    uint8_t *data;                           /*!< @if Eng Data.
                                                  @else   收到的数据内容。@endif */
} sle_c_frame_recv_data_result_t;

/**
 * @if Eng
 * @brief Enum of sle g delete channel parameters.
 * @else
 * @brief G删除链路结果参数。
 * @endif
 */
typedef struct {
    uint16_t c_frame_id;                       /*!< @if Eng C frame id.
                                                    @else   C帧网络id。@endif */
    uint8_t peer_addr_type;                    /*!< @if Eng Peer addr type.
                                                    @else   删除的远端地址类型。@endif */
    uint8_t peer_addr[SLE_ADDR_LEN];           /*!< @if Eng Peer addr.
                                                    @else   删除的远端地址。@endif */
} sle_c_frame_delete_channel_result_t;

/**
 * @if Eng
 * @brief Callback invoked when g add t complete.
 * @par Callback invoked when g add t complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] result   g add t complete result.
 * @param [in] status   error code.
 * @retval #void no return value.
 * @else
 * @brief  g加入t节点完成回调函数。
 * @par    g加入t节点完成回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] result    g加入t节点完成回调参数。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_add_complete_callback)(sle_c_frame_add_completed_result_t *result, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when recv data.
 * @par Callback invoked when recv data.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] result   recv data.
 * @retval #void no return value.
 * @else
 * @brief  G/T收到数据回调函数。
 * @par    G/T收到数据回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] result    收到的数据。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_recv_data_callback)(sle_c_frame_recv_data_result_t *result);

/**
 * @if Eng
 * @brief Callback invoked when g/t delete channel.
 * @par Callback invoked when g/t delete channel.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] result   g/t delete channel result.
 * @param [in] status   error code.
 * @retval #void no return value.
 * @else
 * @brief  g/t删除c帧链路完成回调函数。
 * @par    g/t删除c帧链路完成回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] result    g/t删除c帧链路完成回调参数。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_delete_callback)(sle_c_frame_delete_channel_result_t *result, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when g create c frame net.
 * @par Callback invoked when g create c frame net.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] c_frame_id   c frame id.
 * @param [in] status       error code.
 * @retval #void no return value.
 * @else
 * @brief  创建C帧网络回调函数。
 * @par    创建C帧网络回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] c_frame_id    C帧id。
 * @param [in] status        执行结果错误码。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_g_create_callback)(uint16_t c_frame_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when g recv add req.
 * @par Callback invoked when g recv add req.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] req   t add req.
 * @retval #void no return value.
 * @else
 * @brief  G端收到T端加入C帧网请求回调函数。
 * @par    G端收到T端加入C帧网请求回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] req    G端收到T端加入C帧网络请求回调参数。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_add_req_callback)(sle_c_frame_g_recv_t_add_req_t *req);

/**
 * @if Eng
 * @brief Callback invoked when g delete c frame net.
 * @par Callback invoked when g delete c frame net.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] c_frame_id   c frame id.
 * @param [in] status       error code.
 * @retval #void no return value.
 * @else
 * @brief  销毁C帧网络回调函数。
 * @par    销毁C帧网络回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] c_frame_id    C帧id。
 * @param [in] status        执行结果错误码。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_g_destroy_c_callback)(uint16_t c_frame_id, errcode_t status);

/**
 * @if Eng
 * @brief Struct of SLE c frame g callback function.
 * @else
 * @brief SLE C帧G端回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_c_frame_g_create_callback g_create_cb;             /*!< @if Eng G create net callback.
                                                                @else   创建C帧网络结果回调函数。 @endif */
    sle_c_frame_add_req_callback add_req_cb;               /*!< @if Eng Recv add req callback.
                                                                @else   收到入网请求回调函数。 @endif */
    sle_c_frame_add_complete_callback add_completed_cb;    /*!< @if Eng Add t to net callback.
                                                                @else   添加节点入网完成回调函数。 @endif */
    sle_c_frame_recv_data_callback recv_data_cb;           /*!< @if Eng Recv data callback.
                                                                @else   收到数据回调函数。 @endif */
    sle_c_frame_g_destroy_c_callback g_destroy_c_cb;       /*!< @if Eng Destroy C net callback.
                                                                @else   删除C帧网络结果回调函数。 @endif */
    sle_c_frame_delete_callback delete_channel_cb;         /*!< @if Eng Delete C link callback.
                                                                @else   删除C帧链路结果回调函数。 @endif */
} sle_c_frame_g_callbacks_t;

/**
 * @if Eng
 * @brief Callback invoked when t add g.
 * @par Callback invoked when t add g.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @param [in] c_frame_id   c frame id.
 * @param [in] status       error code.
 * @retval #void no return value.
 * @else
 * @brief  T加入C帧网络结果回调函数。
 * @par    T加入C帧网络结果回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @param [in] c_frame_id    C帧id。
 * @param [in] status        执行结果错误码。
 * @retval 无返回值。
 * @endif
 */
typedef void (*sle_c_frame_t_add_g_callback)(uint16_t c_frame_id, errcode_t status);

/**
 * @if Eng
 * @brief Struct of SLE c frame t callback function.
 * @else
 * @brief SLE C帧T端回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_c_frame_t_add_g_callback t_add_g_cb;                  /*!< @if Eng T add C net result callback.
                                                                   @else   T加入C帧网络结果回调函数。 @endif */
    sle_c_frame_add_complete_callback add_completed_cb;       /*!< @if Eng T add C net complete callback.
                                                                   @else   T加入C帧网络完成回调函数。 @endif */
    sle_c_frame_recv_data_callback recv_data_cb;              /*!< @if Eng Recv data callback.
                                                                   @else   收到数据回调函数。 @endif */
    sle_c_frame_delete_callback delete_channel_cb;            /*!< @if Eng Delete c link result callback.
                                                                   @else   删除C帧链路结果回调函数。 @endif */
} sle_c_frame_t_callbacks_t;

/**
 * @if Eng
 * @brief  use this function to create c frame net.
 * @par Description: use this function to create c frame net.
 * @param [in]  param c frame net param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  G节点创建C帧网络。
 * @par Description: G节点创建C帧网络。
 * @param [in]  param C帧网络参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_g_create_param(const sle_c_frame_g_create_param_t *param);

/**
 * @if Eng
 * @brief  use this function to enter c frame net.
 * @par Description: use this function to enter c frame net.
 * @param [in]  param t add c frame net param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  T节点请求加入C帧网络。
 * @par Description: T节点请求加入C帧网络。
 * @param [in]  param T节点请求加入C帧网络参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_t_add_g_req(const sle_c_frame_t_add_g_req_t *param);

/**
 * @if Eng
 * @brief  use this function to add t to c frame net.
 * @par Description: use this function to add t to c frame net.
 * @param [in]  param t add c frame net param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  G节点添加T节点入网。
 * @par Description: G节点添加T节点入网。
 * @param [in]  param G节点添加T节点入网参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_g_add_t(const sle_c_frame_g_add_t_t *param);

/**
 * @if Eng
 * @brief  use this function to delete channel.
 * @par Description: use this function to delete channel.
 * @param [in]  param delete channel param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  G/T节点删除链路。
 * @par Description: G/T节点删除链路。
 * @param [in]  param G/T节点删除链路参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_delete_channel(const sle_c_frame_delete_channel_t *param);

/**
 * @if Eng
 * @brief  use this function to destroy g.
 * @par Description: use this function to destroy g.
 * @param [in]  c_frame_id   g hdl.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  销毁g端。
 * @par Description: 销毁g端。
 * @param [in]  c_frame_id   G端hdl。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_destroy_g(uint16_t c_frame_id);

/**
 * @if Eng
 * @brief  use this function to send data.
 * @par Description: use this function to send data.
 * @param [in]  param send data param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  通过C帧网络发送数据。
 * @par Description: 通过C帧网络发送数据。
 * @param [in]  param 通过C帧网络发送数据参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_send_data(const sle_c_frame_send_data_t *param);

/**
 * @if Eng
 * @brief  use this function to get ma current clock.
 * @par Description: use this function to get ma current clock.
* @param [out]  sys_time current system clock value.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取MA当前时钟。
 * @par Description: 获取MA当前时钟。
 * @param [out]  sys_time 获取的当前系统时钟值。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_get_sys_time(uint32_t *sys_time);

/**
 * @if Eng
 * @brief  use this function to set phy param.
 * @par Description: use this function to set phy param.
 * @param [in]  param set phy param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  设置phy参数。
 * @par Description: 设置phy参数。
 * @param [in]  param 设置phy参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_set_phy(const sle_c_frame_set_phy_param_t *param);

/**
 * @if Eng
 * @brief  use this function to set private adv data.
 * @par Description: use this function to set private adv data.
 * @param [in]  adv_data   private adv data.
 * @param [in]  adv_hdl    Adv hdl
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  设置私有广播数据。
 * @par Description: 设置私有广播数据。
 * @param [in]  adv_data    私有广播数据。
 * @param [in]  adv_hdl     广播句柄。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_private_set_adv_data(const sle_announce_data_t *adv_data, uint8_t adv_hdl);

/**
 * @if Eng
 * @brief  use this function to register g cbk.
 * @par Description: use this function to register g cbk.
 * @param [in]  func g cbk.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  注册C帧G端回调。
 * @par Description: 注册C帧G端回调。
 * @param [in]  func C帧G端回调参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_register_g_cbk(const sle_c_frame_g_callbacks_t *func);

/**
 * @if Eng
 * @brief  use this function to register t cbk.
 * @par Description: use this function to register t cbk.
 * @param [in]  func t cbk.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  注册C帧T端回调。
 * @par Description: 注册C帧T端回调。
 * @param [in]  func C帧T端回调参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_c_frame_register_t_cbk(const sle_c_frame_t_callbacks_t *func);

#ifdef __cplusplus
}
#endif
#endif /* SLE_C_FRAME */
/**
 * @}
 */
