/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Scan function associated with the kernel interface.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_scan.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_cfg80211.h"
#include "wal_main.h"
#include "wal_event.h"
#include "hmac_ext_if.h"
#include "frw_timer.h"
#include "wal_cfg80211_apt.h"
#include "oal_list.h"
#include "hmac_resource.h"

#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef U64
#define U64 UINT64
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_SCAN_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*****************************************************************************
  3 函数实现
*****************************************************************************/
td_u32 is_p2p_scan_req(oal_cfg80211_scan_request_stru *request)
{
    if (request == OAL_PTR_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{is_p2p_scan_req::param request is NULL}");
        return OAL_FALSE;
    }

    return ((request->n_ssids > 0) && (request->ssids != NULL) &&
            (request->ssids[0].ssid_len == osal_strlen("DIRECT-")) &&
            (osal_memcmp(request->ssids[0].ssid, "DIRECT-", (osal_s32)osal_strlen("DIRECT-")) == 0));
}

td_void wal_free_scan_mgmt_resource(hmac_scan_stru *scan_mgmt)
{
    if (scan_mgmt->request->ssids != OSAL_NULL) {
        free(scan_mgmt->request->ssids);
        scan_mgmt->request->ssids = OSAL_NULL;
    }
    if (scan_mgmt->request->ie != OSAL_NULL) {
        free(scan_mgmt->request->ie);
        scan_mgmt->request->ie = OSAL_NULL;
    }
    free(scan_mgmt->request);
    scan_mgmt->request = OSAL_NULL;
}

/*****************************************************************************
 功能描述  : 逐个上报ssid消息给内核
*****************************************************************************/
static td_void wal_inform_bss_frame(const oal_net_device_stru *netdev, wal_scanned_bss_info_stru *scanned_bss_info)
{
    if (scanned_bss_info->mgmt == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_inform_bss_frame::input param pointer is null, scanned_bss_info[%p]!}");
        return;
    }

    oal_ieee80211_channel_stru *ieee80211_channel = wal_cfg80211_get_channel((td_s32)scanned_bss_info->s_freq);
    if (ieee80211_channel == OSAL_NULL) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_inform_bss_frame::get channel failed, wrong s_freq[%d]}",
                         (td_s32)scanned_bss_info->s_freq);
        return;
    }

    scanned_bss_info->l_signal = scanned_bss_info->l_signal * 100; /* 100 扩大100倍 */

    /* 逐个上报内核bss 信息 */
    cfg80211_inform_bss_frame(netdev, ieee80211_channel, scanned_bss_info);
    return;
}

/*****************************************************************************
 函 数 名  : wal_inform_all_bss
 功能描述  : 上报所有的bss到内核
 输入参数  : hmac_bss_mgmt_stru  *bss_mgmt,
             td_u8   vap_id
*****************************************************************************/
td_void wal_inform_all_bss(const oal_net_device_stru *netdev, const hmac_bss_mgmt_stru *bss_mgmt, td_u8 vap_id)
{
    struct osal_list_head        *entry = OSAL_NULL;
    wal_scanned_bss_info_stru  scanned_bss_info;
    td_u32                     bss_num_not_in_regdomain = 0;

#ifdef _PRE_WLAN_FEATURE_MESH
    hmac_vap_stru *mac_vap = mac_res_get_hmac_vap(vap_id);
    if (mac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_inform_all_bss::mac_vap_get_vap_stru failed}");
        return;
    }
#endif

    /* 获取锁 */
#ifndef FREERTOS_DEFINE
    /* Freertos中, osal_kthreak_lock会关闭任务调度, 但后面wpa_supplicant有获取mutex锁，导致ASSERT异常 */
    osal_kthread_lock();
#endif

    /* 遍历扫描到的bss信息 */
    osal_list_for_each(entry, &(bss_mgmt->bss_list_head)) {
        hmac_scanned_bss_info *scanned_bss = osal_list_entry(entry, hmac_scanned_bss_info, dlist_head);
        td_u8                  chan        = scanned_bss->bss_dscr_info.st_channel.chan_number;
        enum ieee80211_band    band        = (enum ieee80211_band)scanned_bss->bss_dscr_info.st_channel.band;

        /* 判断信道是不是在管制域内，如果不在，则不上报内核 */
        if (hmac_is_channel_num_valid_etc(band, chan) != OAL_SUCC) {
            oam_warning_log2(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::chan=%d,band=%d not in regdomain}", chan, band);
            bss_num_not_in_regdomain++;
            continue;
        }

        /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
        memset_s(&scanned_bss_info, sizeof(wal_scanned_bss_info_stru), 0, sizeof(wal_scanned_bss_info_stru));
        scanned_bss_info.l_signal =
            (scanned_bss->bss_dscr_info.c_rssi > 0) ? 0 : (scanned_bss->bss_dscr_info.c_rssi);

        /* 填bss所在信道的中心频率 */
        scanned_bss_info.s_freq = (td_s16)oal_ieee80211_channel_to_frequency(chan, band);

        /* 填管理帧指针和长度 */
        scanned_bss_info.mgmt     = (oal_ieee80211_mgmt_stru *)(scanned_bss->bss_dscr_info.mgmt_buff);
        scanned_bss_info.mgmt_len = scanned_bss->bss_dscr_info.mgmt_len;

        /* 获取上报的扫描结果的管理帧的帧头 */
        mac_ieee80211_frame_stru *frame_hdr = (mac_ieee80211_frame_stru *)scanned_bss->bss_dscr_info.mgmt_buff;

        /* 如果扫描请求接收到的帧类型有beacon类型，统一修改为probe rsp类型上报，
           为了解决上报内核的扫描结果beacon帧不够敏感的问题，此问题，在01出现过 */
        frame_hdr->frame_control.sub_type = (frame_hdr->frame_control.sub_type == WLAN_BEACON) ?
            WLAN_PROBE_RSP : frame_hdr->frame_control.sub_type;

        /* 上报扫描结果给内核 */
        /* 如果是mesh ap发起的扫描，只上报mesh ap */
#ifdef _PRE_WLAN_FEATURE_MESH
        if (((mac_vap->vap_mode == WLAN_VAP_MODE_MESH) && (scanned_bss->bss_dscr_info.is_td_mesh == OSAL_TRUE)) ||
            (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)) {
            wal_inform_bss_frame(netdev, &scanned_bss_info);
        }
#else
        wal_inform_bss_frame(netdev, &scanned_bss_info);
#endif
    }

    /* 解除锁 */
#ifndef FREERTOS_DEFINE
    osal_kthread_unlock();
#endif

    wifi_printf_always("Scan::vap[%d] find bss_num[%d] in regdomain, other bss_num[%d]\r\n",
        vap_id, (bss_mgmt->bss_num - bss_num_not_in_regdomain), bss_num_not_in_regdomain);
    oam_warning_log2(vap_id, OAM_SF_SCAN, "{wal_inform_all_bss::%d bss not in regdomain,inform kernal bss num=%d}",
                     bss_num_not_in_regdomain, (bss_mgmt->bss_num - bss_num_not_in_regdomain));
}

/*****************************************************************************
 函 数 名  : wal_set_scan_channel
 功能描述  : 提取内核下发扫描信道相关参数
*****************************************************************************/
static td_u32 wal_set_scan_channel(const oal_cfg80211_scan_request_stru *request,
    mac_cfg80211_scan_param_stru *scan_param, hmac_vap_stru *hmac_vap)
{
    td_u32 loop, is_5g_enable;
    td_u32 num_chan_2g = 0;
    td_u32 num_chan_5g = 0;

    if (request->n_channels == 0) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_get_scan_channel_num::channel number is 0 in scan request, is wrong!}");
        return OAL_FAIL;
    }

    scan_param->channels_2g = (osal_u32 *)(scan_param + 1);
    scan_param->channels_5g = (osal_u32 *)(scan_param->channels_2g + request->n_channels);

    is_5g_enable = hmac_device_check_5g_enable(hmac_vap->device_id);

    for (loop = 0; loop < request->n_channels; loop++) {
        td_u16 center_freq;
        td_u32 chn;

        center_freq = request->channels[loop]->center_freq;

        /* 根据中心频率，计算信道号 */
        chn = (td_u32)oal_ieee80211_frequency_to_channel((td_s32)center_freq);

        if (center_freq <= WAL_MAX_FREQ_2G) {
            scan_param->channels_2g[num_chan_2g++] = chn;
        } else {
            if (is_5g_enable == OAL_FALSE) {
                continue;
            }
            scan_param->channels_5g[num_chan_5g++] = chn;
        }
    }

    scan_param->num_channels_2g = (td_u8)num_chan_2g;
    scan_param->num_channels_5g = (td_u8)num_chan_5g;
    scan_param->num_channels_all = (osal_u8)request->n_channels;

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置扫描的SSID
*****************************************************************************/
static td_void wal_set_scan_ssid(const oal_cfg80211_scan_request_stru *request,
    mac_cfg80211_scan_param_stru *scan_param)
{
    td_u32   loop;
    td_u32   ssid_num;

    scan_param->ssid_num = 0;

    if (request->prefix_ssid_scan_flag == OSAL_TRUE) {
        scan_param->ssid_num = (osal_s32)request->n_ssids;
        return;
    }

    /* 取内核下发的ssid的个数 */
    ssid_num = (request->n_ssids > WLAN_SCAN_REQ_MAX_BSS) ? WLAN_SCAN_REQ_MAX_BSS : request->n_ssids;

    /* 将用户下发的ssid信息拷贝到对应的结构体中 */
    if ((ssid_num == 0) || (request->ssids == OSAL_NULL)) {
        return;
    }

    scan_param->ssid_num = (osal_s32)ssid_num;
    for (loop = 0; loop < ssid_num; loop++) {
        scan_param->ssids[loop].ssid_len = request->ssids[loop].ssid_len;
        if (scan_param->ssids[loop].ssid_len > OAL_IEEE80211_MAX_SSID_LEN) {
            oam_warning_log2(0, OAM_SF_SCAN, "{wal_set_scan_ssid::ssid length [%d] is larger than %d.}",
                             scan_param->ssids[loop].ssid_len, OAL_IEEE80211_MAX_SSID_LEN);
            scan_param->ssids[loop].ssid_len = OAL_IEEE80211_MAX_SSID_LEN;
        }

        if (memcpy_s(scan_param->ssids[loop].ssid, OAL_IEEE80211_MAX_SSID_LEN,
                     request->ssids[loop].ssid, scan_param->ssids[loop].ssid_len) != EOK) {
            oam_warning_log0(0, OAM_SF_SCAN, "{wal_set_scan_ssid::fail to copy ssid to scan_param!}");
        }
    }
}

/*****************************************************************************
 函 数 名  : wal_process_timer_for_scan
 功能描述  : 关闭扫描结果老化定时器，启动扫描定时器做扫描超时保护处理
*****************************************************************************/
static td_void  wal_process_timer_for_scan(td_u8 vap_id)
{
    hmac_vap_stru           *hmac_vap = OSAL_NULL;

    /* 获取hmac vap */
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(vap_id);
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(vap_id, OAM_SF_SCAN, "{wal_process_timer_for_scan::hmac_vap is null!}");
        return;
    }
    /* 关闭扫描结果老化定时器 */
    if (hmac_vap->scanresult_clean_timeout.is_registerd == OSAL_TRUE) {
        frw_destroy_timer_entry(&(hmac_vap->scanresult_clean_timeout));
    }
    return;
}

/*****************************************************************************
 函 数 名  : wal_start_scan_req
 功能描述  : 解析内核下发扫描命令相关参数，启动扫描
*****************************************************************************/
td_u32 wal_start_scan_req(oal_net_device_stru *netdev, hmac_scan_stru *scan_mgmt)
{
    mac_cfg80211_scan_param_stru *scan_param = OSAL_NULL;
    td_u32 ret, ie_len;
    size_t all_len;
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    oal_cfg80211_scan_request_stru *request = scan_mgmt->request;

    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_start_scan_req::hmac_vap is null!}");
        return OAL_FAIL;
    }

    ie_len = (request->ie_len > WLAN_WPS_IE_MAX_SIZE) ? WLAN_WPS_IE_MAX_SIZE : request->ie_len;
    all_len = sizeof(mac_cfg80211_scan_param_stru) + request->n_channels * sizeof(osal_u32) * 2 + ie_len; /* 2g 和 5g */

    /* 申请 结构体以及信道,IE的内存, 内存布局为 mac_cfg80211_scan_param_stru结构体 + 2g信道信息 + 5g信道信息 + ie */
    scan_param = (mac_cfg80211_scan_param_stru *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (osal_u16)all_len, OAL_TRUE);
    if (scan_param == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_SCAN, "{wal_start_scan_req::scan failed, alloc memory[%u] failed!}", all_len);
        return OAL_ERR_CODE_PTR_NULL;
    }
    (osal_void)memset_s(scan_param, all_len, 0, all_len);

    /* 解析内核下发的扫描信道列表 */
    if (wal_set_scan_channel(request, scan_param, hmac_vap) != OAL_SUCC) {
        oal_mem_free(scan_param, OAL_TRUE);
        return OAL_FAIL;
    }

    /* 解析内核下发的ssid */
    wal_set_scan_ssid(request, scan_param);

    scan_param->ie = (osal_u8 *)(scan_param + 1) + request->n_channels * sizeof(osal_u32) * 2; /* 2g 和 5g */
    scan_param->ie_len = ie_len;
    /* 有可能不携带ie,所以需要判断ie指针有效性 */
    if ((request->ie != OSAL_NULL) && (ie_len != 0)) {
        if (memcpy_s(scan_param->ie, ie_len, request->ie, ie_len) != EOK) { /* ie_len 已经对 request->ie_len 做过处理 */
            oam_error_log1(0, OAM_SF_SCAN, "{wal_start_scan_req::memcpy request->ie fail, ie_len[%u].}", ie_len);
            oal_mem_free(scan_param, OSAL_FALSE);
            return OAL_FAIL;
        }
    }

    scan_param->scan_type = OAL_ACTIVE_SCAN; /* active scan */
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
    scan_param->acs_scan_flag = request->acs_scan_flag;
#endif

    /* P2P WLAN/P2P 特性情况下，根据扫描的ssid 判断是否为p2p device 发起的扫描，
     * ssid 为"DIRECT-"则认为是p2p device 发起的扫描解析下发扫描的device 是否为p2p device(p2p0)
     */
    if (is_p2p_scan_req(request)) {
        scan_param->is_p2p0_scan = OSAL_TRUE;
    }

    /* 在事件前防止异步调度完成扫描后,发生同步问题 */
    scan_mgmt->complete = OSAL_FALSE;

    /* 传递二级指针 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_START_SCAN,
        (osal_u8 *)scan_param, (osal_u32)all_len);
    oal_mem_free(scan_param, OAL_TRUE);
    if (ret != OAL_SUCC) {
        scan_mgmt->complete = OSAL_TRUE;
        return OAL_FAIL;
    }
    /* 关闭扫描结果老化定时器,启动扫描超时定时器 */
    wal_process_timer_for_scan(hmac_vap->vap_id);

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_send_scan_abort_msg
 功能描述  : 终止扫描
 输入参数  : oal_net_device_stru   *net_dev
 输出参数  : 无
*****************************************************************************/
OAL_STATIC td_u32 wal_send_scan_abort_msg(oal_net_device_stru *netdev)
{
    td_u32                          pedding_data = 0;       /* 填充数据，不使用，只是为了复用接口 */
    td_u32                          ret;

    /* 拋事件通知device侧终止扫描 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SCAN_ABORT,
        (osal_u8 *)&pedding_data, OAL_SIZEOF(pedding_data));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SCAN, "{wal_send_scan_abort_msg::fail to stop scan, error[%u]}", ret);
    }

    return ret;
}

/*************** **************************************************************
 函 数 名  : wal_force_scan_complete
 功能描述  : 通知扫描完成
 输入参数  : oal_net_device_stru   *net_dev,
             td_bool          is_aborted
 输出参数  : 无
 返 回 值  : td_s32
*****************************************************************************/
td_u32 wal_force_scan_complete(oal_net_device_stru *netdev)
{
    hmac_vap_stru *mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_force_scan_complete::Cannot find mac_vap by net_dev!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取hmac device */
    hmac_device_stru *hmac_dev = hmac_res_get_mac_dev_etc(0);
    /* stop的vap和正在扫描的vap不相同则直接返回 */
    if (mac_vap->vap_id != hmac_dev->scan_mgmt.scan_record_mgmt.vap_id) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_SCAN,
                         "{wal_force_scan_complete::stop_vap[%d] is different scan_vap[%d]!}",
                         mac_vap->vap_id, hmac_dev->scan_mgmt.scan_record_mgmt.vap_id);
        return OAL_SUCC;
    }

    hmac_scan_stru *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 结束扫描的时候设置标志为非ANY扫描 */
    scan_mgmt->scan_record_mgmt.is_any_scan = OSAL_FALSE;

    /* 如果是来自内部的扫描 */
    if (scan_mgmt->request == OSAL_NULL) {
        /* 判断是否存在内部扫描，如果存在，也需要停止 */
        if ((hmac_dev->scan_mgmt.is_scanning == OSAL_TRUE) &&
            (mac_vap->vap_id == hmac_dev->scan_mgmt.scan_record_mgmt.vap_id)) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_SCAN, "{wal_force_scan_complete::maybe internal scan,stop scan}");
            /* 终止扫描 */
            wal_send_scan_abort_msg(netdev);
        }

        return OAL_SUCC;
    }

    /* 获取hmac vap */
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->vap_id);
    if (hmac_vap == OSAL_NULL) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_SCAN,
                         "{wal_force_scan_complete::hmac_vap is null, vap_id[%d]!}", mac_vap->vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 删除等待扫描超时定时器 */
    if (hmac_vap->scan_timeout.is_registerd == OSAL_TRUE) {
        frw_destroy_timer_entry(&(hmac_vap->scan_timeout));
    }

    /* 如果是上层下发的扫描请求，则通知内核扫描结束，内部扫描不需通知 */
    if (scan_mgmt->request != OSAL_NULL) {
        /* 对于内核下发的扫描request资源加锁 */
        osal_adapt_kthread_lock();

        /* 上报内核扫描结果 */
        wal_inform_all_bss(netdev, &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt), mac_vap->vap_id);

        /* 通知内核扫描终止 */
        cfg80211_scan_done(netdev, EXT_SCAN_SUCCESS);
        wal_free_scan_mgmt_resource(scan_mgmt);
        scan_mgmt->complete = OSAL_TRUE;
        /* 通知完内核，释放资源后解锁 */
        osal_adapt_kthread_unlock();
        /* 下发device终止扫描 */
        wal_send_scan_abort_msg(netdev);
        oam_info_log1(mac_vap->vap_id, OAM_SF_SCAN,
            "{wal_force_scan_complete::force to stop scan of vap_id[%d]}", mac_vap->vap_id);
    }

    return OAL_SUCC;
}

td_void wal_scan_report_ie_calc(const wal_scanned_bss_info_stru *scanned_bss_info, td_u32 *need_report_ie_len)
{
    td_u32 mgmt_len = scanned_bss_info->mgmt_len;
    td_u32 ie_len = mgmt_len - MAC_80211_FRAME_LEN - MAC_SSID_OFFSET; /* ie payload len */
    oal_ieee80211_mgmt_stru *mgmt = scanned_bss_info->mgmt;
    mac_header_frame_control_stru *frame_control = (mac_header_frame_control_stru *)(uintptr_t)(&mgmt->frame_control);

    td_u8 *ie_payload = (frame_control->sub_type == WLAN_BEACON) ?
        (td_u8 *)(mgmt->u.beacon.variable) : (td_u8 *)(mgmt->u.probe_resp.variable);

    /* 获取ssid ie len */
    td_u8 *ssid_ie = mac_find_ie_etc(MAC_EID_SSID, ie_payload, (osal_s32)ie_len);
    if ((ssid_ie != OSAL_NULL) && (ssid_ie[1] < WLAN_SSID_MAX_LEN)) {
        /* 获取ssid ie的长度 */
        *need_report_ie_len += ssid_ie[1] + MAC_IE_HDR_LEN;
    }

    /* 获取RSN ie len */
    td_u8 *rsn_ie = mac_find_ie_etc(MAC_EID_RSN, ie_payload, (osal_s32)ie_len);
    if (rsn_ie != OSAL_NULL) {
        *need_report_ie_len += rsn_ie[1] + MAC_IE_HDR_LEN;
    }

    /* WPA */
    td_u8 *wpa_ie = hmac_find_vendor_ie_etc(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA, ie_payload, (osal_s32)ie_len);
    if (wpa_ie != OSAL_NULL) {
        *need_report_ie_len += wpa_ie[1] + MAC_IE_HDR_LEN;
    }
    /* WPS */
    td_u8 *wps_ie = hmac_find_vendor_ie_etc(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPS, ie_payload, (osal_s32)ie_len);
    if (wps_ie != OSAL_NULL) {
        *need_report_ie_len += wps_ie[1] + MAC_IE_HDR_LEN;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_check_is_mesh_vap(ie_payload, (td_u8)(ie_len)) == OSAL_TRUE) {
        td_u8 *mesh_conf_ie = mac_find_ie_etc(MAC_EID_MESH_CONF, ie_payload, ie_len);
        if (mesh_conf_ie != OSAL_NULL) {
            *need_report_ie_len += mesh_conf_ie[1] + MAC_IE_HDR_LEN;
        }
        /* Mesh 私有标识字段长度 */
        td_u8 *ext_beacon_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_BEACON, ie_payload, ie_len);
        td_u8 *td_prb_rsp_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_RSP, ie_payload, ie_len);
        if (ext_beacon_ie != OSAL_NULL) {
            *need_report_ie_len += ext_beacon_ie[1] + MAC_IE_HDR_LEN;
        } else if (td_prb_rsp_ie != OSAL_NULL) {
            *need_report_ie_len += td_prb_rsp_ie[1] + MAC_IE_HDR_LEN;
        }

        /* Mesh Optimization域长度 */
        td_u8 *td_opt_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_OPTIMIZATION, ie_payload, ie_len);
        if (td_opt_ie != OSAL_NULL) {
            *need_report_ie_len += td_opt_ie[1] + MAC_IE_HDR_LEN;
        }

        /* Mesh ssid len */
        td_u8 *meshid_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_MESHID, ie_payload, ie_len);
        /* 查找meshid的ie */
        if ((meshid_ie != OSAL_NULL) && (meshid_ie[MAC_MESH_MESHID_LEN_POS] < WLAN_MESHID_MAX_LEN)) {
            /* 获取ssid ie的长度 */
            *need_report_ie_len += meshid_ie[1] + MAC_IE_HDR_LEN;
        }

        /* Mesh Trigger IE len */
        td_u8 *td_mesh_trigger_ie = mac_find_mesh_vendor_ie(MAC_OUISUBTYPE_MESH_TRIGGER, ie_payload, ie_len);
        *need_report_ie_len += (td_mesh_trigger_ie != OSAL_NULL) ? td_mesh_trigger_ie[1] + MAC_IE_HDR_LEN : 0;
    }
#endif
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

