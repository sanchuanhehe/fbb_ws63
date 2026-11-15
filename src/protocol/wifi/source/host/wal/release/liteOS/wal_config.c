/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2023. All rights reserved.
 * Description: WAL配置操作
 * Create: 2012年11月6日
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_types.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "wlan_types_common.h"
#include "hmac_chr.h"
#include "hmac_sr_sta.h"
#include "hmac_feature_dft.h"
#include "wlan_msg.h"

#include "mac_device_ext.h"
#include "mac_vap_ext.h"
#include "mac_resource_ext.h"
#include "wal_event_msg.h"
#include "hmac_resource.h"
#include "hmac_device.h"
#include "hmac_scan.h"
#include "hmac_ext_if.h"
#include "hmac_config.h"
#include "hmac_alg_config.h"
#include "hmac_wmm.h"
#include "wal_ext_if.h"
#include "wal_main.h"
#include "wal_net_bridge.h"
#include "hmac_sdp.h"

#if defined(_PRE_PRODUCT_ID_HOST)
#include "hmac_cali_mgmt.h"
#endif

#include "hmac_m2u.h"

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
#include "hmac_proxy_arp.h"
#endif
#ifdef _PRE_WLAN_FEATURE_FTM
#include "hmac_ftm.h"
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
#include "hmac_smps.h"
#endif
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
#include "hmac_opmode.h"
#endif
#include "plat_pm_wlan.h"
#include "hmac_sta_pm.h"
#include "hmac_beacon.h"
#include "wal_config.h"
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif
#include "hmac_psd.h"
#include "hmac_wmm.h"
#include "hmac_stat.h"
#include "hmac_power.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_CONFIG_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 静态函数声明 */
osal_u32 wal_send_cali_data_etc(oal_net_device_stru *net_dev);

osal_void wal_cfg_init(osal_void)
{
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_ADD_VAP, hmac_config_add_vap_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_START_VAP, hmac_config_start_vap_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_DEL_VAP, hmac_config_del_vap_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_DOWN_VAP, hmac_config_down_vap_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_MODE, hmac_config_set_mode_cb);

    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CURRENT_CHANEL, hmac_config_do_channel_off);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_CURRENT_CHANEL, hmac_config_get_freq_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_STATION_ID, hmac_config_set_mac_addr_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SSID, hmac_config_get_ssid_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SSID, hmac_config_set_ssid_etc);

    /* --start 无调用注册 */
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_BSS_TYPE, hmac_config_set_bss_type_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_BSS_TYPE, hmac_config_get_bss_type_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_BANDWIDTH, hmac_config_set_bandwidth);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_MODE, hmac_config_get_mode_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SHORTGI, hmac_config_get_shortgi20_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SHORTGI_FORTY, hmac_config_get_shortgi40_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SHORTGI_EIGHTY, hmac_config_get_shortgi80_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SHORTGI, hmac_config_set_shortgi20_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SHORTGI_FORTY, hmac_config_set_shortgi40_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SHORTGI_EIGHTY, hmac_config_set_shortgi80_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SHORT_PREAMBLE, hmac_config_set_shpreamble_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SHORT_PREAMBLE, hmac_config_get_shpreamble_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_PROT_MODE, hmac_config_set_prot_mode_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_PROT_MODE, hmac_config_get_prot_mode_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_AUTH_MODE, hmac_config_get_auth_mode_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_AUTH_MODE, hmac_config_set_auth_mode_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_NO_BEACON, hmac_config_set_nobeacon_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_MULTI_BSSID_ENABLE, hmac_config_multi_bssid_enable);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_NO_BEACON, hmac_config_get_nobeacon_etc);
#ifdef _PRE_WLAN_FEATURE_SMPS
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SMPS_MODE, hmac_config_set_smps_mode);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_DTIM_PERIOD, hmac_config_set_dtimperiod_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_DTIM_PERIOD, hmac_config_get_dtimperiod_etc);
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_AMPDU_TX_ON, hmac_config_set_ampdu_tx_on_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_AMSDU_AMPDU_SWITCH, hmac_config_amsdu_ampdu_switch_etc);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_REGDOMAIN_PWR, hmac_config_set_regdomain_pwr_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_REGDOMAIN_PWR, hmac_config_get_regdomain_pwr_etc);
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_REG_INFO, hmac_config_reg_info_etc);
#endif
#ifdef _PRE_WLAN_FEATURE_PNO_SCAN
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_START_SCHED_SCAN, hmac_cfg80211_start_sched_scan_etc);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_SET_MIB_BY_BW, hmac_config_set_mib_by_bw);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_KEY, hmac_config_11i_get_key_etc);

    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_CUSTOMIZE_IE, hmac_config_set_customize_ie);

    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_ASSOC_REQ_IE, hmac_config_get_assoc_req_ie_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_VENDOR_IE, hmac_config_set_vendor_ie);
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_WMM_SWITCH, hmac_config_open_wmm_cb);
#endif
#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_PROXYARP_EN, hmac_proxyarp_on);
#endif
#ifdef _PRE_WLAN_FEATURE_FTM
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_FTM_DBG, hmac_config_ftm_dbg);
#endif
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_OPMODE_NOTIFY, hmac_config_set_opmode_notify_etc);
#endif

#ifdef _PRE_WLAN_FEATURE_HS20
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_QOS_MAP, hmac_config_set_qos_map);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_HOST_DEV_INIT, hmac_config_host_dev_init_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_HOST_DEV_EXIT, hmac_config_host_dev_exit_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_MAX_USER, hmac_config_set_max_user_cb);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_STA_LIST, hmac_config_get_sta_list_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_DESTROY_VAP, hmac_vap_destroy_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_VENDOR_CMD_GET_CHANNEL_LIST, hmac_config_vendor_cmd_get_channel_list_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_REMOVE_APP_IE, hmac_config_remove_app_ie);
    /* --end 无调用注册 */
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_TX_POWER, hmac_config_set_txpower_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_TX_POWER, hmac_config_get_txpower_etc);
#ifdef _PRE_WLAN_DFT_STAT
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_START_SNIFFER_INFO, hmac_config_start_sniffer_info);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_SNIFFER_INFO, hmac_config_get_sniffer_info);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_USER_INFO, hmac_config_user_info_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_VAP_INFO, hmac_config_vap_info_etc);
#endif
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_RANDOM_MAC_OUI, hmac_config_set_random_mac_oui_etc);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_RANDOM_MAC_ADDR_SCAN, hmac_config_set_random_mac_addr_scan);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_KICK_USER, hmac_config_kick_user_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_COUNTRY, hmac_config_set_country_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_COUNTRY, hmac_config_get_country_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SCAN_ABORT, hmac_config_scan_abort_etc);
    /* 以下为内核cfg80211配置的命令 */
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_START_SCAN, hmac_cfg80211_start_scan_sta_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_START_CONNECT, hmac_config_connect_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_SET_CHANNEL, hmac_config_set_channel_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_CONFIG_BEACON, hmac_config_set_beacon_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_ADD_KEY, hmac_config_11i_add_key_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_REMOVE_KEY, hmac_config_11i_remove_key_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_SET_PMKSA, hmac_config_set_pmksa_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_DEL_PMKSA, hmac_config_del_pmksa_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_FLUSH_PMKSA, hmac_config_flush_pmksa_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_MGMT_TX, hmac_wpas_mgmt_tx_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_LINKLOSS_PARAMS, hmac_config_set_linkloss_params);
#ifdef _PRE_WLAN_FEATURE_P2P
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_REMAIN_ON_CHANNEL, hmac_config_remain_on_channel_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG80211_CANCEL_REMAIN_ON_CHANNEL, hmac_config_cancel_remain_on_channel_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_P2P_PS_OPS, hmac_config_set_p2p_ps_ops_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_P2P_PS_NOA, hmac_config_set_p2p_ps_noa_etc);
#endif

    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CFG_VAP_H2D, hmac_config_cfg_vap_h2d_etc);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_ALL_LOG_LEVEL, hmac_config_set_all_log_level_etc);
#endif
#ifdef _PRE_WLAN_FEATURE_WPA3
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_EXTERNAL_AUTH_STATUS,  hmac_config_external_auth);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_UPDATE_OWE_INFO, hmac_update_owe_info);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_USER_APP_IE,  hmac_config_set_user_app_ie_cb);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_SCAN_PARAMS,  hmac_config_set_scan_params_etc);
#ifdef _PRE_WLAN_FEATURE_WS92_MERGE
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CLEAN_SCAN_RESULT, hmac_scan_clean_result);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_WIFI_APP_SERVICE, hmac_scan_handle_app_service);
#endif
#ifdef _PRE_WLAN_FEATURE_11D
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_RD_IE_SWITCH,  hmac_config_set_rd_by_ie_switch_etc);
#endif
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_THRUPUT_TEST, hmac_config_set_thruput_test);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_SCAN_PARAM, hmac_config_set_scan_param);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_RESET_MAC_PHY, hmac_config_reset_mac_phy);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_ADJUST_TX_POWER, hmac_config_adjust_tx_power);
#ifdef _PRE_WLAN_FEATURE_DFX_CHR
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CHR_ERR_CODE, hmac_config_chr_err_code_test);
#endif
#ifdef _PRE_WLAN_DFT_STAT
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_HW_STAT_ENABLE, hmac_config_hw_stat_enable);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_HW_STAT, hmac_get_hw_stat);
#endif
#ifdef _PRE_WLAN_FEATURE_PK_MODE
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_PK_MODE_TH, hmac_config_pk_mode_th);
#endif
#ifdef _PRE_WLAN_DFR_STAT
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_RESET_HW, hmac_config_reset_hw);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_QUEUE_RESET, hmac_config_set_queue_reset);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_PERIOD_CHECK_TASK, hmac_config_set_period_task);
#endif
#ifdef _PRE_WLAN_DFT_STAT
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_VAP_AGGRE_ENABLE, hmac_config_aggre_info_enable);
#endif
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_WFA_CFG_AIFSN, hmac_config_wfa_cfg_aifsn);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_WFA_CFG_CW, hmac_config_wfa_cfg_cw);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_ADDBA_REQ, hmac_config_addba_req_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_DELBA_REQ, hmac_config_delba_req_etc);
#ifdef _PRE_WLAN_CFGID_DEBUG
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_2040_COEXISTENCE, hmac_config_set_2040_coext_support_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_DHCP_DEBUG, hmac_config_rx_dhcp_debug_etc);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_LOG_LEVEL, hmac_config_set_log_level);
#endif
#ifdef _PRE_WLAN_FEATURE_11AX
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_11AX_SOFTAP_PARAM, hmac_config_set_11ax_softap_param);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_RATE_POWER, hmac_config_set_rate_power_offset);
#ifdef _PRE_WLAN_DFT_STAT
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SERVICE_CONTROL_PARA, hmac_config_service_control_etc);
#endif
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_SUSPEND_MODE, hmac_config_set_suspend_mode);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_CAL_TONE, hmac_config_set_cal_tone);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_SET_STA_CONN_PARAS, hmac_config_set_sta_conn_paras);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_DATA_FRAME_RSSI, hmac_config_get_data_frame_rssi);
}

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 功能描述  : 设置EDCA竞争窗口参数
*****************************************************************************/
osal_u32 wal_config_get_wmm_params_etc(oal_net_device_stru *net_dev, osal_u8 *param)
{
    hmac_vap_stru               *vap;

    vap = (hmac_vap_stru *)net_dev->ml_priv;
    if (OAL_UNLIKELY(vap == OAL_PTR_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_config_get_wmm_params_etc::net_dev->ml_priv is null ptr.}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return (osal_u32)hmac_config_get_wmm_params(vap, (hmac_config_wmm_para_stru *)param);
}

osal_u32 wal_send_cali_data_etc(oal_net_device_stru *net_dev)
{
    hmac_vap_stru               *mac_vap;

    mac_vap = (hmac_vap_stru *)net_dev->ml_priv;
    if (mac_vap != OAL_PTR_NULL) {
        hmac_send_cali_data_etc(mac_vap);
    } else {
        wifi_printf("wal_send_cali_data_etc:netdev name %s not have dev priv-vap", net_dev->name);
        oam_warning_log1(0, OAM_SF_CALIBRATE, "wal_send_cali_data_etc:netdev[%p] have no dev priv-vap", net_dev);

        return OAL_FAIL;
    }

    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif
