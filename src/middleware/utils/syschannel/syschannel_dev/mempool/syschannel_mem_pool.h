/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: Header file for syschannel.
 * Create: 2023-05-01
 */

#ifndef __SYSCHANNEL_MEM_POOL_H__
#define __SYSCHANNEL_MEM_POOL_H__

#include "osal_list.h"
#include "osal_atomic.h"
#include "osal_types.h"
#include "soc_osal.h"
#include "syschannel_netbuf.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifdef _PRE_LWIP_SYSCHANNEL_MEM_ALLOC_BUF
#define   LARGE_PKT_LEN                     1700
#define   SHORT_PKT_LEN                     180

typedef struct {
    struct osal_list_head  list;
    osal_u32          total_len;
}syschannel_pkt_queue;

typedef struct {
    syschannel_pkt_queue large_pkt;
    syschannel_pkt_queue short_pkt;
    syschannel_pkt_queue repeater_pkt;
}syschannel_queue;

/* 内存池管理结构体 */
static inline osal_void mem_list_init(syschannel_queue *channel_queue)
{
    OSAL_INIT_LIST_HEAD(&channel_queue->large_pkt.list);
    channel_queue->large_pkt.total_len = 0;
    OSAL_INIT_LIST_HEAD(&channel_queue->short_pkt.list);
    channel_queue->short_pkt.total_len = 0;
    OSAL_INIT_LIST_HEAD(&channel_queue->repeater_pkt.list);
    channel_queue->repeater_pkt.total_len = 0;
}

static inline osal_void mem_list_add_tail(syschannel_pkt_queue *channel_pkt_queue,
    struct osal_list_head *new_node)
{
    osal_list_add(new_node, &channel_pkt_queue->list);
    channel_pkt_queue->total_len++;
}

static inline osal_void mem_list_delete_node(syschannel_pkt_queue *channel_pkt_queue,
    struct osal_list_head *node)
{
    osal_list_del(node);
    channel_pkt_queue->total_len--;
}

osal_void syschannel_init_mem_pool(osal_void);
osal_void syschannel_destory_mem_pool(osal_void);
syschannel_netbuf_stru* syschannel_mem_pool_pop(osal_u32 len);
osal_void syschannel_mem_pool_push(syschannel_netbuf_stru* syschannel_netbuf);
void syschannel_mem_push(void* syschannel_buf);
syschannel_netbuf_stru* sychannel_repeater_netbuf_pop(osal_void);
osal_void sychannel_repeater_netbuf_push(syschannel_netbuf_stru* syschannel_netbuf);
#endif
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif /* end of syschannel_mem_pool.h */
