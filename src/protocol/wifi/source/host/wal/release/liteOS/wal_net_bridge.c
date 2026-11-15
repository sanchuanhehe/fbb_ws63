/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2023. All rights reserved.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "wal_net_bridge.h"
#include "wal_common.h"
#include "hmac_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_NET_BRIDGE_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : wal_bridge_vap_xmit_etc
 功能描述  : 挂接到VAP对应net_device结构体下的发送函数
 输入参数  : buf: SKB结构体,其中data指针指向以太网头
             dev: net_device结构体
*****************************************************************************/
osal_u8    g_sk_pacing_shift = 8;
oal_net_dev_tx_enum  wal_bridge_vap_xmit_etc(oal_netbuf_stru *buf, oal_net_device_stru *dev)
{
    if (OAL_UNLIKELY(dev == OAL_PTR_NULL)) {
        oam_error_log0(0, OAM_SF_TX, "{wal_bridge_vap_xmit_etc::dev = OAL_PTR_NULL!}\r\n");
        oal_netbuf_free(buf);
        return OAL_NETDEV_TX_OK;
    }
    frw_host_post_data(FRW_NETBUF_W2H_DATA_FRAME, wal_util_get_vap_id(dev), buf);
    return OAL_NETDEV_TX_OK;
}

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

