/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#if CHBA_LWIP_SWITCH
#include "sle_chba_bridge.h"
#include "sle_chba_manager.h"
#include "sle_chba_netif_mng.h"

#ifdef _PRE_WLAN_FEATURE_SLE_BRIDGE

sle_tx_wifi_pbuf_t g_sle_tx_wifi_pbuf_cb = NULL;
uint32_t g_wifi_to_sle_pkts_cnt = 0;
uint32_t g_sle_to_wifi_pkts_cnt = 0;
static uint32_t wifi_tx_sle_pbuf(struct pbuf *lwip_buf)
{
    sle_chba_netdev_driver_send(lwip_buf->payload, lwip_buf->len);
    g_wifi_to_sle_pkts_cnt++;
    return 0;
}

bool sle_chba_send_br(struct pbuf *pbuf)
{
    if (g_sle_tx_wifi_pbuf_cb == NULL) {
        return true;
    }
    struct netif *dev = sle_chba_netdev_get();
    if (memcmp(dev->hwaddr, pbuf->payload, SLE_ADDR_LEN) == 0) {
        return true;
    }
    // 组播
    if ((((uint8_t *)pbuf->payload)[0] & MAC_MUTLTICAST_MASK) == 1) {
        struct pbuf *pbuf_new = pbuf_clone(PBUF_RAW, PBUF_RAM, pbuf);
        if (pbuf_new == NULL) {
            return true;
        }
        g_sle_to_wifi_pkts_cnt++;
        g_sle_tx_wifi_pbuf_cb(pbuf_new);
        pbuf_free(pbuf_new);
        return true;
    }
    g_sle_to_wifi_pkts_cnt++;
    g_sle_tx_wifi_pbuf_cb(pbuf);
    pbuf_free(pbuf);
    return false;
}

void sle_wifi_bridge_register(wifi_tx_sle_pbuf_t *wifi_tx_sle_func_ptr, sle_tx_wifi_pbuf_t sle_tx_wifi, uint8_t type)
{
    // 注册
    if (type == SLE_WIFI_BRIDGE_OPEN) {
        *wifi_tx_sle_func_ptr = wifi_tx_sle_pbuf;
        g_sle_tx_wifi_pbuf_cb = sle_tx_wifi;
    } else if (type == SLE_WIFI_BRIDGE_CLOSE) { // 去注册
        *wifi_tx_sle_func_ptr = NULL;
        g_sle_tx_wifi_pbuf_cb = NULL;
    }
}
#endif
#endif