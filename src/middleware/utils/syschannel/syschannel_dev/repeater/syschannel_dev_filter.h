/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: Header file for syschannel_dev_filter.c.
 * Create: 2023-05-01
 * Notes: repeater dev filter api.
 */

#ifndef _SYSCHANNEL_DEV_FILTER_H
#define _SYSCHANNEL_DEV_FILTER_H
#include "osal_types.h"
#include "syschannel_api.h"
#include "syschannel_filter_ipv4.h"
#include "oal_util_hcm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IP6_HLEN                40
#define IP6_NEXTH_HOPBYHOP      0
#define IP6_NEXTH_TCP           6
#define IP6_NEXTH_UDP           17
#define IP6_NEXTH_ENCAPS        41
#define IP6_NEXTH_ROUTING       43
#define IP6_NEXTH_FRAGMENT      44
#define IP6_NEXTH_ICMP6         58
#define IP6_NEXTH_NONE          59
#define IP6_NEXTH_DESTOPTS      60
#define IP6_NEXTH_UDPLITE       136

#define IP_PROTO_IP6IN4  41

#define ETHER_TYPE_RARP         0x8035
#define ETHER_TYPE_IP           0x0800  /* IP protocol */
#define ETHER_TYPE_ARP          0x0806  /* ARP protocol */
#define ETHER_TYPE_IPV6         0x86dd  /* IPv6 */
#define ETHER_TYPE_6LO          0xa0ed  /* 6lowpan包头压缩 */
#define IP4_FILTER_KEY_LEN      17
#define IP6_FILTER_KEY_LEN      29

typedef struct oal_ip6_hdr {
    union {
        struct ip6_hdrctl {
            unsigned int ip6_un1_flow;         /* 20 bits of flow-ID */
            unsigned short ip6_un1_plen;       /* payload length */
            unsigned char ip6_un1_nxt;         /* next header */
            unsigned char ip6_un1_hlim;        /* hop limit */
        } ip6_un1;
        unsigned char ip6_un2_vfc;  /* 4 bits version, top 4 bits class */
    } ip6_ctlun;
    unsigned char ip6_src[WIFI_IPV6_ADDR_LEN]; /* source address */
    unsigned char ip6_dst[WIFI_IPV6_ADDR_LEN]; /* destination address */
} oal_ipv6_hdr_stru;
#define IPV6_NEXT_HDR      ip6_ctlun.ip6_un1.ip6_un1_nxt

typedef struct ipv6_frag_hdr {
    unsigned char nexth;
    unsigned char resv;
    unsigned short offset;
    unsigned int id;
} ipv6_frag_hdr_stru;

osal_u16 syschannel_packet_receive(osal_u8 *data, osal_u8 *channel_id, syschannel_filter_list_stru *syschannel_filter);
syschannel_filter_list_stru *syschannel_get_filter_list(osal_void);
osal_void syschannel_clean_frag_filter(osal_void);
osal_void syschannel_set_ipv4_filter_max_num(osal_void);
osal_u32 syschannel_filter_init(osal_void);
osal_void syschannel_filter_deinit(osal_void);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end _SYSCHANNEL_DEV_FILTER_H */