/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: skb function.
 */
#include "oal_skbuff.h"
#include <malloc.h>
#include "oal_types.h"
#include "oal_netbuf.h"
#include "oal_util_hcm.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_OAL_SKBUFF_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*
 * 功能描述:分配skb内部函数
 */
OAL_STATIC WIFI_HMAC_TCM_TEXT struct sk_buff *_alloc_skb(td_u32 size)
{
    struct sk_buff *skb = NULL;
    td_u8 *data = NULL;

    skb = (struct sk_buff *)memalign(USB_CACHE_ALIGN_SIZE, skb_data_align(sizeof(struct sk_buff)));
    if (skb == NULL) {
        return NULL;
    }

    data = (td_u8 *)memalign(USB_CACHE_ALIGN_SIZE, size);
    if (data == NULL) {
        free(skb);
        return NULL;
    }

    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(skb, sizeof(struct sk_buff), 0, sizeof(struct sk_buff));
    skb->truesize = (osal_u32)skb_truesize(size);
    osal_adapt_atomic_set(&skb->users, 1);
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
    skb->end = skb->tail + size;

    return skb;
}

/*
 * 功能描述:分配skb
 */
WIFI_HMAC_TCM_TEXT struct sk_buff *alloc_skb(td_u32 size)
{
    return _alloc_skb(size);
}

/*
 * 功能描述:dev分配skb
 */
WIFI_HMAC_TCM_TEXT struct sk_buff *dev_alloc_skb(td_u32 length)
{
    /*
     * There is more code here than it seems:
     * __dev_alloc_skb is an inline
     */
    return _dev_alloc_skb(length);
}

/*
 * 功能描述:链表尾部插入skb
 */
WIFI_HMAC_TCM_TEXT td_u8 *skb_put(struct sk_buff *skb, td_u32 len)
{
    td_u8 *tmp = skb_tail_pointer(skb);
    skb->tail += len;
    skb->len  += len;
    if (skb->tail > skb->end) {
        wifi_printf("!!skb_put overflow!!\n");
    }
    return tmp;
}

/*
 * 功能描述:skb出队
 */
WIFI_HMAC_TCM_TEXT WIFI_TCM_TEXT struct sk_buff *skb_dequeue(struct sk_buff_head *list)
{
    td_u32 flags;
    struct sk_buff *result = NULL;

#ifndef FREERTOS_DEFINE
    spin_lock_irqsave(&list->lock, flags);
#else
    osal_spin_lock_irqsave(&list->lock, (long unsigned int *)&flags);
#endif

    result = _skb_dequeue(list);

#ifndef FREERTOS_DEFINE
    spin_unlock_irqrestore(&list->lock, flags);
#else
    osal_spin_unlock_irqrestore(&list->lock, (long unsigned int *)&flags);
#endif

    return result;
}

/*
 * 功能描述:释放skb
 */
OAL_STATIC td_void kfree_skb(struct sk_buff *skb)
{
    if (skb == NULL) {
        return;
    }
    free(skb->head);
    free(skb);
}

/*
 * 功能描述:skb队列尾部
 */
WIFI_HMAC_TCM_TEXT WIFI_TCM_TEXT td_void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
    td_u32 flags;

#ifndef FREERTOS_DEFINE
    spin_lock_irqsave(&list->lock, flags);
#else
    osal_spin_lock_irqsave(&list->lock, (long unsigned int *)&flags);
#endif

    __skb_queue_tail(list, newsk);

#ifndef FREERTOS_DEFINE
    spin_unlock_irqrestore(&list->lock, flags);
#else
    osal_spin_unlock_irqrestore(&list->lock, (long unsigned int *)&flags);
#endif
}

/*
 * 功能描述:skb trim
 */
WIFI_HMAC_TCM_TEXT WIFI_TCM_TEXT td_void skb_trim(struct sk_buff *skb, td_u32 len)
{
    if (skb->len > len) {
        _skb_trim(skb, len);
    }
}

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
OAL_STATIC td_s32 pskb_expand_head_pbuf(struct sk_buff *skb_ori, td_u32 nhead, td_u32 ntail)
{
    td_u32 size = nhead + (td_u32)(skb_end_pointer(skb_ori) - skb_ori->head) + ntail - PBUF_ZERO_COPY_RESERVE;
    oal_lwip_buf       *lwip_buf = NULL;

    size = skb_data_align(size);

    lwip_buf = pbuf_alloc(PBUF_RAW, (td_u16)size, PBUF_RAM);    /* size是否包括了reserve */
    if (lwip_buf == NULL) {
        return -ENOMEM;
    }
    memset_s(lwip_buf->payload, size, 0, size);

    osal_u8 *head_pos = (td_u8 *)lwip_buf->payload - PBUF_ZERO_COPY_RESERVE;
    osal_u8 *data_pos = head_pos + nhead + skb_headroom(skb_ori);
    sk_buff_data_t tail = skb_ori->tail + nhead;
    sk_buff_data_t end = size + PBUF_ZERO_COPY_RESERVE;
    if (memcpy_s(head_pos + nhead, size + PBUF_ZERO_COPY_RESERVE - nhead,
        skb_ori->head, (td_u32)(skb_tail_pointer(skb_ori) - skb_ori->head)) != EOK) {
        wifi_printf("pskb_expand_head :: memcpy_s failed\n");
    }

    pbuf_free((struct pbuf*)(skb_ori->mem_head));

    skb_ori->mem_head = (td_u8 *)lwip_buf;
    skb_ori->head = head_pos;
    skb_ori->data = data_pos;
    skb_ori->tail = tail;
    skb_ori->end = end;
    skb_ori->len = skb_ori->tail - skb_headroom(skb_ori);

    return 0;
}
#endif
/*
 * 功能描述:skb扩展头
 */
td_s32 pskb_expand_head(struct sk_buff *skb, td_u32 nhead, td_u32 ntail, td_s32 gfp_mask)
{
    td_u8 *data = NULL;
    td_u32 size = nhead + (td_u32)(skb_end_pointer(skb) - skb->head) + ntail;
    td_u32 off;
    td_u32 data_off;

    unref_param(gfp_mask);
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    if (!((skb->mem_head == NULL) && (skb->head != NULL))) {
        return pskb_expand_head_pbuf(skb, nhead, ntail);
    }
#endif
    size = skb_data_align(size);
    data = (td_u8 *)memalign(CACHE_ALIGNED_SIZE, size);
    if (!data) {
        return -ENOMEM;
    }
    /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
    memset_s(data, size, 0, size);
    if (memcpy_s(data + nhead, size - nhead, skb->head, (td_u32)(skb_tail_pointer(skb) - skb->head)) != EOK) {
        wifi_printf("pskb_expand_head :: memcpy_s failed\n");
    }

    data_off = (td_u32)(skb->data - skb->head);

    free(skb->head);
    skb->head = data;
    skb->data = skb->head + data_off + nhead;
    skb->end = size;
    off = nhead;
    skb->tail += off;

    return 0;
}

/*
 * 功能描述:dev释放skb
 */
td_void dev_kfree_skb(struct sk_buff *skb)
{
    if (skb == NULL) {
        return;
    }
    kfree_skb(skb);
}

/*
 * 功能描述:skb unshare
 */
WIFI_HMAC_TCM_TEXT struct sk_buff *skb_unshare(struct sk_buff *skb, td_u32 pri)
{
    unref_param(pri);
    return skb;
}

