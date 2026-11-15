/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#if CHBA_LWIP_SWITCH
#include "sle_chba_netif_mng.h"
#include "lwip/netifapi.h"
#ifdef _PRE_WLAN_FEATURE_SLE_BRIDGE
#include "sle_chba_bridge.h"
#endif
#include "sle_common.h"
#include "sle_chba_manager.h"
#include "sle_chba_server.h"
#ifdef _PRE_SYSCHANNEL_FEATURE
#include "syschannel_wal_api.h"
#endif

#define SLE_CHBA_NETIF_NAME "sle"

struct netif g_sle_chba_netdev = {0};
uint32_t g_rx_dropped_cnt = 0;
unsigned long g_msgqueue_id = 0xFFFF;

static uint8_t chba_adapter_netdev_set_hwaddr_cb(struct netif *netif, u8_t *addr, u8_t len)
{
    unused(netif);
    unused(addr);
    unused(len);
    return 0;
}

errcode_t sle_chba_netdev_input_cb(uint8_t *data, uint16_t len)
{
    errcode_t rc = ERRCODE_SUCC;
    struct pbuf *p;
    /*
     * The packet has been retrieved from the transmission
     * medium. Build an skb around it, so upper layers can handle it
     */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    p = pbuf_alloc(PBUF_RAW, len + ETH_PAD_SIZE, PBUF_RAM);
#else
    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
#endif
    if (p == NULL) {
        chba_warn_log("chba rx: low on mem - packet dropped\n");
        g_rx_dropped_cnt++;
        return ERRCODE_MALLOC;
    }
    /*
     * 在以太网报头之前添加的字节数，以确保该报头之后的有效负载对齐。
     * 由于报头长度为14字节，如果没有此填充，IP报头中的地址将不会在32位边界上对齐。
     * 因此，将其设置为2可以加快32位平台的速度。
     */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);
#endif
    rc = pbuf_take(p, data, len);
    if (rc != ERRCODE_SUCC) {
        pbuf_free(p);
        chba_err_log("pbuf_take fail(%d)!!!\r\n", rc);
        g_rx_dropped_cnt++;
        return ERRCODE_MEMCPY;
    }
#ifdef _PRE_SYSCHANNEL_FEATURE
    if (
#ifdef ACHBA_SUPPORT
        g_chba_cfg.chba_mode == 0 &&
#endif
        syschannel_driverif_receive((void *)p) != 2) { //  2:  表示非syschannel报文，继续上报给协议栈；
        return ERRCODE_SUCC;                           // 其他：syschannel报文已上报;
    }
#endif
#ifdef _PRE_WLAN_FEATURE_SLE_BRIDGE
    if (!sle_chba_send_br(p)) {
        return ERRCODE_SUCC;
    }
#endif
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);
#endif
    sle_chba_recv_pkt_t recv_pkt = {.data_len = p->len,
                                    .data_ptr = (uint8_t *)(uintptr_t)p};
    /* Write metadata, and then pass to the receive level */
    int ret = osal_msg_queue_write_copy(g_msgqueue_id, &recv_pkt, sizeof(sle_chba_recv_pkt_t), 0);
    if (ret != 0) {
        pbuf_free(p);
        g_rx_dropped_cnt++;
    }
    return ERRCODE_SUCC;
}

errcode_t chba_adapter_netdev_init(void)
{
    (void)memset_s(&g_sle_chba_netdev, sizeof(struct netif), 0, sizeof(struct netif));
    g_sle_chba_netdev.drv_send         = sle_chba_send_pkt;
    g_sle_chba_netdev.link_layer_type  = WIFI_DRIVER_IF;
    g_sle_chba_netdev.drv_set_hwaddr   = chba_adapter_netdev_set_hwaddr_cb;
    g_sle_chba_netdev.hwaddr_len       = SLE_ADDR_LEN;
    // name用户可修改
    (void)memcpy_s(g_sle_chba_netdev.name, NETIF_NAMESIZE, SLE_CHBA_NETIF_NAME, sizeof(SLE_CHBA_NETIF_NAME));
    ip4_addr_t ip, netmask, gw;
    IP4_ADDR(&ip, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);
    errcode_t rc = netifapi_netif_add(&g_sle_chba_netdev, &ip, &netmask, &gw);
    if (rc != ERRCODE_SUCC) {
        chba_err_log("%s: create netif fail err(%d)!", __func__, rc);
        return ERRCODE_FAIL;
    }
    netif_set_default(&g_sle_chba_netdev);
    netifapi_netif_set_up(&g_sle_chba_netdev);
    netifapi_netif_set_link_down(&g_sle_chba_netdev);
#ifdef _PRE_SYSCHANNEL_FEATURE
    syschannel_netif_create();
#endif

    return ERRCODE_SUCC;
}

errcode_t chba_adapter_netdev_deinit(void)
{
#ifdef _PRE_SYSCHANNEL_FEATURE
    syschannel_netif_clear(&g_sle_chba_netdev);
#endif
    netifapi_netif_remove(&g_sle_chba_netdev);
    g_sle_chba_netdev.drv_send         = NULL;
    g_sle_chba_netdev.drv_set_hwaddr   = NULL;
    return ERRCODE_SUCC;
}

struct netif *sle_chba_netdev_get(void)
{
    return &g_sle_chba_netdev;
}

errcode_t sle_chba_netdev_wake_queue_cb(void)
{
#if defined(DRIVER_STATUS_CHECK) && (DRIVER_STATUS_CHECK)
    struct netif *dev = sle_chba_netdev_get();
    netifapi_wake_queue(dev);
#endif
    return ERRCODE_SUCC;
}

errcode_t sle_chba_netdev_stop_queue_cb(void)
{
#if defined(DRIVER_STATUS_CHECK) && (DRIVER_STATUS_CHECK)
    struct netif *dev = sle_chba_netdev_get();
    netifapi_stop_queue(dev);
#endif
    return ERRCODE_SUCC;
}

void sle_chba_send_pkt(struct netif *dev, struct pbuf *pbuf)
{
    unused(dev);
    sle_chba_netdev_driver_send(pbuf->payload, pbuf->len);
}

errcode_t sle_chba_netdev_set_link_up_cb(void)
{
    struct netif *dev = sle_chba_netdev_get();
    netifapi_netif_set_link_up(dev);
    return ERRCODE_SUCC;
}

errcode_t sle_chba_netdev_set_link_down_cb(void)
{
    struct netif *dev = sle_chba_netdev_get();
    netifapi_netif_set_link_down(dev);
    return ERRCODE_SUCC;
}

errcode_t sle_chba_sample_netdev_register_callbacks(void)
{
    sle_chba_netdev_callbacks_t cbk = { .stop_queue_cb = sle_chba_netdev_stop_queue_cb,
                                        .wake_queue_cb = sle_chba_netdev_wake_queue_cb,
                                        .set_link_up_cb = sle_chba_netdev_set_link_up_cb,
                                        .set_link_down_cb = sle_chba_netdev_set_link_down_cb,
                                        .netdev_input_cb = sle_chba_netdev_input_cb,
                                        };
    return sle_chba_netdev_register_callbacks(&cbk);
}

void sle_chba_sample_recv_pkt_thread(void)
{
#define SLE_CHBA_RECV_QUE_LEN 10
    int ret = osal_msg_queue_create("chba_recv_queue", SLE_CHBA_RECV_QUE_LEN,
        &g_msgqueue_id, 0, sizeof(sle_chba_recv_pkt_t));
    sle_chba_recv_pkt_t recv_pkt = {0};
    struct netif *dev = sle_chba_netdev_get();
    unsigned int len = sizeof(sle_chba_recv_pkt_t);
    while (1) {
        ret = osal_msg_queue_read_copy(g_msgqueue_id, &recv_pkt, &len, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS || len < sizeof(sle_chba_recv_pkt_t)) {
            chba_err_log("osal_msg_queue_read_copy fail(0x%x)\r\n", ret);
            continue;
        }
        driverif_input(dev, (struct pbuf *)recv_pkt.data_ptr);
    }
}
#endif