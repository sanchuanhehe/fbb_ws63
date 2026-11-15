/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE MESH PROVISION API.
 */

/**
 * @defgroup bluetooth_bts_mesh_provision BLE MESH PROVISION API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_PROVISION_H
#define BLE_MESH_PROVISION_H

#include <stdint.h>

#include "bts_def.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif

#define BLE_MESH_RMT_PROV_EX_SCAN_AD_TYPE_LEN (16)

/**
 * @if Eng
 * @brief Enum of prov bearer type.
 * @else
 * @brief 配网承载类型
 * @endif
 */
typedef enum {
    BLE_MESH_PROV_ADV = BIT(0),         /*!< @if Eng advertising bearer
                                             @else   广播承载 @endif */
    BLE_MESH_PROV_GATT = BIT(1),        /*!< @if Eng GATT bearer
                                             @else   GATT承载 @endif */
} ble_mesh_prov_bearer_t;

/**
 * @if Eng
 * @brief Enum of prov flow mask.
 * @else
 * @brief 配网流程标记
 * @endif
 */
typedef enum {
    BLE_MESH_PROV_KEY_REFRESH_PHASE2 = 1,           /*!< @if Eng key refresh phase 2
                                                         @else   密钥刷新阶段2 @endif */
    BLE_MESH_PROV_IV_UPDATE_IN_PROGRESS = 2,        /*!< @if Eng IV update in progress
                                                         @else   IV 更新中 @endif */
} ble_mesh_prov_flags_t;

/**
 * @if Eng
 * @brief Enum of remote prov flow mask.
 * @else
 * @brief 远程配网流程标记
 * @endif
 */
typedef enum {
    BLE_MESH_RMT_PROV_NODE_REFRESH_DEVKEY = 0x00,           /*!< @if Eng device key refresh procedure
                                                                 @else   设备密钥刷新流程 @endif */
    BLE_MESH_RMT_PROV_NODE_REFRESH_ADDR = 0x01,             /*!< @if Eng node address refresh procedure
                                                                 @else   节点地址刷新流程 @endif */
    BLE_MESH_RMT_PROV_NODE_REFRESH_COMPOSITION = 0x02,      /*!< @if Eng node composition refresh procedure
                                                                 @else   节点composition刷新流程 @endif */
} ble_mesh_rmt_prov_node_refresh_t;

/**
 * @if Eng
 * @brief Enum of prov type.
 * @else
 * @brief 配网方式
 * @endif
 */
typedef enum {
    BLE_MESH_PROV_TYPE_SIG_MESH = 1,           /*!< @if Eng Sig mesh standard provision
                                                    @else  Sig mesh标准配网 @endif */
    BLE_MESH_PROV_TYPE_CUSTOM   = 2,           /*!< @if Eng Custom define provision
                                                     @else   用户自定义配网 @endif */
} ble_mesh_prov_type_t;

/**
 * @if Eng
 * @brief struct of rmt prov extend scan ad filter.
 * @else
 * @brief 远程配网扩展扫描ad filter结构
 * @endif
 */
typedef struct ble_mesh_rmt_prov_ex_scan {
    uint8_t ad_count;                                       /*!< @if Eng remote prov extend scan ad count
                                                                 @else  远程配网扩展扫描ad计数 @endif */
    uint8_t ad_type[BLE_MESH_RMT_PROV_EX_SCAN_AD_TYPE_LEN]; /*!< @if Eng remote prov extend scan ad type
                                                                  @else   远程配网扩展扫描ad类型 @endif */
} ble_mesh_rmt_prov_ex_scan_t;

/**
 * @if Eng
 * @brief struct of local net info.
 * @else
 * @brief 本地网络信息
 * @endif
 */
typedef struct {
    uint16_t addr;                                          /*!< @if Eng Local Unicast Address
                                                                 @else  本地单播地址 @endif */
    uint16_t net_idx;                                       /*!< @if Eng Local Network Key Index
                                                                 @else  本地网络Key索引 @endif */
    uint8_t net_key[BLE_MESH_NET_KEY_LEN];                  /*!< @if Eng Local Network key
                                                                 @else  本地网络key @endif */
    uint8_t dev_key[BLE_MESH_DEV_KEY_LEN];                  /*!< @if Eng Local Device key
                                                                 @else  本地设备key @endif */
    uint32_t iv_idx;                                        /*!< @if Eng Local Current IV Index
                                                                 @else  本地当前IV索引 @endif */
    uint8_t flags;                                          /*!< @if Eng Local Key Refresh Flag and IV Update Flag
                                                                 @else  本地密钥更新标志和IV更新标志 @endif */
} ble_mesh_local_net_info_t;

/**
 * @if Eng
 * @brief callback function when receiving unprovisioned beacon.
 * @par after the callback function is registered, the unprovisioned beacon received through this interface.
 * @attention 1. This function is called in mesh_service context,should not be blocked or do long time waiting.
 * @param [in] uuid     UUID of the unprovisioned device.
 * @param [in] oob_info OOB information.
 * @param [in] uri_hash uri of the unprovisioned device.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  收到未入网beacon报文回调接口。
 * @par    注册该回调函数之后，配网器收到报文会通过此接口上报。
 * @attention  1. 该回调函数运行于mesh_service线程，不能阻塞或长时间等待。
 * @param  [in] uuid     待入网设备UUID。
 * @param  [in] oob_info 入网OOB信息。
 * @param  [in] uri_hash 待入网设备URI信息。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_unprov_beacon_recv_callback)(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info,
                                                     uint32_t *uri_hash);

/**
 * @if Eng
 * @brief callback function when receiving remote device unprovisioned beacon.
 * @par after the callback function is registered, the remote device unprovisioned beacon received through this func.
 * @attention 1. This function is called in mesh_service context, should not be blocked or do long time waiting.
 * @param [in] uuid     UUID of the unprovisioned device.
 * @param [in] oob_info OOB information.
 * @param [in] uri_hash uri of the unprovisioned device.
 * @param [in] rssi     rssi of the unprovisioned device.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  收到远程配网设备未入网beacon报文回调接口。
 * @par    注册该回调函数之后，配网器收到报文会通过此接口上报。
 * @attention  1. 该回调函数运行于mesh_service线程，不能阻塞或长时间等待。
 * @param  [in] uuid     待入网设备UUID。
 * @param  [in] oob_info 入网OOB信息。
 * @param  [in] uri_hash 待入网设备URI信息。
 * @param  [in] rssi     待入网设备RSSI信息。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_rmt_unprov_beacon_recv_callback)(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info,
                                                         uint32_t *uri_hash, int8_t rssi);

/**
 * @if Eng
 * @brief callback function when mesh link is opened.
 * @par after the callback function is registered, this func will be called when mesh adv link is opened.
 * @attention none.
 * @param [in] bearer  bearer type   { @ref ble_mesh_prov_bearer_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  mesh链路打开回调接口。
 * @par    mesh链路打开回调接口。
 * @attention 无
 * @param  [in] bearer   承载类型 { @ref ble_mesh_prov_bearer_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_link_open_callback)(ble_mesh_prov_bearer_t bearer);

/**
 * @if Eng
 * @brief callback function when mesh link is closed.
 * @par after the callback function is registered, this func will be called when mesh adv link is closed.
 * @attention none.
 * @param [in] bearer  bearer type   { @ref ble_mesh_prov_bearer_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  mesh链路关闭回调接口。
 * @par    mesh链路关闭回调接口。
 * @attention 无
 * @param  [in] bearer   承载类型 { @ref ble_mesh_prov_bearer_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_link_close_callback)(ble_mesh_prov_bearer_t bearer);

/**
 * @if Eng
 * @brief mesh device provision complete callback.
 * @par mesh device provision complete callback.
 * @attention None
 * @param [in] net_idx net key index.
 * @param [in] addr    mesh network address.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  mesh配网完成回调。
 * @par    mesh配网完成回调。
 * @attention 无
 * @param  [in] net_idx   网络密钥索引。
 * @param  [in] addr      mesh网络地址。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_prov_complete_callback)(uint16_t net_idx, uint16_t addr);

/**
 * @if Eng
 * @brief mesh node added callback.
 * @par mesh node added callback.
 * @attention None.
 * @param  None.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  mesh节点增加回调。
 * @par    mesh节点增加回调。
 * @attention 无
 * @param  无
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_node_added_callback)(uint16_t net_idx, uint8_t uuid[BLE_MESH_UUID_LEN],
                                             uint16_t addr, uint8_t num_elem);

/**
 * @if Eng
 * @brief mesh node reset callback.
 * @par mesh node callback.
 * @attention None.
 * @param  None.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @else
 * @brief  mesh节点重置回调。
 * @par    mesh节点重置回调。
 * @attention 无
 * @param  无
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see ble_mesh_prov_t
 * @endif
 */
typedef void (*ble_mesh_node_reset_callback)(void);

/**
 * @if Eng
 * @brief Struct of BLE prov callback function.
 * @else
 * @brief BLE配网回调函数接口定义。
 * @endif
 */
typedef struct ble_mesh_prov {
    uint8_t static_value_len;                                               /*!< @if Eng static oob auth value len
                                                                                 @else   静态OOB鉴权数据长度 @endif */
    uint8_t *static_auth_value;                                             /*!< @if Eng static oob auth value
                                                                                 @else   静态OOB鉴权数据 @endif */
    const char *uri;                                                        /*!< @if Eng uri hash value
                                                                                 @else   uri hash 值 @endif */
    ble_mesh_unprov_beacon_recv_callback unprovisioned_beacon_recv;         /*!< @if Eng unprovisioned beacon
                                                                                         receive callback
                                                                                 @else   收到未入网beacon报文回调函数 @endif */
    ble_mesh_rmt_unprov_beacon_recv_callback rmt_unprovisioned_beacon_recv; /*!< @if Eng remote unprovisioned beacon
                                                                                         receive callback
                                                                                 @else   收到未入网beacon报文回调函数 @endif */
    ble_mesh_link_open_callback  link_open;                                 /*!< @if Eng mesh link open callback
                                                                                 @else   mesh 链路打开回调 @endif */
    ble_mesh_link_close_callback link_close;                                /*!< @if Eng mesh link close callback
                                                                                 @else   mesh 链路关闭回调 @endif */
    ble_mesh_prov_complete_callback provisioned_complete;                   /*!< @if Eng provision complete callback
                                                                                 @else   配网完成回调 @endif */
    ble_mesh_node_reset_callback reset;                                     /*!< @if Eng mesh node reset callback
                                                                                 @else   mesh节点重置回调函数 @endif */
    ble_mesh_node_added_callback node_added;                                /*!< @if Eng mesh node added callback
                                                                                 @else   mesh节点增加回调函数 @endif */
} ble_mesh_prov_t;

/**
 * @if Eng
 * @brief Use this funtion to init unprovisioned device.
 * @par   Use this funtion to init unprovisioned device.
 * @attention NULL
 * @param  [in] bearers bearers type { @ref ble_mesh_prov_bearer_t }.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  未配网设备初始化。
 * @par    未配网设备初始化。
 * @attention 无
 * @param  [in] bearers 配网承载类型 { @ref ble_mesh_prov_bearer_t }。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t ble_mesh_prov_enable(ble_mesh_prov_bearer_t bearers);

/**
 * @if Eng
 * @brief Use this funtion to init provisioner.
 * @par   Use this funtion to init provisioner.
 * @attention NULL
 * @param  [in] addr Unicast address of the primary element.
 * @param  [in] net_idx Index of the netkey.
 * @param  [in] flags flags bitmask { @ref ble_mesh_prov_flags_t }.
 * @param  [in] iv_index current value of the IV Index.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  配网器初始化。
 * @par    配网器初始化。
 * @attention 无
 * @param  [in] addr    本端地址。
 * @param  [in] net_idx 网络密钥索引。
 * @param  [in] flags   配网flags参数 { @ref ble_mesh_prov_flags_t }。
 * @param  [in] iv_index IV索引。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_provisioner_enable(uint16_t addr, uint16_t net_idx, uint8_t flags, uint32_t iv_index);

/**
 * @if Eng
 * @brief Use this funtion to open provision link.
 * @par Use this funtion to open provision link.
 * @attention None
 * @param [in] uuid    UUID of the unprovisioned device.
 * @param [in] net_idx net key index.
 * @param [in] addr    mesh network address.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  打开配网链路。
 * @par    打开配网链路。
 * @attention 无
 * @param  [in] uuid    待入网设备UUID。
 * @param  [in] net_idx 网络密钥索引。
 * @param  [in] addr    mesh网络地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_adv_open(const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t addr);

/**
 * @if Eng
 * @brief Use this funtion to specifies the MAC address of the unprovisioned device.
 * @par Use this funtion to specifies the MAC address of the unprovisioned device.
 * @attention None
 * @param [in] addr    unprovisioned device addr.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  配网器指定待入网设备的mac地址。
 * @par    配网器指定待入网设备的mac地址。
 * @attention 无
 * @param  [in] addr    待入网设备的MAC地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_prov_addr(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get the MAC address of the unprovisioned device on provisioner.
 * @par Use this funtion to get the MAC address of the unprovisioned device on provisioner.
 * @attention None
 * @param [in] addr    unprovisioned device addr.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  配网器获取待入网设备的mac地址。
 * @par    配网器获取待入网设备的mac地址。
 * @attention 无
 * @param  [in] addr    待入网设备的MAC地址。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
void ble_mesh_get_addr_net(bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to start remote provisioning scan.
 * @par Use this funtion to start remote provisioning scan.
 * @attention None
 * @param [in] net_idx         net key index.
 * @param [in] addr            remote provisioning server mesh network address.
 * @param [in] uuid            UUID of the unprovisioned device.
 * @param [in] items_limit     maximum number of scanned items to be reported.
 * @param [in] timeout         time limit for a scan.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief 开始远程配网扫描。
 * @par    开始远程配网扫描。
 * @attention 无
 * @param  [in] net_idx         网络密钥索引。
 * @param  [in] addr            远程配网服务端的mesh网络地址。
 * @param  [in] uuid            待入网设备UUID。
 * @param  [in] items_limit     扫描结果个数限制。
 * @param  [in] timeout         扫描超时时间。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_start_rmt_scan(uint16_t net_idx, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN],
                                     uint8_t items_limit, uint8_t timeout);

/**
 * @if Eng
 * @brief Use this funtion to start extend remote provisioning scan.
 * @par Use this funtion to start extend remote provisioning scan.
 * @attention None
 * @param [in] net_idx         net key index.
 * @param [in] addr            remote provisioning server mesh network address.
 * @param [in] uuid            UUID of the unprovisioned device.
 * @param [in] timeout         time limit for a scan.
 * @param [in] ad_filter       ad filter struct @ref ble_mesh_rmt_prov_ex_scan_t.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief 开始远程配网扩展扫描。
 * @par    开始远程配网扩展扫描。
 * @attention 无
 * @param  [in] net_idx         网络密钥索引。
 * @param  [in] addr            远程配网服务端的mesh网络地址。
 * @param  [in] uuid            待入网设备UUID。
 * @param  [in] timeout         扫描超时时间。
 * @param  [in] ad_filter       ad过滤结构体 @ref ble_mesh_rmt_prov_ex_scan_t。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_start_rmt_ex_scan(uint16_t net_idx, uint16_t addr, const uint8_t *uuid, uint8_t timeout,
                                        ble_mesh_rmt_prov_ex_scan_t *ad_filter);

/**
 * @if Eng
 * @brief Use this funtion to establish the provisioning bearer
 *        between a node supporting the remote provisioning server model and an unprovisioned device.
 * @par Use this funtion to establish the provisioning bearer
 *      between a node supporting the remote provisioning server model and an unprovisioned device.
 * @attention If UUID field is present, the NPPI Procedure field is prohibited;
 *            otherwise, the NPPI Procedure field is mandatory.
 * @param [in] uuid         UUID of the unprovisioned device.
 * @param [in] net_idx      net key index.
 * @param [in] rmt_addr     remote provisioning server mesh network address.
 * @param [in] prov_addr    maximum number of scanned items to be reported.
 * @param [in] nppi         node provisioning protocol interface procedure { @ref ble_mesh_rmt_prov_node_refresh_t }.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  打开远程配网服务端和未入网节点的链路。
 * @par    打开远程配网服务端和未入网节点的链路。
 * @attention 如果指定了uuid则参数nppi不生效，否则nppi参数必选。
 * @param  [in] uuid         待入网设备UUID。
 * @param  [in] net_idx      网络密钥索引。
 * @param  [in] rmt_addr     远程配网服务端的mesh网络地址。
 * @param  [in] prov_addr    未入网节点分配的mesh地址。
 * @param  [in] nppi         节点配网协议接口{ @ref ble_mesh_rmt_prov_node_refresh_t }。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_rmt_adv_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t rmt_addr, uint16_t prov_addr, uint8_t nppi);

/**
 * @if Eng
 * @brief Use this interface to make the provisioned device to unprovisioned.
 * @par Use this interface to make the provisioned device to unprovisioned.
 * @attention None.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.The node reset result is returned through the node_reset interface callback
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口将已入网的mesh节点离网。
 * @par    使用此接口将已入网的mesh节点离网。
 * @attention 无。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。离网结果通过注册的node_reset接口回调 @see ble_mesh_node_reset_callback
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_node_reset(void);

/**
 * @if Eng
 * @brief Use this interface to start the unprovision device to send unprovision beacon.
 * @par Use this interface to start the unprovision device to send unprovision beacon.
 * @attention None.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口启动待入网设备发送未入网beacon。
 * @par    使用此接口启动待入网设备发送未入网beacon。
 * @attention 无。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_prov_start_unprov_beacon_send(void);

/**
 * @if Eng
 * @brief Use this interface to stop the unprovision device to send unprovision beacon.
 * @par Use this interface to stop the unprovision device to send unprovision beacon.
 * @attention None.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口停止待入网设备发送未入网beacon。
 * @par    使用此接口停止待入网设备发送未入网beacon。
 * @attention 无。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_prov_stop_unprov_beacon_send(void);

/**
 * @if Eng
 * @brief Use this interface to get the device provision status.
 * @par Use this interface to get the device provision status.
 * @attention None.
 * @retval false unprovisioned; true provisioned.
 * @par Dependency:
 * @else
 * @brief  使用此接口获取设备的入网状态。
 * @par    使用此接口获取设备的入网状态。
 * @attention 无。
 * @retval false 未配网; true 已入网;
 * @par 依赖:
 * @endif
 */
bool ble_mesh_device_is_provisioned(void);

/**
 * @if Eng
 * @brief Use this interface to get the device mesh primary address.
 * @par Use this interface to get the device mesh primary address.
 * @attention None.
 * @retval mesh primary address.
 * @par Dependency:
 * @else
 * @brief  使用此接口获取设备的mesh主地址。
 * @par    使用此接口获取设备的mesh主地址。
 * @attention 无。
 * @retval 设备mesh主地址
 * @par 依赖:
 * @endif
 */
uint16_t ble_mesh_get_primary_addr(void);

/**
 * @if Eng
 * @brief Use this interface to set thelink id for exchanging provision adv.
 * @par Use this interface to set thelink id for exchanging provision adv.
 * @attention After the link ID is set, only provision adv of the corresponding link ID is received.
 * @param [in] link_id         link id.
 * @param  [in] is_filter      whether to filter out prov adv data.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口设置用于配网报文交互的链路ID。
 * @par    使用此接口设置用于配网报文交互的链路ID。
 * @attention 设置link id后，只会接收对应link id的配网广播。
 * @param  [in] link_id        链路id。
 * @param  [in] is_filter      是否过滤配网广播数据。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_prov_link_id(uint32_t link_id, bool is_filter);

/**
 * @if Eng
 * @brief Use this interface to set the device local network information.
 * @par Use this interface to set the device local network information.
 * @attention None.
 * @param [in] net_info         Local Network Information.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口设置设备的本地网络信息。
 * @par    使用此接口设置设备的本地网络信息。
 * @attention 无。
 * @param  [in] net_info        设备的本地网络信息。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_local_net_info(ble_mesh_local_net_info_t *net_info);

/**
 * @if Eng
 * @brief Use this interface to set the provision type of the device.
 * @par Use this interface to set the provision type of the device.
 * @attention None.
 * @param [in] prov_type         provision type.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  使用此接口设置设备的配网方式。
 * @par    使用此接口设置设备的配网方式。
 * @attention 无。
 * @param  [in] prov_type        配网方式
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_set_prov_type(ble_mesh_prov_type_t prov_type);

/**
 * @if Eng
 * @brief This API is called on a remote provisioning server node to stop remote provisioning scan.
 * @par This API is called on a remote provisioning server node to stop remote provisioning scan.
 * @attention None
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief 在远程配网服务节点，执行此接口停止远程配网扫描。
 * @par    在远程配网服务节点，执行此接口停止远程配网扫描。
 * @attention 无
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_stop_rmt_scan_local(void);

/**
 * @if Eng
 * @brief This API is called on a remote provisioning server node to start remote provisioning scan.
 * @par This API is called on a remote provisioning server node to start remote provisioning scan.
 * @attention If the UUID is all 0s, requesting a scan for all unprovisioned devices.
 * @param [in] net_idx         net key index.
 * @param [in] addr            remote provisioning client mesh network address.
 * @param [in] uuid            UUID of the unprovisioned device.
 * @param [in] items_limit     maximum number of scanned items to be reported.
 * @param [in] timeout         time limit for a scan.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief 在远程配网服务节点，执行此接口启动远程配网扫描。
 * @par    在远程配网服务节点，执行此接口启动远程配网扫描。
 * @attention 如果UUID为全0，则扫描全部未入网设备。
 * @param  [in] net_idx         网络密钥索引。
 * @param  [in] addr            远程配网客户端的mesh网络地址。
 * @param  [in] uuid            待入网设备UUID。
 * @param  [in] items_limit     扫描结果个数限制。
 * @param  [in] timeout         扫描超时时间。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_pb_start_rmt_scan_local(uint16_t net_index, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN],
                                           uint8_t items_limit, uint8_t timeout);
#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
