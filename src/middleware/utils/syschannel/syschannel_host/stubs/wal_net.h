#ifndef __WAL_NET_H__
#define __WAL_NET_H__

#include "securec.h"
#include "soc_osal.h"
#include "osal_types.h"
#include "oam_ext_if.h"
#include "oal_netdev.h"
#include "oal_netbuf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* �궨�� */
#define ETHER_ADDR_LEN          6
#define WLAN_MAC_ADDR_LEN       6
#define NUM_2                   2
#define ether_is_multicast(_a) (*(_a) & 0x01)
#define osal_array_size(_array)             (sizeof(_array) / sizeof((_array)[0]))
static oal_net_device_stru *g_stubs_net_device[2];
/* inline �������� */
static inline oal_net_device_stru *wal_net_alloc_netdev(osal_u32 sizeof_priv, const osal_char *netdev_name,
    osal_void *set_up)
{
    unref_param(sizeof_priv);
    unref_param(set_up);
    osal_u32 size = strlen((const osal_char *)netdev_name) + 1; /* 加上'\0' */

    oal_net_device_stru *pst_net_dev = (oal_net_device_stru *)osal_kzalloc(sizeof(oal_net_device_stru), 0);

    /* copy name to netdeivce */
    if (memcpy_s(pst_net_dev->name, OAL_IF_NAME_SIZE, netdev_name, size) != EOK) {
        osal_kfree(pst_net_dev);
        return OSAL_NULL;
    }
    return pst_net_dev;
}

static inline osal_void wal_net_free_netdev(oal_net_device_stru *netdev)
{
    if (netdev == NULL) {
        return;
    }

    if (netdev->priv != NULL) {
        osal_kfree((void *)netdev->priv);
    }

    osal_kfree((void *)netdev);
}

static inline osal_s32 oal_net_register_netdev(oal_net_device_stru *p_net_device)
{
    osal_u32 u_i;
    for (u_i = 0; u_i < NUM_2; u_i++) {
        if (g_stubs_net_device[u_i] == NULL) {
            g_stubs_net_device[u_i] = p_net_device;
            return 0;
        }
    }
    return -1;
}

static inline osal_void oal_net_unregister_netdev(oal_net_device_stru *p_net_device)
{
    osal_u32 u_i;

    for (u_i = 0; u_i < NUM_2; u_i++) {
        if (g_stubs_net_device[u_i] == p_net_device) {
            g_stubs_net_device[u_i] = NULL;
            wal_net_free_netdev(p_net_device);
            return;
        }
    }
}

/* �������� */
osal_s32 netdev_register(osal_void);
osal_void netdev_unregister(osal_void);
osal_s32 wal_rx_data_proc(oal_netbuf_stru *netbuf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __WAL_NET_H__ */
