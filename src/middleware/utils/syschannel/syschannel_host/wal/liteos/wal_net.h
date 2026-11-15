/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: wal_net.c的API接口文件。
 * Create: 2024-10-17
 */
#ifndef __WAL_NET_H__
#define __WAL_NET_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "securec.h"
#include "osal_types.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "hcc_cfg_comm.h"
#include "syschannel_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
typedef struct netif                        oal_lwip_netif;
typedef struct pbuf                         oal_lwip_buf;
typedef ip4_addr_t                          oal_ip_addr_t;
#define OAL_IP4_ADDR    IP4_ADDR
#define ETHER_ADDR_LEN          6

/*****************************************************************************
    5 函数声明
*****************************************************************************/
osal_s32 netdev_register(osal_void);
osal_void netdev_unregister(osal_void);
osal_u32 syschannel_host_alloc_netbuf(hcc_queue_type queue_id, osal_u32 len, osal_u8 **buf, osal_u8 **user_param);
osal_void syschannel_host_netbuf_free(hcc_queue_type queue_id, osal_u8 *buf, osal_u8 *user_param);
osal_u32 syschannel_host_rx_data_process(hcc_queue_type queue_id, osal_u8 stype,
    osal_u8 *buf, osal_u32 len, osal_u8 *user_param);
osal_u32 syschannel_host_sync_mac_addr(osal_u8 *buf, osal_u32 len);
osal_u32 syschannel_host_sync_ip_addr(osal_u8 *buf, osal_u32 len);
ip_addr_t syschannel_host_get_ip_addr(osal_void);
ip_addr_t syschannel_host_get_netmask_addr(osal_void);
ip_addr_t syschannel_host_get_gw_addr(osal_void);
osal_u8* syschannel_host_get_mac_addr(osal_void);
osal_u32 syschannel_liteos_host_tx_data_adapt(oal_lwip_buf *lwip_buf, syschannel_service_type type, osal_u32 sub_type);
osal_void syschannel_send_pkt_register(osal_void *pkt_fn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __WAL_NET_H__ */
