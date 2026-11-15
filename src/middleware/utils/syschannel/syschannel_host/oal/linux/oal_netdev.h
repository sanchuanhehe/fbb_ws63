#ifndef __OAL_NETDEV_H__
#define __OAL_NETDEV_H__

#include <net/iw_handler.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include "osal_types.h"
#include "oal_netbuf.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* 新增linux结构体和宏 */
#define OAL_SIOCIWFIRSTPRIV         SIOCIWFIRSTPRIV
#define OAL_IW_PRIV_TYPE_CHAR       IW_PRIV_TYPE_CHAR       /* Char as character */
#define OAL_IFF_RUNNING             IFF_RUNNING
#define OAL_IF_NAME_SIZE            16

#define oal_netdevice_destructor(_pst_dev)          ((_pst_dev)->destructor)
#define oal_netdevice_qdisc(_pst_dev, pst_val)      ((_pst_dev)->qdisc = (pst_val))
#define oal_netdevice_ifalias(_pst_dev)             ((_pst_dev)->ifalias)
#define oal_netdevice_watchdog_timeo(_pst_dev)      ((_pst_dev)->watchdog_timeo)
#define oal_netdevice_flags(_pst_dev)               ((_pst_dev)->flags)
#define oal_netif_running(net_dev)                  netif_running(net_dev)
#define oal_dev_put(_pst_dev)                       dev_put(_pst_dev)
#define oal_net_dev_priv(_pst_dev)                  ((_pst_dev)->ml_priv)
#define oal_net_dev_wireless_dev(_pst_dev)          ((_pst_dev)->ieee80211_ptr)

#define OAL_NETDEV_TX_OK            NETDEV_TX_OK
#define OAL_NETDEV_TX_BUSY          NETDEV_TX_BUSY
#define OAL_NETDEV_TX_LOCKED        NETDEV_TX_LOCKED

#define OAL_INIT_NET                init_net
/* linux 结构体 */
typedef struct net_device           oal_net_device_stru;
typedef struct net_device_ops       oal_net_device_ops_stru;
typedef struct net_device_stats     oal_net_device_stats_stru;
typedef struct ifreq                oal_ifreq_stru;
typedef struct iw_handler_def       oal_iw_handler_def_stru;
typedef struct iw_priv_args         oal_iw_priv_args_stru;
typedef iw_handler                  oal_iw_handler;
typedef netdev_tx_t                 oal_net_dev_tx_enum;
typedef struct sock                 oal_sock_stru;

typedef struct iw_request_info      oal_iw_request_info_stru;
typedef struct iw_point             oal_iw_point_stru;
typedef struct sockaddr             oal_sockaddr_stru;

static inline osal_u32 oal_net_register_netdev(oal_net_device_stru* netdev)
{
    return register_netdev(netdev);
}

static inline osal_u16 oal_eth_type_trans(oal_netbuf_stru *netbuf, oal_net_device_stru *device)
{
    return eth_type_trans(netbuf, device);
}

static inline osal_s32 oal_netif_rx_ni(oal_netbuf_stru *netbuf)
{
    return netif_rx_ni(netbuf);
}

oal_net_device_stru* oal_dev_get_by_name(const osal_char* pc_name);
osal_void oal_net_unregister_netdev(oal_net_device_stru* netdev);
osal_void oal_ether_setup(oal_net_device_stru *net_device);
osal_void oal_netif_stop_queue(oal_net_device_stru* netdev);
osal_void oal_netif_wake_queue(oal_net_device_stru* netdev);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif