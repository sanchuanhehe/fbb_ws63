/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: wal net file.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_net.h"
#include "oal_net.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_main.h"
#include "hmac_config.h"
#include "wal_event_msg.h"
#include "soc_customize_wifi.h"
#include "wal_cfg80211_apt.h"
#include "wal_scan.h"
#include "wal_ccpriv.h"
#include "wal_vlwip.h"
#include "wal_ioctl.h"
#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#if defined(_PRE_WLAN_FEATURE_WS63) || defined(_PRE_WLAN_FEATURE_WS53)
#include "lwip/prot/etharp.h"
#endif
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
#endif
#endif
#include "hmac_feature_interface.h"
#include "hmac_wapi.h"
#ifdef _PRE_WLAN_FEATURE_MESH
#include "dmac_ext_if_hcm.h"
#include "hmac_vap.h"
#include "hmac_user.h"
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
#include "wal_cfg80211.h"
#endif
#include "oal_ext_if.h"
#include "mac_addr.h"
#include "hmac_thruput_test.h"
#ifdef _PRE_WLAN_PEAK_PERFORMANCE_DFX
#include "hmac_tx_pk.h"
#endif
#ifdef _PRE_SYSCHANNEL_FEATURE
#include "syschannel_wal_api.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_NET_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
wal_delay_report_stru g_delay_report;

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 超时以及超过ap最大关联数量的eapol释放掉 防止WPA异常时大量占用pbuff资源
             超时时间为粗略统计 用于输出WPA异常的维测
 输入参数  : netdev skb链表
*****************************************************************************/
OAL_STATIC td_void hwal_handle_eapol_list(oal_net_device_stru *netdev)
{
    oal_netbuf_stru     *skb_buf = OSAL_NULL;
    oal_ext_eapol_stru *ext_eapol = &netdev->ext_eapol;
    td_u16              cur_time = (td_u16)oal_get_seconds();    /* 获取失败不需要关注 */
    td_u16              time_interval;
    const td_u16        time_5_minutes = 300;
    /* eapol最大数量取mesh和softap的最大关联用户 确保所有用户同时发起eapol OK */
    td_u8               max_eapol_cnt = (WLAN_SOFTAP_ASSOC_USER_MAX_NUM > WLAN_MESHAP_ASSOC_USER_MAX_NUM) ?
                        WLAN_SOFTAP_ASSOC_USER_MAX_NUM : WLAN_MESHAP_ASSOC_USER_MAX_NUM;
    /* 链表为空表示首个EAPOL入队 更新入队时间和数量 */
    if (oal_netbuf_list_empty(&ext_eapol->eapol_skb_head) == OSAL_TRUE) {
        ext_eapol->enqueue_time = cur_time;
        ext_eapol->eapol_cnt = 0;
        return;
    }
    /* 非空则判断首个入队时间超过5min或者数量超限 则释放第一个并更新时间 */
    if (cur_time < ext_eapol->enqueue_time) {
        time_interval = 0xffff - ext_eapol->enqueue_time + cur_time;   /* 计数翻转 */
    } else {
        time_interval = cur_time - ext_eapol->enqueue_time;
    }
    if ((time_interval > time_5_minutes) || (ext_eapol->eapol_cnt == max_eapol_cnt)) {
        /* 输出不同维测用于定位 */
        if (time_interval > time_5_minutes) {
            oam_warning_log1(0, 0, "hwal_handle_eapol_list:wpa over time:%d to handle eapol.", time_interval);
        } else {
            oam_warning_log1(0, 0, "hwal_handle_eapol_list:eapol number reach to max:%d.", max_eapol_cnt);
        }
        ext_eapol->eapol_cnt--;   /* 逻辑保证不会翻转 */
        ext_eapol->enqueue_time = cur_time;
        skb_buf = oal_netbuf_delist(&netdev->ext_eapol.eapol_skb_head);
        if (skb_buf == OSAL_NULL) {
            oam_error_log0(0, 0, "hwal_handle_eapol_list:: get netbuf is null.");
            return;
        }
        oal_netbuf_free(skb_buf);
    }
}

/*****************************************************************************
 功能描述  : liteos数据接收接口，实现数据上报lwip协议栈
 输入参数  : oal_netbuf_stru *pst_netbuf
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_u32 hwal_netif_rx(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf)
{
    td_u16 ether_type;
    oal_ether_header_stru *eth_hdr = OSAL_NULL;
    oal_ext_eapol_stru *ext_eapol = OSAL_NULL;
    osal_bool type = OSAL_FALSE;
    osal_void *fhook = hmac_get_feature_fhook(HMAC_FHOOK_WAPI_GET_ETHER_TYPE);

    if ((netbuf == OSAL_NULL) || (netdev == OSAL_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    if (netdev->lwip_netif == OSAL_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }
#endif

    /* 获取帧类型 */
    eth_hdr     = (oal_ether_header_stru *)(netbuf->data);
    ether_type   = eth_hdr->ether_type;

    if (fhook != OSAL_NULL) {
        type = ((hmac_wapi_get_ether_type_cb)fhook)(ether_type);
    }
    /* 根据不同帧类型，进行相应处理 */
    if ((ether_type == oal_host2net_short(ETHER_TYPE_IP)) || (ether_type == oal_host2net_short(ETHER_TYPE_ARP)) ||
        (ether_type == oal_host2net_short(ETHER_TYPE_RARP)) || (ether_type == oal_host2net_short(ETHER_TYPE_IPV6)) ||
        (ether_type == oal_host2net_short(ETHER_TYPE_6LO))) {
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
        hwal_lwip_receive(netdev->lwip_netif, netbuf);
#else
        hwal_lwip_receive(netbuf);
#endif
    } else if (ether_type == oal_host2net_short(ETHER_TYPE_PAE) || type) {
        ext_eapol = &netdev->ext_eapol;
        /* 入队后抛事件让WPA获取 */
        if ((ext_eapol->register_code == OSAL_TRUE) && (ext_eapol->notify_callback != OSAL_NULL)) {
            /* 入队前判断当前链表，超时2min以及超过ap最大关联数量的eapol释放掉 防止WPA异常时大量占用pbuff资源 */
            /* mesh下6个，softap 2个 */
            hwal_handle_eapol_list(netdev);
            /* 当前eapol正常入队并通知wpa处理 */
            ext_eapol->eapol_cnt++;   /* 入队+1 */
            oal_netbuf_list_tail(&ext_eapol->eapol_skb_head, netbuf);
            ext_eapol->notify_callback(netdev->name, ext_eapol->context);
        } else {
            /* 未注册则直接释放netbuff */
            oal_netbuf_free(netbuf);
            oam_error_log0(0, 0, "eapol process is not register.\r\n");
        }
    } else {
        oal_netbuf_free(netbuf);
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : hwal_lwip_skb_alloc
 功能描述  : 申请netbuf空间，取最大包长加上头空间及尾空间预留
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru *hwal_lwip_skb_alloc(const oal_net_device_stru *netdev, td_u16 lwip_buflen)
{
    oal_netbuf_stru *netbuf = OSAL_NULL;

    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_skb_alloc net_dev NULL.");
        return OSAL_NULL;
    }
    /* 申请大小为HCC允许最大长度加上头空间及尾空间 */
    netbuf     = oal_netbuf_alloc(lwip_buflen, 0, WLAN_MEM_NETBUF_ALIGN);
    return netbuf;
}
#ifdef _PRE_LWIP_ZERO_COPY
/*****************************************************************************
 函 数 名  : hwal_skb_struct_alloc
 功能描述  : 单独申请struct sk_buff
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru *hwal_skb_struct_alloc(td_void)
{
    oal_netbuf_stru *skb = oal_malloc_netbuf_stru(ZERO_COPY_LWIP_DRIECT_TX);
    if (OAL_UNLIKELY(skb == OSAL_NULL)) {
        oam_warning_log0(0, 0, "{hwal_skb_struct_alloc::oal_malloc_netbuf_stru err}");
        return OSAL_NULL;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(skb, sizeof(oal_netbuf_stru), 0, sizeof(oal_netbuf_stru));

    osal_adapt_atomic_set(&skb->users, 1);

    return skb;
}

/*****************************************************************************
 函 数 名  : hwal_skb_struct_free
 功能描述  : 单独释放struct sk_buff
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_u32 hwal_skb_struct_free(oal_netbuf_stru *sk_buf)
{
    if (sk_buf == OSAL_NULL) {
        return OAL_FAIL;
    }

    oal_free_netbuf_stru(sk_buf);

    return OAL_SUCC;
}
#endif

#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
/*****************************************************************************
 函 数 名  : hwal_pbuf_convert_2_skb
 功能描述  : 将Lwip的pbuf向skb转换
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_u32 hwal_pbuf_convert_2_skb(oal_lwip_buf *lwip_buf, oal_netbuf_stru **sk_buf)
{
    oal_netbuf_stru *sk_buf_tmp = NULL;
    osal_u8 *pkt_pos = (osal_u8 *)lwip_buf;

    /* 结构为pbuf的头子，skb的头子，reserve的80个字节，data字段
    * 1. 检查报文是否来源于pkt ram
    * 2. 检查报文头部是否有足够的空间转换
    * 3. 检查报文尾部是否有足够的空间放描述符
    */
    if (lwip_buf->pkt_buf == NULL) {
        return OAL_FAIL;
    }

    if ((pkt_pos + oal_mem_get_zerocopy_hdr_size() + PBUF_ZERO_COPY_RESERVE) > (osal_u8 *)(lwip_buf->payload)) {
        return OAL_FAIL;
    }

    if ((osal_u8 *)(lwip_buf->payload) + lwip_buf->len > oal_netbuf_dscr(lwip_buf->pkt_buf)) {
        return OAL_FAIL;
    }

    sk_buf_tmp = (oal_netbuf_stru *)(pkt_pos + oal_mem_get_buf_size());
    memset_s(sk_buf_tmp, sizeof(oal_netbuf_stru), 0, sizeof(oal_netbuf_stru));

    sk_buf_tmp->mem_head   = (td_u8 *)lwip_buf;
    sk_buf_tmp->head       = (td_u8 *)lwip_buf->payload - PBUF_ZERO_COPY_RESERVE;
    sk_buf_tmp->data       = (td_u8 *)lwip_buf->payload;
    skb_reset_tail_pointer(sk_buf_tmp);
    sk_buf_tmp->tail      += lwip_buf->len;
    sk_buf_tmp->end        = oal_netbuf_dscr(lwip_buf->pkt_buf) - sk_buf_tmp->head;
    sk_buf_tmp->len        = lwip_buf->len;
    sk_buf_tmp->pkt_buf    = lwip_buf->pkt_buf;
    sk_buf_tmp->end = oal_dmac_netbuf_end(lwip_buf->pkt_buf) - sk_buf_tmp->head;

    pbuf_ref(lwip_buf);

    *sk_buf = sk_buf_tmp;

    return OAL_SUCC;
}
#elif defined(_PRE_LWIP_ZERO_COPY)
/*****************************************************************************
 函 数 名  : hwal_pbuf_convert_2_skb
 功能描述  : 将Lwip的pbuf向skb转换
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_u32 hwal_pbuf_convert_2_skb(oal_lwip_buf *lwip_buf, oal_netbuf_stru *sk_buf)
{
    /*
    1.skb/pbuf指针转换
    2.sk/pbuf长度转换
    3.pbuf->ref自增，确保数据由HCC成功发送后再释放空间
    */
    if ((lwip_buf == OSAL_NULL) || (sk_buf == OSAL_NULL)) {
        oam_error_log2(0, 0, "[hwal_pbuf_convert_2_skb] lwip_buf[%p],sk_buf[%p]!",
            (uintptr_t)lwip_buf, (uintptr_t)sk_buf);
        return OAL_FAIL;
    }

    /*
                                 pbuf's memory distribution
    |-----PBUF-----|--------RESERVE--------|----------PAYLOAD---------|---TAILROOM---|

                          converted sk_buff's ptr according to pbuf
    mem_head       head                    data                       tail           end
    */
    sk_buf->mem_head   = (td_u8 *)lwip_buf;
    sk_buf->head       = (td_u8 *)lwip_buf->payload - PBUF_ZERO_COPY_RESERVE;
    sk_buf->data       = (td_u8 *)lwip_buf->payload;
    skb_reset_tail_pointer(sk_buf);
    sk_buf->tail      += lwip_buf->len;
    /* 内存申请时，已添加四字节对齐，此处不用担心内存越界 */
    sk_buf->end        = oal_nlmsg_align((td_u32)sk_buf->tail) + PBUF_ZERO_COPY_TAILROOM;
    sk_buf->len        = lwip_buf->len;

    pbuf_ref(lwip_buf);

    return OAL_SUCC;
}
#endif

#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
/*****************************************************************************
 函 数 名  : hwal_skb_convert_2_pbuf
 功能描述  : 将skb向Lwip的pbuf转换
*****************************************************************************/
oal_lwip_buf *hwal_skb_convert_2_pbuf(const oal_netbuf_stru *sk_buf)
{
    oal_lwip_buf       *lwip_buf  = OSAL_NULL;
    oal_dmac_netbuf_stru *pkt_buf = sk_buf->pkt_buf;

    if (pkt_buf == NULL) {
        return NULL;
    }

    /* oal_netbuf_payload(netbuf) 数据实际存储的位置,数据不能覆盖到skb的头子
       oal_dmac_netbuf_payload(pkt_buf) lwip+skb头子的位置 */
    pkt_buf->pkt_buf_offset = 0;
    if (oal_netbuf_payload(sk_buf) < oal_dmac_netbuf_payload(pkt_buf)) {
        return NULL;
    }

    pkt_buf->pkt_buf_offset = 0;
    lwip_buf = (oal_lwip_buf *)oal_netbuf_lwip(pkt_buf);
    memset_s(lwip_buf, sizeof(oal_lwip_buf), 0, sizeof(oal_lwip_buf));
    lwip_buf->pkt_buf = pkt_buf;
    lwip_buf->next = NULL;
    lwip_buf->list = NULL;
    lwip_buf->type_internal = (u8_t)PBUF_RAM;
    lwip_buf->flags = 0;
    lwip_buf->if_idx = NETIF_NO_INDEX;
    (void)atomic_set(&lwip_buf->ref, 1);
    lwip_buf->payload = sk_buf->data;
    lwip_buf->tot_len = (td_u16)sk_buf->len ; /* 32bit --->16bit */
    lwip_buf->len     = (td_u16)sk_buf->len;  /* 32bit --->16bit */

    return lwip_buf;
}
#elif defined(_PRE_LWIP_ZERO_COPY)
/*****************************************************************************
 函 数 名  : hwal_skb_convert_2_pbuf
 功能描述  : 将skb向Lwip的pbuf转换
*****************************************************************************/
oal_lwip_buf *hwal_skb_convert_2_pbuf(const oal_netbuf_stru *sk_buf)
{
    /*
    1.skb/pbuf指针转换
    2.sk/pbuf长度转换 len, tot_len
    */
    oal_lwip_buf       *lwip_buf  = OSAL_NULL;

    if ((sk_buf == OSAL_NULL) || (sk_buf->mem_head == OSAL_NULL)) {
        oam_error_log0(0, 0, "[hwal_skb_convert_2_pbuf] sk_buf or p_mem_head = NULL!");
        return OSAL_NULL;
    }

    lwip_buf          = (oal_lwip_buf *)sk_buf->mem_head;
    lwip_buf->payload = sk_buf->data;
    lwip_buf->tot_len = (td_u16)sk_buf->len ; /* 32bit --->16bit */
    lwip_buf->len     = (td_u16)sk_buf->len;  /* 32bit --->16bit */

    return lwip_buf;
}
#endif

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#ifndef _PRE_LWIP_ZERO_COPY
OAL_STATIC oal_lwip_buf *hwal_get_converted_pbuf(const oal_netbuf_stru *drv_buf)
{
    oal_lwip_buf       *lwip_buf = OSAL_NULL;
    /* 申请LWIP协议栈处理内存 32bit->16bit */
    /* 深空间 */
    lwip_buf = pbuf_alloc(PBUF_RAW, (td_u16)(OAL_NETBUF_LEN(drv_buf) + ETH_PAD_SIZE), PBUF_RAM);
    if (lwip_buf == OSAL_NULL) {
        oam_error_log1(0, 0, "hwal_lwip_receive pbuf_alloc failed! len = %d", OAL_NETBUF_LEN(drv_buf));
        return OSAL_NULL;
    }

    /* 将payload地址往后偏移2字节 */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    /* 赋值 */
    pbuf_header(lwip_buf, -ETH_PAD_SIZE);
#endif

    /* 将内存复制到LWIP协议栈处理内存 */
    if (memcpy_s(lwip_buf->payload, OAL_NETBUF_LEN(drv_buf),
        OAL_NETBUF_DATA(drv_buf), OAL_NETBUF_LEN(drv_buf)) != EOK) {
        oam_error_log0(0, 0, "{hwal_lwip_receive::mem safe function err!}");
        return OSAL_NULL;
    }

    return lwip_buf;
}
#endif

oal_lwip_buf *hwal_netbuf_2_pbuf(const oal_netbuf_stru *drv_buf)
{
    oal_lwip_buf *lwip_buf = OSAL_NULL;

#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    lwip_buf = hwal_skb_convert_2_pbuf(drv_buf);
    if (lwip_buf == OSAL_NULL) {    /* 如果零拷贝检查失败，则尝试malloc，使用非零拷贝的方式 */
        lwip_buf = hwal_get_converted_pbuf(drv_buf);
        oal_netbuf_free((oal_netbuf_stru *)drv_buf);
    }
#elif defined(_PRE_LWIP_ZERO_COPY)
    lwip_buf = hwal_skb_convert_2_pbuf(drv_buf);
    if (hwal_skb_struct_free((oal_netbuf_stru *)drv_buf) != OAL_SUCC) {
        oam_error_log0(0, 0, "[hwal_lwip_receive] skb_struct free fail");
    }
    if (lwip_buf == OSAL_NULL) {
        oam_error_log0(0, 0, "[hwal_lwip_receive] skb_convert_2_pbuf, lwip_buf is null!");
    }
#else
    lwip_buf = hwal_get_converted_pbuf(drv_buf);
    oal_netbuf_free((oal_netbuf_stru *)drv_buf);
#endif
    return lwip_buf;
}

/*****************************************************************************
 函 数 名  : hwal_lwip_receive
 功能描述  : 保存网络数据并上报lwip接口，不负责释放netbuf内存
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_void hwal_lwip_receive(oal_lwip_netif *netif, const oal_netbuf_stru *drv_buf)
{
    oal_lwip_buf       *lwip_buf = OSAL_NULL;
#ifdef _PRE_WLAN_FEATURE_LWIP_NETIF_USE_L2_METRICS
    osal_s8            rssi_dbm;
    dmac_rx_ctl_stru   *rx_cb = OSAL_NULL;
#endif

    if ((netif == OSAL_NULL) || (drv_buf == OSAL_NULL)) {
        oam_error_log0(0, 0, "hwal_lwip_receive netif is null!");
        return;
    }
#ifdef _PRE_WLAN_FEATURE_LWIP_NETIF_USE_L2_METRICS
    rx_cb = (dmac_rx_ctl_stru *)oal_netbuf_cb(drv_buf);
    rssi_dbm = rx_cb->rx_statistic.rssi_dbm;
#endif

    /* netbuf转换pbuf */
    lwip_buf = hwal_netbuf_2_pbuf(drv_buf);
    if (lwip_buf == OSAL_NULL) {
        return;
    }

#ifdef _PRE_WLAN_FEATURE_LWIP_NETIF_USE_L2_METRICS
    PBUF_SET_RSSI(lwip_buf, rssi_dbm);
    PBUF_SET_LQI(lwip_buf, 0);
#endif

#ifdef _PRE_SYSCHANNEL_FEATURE
    if (syschannel_driverif_receive((osal_void *)lwip_buf) != OAL_CONTINUE) {
        return;
    }
#endif

    /* 将payload地址前移2字节 */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    /* 赋值 */
    pbuf_header(lwip_buf, ETH_PAD_SIZE);
#endif

    /* 上报协议栈 */
    driverif_input(netif, lwip_buf);
}

#ifndef _PRE_LWIP_ZERO_COPY
WIFI_HMAC_TCM_TEXT OAL_STATIC oal_netbuf_stru *hwal_get_converted_skb(oal_lwip_buf *lwip_buf,
    const oal_net_device_stru *netdev)
{
    oal_netbuf_stru *converted_skb = OSAL_NULL;

#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    converted_skb = hwal_lwip_skb_alloc(netdev, lwip_buf->tot_len + 32 - (ETHER_HDR_LEN - SNAP_LLC_FRAME_LEN)); /* 32 */
    if (converted_skb == OSAL_NULL) {
        return OSAL_NULL;
    }
    skb_reserve(converted_skb, 32 - (ETHER_HDR_LEN - SNAP_LLC_FRAME_LEN)); /* 32 偏移量 */
#else
    converted_skb = hwal_lwip_skb_alloc(netdev, lwip_buf->tot_len);
    if (converted_skb == OSAL_NULL) {
        return OSAL_NULL;
    }
#endif

    if (lwip_buf->tot_len != lwip_buf->len) {
        oam_error_log2(0, 0, "hwal_get_converted_skb, len = %d, tot_len = %d", lwip_buf->len, lwip_buf->tot_len);
        oal_netbuf_free(converted_skb);
        return OSAL_NULL;
    }

    oal_netbuf_put(converted_skb, lwip_buf->len);
    if (memcpy_s(OAL_NETBUF_DATA(converted_skb), lwip_buf->len, lwip_buf->payload, lwip_buf->len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{hwal_get_converted_skb::memcpy_s function err!}");
        oal_netbuf_free(converted_skb);
        return OSAL_NULL;
    }

    return converted_skb;
}
#endif

WIFI_HMAC_TCM_TEXT OAL_STATIC td_void uapi_lwip_send_pk(oal_net_device_stru *netdev, oal_netbuf_stru *skb)
{
    frw_msg msg = {0};
    hmac_vap_stru *hmac_vap = OSAL_NULL;
#ifdef _PRE_WLAN_PEAK_PERFORMANCE_DFX
#ifdef _PRE_WLAN_FEATURE_PK
    if (hmac_is_thruput_enable(THRUPUT_TX_PK)) {
        hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
        msg.data = (td_u8 *)&skb;
        msg.data_len = (td_u16)sizeof(oal_netbuf_stru **);
        hmac_bridge_vap_xmit_etc_pk(hmac_vap, &msg);
        return;
    }
#endif
#endif
    /* 默认不走wifi_frw_txdata任务 */
    if (hmac_is_thruput_enable(THRUPUT_RESUME_FRW_TX_DATA) == 0) {
        hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
        msg.data = (td_u8 *)&skb;
        msg.data_len = (td_u16)sizeof(oal_netbuf_stru **);
        hmac_bridge_vap_xmit_etc(hmac_vap, &msg);
        return;
    }

    frw_host_post_data(FRW_NETBUF_W2H_DATA_FRAME, wal_util_get_vap_id(netdev), skb);
}

/*****************************************************************************
 函 数 名  : uapi_lwip_send
 功能描述  : 向LWIP协议栈注册的发送回调函数
*****************************************************************************/
WIFI_HMAC_TCM_TEXT OAL_STATIC td_void uapi_lwip_send(oal_lwip_netif *netif, oal_lwip_buf *lwip_buf)
{
    oal_netbuf_stru *converted_skb = OSAL_NULL;

    if ((lwip_buf == OSAL_NULL) || (netif == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_lwip_send parameter NULL.");
        return;
    }
    oal_net_device_stru *netdev = (oal_net_device_stru *)netif_get_netdev(netif);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "netif->state parameter NULL.");
        return;
    }
    hmac_delay(5); /* 5：发送打点时延 */

#ifdef _PRE_WIFI_DEBUG
    if (wal_tx_data_type_debug_on() == OSAL_TRUE) {
        wal_print_tx_data_type((mac_ether_header_stru *)lwip_buf->payload);
    }
#endif

#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    /* HCC发送成功之前,Lwip重传包不做再次下放,直接返回,避免对内存重复操作 */
    if (lwip_buf->ref >= 2) { /* 2 不再下方，直接返回 */
        return;
    }

    if (hwal_pbuf_convert_2_skb(lwip_buf, &converted_skb) != OAL_SUCC) { /* 零拷贝失败后尝试拷贝去发包 */
        converted_skb = hwal_get_converted_skb(lwip_buf, netdev);
        if (converted_skb == OSAL_NULL) {
            return;
        }
    }
#elif defined(_PRE_LWIP_ZERO_COPY)
    /* HCC发送成功之前,Lwip重传包不做再次下放,直接返回,避免对内存重复操作 */
    if (lwip_buf->ref >= 2) { /* 2 不再下方，直接返回 */
        return;
    }
    converted_skb = hwal_skb_struct_alloc();
    if (converted_skb == OSAL_NULL) {
        return;
    }
    if (hwal_pbuf_convert_2_skb(lwip_buf, converted_skb) != OAL_SUCC) {
        oal_netbuf_free(converted_skb);
        return;
    }
#else
    converted_skb = hwal_get_converted_skb(lwip_buf, netdev);
    if (converted_skb == OSAL_NULL) {
        return;
    }
#endif

    converted_skb->queue_mapping = 0;
    uapi_lwip_send_pk(netdev, converted_skb);
}
#else
/*****************************************************************************
 函 数 名  : hwal_lwip_receive
 功能描述  : 保存网络数据并上报lwip接口，不负责释放netbuf内存
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_void hwal_lwip_receive(const oal_netbuf_stru *drv_buf)
{
    if (drv_buf == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_receive drv_buf is null!");
        return;
    }
    lwip_input(drv_buf->data, OAL_NETBUF_LEN(drv_buf));
    oal_netbuf_free((oal_netbuf_stru *)drv_buf);
    return;
}
#endif

#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 函 数 名  : wal_mesh_inform_tx_data_info
 功能描述  : MESH 驱动上报Lwip 单播数据帧的一些信息。
*****************************************************************************/
td_u32 wal_mesh_inform_tx_data_info(frw_event_mem_stru *event_mem)
{
    frw_event_stru              *event = OSAL_NULL;
    oal_net_device_stru *netdev = OSAL_NULL;

    event  = (frw_event_stru *)event_mem->data;

    netdev = hmac_vap_get_net_device_etc(event->event_hdr.vap_id);
    if ((netdev == OSAL_NULL) || (netdev->lwip_netif == OSAL_NULL)) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY,
            "{wal_mesh_inform_tx_data_info::get net device ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return OAL_SUCC;
}

/* wal_mesh_report_assoc_info没有申明 */
td_u32 wal_mesh_report_assoc_info(const mac_user_assoc_info_stru *assoc_info, oal_net_device_stru *netdev)
{
    unref_param_prv(netdev);
    if ((netdev == OSAL_NULL) || (netdev->lwip_netif == OSAL_NULL)) {
        return OAL_FAIL;
    }

    if (assoc_info->is_mesh_user == OSAL_TRUE) {
        oam_warning_log3(0, OAM_SF_ANY,
            "{wal_mesh_report_mesh_user_info:report add mesh peer to lwip,bcn_prio=%d,role=%d,rssi=%d}",
            assoc_info->bcn_prio, assoc_info->is_initiative_role, (td_s32)(-(assoc_info->conn_rx_rssi)));
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::report add sta to lwip}");
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : MESH 驱动上报某用户关联状态改变。
 输入参数  : frw_event_mem_stru *event_mem
*****************************************************************************/
td_u32 wal_mesh_report_mesh_user_info(frw_event_mem_stru *event_mem)
{
    frw_event_stru            *event      = (frw_event_stru *)event_mem->data;
    mac_user_assoc_info_stru *assoc_info = (mac_user_assoc_info_stru *)event->event_data;
    oal_net_device_stru       *netdev     = hmac_vap_get_net_device_etc(event->event_hdr.vap_id);
    if ((netdev == OSAL_NULL) || (netdev->lwip_netif == OSAL_NULL)) {
        oam_error_log0(event->event_hdr.vap_id, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::netdev null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

#define MAC_USER_STATE_DEL 0
    if (assoc_info->assoc_state == MAC_USER_STATE_DEL) {
        if (assoc_info->is_mesh_user == OSAL_TRUE) {
            oam_warning_log2(0, OAM_SF_ANY,
                "{wal_mesh_report_mesh_user_info:report del mesh peer to lwip,mac addr[%x:%x]}",
                assoc_info->user_addr[4], assoc_info->user_addr[5]); /* 4 5 地址位数 */
        } else {
            oam_warning_log2(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::report del sta to lwip,mac addr[%x:%x]}",
                assoc_info->user_addr[4], assoc_info->user_addr[5]); /* 4 5 地址位数 */
        }

        return OAL_SUCC;
    } else if (assoc_info->assoc_state == MAC_USER_STATE_ASSOC) {
        td_u32 ret = wal_mesh_report_assoc_info(assoc_info, netdev);
        return ret;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mesh_report_mesh_user_info::rx unsupport state}");

        return OAL_FAIL;
    }
}
#endif

/*****************************************************************************
 功能描述  : 驱动上报去关联状态改变到LWIP。
*****************************************************************************/
OAL_STATIC td_void wal_report_sta_disassoc_to_lwip(const oal_net_device_stru *netdev)
{
    unref_param(netdev);
#if defined(LWIP_API_MESH) && LWIP_API_MESH
    /* 内部调用函数，参数合法性由调用者保证 */
    if (netdev->lwip_netif->linklayer_event != OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_disassoc_to_lwip::LL_EVENT_AP_DISCONN}\r\n");
        netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_AP_DISCONN, OSAL_NULL);
    }
#endif
#if defined(LWIP_LINK_STATUS_CALLBACK)&&LWIP_LINK_STATUS_CALLBACK
    (td_void)tcpip_callback_with_block(netif_set_link_down_interface, (void *)(netdev->lwip_netif), 1);
    oam_warning_log0(0, OAM_SF_ANY, "{wal_report_sta_disassoc_to_lwip::report sta disconn succ to lwip!}\r\n");
#endif
}

/*****************************************************************************
 功能描述  : 驱动延时上报去关联状态到LWIP的超时处理函数
*****************************************************************************/
OAL_STATIC td_u32 wal_delay_report_timeout_fn(td_void *arg)
{
    oal_net_device_stru *netdev = (oal_net_device_stru *)arg;
    /* 上报LWIP时先删除定时器 内部判断定时器是否已注册 */
    g_delay_report.enable = OSAL_FALSE;   /* 上报一次后再未重新关联成功前此后的每次去关联均需要上报 */
    frw_destroy_timer_entry(&(g_delay_report.delay_timer));
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_delay_report_timeout_fn::null param.");
        return OAL_FAIL;
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    if (netdev->lwip_netif == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_delay_report_timeout_fn::lwip_netif null param.");
        return OAL_FAIL;
    }
#endif
    /* 上报LWIP */
    wal_report_sta_disassoc_to_lwip(netdev);
    /* 上报wpa,以便WPA上报用户 */
    return cfg80211_timeout_disconnected(netdev);
}

OAL_STATIC osal_s32 wal_report_lwip_assoc_info(hmac_sta_report_assoc_info_stru *sta_asoc, oal_net_device_stru *netdev)
{
    unref_param(netdev);
#if defined(LWIP_API_MESH) && LWIP_API_MESH
    oal_event_ap_conn_stru conn_info;

    if (memcpy_s(conn_info.addr.addr, WLAN_MAC_ADDR_LEN, sta_asoc->auc_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_report_lwip_assoc_info::mem safe function err!}");
        return OAL_FAIL;
    }
    conn_info.addr.addrlen = WLAN_MAC_ADDR_LEN;
    conn_info.rssi = (osal_s8)(-(sta_asoc->rssi));
    conn_info.is_mesh_ap = sta_asoc->conn_to_mesh;

    if (netdev->lwip_netif->linklayer_event != OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_report_lwip_assoc_info::LL_EVENT_AP_CONN}\r\n");
        netdev->lwip_netif->linklayer_event(netdev->lwip_netif, LL_EVENT_AP_CONN, (td_u8 *)&conn_info);
    }
    oam_warning_log_alter(0, OAM_SF_ANY, "{report_lwip_assoc_info::rssi=%x,is_mesh_ap=%d,mac addr=%x:%x:%x:%x:XX:XX}",
        6, conn_info.rssi, sta_asoc->conn_to_mesh, sta_asoc->auc_mac_addr[0], sta_asoc->auc_mac_addr[1], /* 6个参数 */
        sta_asoc->auc_mac_addr[2], sta_asoc->auc_mac_addr[3]); /* mac地址 2 3 */
#else
    unref_param(sta_asoc);
#endif
#if defined(LWIP_LINK_STATUS_CALLBACK)&&LWIP_LINK_STATUS_CALLBACK
    (td_void)tcpip_callback_with_block(netif_set_link_up_interface, (void *)(netdev->lwip_netif), 1);
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 驱动上报sta关联/去关联AP
 输入参数  : frw_event_mem_stru *event_mem
 返 回 值  :td_u32
 修改历史      :
  1.日    期   : 2019年7月9日
    修改内容   : 新生成函数
*****************************************************************************/
osal_s32 wal_report_sta_assoc_info(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    hmac_sta_report_assoc_info_stru *sta_asoc = (hmac_sta_report_assoc_info_stru *)msg->data;
    oal_net_device_stru             *netdev   = hmac_vap_get_net_device_etc(hmac_vap->vap_id);

    if ((netdev == OSAL_NULL)) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ANY, "{wal_report_sta_assoc_info::net device null}");
        return OAL_ERR_CODE_PTR_NULL;
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    if ((netdev->lwip_netif == OSAL_NULL)) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ANY, "{wal_report_sta_assoc_info::netif null}");
        return OAL_ERR_CODE_PTR_NULL;
    }
#endif
    /* WPA自动重连标志仅限当次关联,关联状态变更则清除,先保存后清除 */
    td_u8 reconn_status = g_delay_report.reconn;
    g_delay_report.reconn = OSAL_FALSE;
    if (sta_asoc->is_assoc == OSAL_TRUE) {
        /* WPA自动重连,不上报关联事件到LWIP */
        if ((reconn_status == OSAL_TRUE) && (g_delay_report.enable == OSAL_TRUE)) {
            return OAL_SUCC;
        }
        if (wal_report_lwip_assoc_info(sta_asoc, netdev) != OAL_SUCC) {
            return OAL_FAIL;
        }
    } else {
        /* 是否启用WPA重联,启用后启动定时器起延时上报LWIP */
        if (g_delay_report.enable == OSAL_FALSE) {
            wal_report_sta_disassoc_to_lwip(netdev);    /* 未启动延时上报则直接上报LWIP */
            return OAL_SUCC;
        }
        /* 延时上报且定时器未生效则启动定时器 */
        if (g_delay_report.delay_timer.is_enabled == OSAL_FALSE) {
            frw_create_timer_entry(&(g_delay_report.delay_timer), wal_delay_report_timeout_fn,
                g_delay_report.delay_timer.timeout, (td_void *)netdev, OSAL_FALSE);
        }
    }
    return OAL_SUCC;
}

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
/*****************************************************************************
 函 数 名  : uapi_lwip_set_hwaddr
 功能描述  : 向LWIP协议栈注册的修改mac地址回调函数
 输入参数  : [1]netif
             [2]addr
             [3]len
 输出参数  : 无
 返 回 值  : td_u32
*****************************************************************************/
OAL_STATIC td_u8 uapi_lwip_set_hwaddr(struct netif *netif, td_u8 *addr, td_u8 len)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    if (netif == NULL) {
        oam_error_log0(0, 0, "netif is NULL!");
        return (td_u8)OAL_FAIL;
    }
    if (addr == NULL) {
        oam_error_log0(0, 0, "addr is NULL!");
        return (td_u8)OAL_FAIL;
    }

    if ((addr[0] & 0x01) != 0) {
        oam_error_log0(0, 0, "config a muticast mac address, please check!");
        return (td_u8)OAL_FAIL;
    }
    if (len != ETHER_ADDR_LEN) {
        oam_error_log1(0, 0, "config wrong mac address len=%u.", len);
        return (td_u8)OAL_FAIL;
    }

    netdev = (oal_net_device_stru *)netif_get_netdev(netif);
    if (netdev == NULL) {
        oam_error_log0(0, 0, "netdev is NULL!");
        return (td_u8)OAL_FAIL;
    }

    /* 如果netdev是running状态，则直接返回失败 */
    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) != 0) {
        oam_error_log0(0, 0, "netdev is running!");
        return (td_u8)OAL_FAIL;
    }
#ifdef _PRE_WLAN_DFT_STAT
    wal_record_wifi_external_log((osal_u32)__builtin_return_address(0));
#endif

    /* 更新netdev中的mac地址 */
    if (memcpy_s(netdev->dev_addr, ETHER_ADDR_LEN, addr, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_lwip_set_hwaddr::mem safe function err!}");
        return (td_u8)OAL_FAIL;
    }
    /* 设置netdevice的MAC地址，MAC地址在HMAC层被初始化到MIB中 */
    if (wal_set_mac_to_mib(netdev) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_lwip_set_hwaddr::wal_set_mac_to_mib fail!}\r\n");
        return (td_u8)OAL_FAIL;
    }

    return (td_u8)OAL_SUCC;
}
#if defined(LWIP_L2_NETDEV_STATUS_CALLBACK) && (LWIP_L2_NETDEV_STATUS_CALLBACK)
/*****************************************************************************
 功能描述  : 向LWIP协议栈注册的连接变化回调函数
 输入参数  : struct netif *netif
*****************************************************************************/
OAL_STATIC td_void wal_lwip_status_callback(struct netif *netif)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    if ((netif == OSAL_NULL) || (netif_get_netdev(netif) == OSAL_NULL)) {
        return;
    }
    netdev = (oal_net_device_stru *)netif_get_netdev(netif);
    if ((netif->flags & NETIF_FLAG_UP) != 0) {
        OAL_NETDEVICE_FLAGS(netdev) |= OAL_IFF_RUNNING;   /* 将net device的flag设为up */
        wal_netdev_open(netdev);
    } else {
        wal_netdev_stop(netdev);
        OAL_NETDEVICE_FLAGS(netdev) &= ~OAL_IFF_RUNNING;   /* 将net device的flag设为down */
    }
}
#endif


OAL_STATIC oal_lwip_netif *hwal_lwip_netif_alloc(osal_bool *is_new_alloc)
{
    osal_bool new_alloc;
#ifndef _PRE_WLAN_LWIP_NETIF_CUSTOM_ALLOC
    oal_lwip_netif *netif = malloc(sizeof(oal_lwip_netif));
    new_alloc = OSAL_TRUE;
#else
    oal_lwip_netif *netif = netif_get_by_index(1); // single netif idx = num + 1
    new_alloc = OSAL_FALSE;
#endif
    if (netif == OSAL_NULL) {
        return OSAL_NULL;
    }
    if (new_alloc == OSAL_TRUE) {
        /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
        memset_s(netif, sizeof(oal_lwip_netif), 0, sizeof(oal_lwip_netif));
    }

    *is_new_alloc = new_alloc;
    return netif;
}

OAL_STATIC osal_void hwal_lwip_netif_free(oal_lwip_netif *netif, osal_bool is_new_alloc)
{
    if (is_new_alloc == OSAL_TRUE) {
        free(netif);
    }
}
/*****************************************************************************
 函 数 名  : hwal_lwip_register_netdev
 功能描述  : LWIP协议栈初始化
*****************************************************************************/
td_u32 hwal_lwip_register(oal_net_device_stru *netdev)
{
    td_s32 size;
    osal_bool is_new_alloc = OSAL_TRUE;
    oal_lwip_netif *netif = OSAL_NULL;
    oal_ip_addr_t gw, ip, netmask;

    netif = hwal_lwip_netif_alloc(&is_new_alloc);
    if (netif == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_register_netdev failed mem alloc NULL.");
        /* 返回 */
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    OAL_IP4_ADDR(&gw, 0, 0, 0, 0);
    OAL_IP4_ADDR(&ip, 0, 0, 0, 0);
    OAL_IP4_ADDR(&netmask, 0, 0, 0, 0);

    /* 网络设备注册 */
    netif_set_netdev(netif, netdev);
    netif->drv_set_hwaddr   = uapi_lwip_set_hwaddr;
    netif->hwaddr_len       = ETHARP_HWADDR_LEN;
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    netif->drv_send         = uapi_lwip_send;
    netif->link_layer_type  = WIFI_DRIVER_IF;
#endif

    /* 用dev名称初始化netif名称 */
    if (strncpy_s(netif->name, IFNAMSIZ, netdev->name, IFNAMSIZ - 2) != EOK) { /* 2 size减2 */
        oam_error_log0(0, OAM_SF_ANY, "{hwal_lwip_register_netdev::strncpy_s err!}");
        goto failure;
    }
    netif->name[IFNAMSIZ - 2] = '\0'; /* 2 string最后一位 */

#ifndef _PRE_WLAN_LWIP_NETIF_CUSTOM_ALLOC
    if (netifapi_netif_add(netif, &ip, &netmask, &gw) != OAL_SUCC) {
        oam_error_log0(0, 0, "hwal_lwip_register_netdev failed netif_add NULL.");
        goto failure;
    }
#endif

    /* 根据添加后接口名(含序号)更新dev名称 */
    size = snprintf_s(netdev->name, IFNAMSIZ, (IFNAMSIZ - 1), "%s%"U16_F, netif->name, netif->num);
    if ((size != -1) && (size < OAL_IF_NAME_SIZE)) {
        netdev->name[size] = '\0';
    } else {
        goto failure;
    }

#if defined(LWIP_L2_NETDEV_STATUS_CALLBACK) && (LWIP_L2_NETDEV_STATUS_CALLBACK)
    netif->l2_netdev_status_callback = wal_lwip_status_callback;
#endif

    if (memcpy_s(netif->hwaddr, NETIF_MAX_HWADDR_LEN, netdev->dev_addr, ETHER_ADDR_LEN) != EOK) {
        goto failure;
    }
    netdev->lwip_netif = netif;
    return OAL_SUCC;
failure:
    hwal_lwip_netif_free(netif, is_new_alloc);
    return OAL_FAIL;
}
#else
td_u32 hwal_lwip_register(oal_net_device_stru *netdev)
{
    td_s32 size;

    /* 网络设备注册 */
    netif_set_netdev(NULL, netdev);

    /* 根据添加后接口名(含序号)更新dev名称 */
    size = snprintf_s(netdev->name, OAL_IF_NAME_SIZE, (OAL_IF_NAME_SIZE - 1), "%s%u", netdev->name, 0);
    if ((size != -1) && (size < OAL_IF_NAME_SIZE)) {
        netdev->name[size] = '\0';
    } else {
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : hwal_lwip_deinit
 功能描述  : LWIP协议栈去初始化
*****************************************************************************/
td_void hwal_lwip_unregister_netdev(oal_net_device_stru *netdev)
{
    if (netdev == OSAL_NULL) {
        return;
    }

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    osal_u8 netif_state;
    if (netdev->lwip_netif != OSAL_NULL) {
        netif_state = netif_is_up(netdev->lwip_netif);
        if (netif_state != 0) {
            wal_netdev_stop(netdev);
        }
#ifndef _PRE_WLAN_LWIP_NETIF_CUSTOM_ALLOC
        netifapi_netif_remove(netdev->lwip_netif);
        free(netdev->lwip_netif);
#endif
    }

    netdev->lwip_netif = OSAL_NULL;
#else
    /* 网络设备去注册 */
    netif_set_netdev(OSAL_NULL, OSAL_NULL);
    wal_netdev_stop(netdev);
#endif
    return;
}

/*****************************************************************************
 功能描述  : 设置延时上报关联状态的相关参数
*****************************************************************************/
td_void wal_set_delay_report_config(td_u8 enable, td_u16 timeout)
{
    g_delay_report.enable = enable & BIT0;
    g_delay_report.delay_timer.timeout = timeout * 1000;    /* 1000:上层传入s,转化为ms */
    if (g_delay_report.enable == OSAL_FALSE) {
        /* 如果定时器执行中则立即上报 */
        if (g_delay_report.delay_timer.is_enabled == OSAL_TRUE) {
            frw_timer_stop_timer(&(g_delay_report.delay_timer));
            wal_delay_report_timeout_fn(g_delay_report.delay_timer.timeout_arg);
        }
    }
    /* WPA刷新状态时删除定时器 */
    frw_destroy_timer_entry(&(g_delay_report.delay_timer));
}

/*****************************************************************************
 功能描述  : 设置是否为WPA自动重连标志
*****************************************************************************/
OAL_STATIC td_void wal_set_auto_conn_status(td_u8 auto_reconn)
{
    g_delay_report.reconn = auto_reconn;
}

/*****************************************************************************
 功能描述  : 判断自动重连时是否连接同一个BSSID
*****************************************************************************/
td_u32 wal_check_auto_conn_status(oal_net_device_stru *netdev, td_u8 auto_reconn, td_u8 *bssid)
{
    static td_u8 last_conn_bssid[WLAN_MAC_ADDR_LEN]; /* 存放上次关联的BSSID */

    /* 非自动重连 直接返回保持原有逻辑 */
    if (auto_reconn == OSAL_FALSE) {
        wal_set_auto_conn_status(auto_reconn);
    } else {
        /* 为自动重连 bssid不为空与上一次关联bssid不同禁用自动重连并停止关联 */
        if ((bssid != OSAL_NULL) && (osal_memcmp(last_conn_bssid, bssid, WLAN_MAC_ADDR_LEN) != 0)) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_check_auto_conn_status::reconn a new bssid.}");
            /* reason code置为BEACON LOST的255 表示与当前ap失联 */
            cfg80211_disconnected(netdev, 0xFF, OSAL_NULL, 0);
            wal_set_delay_report_config(0, 0);
            return OAL_FAIL;
        }
    }
    if (bssid != OSAL_NULL) {
        (td_void)memcpy_s(last_conn_bssid, WLAN_MAC_ADDR_LEN, bssid, WLAN_MAC_ADDR_LEN);
    }
    return OAL_SUCC;
}

#endif

OAL_STATIC td_u32 wal_netdev_open_send_event(oal_net_device_stru *netdev)
{
    td_u32              ret;
    mac_cfg_start_vap_param_stru vap_info;
    (osal_void)memset_s(&vap_info, OAL_SIZEOF(vap_info), 0, OAL_SIZEOF(vap_info));

    vap_info.net_dev = netdev;

#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(netdev->ieee80211_ptr->iftype);
    if (p2p_mode == WLAN_P2P_BUTT) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open_send_event:wal_wireless_iftype_to_mac_p2p_mode_etc BUFF}");
        return OAL_FAIL;
    }
    vap_info.p2p_mode = p2p_mode;
#endif

    vap_info.mgmt_rate_init_flag = OSAL_TRUE;

    /* 发送消息 */
    ret = (td_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_START_VAP,
        (osal_u8 *)&vap_info, OAL_SIZEOF(vap_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open_send_event::START_VAP return err code %u!}\r\n", ret);
        wifi_printf("{wal_netdev_open_send_event::START_VAP return err code %u!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_netdev_open
 功能描述  : 启用VAP
*****************************************************************************/
td_u32  wal_netdev_open(oal_net_device_stru *netdev)
{
    td_u32              ret;

    if ((OAL_UNLIKELY(netdev == OSAL_NULL)) || (OAL_UNLIKELY(netdev->ieee80211_ptr == OSAL_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_open::net_dev is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_open::iftype:%d.!}\r\n", netdev->ieee80211_ptr->iftype);

    if ((netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        || (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_MESH_POINT)
#endif
    ) {
        mac_cfg_ssid_param_stru fill_msg;
        frw_msg cfg_info;
        memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
        cfg_info.rsp_buf_len = sizeof(fill_msg);
        cfg_info.rsp = (osal_u8 *)&fill_msg;
        ret = (td_u32)send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_SSID, &cfg_info);
        if ((ret != OAL_SUCC) || (cfg_info.rsp == OSAL_NULL)) {
            oam_warning_log1(0, OAM_SF_ANY, "{_wal_netdev_open::send_sync_cfg_to_host return err code %d!}\r\n", ret);
            return ret;
        }

        /* 处理返回消息 */
        td_u8                    ssid_len      = fill_msg.ssid_len;

        if (ssid_len == 0) {
            oam_warning_log0(0, OAM_SF_ANY, "{_wal_netdev_open::ssid not set,no need to start vap.!}\r\n");
            return OAL_SUCC;
        }
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    ret = wal_netdev_open_send_event(netdev);
    if (ret != OAL_SUCC) {
        return ret;
    }

    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) == 0) {
        OAL_NETDEVICE_FLAGS(netdev) |= OAL_IFF_RUNNING;
    }
    return OAL_SUCC;
}

OAL_STATIC td_u32 wal_netdev_stop_del_vap(const oal_net_device_stru *netdev)
{
    /* wlan0/p2p0 down时 删除VAP */
    if (OAL_UNLIKELY(netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop_del_vap::netdev->ieee80211_ptr is NULL!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop_del_vap::enter}");
    if (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_AP || netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION ||
        netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_DEVICE
#ifdef _PRE_WLAN_FEATURE_MESH
        || netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_MESH_POINT
#endif
        ) {
#ifdef _PRE_WLAN_FEATURE_P2P
        /* 用于删除p2p小组 */
        hmac_vap_stru *hmac_vap = (hmac_vap_stru *)(netdev->ml_priv);
        if (hmac_vap == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_netdev_stop_del_vap::hmac_vap is null, netdev released.}");
            return OAL_SUCC;
        }
        if (is_legacy_vap(hmac_vap)) {
            return OAL_SUCC;
        }

        hmac_device_stru *mac_dev = hmac_res_get_mac_dev_etc(0);
        if (mac_dev != OSAL_NULL) {
            wal_del_p2p_group(mac_dev);
        }

#endif
        return OAL_SUCC;
    }
    oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop_del_vap::netdev->ieee80211_ptr->iftype unknow!}");
    return OAL_CONTINUE;
}

/*****************************************************************************
 函 数 名  : wal_netdev_stop
 功能描述  : 停用vap
*****************************************************************************/
td_u32  wal_netdev_stop(oal_net_device_stru *netdev)
{
    td_u32 stop_ret;
    td_s32 util_ret;

    if (OAL_UNLIKELY((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == NULL))) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop::net_dev/ieee80211_ptr is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* stop vap时先删除延时上报定时器 */
    if (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) {
        frw_destroy_timer_entry(&(g_delay_report.delay_timer));
        g_delay_report.enable = OSAL_FALSE;
    }
    oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::iftype:%d.}\r\n", netdev->ieee80211_ptr->iftype);
    /* 如果netdev不是running状态，则直接返回成功 */
    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_netdev_stop::vap is already down!}\r\n");
        return OAL_SUCC;
    }
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    wal_force_scan_complete(netdev);
#endif

    /* 发送消息 */
    util_ret = wal_down_vap(netdev, wal_util_get_vap_id(netdev));
    if (OAL_UNLIKELY(util_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::wal_down_vap return err code %d!}", util_ret);
        wifi_printf("{wal_netdev_stop::wal_down_vap return err code %d!}\r\n", util_ret);
        return (td_u32)util_ret;
    }

    stop_ret = wal_netdev_stop_del_vap(netdev);
    if (OAL_UNLIKELY(stop_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_netdev_stop::wal_netdev_stop_del_vap return err code %u!}", stop_ret);
    }
    return OAL_SUCC;
}

OAL_STATIC td_s32 wal_init_netdev(nl80211_iftype_uint8 type, oal_net_device_stru *netdev, oal_wireless_dev *wdev)
{
    td_u8 ac_addr[ETHER_ADDR_LEN] = {0};

    /* 对netdevice进行赋值 */
    if (get_dev_addr(ac_addr, ETHER_ADDR_LEN, type) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::get_dev_addr failed!}\r\n");
        oal_net_free_netdev(netdev);
        return OAL_FAIL;
    }

    if (memcpy_s(netdev->dev_addr, ETHER_ADDR_LEN, ac_addr, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::mem safe function err!}");
        oal_net_free_netdev(netdev);
        return OAL_FAIL;
    }
    OAL_NETDEVICE_WDEV(netdev)            = wdev;
    wdev->netdev = netdev;
    wdev->iftype = type;

    OAL_NETDEVICE_FLAGS(netdev) &= ~OAL_IFF_RUNNING;   /* 将net device的flag设为down */

    return OAL_SUCC;
}

OAL_STATIC td_s32 wal_init_netif(nl80211_iftype_uint8 type, oal_net_device_stru *netdev)
{
    unref_param_prv(type);
    /* 注册net_device */
    td_u32 ret = oal_net_register_netdev(netdev);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_init_wlan_netdev::oal_net_register_netdev return error code %d}", ret);
        oal_net_free_netdev(netdev);
        return OAL_FAIL;
    }

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    struct netif *netif = netdev->lwip_netif;
    if (netif == OSAL_NULL) {
        oal_net_unregister_netdev(netdev);
        oal_net_free_netdev(netdev);
        oam_error_log0(0, OAM_SF_ANY, "wal_init_wlan_netdev:netif is null");
        return OAL_ERR_CODE_PTR_NULL;
    }

#if defined(LWIP_IPV6) && (LWIP_IPV6)
    /* 设置IPv6 linklocal address(SLAAC) */
    if ((type == NL80211_IFTYPE_AP) || (type == NL80211_IFTYPE_MESH_POINT)) {
        (td_void)netifapi_set_ip6_autoconfig_disabled(netdev->lwip_netif);
#ifdef _PRE_WLAN_FEATURE_LWIP_ND6_ROUTER
        (td_void)netifapi_set_accept_ra(netif, OSAL_FALSE);
        (td_void)netifapi_set_ipv6_forwarding(netif, OSAL_TRUE);
        (td_void)netifapi_set_ra_enable(netif, OSAL_TRUE);
#endif
        (td_void)netifapi_netif_add_ip6_linklocal_address(netif, OSAL_TRUE);
    }
#endif // LWIP_IPV6

    netifapi_netif_set_default(netif);
#endif
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
    if (wal_init_wlan_vap(netdev) != OAL_SUCC) {
        oal_net_unregister_netdev(netdev);
        oal_net_free_netdev(netdev);
        return OAL_FAIL;
    }

#if defined(_PRE_WLAN_FEATURE_VLWIP) && !defined(_PRE_WLAN_FEATURE_MFG_ONLY)
    if (wal_vlwip_get_netif_type() != netdev->ieee80211_ptr->iftype) {
        return OAL_SUCC;
    }
    if (wal_vlwip_register_netdev(netdev) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "wal_init_wlan_netdev:vlwip netif register failed");
    }
#endif
    return OAL_SUCC;
}

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
/* 功能描述:根据驱动和lwip解耦方案，down和up掉驱动的netdev接口的时候同时通知netif去置位netif标志 */
td_void wal_netif_set_down(struct netif *netif, oal_net_device_stru *netdev)
{
    netifapi_netif_set_down(netif);
    if (wal_netdev_stop(netdev) != OAL_SUCC) {
        oam_error_log0(0, 0, "wal_netif_set_down:stop netdev failed");
    }
}

td_void wal_netif_set_up(struct netif *netif, oal_net_device_stru *netdev)
{
    netifapi_netif_set_up(netif);
    if (wal_netdev_open(netdev) != OAL_SUCC) {
        oam_error_log0(0, 0, "wal_netif_set_up:open netdev failed");
    }
}
#else
td_void wal_netif_set_down(oal_net_device_stru *netdev)
{
    if (wal_netdev_stop(netdev) != OAL_SUCC) {
        oam_error_log0(0, 0, "wal_netif_set_down:stop netdev failed");
    }
}

td_void wal_netif_set_up(oal_net_device_stru *netdev)
{
    if (wal_netdev_open(netdev) != OAL_SUCC) {
        oam_error_log0(0, 0, "wal_netif_set_up:open netdev failed");
    }
}
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
#endif

static osal_u32 wal_init_mode(oal_net_device_stru *netdev, wal_protocol_mode_enum mode)
{
    osal_s8 mode_str[40] = {0};     /* 40 预留协议模式字符串空间 */
    /* 安全编程规则6.6例外（5）源内存全部是静态字符串常量（目标内存已有足够的存储空间） */
    strcpy_s((osal_char *)mode_str, sizeof(mode_str), "11bgnax");
    if (mode == WIFI_MODE_11B_G_N) {
        /* 安全编程规则6.6例外（5）源内存全部是静态字符串常量（目标内存已有足够的存储空间） */
        strcpy_s((osal_char *)mode_str, sizeof(mode_str), "11bgn");
    } else if (mode == WIFI_MODE_11B_G) {
        /* 安全编程规则6.6例外（5）源内存全部是静态字符串常量（目标内存已有足够的存储空间） */
        strcpy_s((osal_char *)mode_str, sizeof(mode_str), "11bg");
    } else if (mode == WIFI_MODE_11B) {
        /* 安全编程规则6.6例外（5）源内存全部是静态字符串常量（目标内存已有足够的存储空间） */
        strcpy_s((osal_char *)mode_str, sizeof(mode_str), "11b");
    }

    if (uapi_ccpriv_set_mode(netdev, (osal_s8 *)mode_str) != OAL_SUCC) {
        wal_deinit_wlan_vap(netdev);
        oal_net_unregister_netdev(netdev);
        oal_net_free_netdev(netdev);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}
/*****************************************************************************
 功能描述  : 初始化wlan设备
 输入参数  : [1]type 设备类型
             [2]mode 模式
 输出参数  : [1]ifname 设备名
             [2]len 设备名长度
 返 回 值  : 错误码
*****************************************************************************/
/* 建议5.5误检，在2024行作为strncpy_s函数的第一个参数传入 */
td_s32 wal_init_drv_wlan_netdev(nl80211_iftype_uint8 type, wal_protocol_mode_enum mode, td_char *ifname, td_u32 *len)
{
    oal_net_device_stru *netdev          = OSAL_NULL;
    td_u32               dev_name_len    = OAL_IF_NAME_SIZE;
    td_char              dev_name[OAL_IF_NAME_SIZE];

    /* 校验是否可以起netdev,可以的话分配netdev名称 */
    if (oal_net_check_and_get_devname(type, dev_name, &dev_name_len) != OAL_SUCC) {
        return OAL_FAIL;
    }

    netdev = oal_net_alloc_netdev(dev_name, OAL_IF_NAME_SIZE);
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::oal_net_alloc_netdev return null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_wireless_dev *wdev = (oal_wireless_dev *)malloc(sizeof(oal_wireless_dev));
    if (OAL_UNLIKELY(wdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::alloc mem, wdev is null ptr!}");
        oal_net_free_netdev(netdev);
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(wdev, sizeof(oal_wireless_dev), 0, sizeof(oal_wireless_dev));

    td_s32 ret = wal_init_netdev(type, netdev, wdev);
    if (ret != OAL_SUCC) {
        free(wdev);
        return ret;
    }

    ret = wal_init_netif(type, netdev);
    if (ret != OAL_SUCC) {
        free(wdev);
        return ret;
    }

    /* 设置netdevice的MAC地址，MAC地址在HMAC层被初始化到MIB中 */
    if (wal_set_mac_to_mib(netdev) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_init_drv_wlan_netdev::wal_set_mac_to_mib fail!}\r\n");
        wal_deinit_wlan_vap(netdev);
        oal_net_unregister_netdev(netdev);
        oal_net_free_netdev(netdev);
        free(wdev);
        return OAL_FAIL;
    }
    if (wal_init_mode(netdev, mode) != OAL_SUCC) {
        free(wdev);
        return OAL_FAIL;
    }

    if ((*len) >= (td_u32)(osal_strlen(netdev->name) + 1) &&
        strncpy_s(ifname, *len, netdev->name, osal_strlen(netdev->name) + 1) != EOK) {  /* 包含结束符 */
        free(wdev);
        return OAL_FAIL;
    }
    *len = osal_strlen(netdev->name);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 去初始化wlan设备
 输入参数  : *ifname 设备名
 返 回 值：错误码
*****************************************************************************/
td_s32 wal_deinit_drv_wlan_netdev(const td_char *ifname)
{
    oal_net_device_stru        *netdev = OSAL_NULL;
    td_u32                      ret;

    /* 根据dev_name找到dev */
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_netdev::the net_device is not exist!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

#if defined(_PRE_WLAN_FEATURE_VLWIP) && !defined(_PRE_WLAN_FEATURE_MFG_ONLY)
    wal_vlwip_unregister_netdev(netdev);
#endif

    ret = wal_deinit_wlan_vap(netdev);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_netdev::deinit vap failed!}\r\n");
        return OAL_FAIL;
    }
    /* 驱动删除vap后在解注册lwip,防止wifi驱动删除失败后netdev已释放,驱动再次调用netif后死机 */
    oal_net_unregister_netdev(netdev);

    oal_net_free_netdev(netdev);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 判断netdev是否处于busy
*****************************************************************************/
td_u8 wal_dev_is_running(td_void)
{
    td_u8 loop;
    oal_net_device_stru *netdev = OSAL_NULL;

    for (loop = 0; loop < WLAN_VAP_NUM_PER_BOARD; loop++) {
        netdev = oal_get_past_net_device_by_index(loop);
        if (netdev == OSAL_NULL) {
            continue;
        }

        if (OAL_UNLIKELY((OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(netdev)) != 0)) {
            return OAL_TRUE;
        }
    }
    return OAL_FALSE;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
