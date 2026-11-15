#ifndef __OAL_NETBUFF_H__
#define __OAL_NETBUFF_H__

#include <linux/skbuff.h>
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// netbuf相关的接口
typedef struct sk_buff                              oal_netbuf_stru;
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

osal_u32 oal_netbuf_free(oal_netbuf_stru *netbuf);
osal_u8* oal_netbuf_put(oal_netbuf_stru* netbuf, osal_u32 len);
oal_netbuf_stru* oal_netbuf_alloc(osal_u32 ul_size, osal_u32 l_reserve, osal_u32 l_align);

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

static inline struct sk_buff *oal_netdev_alloc_skb(struct net_device *dev, unsigned int length)
{
    return __netdev_alloc_skb(dev, length, GFP_ATOMIC);
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
