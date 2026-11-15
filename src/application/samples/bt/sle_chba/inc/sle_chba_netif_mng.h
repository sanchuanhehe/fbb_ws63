/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef SLE_CHBA_NETIF_MNG_H
#define SLE_CHBA_NETIF_MNG_H
#if CHBA_LWIP_SWITCH
#include "errcode.h"
#include "stdint.h"
#include "lwip/pbuf.h"

errcode_t chba_adapter_netdev_init(void);
errcode_t chba_adapter_netdev_deinit(void);
errcode_t sle_chba_sample_netdev_register_callbacks(void);
struct netif *sle_chba_netdev_get(void);
void sle_chba_send_pkt(struct netif *dev, struct pbuf *pbuf);
void sle_chba_sample_recv_pkt_thread(void);
#endif
#endif