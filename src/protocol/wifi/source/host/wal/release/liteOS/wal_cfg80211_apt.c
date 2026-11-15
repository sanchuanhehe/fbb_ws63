/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: wal event file.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_cfg80211_apt.h"
#include "wal_net.h"
#include "hmac_ext_if.h"
#include "wal_event_msg.h"
#include "wal_wpa_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_CFG80211_APT_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 外部函数声明
*****************************************************************************/
/*****************************************************************************
  3 全局变量定义
*****************************************************************************/
wifi_promis_cb g_upload_frame_func = OSAL_NULL;
ext_send_event_cb       g_send_event_func   = OSAL_NULL;

/*****************************************************************************
  4 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : drv_soc_register_send_event_cb
 功能描述  : 提供给用户的回调函数注册接口，回调函数用于驱动向wpa抛事件
 输入参数  : [1]func
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
td_s32 drv_soc_register_send_event_cb(ext_send_event_cb func)
{
    if ((g_send_event_func != NULL) && (func != NULL)) {
        g_send_event_func = func;
        return -OAL_FAIL;
    }
    g_send_event_func = func;
    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : cfg80211_new_sta
 功能描述  : 上报new station事件接口
*****************************************************************************/
td_u32 cfg80211_new_sta(const oal_net_device_stru *netdev, const td_u8 *mac_addr,
    td_u8 addr_len, const oal_station_info_stru *sinfo)
{
    ext_new_sta_info_stru new_sta_info = {0};

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }

    if (sinfo->assoc_req_ies_len == 0) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_new_sta::assoc_req_ies or assoc_req_ies_len null error %p,%d.}\r\n",
            (uintptr_t)sinfo->assoc_req_ies, sinfo->assoc_req_ies_len);
        return OAL_FAIL;
    }

    new_sta_info.ielen = sinfo->assoc_req_ies_len;
    new_sta_info.l_reassoc = 0;
    if (memcpy_s(new_sta_info.macaddr, ETH_ADDR_LEN, mac_addr, addr_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::mem safe function err!}");
        return OAL_FAIL;
    }

    new_sta_info.ie = (td_u8 *)malloc(sinfo->assoc_req_ies_len);
    if (new_sta_info.ie == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::new_sta_info->ie malloc error!}");
        return OAL_FAIL;
    }
    if (memcpy_s(new_sta_info.ie, sinfo->assoc_req_ies_len,
        sinfo->assoc_req_ies, sinfo->assoc_req_ies_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::mem safe function err!}");
        free(new_sta_info.ie);
        return OAL_FAIL;
    }

    td_u32 ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_NEW_STA, (td_u8 *)&new_sta_info,
        (td_u32)sizeof(ext_new_sta_info_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_new_sta::g_send_event_func fail.}\r\n");
        free(new_sta_info.ie);
    }
    return ret;
}

/*****************************************************************************
 功能描述  : 上报new station事件接口
 输入参数  : oal_net_device_stru *dev, const td_u8 *mac_addr,
            oal_gfp_enum_uint8 gfp
 修改历史      :
  1.日    期   : 2016年6月13日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 cfg80211_del_sta(oal_net_device_stru *netdev, const td_u8 *mac_addr, td_u8 addr_len)
{
    td_u32 l_ret;

    /* 入参检查 */
    if ((netdev == OSAL_NULL) || (mac_addr == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_del_sta::dev or mac_addr ptr NULL %p,%p.}\r\n",
            (uintptr_t)netdev, (uintptr_t)mac_addr);
        return OAL_FAIL;
    }

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_del_sta::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }
    /* 维测信息，向wpa上报del sta消息，2、3表示mac地址第3、4位 */
    wifi_printf("report del_sta[%02X:%02X:%02X:%02X:XX:XX]\r\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3]);
    l_ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_DEL_STA, (td_u8 *)mac_addr, addr_len);
    return l_ret;
}

/*****************************************************************************
 功能描述  : 上报管理帧数据接口
 输入参数  : struct wireless_dev *wdev, td_s32 freq, td_s32 sig_mbm,
             const td_u8 *buf, size_t len, oal_gfp_enum_uint8 gfp
*****************************************************************************/
td_u8 cfg80211_rx_mgmt(const oal_net_device_stru *netdev, td_s32 freq, td_s32 l_sig_mbm,
    const td_u8 *buf, size_t len)
{
    ext_rx_mgmt_stru rx_mgmt = {0};
    td_u32 l_ret;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::g_send_event_func is null.}");
        return OSAL_FALSE;
    }
    /* 入参检查 */
    if (buf == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::wdev or buf ptr NULL.}");
        return OSAL_FALSE;
    }
    rx_mgmt.len       = len;
    rx_mgmt.freq      = (td_u32)freq;
    rx_mgmt.l_sig_mbm = l_sig_mbm;
    if (len != 0) {
        rx_mgmt.buf = malloc(len);
        if (rx_mgmt.buf == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::rx_mgmt->buf malloc error.}");
            return OSAL_FALSE;
        }
        if (memcpy_s(rx_mgmt.buf, len, buf, len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::mem safe function err!}");
            free(rx_mgmt.buf);
            return OSAL_FALSE;
        }
    }

    l_ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_RX_MGMT, (td_u8 *) &rx_mgmt,
                                      sizeof(ext_rx_mgmt_stru));
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::g_send_event_func fail.}\r\n");
        if (rx_mgmt.buf != OSAL_NULL) {
            free(rx_mgmt.buf);
        }
        return OSAL_FALSE;
    }

    return OSAL_TRUE;
}

/*****************************************************************************
 功能描述  : 扫描结果上报接口
*****************************************************************************/
td_void cfg80211_inform_bss_frame(const oal_net_device_stru *netdev, const oal_ieee80211_channel *ieee80211_channel,
    const wal_scanned_bss_info_stru *scanned_bss_info)
{
    ext_scan_result_stru scan_result = {0};

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::g_send_event_func is null.}\r\n");
        return;
    }

    if (scanned_bss_info->mgmt == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame:: null param}");
        return ;
    }

    td_u32 ie_len = scanned_bss_info->mgmt_len - (td_u32)OAL_OFFSET_OF(oal_ieee80211_mgmt_stru, u.probe_resp.variable);

    scan_result.variable = malloc(ie_len);
    if (scan_result.variable == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mallocErr. ie_len[%d]}", ie_len);
        return;
    }

    /* 安全编程规则6.6例外（3）从堆中分配内存后，赋予初值 */
    memset_s(scan_result.variable, ie_len, 0, ie_len);

    scan_result.ie_len        = ie_len;
    scan_result.beacon_ie_len = 0;
    scan_result.beacon_int    = (td_s16)scanned_bss_info->mgmt->u.probe_resp.beacon_int;
    scan_result.s_caps        = (td_s16) scanned_bss_info->mgmt->u.probe_resp.capab_info;
    scan_result.l_level       = scanned_bss_info->l_signal;
    scan_result.l_freq        = ieee80211_channel->center_freq;
    scan_result.l_flags       = (td_s32)ieee80211_channel->flags;

    if (memcpy_s(scan_result.bssid, ETH_ADDR_LEN, scanned_bss_info->mgmt->bssid, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mem safe function err!}");
        free(scan_result.variable);
        return;
    }
    if (memcpy_s(scan_result.variable, ie_len, scanned_bss_info->mgmt->u.probe_resp.variable, ie_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::mem safe function err!}");
        oal_free(scan_result.variable);
        return;
    }

    if ((td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_SCAN_RESULT, (td_u8 *)&scan_result,
        sizeof(ext_scan_result_stru)) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::g_send_event_func fail.}");
        free(scan_result.variable);
    }
}

#ifdef _PRE_WLAN_FEATURE_MESH
td_u32 cfg80211_scan_report_fill_mesh_ie(td_u8 *ie_buf, td_u8 *ie_payload, td_u32 ie_payload_len)
{
    td_u8 *mesh_conf_ie = mac_find_ie_etc(MAC_EID_MESH_CONF, ie_payload, ie_payload_len);
    if (mesh_conf_ie != OSAL_NULL) {
        if (memcpy_s(ie_buf, mesh_conf_ie[1] + MAC_IE_HDR_LEN, mesh_conf_ie, mesh_conf_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += mesh_conf_ie[1] + MAC_IE_HDR_LEN;
    }

    td_u8 *ext_beacon_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_BEACON, ie_payload, ie_payload_len);
    td_u8 *td_prb_rsp_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_RSP, ie_payload, ie_payload_len);
    if (ext_beacon_ie != OSAL_NULL) {
        if (memcpy_s(ie_buf, ext_beacon_ie[1] + MAC_IE_HDR_LEN, ext_beacon_ie,
            ext_beacon_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += ext_beacon_ie[1] + MAC_IE_HDR_LEN;
    } else if (td_prb_rsp_ie != OSAL_NULL) {
        if (memcpy_s(ie_buf, td_prb_rsp_ie[1] + MAC_IE_HDR_LEN, td_prb_rsp_ie,
            td_prb_rsp_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += td_prb_rsp_ie[1] + MAC_IE_HDR_LEN;
    }

    /* Mesh Optimization域长度 */
    td_u8 *td_optimization_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_OPTIMIZATION,
        ie_payload, ie_payload_len);
    if (td_optimization_ie != OSAL_NULL) {
        if (memcpy_s(ie_buf, td_optimization_ie[1] + MAC_IE_HDR_LEN, td_optimization_ie,
            td_optimization_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += td_optimization_ie[1] + MAC_IE_HDR_LEN;
    }

    /* ssid */
    td_u8 *meshid_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_MESHID, ie_payload, ie_payload_len);
    /* 查找meshid的ie */
    if ((meshid_ie != OSAL_NULL) && (meshid_ie[MAC_MESH_MESHID_LEN_POS] < WLAN_MESHID_MAX_LEN)) {
        /* 获取ssid ie的长度 */
        if (memcpy_s(ie_buf, meshid_ie[1] + MAC_IE_HDR_LEN, meshid_ie, meshid_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += meshid_ie[1] + MAC_IE_HDR_LEN;
    }

    /* Trigger IE */
    td_u8 *td_mesh_trigger_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_TRIGGER, ie_payload, ie_payload_len);
    if (td_mesh_trigger_ie != OSAL_NULL) {
        if (memcpy_s(ie_buf, td_mesh_trigger_ie[1] + MAC_IE_HDR_LEN, td_mesh_trigger_ie,
            td_mesh_trigger_ie[1] + MAC_IE_HDR_LEN) != EOK) {
            return OAL_FAIL;
        }
        ie_buf += td_mesh_trigger_ie[1] + MAC_IE_HDR_LEN;
    }
    return OAL_SUCC;
}
#endif

OAL_STATIC td_void cfg80211_connect_result_return(const oal_net_device_stru *netdev,
    ext_connect_result_stru* connect_result)
{
    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::g_send_event_func is null.}\r\n");
        return;
    }

    if ((td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_CONNECT_RESULT, (td_u8 *)connect_result,
        sizeof(ext_connect_result_stru)) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::g_send_event_func fail.}\r\n");
        if (connect_result->req_ie != OSAL_NULL) {
            free(connect_result->req_ie);
            connect_result->req_ie = OSAL_NULL;
        }
        if (connect_result->resp_ie != OSAL_NULL) {
            free(connect_result->resp_ie);
            connect_result->resp_ie = OSAL_NULL;
        }
    }

    return;
}

/*****************************************************************************
 功能描述  : 关联结果上报接口
*****************************************************************************/
td_void cfg80211_connect_result(const oal_net_device_stru *netdev, const oal_connet_result_stru *connet_result)
{
    ext_connect_result_stru connect_result = {0};

    if (g_send_event_func == OSAL_NULL || netdev == OSAL_NULL) {
        oam_warning_log1(0, 0, "{cfg80211_connect_result::g_send_event_func or dev null %p}", (uintptr_t)netdev);
        return;
    }

    if (memcpy_s(connect_result.bssid, ETH_ADDR_LEN, connet_result->bssid, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
        return;
    }

    if ((connet_result->req_ie != OSAL_NULL) && (connet_result->req_ie_len != 0)) {
        connect_result.req_ie = malloc(connet_result->req_ie_len);
        connect_result.req_ie_len = connet_result->req_ie_len;
        if (connect_result.req_ie == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::connect_result->req_ie zalloc error}");
            /* 返回 */
            return;
        }
        if (memcpy_s(connect_result.req_ie, connet_result->req_ie_len,
            connet_result->req_ie, connet_result->req_ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
            free(connect_result.req_ie);
            return;
        }
    }

    if ((connet_result->rsp_ie != OSAL_NULL) && (connet_result->rsp_ie_len != 0)) {
        connect_result.resp_ie = malloc(connet_result->rsp_ie_len);
        connect_result.resp_ie_len = connet_result->rsp_ie_len;
        if (connect_result.resp_ie == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::resp_ie zalloc error}");
            free(connect_result.req_ie);
            /* 返回 */
            return;
        }
        if (memcpy_s(connect_result.resp_ie, connet_result->rsp_ie_len,
            connet_result->rsp_ie, connet_result->rsp_ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_connect_result::mem safe function err!}");
            free(connect_result.req_ie);
            free(connect_result.resp_ie);
            return;
        }
    }

    connect_result.status = connet_result->status_code;
    connect_result.freq = connet_result->freq;

    cfg80211_connect_result_return(netdev, &connect_result);

    return;
}

/*****************************************************************************
 功能描述  : 去关联上报接口
*****************************************************************************/
td_u32 cfg80211_disconnected(const oal_net_device_stru *netdev, td_u16 reason, const td_u8 *ie, td_u32 ie_len)
{
    ext_disconnect_stru disconnect = {0};
    td_u32 l_ret;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::g_send_event_func is null.}");
        return OAL_FAIL;
    }

    if ((ie != OSAL_NULL) && (ie_len != 0)) {
        disconnect.ie = malloc(ie_len);
        disconnect.ie_len = ie_len;
        if (disconnect.ie == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::disconnect.ie malloc error.}");
            /* 返回 */
            return OAL_FAIL;
        }
        if (memcpy_s(disconnect.ie, ie_len, ie, ie_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::mem safe function err!}");
            free(disconnect.ie);
            return OAL_FAIL;
        }
    }
    disconnect.reason = reason;

    l_ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_DISCONNECT, (td_u8 *)&disconnect,
                                      sizeof(ext_disconnect_stru));
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_disconnected::g_send_event_func fail.}");
        if (disconnect.ie != OSAL_NULL) {
            free(disconnect.ie);
        }
    }

    return l_ret;
}


/*****************************************************************************
 功能描述  : 延时上报机制中超时上报去关联给WPA
*****************************************************************************/
td_u32 cfg80211_timeout_disconnected(const oal_net_device_stru *netdev)
{
    td_u32 l_ret;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_timeout_disconnected::g_send_event_func is null.}");
        return OAL_FAIL;
    }

    l_ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_TIMEOUT_DISCONN, OSAL_NULL, 0);
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_timeout_disconnected::g_send_event_func fail.}");
    }

    return l_ret;
}

/*****************************************************************************
 功能描述  : 扫描完成上报接口
*****************************************************************************/
td_void cfg80211_scan_done(const oal_net_device_stru *netdev, ext_scan_status_enum status)
{
    ext_driver_scan_status_stru scan_ctl_status;

    scan_ctl_status.scan_status = status;
    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_scan_done::g_send_event_func is null.}\r\n");
        return;
    }
    g_send_event_func(netdev->name, EXT_ELOOP_EVENT_SCAN_DONE, (td_u8 *)&scan_ctl_status,
                      sizeof(ext_driver_scan_status_stru));
}

/*****************************************************************************
 功能描述  : STA根据CSA切换信道事件上报
*****************************************************************************/
td_u32 cfg80211_csa_channel_switch(const oal_net_device_stru *netdev, td_s32 freq)
{
    td_u32               ret;
    ext_ch_switch_stru  channel_switch;

    channel_switch.freq = freq;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_mesh_report_accept_peer::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }

    ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_CHANNEL_SWITCH,
                                    (td_u8 *)&channel_switch, (td_u32)sizeof(ext_ch_switch_stru));
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_WPA3
/*****************************************************************************
 功能描述  : SAE External Auth事件上报，触发wpa开始sae认证
*****************************************************************************/
td_void cfg80211_external_auth_req(const oal_net_device_stru *netdev, const oal_external_auth_stru *ext_auth)
{
    ext_external_auth_stru external_auth = {0};

    if (g_send_event_func == OSAL_NULL || netdev == OSAL_NULL) {
        oam_warning_log1(0, 0, "{cfg80211_external_auth_req::g_send_event_func or dev null %p}", (uintptr_t)netdev);
        return;
    }

    if (memcpy_s(external_auth.bssid, ETH_ADDR_LEN, ext_auth->bssid, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::mem safe function err!}");
        return;
    }

    if ((ext_auth->ssid != OSAL_NULL) && (ext_auth->ssid_len != 0)) {
        external_auth.ssid = malloc(ext_auth->ssid_len);
        external_auth.ssid_len = ext_auth->ssid_len;
        if (external_auth.ssid == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::ssid zalloc error}");
            /* 返回 */
            return;
        }
        if (memcpy_s(external_auth.ssid, ext_auth->ssid_len, ext_auth->ssid, ext_auth->ssid_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::mem safe function err!}");
            free(external_auth.ssid);
            return;
        }
    }

    if (ext_auth->pmkid[0] != '\0') {
        external_auth.pmkid = malloc(WLAN_PMKID_LEN * sizeof(td_u8));
        if (external_auth.pmkid == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::pmkid zalloc error}");
            free(external_auth.ssid);
            return;
        }
        if (memcpy_s(external_auth.pmkid, WLAN_PMKID_LEN, ext_auth->pmkid, WLAN_PMKID_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::mem safe function err!}");
            free(external_auth.ssid);
            free(external_auth.pmkid);
            return;
        }
    }

    external_auth.key_mgmt_suite = ext_auth->key_mgmt_suite;
    external_auth.status = ext_auth->status;
    external_auth.auth_action = ext_auth->auth_action;

    if ((td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_EXTERNAL_AUTH, (td_u8 *)&external_auth,
        sizeof(ext_external_auth_stru)) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_external_auth_req::g_send_event_func fail.}\r\n");
        free(external_auth.ssid);
        free(external_auth.pmkid);
    }

    return;
}

td_void cfg80211_owe_info(const oal_net_device_stru *netdev, const ext_update_dh_ie_stru *owe_info)
{
    if (g_send_event_func == OSAL_NULL || netdev == OSAL_NULL) {
        oam_warning_log1(0, 0, "{cfg80211_owe_info::g_send_event_func or dev null %p}", (uintptr_t)netdev);
        return;
    }
    if ((td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_OWE_INFO, (td_u8 *)owe_info,
        sizeof(ext_update_dh_ie_stru)) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_owe_info::g_send_event_func fail.}\r\n");
    }

    return;
}

#endif
#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 功能描述  : Mesh上报需要发送Mesh Peering Close Frame
 被调函数  :
  1.日    期   : 2019年1月26日
    修改内容   : 新生成函数

*****************************************************************************/
td_u32 cfg80211_mesh_close(const oal_net_device_stru *netdev, const td_u8 *mac_addr, td_u8 addr_len, td_u16 reason)
{
    ext_mesh_close_peer_stru mesh_close_peer;

    if (memset_s(&mesh_close_peer, sizeof(ext_mesh_close_peer_stru), 0,
        sizeof(ext_mesh_close_peer_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::mem safe function err!}");
        return OAL_FAIL;
    }

    if (memcpy_s(mesh_close_peer.macaddr, ETH_ADDR_LEN, mac_addr, addr_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::mem safe function err!}");
        return OAL_FAIL;
    }

    mesh_close_peer.reason = reason;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_mesh_close::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }

    g_send_event_func(netdev->name, EXT_ELOOP_EVENT_MESH_CLOSE, (td_u8 *)&mesh_close_peer,
                      sizeof(ext_mesh_close_peer_stru));
    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 功能描述  : 保持在指定信道上报
 修改历史      :
  1.日    期   : 2019年4月28日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 cfg80211_remain_on_channel(const oal_net_device_stru *netdev, td_u32 freq, td_u32 duration)
{
    ext_on_channel_stru remain_on_channel;
    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_remain_on_channel::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }
    remain_on_channel.freq = (td_s32)freq;
    remain_on_channel.duration = (td_s32)duration;

    g_send_event_func(netdev->name, EXT_ELOOP_EVENT_REMAIN_ON_CHANNEL, (td_u8 *)&remain_on_channel,
                      sizeof(ext_on_channel_stru));
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 取消保持在指定信道事件上报
 修改历史      :
  1.日    期   : 2019年4月28日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 cfg80211_cancel_remain_on_channel(const oal_net_device_stru *netdev, td_u32 freq)
{
    ext_on_channel_stru remain_on_channel;
    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_mesh_report_accept_peer::g_send_event_func is null.}\r\n");
        return OAL_FAIL;
    }
    remain_on_channel.freq = (td_s32)freq;

    g_send_event_func(netdev->name, EXT_ELOOP_EVENT_CANCEL_REMAIN_ON_CHANNEL, (td_u8 *)&remain_on_channel,
                      sizeof(ext_on_channel_stru));
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 上报发送状态接口
 输入参数  : struct wireless_dev *wdev, osal_u64 cookie,
             const td_u8 *buf, size_t len, bool ack, oal_gfp_enum_uint8 gfp
*****************************************************************************/
td_u8 cfg80211_mgmt_tx_status(struct wireless_dev *wdev, const td_u8 *buf, size_t len, td_u8 ack)
{
    ext_tx_status_stru tx_status;
    td_u32              l_ret;

    if (g_send_event_func == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::g_send_event_func is null.}\r\n");
        return OSAL_FALSE;
    }

    if (memset_s(&tx_status, sizeof(ext_tx_status_stru), 0, sizeof(ext_tx_status_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::mem safe function err!}");
        return OSAL_FALSE;
    }
    /* 入参检查 */
    if ((wdev == OSAL_NULL) || (wdev->netdev == OSAL_NULL) || (buf == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::wdev or buf ptr NULL %p,%p.}\r\n",
            (uintptr_t)wdev, (uintptr_t)buf);
        return OSAL_FALSE;
    }

    tx_status.buf  = OSAL_NULL;
    tx_status.len      = len;
    tx_status.ack      = ack;

    if (len != 0) {
        tx_status.buf = malloc(len);
        if (tx_status.buf == OSAL_NULL) {
            oam_error_log1(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::tx_status->buf malloc error %p.}",
                (uintptr_t)tx_status.buf);
            return OSAL_FALSE;
        }
        if (memcpy_s(tx_status.buf, len, buf, len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::mem safe function err!}");
            free(tx_status.buf);
            return OSAL_FALSE;
        }
    }

    l_ret = (td_u32)g_send_event_func(wdev->netdev->name, EXT_ELOOP_EVENT_TX_STATUS, (td_u8 *)&tx_status,
                                      sizeof(ext_tx_status_stru));
    if (l_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::g_send_event_func fail.}\r\n");
        if (tx_status.buf != OSAL_NULL) {
            free(tx_status.buf);
        }
        return OSAL_FALSE;
    }

    return OSAL_TRUE;
}
#endif

#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_WLAN_FEATURE_11R)
/*****************************************************************************
 函 数 名  : cfg80211_ft_event_proc_sta_etc
 功能描述  : STA上报FT事件处理
*****************************************************************************/
td_u32 cfg80211_ft_event_proc_sta_etc(const oal_net_device_stru *netdev,
    const oal_cfg80211_ft_event_stru *cfg_ft_event)
{
    td_u32 ret = (td_u32)g_send_event_func(netdev->name, EXT_ELOOP_EVENT_FT_RESPONSE,
        (td_u8 *)cfg_ft_event, sizeof(oal_cfg80211_ft_event_stru));
    if (ret != OAL_SUCC) {
        return OAL_FAIL;
    }
    return OAL_SUCC;
}
#endif // _PRE_WLAN_FEATURE_11R
osal_s32 wal_do_upload_promis_frame_cb(const hmac_rx_mgmt_event_stru *frame)
{
    osal_void *recv_buf = OSAL_NULL;
    osal_s32 frame_len;
    osal_s8 rssi;

    if (frame == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_do_upload_promis_frame_cb::frame is null");
        return OAL_ERR_CODE_PTR_NULL;
    }
    recv_buf = (osal_void *)frame->buf;
    frame_len = (osal_s32)frame->len;
    rssi = (osal_s8)frame->rssi;

    if (g_upload_frame_func != OSAL_NULL) {
        g_upload_frame_func(recv_buf, frame_len, rssi);
    }

    osal_kfree(recv_buf);
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_register_upload_frame_cb
 功能描述  : 供用户调用，注册处理驱动上报的数据包函数
*****************************************************************************/
osal_void wal_register_upload_frame_cb(wifi_promis_cb func)
{
    osal_adapt_kthread_lock();
    g_upload_frame_func = func;
    osal_adapt_kthread_unlock();
}

OAL_STATIC wifi_rx_mgmt_cb g_report_mgmt_frame_func = OSAL_NULL;
osal_void uapi_wlan_register_report_mgmt_frame(wifi_rx_mgmt_cb mgmt_process_cb)
{
    osal_adapt_kthread_lock();
    g_report_mgmt_frame_func = mgmt_process_cb;
    osal_adapt_kthread_unlock();
}

osal_s32 wal_do_report_rx_mgmt_frame_cb(hmac_rx_mgmt_event_stru *mgmt_frame)
{
    osal_void *recv_buf = OSAL_NULL;
    osal_u16 frame_len;
    osal_s8 rssi;

    if (mgmt_frame == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_do_report_rx_mgmt_frame_cb::mgmt_frame is null");
        return OAL_ERR_CODE_PTR_NULL;
    }
    recv_buf = (osal_void *)mgmt_frame->buf;
    frame_len = mgmt_frame->len;
    rssi = (osal_s8)mgmt_frame->rssi;

    if (g_report_mgmt_frame_func != OSAL_NULL) {
        g_report_mgmt_frame_func(recv_buf, frame_len, rssi);
    }
    osal_kfree(recv_buf);
    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

