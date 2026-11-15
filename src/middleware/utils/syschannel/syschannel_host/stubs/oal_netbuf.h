#ifndef __OAL_NETBUFF_H__
#define __OAL_NETBUFF_H__

#include "osal_types.h"
#include "osal_atomic.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef osal_u32 sk_buff_data_t;
struct sk_buff {
    /* These two members must be first. */
    struct sk_buff *next;
    struct sk_buff *prev;

    osal_void        *dev;               /* for hwal_netif_rx */
    osal_u32          len;
    osal_u32          data_len;
    osal_u16          queue_mapping;

    /* These elements must be at the end, see alloc_skb() for details. */
    osal_u8 *tail;
    osal_u8 *end;

    osal_s8           cb[48];  /* 48: SIZE(0..48) */
    osal_u8          *head;
    osal_u8          *data;

    osal_u32          truesize;
    osal_u32          priority;
    osal_atomic       users;

    /* use for lwip_pbuf zero_copy:actual start addr of memory space */
    osal_u8           *mem_head;
    osal_u32          cloned;
    osal_u32          protocol;

    osal_u8           *transport_header;
    osal_u8           *network_header;

    osal_u8           *mac_header;
    osal_u8           resv[2];   /* 2: bytes保留字段 */
};

struct sk_buff_head {
    /* These two members must be first. */
    struct sk_buff *next;
    struct sk_buff *prev;

    osal_u32 qlen;
    unsigned int lock;
};
// netbuf相关的接口
typedef struct sk_buff                              oal_netbuf_stru;
typedef struct sk_buff_head                         oal_netbuf_head_stru;
#define oal_netbuf_list_num(_pst_head)              ((_pst_head)->qlen)
#define oal_netbuf_next(_pst_buf)                   ((_pst_buf)->next)
#define oal_netbuf_prev(_pst_buf)                   ((_pst_buf)->prev)
#define oal_netbuf_head_next(_pst_buf_head)         ((_pst_buf_head)->next)
#define oal_netbuf_head_prev(_pst_buf_head)         ((_pst_buf_head)->prev)
#define oal_netbuf_protocol(_pst_buf)               ((_pst_buf)->protocol)
#define oal_netbuf_last_rx(_pst_buf)                ((_pst_buf)->last_rx)
#define oal_netbuf_data(_pst_buf)                   ((_pst_buf)->data)
#define oal_netbuf_header(_pst_buf)                 ((_pst_buf)->data)
#define oal_netbuf_payload(_pst_buf)                ((_pst_buf)->data)
#define oal_netbuf_cb(_pst_buf)                     ((_pst_buf)->cb)
#define oal_netbuf_cb_size()                        (sizeof(((oal_netbuf_stru*)0)->cb))
#define oal_netbuf_len(_pst_buf)                    ((_pst_buf)->len)
#define oal_netbuf_priority(_pst_buf)               ((_pst_buf)->priority)

#define OAL_NETBUF_TAIL                              skb_tail_pointer
#define OAL_NETBUF_QUEUE_TAIL                        skb_queue_tail
#define OAL_NETBUF_QUEUE_HEAD_INIT                   skb_queue_head_init
#define OAL_NETBUF_DEQUEUE                           skb_dequeue

#ifndef NET_SKB_PAD
#define NET_SKB_PAD 64
#endif
osal_u32 oal_netbuf_free(oal_netbuf_stru *netbuf);
oal_netbuf_stru* oal_netbuf_alloc(osal_u32 ul_size, osal_u32 l_reserve, osal_u32 l_align);
osal_s32 pskb_expand_head(struct sk_buff *skb, osal_s32 nhead, osal_s32 ntail, osal_s32 gfp_mask);
osal_u8 *skb_put(struct sk_buff *skb, osal_u32 len);

static inline osal_u8 *skb_pull(struct sk_buff *skb, osal_u32 len)
{
    skb->len -= len;
    return skb->data += len;
}

static inline osal_u8 *skb_push(struct sk_buff *skb, osal_u32 len)
{
    if (skb->data - len < skb->head) {
        return NULL;
    }

    skb->data -= len;
    skb->len  += len;
    return skb->data;
}

static inline osal_s32 skb_headroom(const struct sk_buff *skb)
{
    return (osal_s32)(skb->data - skb->head);
}

static inline osal_s32 skb_is_nonlinear(const struct sk_buff *skb)
{
    return skb->data_len;
}

static inline osal_void skb_reserve(struct sk_buff *skb, osal_u32 len)
{
    skb->data += len;
    skb->tail += len;
}

static inline osal_u32 skb_tailroom(const struct sk_buff *skb)
{
    return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}


static inline osal_u8* oal_netbuf_pull(oal_netbuf_stru *netbuf, osal_u32 len)
{
    if (len > netbuf->len) {
        return OSAL_NULL;
    }

    return skb_pull(netbuf, len);
}

/*****************************************************************************
 功能描述  : 在缓冲区开头增加数据
*****************************************************************************/
static inline osal_u8*  oal_netbuf_push(oal_netbuf_stru* netbuf, osal_u32 len)
{
    return skb_push(netbuf, len);
}

/*****************************************************************************
功能描述  : 在缓冲区尾部增加数据
*****************************************************************************/
static inline osal_u8* oal_netbuf_put(oal_netbuf_stru* netbuf, osal_u32 len)
{
    return skb_put(netbuf, len);
}

/*****************************************************************************
 功能描述  : 获取头部空间大小
*****************************************************************************/
static inline osal_u32  oal_netbuf_headroom(const oal_netbuf_stru* netbuf)
{
    return (osal_u32)skb_headroom(netbuf);
}

/*****************************************************************************
 功能描述  : 获取尾部空间大小
*****************************************************************************/
static inline osal_u32  oal_netbuf_tailroom(const oal_netbuf_stru* netbuf)
{
    return (osal_u32)skb_tailroom(netbuf);
}

/*****************************************************************************
功能描述: 扩展netbuf 头部空间
*****************************************************************************/
static inline osal_s32 oal_netbuf_expand_head(oal_netbuf_stru *netbuf, osal_s32 nhead,
    osal_s32 ntail, osal_s32 gfp_mask)
{
    return pskb_expand_head(netbuf, nhead, ntail, gfp_mask);
}

struct sk_buff *alloc_skb(osal_u32 size);
static inline struct sk_buff *oal_netbuf_alloc_skb(unsigned int length)
{
    struct sk_buff *skb = alloc_skb(length + NET_SKB_PAD);
    if (skb != NULL)
        skb_reserve(skb, NET_SKB_PAD);
    return skb;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif