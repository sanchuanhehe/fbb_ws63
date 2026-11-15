/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: wal wpa ioctl file.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_net.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_ioctl.h"
#include "wal_event_msg.h"
#include "hmac_ext_if.h"
#include "oal_ext_if.h"
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#include "wal_cfg80211.h"
#include "wal_cfg80211_apt.h"
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/netifapi.h"
#endif // #ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
#include "msg_rekey_offload_rom.h"
#endif
#endif
#include "mac_addr.h"
#include "wal_wpa_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_WPA_IOCTL_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
const hwal_ioctl_handler g_ast_hwal_ioctl_handlers[] = {
    (hwal_ioctl_handler) uapi_ioctl_set_ap,             /* EXT_IOCTL_SET_AP     */
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    (hwal_ioctl_handler) uapi_ioctl_new_key,            /* EXT_IOCTL_NEW_KEY    */
    (hwal_ioctl_handler) uapi_ioctl_del_key,            /* EXT_IOCTL_DEL_KEY    */
    (hwal_ioctl_handler) uapi_ioctl_set_key,            /* EXT_IOCTL_SET_KEY    */
    (hwal_ioctl_handler) uapi_ioctl_send_mlme,          /* EXT_IOCTL_SEND_MLME  */
#else
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_send_eapol,         /* EXT_IOCTL_SEND_EAPOL */
    (hwal_ioctl_handler) uapi_ioctl_receive_eapol,      /* EXT_IOCTL_RECEIVE_EAPOL */
    (hwal_ioctl_handler) uapi_ioctl_enable_eapol,       /* EXT_IOCTL_ENALBE_EAPOL */
    (hwal_ioctl_handler) uapi_ioctl_disable_eapol,      /* EXT_IOCTL_DISABLE_EAPOL */
    (hwal_ioctl_handler) uapi_ioctl_get_addr,           /* HIIS_IOCTL_GET_ADDR */
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    (hwal_ioctl_handler) uapi_ioctl_set_mode,           /* EXT_IOCTL_SET_MODE */
    (hwal_ioctl_handler) uapi_ioctl_get_mode,           /* EXT_IOCTL_SET_MODE */
    (hwal_ioctl_handler) uapi_ioctl_del_beacon,         /* EXT_IOCTL_DEL_BEACON */
    (hwal_ioctl_handler) uapi_ioctl_get_hw_feature,     /* HIIS_IOCTL_GET_HW_FEATURE */
    (hwal_ioctl_handler) uapi_ioctl_scan,               /* EXT_IOCTL_SCAN */
    (hwal_ioctl_handler) uapi_ioctl_disconnect,         /* EXT_IOCTL_DISCONNET */
    (hwal_ioctl_handler) uapi_ioctl_assoc,              /* EXT_IOCTL_ASSOC */
#else
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_set_netdev,         /* EXT_IOCTL_SET_NETDEV */
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    (hwal_ioctl_handler) uapi_ioctl_change_beacon,      /* EXT_IOCTL_CHANGE_BEACON */
#else
    OSAL_NULL,
#endif
#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
    (hwal_ioctl_handler) uapi_ioctl_set_rekey_info,     /* EXT_IOCTL_SET_REKEY_INFO */
#else
    OSAL_NULL,
#endif
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    (hwal_ioctl_handler) uapi_ioctl_sta_remove,         /* EXT_IOCTL_STA_REMOVE */
#else
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_send_action,        /* EXT_IOCTL_SEND_ACTION */
#ifdef _PRE_WLAN_FEATURE_MESH
    (hwal_ioctl_handler) uapi_ioctl_set_mesh_user,      /* EXT_IOCTL_SET_MESH_USER */
    (hwal_ioctl_handler) uapi_ioctl_set_mesh_gtk,       /* EXT_IOCTL_SET_MESH_GTK */
    (hwal_ioctl_handler) uapi_ioctl_set_accept_peer,    /* EXT_IOCTL_EN_ACCEPT_PEER */
    (hwal_ioctl_handler) uapi_ioctl_set_accept_sta,     /* EXT_IOCTL_EN_ACCEPT_STA */
#else
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    (hwal_ioctl_handler) uapi_ioctl_add_if,             /* EXT_IOCTL_ADD_IF */
    (hwal_ioctl_handler) uapi_ioctl_probe_req_report,   /* EXT_IOCTL_PROBE_REQUEST_REPORT */
    (hwal_ioctl_handler) uapi_ioctl_remain_on_channel,  /* EXT_IOCTL_REMAIN_ON_CHANNEL */
    (hwal_ioctl_handler) uapi_ioctl_cancel_remain_on_channel,    /* EXT_IOCTL_CANCEL_REMAIN_ON_CHANNEL */
    (hwal_ioctl_handler) uapi_ioctl_set_p2p_noa,        /* EXT_IOCTL_SET_P2P_NOA */
    (hwal_ioctl_handler) uapi_ioctl_set_p2p_powersave,  /* EXT_IOCTL_SET_P2P_POWERSAVE */
#else
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_set_ap_wps_p2p_ie,  /* EXT_IOCTL_SET_AP_WPS_P2P_IE */
#ifdef _PRE_WLAN_FEATURE_P2P
    (hwal_ioctl_handler) uapi_ioctl_remove_if,          /* EXT_IOCTL_REMOVE_IF */
    (hwal_ioctl_handler) uapi_ioctl_get_p2p_addr,       /* EXT_IOCTL_GET_P2P_MAC_ADDR */
#else
    OSAL_NULL,
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_get_drv_flags,       /* EXT_IOCTL_GET_DRIVER_FLAGS */
    (hwal_ioctl_handler) uapi_ioctl_set_user_app_ie,     /* EXT_IOCTL_SET_USR_APP_IE */
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    (hwal_ioctl_handler) uapi_ioctl_set_delay_report,    /* EXT_IOCTL_DELAY_REPORT */
#else
    OSAL_NULL,
#endif
#ifdef _PRE_WLAN_FEATURE_WPA3
    (hwal_ioctl_handler) uapi_ioctl_send_ext_auth_status, /* EXT_IOCTL_SEND_EXT_AUTH_STATUS */
#else
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_add_pmkid, /* EXT_IOCTL_ADD_PMKID */
    (hwal_ioctl_handler) uapi_ioctl_remove_pmkid, /* EXT_IOCTL_DEL_PMKID */
    (hwal_ioctl_handler) uapi_ioctl_flush_pmkid, /* EXT_IOCTL_FLUSH_PMKID */
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
    (hwal_ioctl_handler) uapi_ioctl_get_survey,         /* EXT_IOCTL_GET_SURVEY */
#else
    OSAL_NULL,
#endif
#if defined(_PRE_WLAN_FEATURE_ROAM) && defined(_PRE_WLAN_FEATURE_11R)
    (hwal_ioctl_handler) uapi_ioctl_update_ft_ies,  /* UAPI_IOCTL_UPDATE_FT_IES */
#else
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_ioctl_set_csi_config,      /* EXT_IOCTL_SET_CSI_CONFIG */
    (hwal_ioctl_handler) uapi_ioctl_csi_switch,          /* EXT_IOCTL_CSI_SWITCH */
#ifdef _PRE_WLAN_FEATURE_WPA3
    (hwal_ioctl_handler) uapi_ioctl_update_dh_ie, /* EXT_IOCTL_UPDATE_DH_IE */
#else
    OSAL_NULL,
#endif
    (hwal_ioctl_handler) uapi_wifi_app_service, /* EXT_IOCTL_WIFI_APP_SERVICE */
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : set key数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_set_key(const td_char *puc_ifname, td_void *buf)
{
    td_u8                    key_index;
    td_bool                  unicast = OSAL_TRUE;
    td_bool                  multicast = OSAL_FALSE;
    ext_key_ext_stru       *key_ext = OSAL_NULL;
    oal_net_device_stru     *netdev = OSAL_NULL;

    netdev   = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_set_key:netdev is NULL.");
        return -EXT_EFAIL;
    }

    key_ext      = (ext_key_ext_stru *)buf;
    key_index     = (td_u8)key_ext->key_idx;

    if (key_ext->def == EXT_TRUE) {
        unicast = OSAL_TRUE;
        multicast = OSAL_TRUE;
    }

    if (key_ext->defmgmt == EXT_TRUE) {
        multicast = OSAL_TRUE;
    }

    if (key_ext->default_types == EXT_KEY_DEFAULT_TYPE_UNICAST) {
        unicast = OSAL_TRUE;
    } else if (key_ext->default_types == EXT_KEY_DEFAULT_TYPE_MULTICAST) {
        multicast = OSAL_TRUE;
    }

    return (td_s32)wal_cfg80211_set_default_key(netdev, key_index, unicast, multicast);
}

/*****************************************************************************
 功能描述  : set key数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_new_key(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru     *netdev = OSAL_NULL;
    ext_key_ext_stru       *key_ext = OSAL_NULL;
    oal_key_params_stru      params = {0};
    cfg80211_add_key_info_stru cfg80211_add_key_info;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_new_key:netdev is NULL.");
        return -EXT_EFAIL;
    }

    key_ext  = (ext_key_ext_stru *)buf;

    cfg80211_add_key_info.key_index = (td_u8)key_ext->key_idx;
    cfg80211_add_key_info.pairwise  = (key_ext->l_type == EXT_KEYTYPE_PAIRWISE);

    params.key       = (td_u8 *)(key_ext->key);
    params.key_len   = (td_s32)key_ext->key_len;
    params.seq_len   = (td_s32)key_ext->seq_len;
    params.seq       = key_ext->seq;
    params.cipher    = key_ext->cipher;

    return (td_s32)wal_cfg80211_add_key(netdev, &cfg80211_add_key_info, key_ext->addr, &params);
}

/*****************************************************************************
 功能描述  : del key数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_del_key(const td_char *puc_ifname, td_void *buf)
{
    td_bool                  pairwise;
    oal_net_device_stru     *netdev = OSAL_NULL;
    ext_key_ext_stru       *key_ext = OSAL_NULL;

    netdev   = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_del_key:netdev is NULL.");
        return -EXT_EFAIL;
    }

    key_ext  = (ext_key_ext_stru *)buf;
    pairwise  = (key_ext->l_type == EXT_KEYTYPE_PAIRWISE);

    return (td_s32)wal_cfg80211_remove_key(netdev, (td_u8)key_ext->key_idx, pairwise, key_ext->addr);
}
#endif

OAL_STATIC td_void hwal_init_ap_setting(const ext_ap_settings_stru *apsettings, oal_ap_settings_stru *oal_apsettings)
{
    if ((apsettings == OSAL_NULL) || (oal_apsettings == OSAL_NULL)) {
        oam_error_log0(0, 0, "hwal_init_ap_setting :ptr is NULL.");
        return;
    }
    oal_apsettings->ssid_len        = apsettings->ssid_len;
    oal_apsettings->beacon_interval = apsettings->l_beacon_interval;
    oal_apsettings->dtim_period     = apsettings->l_dtim_period;
    oal_apsettings->hidden_ssid     = (enum nl80211_hidden_ssid)(apsettings->hidden_ssid);
    oal_apsettings->beacon.head_len = apsettings->beacon_data.head_len;
    oal_apsettings->beacon.tail_len = apsettings->beacon_data.tail_len;

    oal_apsettings->ssid          = apsettings->ssid;
    oal_apsettings->beacon.head   = apsettings->beacon_data.head;
    oal_apsettings->beacon.tail   = apsettings->beacon_data.tail;
    oal_apsettings->auth_type     = (enum nl80211_auth_type)(apsettings->auth_type);
    oal_apsettings->sae_pwe = apsettings->sae_pwe;
}

/*****************************************************************************
 功能描述  : set ap数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_set_ap(const td_char *puc_ifname, td_void *buf)
{
    oal_ap_settings_stru oal_apsettings = {0};

    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_set_ap :netdev is NULL.");
        return -EXT_EFAIL;
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    if (netdev->lwip_netif == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_set_ap :lwip_netif is NULL.");
        return -EXT_EFAIL;
    }
#endif

    ext_ap_settings_stru *apsettings = (ext_ap_settings_stru *)buf;

    hwal_init_ap_setting(apsettings, &oal_apsettings);

    if (netdev->ieee80211_ptr == OSAL_NULL) {
        netdev->ieee80211_ptr = (oal_wireless_dev*)malloc(sizeof(struct wireless_dev));
        if (netdev->ieee80211_ptr == OSAL_NULL) {
            oam_error_log0(0, 0, "ieee80211_ptr parameter NULL.");
            return -EXT_EFAIL;
        }
        /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
        memset_s(netdev->ieee80211_ptr, sizeof(struct wireless_dev), 0, sizeof(struct wireless_dev));
    }

    if (netdev->ieee80211_ptr->preset_chandef.chan == OSAL_NULL) {
        netdev->ieee80211_ptr->preset_chandef.chan = (oal_ieee80211_channel*)malloc(sizeof(oal_ieee80211_channel));
        if (netdev->ieee80211_ptr->preset_chandef.chan == OSAL_NULL) {
            free(netdev->ieee80211_ptr);
            netdev->ieee80211_ptr = OSAL_NULL;

            oam_error_log0(0, 0, "chan parameter NULL.");
            return -EXT_EFAIL;
        }
        /* 安全编程规则6.6例外(3)从堆中分配内存后，赋予初值 */
        memset_s(netdev->ieee80211_ptr->preset_chandef.chan, sizeof(oal_ieee80211_channel), 0,
            sizeof(oal_ieee80211_channel));
    }

    netdev->ieee80211_ptr->preset_chandef.width = (enum nl80211_channel_type)apsettings->freq_params.l_bandwidth;
    netdev->ieee80211_ptr->preset_chandef.center_freq1   = apsettings->freq_params.l_center_freq1;
    netdev->ieee80211_ptr->preset_chandef.chan->hw_value = (td_u16)apsettings->freq_params.l_channel;
    netdev->ieee80211_ptr->preset_chandef.chan->band     = OAL_IEEE80211_BAND_2GHZ;
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    td_s32 ret = (td_s32)wal_cfg80211_start_ap(netdev, &oal_apsettings);
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    (td_void)netifapi_netif_set_link_up(netdev->lwip_netif);
#endif
    return ret;
#else
    unref_param_prv(oal_apsettings);
    return OAL_SUCC;
#endif
}

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 修改beacon帧参数
*****************************************************************************/
td_s32 uapi_ioctl_change_beacon(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru     *netdev = OSAL_NULL;
    oal_beacon_data_stru     beacon = {0};
    ext_ap_settings_stru   *apsettings = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_change_beacon:netdev is NULL.");
        return -EXT_EFAIL;
    }
    apsettings       = (ext_ap_settings_stru *)buf;

    /* 获取修改beacon帧参数的结构体 */
    beacon.head       = apsettings->beacon_data.head;
    beacon.tail       = apsettings->beacon_data.tail;
    beacon.head_len   = apsettings->beacon_data.head_len;
    beacon.tail_len   = apsettings->beacon_data.tail_len;

    return (td_s32)wal_cfg80211_change_beacon(netdev, &beacon);
}
/*****************************************************************************
 功能描述  : send mlme数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_send_mlme(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru         *netdev = OSAL_NULL;
    ext_mlme_data_stru         *mlme_data = OSAL_NULL;
    oal_ieee80211_channel        chan = {0};

    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_ioctl_send_mlme:netdev is NULL.");
        return -EXT_EFAIL;
    }

    mlme_data = (ext_mlme_data_stru *)buf;
    chan.center_freq = (td_u16)mlme_data->freq;
#ifdef _PRE_WLAN_FEATURE_P2P
    /* 仅P2P需要创建cokie并上报发送状态至WPA */

    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_send_mlme}");
    return (td_s32)wal_cfg80211_p2p_mgmt_tx(netdev->ieee80211_ptr, &chan, mlme_data,
        mlme_data->pull_send_action_cookie);
#else
    return (td_s32)wal_cfg80211_mgmt_tx(netdev, &chan, mlme_data);
#endif
}
#endif

/*****************************************************************************
 功能描述  : 发送EAPOL报文
*****************************************************************************/
td_s32 uapi_ioctl_send_eapol(const td_char *puc_ifname, td_void *buf)
{
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    ext_tx_eapol_stru     *tx_eapol = OSAL_NULL;
    oal_net_device_stru    *netdev = OSAL_NULL;
    oal_netbuf_stru        *netbuf = OSAL_NULL;

    netdev      = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_send_eapol:netdev is NULL.");
        return -EXT_EFAIL;
    }
    tx_eapol    = (ext_tx_eapol_stru *)buf;
    /* 增加EAPOL帧长度判断 */
    if ((tx_eapol->len > WLAN_LARGE_PAYLOAD_SIZE) || (tx_eapol->len < ETHER_HDR_LEN)) {
        oam_error_log1(0, 0, "uapi_ioctl_send_eapol length invalid: %d.", tx_eapol->len);
        return OAL_FAIL;
    }
    /* 申请SKB内存内存发送 */
    netbuf      = hwal_lwip_skb_alloc(netdev, (td_u16)tx_eapol->len);
    if ((netbuf == OSAL_NULL) || (netbuf->data == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_ioctl_send_eapol skb_alloc NULL.");
        return OAL_FAIL;
    }

    oal_netbuf_put(netbuf, tx_eapol->len);
    if (tx_eapol->buf != OSAL_NULL) {
        if (memcpy_s(oal_netbuf_data(netbuf), tx_eapol->len,
            tx_eapol->buf, tx_eapol->len) != EOK) {
            oal_netbuf_free(netbuf);
            oam_error_log0(0, 0, "{uapi_ioctl_send_eapol::mem safe function err!}");
            return OAL_FAIL;
        }
    }

    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_send_eapol}", wal_util_get_vap_id(netdev));
    frw_host_post_data(FRW_NETBUF_W2H_DATA_FRAME, wal_util_get_vap_id(netdev), netbuf);
    return OAL_NETDEV_TX_OK;
#else
    unref_param_prv(puc_ifname);
    unref_param_prv(buf);
    return OAL_SUCC;
#endif
}

/*****************************************************************************
 功能描述  : 处理接收EAPOL报文
*****************************************************************************/
td_s32 uapi_ioctl_receive_eapol(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru    *netdev = OSAL_NULL;
    oal_netbuf_stru        *skb_buf = OSAL_NULL;
    ext_rx_eapol_stru     *rx_eapol = OSAL_NULL;

    rx_eapol    = (ext_rx_eapol_stru *)buf;
    netdev      = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_receive_eapol:netdev is NULL.\r\n");
        return OAL_FAIL;
    }

    if (oal_netbuf_list_empty(&netdev->ext_eapol.eapol_skb_head) == OSAL_TRUE) {
        /* 此处hostapd在取链表数据时，会一直取到链表为空，所以每次都会打印，此时为正常打印 */
        /* 所以设置为info */
        oam_info_log0(0, 0, "uapi_ioctl_receive_eapol eapol pkt Q empty.\r\n");
        return OAL_FAIL;
    }

    skb_buf     = oal_netbuf_delist(&netdev->ext_eapol.eapol_skb_head);
    if (skb_buf == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_receive_eapol:: oal_netbuf_delist is NULL.\r\n");
        return OAL_FAIL;
    }

    if (skb_buf->len > rx_eapol->len) {
        /* 如果收到EAPOL报文大小超过接收报文内存，返回失败 */
        oam_error_log2(0, 0, "uapi_ioctl_receive_eapol eapol pkt len(%d) > buf size(%d).\r\n", skb_buf->len,
            rx_eapol->len);
        oal_netbuf_free(skb_buf);
        return OAL_FAIL;
    }

    if (skb_buf->data != OSAL_NULL) {
        if (memcpy_s(rx_eapol->buf, skb_buf->len, skb_buf->data, skb_buf->len) != EOK) {
            oam_error_log0(0, 0, "{uapi_ioctl_receive_eapol::mem safe function err!}");
            oal_netbuf_free(skb_buf);
            return OAL_FAIL;
        }
    }

    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_receive_eapol}", wal_util_get_vap_id(netdev));
    rx_eapol->len = skb_buf->len;

    oal_netbuf_free(skb_buf);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 向驱动注册处理接收EAPOL报文回调
*****************************************************************************/
td_s32 uapi_ioctl_enable_eapol(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru    *netdev = OSAL_NULL;
    ext_enable_eapol_stru *enable_param = OSAL_NULL;

    enable_param    = (ext_enable_eapol_stru *)buf;
    netdev          = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_enable_eapol:netdev is NULL.");
        return -EXT_EFAIL;
    }

    netdev->ext_eapol.register_code       = OSAL_TRUE;
    netdev->ext_eapol.notify_callback   = enable_param->callback;
    netdev->ext_eapol.context           = enable_param->contex;

    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_enable_eapol}");
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 向驱动解注册处理EAPOL报文回调
*****************************************************************************/
td_s32 uapi_ioctl_disable_eapol(const td_char *puc_ifname, const td_void *buf)
{
    oal_net_device_stru    *netdev = OSAL_NULL;
    unref_param_prv(buf);

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_disable_eapol:netdev is NULL.");
        return -EXT_EFAIL;
    }

    netdev->ext_eapol.register_code       = OSAL_FALSE;
    netdev->ext_eapol.notify_callback   = OSAL_NULL;
    netdev->ext_eapol.context           = OSAL_NULL;

    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_disable_eapol}");
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 从驱动获取MAC地址
*****************************************************************************/
td_s32 uapi_ioctl_get_addr(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru    *netdev = OSAL_NULL;

    /* 调用获取MAC地址操作 */
    netdev  = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_get_addr:netdev is NULL.");
        return -EXT_EFAIL;
    }
    if (memcpy_s(buf, ETH_ADDR_LEN, netdev->dev_addr, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, 0, "{uapi_ioctl_get_addr::mem safe function err!}");
        return -EXT_EFAIL;
    }

    return OAL_SUCC;
}

#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
/*****************************************************************************
 功能描述  : 从驱动获取survey_list
*****************************************************************************/
td_s32 uapi_ioctl_get_survey(const td_char *puc_ifname, td_void *buf)
{
    oal_survey_info_stru sinfo;
    oal_net_device_stru *netdev = OSAL_NULL;
    ext_survey_results_stru *survey = OSAL_NULL;
    struct survey_results *survey_results = OSAL_NULL;
    td_s32 ret;
    td_u32 ifidx;

    netdev  = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_get_survey:netdev is NULL.}");
        return -EXT_EFAIL;
    }

    survey = (ext_survey_results_stru *)buf;
    if (survey == OSAL_NULL || survey->add_survey == OSAL_NULL || survey->survey_results == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_get_survey:survey params is NULL.}");
        return -EXT_EFAIL;
    }

    ifidx = survey->ifidx;
    survey_results = (struct survey_results *)survey->survey_results;
    ret = wal_cfg80211_get_survey_etc(netdev, ifidx, &sinfo);
    if (ret != OAL_SUCC) {
        return ret;
    }
    survey->add_survey(&sinfo, ifidx, survey_results);

    return OAL_SUCC;
}
#endif

#if defined(_PRE_WLAN_FEATURE_ROAM) && defined(_PRE_WLAN_FEATURE_11R)
td_s32 uapi_ioctl_update_ft_ies(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    oal_ft_ies_stru *ft_ies = OSAL_NULL;
    td_s32 ret;
    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_update_ft_ies:netdev is NULL.}");
        return -EXT_EFAIL;
    }

    ft_ies = (oal_ft_ies_stru *)buf;
    if (ft_ies == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_update_ft_ies:ft_ies params is NULL.}");
        return -EXT_EFAIL;
    }

    ret = wal_cfg80211_update_ft_ies(netdev, ft_ies);
    if (ret != OAL_SUCC) {
        return ret;
    }

    return OAL_SUCC;
}
#endif

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 从驱动获取HW feature
*****************************************************************************/
td_s32 uapi_ioctl_get_hw_feature(const td_char *puc_ifname, td_void *buf)
{
    td_u32 loop;
    ext_hw_feature_data_stru *hw_feature_data = (ext_hw_feature_data_stru *)buf;
    oal_ieee80211_supported_band *band = wal_get_wifi_2ghz_band();

    unref_param_prv(puc_ifname);
    hw_feature_data->l_channel_num = band->n_channels;
    hw_feature_data->ht_capab   = band->ht_cap.cap;
    for (loop = 0; loop < (td_u32)band->n_channels; ++loop) {
        hw_feature_data->iee80211_channel[loop].flags      = band->channels[loop].flags;
        hw_feature_data->iee80211_channel[loop].freq       = band->channels[loop].center_freq;
        hw_feature_data->iee80211_channel[loop].channel = band->channels[loop].hw_value;
    }
    for (loop = 0; loop < (td_u32) band->n_bitrates; ++loop) {
        hw_feature_data->aus_bitrate[loop] = band->bitrates[loop].bitrate;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_P2P
static td_s32 hwal_set_addr(oal_net_device_stru *netdev, nl80211_iftype_uint8 type)
{
    td_u8 ac_addr[ETHER_ADDR_LEN] = {0};

    /* 对netdevice进行赋值 */
    if (get_dev_addr(ac_addr, ETHER_ADDR_LEN, type) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_CFG, "{hwal_set_addr::get_dev_addr fail!}");
        return OAL_FAIL;
    }

    if (memcpy_s(netdev->dev_addr, ETHER_ADDR_LEN, ac_addr, ETHER_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_CFG, "{hwal_set_addr::mem safe function err!}");
        return OAL_FAIL;
    }

    if (wal_set_mac_to_mib(netdev) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_CFG, "{hwal_set_addr::wal_set_mac_to_mib fail!}");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : AP STA p2p模式切换
*****************************************************************************/
td_s32 uapi_ioctl_set_mode(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru     *netdev = OSAL_NULL;
    ext_set_mode_stru      *set_mode = OSAL_NULL;
    set_mode    = (ext_set_mode_stru *)buf;
    netdev      = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_ioctl_hapdinit netdev NULL.\r\n");
        return -EXT_EFAIL;
    }
    /* 检查VAP 当前模式和目的模式是否相同，如果相同则直接返回 */
    if (netdev->ieee80211_ptr->iftype == set_mode->iftype) {
        oam_warning_log1(0, OAM_SF_CFG, "uapi_ioctl_set_mode::same iftype[%d],do not need change!", set_mode->iftype);
        return OAL_SUCC;
    }
    oam_warning_log2(0, OAM_SF_CFG, "{uapi_ioctl_set_mode::[%d][%d]}", netdev->ieee80211_ptr->iftype, set_mode->iftype);
    td_u32 ret = wal_cfg80211_intf_mode_check(netdev, set_mode->iftype);
    if (ret != OAL_CONTINUE) {
        return (td_s32)ret;
    }

    if ((set_mode->iftype == NL80211_IFTYPE_AP) || (set_mode->iftype == NL80211_IFTYPE_MESH_POINT)) {
        netdev->ieee80211_ptr->iftype = set_mode->iftype;
        return (td_s32)wal_setup_vap(netdev);
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    /* 设备为P2P 设备才需要进行change virtual interface */
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{uapi_ioctl_set_mode::dev->ml_priv fail!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (is_legacy_vap(hmac_vap)) {
        return OAL_SUCC;
    }

    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(hmac_vap->device_id);
    /* 在GC的连接中将p2p_dev_clear置1，避免在删除vap成功后出现没vap存在情况下时清空hmac_device，清除上次的扫描结果 */
    hmac_device->set_mode_iftype = set_mode->iftype;

    /* 如果当前模式和目的模式不同，则需要:
       1. 停止 VAP
       2. 删除 VAP
       3. 重新创建对应模式VAP
       4. 启动VAP
    */
    /* 停止VAP */
    wal_netdev_stop(netdev);
    if (wal_deinit_wlan_vap(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }
    netdev->ieee80211_ptr->iftype = set_mode->iftype;
    if (wal_init_wlan_vap(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }

    /* 对netdevice进行赋值 */
    if (hwal_set_addr(netdev, set_mode->iftype) != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_init_wlan_netdev::hwal_set_addr fail!}");
        return OAL_FAIL;
    }

    /* 启动VAP */
    wal_netdev_open(netdev);
#endif
    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_set_mode}");
    return OAL_SUCC;
}

td_s32 uapi_ioctl_get_mode(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    ext_set_mode_stru *get_mode = OSAL_NULL;
    get_mode = (ext_set_mode_stru *)buf;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_ioctl_hapdinit netdev NULL.\r\n");
        return -EXT_EFAIL;
    }
    get_mode->iftype = netdev->ieee80211_ptr->iftype;

    return OAL_SUCC;
}

td_s32 uapi_ioctl_del_beacon(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    unref_param(buf);
    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "hwal_ioctl_hapdinit netdev NULL.\r\n");
        return -EXT_EFAIL;
    }

    return (td_s32)wal_cfg80211_del_beacon(netdev);
}
#endif

/*****************************************************************************
 功能描述  : set_netdev
*****************************************************************************/
td_s32 uapi_ioctl_set_netdev(const td_char *puc_ifname, const td_void *buf)
{
    unref_param_prv(puc_ifname);
    unref_param_prv(buf);
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    oal_net_device_stru *netdev = OSAL_NULL;
    td_u8                status;

    status  = *(td_u8 *)buf;
    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_ioctl_set_netdev ieee80211_ptr is NULL");
        return -EXT_EFAIL;
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    struct netif *netif = netdev->lwip_netif;
    if (netif == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_set_netdev cannot find netif");
        return -EXT_EFAIL;
    }
    if (status == EXT_FALSE) {
        wal_netif_set_down(netif, netdev);
    } else if (status == EXT_TRUE) {
        wal_netif_set_up(netif, netdev);
    } else {
        oam_error_log1(0, 0, "uapi_ioctl_set_netdev netdev ERROR: %d\r\n", status);
        return -EXT_EFAIL;
    }
#else // _PRE_WLAN_USE_CUSTOM_LWIP
    if (status == EXT_FALSE) {
        wal_netif_set_down(netdev);
    } else if (status == EXT_TRUE) {
        wal_netif_set_up(netdev);
    } else {
        oam_error_log1(0, 0, "uapi_ioctl_set_netdev netdev ERROR: %d\r\n", status);
        return -EXT_EFAIL;
    }
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
#endif

    return EXT_SUCC;
}

OAL_STATIC td_s32 uapi_ioctl_scan_set_ssid(const ext_scan_stru *scan_params, oal_cfg80211_scan_request_stru *request)
{
    td_u32 count = 0;
    td_u32 l_loop;

    if (scan_params->num_ssids == 0) {
        oam_error_log0(0, 0, "uapi_ioctl_scan::ssid number is 0, return.");
        free(request);
        return -EXT_EFAIL;
    }

    request->ssids = (oal_cfg80211_ssid_stru *)malloc(scan_params->num_ssids * sizeof(oal_cfg80211_ssid_stru));
    if (request->ssids == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_scan::memory is too low, fail to alloc for scan ssid.");
        free(request);
        return -EXT_EFAIL;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(request->ssids, scan_params->num_ssids * sizeof(oal_cfg80211_ssid_stru), 0,
        scan_params->num_ssids * sizeof(oal_cfg80211_ssid_stru));

    for (l_loop = 0; l_loop < scan_params->num_ssids; l_loop++) {
        if (count >= EXT_WPAS_MAX_SCAN_SSIDS) {
            break;
        }

        if (scan_params->ssids[l_loop].ssid_len > IEEE80211_MAX_SSID_LEN) {
            oam_warning_log0(0, 0, "uapi_ioctl_scan::one ssid's length is wrong, skip it");
            continue;
        }

        request->ssids[count].ssid_len = (td_u8)scan_params->ssids[l_loop].ssid_len;
        if (memcpy_s(request->ssids[count].ssid, OAL_IEEE80211_MAX_SSID_LEN,
            scan_params->ssids[l_loop].ssid, scan_params->ssids[l_loop].ssid_len) != EOK) {
            oam_warning_log0(0, 0, "{uapi_ioctl_scan::mem safe function err!}");
            continue;
        }
        count++;
    }
    request->n_ssids = count;

    return EXT_SUCC;
}

OAL_STATIC td_s32 uapi_ioctl_scan_set_user_ie(const ext_scan_stru *scan_params, oal_cfg80211_scan_request_stru *request)
{
    if ((scan_params->extra_ies != OSAL_NULL) && (scan_params->extra_ies_len != 0)) {
        request->ie = (td_u8 *)malloc(scan_params->extra_ies_len);
        if (request->ie == OSAL_NULL) {
            oam_error_log0(0, 0, "uapi_ioctl_scan::memory is too low, fail to alloc for scan ie.");
            goto scan_fail;
        }

        if (memcpy_s(request->ie, scan_params->extra_ies_len, scan_params->extra_ies,
                     scan_params->extra_ies_len) != EOK) {
            oam_warning_log0(0, 0, "{uapi_ioctl_scan::mem safe function err!}");
            goto scan_fail;
        }
        request->ie_len = scan_params->extra_ies_len;
    }

    return EXT_SUCC;

scan_fail:
    if (request->ie != OSAL_NULL) {
        free(request->ie);
    }

    if (request->ssids != OSAL_NULL) {
        free(request->ssids);
    }

    free(request);

    return -EXT_EFAIL;
}

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
OAL_STATIC td_s32 uapi_ioctl_scan_set_channel(const ext_scan_stru *scan_params, oal_cfg80211_scan_request_stru *request)
{
    td_u32 l_loop;
    td_u32 count = 0;
    oal_ieee80211_supported_band *bands = wal_get_wifi_2ghz_band(); /* 仅支持2G带宽 */
    oal_ieee80211_channel *chan = OSAL_NULL;

    if ((scan_params->pl_freqs == OSAL_NULL) || (scan_params->num_freqs == 0)) {
        /* 不指定信道扫描，将支持的信道全部配置到request里 */
        for (l_loop = 0; l_loop < (td_u32)bands->n_channels; l_loop++) {
            chan = &(bands->channels[l_loop]);
            if ((chan->flags & EXT_CHAN_DISABLED) != 0) {
                continue;
            }

            request->channels[count++] = chan;
        }
    } else {
        /* 指定了信道扫描 */
        for (l_loop = 0; l_loop < scan_params->num_freqs; l_loop++) {
            chan = wal_cfg80211_get_channel(scan_params->pl_freqs[l_loop]);
            if (chan == OSAL_NULL) {
                oam_error_log0(0, 0, "uapi_ioctl_scan_set_channel::skip one channel that not supported.");
                continue;
            }

            request->channels[count++] = chan;
        }
    }

    if (count == 0) {
        oam_error_log0(0, 0, "uapi_ioctl_scan_set_channel::can not find supported channel, return.");
        free(request);
        return -EXT_EFAIL;
    }
    request->n_channels = count;

    return EXT_SUCC;
}
static osal_s32 uapi_scan_adapt(oal_cfg80211_scan_request_stru *request, hmac_scan_stru *scan_mgmt)
{
    /* 保存当前下发的扫描请求到本地 内存释放统一由hmac操作 */
    scan_mgmt->request = request;

    /* 提前预处理IE，如果IE指针为空，直接将IE len也置为0 */
    scan_mgmt->request->ie_len = (scan_mgmt->request->ie == OSAL_NULL) ? 0 : scan_mgmt->request->ie_len;

    if (wal_cfg80211_scan(request, scan_mgmt) != OAL_SUCC) {
        if (scan_mgmt->request != OSAL_NULL) {
            if (scan_mgmt->request->ie != OSAL_NULL) {
                free(scan_mgmt->request->ie);
                scan_mgmt->request->ie = OSAL_NULL;
            }

            if (scan_mgmt->request->ssids != OSAL_NULL) {
                free(scan_mgmt->request->ssids);
                request->ssids = OSAL_NULL;
            }
            free(scan_mgmt->request);
            scan_mgmt->request = OSAL_NULL;
        }
        return -EXT_EFAIL;
    }
    return OAL_SUCC;
}
/*****************************************************************************
 功能描述  : 发送扫描至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_scan(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_ifname);
    hmac_device_stru    *hmac_dev = OSAL_NULL;
    hmac_scan_stru      *scan_mgmt = OSAL_NULL;

    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_scan::fail to acquire netdev from ifname, return.");
        return -EXT_EFAIL;
    }

    hmac_dev = hmac_res_get_mac_dev_etc(0);
    scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 如果扫描未完成，则直接返回 */
    if (scan_mgmt->complete == OSAL_FALSE) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan:the last scan is still running,refuse this scan request}");
        return -EXT_EFAIL;
    }

    oal_cfg80211_scan_request_stru *request =
        (oal_cfg80211_scan_request_stru *)malloc(sizeof(oal_cfg80211_scan_request_stru));
    if (request == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_scan::memory is too low, fail to alloc for scan request.");
        /* 变量初始化赋值 */
        return -EXT_EFAIL;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(request, sizeof(oal_cfg80211_scan_request_stru), 0, sizeof(oal_cfg80211_scan_request_stru));

    ext_scan_stru *scan_params = (ext_scan_stru *)buf;
    request->dev      = netdev;
    request->wdev     = netdev->ieee80211_ptr;
    request->n_ssids  = scan_params->num_ssids;
    request->prefix_ssid_scan_flag = scan_params->prefix_ssid_scan_flag;

    /* 配置扫描信道 */
    td_s32 ret = uapi_ioctl_scan_set_channel(scan_params, request);
    if (ret != EXT_SUCC) {
        return ret;
    }

#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
    request->acs_scan_flag = (osal_u8)scan_params->acs_scan_flag;
    if (scan_params->acs_scan_flag == TD_FALSE) {
#endif
        /* 配置SSID */
        ret = uapi_ioctl_scan_set_ssid(scan_params, request);
        if (ret != EXT_SUCC) {
            return ret;
        }

        /* 配置user ie */
        ret = uapi_ioctl_scan_set_user_ie(scan_params, request);
        if (ret != EXT_SUCC) {
            return ret;
        }
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
    }
#endif

    return uapi_scan_adapt(request, scan_mgmt);
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 功能描述  : 添加接口
*****************************************************************************/
td_s32 uapi_ioctl_add_if(const td_char *puc_ifname, td_void *buf)
{
    ext_if_add_stru   *if_add = OSAL_NULL;
    td_u8               ifname[IFNAMSIZ];
    td_u32              len = (td_u32)sizeof(ifname);
    td_s32              ret;

    if_add = (ext_if_add_stru *)buf;
    ret = wal_init_drv_wlan_netdev((nl80211_iftype_uint8)if_add->type, WAL_PHY_MODE_11N, (td_char *)ifname, &len);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_ioctl_add_if: wal_init_drv_wlan_netdev failed!");
        return -EXT_EFAIL;
    }
    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_add_if}");
    return EXT_SUCC;
}

/*****************************************************************************
 功能描述  : 删除接口
*****************************************************************************/
td_s32 uapi_ioctl_remove_if(const td_char *puc_ifname, td_void *buf)
{
    ext_if_remove_stru             *if_remove = OSAL_NULL;
    td_s32                           ret;

    if_remove = (ext_if_remove_stru *)buf;

    ret = wal_deinit_drv_wlan_netdev((const td_char *)if_remove->ifname);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_ioctl_remove_if: wal_deinit_drv_wlan_netdev failed!");
        return -EXT_EFAIL;
    }
    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ioctl_remove_if}");
    return EXT_SUCC;
}

/*****************************************************************************
 功能描述  : 获取P2P GO/GC的MAC地址
*****************************************************************************/
td_s32 uapi_ioctl_get_p2p_addr(const td_char *puc_ifname, td_void *buf)
{
    ext_get_p2p_addr_stru          *get_p2p_addr = OSAL_NULL;
    td_u32                           ret;

    get_p2p_addr = (ext_get_p2p_addr_stru *)buf;
    ret = get_dev_addr(get_p2p_addr->mac_addr, ETH_ADDR_LEN, (td_u8)get_p2p_addr->type);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_ioctl_get_p2p_addr: get_dev_addr failed!");
        return -EXT_EFAIL;
    }
    return EXT_SUCC;
}

/*****************************************************************************
 功能描述  : 发送探测请求帧上报命令到驱动层
*****************************************************************************/
td_s32 uapi_ioctl_probe_req_report(const td_char *puc_ifname, const td_void *buf)
{
    oal_net_device_stru   *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_probe_req_report::netdev null!}");
        return -EXT_EFAIL;
    }

    return EXT_SUCC;
}

/*****************************************************************************
 功能描述  : 保持在指定信道
*****************************************************************************/
td_s32 uapi_ioctl_remain_on_channel(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru   *netdev = OSAL_NULL;
    oal_wireless_dev      *wdev = OSAL_NULL;
    osal_u64                 pull_cookie = 0;
    td_u8                  channel_idx;
    ext_on_channel_stru  *param = OSAL_NULL;
    oal_ieee80211_channel* wifi_2ghz_channels = wal_get_g_wifi_2ghz_channels();

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ioctl_remain_on_channel::netdev null!}");
        return -EXT_EFAIL;
    }
    wdev = netdev->ieee80211_ptr;

    /* 接收由wpa下发的参数 param[0] = freq, param[1] = duration */
    param = (ext_on_channel_stru *)buf;
    channel_idx = (td_u8)oal_ieee80211_frequency_to_channel(param->freq);

    oam_warning_log1(0, OAM_SF_ANY, "{uapi_ioctl_remain_on_channel, time[%d]}", (td_u32)param->duration);
    return (td_s32)wal_cfg80211_remain_on_channel(wdev, &(wifi_2ghz_channels[channel_idx - 1]),
                                                  (td_u32)param->duration, &pull_cookie);
}

/*****************************************************************************
 功能描述  : 取消在指定信道
*****************************************************************************/
td_s32 uapi_ioctl_cancel_remain_on_channel(const td_char *puc_ifname, const td_void *buf)
{
    oal_net_device_stru   *netdev = OSAL_NULL;
    oal_wireless_dev      *wdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_cancel_remain_on_channel: netdev NULL.");
        return -EXT_EFAIL;
    }
    wdev = netdev->ieee80211_ptr;

    /* 若为1表示当前操作为保持在指定信道，为0表示取消保持在指定信道 */
    return (td_s32)wal_cfg80211_cancel_remain_on_channel(wdev, (osal_u64)0);
}

/*****************************************************************************
 功能描述  : 下发p2p的noa参数到驱动
*****************************************************************************/
td_s32 uapi_ioctl_set_p2p_noa(const td_char *puc_ifname, td_void *buf)
{
    mac_cfg_p2p_noa_param_stru mac_cfg_p2p_noa;
    oal_net_device_stru       *netdev = OSAL_NULL;
    ext_p2p_noa_stru         *p2p_noa = OSAL_NULL;
    hmac_device_stru           *mac_dev = OSAL_NULL;
    td_u32                     ret;

    mac_dev = hmac_res_get_mac_dev_etc(0);
    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ioctl_set_p2p_noa::netdev NULL!}");
        return -EXT_EFAIL;
    }
    p2p_noa = (ext_p2p_noa_stru *)buf;

    mac_cfg_p2p_noa.count = p2p_noa->count;
    mac_cfg_p2p_noa.start_time = (td_u32)p2p_noa->start;
    mac_cfg_p2p_noa.duration = (td_u32)p2p_noa->duration;
    mac_cfg_p2p_noa.interval = mac_dev->beacon_interval;

    ret = wal_ioctl_set_p2p_noa(netdev, &mac_cfg_p2p_noa);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_ioctl_set_p2p_noa: wal_ioctl_set_p2p_noa failed!");
        return -EXT_EFAIL;
    }
    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_set_p2p_noa}", wal_util_get_vap_id(netdev));
    return EXT_SUCC;
}

/*****************************************************************************
 功能描述  : 下发p2p的powersave参数到驱动
*****************************************************************************/
td_s32 uapi_ioctl_set_p2p_powersave(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru       *netdev = OSAL_NULL;
    ext_p2p_power_save_stru  *p2p_power_save = OSAL_NULL;
    mac_cfg_p2p_ops_param_stru p2p_ops_param = {0};
    td_u32                     ret;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ioctl_set_p2p_powersave::netdev null!}");
        return -EXT_EFAIL;
    }
    p2p_power_save = (ext_p2p_power_save_stru *)buf;
    p2p_ops_param.ct_window = (td_u8)p2p_power_save->ctwindow;

    if (p2p_power_save->opp_ps != -1) {
        p2p_ops_param.ops_ctrl = (td_s8)p2p_power_save->opp_ps;
    }

    ret = wal_ioctl_set_p2p_ops(netdev, &p2p_ops_param);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_ioctl_set_p2p_powersave: wal_ioctl_set_p2p_ops failed!");
        return -EXT_EFAIL;
    }

    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_set_p2p_powersave}", wal_util_get_vap_id(netdev));
    return EXT_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : 设置app ie 到wifi驱动
*****************************************************************************/
td_s32 uapi_ioctl_set_ap_wps_p2p_ie(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    ext_app_ie_stru    *app_ie = OSAL_NULL;
    oal_app_ie_stru      wps_p2p_ie = {0};

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_ioctl_set_ap_wps_p2p_ie parameter NULL.");
        return -EXT_EFAIL;
    }
    app_ie = (ext_app_ie_stru *)buf;
    wps_p2p_ie.ie_len = app_ie->ie_len;
    wps_p2p_ie.app_ie_type = app_ie->app_ie_type;

    if (wps_p2p_ie.ie_len > WLAN_WPS_IE_MAX_SIZE) {
        oam_error_log0(0, 0, "app ie length is too large!");
        return -EXT_EFAIL;
    }
    if (memcpy_s(wps_p2p_ie.ie, WLAN_WPS_IE_MAX_SIZE, app_ie->ie, app_ie->ie_len) != EOK) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_ap_wps_p2p_ie::mem safe function err!}");
        return -EXT_EFAIL;
    }

    wifi_printf("uapi_ioctl_set_ap_wps_p2p_ie");
    return (td_s32)wal_ioctl_set_wps_p2p_ie(netdev, wps_p2p_ie.ie, wps_p2p_ie.ie_len,
                                            wps_p2p_ie.app_ie_type);
}

/*****************************************************************************
 功能描述  : 获取驱动相关标志位
*****************************************************************************/
td_s32 uapi_ioctl_get_drv_flags(const td_char *puc_ifname, td_void *buf)
{
    ext_get_drv_flags_stru *get_drv_flag = (ext_get_drv_flags_stru *)buf;
    oal_net_device_stru *netdev = OSAL_NULL;
    /* 获取下发的ifname对应的type类型 */
    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_ioctl_scan::fail to acquire netdev from ifname, return.");
        return -EXT_EFAIL;
    }
#if defined(_PRE_WLAN_FEATURE_P2P) || defined(_PRE_WLAN_FEATURE_WPA3)
    switch (netdev->ieee80211_ptr->iftype) {
        case NL80211_IFTYPE_P2P_GO:
            get_drv_flag->drv_flags = (osal_u64)(EXT_DRIVER_FLAGS_AP);
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            get_drv_flag->drv_flags = (osal_u64)(EXT_DRIVER_FLAGS_P2P_CONCURRENT |
                                               EXT_DRIVER_FLAGS_P2P_CAPABLE);
            break;
#ifdef _PRE_WLAN_FEATURE_WPA3
        case NL80211_IFTYPE_STATION:
        case NL80211_IFTYPE_AP:
            get_drv_flag->drv_flags = (osal_u64)(EXT_DRIVER_FLAGS_SAE);
            break;
#endif
        default:
            get_drv_flag->drv_flags = 0;
    }
#else
    unref_param_prv(puc_ifname);
    get_drv_flag->drv_flags = 0;
#endif

#ifdef _PRE_WLAN_FEATURE_11R
    if (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) {
        get_drv_flag->drv_flags |= EXT_DRIVER_FLAGS_UPDATE_FT_IES;
    }
#endif
    return EXT_SUCC;
}

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 发送去关联至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_disconnect(const td_char *puc_ifname, const td_void *buf)
{
    oal_net_device_stru         *netdev = OSAL_NULL;
    td_u16                       reason_code;

    netdev      = oal_get_netdev_by_name(puc_ifname);
    reason_code  = *(td_u16 *)buf;

    oam_warning_log1(0, OAM_SF_ANY, "{uapi_ioctl_disconnect:: reason_code is %u}", reason_code);
    return (td_s32)wal_cfg80211_disconnect(netdev, reason_code);
}
/*****************************************************************************
 功能描述  : 判断是否为无效ie
*****************************************************************************/
td_u8 hwal_is_valid_ie_attr(const td_u8 *ie, td_u32 ie_len)
{
    td_u8 elemlen;
    const td_u8 *elem_ie = ie;
    td_u32 len = ie_len;

    /* ie可以为空 */
    if (elem_ie == OSAL_NULL) {
        return OSAL_TRUE;
    }

    while (len != 0) {
        if (len < 2) { /* 2 无效ie */
            oam_error_log0(0, 0, "ie parameter FALSE.");
            return OSAL_FALSE;
        }
        len -= 2; /* 2 步长 */

        elemlen = elem_ie[1];
        if (elemlen > len) {
            oam_error_log0(0, 0, "ie parameter FALSE.");
            return OSAL_FALSE;
        }
        len -= elemlen;
        elem_ie += 2 + elemlen; /* 2 步长 */
    }

    return OSAL_TRUE;
}

/*****************************************************************************
 功能描述  : 发送关联至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_assoc(const td_char *puc_ifname, td_void *buf)
{
    oal_cfg80211_connect_params_stru sme = {0};
    oal_net_device_stru *netdev       = oal_get_netdev_by_name(puc_ifname);
    ext_associate_params_stru *assoc = (ext_associate_params_stru *)buf;

    if ((assoc->ssid == OSAL_NULL) || (assoc->ssid_len == 0) || (netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "assoc parameter NULL.");
        return -EXT_EFAIL;
    }

    sme.ssid        = assoc->ssid;
    sme.ssid_len    = assoc->ssid_len;
    if (hwal_is_valid_ie_attr(assoc->ie, assoc->ie_len) == OSAL_FALSE) {
        return -EXT_EFAIL;
    }
    sme.ie          = assoc->ie;
    sme.ie_len      = assoc->ie_len;

    if (assoc->auth_type > NL80211_AUTHTYPE_AUTOMATIC) {
        oam_error_log0(0, 0, "assoc->auth_type ERROR.");
        return -EXT_EFAIL;
    } else {
        sme.auth_type = assoc->auth_type;
    }

    sme.channel = wal_cfg80211_get_channel((td_s32)assoc->freq);
    if ((sme.channel == OSAL_NULL) || ((sme.channel->flags & EXT_CHAN_DISABLED) != 0)) {
        oam_error_log0(0, 0, "st_sme.channel ERROR.");
        return -EXT_EFAIL;
    }
    sme.bssid = assoc->bssid;
    sme.privacy = assoc->privacy;

    if ((assoc->mfp != EXT_MFP_REQUIRED) && (assoc->mfp != EXT_MFP_NO) && (assoc->mfp != EXT_MFP_OPTIONAL)) {
        oam_error_log1(0, 0, "assoc->mfp ERROR. mfp = %d", assoc->mfp);
        return -EXT_EFAIL;
    }

    sme.mfp = (enum nl80211_mfp)assoc->mfp;

    if (assoc->key != OSAL_NULL) {
        sme.key     = assoc->key;
        sme.key_len = assoc->key_len;
        sme.key_idx = assoc->key_idx;
    }

    if (memcpy_s(&sme.crypto, sizeof(ext_crypto_settings_stru),
        assoc->crypto, sizeof(ext_crypto_settings_stru)) != EOK) {
        oam_error_log0(0, 0, "{uapi_ioctl_assoc::mem safe function err!}");
        return -EXT_EFAIL;
    }
    /* 仅同ssid不同bssid场景下会返回失败,此时仍上报成功到wpa,
     * 同时中止关联并上报disconnect到wpa,防止wpa空等10s */
    if (wal_check_auto_conn_status(netdev, assoc->auto_conn, sme.bssid) != OAL_SUCC) {
        return OAL_SUCC;
    }

    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_assoc}", wal_util_get_vap_id(netdev));
    return (td_s32)wal_cfg80211_connect(netdev, &sme);
}
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/*****************************************************************************
 功能描述  : set rekey info数据传递至WAL层
*****************************************************************************/
td_s32 uapi_ioctl_set_rekey_info(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru     *netdev = OSAL_NULL;
    ext_rekey_offload_stru *rekey_offload = OSAL_NULL;

    netdev          = oal_get_netdev_by_name(puc_ifname);
    rekey_offload   = (ext_rekey_offload_stru *)buf;

    return (td_s32)wal_cfg80211_set_rekey_info(netdev, (mac_rekey_offload_stru *)rekey_offload);
}
#endif

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 删除station命令下发至wal层
*****************************************************************************/
td_s32 uapi_ioctl_sta_remove(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru                  *netdev = OSAL_NULL;
    td_u8                                *mac_addr = OSAL_NULL;

    netdev      = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_sta_remove:netdev is NULL.}");
        return OAL_FAIL;
    }

    mac_addr         = (td_u8*)buf;

    oam_warning_log1(0, OAM_SF_ANY, "vap_id[%d] {uapi_ioctl_sta_remove}", wal_util_get_vap_id(netdev));
    return (td_s32)wal_cfg80211_del_station(netdev, mac_addr);
}
#endif

/*****************************************************************************
 功能描述  : WPA 发送Action帧,在已组好的帧体前加入Action帧帧头
*****************************************************************************/
td_s32 uapi_ioctl_send_action(const td_char *puc_ifname, td_void *buf)
{
    unref_param(buf);
    oal_net_device_stru *netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_send_action:netdev is NULL.}");
        return OAL_FAIL;
    }

    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 功能描述  : Mesh中Wpa_supplicant负责整个关联过程，通过该接口设置Mesh 类型User状态
*****************************************************************************/
td_s32 uapi_ioctl_set_mesh_user(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_mesh_user:netdev is NULL.}");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : Mesh中Wpa_supplicant负责整个关联过程，通过该接口设置Mesh用户的组播密钥
*****************************************************************************/
td_s32 uapi_ioctl_set_mesh_gtk(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    ext_set_mesh_user_gtk_str *gtk_params = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_mesh_gtk:netdev is NULL.}");
        return -EXT_EFAIL;
    }

    gtk_params = (ext_set_mesh_user_gtk_str *)buf;

    /* 只支持ccmp-128 */
    if (gtk_params->gtk_len != WLAN_CCMP_KEY_LEN) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_mesh_gtk::wrong gtk len,only ccmp-128.}");
        return -EXT_EFAIL;
    }
    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : wpa通过该接口设置Accept Peer的值
*****************************************************************************/
td_s32 uapi_ioctl_set_accept_peer(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru             *netdev = OSAL_NULL;
    ext_enable_accept_peer_stru    *accept_peer = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_accept_peer:netdev is NULL.}");
        return OAL_FAIL;
    }

    accept_peer = (ext_enable_accept_peer_stru *)buf;

    if (accept_peer->enable_accept_peer > 1) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_accept_peer::invalid accept peer value.}");
        return OAL_FAIL;
    }
    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : wpa通过该接口设置Accept Sta的值
*****************************************************************************/
td_s32 uapi_ioctl_set_accept_sta(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru         *netdev = OSAL_NULL;
    ext_enable_accept_sta_stru *accept_sta = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_accept_sta:netdev is NULL.}");
        return OAL_FAIL;
    }

    accept_sta = (ext_enable_accept_sta_stru *)buf;

    if (accept_sta->enable_accept_sta > 1) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_accept_sta::invalid accept peer value.}");
        return OAL_FAIL;
    }
    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}
#endif

td_s32 uapi_ioctl_set_csi_config(const td_char *puc_ifname, const td_void *buf)
{
    td_s32 ret;
    mac_csi_usr_config_stru csi_config = {0};
    oal_net_device_stru *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_csi_config:netdev is NULL.}");
        return OAL_FAIL;
    }

    csi_config = *(mac_csi_usr_config_stru *)buf;
    /* 消息发送到相应芯片架构的hmac模块处理 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_SET_CSI_PARAM,
        (osal_u8 *)&csi_config, OAL_SIZEOF(mac_csi_usr_config_stru));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ioctl_set_csi_config::return err code [%d]!}", ret);
    }
    return ret;
}

td_s32 uapi_ioctl_csi_switch(const td_char *puc_ifname, const td_void *buf)
{
    td_s32 ret;
    td_s32 switch_flag = 0;
    oal_net_device_stru *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_csi_switch:netdev is NULL.}");
        return OAL_FAIL;
    }

    switch_flag = *(td_s32 *)buf;
    /* 消息发送到相应芯片架构的hmac模块处理 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_CSI_SWITCH,
        (osal_u8 *)&switch_flag, OAL_SIZEOF(td_s32));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ioctl_csi_switch::return err code [%d]!}", ret);
    }
    return ret;
}

/*****************************************************************************
 功能描述  : wpa通过该接口设置user app ie的值
*****************************************************************************/
td_s32 uapi_ioctl_set_user_app_ie(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;
    oal_app_ie_stru      oal_app_ie = {0};
    ext_usr_app_ie_stru *app_ie = OSAL_NULL;
    td_s32                ret;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_accept_sta:netdev is NULL.}");
        return OAL_FAIL;
    }

    app_ie = (ext_usr_app_ie_stru *)buf;
    /* 设置配置命令参数 */
    oal_app_ie.ie_len = (app_ie->set == 0) ? 0 : app_ie->ie_len; /* 0:删除APP_IE, 1:添加APP_IE */
    if (oal_app_ie.ie_len != 0) {
        if (memcpy_s(oal_app_ie.ie, WLAN_WPS_IE_MAX_SIZE, app_ie->ie,
            oal_app_ie.ie_len) != EOK) {
            return OAL_FAIL;
        }
    }
    oal_app_ie.app_ie_type = app_ie->ie_index;
    oal_app_ie.ie_type_bitmap = app_ie->bitmap;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_USER_APP_IE,
        (osal_u8 *)&oal_app_ie, OAL_SIZEOF(oal_app_ie));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, 0, "{uapi_ioctl_set_user_app_ie::wal_sync_post2hmac_no_rsp failed[%d].}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : wpa通过该接口设置延时上报机制参数
*****************************************************************************/
td_s32 uapi_ioctl_set_delay_report(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;
    ext_delay_report_stru *delay_report = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{uapi_ioctl_set_delay_report:netdev is NULL.}");
        return OAL_FAIL;
    }
    /* 仅STA支持配置 */
    if (netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION) {
        oam_warning_log1(0, 0, "{uapi_ioctl_set_delay_report:type[%d] isn't support this feature.}",
            netdev->ieee80211_ptr->iftype);
        return OAL_FAIL;
    }
    delay_report = (ext_delay_report_stru *)buf;
    wal_set_delay_report_config(delay_report->enable, delay_report->timeout);

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_WPA3
/*****************************************************************************
 功能描述  : 处理External Auth status的ioctl
*****************************************************************************/
td_s32 uapi_ioctl_send_ext_auth_status(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;
    ext_external_auth_stru *ext_auth = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{hwal_ioctl_external_auth:netdev is NULL.}");
        return OAL_FAIL;
    }
    /* 仅STA支持配置 */
    if ((netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION) &&
        (netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP)) {
        oam_warning_log1(0, 0, "{hwal_ioctl_external_auth:type[%d] isn't support this feature.}",
            netdev->ieee80211_ptr->iftype);
        return OAL_FAIL;
    }
    ext_auth = (ext_external_auth_stru *)buf;
    wal_cfg80211_external_auth_status(netdev, ext_auth);

    return OAL_SUCC;
}

td_s32 uapi_ioctl_update_dh_ie(const td_char *ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{uapi_ioctl_update_dh_ie:netdev is NULL.}");
        return OAL_FAIL;
    }
    /* 仅AP支持 */
    if (netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP) {
        oam_warning_log1(0, 0, "{uapi_ioctl_update_dh_ie:type[%d] isn't support this feature.}",
            netdev->ieee80211_ptr->iftype);
        return OAL_FAIL;
    }
    wal_cfg80211_update_dh_ie(netdev, (ext_update_dh_ie_stru *)buf);

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : 添加 pmkid的ioctl
*****************************************************************************/
td_s32 uapi_ioctl_add_pmkid(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;
    ext_pmkid_params *pmkid_params = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{uapi_ioctl_add_pmkid:netdev is NULL.}");
        return OAL_FAIL;
    }

    pmkid_params = (ext_pmkid_params *)buf;
    return (td_s32)wal_cfg80211_set_pmksa(netdev, pmkid_params);
}

/*****************************************************************************
 功能描述  : 删除 pmkid的ioctl
*****************************************************************************/
td_s32 uapi_ioctl_remove_pmkid(const td_char *puc_ifname, td_void *buf)
{
    oal_net_device_stru  *netdev = OSAL_NULL;
    ext_pmkid_params *pmkid_params = OSAL_NULL;

    netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{uapi_ioctl_remove_pmkid:netdev is NULL.}");
        return OAL_FAIL;
    }

    pmkid_params = (ext_pmkid_params *)buf;
    return (td_s32)wal_cfg80211_remove_pmksa(netdev, pmkid_params);
}

/*****************************************************************************
 功能描述  : 删除 所有pmkid的ioctl
*****************************************************************************/
td_s32 uapi_ioctl_flush_pmkid(const td_char *puc_ifname, td_void *buf)
{
    unref_param(buf);
    oal_net_device_stru  *netdev = oal_get_netdev_by_name(puc_ifname);
    if ((netdev == OSAL_NULL) || (netdev->ieee80211_ptr == OSAL_NULL)) {
        oam_error_log0(0, 0, "{uapi_ioctl_flush_pmkid:netdev is NULL.}");
        return OAL_FAIL;
    }
    return (td_s32)wal_cfg80211_flush_pmksa(netdev);
}
/*****************************************************************************
 功能描述  : 驱动对wpa提供统一调用接口
*****************************************************************************/
td_s32 drv_soc_hwal_wpa_ioctl(td_char *pc_ifname, const ext_ioctl_command_stru *cmd)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    if (cmd == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hwal_wpa_ioctl::null point.");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 根据dev_name找到dev */
    netdev = oal_get_netdev_by_name(pc_ifname);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{hwal_wpa_ioctl::oal_get_netdev_by_name return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) == 0) &&
        ((cmd->cmd == EXT_IOCTL_SEND_MLME) || (cmd->cmd == EXT_IOCTL_SEND_EAPOL) ||
        (cmd->cmd == EXT_IOCTL_RECEIVE_EAPOL) ||
#ifdef _PRE_WLAN_FEATURE_MESH
        (cmd->cmd == EXT_IOCTL_SEND_ACTION) ||
#endif
        (cmd->cmd == EXT_IOCTL_SCAN
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
        && (netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP)
#endif
        ) || (cmd->cmd == EXT_IOCTL_ASSOC) || (cmd->cmd == EXT_IOCTL_WIFI_APP_SERVICE))) {
        return -EXT_EFAIL;
    }

    if ((cmd->cmd < HWAL_EVENT_BUTT) && (g_ast_hwal_ioctl_handlers[cmd->cmd] != OSAL_NULL) && (cmd->buf != OSAL_NULL)) {
#ifdef _PRE_WLAN_DFT_STAT
        wal_record_wifi_external_log((td_u32)g_ast_hwal_ioctl_handlers[cmd->cmd]);
#endif
        return g_ast_hwal_ioctl_handlers[cmd->cmd](pc_ifname, cmd->buf);
    }
    oam_error_log1(0, 0, "hwal_wpa_ioctl ::The CMD[%d] handlers is NULL.\r\n", cmd->cmd);

    return -EXT_EFAIL;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

