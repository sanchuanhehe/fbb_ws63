/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: syschannel
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_net.h"
#include "securec.h"
#include "hcc_if.h"
#include "syschannel_host_adapt.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "hcc_cfg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

oal_lwip_netif* g_lwip_netif = OSAL_NULL;

osal_u32 syschannel_host_alloc_netbuf(hcc_queue_type queue_id, osal_u32 len, osal_u8 **buf, osal_u8 **user_param)
{
    oal_lwip_buf *pbuf = OSAL_NULL;

    unref_param(queue_id);
    if (buf == OSAL_NULL || user_param == OSAL_NULL) {
        return OSAL_NOK;
    }

    pbuf = pbuf_alloc(PBUF_RAW, (osal_u16)len, PBUF_RAM);
    if (pbuf == OSAL_NULL) {
        *buf = OSAL_NULL;
        *user_param = OSAL_NULL;
        return OSAL_NOK;
    }

    *buf = (osal_u8 *)pbuf->payload;
    *user_param = (osal_u8 *)pbuf;
    return OSAL_OK;
}

osal_void syschannel_host_netbuf_free(hcc_queue_type queue_id, osal_u8 *buf, osal_u8 *user_param)
{
    unref_param(queue_id);
    unref_param(buf);
    if (user_param == OSAL_NULL) {
        return;
    }

    pbuf_free((oal_lwip_buf *)user_param);
}
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
typedef osal_void(*syschannel_send_pkt_fn)(osal_void *buf);
syschannel_send_pkt_fn g_syschannel_send_pkt_fn = OSAL_NULL;

osal_void syschannel_send_pkt_register(osal_void *pkt_fn)
{
    g_syschannel_send_pkt_fn = (syschannel_send_pkt_fn)pkt_fn;
}
#endif
osal_u32 syschannel_host_rx_data_process(hcc_queue_type queue_id, osal_u8 stype,
    osal_u8 *buf, osal_u32 len, osal_u8 *user_param)
{
    oal_lwip_buf *lwip_buf = OSAL_NULL;
    syschannel_hdr_stru *syschannel_hdr = OSAL_NULL;
    osal_u16 data_len;
    osal_u8 pad_offset;

    unref_param(queue_id);
    unref_param(stype);
    unref_param(buf);
    unref_param(len);

    if (user_param == OSAL_NULL || g_lwip_netif == OSAL_NULL) {
        osal_printk("syschannel_host_rx_data_process:: param/netif null!\r\n");
        return OSAL_NOK;
    }

    lwip_buf = (oal_lwip_buf *)user_param;
    if (pbuf_header(lwip_buf, (osal_s16)(-hcc_get_head_len())) != OSAL_OK) {
        osal_printk("syschannel_host_rx_data_process:: headroom is not enough\r\n");
        goto pbuf_fail;
    }

    syschannel_hdr = (syschannel_hdr_stru *)lwip_buf->payload;
    data_len = syschannel_hdr->date_len;
    /* 转下字节序 */
    data_len = syschannel_ntohs(data_len);
    pad_offset = syschannel_hdr->pad_offset;
    if (pbuf_header(lwip_buf, (osal_s16)(-SYSCHANNEL_HDR_LEN)) != OSAL_OK) {
        goto pbuf_fail;
    }

    /* 判断是否经过4字节对齐偏移 */
    if ((pad_offset != 0) && pbuf_header(lwip_buf, (osal_s16)(-pad_offset)) != OSAL_OK) {
        goto pbuf_fail;
    }

    lwip_buf->len = data_len;
    lwip_buf->tot_len = data_len;
    /* 将payload地址前移2字节 */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    /* 赋值 */
    pbuf_header(lwip_buf, ETH_PAD_SIZE);
#endif

#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
    if (g_syschannel_send_pkt_fn != OSAL_NULL) {
        g_syschannel_send_pkt_fn((osal_void *)lwip_buf);
        pbuf_free(lwip_buf);
        return OSAL_OK;
    }
#endif

    /* 上报协议栈 */
    driverif_input(g_lwip_netif, lwip_buf);
    return OSAL_OK;
pbuf_fail:
    pbuf_free(lwip_buf);
    return OSAL_NOK;
}

osal_u32 syschannel_pbuf_adapt(oal_lwip_buf *lwip_buf, osal_u16 data_len)
{
    osal_u32 old_addr;
    osal_u32 new_addr;
    osal_s8 pad_offset;
    syschannel_hdr_stru *syschannel_hdr = OSAL_NULL;

    /* 先进行地址4字节对齐，记录下偏移值 */
    old_addr = (uintptr_t)lwip_buf->payload;
    new_addr = channel_round_down(old_addr, SYSCHANNEL_ADDR_ALIGN); /* HCC需要4字节对齐 */
    pad_offset = (osal_s8)(old_addr - new_addr);
    if ((pad_offset != 0) && (pbuf_header(lwip_buf, (osal_s16)pad_offset) != OSAL_OK)) {
        osal_printk("syschannel_host_tx_data_adapt:: headroom is not enough\r\n");
        return OSAL_NOK;
    }

    /* 先push syschannel头，保存帧体真实长度 */
    if (pbuf_header(lwip_buf, (osal_s16)SYSCHANNEL_HDR_LEN) != OSAL_OK) {
        return OSAL_NOK;
    }
    syschannel_hdr = (syschannel_hdr_stru *)lwip_buf->payload;
    /* 转下字节序 */
    syschannel_hdr->date_len = syschannel_htons(data_len);
    syschannel_hdr->pad_offset = pad_offset;

    /* 再push hcc头 */
    if (pbuf_header(lwip_buf, (osal_s16)sizeof(hcc_header)) != OSAL_OK) {
        return OSAL_NOK;
    }
    return OSAL_OK;
}

osal_u32 syschannel_liteos_host_tx_data_adapt(oal_lwip_buf *lwip_buf, syschannel_service_type type, osal_u32 sub_type)
{
    osal_u32 ret = OSAL_NOK;
    hcc_transfer_param param;
    osal_u16 fc_flag;
    osal_u8 queue_id;
    osal_u16 data_len = 0;
    osal_u32 payload_len = 0;
    syschannel_handler *syschannel_handler = OSAL_NULL;
    osal_u8 *payload = OSAL_NULL;

#ifdef _PRE_SYSCHANNEL_DEBUG
    osal_printk("syschannel_liteos_host_tx_data_adapt\r\n");
#endif
    syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_handler is inuse\r\n");
        return OSAL_NOK;
    }

    /* 记录帧体真实长度 */
    data_len = (osal_u16)lwip_buf->len;
    syschannel_config_hcc_flowctrl_type(type, &fc_flag, &queue_id);
    syschannel_init_hcc_service_hdr(&param, type, sub_type, queue_id, fc_flag);
    param.user_param = (osal_u8 *)lwip_buf;

    /* hcc + syschannel头填充 */
    if (syschannel_pbuf_adapt(lwip_buf, data_len) != OSAL_OK) {
        return OSAL_NOK;
    }

    payload_len = lwip_buf->len;
    payload_len = channel_round_up(payload_len, SYSCHANNEL_DATA_ALIGN);
    payload = (osal_u8 *)lwip_buf->payload;
    ret = hcc_tx_data(syschannel_handler->hcc_id, payload, (td_u16)payload_len, &param);
    if (ret != OSAL_OK) {
#ifdef _PRE_SYSCHANNEL_DEBUG
        osal_printk("syschannel_liteos_host_tx_data_adapt::hcc_tx_data failed ret: 0x%x\r\n", ret);
#endif
        return OSAL_NOK;
    }
#ifdef _PRE_SYSCHANNEL_DEBUG
    osal_printk("syschannel_liteos_host_tx_data_adapt::hcc_tx_data end\r\n");
#endif
    return OSAL_OK;
}

/*****************************************************************************
 函 数 名  : wal_lwip_send
 功能描述  : 向LWIP协议栈注册的发送回调函数
*****************************************************************************/
td_void wal_lwip_send(oal_lwip_netif *netif, oal_lwip_buf *lwip_buf)
{
    osal_u32 ret;

    if ((lwip_buf == OSAL_NULL) || (netif == OSAL_NULL)) {
        osal_printk("wal_lwip_send parameter NULL.\r\n");
        return;
    }

    if (lwip_buf->ref >= 2) { /* 引用计数>=2，不再下放，直接返回 */
        return;
    }

    /* 引用计数+1,避免pbuf被重复释放 */
    pbuf_ref(lwip_buf);
    ret = syschannel_liteos_host_tx_data_adapt(lwip_buf, SYSCHANNEL_SERVICE_TYPE_PKT, 0);
    if (ret != OSAL_OK) {
        pbuf_free(lwip_buf);
    }
}

/*****************************************************************************
 函 数 名  : wal_lwip_netif_alloc
 功能描述  : 申请netif
*****************************************************************************/
static oal_lwip_netif *wal_lwip_netif_alloc(osal_void)
{
    oal_lwip_netif *netif = (oal_lwip_netif *)malloc(sizeof(oal_lwip_netif));
    if (netif == OSAL_NULL) {
        return OSAL_NULL;
    }

    g_lwip_netif = netif;
    memset_s(netif, sizeof(oal_lwip_netif), 0, sizeof(oal_lwip_netif));
    return netif;
}

static osal_void wal_lwip_netif_free(osal_void)
{
    free(g_lwip_netif);
    g_lwip_netif = OSAL_NULL;
}

osal_s32 wal_netif_init(oal_lwip_netif **netif, oal_ip_addr_t *gw, oal_ip_addr_t *ip, oal_ip_addr_t *netmask)
{
    *netif = wal_lwip_netif_alloc();
    if (*netif == OSAL_NULL) {
        osal_printk("netif_register:: netif is NULL\r\n");
        return OSAL_NOK;
    }

    OAL_IP4_ADDR(gw, 0, 0, 0, 0);
    OAL_IP4_ADDR(ip, 0, 0, 0, 0);
    OAL_IP4_ADDR(netmask, 0, 0, 0, 0);
    (*netif)->hwaddr_len       = ETHARP_HWADDR_LEN;
    (*netif)->drv_send         = wal_lwip_send;
    (*netif)->link_layer_type  = WIFI_DRIVER_IF;
    return OSAL_OK;
}

/*****************************************************************************
 函 数 名  : netdev_register
 功能描述  : LWIP协议栈注册
*****************************************************************************/
osal_s32 netdev_register(osal_void)
{
    osal_s32 ret;
    oal_ip_addr_t gw;
    oal_ip_addr_t ip;
    oal_ip_addr_t netmask;
    oal_lwip_netif *netif = OSAL_NULL;
    osal_u8 mac_addr[ETHER_ADDR_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
    osal_char *name = "bridge_dev";
#else
    osal_char *name = "wlan";
#endif

    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_handler is not inuse\r\n");
        return OSAL_NOK;
    }

    if (g_lwip_netif != OSAL_NULL) {
        osal_printk("g_lwip_netif is not NULL\r\n");
        return OSAL_NOK;
    }

    if (wal_netif_init(&netif, &gw, &ip, &netmask) != OSAL_OK) {
        return OSAL_NOK;
    }

    /* 初始化netif名称 */
    if (strncpy_s(netif->name, IFNAMSIZ, name, IFNAMSIZ - 2) != EOK) { /* 2 size减2 */
        osal_printk("{netif_register::strncpy_s err!}\r\n");
        goto failure;
    }
    netif->name[IFNAMSIZ - 2] = '\0'; /* 2 string最后一位 */

    if (memcpy_s(netif->hwaddr, NETIF_MAX_HWADDR_LEN, &mac_addr[0], ETHER_ADDR_LEN) != EOK) {
        goto failure;
    }

    if (netifapi_netif_add(netif, &ip, &netmask, &gw) != OSAL_OK) {
        osal_printk("netifapi_netif_add failed.\r\n");
        goto failure;
    }

    osal_printk("netif_register SUCCESSFULLY\r\n");
    ret = hcc_send_message(syschannel_handler->hcc_id, H2D_MSG_SYNC_MAC_IP, 0);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_sync_mac_ip tx failed\r\n");
        return ret;
    }
    return OSAL_OK;
failure:
    wal_lwip_netif_free();
    return OSAL_NOK;
}

/*****************************************************************************
 函 数 名  : netdev_unregister
 功能描述  : LWIP协议栈去注册
*****************************************************************************/
osal_void netdev_unregister(osal_void)
{
    if (g_lwip_netif == OSAL_NULL) {
        return;
    }

    netifapi_netif_remove(g_lwip_netif);
    wal_lwip_netif_free();
}

osal_u32 syschannel_host_sync_mac_addr(osal_u8 *buf, osal_u32 len)
{
    if (g_lwip_netif == OSAL_NULL || len <= SYSCHANNEL_MSG_TYPE_LEN) {
        return OSAL_NOK;
    }

    /* 同步dev mac地址 */
    if (memcpy_s(g_lwip_netif->hwaddr, NETIF_MAX_HWADDR_LEN, (buf + 1), NETIF_MAX_HWADDR_LEN) != OSAL_OK) {
        osal_printk("syschannel_host_sync_mac_addr fail\r\n");
        return OSAL_NOK;
    }
    return OSAL_OK;
}

osal_u32 syschannel_host_sync_ip_addr(osal_u8 *buf, osal_u32 len)
{
    ip_addr_sync_stru *ip_addr_stru = {0};

    if (g_lwip_netif == OSAL_NULL || len <= SYSCHANNEL_MSG_TYPE_LEN) {
        return OSAL_NOK;
    }

    ip_addr_stru = (ip_addr_sync_stru *)(buf + SYSCHANNEL_MSG_TYPE_LEN);
    netif_set_addr(g_lwip_netif, (ip4_addr_t *)ip_addr_stru->ip_addr, (ip4_addr_t *)ip_addr_stru->mask_addr,
        (ip4_addr_t *)ip_addr_stru->gw_addr);
    (void)netifapi_netif_set_up(g_lwip_netif);
    return OSAL_OK;
}

ip_addr_t syschannel_host_get_ip_addr(osal_void)
{
    ip_addr_t ret = {0};
    if (g_lwip_netif == OSAL_NULL) {
        return ret;
    }
    return g_lwip_netif->ip_addr;
}

ip_addr_t syschannel_host_get_netmask_addr(osal_void)
{
    ip_addr_t ret = {0};
    if (g_lwip_netif == OSAL_NULL) {
        return ret;
    }
    return g_lwip_netif->netmask;
}

ip_addr_t syschannel_host_get_gw_addr(osal_void)
{
    ip_addr_t ret = {0};
    if (g_lwip_netif == OSAL_NULL) {
        return ret;
    }
    return g_lwip_netif->gw;
}

osal_u8* syschannel_host_get_mac_addr(osal_void)
{
    if (g_lwip_netif == OSAL_NULL) {
        return OSAL_NULL;
    }
    return g_lwip_netif->hwaddr;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
