/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: oal net file.
 */

#include "oal_net.h"
#include "mac_frame.h"
#include "mac_data.h"
#include "soc_diag_wdk.h"
#include "oam_ext_if.h"
#include "wal_net.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_OAL_NET_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#define wlan_tos_to_hcc_queue(_tos) (      \
        (((_tos) == 0) || ((_tos) == 3)) ? WLAN_UDP_BE_QUEUE : \
        (((_tos) == 1) || ((_tos) == 2)) ? WLAN_UDP_BK_QUEUE : \
        (((_tos) == 4) || ((_tos) == 5)) ? WLAN_UDP_VI_QUEUE : \
        WLAN_UDP_VO_QUEUE)

#define WLAN_DATA_VIP_QUEUE (WLAN_HI_QUEUE)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
oal_net_device_stru *g_past_net_device[WLAN_VAP_SUPPORT_MAX_NUM_LIMIT] = {OSAL_NULL};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
oal_net_device_stru* oal_get_past_net_device_by_index(td_u32 netdev_index)
{
    return g_past_net_device[netdev_index];
}

td_void oal_set_past_net_device_by_index(td_u32 netdev_index, oal_net_device_stru *netdev)
{
    if (netdev_index >= WLAN_VAP_SUPPORT_MAX_NUM_LIMIT) {
        return;
    }
    g_past_net_device[netdev_index] = netdev;
}

OAL_STATIC OAL_INLINE osal_u8 oal_high_half_byte(osal_u8 a)
{
    return (((a) & 0xF0) >> OAL_BYTE_HALF_WIDTH);
}

OAL_STATIC OAL_INLINE osal_u8 oal_low_half_byte(osal_u8 a)
{
    return ((a) & 0x0F);
}

/*****************************************************************************
 功能描述  : 根据名字寻找netdevice

 修改历史      :
  1.日    期   : 2012年12月12日
    修改内容   : 新生成函数
*****************************************************************************/
oal_net_device_stru* oal_get_netdev_by_name(const td_char* pc_name)
{
    td_u32 netdev_index;
    oal_net_device_stru *netdev = OSAL_NULL;

    if (pc_name == OSAL_NULL) {
        return OSAL_NULL;
    }
    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if ((netdev != OSAL_NULL) &&
            (osal_strcmp((const td_char*)netdev->name, (const td_char*)pc_name) == 0)) {
            return netdev;
        }
    }

    return OSAL_NULL;
}

/*****************************************************************************
 功能描述  : 根据类型寻找netdevice

 修改历史      :
  1.日    期   : 2012年12月12日
    修改内容   : 新生成函数
*****************************************************************************/
oal_net_device_stru* oal_get_netdev_by_type(nl80211_iftype_uint8 type)
{
    td_u32               netdev_index;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_u32               dev_name_len = OAL_IF_NAME_SIZE;
    td_char              dev_name[OAL_IF_NAME_SIZE];

    if (type >= NUM_NL80211_IFTYPES) {
        return OSAL_NULL;
    }

    /* strncpy源内存全部是静态字符串常量，可以不用安全函数 */
    switch (type) {
        case NL80211_IFTYPE_STATION:
            strncpy_s(dev_name, dev_name_len, "wlan", osal_strlen("wlan") + 1);
            break;
        case NL80211_IFTYPE_AP:
            strncpy_s(dev_name, dev_name_len, "ap", osal_strlen("ap") + 1);
            break;
#ifdef _PRE_WLAN_FEATURE_P2P
        case NL80211_IFTYPE_P2P_DEVICE:
        case NL80211_IFTYPE_P2P_CLIENT:
            /* fall-through */
        case NL80211_IFTYPE_P2P_GO:
            strncpy_s(dev_name, dev_name_len, "p2p", osal_strlen("p2p") + 1);
            break;
#endif
        case NL80211_IFTYPE_MESH_POINT:
            strncpy_s(dev_name, dev_name_len, "mesh", osal_strlen("mesh") + 1);
            break;
        default:
            oam_error_log0(0, 0, "{oal_get_netdev_by_type::not supported dev type!}");
            return OSAL_NULL;
    }
    dev_name_len = osal_strlen((td_char *)dev_name);

    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if ((netdev != OSAL_NULL) &&
            (osal_adapt_strncmp((const td_char*)netdev->name, dev_name, dev_name_len) == 0)) {
            return netdev;
        }
    }

    return OSAL_NULL;
}

/*****************************************************************************
 功能描述  :校验设备类型并尝试获取设备名
*****************************************************************************/
td_u32 oal_net_check_and_get_devname(nl80211_iftype_uint8 type, char* dev_name, td_u32* len)
{
    td_u32   netdev_index;
    td_s32   netdev_count = 0;
    oal_net_device_stru *netdev = OSAL_NULL;

    /* 获取已注册netdev信息 */
    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if (netdev != OSAL_NULL) {
            ++netdev_count;
        }
    }
    /* 最多只支持3个netdev共存 */
    if (netdev_count > 3) {  /* 3: 最多只支持4个netdev共存 */
        oam_error_log0(0, 0, "{oal_net_check_and_get_devname::already have 4 vaps. Could not start a new one!}");
        return OAL_FAIL;
    }

    /* strncpy源内存全部是静态字符串常量，可以不用安全函数 */
    switch (type) {
        case NL80211_IFTYPE_STATION:
            strncpy_s(dev_name, *len, "wlan", osal_strlen("wlan") + 1);
            break;
        case NL80211_IFTYPE_AP:
            strncpy_s(dev_name, *len, "ap", osal_strlen("ap") + 1);
            break;
#ifdef _PRE_WLAN_FEATURE_P2P
        case NL80211_IFTYPE_P2P_DEVICE:
        case NL80211_IFTYPE_P2P_CLIENT:
            /* fall-through */
        case NL80211_IFTYPE_P2P_GO:
            strncpy_s(dev_name, *len, "p2p", osal_strlen("p2p") + 1);
            break;
#endif
        case NL80211_IFTYPE_MESH_POINT:
            strncpy_s(dev_name, *len, "mesh", osal_strlen("mesh") + 1);
            break;
        default:
            oam_error_log0(0, 0, "{oal_net_check_and_get_devname::not supported dev type!}");
            return OAL_FAIL;
    }
    *len = osal_strlen(dev_name);
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : oal_netbuf_is_dhcp_port_etc
 功能描述  : 判断是否是dhcp帧 port
*****************************************************************************/
OAL_INLINE oal_bool_enum_uint8 oal_netbuf_is_dhcp_port_etc(const oal_udp_header_stru *udp_hdr)
{
    if (((oal_host2net_short(udp_hdr->source) == 68) && /* 68 dhcp帧 port */
        (oal_host2net_short(udp_hdr->dest) == 67)) || /* 67 dhcp帧 port */
        ((oal_host2net_short(udp_hdr->source) == 67) && /* 67 dhcp帧 port */
        (oal_host2net_short(udp_hdr->dest) == 68))) { /* 68 dhcp帧 port */
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

/*****************************************************************************
 函 数 名  : oal_netbuf_is_dhcp6_etc
 功能描述  : 判断是否是DHCP6帧
*****************************************************************************/
oal_bool_enum_uint8 oal_netbuf_is_dhcp6_etc(oal_ipv6hdr_stru  *ipv6hdr)
{
    oal_udp_header_stru           *udp_hdr;

    if (ipv6hdr->nexthdr == MAC_UDP_PROTOCAL) {
        udp_hdr = (oal_udp_header_stru *)(ipv6hdr + 1);

        if (((oal_host2net_short(udp_hdr->source) == MAC_IPV6_UDP_SRC_PORT) &&
             (oal_host2net_short(udp_hdr->dest) == MAC_IPV6_UDP_DES_PORT)) ||
            ((oal_host2net_short(udp_hdr->source) == MAC_IPV6_UDP_DES_PORT) &&
             (oal_host2net_short(udp_hdr->dest) == MAC_IPV6_UDP_SRC_PORT))) {
            return OAL_TRUE;
        }
    }

    return OAL_FALSE;
}

oal_net_device_stru* oal_net_alloc_netdev(const td_char *name, td_u8 max_name_len)
{
    td_u32               size;
    oal_net_device_stru *netdev = OSAL_NULL;

    unref_param(max_name_len);

    if (name == OSAL_NULL) {
        return OSAL_NULL;
    }
    size = osal_strlen((const td_char*)name) + 1;
    netdev = (oal_net_device_stru*)malloc(sizeof(oal_net_device_stru));
    if (netdev == OSAL_NULL) {
        return OSAL_NULL;
    }

    memset_s(netdev, sizeof(oal_net_device_stru), 0, sizeof(oal_net_device_stru));

    if (memcpy_s(netdev->name, OAL_IF_NAME_SIZE, name, size) != EOK) {
        free(netdev);
        return OSAL_NULL;
    }

    return netdev;
}

td_void oal_net_free_netdev(oal_net_device_stru *netdev)
{
    if (netdev == OSAL_NULL) {
        return;
    }
    if (netdev->priv != OSAL_NULL) {
        free((td_void *)netdev->priv);
        netdev->priv = OSAL_NULL;
    }
    if (netdev->ieee80211_ptr != OSAL_NULL) {
        if (netdev->ieee80211_ptr->preset_chandef.chan != OSAL_NULL) {
            free(netdev->ieee80211_ptr->preset_chandef.chan);
            netdev->ieee80211_ptr->preset_chandef.chan = OSAL_NULL;
        }
        free(netdev->ieee80211_ptr);
        netdev->ieee80211_ptr = OSAL_NULL;
    }
    oal_free_netbuf_list(&(netdev->ext_eapol.eapol_skb_head));
    free((td_void *)netdev);
}

td_u32 oal_net_register_netdev(oal_net_device_stru* netdev)
{
    td_u32   netdev_index;
    td_u8    dev_register = OSAL_FALSE;
    oal_net_device_stru *netdev_temp = OSAL_NULL;

    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_lwip_register_netdev parameter NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (netdev_index = 0; netdev_index < WLAN_VAP_SUPPORT_MAX_NUM_LIMIT; netdev_index++) {
        netdev_temp = oal_get_past_net_device_by_index(netdev_index);
        if (netdev_temp == OSAL_NULL) {
            oal_set_past_net_device_by_index(netdev_index, netdev);

            dev_register = OSAL_TRUE;
            break;
        }
    }

    if (dev_register != OSAL_TRUE) {
        return OAL_FAIL;
    }

    if (osal_adapt_strncmp(netdev->name, "Featureid", osal_strlen("Featureid")) == 0) {
        return OAL_SUCC;
    }

    oal_netbuf_head_init(&netdev->ext_eapol.eapol_skb_head);

    if (hwal_lwip_register(netdev) != OAL_SUCC) {
        oal_set_past_net_device_by_index(netdev_index, OSAL_NULL);

        return OAL_FAIL;
    }

    return OAL_SUCC;
}

td_void oal_net_unregister_netdev(oal_net_device_stru* netdev)
{
    td_u32    netdev_index;
    oal_net_device_stru *netdev_temp = OSAL_NULL;

    if (netdev == OSAL_NULL) {
        return;
    }

    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev_temp = oal_get_past_net_device_by_index(netdev_index);
        if (netdev_temp == netdev) {
            oal_set_past_net_device_by_index(netdev_index, OSAL_NULL);
            hwal_lwip_unregister_netdev(netdev);
            return;
        }
    }
}

/*****************************************************************************
 函 数 名  : oal_netbuf_is_tcp_ack_etc
 功能描述  : 判断ipv4 tcp报文是否为tcp 关键帧，包括TCP ACK\TCP SYN帧等
*****************************************************************************/
WIFI_HMAC_TCM_TEXT oal_bool_enum_uint8 oal_netbuf_is_tcp_ack_etc(oal_ip_header_stru  *ip_hdr)
{
    oal_tcp_header_stru    *tcp_hdr;
    osal_u32              ip_pkt_len;
    osal_u32              ip_header_len;
    osal_u32              tcp_header_len;

    ip_pkt_len   = oal_net2host_short(ip_hdr->tot_len);
    ip_header_len   = (oal_low_half_byte(ip_hdr->ihl)) << 2; /* 2左移位数 */
    tcp_hdr     = (oal_tcp_header_stru *)((osal_u8 *)ip_hdr + ip_header_len);
    tcp_header_len  = (oal_high_half_byte(tcp_hdr->offset)) << 2; /* 2左移位数 */
    if (tcp_header_len + ip_header_len == ip_pkt_len) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

/*****************************************************************************
 函 数 名  : oal_netbuf_is_icmp_etc
 功能描述  : 判断ipv4 是否为icmp报文
 输入参数  : ip_hdr: IP报文头部
*****************************************************************************/
WIFI_TCM_TEXT oal_bool_enum_uint8 oal_netbuf_is_icmp_etc(oal_ip_header_stru  *ip_hdr)
{
    osal_u8  protocol;
    protocol = ip_hdr->protocol;

    /* ICMP报文检查 */
    if (protocol == MAC_ICMP_PROTOCAL) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

/*****************************************************************************
 函 数 名  : mac_is_tcp_ack6
 功能描述  : 判断ipv6 tcp报文是否为tcp ack
*****************************************************************************/
oal_bool_enum_uint8 oal_netbuf_is_tcp_ack6_etc(oal_ipv6hdr_stru  *ipv6hdr)
{
    oal_tcp_header_stru    *tcp_hdr;
    osal_u32              ip_pkt_len;
    osal_u32              tcp_hdr_len;

    tcp_hdr     = (oal_tcp_header_stru *)(ipv6hdr + 1);
    ip_pkt_len   = oal_net2host_short(ipv6hdr->payload_len); /* ipv6 净载荷, ipv6报文头部固定为40字节 */
    tcp_hdr_len  = (oal_high_half_byte(tcp_hdr->offset)) << 2; /* 2左移位数 */
    if (tcp_hdr_len == ip_pkt_len) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

/*****************************************************************************
 函 数 名  : oal_netbuf_select_queue_etc
 功能描述  : 为以太网来包选择合的subqueue入队
*****************************************************************************/
osal_u16 oal_netbuf_select_queue_etc(oal_netbuf_stru *buf)
{
    oal_ether_header_stru *ether_header = OSAL_NULL;
    oal_ip_header_stru *pst_ip = OSAL_NULL;
    oal_ipv6hdr_stru *ipv6 = OSAL_NULL;
    oal_udp_header_stru *udp_hdr = OSAL_NULL;
    osal_u8 queue = WLAN_NORMAL_QUEUE;
    osal_u32 ip_hdr_len;

    /* 获取以太网头 */
    ether_header = (oal_ether_header_stru *)OAL_NETBUF_DATA(buf);

    switch (ether_header->ether_type) {
        case oal_host2net_short(ETHER_TYPE_IP):

            pst_ip = (oal_ip_header_stru *)(ether_header + 1);      /* 偏移一个以太网头，取ip头 */

            /* 对udp报文区分qos入队 */
            if (pst_ip->protocol == MAC_UDP_PROTOCAL) {
                queue = WLAN_UDP_DATA_QUEUE;
                /* 如果是DHCP帧，则进入DATA_HIGH_QUEUE */
                ip_hdr_len = pst_ip->ihl << 2;       /* 左移2bit:值对应4字节 */
                udp_hdr = (oal_udp_header_stru *)((osal_u8 *)pst_ip + ip_hdr_len);
                /* 增加dhcp不是分片报文的判断 */
                if (((pst_ip->frag_off & 0xFF1F) == 0) && (oal_netbuf_is_dhcp_port_etc(udp_hdr) == OAL_TRUE)) {
                    queue = WLAN_DATA_VIP_QUEUE;
                }
            } else if (pst_ip->protocol == MAC_TCP_PROTOCAL) { /* 区分TCP ack与TCP data报文 */
                if (oal_netbuf_is_tcp_ack_etc(pst_ip) == OAL_TRUE) {
                    queue = WLAN_TCP_ACK_QUEUE;
                } else {
                    queue = WLAN_TCP_DATA_QUEUE;
                }
            }
            break;

        case oal_host2net_short(ETHER_TYPE_IPV6):

            /* 从IPv6 traffic class字段获取优先级 */
            /*----------------------------------------------------------------------
                IPv6包头 前32为定义
             -----------------------------------------------------------------------
            | 版本号 | traffic class   | 流量标识 |
            | 4bit   | 8bit(同ipv4 tos)|  20bit   |
            -----------------------------------------------------------------------*/
            ipv6    = (oal_ipv6hdr_stru *)(ether_header + 1); /* 偏移一个以太网头，取ip头 */
            if (ipv6->nexthdr == MAC_UDP_PROTOCAL) { /* UDP报文 */
                queue = WLAN_UDP_DATA_QUEUE;
            } else if (ipv6->nexthdr == MAC_TCP_PROTOCAL) {
                if (oal_netbuf_is_tcp_ack6_etc(ipv6) == OAL_TRUE) { /* TCP报文 */
                    queue = WLAN_TCP_ACK_QUEUE;
                } else {
                    queue = WLAN_TCP_DATA_QUEUE;
                }
            } else if (oal_netbuf_is_dhcp6_etc((oal_ipv6hdr_stru *)(ether_header + 1)) == OAL_TRUE) {
                /* 如果是DHCPV6帧，则进入WLAN_DATA_VIP_QUEUE队列缓存 */
                queue = WLAN_DATA_VIP_QUEUE;
            }
            break;

        case oal_host2net_short(ETHER_TYPE_PAE):
            /* 如果是EAPOL帧，则进入VO队列发送 */
            queue = WLAN_DATA_VIP_QUEUE;
            break;

        /* TDLS帧处理，建链保护，入高优先级TID队列 */
        case oal_host2net_short(ETHER_TYPE_TDLS):
            queue = WLAN_DATA_VIP_QUEUE;
            break;

        /* PPPOE帧处理，建链保护(发现阶段, 会话阶段)，入高优先级TID队列 */
        case oal_host2net_short(ETHER_TYPE_PPP_DISC):
        case oal_host2net_short(ETHER_TYPE_PPP_SES):
            queue = WLAN_DATA_VIP_QUEUE;
            break;

        case oal_host2net_short(ETHER_TYPE_WAI):
            queue = WLAN_DATA_VIP_QUEUE;
            break;
        default:
            queue = WLAN_NORMAL_QUEUE;
            break;
    }

    return queue;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
