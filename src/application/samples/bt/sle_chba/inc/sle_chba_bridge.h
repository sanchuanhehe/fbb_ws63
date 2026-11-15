/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef SLE_CHBA_BRIDGE_H
#define SLE_CHBA_BRIDGE_H
#include "sle_chba_netif_mng.h"
#if CHBA_LWIP_SWITCH
#include "lwip/netifapi.h"

#ifdef _PRE_WLAN_FEATURE_SLE_BRIDGE

enum {
    SLE_WIFI_BRIDGE_CLOSE,
    SLE_WIFI_BRIDGE_OPEN,
};

#define MAC_MUTLTICAST_MASK 0x1

typedef uint32_t (*sle_tx_wifi_pbuf_t)(struct pbuf *lwip_buf);
typedef uint32_t (*wifi_tx_sle_pbuf_t)(struct pbuf *lwip_buf);

bool sle_chba_send_br(struct pbuf *pbuf);
void sle_wifi_bridge_register(wifi_tx_sle_pbuf_t *wifi_tx_sle_func_ptr, sle_tx_wifi_pbuf_t sle_tx_wifi, uint8_t type);
#endif
#endif
#endif