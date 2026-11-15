/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: netbuf function.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_netbuf.h"
#include <mac_frame.h>
#include <dmac_ext_if_hcm.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_OAL_NETBUF_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量
*****************************************************************************/
#ifdef _PRE_LWIP_ZERO_COPY
#ifdef _PRE_LWIP_ZERO_COPY_MALLOC_SKB
/*****************************************************************************
 功能描述  : 分配g_netbuf_list结构
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru* oal_malloc_netbuf_stru(td_u8 direct)
{
    oal_netbuf_stru *netbuf = OSAL_NULL;
    unref_param(direct);

    netbuf = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sizeof(oal_netbuf_stru), OAL_TRUE);
    if (netbuf == OSAL_NULL) {
        oam_warning_log0(0, 0, "{hwal_skb_struct_alloc::oal_malloc_netbuf_stru err}");
    }
    return netbuf;
}

/*****************************************************************************
 功能描述  : 释放netbuf_list结构
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_void oal_free_netbuf_stru(oal_netbuf_stru* netbuf)
{
    oal_mem_free(netbuf, OAL_TRUE);
}
#else

#define ZERO_COPY_LWIP_NETBUF_CNT 32
#define ZERO_COPY_LWIP_DRIECT_MAX 24   // 某一个方向申请的包最多的数量限制，否则会导致另一个方向申请不到包
OAL_STATIC netbuf_stru g_netbuf_list[ZERO_COPY_LWIP_NETBUF_CNT];

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 功能描述  : 初始化g_netbuf_list结构
*****************************************************************************/
td_void oal_init_netbuf_stru(td_void)
{
    td_u32 count;
    for (count = 0; count < ZERO_COPY_LWIP_NETBUF_CNT; count++) {
        g_netbuf_list[count].flag = 0;
    }
}

WIFI_HMAC_TCM_TEXT td_u32 oal_malloc_netbuf_used(td_u8 direct)
{
    td_u32 count;
    td_u32 total = 0;

    osal_adapt_kthread_lock();
    for (count = 0; count < ZERO_COPY_LWIP_NETBUF_CNT; count++) {
        if ((g_netbuf_list[count].flag == 1) && (g_netbuf_list[count].direct == direct)) {
            total++;
        }
    }

    osal_adapt_kthread_unlock();
    return total;
}

/*****************************************************************************
 功能描述  : 分配g_netbuf_list结构
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru* oal_malloc_netbuf_stru(td_u8 direct)
{
    td_u32 count;

    if (oal_malloc_netbuf_used(direct) >= ZERO_COPY_LWIP_DRIECT_MAX) {
        return NULL;
    }

    osal_adapt_kthread_lock();
    for (count = 0; count < ZERO_COPY_LWIP_NETBUF_CNT; count++) {
        if (g_netbuf_list[count].flag == 0) {
            g_netbuf_list[count].flag = 1;
            g_netbuf_list[count].direct = direct;
            osal_adapt_kthread_unlock();
            return &g_netbuf_list[count].netbuf;
        }
    }
    osal_adapt_kthread_unlock();

    return NULL;
}

/*****************************************************************************
 功能描述  : 释放netbuf_list结构
*****************************************************************************/
WIFI_HMAC_TCM_TEXT td_void oal_free_netbuf_stru(oal_netbuf_stru *netbuf)
{
    osal_adapt_kthread_lock();
    netbuf_stru *netbuf_str = (netbuf_stru *)(uintptr_t)netbuf;
    netbuf_str->flag = 0;
    osal_adapt_kthread_unlock();
}
#endif
#endif

td_u32 oal_netbuf_free(oal_netbuf_stru *netbuf)
{
#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    oal_dmac_netbuf_stru *pkt_buf = NULL;
#endif
    if (netbuf == NULL) {
        return OAL_FAIL;
    }

#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    pkt_buf = netbuf->pkt_buf;
    if (pkt_buf != NULL) {
        if (pkt_buf->pkt_src == PKT_BUF_SRC_LWIP) {
            pkt_buf->pkt_buf_offset = 0;
            pbuf_free((struct pbuf*)oal_netbuf_lwip(pkt_buf));
            return OAL_SUCC;
        } else if (pkt_buf->pkt_src == PKT_BUF_SRC_DMAC) {
            oal_mem_netbuf_free(pkt_buf);
            return OAL_SUCC;
        } else {
            wifi_printf("no pkt_buf_src, default free, may error!\n");
        }
    }
#endif

    /* 释放调用alloc_skb接口申请的内存 */
    if ((netbuf->mem_head == NULL) && (netbuf->head != NULL)) {
        dev_kfree_skb(netbuf);
#ifdef _PRE_LWIP_ZERO_COPY
    } else {
        /* 释放调用pbuf_alloc接口申请的内存 */
        pbuf_free((struct pbuf*)(netbuf->mem_head)); // 释放原pbuf结构体&data区
        oal_free_netbuf_stru(netbuf); // 释放skbuf结构体
#endif
    }

    return OAL_SUCC;
}


/*****************************************************************************
 功能描述  : 释放eapol链表所有节点
*****************************************************************************/
td_void oal_free_netbuf_list(oal_netbuf_head_stru* list_head)
{
    oal_netbuf_stru *skb_buf = NULL;

    while (oal_netbuf_list_empty(list_head) == OAL_FALSE) {
        skb_buf = oal_netbuf_delist(list_head);
        /* 不为空且出现空指针 链表异常 */
        if (skb_buf == NULL) {
            wifi_printf("oal_free_netbuff_list:: get netbuf is null.");
            return;
        }
        oal_netbuf_free(skb_buf);
    }
}


#ifdef _PRE_LWIP_ZERO_COPY
/*****************************************************************************
 功能描述  : 申请lwip pbuf结构体
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru* oal_pbuf_netbuf_alloc(td_u32 len)
{
    oal_netbuf_stru* netbuf = NULL;
    oal_lwip_buf*    lwip_buf = NULL;

    /*                                pbuf's memory distribution
    |---PBUF_STRU---|-------HEADROOM-------|-------payload-----|----TAILROOM-----|

    mem_head      head                     data                tail              end
    */
    lwip_buf = pbuf_alloc(PBUF_RAW, (td_u16)len, PBUF_RAM);
    if (lwip_buf == NULL) {
        return NULL;
    }

    netbuf = oal_malloc_netbuf_stru(ZERO_COPY_LWIP_DRIECT_RX);
    if (netbuf == NULL) {
        pbuf_free(lwip_buf);
        return NULL;
    }

    memset_s(netbuf, sizeof(oal_netbuf_stru), 0, sizeof(oal_netbuf_stru));
    osal_adapt_atomic_set(&netbuf->users, 1);

    netbuf->mem_head   = (td_u8*)lwip_buf;
    netbuf->head       = (td_u8*)lwip_buf->payload - PBUF_ZERO_COPY_RESERVE;
    netbuf->data       = netbuf->head;  /* 需要将cb域传到ko侧，因此data指向head */
    skb_reset_tail_pointer(netbuf);     /* netbuf->tail和len由业务调整 */
    netbuf->end        = netbuf->tail + len + PBUF_ZERO_COPY_RESERVE + PBUF_ZERO_COPY_TAILROOM;

    return netbuf;
}
#endif

/*****************************************************************************
 功能描述  : 为netbuf申请内存
 输入参数  : size: 分配内存的大小
             l_reserve: data跟指针头之间要预留的长度
 返 回 值  : 成功返回结构体指针；失败返回NULL
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_netbuf_stru* oal_netbuf_alloc(td_u32 size, td_u32 l_reserve, td_u32 align)
{
    oal_netbuf_stru *netbuf = NULL;
    td_u32          offset;
    td_u32          size_input = size;
    td_u32          align_input = align;

#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    align_input = (td_u32)CACHE_ALIGNED_SIZE;
#endif
    if (align_input != 0) {
        size_input += (align_input - 1);
    }

    netbuf = dev_alloc_skb(size_input);
    if (netbuf == NULL) {
        return NULL;
    }
    skb_reserve(netbuf, l_reserve);
    netbuf->mem_head = NULL;
#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    netbuf->pkt_buf = NULL;
#endif
    offset = ((uintptr_t)netbuf->data) % (uintptr_t)align_input;
    if (offset != 0) {
        skb_reserve(netbuf, align_input - offset);
    }

    return netbuf;
}

#ifdef _PRE_WLAN_TCP_OPT
/*****************************************************************************
功能描述  : 释放报文结构体内存空间，可用于中断环境
*****************************************************************************/
td_void  oal_netbuf_free_any(oal_netbuf_stru *netbuf)
{
    /* 释放调用alloc_skb接口申请的内存 */
    if ((netbuf->mem_head == NULL) && (netbuf->head != NULL)) {
        dev_kfree_skb_any(netbuf);
    } else {
    /* 释放调用pbuf_alloc接口申请的内存 */
#if !defined(_PRE_WLAN_FEATURE_MFG_ONLY) && !defined(_PRE_WLAN_QUICK_SEND)
        pbuf_free((struct pbuf*)(netbuf->mem_head));
#endif
        free(netbuf);
    }
}
#endif

/* 参考hmac_rx_copy_netbuff_etc */
oal_netbuf_stru* _oal_netbuf_copy(oal_netbuf_stru *src_netbuf, oal_gfp_enum_uint8 priority)
{
    oal_netbuf_stru *ppst_dest_netbuf = OAL_PTR_NULL;

    if (src_netbuf == OAL_PTR_NULL) {
        return OAL_PTR_NULL;
    }

    unref_param(priority);

    ppst_dest_netbuf = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (ppst_dest_netbuf == OAL_PTR_NULL) {
        return OAL_PTR_NULL;
    }

    /* 信息复制 */
    /* modify src bug */
    (void)memcpy_s(oal_netbuf_cb(ppst_dest_netbuf), OAL_SIZEOF(mac_rx_ctl_stru),
        oal_netbuf_cb(src_netbuf), OAL_SIZEOF(mac_rx_ctl_stru));
    if (memcpy_s(oal_netbuf_data(ppst_dest_netbuf), OAL_NETBUF_LEN(src_netbuf),
        oal_netbuf_data(src_netbuf), OAL_NETBUF_LEN(src_netbuf)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{_oal_netbuf_copy::memcpy_s data error}");
    }

    /* 设置netbuf长度、TAIL指针 */
    oal_netbuf_put(ppst_dest_netbuf, oal_netbuf_get_len(src_netbuf));

    return ppst_dest_netbuf;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

