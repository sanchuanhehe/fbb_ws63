/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: Header file for syschannel_filter_ipv4.c.
 * Create: 2023-05-01
 * Notes: repeater dev filter api.
 */

#ifndef _SYSCHANNEL_FILTER_IPV4_H
#define _SYSCHANNEL_FILTER_IPV4_H
#include "osal_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6
#define WIFI_FILTER_MAX_NUM   20
#define WIFI_FRAG_MAX_NUM     10
#define WIFI_IPV6_ADDR_LEN    16

typedef enum {
    WIFI_IP_NO_FRAG              = 0x00,
    WIFI_IP_FIRST_FRAG           = 0x01,
    WIFI_IP_MIDDLE_FRAG          = 0x02,
    WIFI_IP_LAST_FRAG            = 0x03,
    WIFI_IP_FRAG_BUTT
} wifi_ip_frag_enum;

typedef struct oal_ipv4_frag {
    unsigned int   ip;
    unsigned short id;
    unsigned short us_dir;
    unsigned char  channel_id; /* 用于非第一帧的同步信息 */
    unsigned char  resv[3];    // 4字节对齐
} oal_ipv4_frag_info;

typedef struct oal_ipv6_frag {
    unsigned char  auc_ip[WIFI_IPV6_ADDR_LEN];
    unsigned int   id;
    unsigned short us_dir;
    unsigned char  channel_id; /* 用于非第一帧的同步信息 */
    unsigned char  resv;    // 4字节对齐
} oal_ipv6_frag_info;

typedef struct {
    unsigned int   remote_ip;
    unsigned short local_port;
    unsigned short localp_min;
    unsigned short localp_max;
    unsigned short remote_port;
    unsigned short remotep_min;
    unsigned short remotep_max;
    unsigned char  packet_type;
    unsigned char  config_type;
    unsigned char  match_mask;
    unsigned char  channel_id; /* config_type为WIFI_FILTER_VLWIP时需要指定channel_id */
} syschannel_ipv4_filter_stru;

typedef struct {
    unsigned char  remote_ip[WIFI_IPV6_ADDR_LEN];
    unsigned short local_port;
    unsigned short localp_min;
    unsigned short localp_max;
    unsigned short remote_port;
    unsigned short remotep_min;
    unsigned short remotep_max;
    unsigned char  packet_type;
    unsigned char  config_type;
    unsigned char  match_mask;
    unsigned char  channel_id; /* config_type为WIFI_FILTER_VLWIP时需要指定channel_id */
} syschannel_ipv6_filter_stru;

typedef struct syschannel_filter_list {
    syschannel_ipv4_filter_stru ipv4_filters[WIFI_FILTER_MAX_NUM];
    syschannel_ipv6_filter_stru ipv6_filters[WIFI_FILTER_MAX_NUM];
    oal_ipv4_frag_info ipv4_frag_cache;
    oal_ipv4_frag_info ipv4_frags[WIFI_FRAG_MAX_NUM];
    oal_ipv6_frag_info ipv6_frag_cache;
    oal_ipv6_frag_info ipv6_frags[WIFI_FRAG_MAX_NUM];
    unsigned char ipv4_frag_pos;
    unsigned char ipv4_frag_num;
    unsigned char ipv6_frag_pos;
    unsigned char ipv6_frag_num;
    unsigned char ipv4_filter_cache;
    unsigned char ipv4_filter_num;
    unsigned char ipv4_filter_max_num;   // 为了防止rom化将规格定死，保存一份最大值
    unsigned char ipv6_filter_cache;
    unsigned char ipv6_filter_num;
    unsigned char default_netif;
    unsigned char default_channel_id;
    unsigned char resv;                  // 4字节对齐
} syschannel_filter_list_stru;

osal_u16 syschannel_forward_ipv4_packet(osal_u8 *data, osal_u8 *channel_id,
    syschannel_filter_list_stru *syschannel_filter);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end _SYSCHANNEL_DEV_FILTER_H */