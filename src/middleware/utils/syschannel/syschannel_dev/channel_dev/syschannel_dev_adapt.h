/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Author: IoT software develop group.
 * Create: 2023-03-17.
 */

#ifndef __SYSCHANNEL_DEV_ADAPT_H__
#define __SYSCHANNEL_DEV_ADAPT_H__

#include "osal_types.h"
#include "soc_osal.h"
#include "hcc_if.h"
#include "hcc_cfg.h"
#include "syschannel_api.h"
#include "syschannel_common.h"
#include "syschannel_netbuf.h"
#include "oal_util_hcm.h"
#include "lwip/netifapi.h"
#include "oal_net_cfg80211.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define SYSCHANNEL_DEV_MAX_NUM    2
#define SYSCHANNEL_MAC_ADDR_LEN   6

osal_u32 syschannel_dev_create(bus_type type, osal_u8 *channel_id);
osal_u32 syschannel_check_channel_status(osal_u8 channel_id);
unsigned int syschannel_send_msg(syschannel_handler *handler, char* buf, int length, int syschannel_sub_type_msg);
osal_u32 syschannel_tx_data_adapt(osal_u8 channel_id, syschannel_netbuf_stru *syschannel_netbuf,
    syschannel_service_type type, osal_u32 sub_type);
osal_void syschannel_dev_sync_mac_ip_addr(const osal_u8 *addr, const ip4_addr_t *ipaddr,
    const ip4_addr_t *netmask, const ip4_addr_t *gw);
osal_u32 syschannel_dev_create_task(bus_type type);
/* 定时器相关函数 */
osal_void syschannel_dev_heartbeat_timer_create(osal_u8 channel_id);
osal_void syschannel_dev_heartbeat_timer_destroy(osal_u8 channel_id);
osal_void syschannel_dev_heartbeat_timer_restart(osal_u8 channel_id);
osal_u32 syschannel_dev_heartbeat_timeout(osal_u8 channel_id);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif