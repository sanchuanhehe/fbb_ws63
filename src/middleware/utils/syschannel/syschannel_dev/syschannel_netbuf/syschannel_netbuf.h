/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Author: IoT software develop group
 * Create: 2023-05-01
 */

#ifndef SYSCHANNEL_NETBUFF_H
#define SYSCHANNEL_NETBUFF_H

#include "osal_types.h"
#include "soc_osal.h"
#include "osal_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    MEM_SHORT_PKT_ALLOC,
    MEM_LARGE_PKT_ALLOC,
    MEM_REPEATER_ALLOC,
    MEM_NETBUF_MIXED_ALLOC,
    MEM_TYPE_BUTT
}syschannel_mem_type_enum;

typedef struct {
    struct osal_list_head       list;                   /* 用于挂接到队列,串联成表 */
    osal_u8                     *buf;                   /* 记录内存指针 */
    osal_u8                     *head;                  /* 记录数据payload指针 */
    osal_u8                     type;                   /* 记录类型，区分转发的buf和池子申请的 */
    osal_u8                     resv[3];                /* 4字节对齐 */
} syschannel_netbuf_stru;

osal_u32 syschannel_netbuf_get_len(syschannel_netbuf_stru *syschannel_netbuf);
osal_u32 syschannel_netbuf_set_len(syschannel_netbuf_stru *syschannel_netbuf, osal_u32 len);
osal_u8* syschannel_netbuf_data(syschannel_netbuf_stru *syschannel_netbuf);
syschannel_netbuf_stru* syschannel_netbuf_alloc(osal_u32 len);
osal_void syschannel_netbuf_free(syschannel_netbuf_stru *syschannel_netbuf);
osal_u32 syschannel_netbuf_pull(syschannel_netbuf_stru *syschannel_netbuf, osal_s32 len);
osal_u32 syschannel_netbuf_push(syschannel_netbuf_stru *syschannel_netbuf, osal_s32 len);
syschannel_netbuf_stru* syschannel_netbuf_stru_alloc(osal_void);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif