#ifndef __OAL_NETDEV_H__
#define __OAL_NETDEV_H__

#include "osal_types.h"
#include "oal_netbuf.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* 新增linux结构体和宏 */
#define OAL_SIOCIWFIRSTPRIV         0x8BE0
#define OAL_IW_PRIV_TYPE_CHAR       0x2000       /* Char as character */
#define OAL_IFF_RUNNING             0x40
#define OAL_IF_NAME_SIZE            16

#define oal_netdevice_destructor(_pst_dev)          ((_pst_dev)->destructor)
#define oal_netdevice_qdisc(_pst_dev, pst_val)      ((_pst_dev)->qdisc = (pst_val))
#define oal_netdevice_ifalias(_pst_dev)             ((_pst_dev)->ifalias)
#define oal_netdevice_watchdog_timeo(_pst_dev)      ((_pst_dev)->watchdog_timeo)
#define oal_netdevice_flags(_pst_dev)               ((_pst_dev)->flags)
#define oal_dev_put(_pst_dev)
#define oal_net_dev_priv(_pst_dev)                  ((_pst_dev)->ml_priv)
#define oal_net_dev_wireless_dev(_pst_dev)          ((_pst_dev)->ieee80211_ptr)

typedef enum {
    OAL_NETDEV_TX_OK = 0x00,
    OAL_NETDEV_TX_BUSY = 0x10,
    OAL_NETDEV_TX_LOCKED = 0x20,
} oal_net_dev_tx_enum;

typedef enum nl80211_channel_type {
    NL80211_CHAN_NO_HT,
    NL80211_CHAN_HT20,
    NL80211_CHAN_HT40MINUS,
    NL80211_CHAN_HT40PLUS
} oal_nl80211_channel_type;
typedef osal_u8 oal_nl80211_channel_type_uint8;

enum nl80211_iftype {
    NL80211_IFTYPE_UNSPECIFIED,
    NL80211_IFTYPE_ADHOC,
    NL80211_IFTYPE_STATION,
    NL80211_IFTYPE_AP,
    NL80211_IFTYPE_AP_VLAN,
    NL80211_IFTYPE_WDS,
    NL80211_IFTYPE_MONITOR,
    NL80211_IFTYPE_MESH_POINT,
    NL80211_IFTYPE_P2P_CLIENT,
    NL80211_IFTYPE_P2P_GO,
    NL80211_IFTYPE_P2P_DEVICE,
    /* keep last */
    NUM_NL80211_IFTYPES,
    NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};
typedef osal_u8 nl80211_iftype_uint8;

#define OAL_INIT_NET                init_net

typedef struct {
    osal_u16 cmd;   /* Wireless Extension command */
    osal_u16 flags; /* More to come ;-) */
} oal_iw_request_info_stru;

typedef struct {
    osal_void *pointer; /* Pointer to the data  (in user space) */
    osal_u16 length;    /* number of fields or size in bytes */
    osal_u16 flags;     /* Optional params */
} oal_iw_point_stru;

typedef struct {
    osal_s32 value;   /* The value of the parameter itself */
    osal_u8 fixed;    /* Hardware should not use auto select */
    osal_u8 disabled; /* Disable the feature */
    osal_u16 flags;   /* Various specifc flags (if any) */
} oal_iw_param_stru;

typedef struct {
    osal_s32 m;    /* Mantissa */
    osal_s16 e;    /* Exponent */
    osal_u8 i;     /* List index (when in range struct) */
    osal_u8 flags; /* Flags (fixed/auto) */
} oal_iw_freq_stru;

typedef struct {
    osal_u8 qual;    /* link quality (%retries, SNR, %missed beacons or better...) */
    osal_u8 level;   /* signal level (dBm) */
    osal_u8 noise;   /* noise level (dBm) */
    osal_u8 updated; /* Flags to know if updated */
} oal_iw_quality_stru;

typedef struct {
    osal_u16 sa_family;  /* address family, AF_xxx   */
    osal_u8 sa_data[14]; /* 14 bytes of protocol address */
} oal_sockaddr_stru;

typedef union {
    /* Config - generic */
    osal_char             name[OAL_IF_NAME_SIZE];
    oal_iw_point_stru   essid;      /* Extended network name */
    oal_iw_param_stru   nwid;       /* network id (or domain - the cell) */
    oal_iw_freq_stru    freq;       /* frequency or channel : * 0-1000 = channel * > 1000 = frequency in Hz */
    oal_iw_param_stru   sens;       /* signal level threshold */
    oal_iw_param_stru   bitrate;    /* default bit rate */
    oal_iw_param_stru   txpower;    /* default transmit power */
    oal_iw_param_stru   rts;        /* RTS threshold threshold */
    oal_iw_param_stru   frag;       /* Fragmentation threshold */
    osal_u32              mode;       /* Operation mode */
    oal_iw_param_stru   retry;      /* Retry limits & lifetime */
    oal_iw_point_stru   encoding;   /* Encoding stuff : tokens */
    oal_iw_param_stru   power;      /* PM duration/timeout */
    oal_iw_quality_stru qual;       /* Quality part of statistics */
    oal_sockaddr_stru   ap_addr;    /* Access point address */
    oal_sockaddr_stru   addr;       /* Destination address (hw/mac) */
    oal_iw_param_stru   param;      /* Other small parameters */
    oal_iw_point_stru   data;       /* Other large parameters */
} oal_iwreq_data_union;

struct oal_net_device;
typedef osal_u32 (*oal_iw_handler)(struct oal_net_device* dev, oal_iw_request_info_stru* info,
    oal_iwreq_data_union* wrqu, osal_s8* extra);

/* 私有IOCTL接口信息 */
typedef struct {
    osal_u32       cmd;                       /* ioctl命令号 */
    osal_u16       set_args;                  /* 类型和参数字符个数 */
    osal_u16       get_args;                  /* 类型和参数字符个数 */
    osal_char      name[OAL_IF_NAME_SIZE];    /* 私有命令名 */
} oal_iw_priv_args_stru;

typedef struct {
    osal_u32 fake;
} oal_iw_statistics_stru;

typedef struct {
    osal_u32 handle;
} oal_qdisc_stru;

typedef struct {
    const oal_iw_handler*      standard;
    osal_u16                   num_standard;
#ifdef CONFIG_WEXT_PRIV
    osal_u16                   num_private;
    osal_u16                   num_private_args;
    const oal_iw_handler        *private;
    const oal_iw_priv_args_stru* private_args;
#endif
    oal_iw_statistics_stru*     (*get_wireless_stats)(struct oal_net_device* dev);
} oal_iw_handler_def_stru;

typedef struct _oal_hisi_eapol_stru {
    osal_bool register_code;
    osal_u8 eapol_cnt;     /* 当前链表中eapol数量 */
    osal_u16 enqueue_time; /* 记录的eapol帧最早入队时间 单位s 用于超时老化 */
    osal_void *context;
    osal_void (*notify_callback)(osal_void *name, osal_void *context);
    oal_netbuf_head_stru eapol_skb_head;
} oal_hisi_eapol_stru;

/* linux 结构体 */
typedef struct {
    osal_u32   rx_packets;     /* total packets received   */
    osal_u32   tx_packets;     /* total packets transmitted    */
    osal_u32   rx_bytes;       /* total bytes received     */
    osal_u32   tx_bytes;       /* total bytes transmitted  */
    osal_u32   rx_errors;      /* bad packets received     */
    osal_u32   tx_errors;      /* packet transmit problems */
    osal_u32   rx_dropped;     /* no space in linux buffers    */
    osal_u32   tx_dropped;     /* no space available in linux  */
    osal_u32   multicast;      /* multicast packets received   */
    osal_u32   collisions;

    /* detailed rx_errors: */
    osal_u32   rx_length_errors;
    osal_u32   rx_over_errors;     /* receiver ring buff overflow  */
    osal_u32   rx_crc_errors;      /* recved pkt with crc error    */
    osal_u32   rx_frame_errors;    /* recv'd frame alignment error */
    osal_u32   rx_fifo_errors;     /* recv'r fifo overrun      */
    osal_u32   rx_missed_errors;   /* receiver missed packet   */

    /* detailed tx_errors */
    osal_u32   tx_aborted_errors;
    osal_u32   tx_carrier_errors;
    osal_u32   tx_fifo_errors;
    osal_u32   tx_heartbeat_errors;
    osal_u32   tx_window_errors;

    /* for cslip etc */
    osal_u32   rx_compressed;
    osal_u32   tx_compressed;
} oal_net_device_stats_stru;

#define ETHER_ADDR_LEN          6
typedef struct netif oal_lwip_netif;
typedef struct oal_net_device {
    osal_char                      name[OAL_IF_NAME_SIZE];
    osal_void*                     ml_priv;
    struct oal_net_device_ops*   netdev_ops;
    osal_u32                      last_rx;
    osal_u32                      flags;
#ifdef CONFIG_WIRELESS_EXT
    oal_iw_handler_def_stru*     wireless_handlers;
#endif
    osal_u8                       dev_addr[ETHER_ADDR_LEN];
    osal_u8                       addr_idx;
    osal_u8                       resv;
    osal_s32                      tx_queue_len;
    osal_u16                      hard_header_len;
    osal_u16                      type;
    osal_u16                      needed_headroom;
    osal_u16                      needed_tailroom;
    struct oal_net_device*       master;
    oal_qdisc_stru*              qdisc;
    osal_u8*                       ifalias;
    osal_u8                       addr_len;
    osal_u8                       resv2[3];  /* 3: bytes保留字段 */
    osal_s32                      watchdog_timeo;
    oal_net_device_stats_stru   stats;
    osal_u32                      mtu;
    osal_void                     (*destructor)(struct oal_net_device*);
    osal_void*                     priv;
    osal_u32                      num_tx_queues;
    oal_hisi_eapol_stru         hisi_eapol;   /* EAPOL报文收发数据结构 */
    oal_lwip_netif*              lwip_netif; /* LWIP协议栈数据结构 */
#ifdef _PRE_WLAN_FEATURE_VLWIP
    oal_lwip_netif*              vlwip_netif;
#endif
} oal_net_device_stru;

typedef struct oal_net_notify {
    osal_u32 ip_addr;
    osal_u32 notify_type;
} oal_net_notify_stru;

typedef struct {
    unsigned int fake;
    unsigned char *ifr_data;
} oal_ifreq_stru;

typedef struct {
    osal_s32 sk_wmem_queued;
} oal_sock_stru;

typedef struct oal_net_device_ops {
    osal_s32                      (*ndo_init)(oal_net_device_stru*);
    osal_s32                      (*ndo_open)(struct oal_net_device*);
    osal_s32                      (*ndo_stop)(struct oal_net_device*);
    oal_net_dev_tx_enum         (*ndo_start_xmit) (oal_netbuf_stru*, struct oal_net_device*);
    osal_void                     (*ndo_set_multicast_list)(struct oal_net_device*);
    oal_net_device_stats_stru*  (*ndo_get_stats)(oal_net_device_stru*);
    osal_s32                      (*ndo_do_ioctl)(struct oal_net_device*, oal_ifreq_stru*, osal_s32);
    osal_s32                      (*ndo_change_mtu)(struct oal_net_device*, osal_s32);
    osal_s32                      (*ndo_set_mac_address)(struct oal_net_device*, osal_void*);
    osal_s16                      (*ndo_select_queue)(oal_net_device_stru* dev, oal_netbuf_stru*);
    osal_s32                      (*ndo_netif_notify)(oal_net_device_stru*, oal_net_notify_stru*);
} oal_net_device_ops_stru;

oal_net_device_stru* oal_dev_get_by_name(const osal_char* pc_name);
osal_void oal_ether_setup(oal_net_device_stru *net_device);
osal_void oal_netif_stop_queue(oal_net_device_stru* netdev);
osal_void oal_netif_wake_queue(oal_net_device_stru* netdev);

struct oal_ether_header {
    osal_u8 ether_dhost[ETHER_ADDR_LEN];
    osal_u8 ether_shost[ETHER_ADDR_LEN];
    osal_u16 ether_type;
};
typedef struct oal_ether_header oal_ether_header_stru;
static inline osal_u16 oal_eth_type_trans(oal_netbuf_stru *pst_netbuf, oal_net_device_stru *device)
{
    pst_netbuf->dev = device;
    oal_netbuf_pull(pst_netbuf, sizeof(oal_ether_header_stru));

    return 0;
}

static inline osal_s32 oal_netif_rx_ni(oal_netbuf_stru *pst_netbuf)
{
    oal_netbuf_free(pst_netbuf);

    return 0;
}

static inline struct sk_buff *oal_netdev_alloc_skb(oal_net_device_stru *netdev, unsigned int length)
{
    netdev = netdev;
    return oal_netbuf_alloc_skb(length);
}

static inline osal_u32 oal_netif_running_func(oal_net_device_stru *netdev)
{
    return netdev->flags;
}

#define oal_netif_running(_pst_net_dev) oal_netif_running_func(_pst_net_dev)
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif