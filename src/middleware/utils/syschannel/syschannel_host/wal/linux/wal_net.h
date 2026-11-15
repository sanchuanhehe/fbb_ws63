#ifndef __WAL_NET_H__
#define __WAL_NET_H__

#include <linux/netdevice.h>
#include <linux/version.h>
#include <linux/nl80211.h>
#include <linux/etherdevice.h>

#include "securec.h"
#include "soc_osal.h"
#include "osal_types.h"
#include "oal_netdev.h"
#include "oal_netbuf.h"
#include "hcc_cfg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* �궨�� */
#define ETHER_ADDR_LEN          6
#define WLAN_MAC_ADDR_LEN       6
#define ether_is_multicast(_a) (*(_a) & 0x01)
#define osal_array_size(_array)             (sizeof(_array) / sizeof((_array)[0]))

/* inline �������� */
static inline oal_net_device_stru *wal_net_alloc_netdev(osal_u32 sizeof_priv, const osal_char *netdev_name,
    osal_void *set_up)
{
    oal_net_device_stru *tmp_netdev = OSAL_NULL;

    if ((netdev_name == OSAL_NULL) || (set_up == OSAL_NULL)) {
        return OSAL_NULL;
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0))
    tmp_netdev = alloc_netdev(sizeof_priv, netdev_name, NET_NAME_UNKNOWN, set_up);
#else
    tmp_netdev = alloc_netdev(sizeof_priv, netdev_name, set_up);
#endif
    if (tmp_netdev == OSAL_NULL) {
        osal_printk("oal_net_alloc_netdev:: netdev is NULL");
    }
    return tmp_netdev;
}

static inline osal_void wal_net_free_netdev(oal_net_device_stru *netdev)
{
    if (netdev == OSAL_NULL) {
        return;
    }
    free_netdev(netdev);
}

/* �������� */
osal_s32 netdev_register(osal_void);
osal_void netdev_unregister(osal_void);
osal_u32 syschannel_host_alloc_netbuf(hcc_queue_type queue_id, osal_u32 len, osal_u8 **buf, osal_u8 **user_param);
osal_void syschannel_host_netbuf_free(hcc_queue_type queue_id, osal_u8 *buf, osal_u8 *user_param);
osal_u32 syschannel_host_rx_data_process(hcc_queue_type queue_id, osal_u8 stype,
    osal_u8 *buf, osal_u32 len, osal_u8 *user_param);
osal_u32 syschannel_host_sync_mac_addr(osal_u8 *buf, osal_u32 len);
osal_u32 syschannel_host_sync_ip_addr(osal_u8 *buf, osal_u32 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __WAL_NET_H__ */
