/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Author: IoT software develop group.
 * Create: 2023-03-17.
 */

#ifndef SYSCHANNEL_WAL_API_H
#define SYSCHANNEL_WAL_API_H
#include "osal_types.h"
#include "syschannel_netbuf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct netif oal_lwip_netif;

osal_void syschannel_driverif_send(syschannel_netbuf_stru *syschannel_netbuf);
osal_u32 syschannel_driverif_receive(osal_void *buf);
osal_void syschannel_netif_clear(oal_lwip_netif *netif);
osal_void syschannel_netif_create(osal_void);
osal_void* get_syschannel_netif(osal_void);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif