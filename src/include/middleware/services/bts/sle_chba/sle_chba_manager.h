/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 *
 * Description: SLE CHBA NET DEVICE PUBLIC API module.
 */
#ifndef SLE_IP_SERVER_H
#define SLE_IP_SERVER_H

#include "sle_common.h"
#include "errcode.h"

enum sle_chba_role {
    CHBA_ROLE_AP, // AP模式下，设备作为无线网络中心，管理STA，提供数据中继，支持多设备连接。sle作为client端
    CHBA_ROLE_STA, // STA模式下，为叶子节点。终端设备连接到无线网络，进行数据传输。sle作为server端
};

typedef struct {
    uint8_t conn_id;
    sle_addr_t remote_addr;
    uint32_t tx_pkts_cnt;
    uint32_t tx_bytes;
    uint32_t rx_pkts_cnt;
    uint32_t rx_bytes;
} sle_ip_link_info;

errcode_t sle_chba_netdev_create(uint8_t chba_role, uint8_t chba_mode);

errcode_t sle_chba_netdev_destroy(void);

errcode_t sle_chba_netdev_add_link(uint16_t conn_id, const sle_addr_t *remote_addr);

errcode_t sle_chba_netdev_del_link(uint16_t conn_id, const sle_addr_t *remote_addr);

errcode_t sle_chba_netdev_get_linkinfo(uint16_t conn_id, sle_ip_link_info *link);

errcode_t sle_chba_netdev_driver_send(uint8_t *data, uint16_t len);

typedef errcode_t (*sle_chba_netdev_stop_queue_callback)(void);
typedef errcode_t (*sle_chba_netdev_wake_queue_callback)(void);
typedef errcode_t (*sle_chba_netdev_set_link_up_callback)(void);
typedef errcode_t (*sle_chba_netdev_set_link_down_callback)(void);
typedef errcode_t (*sle_chba_netdev_input_callback)(uint8_t *data, uint16_t len);

typedef struct {
    sle_chba_netdev_stop_queue_callback stop_queue_cb;              /*!< @if Eng Chba net device stop queue callback.
                                                                    @else   CHBA网络设备停止发送队列回调函数。 @endif */
    sle_chba_netdev_wake_queue_callback wake_queue_cb;              /*!< @if Eng Chba net device wake queue callback.
                                                                    @else   CHBA网络设备唤醒发送队列回调函数。 @endif */
    sle_chba_netdev_set_link_up_callback set_link_up_cb;            /*!< @if Eng Chba net device set link up callback.
                                                                    @else   CHBA网络设备设置链路启用回调函数。 @endif */
    sle_chba_netdev_set_link_down_callback set_link_down_cb;        /*!< @if Eng Chba net device set link down callback.
                                                                    @else   CHBA网络设备设置链路禁用回调函数。 @endif */
    sle_chba_netdev_input_callback netdev_input_cb;                 /*!< @if Eng Chba net device input callback.
                                                                    @else   CHBA网络设备数据上报回调函数。 @endif */
} sle_chba_netdev_callbacks_t;

errcode_t sle_chba_netdev_register_callbacks(sle_chba_netdev_callbacks_t *func);

#endif