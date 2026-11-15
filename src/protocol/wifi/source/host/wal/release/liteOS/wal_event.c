/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Receive the message reported by the driver and report it to the kernel.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_main.h"
#include "wal_scan.h"
#include "wal_ioctl.h"
#include "wal_ccpriv.h"
#include "alg_tpc.h"
#include "hmac_power.h"
#include "mac_vap_ext.h"
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#include "wal_cfg80211.h"
#include "wal_cfg80211_apt.h"
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif

#ifdef _PRE_WLAN_FEATURE_WPA3
#include "hmac_mgmt_sta.h"
#endif
#include "hmac_feature_dft.h"
#ifdef _PRE_WLAN_FEATURE_11D
#include "wal_11d.h"
#endif
#include "msg_alg_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_EVENT_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
td_u16 g_mac_status_code = 0xffff;
td_u16 g_mac_reason_code = 0xffff;

uapi_wifi_report_tx_params_callback g_wal_report_tx_params_callback = OSAL_NULL;
/*****************************************************************************
  3 函数实现
*****************************************************************************/
td_u16 *wal_get_mac_status_code(td_void)
{
    return &g_mac_status_code;
}

td_u16 *wal_get_mac_reason_code(td_void)
{
    return &g_mac_reason_code;
}

#ifdef _PRE_WLAN_FEATURE_WS92_MERGE
OAL_STATIC td_u32 wal_scan_result_clean_timeout_fn(td_void *arg)
{
    td_u32 ret;
    td_u32 expire_time = 25000; /* 25000：驱动扫描老化时间25s */
    oal_net_device_stru *netdev = (oal_net_device_stru *)arg;

    ret = (osal_u32)wal_async_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CLEAN_SCAN_RESULT,
        (osal_u8 *)&expire_time, OAL_SIZEOF(expire_time), FRW_POST_PRI_LOW);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_scan_result_clean_timeout_fn::return err code [%u]!}\r\n", ret);
    }
    return ret;
}
#endif

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 函 数 名  : wal_scan_comp_proc_sta
 功能描述  : STA上报扫描完成事件处理
*****************************************************************************/
osal_s32  wal_scan_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
#define WLAN_SCANRESULT_CLEAN_TIME         90000        /* 扫描结果老化时间90s */

    oal_net_device_stru *netdev = OSAL_NULL;
    /* 获取hmac vap结构体 */
    if (hmac_vap->net_device == OSAL_NULL) {
        oam_warning_log0(hmac_vap->vap_id, OAM_SF_SCAN, "{wal_scan_comp_proc_sta::param is NULL!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    netdev = hmac_vap->net_device;

    /* 删除等待扫描超时定时器 */
    if (hmac_vap->scan_timeout.is_registerd == OSAL_TRUE) {
        frw_destroy_timer_entry(&(hmac_vap->scan_timeout));
    }

    /* 获取hmac device 指针 */
    hmac_device_stru *hmac_dev  = hmac_res_get_mac_dev_etc(0);
    hmac_scan_stru   *scan_mgmt = &(hmac_dev->scan_mgmt);

    /* 无论是否是ANY扫描，扫描完成时需要恢复标志为当前非ANY扫描 */
    scan_mgmt->scan_record_mgmt.is_any_scan = OSAL_FALSE;

    /* 获取扫描结果的管理结构地址 */
    hmac_bss_mgmt_stru *bss_mgmt = &(hmac_dev->scan_mgmt.scan_record_mgmt.bss_mgmt);

    /* 获取驱动上报的扫描结果结构体指针 */
    hmac_scan_rsp_stru *scan_rsp = (hmac_scan_rsp_stru *)msg->data;

    /* 如果扫描返回结果的非成功，打印维测信息 */
    if (scan_rsp->result_code != MAC_SCAN_SUCCESS) {
        oam_warning_log1(hmac_vap->vap_id, OAM_SF_SCAN, "wal_scan_comp_proc_sta:Err=%d", scan_rsp->result_code);
    }

    /* 扫描成功时上报所有扫描到的bss */
    wal_inform_all_bss(netdev, bss_mgmt, hmac_vap->vap_id);

    /* 对于内核下发的扫描request资源加锁 */
#ifndef FREERTOS_DEFINE
    /* Freertos中, osal_kthreak_lock会关闭任务调度, 但后面wpa_supplicant有获取mutex锁，导致ASSERT异常 */
    osal_kthread_lock();
#endif

    /* 上层下发的普通扫描进行对应处理 */
    if (scan_mgmt->request != OSAL_NULL)  {
        /* 通知 kernel scan 已经结束 */
        cfg80211_scan_done(netdev, EXT_SCAN_SUCCESS);
        wal_free_scan_mgmt_resource(scan_mgmt);
        scan_mgmt->complete = OSAL_TRUE;
    }

    /* 通知完内核，释放资源后解锁 */
#ifndef FREERTOS_DEFINE
    osal_kthread_unlock();
#endif

#ifdef _PRE_WLAN_FEATURE_WS92_MERGE
    frw_create_timer_entry(&(hmac_vap->scanresult_clean_timeout),
                           wal_scan_result_clean_timeout_fn,
                           WLAN_SCANRESULT_CLEAN_TIME,
                           (void *)netdev,
                           OAL_FALSE);
#endif

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11D
/* sta关联成功后，更新自己的管制域信息 */
static osal_u32 wal_regdomain_update_sta(osal_u8 vap_id)
{
    osal_s8 *desired_country;
    oal_net_device_stru           *net_dev;
    td_u32                      ret;
    oal_bool_enum_uint8            updata_rd_by_ie_switch;
    /* 是否打开自动更新国家码开关 */
    hmac_vap_get_updata_rd_by_ie_switch_etc(vap_id, &updata_rd_by_ie_switch);

    if (updata_rd_by_ie_switch == OAL_FALSE) {
        oam_warning_log1(0, OAM_SF_ANY,
            "vap_id[%d] {wal_regdomain_update_sta::updata_rd_by_ie_switch is OAL_FALSE!}", vap_id);
        return OAL_FAIL;
    }
    desired_country = hmac_vap_get_desired_country_etc(vap_id);
    if (OAL_UNLIKELY(desired_country == OAL_PTR_NULL)) {
        oam_error_log1(0, OAM_SF_ANY, "vap_id[%d] {wal_regdomain_update_sta::desired_country is null ptr!}",
                       vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 期望的国家码全为0，表示对端AP的国家码不存在，采用sta当前默认的国家码 */
    if ((desired_country[0] == 0) && (desired_country[1] == 0)) {
        oam_warning_log1(0, OAM_SF_ANY,
            "vap_id[%d] {wal_regdomain_update_sta::ap does not have country ie, use default!}", vap_id);
        return OAL_SUCC;
    }

    net_dev = hmac_vap_get_net_device_etc(vap_id);
    if (OAL_UNLIKELY(net_dev == OAL_PTR_NULL)) {
        oam_error_log1(0, OAM_SF_ANY, "vap_id[%d] {wal_regdomain_update_sta::net_dev is null ptr!}",
                       vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 配置国家码 */
    ret = (osal_u32)wal_util_set_country_code(net_dev, desired_country);
    if (ret != OAL_SUCC) {
        oam_warning_log2(0, OAM_SF_ANY,
            "vap_id[%d] {wal_regdomain_update_sta_etc::wal_util_set_country_code err code %d!}", vap_id, ret);
        return OAL_FAIL;
    }
    oam_warning_log3(0, OAM_SF_ANY, "vap_id[%d] {wal_util_set_country_code::country is %u, %u!}", vap_id,
        (osal_u8)desired_country[0], (osal_u8)desired_country[1]);
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : wal_asoc_comp_proc_sta
 功能描述  : STA上报关联完成事件处理
*****************************************************************************/
osal_s32  wal_asoc_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_connet_result_stru       connet_result = {0};
    oal_net_device_stru         *netdev = OSAL_NULL;
    hmac_asoc_rsp_stru          *asoc_rsp = OSAL_NULL;
    asoc_rsp  = (hmac_asoc_rsp_stru *)msg->data;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (netdev == OSAL_NULL) {
        free(asoc_rsp->asoc_rsp_ie_buff);
        asoc_rsp->asoc_rsp_ie_buff = OSAL_NULL;
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC,
            "{wal_asoc_comp_proc_sta::get net device ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 准备上报内核的关联结果结构体 */
    if (memcpy_s(connet_result.bssid, WLAN_MAC_ADDR_LEN,
                 asoc_rsp->addr_ap, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::mem safe function err!}");
        free(asoc_rsp->asoc_rsp_ie_buff);
        asoc_rsp->asoc_rsp_ie_buff = OSAL_NULL;
        return OAL_FAIL;
    }
    connet_result.req_ie       = asoc_rsp->asoc_req_ie_buff;
    connet_result.req_ie_len    = asoc_rsp->asoc_req_ie_len;
    connet_result.rsp_ie       = asoc_rsp->asoc_rsp_ie_buff;
    connet_result.rsp_ie_len    = asoc_rsp->asoc_rsp_ie_len;
    connet_result.status_code   = asoc_rsp->status_code;
    connet_result.freq          = asoc_rsp->freq;
    connet_result.connect_status = asoc_rsp->result_code;
    /* 调用内核接口，上报关联结果 */
    cfg80211_connect_result(netdev, &connet_result);
    /* 释放关联管理帧内存 */
    free(asoc_rsp->asoc_rsp_ie_buff);
    asoc_rsp->asoc_rsp_ie_buff = OSAL_NULL;
    g_mac_status_code = (osal_u16)asoc_rsp->status_code;
#ifdef _PRE_WLAN_FEATURE_11D
    /* 如果关联成功，sta根据AP的国家码设置自己的管制域 */
    if (asoc_rsp->status_code == HMAC_MGMT_SUCCESS) {
        wal_regdomain_update_sta(hmac_vap->vap_id);
    }
#endif
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_disasoc_comp_event_proc
 功能描述  : STA上报去关联完成事件处理
*****************************************************************************/
osal_s32  wal_disasoc_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_disconnect_result_stru   disconnect_result = {0};
    oal_net_device_stru         *netdev = OSAL_NULL;
    td_u16                      *pul_reason_code = OSAL_NULL;
    td_u16                       disass_reason_code;
    td_u32                       ret;
    td_u32                       code_max = MAC_REASON_MAX;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (netdev == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC,
            "{wal_disasoc_comp_proc_sta::get net device ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取去关联原因码指针 */
    pul_reason_code = (td_u16 *)msg->data;
    disass_reason_code = (*pul_reason_code);
    disass_reason_code = disass_reason_code & ~code_max;
    g_mac_reason_code = (*pul_reason_code) & 0x3fff;

    /* 准备上报内核的关联结果结构体 */
    disconnect_result.reason_code = disass_reason_code;

    /* 调用内核接口，上报去关联结果 */
    ret = cfg80211_disconnected(netdev, disconnect_result.reason_code, disconnect_result.pus_disconn_ie,
                                disconnect_result.disconn_ie_len);

    oam_warning_log3(hmac_vap->vap_id, OAM_SF_ASSOC,
        "{wal_disasoc_comp_proc_sta reason_code[%u] ,hmac_reason_code[%u] ret %d!}",
        disass_reason_code, (*pul_reason_code), ret); /* 右移16位 */
    return (osal_s32)ret;
}

/*****************************************************************************
 函 数 名  : wal_connect_new_sta_proc_ap
 功能描述  : 驱动上报内核bss网络中新加入了一个STA
*****************************************************************************/
osal_s32  wal_connect_new_sta_proc_ap(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    td_u8                 user_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    oal_station_info_stru station_info = {0};

    /* 获取net_device */
    oal_net_device_stru *netdev = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (netdev == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::netdev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 向内核标记填充了关联请求帧的ie信息 */

    station_info.filled |=  STATION_INFO_ASSOC_REQ_IES;

    hmac_asoc_user_req_ie_stru *asoc_user_req_info = (hmac_asoc_user_req_ie_stru *)(msg->data);
    station_info.assoc_req_ies = asoc_user_req_info->assoc_req_ie_buff;
    if (station_info.assoc_req_ies == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::asoc ie is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    station_info.assoc_req_ies_len = asoc_user_req_info->assoc_req_ie_len;

    /* 获取关联user mac addr */
    if (memcpy_s(user_mac_addr, WLAN_MAC_ADDR_LEN,
        (td_u8 *)asoc_user_req_info->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_connect_new_sta_proc_ap::mem safe function err!}");
        return OAL_FAIL;
    }

    /* 调用内核接口，上报STA关联结果 */
    td_u32 ret = cfg80211_new_sta(netdev, user_mac_addr, WLAN_MAC_ADDR_LEN, &station_info);
    if (ret != OAL_SUCC) {
        oam_warning_log1(hmac_vap->vap_id, OAM_SF_ASSOC,
            "{wal_connect_new_sta_proc_ap::oal_cfg80211_new_sta fail[%d]!}\r\n", ret);
        return (osal_s32)ret;
    }

    oam_warning_log4(hmac_vap->vap_id, OAM_SF_ASSOC,
        "{wal_connect_new_sta_proc_ap mac[%02X:%02X:%02X:%02X:XX:XX] OK!}\r\n",
        user_mac_addr[0], user_mac_addr[1], user_mac_addr[2], user_mac_addr[3]); /* 0/1/2/3 MAC地址位数 */

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_disconnect_sta_proc_ap
 功能描述  : 驱动上报内核bss网络中删除了一个STA
*****************************************************************************/
osal_s32  wal_disconnect_sta_proc_ap(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_net_device_stru       *netdev = OSAL_NULL;
    td_u8                      user_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    osal_s32                     ret;

    /* 获取net_device */
    netdev = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (netdev == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC,
            "{wal_disconnect_sta_proc_ap::get net device ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 获取去关联user mac addr */
    if (memcpy_s(user_mac_addr, WLAN_MAC_ADDR_LEN,
        (td_u8 *)msg->data, WLAN_MAC_ADDR_LEN) != EOK) { /* event->event_data: 可变数组 */
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_disconnect_sta_proc_ap::mem safe function err!}");
        return OAL_FAIL;
    }
    /* 调用内核接口，上报STA去关联结果 */
    ret = (osal_s32)cfg80211_del_sta(netdev, user_mac_addr, WLAN_MAC_ADDR_LEN);
    if (ret != OAL_SUCC) {
        oam_warning_log1(hmac_vap->vap_id, OAM_SF_ASSOC,
            "{wal_disconnect_sta_proc_ap::cfg80211_del_sta fail[%d]!}\r\n", ret);
        return ret;
    }

    oam_warning_log3(hmac_vap->vap_id, OAM_SF_ASSOC,
        "{wal_disconnect_sta_proc_ap mac[%x %x %x] OK!}\r\n",
        user_mac_addr[3], user_mac_addr[4], user_mac_addr[5]); /* 3 4 5 数组位数 */

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_send_mgmt_to_host
 功能描述  : 驱动上报内核接收到管理帧
*****************************************************************************/
osal_s32  wal_send_mgmt_to_host(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_net_device_stru          *netdev = OSAL_NULL;
    td_s32                     l_freq;
    td_u8                    *buf = OSAL_NULL;
    td_u16                    len;
    td_u8                    ret;
    hmac_rx_mgmt_event_stru      *mgmt_frame = OSAL_NULL;
    mgmt_frame  = (hmac_rx_mgmt_event_stru *)(msg->data);
    unref_param(hmac_vap);

    /* 获取net_device */
    netdev = oal_get_netdev_by_name((const osal_char *)mgmt_frame->ac_name);
    if (netdev == OSAL_NULL || netdev->ieee80211_ptr == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ANY, "{wal_send_mgmt_to_host::get net device ptr is null!}");
        /* 管理帧丢弃 */
        hmac_dft_print_drop_frame_info(THIS_FILE_ID, __LINE__, 1, OAL_PTR_NULL);
        free(mgmt_frame->buf);
        return OAL_ERR_CODE_PTR_NULL;
    }

    buf = mgmt_frame->buf;
    len  = mgmt_frame->len;
    l_freq  = mgmt_frame->l_freq;

    oam_warning_log2(0, OAM_SF_ANY, "{wal_send_mgmt_to_host::fc[0x%04x], if_type[%d]!}\r\n",
        ((oal_ieee80211_mgmt *)buf)->frame_control, netdev->ieee80211_ptr->iftype);
    /* 调用内核接口，上报接收到管理帧 */
    ret = cfg80211_rx_mgmt(netdev, l_freq, 0, buf, len);
    if (ret != OSAL_TRUE) {
        oam_warning_log2(hmac_vap->vap_id, OAM_SF_ANY, "{wal_send_mgmt_to_host::fc[0x%04x], if_type[%d]!}\r\n",
                         ((oal_ieee80211_mgmt *)buf)->frame_control, netdev->ieee80211_ptr->iftype);
        oam_warning_log3(hmac_vap->vap_id, OAM_SF_ANY,
            "{wal_send_mgmt_to_host::cfg80211_rx_mgmt_ext fail[%d]!len[%d], freq[%d]}\r\n", ret, len, l_freq);
        free(buf);
#ifdef _PRE_WLAN_DFT_STAT
        hmac_pkts_rx_stat(hmac_vap->vap_id, 1, RX_M_SEND_LAN_FAIL_PKTS);
#endif
        return OAL_FAIL;
    }
#ifdef _PRE_WLAN_DFT_STAT
    hmac_pkts_rx_stat(hmac_vap->vap_id, 1, RX_M_SEND_LAN_SUCC_PKTS);
#endif
    free(buf);
    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 函 数 名  : wal_p2p_listen_timeout
 功能描述  : HMAC上报监听超时
*****************************************************************************/
osal_s32 wal_p2p_listen_timeout(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_wireless_dev             *wdev               = OSAL_NULL;
    hmac_p2p_listen_expired_stru *p2p_listen_expired = OSAL_NULL;
    oal_ieee80211_channel_stru    listen_channel;
    td_u32                        ret;
    p2p_listen_expired = (hmac_p2p_listen_expired_stru *)(msg->data);

    wdev = p2p_listen_expired->wdev;
    listen_channel = p2p_listen_expired->st_listen_channel;
    /* 调用内核接口，上报报监听超时 */
    ret = cfg80211_cancel_remain_on_channel(wdev->netdev, listen_channel.center_freq);
    if (ret != OAL_SUCC) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_P2P, "{wal_p2p_listen_timeout!}");
        return ret;
    }

    return OAL_SUCC;
}
#endif

td_void wal_set_tpc_mode(td_u32 mode)
{
    mac_ioctl_alg_param_stru alg_param = {0};
    td_u32 ret, vap_idx;
    hmac_device_stru *hmac_device  = hmac_res_get_mac_dev_etc(0);

    alg_param.alg_cfg = MAC_ALG_CFG_TPC_MODE;
    alg_param.is_negtive = OSAL_FALSE;
    alg_param.value = mode;

    for (vap_idx = 0; vap_idx < hmac_device->vap_num; vap_idx++) {
        hmac_vap_stru *hmac_vap = mac_res_get_hmac_vap(hmac_device->vap_id[vap_idx]);
        if (osal_unlikely(hmac_vap == OSAL_NULL)) {
            oam_warning_log1(0, OAM_SF_TPC, "vap_id[%d] {hmac_vap null.}", hmac_device->vap_id[vap_idx]);
        } else {
            hmac_pow_set_vap_tx_power(hmac_vap, HAL_POW_SET_TYPE_REFRESH);
            break;
        }
    }

    /* 抛事件到wal层处理 */
    ret = (td_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_ALG_PARAM,
        (osal_u8 *)&alg_param, OAL_SIZEOF(alg_param));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_at_set_tpc::wal_sync_post2hmac_no_rsp return err code [%u]!}", ret);
        return;
    }

    return;
}

td_void wal_set_tpc_mode_add_3db(oal_bool_enum_uint8 flag, oal_bool_enum_uint8 under_ragulation_flag)
{
    alg_set_api_tpc_api_3db_mode(flag, under_ragulation_flag);
    return;
}

#ifdef _PRE_WLAN_FEATURE_WPA3
/*****************************************************************************
 函 数 名  : wal_trigger_external_auth_sta
 功能描述  : 驱动上报WPA,触发SAE认证
*****************************************************************************/
osal_s32 wal_trigger_external_auth_sta(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_external_auth_stru external_auth = {0};

    /* 获取net_device */
    oal_net_device_stru *netdev = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (netdev == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC, "{wal_trigger_external_auth_sta::netdev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_external_auth_stru *ext_auth = (hmac_external_auth_stru *)(msg->data);
    external_auth.ssid = ext_auth->ssid;
    if (external_auth.ssid == OSAL_NULL) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ASSOC, "{wal_trigger_external_auth_sta::ssid is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    external_auth.ssid_len = ext_auth->ssid_len;

    if (memcpy_s(external_auth.bssid, WLAN_MAC_ADDR_LEN, ext_auth->bssid, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_trigger_external_auth_sta::mem safe function err!}");
        free(ext_auth->ssid);
        ext_auth->ssid = OSAL_NULL;
        return OAL_FAIL;
    }
    if (memcpy_s(external_auth.pmkid, WLAN_PMKID_LEN, ext_auth->pmkid, WLAN_PMKID_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_trigger_external_auth_sta::mem safe function err!}");
        free(ext_auth->ssid);
        ext_auth->ssid = OSAL_NULL;
        return OAL_FAIL;
    }
    external_auth.key_mgmt_suite = ext_auth->key_mgmt_suite;
    external_auth.status         = ext_auth->status;

    if (external_auth.status == MAC_SUCCESSFUL_STATUSCODE) {
        external_auth.auth_action = EXT_EXTERNAL_AUTH_START;
    } else {
        external_auth.auth_action = EXT_EXTERNAL_AUTH_ABORT;
    }
    /* 调用内核接口，上报STA关联结果 */
    cfg80211_external_auth_req(netdev, &external_auth);

    oam_warning_log4(hmac_vap->vap_id, OAM_SF_ASSOC,
        "{wal_trigger_external_auth_sta mac[%02X:%02X:%02X:%02X:XX:XX] trigger WPA SAE Auth.}\r\n",
        /* 0/1/2/3 MAC地址位数 */
        external_auth.bssid[0], external_auth.bssid[1], external_auth.bssid[2], external_auth.bssid[3]);
    oam_warning_log1(hmac_vap->vap_id, OAM_SF_ASSOC, "{wal_trigger_external_auth_sta Action[%d]!}\r\n",
        external_auth.auth_action);
    free(ext_auth->ssid);
    ext_auth->ssid = OSAL_NULL;

    return OAL_SUCC;
}

osal_s32 wal_cfg80211_report_owe_info(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_net_device_stru *net_device = OAL_PTR_NULL;
    hmac_asoc_user_req_ie_stru *req_info = OAL_PTR_NULL;
    ext_update_dh_ie_stru owe_info = {0};

    /* 获取net_device */
    net_device = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (net_device == OAL_PTR_NULL) {
        oam_error_log1(0, OAM_SF_ASSOC, "vap_id[%d] {wal_cfg80211_report_owe_info::get net device ptr is null!}",
            hmac_vap->vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    req_info = (hmac_asoc_user_req_ie_stru *)(msg->data);
    if (memcpy_s(owe_info.peer, sizeof(owe_info.peer), (osal_u8 *)req_info->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_cfg80211_report_owe_info::memcpy_s peer error}");
        return OAL_FAIL;
    }
    owe_info.status = MAC_SUCCESSFUL_STATUSCODE;
    owe_info.ie = req_info->assoc_req_ie_buff;
    owe_info.ie_len = req_info->assoc_req_ie_len;
    oam_warning_log1(0, OAM_SF_ASSOC, "{wal_cfg80211_report_owe_info::report ie_len=%u}", owe_info.ie_len);
    cfg80211_owe_info(net_device, &owe_info);

    return OAL_SUCC;
}

#endif

osal_void wal_register_tx_params_callback(uapi_wifi_report_tx_params_callback func)
{
    g_wal_report_tx_params_callback = func;
}

osal_u32 wal_report_tx_params(mac_cfg_ar_tx_params_stru *ar_tx_params)
{
    ext_wifi_report_tx_params data = {0};

    if (ar_tx_params == OAL_PTR_NULL) {
        oam_error_log0(0, OAM_SF_TX, "{wal_report_tx_params::ar_tx_params is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    data.best_rate = ar_tx_params->tx_best_rate;

    if (g_wal_report_tx_params_callback == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return g_wal_report_tx_params_callback(&data);
}

osal_s32 wal_upload_rx_mgmt_frame(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    unref_param(hmac_vap);
    hmac_rx_mgmt_event_stru *mgmt_frame = (hmac_rx_mgmt_event_stru *)(msg->data);

    return wal_do_report_rx_mgmt_frame_cb(mgmt_frame);
}

osal_s32 wal_upload_rx_promis_frame(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    unref_param(hmac_vap);
    hmac_rx_mgmt_event_stru *frame = (hmac_rx_mgmt_event_stru *)(msg->data);

    return wal_do_upload_promis_frame_cb(frame);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
