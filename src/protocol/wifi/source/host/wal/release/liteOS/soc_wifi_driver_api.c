/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: WAL layer external API interface implementation.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#include "wal_cfg80211_apt.h"
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/netifapi.h"
#endif // #ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#else
#include "soc_wifi_driver_wpa_if.h"
#endif
#include "hmac_ext_if.h"
#include "hmac_user.h"
#include "soc_wifi_api.h"
#include "soc_customize_wifi.h"
#include "wal_11d.h"
#include "wal_event_msg.h"
#include "wal_main.h"
#include "wal_ccpriv.h"
#include "wal_ioctl.h"
#include "wal_net.h"
#include "wal_event.h"

#include "soc_wifi_driver_api.h"

#include "wlan_mib_hcm.h"
#include "wlan_spec.h"
#include "plat_pm_wlan.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "hmac_main.h"
#include "hmac_alg_config.h"
#include "common_dft.h"
#include "wal_liteos_sdp.h"
#include "hmac_sdp.h"
#include "frw_common.h"
#include "hmac_psd.h"
#include "mac_vap_ext.h"
#include "hmac_config.h"
#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
#include "msg_wow_rom.h"
#endif
#include "msg_psm_rom.h"
#include "wal_scan.h"
#include "alg_main.h"
#ifdef _PRE_WLAN_FEATURE_WS73
#include "mac_addr.h"
#endif
#include "gpio_ext.h"
#ifdef _PRE_WLAN_FEATURE_POWERSAVE
#include "hmac_sta_pm.h"
#endif

#ifndef _PRE_WLAN_FEATURE_WS73
#include "main.h"
#endif

#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
#include "nv.h"
#endif

#ifdef _PRE_PRODUCT_ID_HOST
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "hmac_scan.h"
#endif
#endif
#ifdef _PRE_WLAN_TCP_OPT
#include "hmac_tcp_opt_struc.h"
#endif
#if defined(_PRE_WLAN_FEATURE_SINGLE_PROXYSTA) && defined(_PRE_WLAN_FEATURE_LOCAL_BRIDGE)
#include "hmac_single_proxysta.h"
#endif
#include "hmac_feature_dft.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WIFI_DRIVER_API_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#ifndef IFNAMSIZ
#define IFNAMSIZ  16
#endif

#define EXT_WIFI_INVALID_CHANNEL 0xFF
#define EXT_WIFI_CUSTOM_PKT_MIN_LEN 24
#define EXT_WIFI_CUSTOM_PKT_MAX_LEN 1400
#define EXT_WIFI_RETRY_MAX_NUM 15
#define EXT_WIFI_RETRY_MAX_TIME 200
#define EXT_WIFI_MAC_LEN 6

#define SCAN_CNT_DEFAULT                    2 /* 默认扫描次数 */
#define SCAN_TIME_DEFAULT                   20 /* 默认驻留时间 */
#define SCAN_CHANNEL_INTERVAL_DEFAULT       6 /* 默认n信道间隔 */
#define WORK_TIME_ON_HOME_CHANNEL_DEFAULT   110 /* 默认工作信道工作时间 */
#define SINGLE_PROBE_REQ_SEND_TIMES_DEFAULT 1 /* 默认单个probe req发送1次 */
#define ALL_SCAN_PARAM_DEFAULT_ENABLE       (WIFI_SCAN_CNT_DEFAULT_ENABLE | \
                                            WIFI_SCAN_TIME_DEFAULT_ENABLE | \
                                            WIFI_SCAN_CHANNEL_INTERVAL_DEFAULT_ENABLE | \
                                            WIFI_WORK_TIME_ON_HOME_CHANNEL_DEFAULT_ENABLE | \
                                            WIFI_SINGLE_PROBE_REQ_SEND_TIMES_DEFAULT_ENABLE)

#ifdef _PRE_PRODUCT_ID_HOST
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
/*****************************************************************************
 功能描述 : 初始化定制化ini文件host侧全局变量
*****************************************************************************/
wifi_csi_data_cb g_csi_data_func = 0;
wifi_psd_cb g_psd_data_func = NULL;

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
static void hwifi_cfg_mac_device_capability_param_init_nv_customize(void)
{
    uint8_t priv_value = 0;
    uint16_t priv_value_len = 0;
    uint32_t nv_ret = OAL_FAIL;
    mac_device_capability_stru *mac_device_capability = mac_device_get_capability();

    nv_ret = uapi_nv_read(NV_ID_SU_BFEE, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_mac_device_capability_param_init_nv_customize nv su_bfee[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        mac_device_capability[0].su_bfmee_is_supp = (priv_value == 0) ? 0 : 1;
    }

    nv_ret = uapi_nv_read(NV_ID_TX_LDPC, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_mac_device_capability_param_init_nv_customize nv ldpc_tx[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        mac_device_capability[0].ldpc_is_supp = (priv_value == 0) ? 0 : 1;
    }

    nv_ret = uapi_nv_read(NV_ID_RX_STBC, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_mac_device_capability_param_init_nv_customize nv stbc_rx[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        mac_device_capability[0].rx_stbc_is_supp = (priv_value == 0) ? 0 : 1;
    }
}

static void hwifi_cfg_ampdu_amsdu_param_init_nv_customize(void)
{
    uint8_t priv_value = 0;
    uint16_t priv_value_len = 0;
    uint32_t nv_ret = OAL_FAIL;

    nv_ret = uapi_nv_read(NV_ID_AMSDU_TX_NUM, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv amsdu_tx_num[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_amsdu_num(priv_value);
    }

    nv_ret = uapi_nv_read(NV_ID_AMSDU_TX_ON, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv amsdu_tx_on[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_amsdu_tx_active(priv_value);
    }

    nv_ret = uapi_nv_read(NV_ID_AMPDU_AMSDU_TX_ON, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv ampdu_amsdu_tx_on[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_ampdu_amsdu_tx_active(priv_value);
    }

    nv_ret = uapi_nv_read(NV_ID_AMPDU_TX_MAX_MPDU_NUM, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv ampdu_tx_max_mpdu_num[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_ampdu_tx_max_num(priv_value);
    }

    nv_ret = uapi_nv_read(NV_ID_AMPDU_RX_MAX_MPDU_NUM, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv ampdu_rx_max_mpdu_num[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_ampdu_rx_max_num(priv_value);
    }

    nv_ret = uapi_nv_read(NV_ID_AMPDU_TX_BAW_SIZE, sizeof(priv_value), &priv_value_len, &priv_value);
    wifi_printf("hwifi_cfg_ampdu_amsdu_param_init_nv_customize nv ampdu_tx_baw_size[%d]\r\n", priv_value);
    if (nv_ret == OAL_SUCC) {
        hwifi_set_ampdu_tx_baw_size(priv_value);
    }
    return;
}
#endif

static void hwifi_cfg_mac_device_capability_param_init(void)
{
    int32_t priv_value = 0;
    int32_t ret = OAL_FAIL;
    mac_device_capability_stru *mac_device_capability = mac_device_get_capability();

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_BW_MAX_WITH, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: bw max with[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].channel_width = (wlan_bw_cap_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SU_BFER, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: su bfer[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].su_bfmer_is_supp = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SU_BFEE, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: su bfee[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].su_bfmee_is_supp = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_MU_BFER, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: mu bfer[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].mu_bfmer_is_supp = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_MU_BFEE, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: mu bfee[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].mu_bfmee_is_supp = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_LDPC, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ldpc[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].ldpc_is_supp = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_BA_32BIT_BITMAP, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ba_32bitmap[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].ba_bitmap_support_32bit = (oal_bool_enum_uint8)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_MTID_AGGR_RX, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: mtid_aggr_rx[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].mtid_aggregation_rx_support = (uint8_t)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_RX_STBC, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: rx_stbc[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        mac_device_capability[0].rx_stbc_is_supp = (uint8_t)priv_value;
    }
}

#ifdef _PRE_WLAN_TCP_OPT
static void hwifi_cfg_tcp_ack_param_init(void)
{
    osal_s32 priv_value = 0;
    osal_s32 ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_TCP_ACK_FILTER_EN, &priv_value);
    wifi_printf("hwifi_cfg_tcp_ack_param_init: tcp_ack_filter_enable[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        g_st_tcp_ack_filter.tcp_ack_filter_en = priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_TCP_ACK_MAX_NUM_START_PROCESS, &priv_value);
    wifi_printf("hwifi_cfg_tcp_ack_param_init: tcp_ack_max_num_start_process[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        g_st_tcp_ack_filter.tcp_ack_max_num_start_process = priv_value;
    }
    return;
}
#endif

static void hwifi_cfg_frw_param_init(void)
{
    osal_s32 priv_value = 0;
    osal_s32 ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_TXDATA_QUE_LIMIT, &priv_value);
    if (ret == OAL_SUCC) {
        frw_set_data_queue_limit((osal_u32)priv_value);
    }
    return;
}

static void hwifi_cfg_ampdu_amsdu_param_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_AMSDU_NUM, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: amsdu num [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_amsdu_num((osal_u8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_AMSDU_TX_ON, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: amsdu tx on [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_amsdu_tx_active((osal_u8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_AMPDU_AMSDU_TX_ON, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ampdu amsdu tx on [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_ampdu_amsdu_tx_active((osal_u8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_INIT_AMPDU_TX_MAX_NUM, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ampdu tx max num [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_ampdu_tx_max_num((osal_u8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_INIT_AMPDU_RX_MAX_NUM, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ampdu rx max num [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_ampdu_rx_max_num((osal_u8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_INIT_AMPDU_TX_BAW_SIZE, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ampdu tx baw size [%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_ampdu_tx_baw_size((osal_u8)priv_value);
    }
    return;
}

static void hwifi_cfg_user_ap_num_param_init(void)
{
    int32_t priv_value = 0;
    int32_t ret = OAL_FAIL;
    uint8_t *user_num_addr = mac_get_user_res_num_addr();

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_USER_NUM, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: user num[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        *user_num_addr = (priv_value < 1) ? 1 :
            ((priv_value > WLAN_ASSOC_USER_MAX_NUM) ? WLAN_ASSOC_USER_MAX_NUM : (oal_bool_enum_uint8)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_HMAC_MAX_AP_NUM, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: hmac_max_ap_num[%d] ret[%d]\r\n", priv_value, ret);
    if (ret != OAL_SUCC || priv_value <= 0) {
        // 如果读取失败,使用默认值200进行初始化
        priv_value = 200;
    }
    hwifi_set_hmac_max_ap_num_etc((osal_u16)priv_value);
}

#ifdef _PRE_WLAN_DFR_STAT
static void  hwifi_cfg_self_healing_param_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SELF_HEALING_ENABLE, &priv_value);
    if (ret == OAL_SUCC) {
        hwifi_set_self_healing_enable_etc((uint32_t)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SELF_HEALING_PERIOD, &priv_value);
    if (ret == OAL_SUCC) {
        hwifi_set_self_healing_period_etc((uint32_t)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SELF_HEALING_CNT, &priv_value);
    if (ret == OAL_SUCC) {
        hwifi_set_self_healing_cnt_etc((uint32_t)priv_value);
    }
    return;
}
#endif

#ifdef _PRE_WLAN_FEATURE_DAQ
static void hwifi_cfg_data_sample_param_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u8 nv_data_sample;
    osal_u16 nv_data_sample_len = 0;
    osal_u32 nv_ret;
#endif
    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_DATA_SAMPLE, &priv_value);
    wifi_printf("hwifi_cfg_data_sample_param_init: data_sample[%d] ret[%d]\r\n", priv_value, ret);
    if (ret != OAL_SUCC) {
        wifi_printf("hwifi_cfg_data_sample_param_init: data_sample read fail ret[%d]\r\n", ret);
    }
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    nv_ret = uapi_nv_read(NV_ID_DATA_SAMPLE, sizeof(nv_data_sample), &nv_data_sample_len, &nv_data_sample);
    if (nv_ret == OAL_SUCC) {
        priv_value = nv_data_sample;
        wifi_printf("hwifi_cfg_data_sample_param_init nv data_sample[%d]\r\n", priv_value);
    }
#endif
    hwifi_set_data_sample((priv_value <= 0) ? OSAL_FALSE : OSAL_TRUE);
    return;
}
#endif

#ifdef _PRE_WLAN_FEATURE_APF
static void hwifi_cfg_apf_enable_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_APF_ENABLE, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: apf enable[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_apf_enable((priv_value <= 0) ? OSAL_FALSE : OSAL_TRUE);
    }
    return;
}
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
static void hwifi_cfg_wow_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_WOW_EVENT, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: wow_event[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_wow_event((uint32_t)priv_value);
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_WOW_ENABLE, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: wow_enable[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_wow_enable((uint8_t)priv_value);
    }

    return;
}
#endif

#ifdef _PRE_WLAN_SMOOTH_PHASE
static void hwifi_cfg_smooth_phase_en_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SMOOTH_PHASE_EN, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: smooth_phase_en[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_smooth_phase_en((priv_value == 0) ? OSAL_FALSE : OSAL_TRUE);
    }
    return;
}
#endif

static void hwifi_cfg_compatibility_er_su_th_init(void)
{
    osal_s32 priv_value = 0;
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    osal_u8  nv_priv_value = 0;
    osal_u16 priv_value_len = 0;
#endif

    if (hwifi_get_init_priv_value(WLAN_CFG_PRIV_COMPATIBILITY_ER_SU_TH, &priv_value) == OAL_SUCC) {
        hwifi_set_compatibility_er_su_th((osal_u8)priv_value);
    }

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    if (uapi_nv_read(NV_ID_ER_SU_FAIL_CNT_TH, sizeof(nv_priv_value), &priv_value_len, &nv_priv_value) == OAL_SUCC) {
        hwifi_set_compatibility_er_su_th(nv_priv_value);
    }
#endif
}

#ifdef _PRE_WLAN_FEATURE_ROAM
static void hwifi_cfg_roam_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    wlan_customize_stru *wlan_customize_etc = (wlan_customize_stru *)hwifi_wlan_customize_etc();

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_ROAM_TRIGGER_RSSI_2G, &priv_value);
    if (ret == OAL_SUCC) {
        wlan_customize_etc->c_roam_trigger_b = (int8_t)priv_value;
    }

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_ROAM_DELTA_RSSI_2G, &priv_value);
    if (ret == OAL_SUCC) {
        wlan_customize_etc->c_roam_delta_b = (int8_t)priv_value;
    }
#endif
#ifdef _PRE_WLAN_FEATURE_11R
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    osal_u8 nv_roam_over_ds;
    osal_u16 nv_roam_over_ds_len = 0;
    osal_u32 nv_ret;

    nv_ret = uapi_nv_read(NV_ID_ROAM_OVER_DS_ENABLE,
        sizeof(nv_roam_over_ds), &nv_roam_over_ds_len, &nv_roam_over_ds);
    if (nv_ret == OAL_SUCC) {
        priv_value = nv_roam_over_ds;
        wifi_printf("hwifi_cfg_roam_init nv over_ds_en[%d]\r\n", nv_roam_over_ds);
    }
#else
    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_OVER_DS_EN, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: over_ds_en[%d] ret[%d]\r\n", priv_value, ret);
#endif
    if (ret == OAL_SUCC) {
        mac_set_pst_mac_voe_custom_over_ds_param((priv_value == 0) ? OSAL_FALSE : OSAL_TRUE);
    }
#endif
    return;
}
#endif

#ifdef _PRE_WLAN_FEATURE_BTCOEX
static void hwifi_cfg_ext_coex_init(void)
{
    int32_t priv_value = 0;
    int32_t ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_EXT_COEX_EN, &priv_value);
    wifi_printf("hwifi_cfg_host_global_init_param: ext_coex_en[%d] ret[%d]\r\n", priv_value, ret);
    if (ret == OAL_SUCC) {
        hwifi_set_ext_coex_en((priv_value == 0) ? OSAL_FALSE : OSAL_TRUE);
    }
    return;
}
#endif

/*****************************************************************************
 功能描述      : 扫描probe req报文是否发送所有 ie字段
*****************************************************************************/
static void hwifi_cfg_scan_probe_all_ie_cfg_init(void)
{
    osal_s32 priv_value = 0;
    osal_s32 ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SCAN_PROBE_REQ_ALL_IE, &priv_value);
    wifi_printf("hwifi_cfg_scan_probe_ie_cfg_init ret %d priv_value %d\r\n", ret, priv_value);
    if (ret != OAL_SUCC || priv_value < 0) {
        /* 如果读取失败,使用默认值1进行初始化, 默认携带所有IE */
        priv_value = 1;
    }

    hmac_scan_set_probe_req_all_ie_cfg((osal_u8)priv_value);
    return;
}

/*****************************************************************************
 功能描述      : sta扫描probe req报文是否删除WPS ie字段
*****************************************************************************/
static void hwifi_cfg_scan_probe_del_wps_ie_cfg_init(void)
{
    osal_s32 priv_value = 0;
    osal_s32 ret;

    ret = hwifi_get_init_priv_value(WLAN_CFG_PRIV_SCAN_PROBE_REQ_DEL_WPS_IE, &priv_value);
    wifi_printf("hwifi_cfg_scan_probe_del_wps_ie_cfg_init ret %d priv_value %d\r\n", ret, priv_value);
    if (ret != OAL_SUCC || priv_value < 0) {
        /* 如果读取失败,使用默认值1进行初始化, 默认携带所有IE */
        priv_value = 1;
    }

    hmac_scan_set_probe_req_del_wps_ie_cfg((osal_u8)priv_value);

    return;
}

OAL_STATIC int32_t hwifi_cfg_host_global_init_param(void)
{
    hwifi_cfg_mac_device_capability_param_init();
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    hwifi_cfg_mac_device_capability_param_init_nv_customize();
#endif

#ifdef _PRE_WLAN_TCP_OPT
    hwifi_cfg_tcp_ack_param_init();
#endif
    hwifi_cfg_frw_param_init();

    /* ampdu/amsdu聚合相关定制化参数配置 */
    hwifi_cfg_ampdu_amsdu_param_init();
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    hwifi_cfg_ampdu_amsdu_param_init_nv_customize();
#endif

    hwifi_cfg_user_ap_num_param_init();

    hwifi_cfg_scan_probe_all_ie_cfg_init();
    hwifi_cfg_scan_probe_del_wps_ie_cfg_init();

#ifdef _PRE_WLAN_FEATURE_DAQ
    hwifi_cfg_data_sample_param_init();
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
    hwifi_cfg_wow_init();
#endif

#ifdef _PRE_WLAN_FEATURE_APF
    hwifi_cfg_apf_enable_init();
#endif

#ifdef _PRE_WLAN_DFR_STAT
    hwifi_cfg_self_healing_param_init();
#endif

#ifdef _PRE_WLAN_SMOOTH_PHASE
    hwifi_cfg_smooth_phase_en_init();
#endif

#ifdef _PRE_WLAN_FEATURE_ROAM
    hwifi_cfg_roam_init();
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    hwifi_cfg_ext_coex_init();
#endif
    hwifi_cfg_compatibility_er_su_th_init();
    return OAL_SUCC;
}

#endif /* _PRE_PLAT_FEATURE_CUSTOMIZE */
#if (!defined(_PRE_PRODUCT_ID_HIMPXX_DEV))
static void builder_module_exit(td_u16 bitmap)
{
#if (!defined(_PRE_PRODUCT_ID_HIMPXX_DEV))
    if ((BIT8 & bitmap) != 0) {
            wal_main_exit();
        }
    if ((BIT7 & bitmap) != 0) {
            hmac_main_exit_etc();
        }
    if ((BIT6 & bitmap) != 0) {
            frw_main_exit_etc();
        }
        wlan_close();
#elif (!defined(_PRE_PRODUCT_ID_HOST))
    if (BIT6 & bitmap) {
                alg_hmac_main_exit();
            }

    if (BIT5 & bitmap) {
            hmac_main_exit();
        }
    if (BIT4 & bitmap) {
            hal_hmac_main_exit();
        }

    platform_module_exit(bitmap);

#endif

    return;
}
#endif
td_u8 g_uc_custom_cali_done_etc = OAL_FALSE;
td_u8 uapi_get_custom_cali_done_etc(osal_void)
{
    return g_uc_custom_cali_done_etc;
}

#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
static osal_wait g_wait_device_ready;
static osal_s32 g_device_ready = OSAL_FALSE;
#define WAIT_DEVICE_TIME 20
#define WAIT_DEVICE_COUNT 500

osal_u32 device_init_ready(osal_u8 *cb_data)
{
    unref_param(cb_data);
    wifi_printf("=============device_init_ready===========\r\n");

    g_device_ready = OSAL_TRUE;
    osal_adapt_wait_wakeup(&g_wait_device_ready);
    return OAL_SUCC;
}

static osal_s32 device_is_ready(const osal_void *param)
{
    return g_device_ready;
}
#endif

OAL_STATIC osal_u32 host_module_wake_up_dev(void)
{
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    osal_u32 count_loop = 0;
#endif

#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
    /* host、device共核部署，直接初始化device */
    device_main_init();
    wifi_printf("host_module_wake_up_dev:: device_main_init finish!\r\n");
#else
    osal_wait_init(&g_wait_device_ready);
    do {
        td_s32 ret = hcc_send_message(HCC_CHANNEL_AP, H2D_MSG_WLAN_OPEN, HCC_ACTION_TYPE_TEST); /* can wakeup dev */
        if (ret != OAL_SUCC) {
            return  OAL_FAIL;
        }

        ret = osal_wait_timeout_interruptible(&g_wait_device_ready, device_is_ready, NULL, WAIT_DEVICE_TIME);
        if (ret > 0) {
            break;
        }
        count_loop++;
        /* 等待WAIT_DEVICE_COUNT*WAIT_DEVICE_TIME 10s时间后，认定device初始化失败(不理会device侧)直接启动host后续逻辑 */
        if (count_loop > WAIT_DEVICE_COUNT) {
            wifi_printf("wait device ready for 10s, timeout!\n");
            break;
        }
    } while (1);
#endif
    /* 开机校准和定制化参数下发 */
    if (hwifi_hcc_customize_h2d_data_cfg() != OAL_SUCC) {
        wifi_printf("host_module_wake_up_dev:: hwifi_hcc_customize_h2d_data_cfg fail.\r\n");
    }
    g_uc_custom_cali_done_etc = OAL_TRUE;
    return OAL_SUCC;
}

OAL_STATIC osal_s32 host_module_init_etc(void)
{
    osal_s32 ret = OAL_FAIL;
    osal_u16 bitmap = 0;
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    osal_msleep(1000); /* 53B060后C核初始化比A核慢，A核需要时延1000ms */
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    /* 读定制化配置文件&NVRAM */
    wal_customize_init();
    /* 配置host全局变量值 */
    hwifi_cfg_host_global_init_param();

#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

    ret = hmac_main_init_etc();
    if (ret != OAL_SUCC) {
        wifi_printf("host_module_init_etc: hmac_main_init_etc return error code:%d\r\n", ret);
        return ret;
    }
#ifdef _PRE_LWIP_ZERO_COPY
#ifndef _PRE_LWIP_ZERO_COPY_MALLOC_SKB
    oal_init_netbuf_stru();
#endif
#endif
    ret = (osal_s32)wal_main_init();
    if (ret != OAL_SUCC) {
        wifi_printf("host_module_init_etc:wal_main_init_etc return error code:%d\r\n", ret);
        bitmap = BIT7;
        builder_module_exit(bitmap);
        return ret;
    }

    ret = (osal_s32)host_module_wake_up_dev();
    if (ret != OAL_SUCC) {
        wifi_printf("host_module_init_etc:host_module_wake_up_dev return error code:%d\r\n", ret);
        return ret;
    }

    ret = hmac_main_init_later();
    if (ret != OAL_SUCC) {
        wifi_printf("host_module_init_etc:hmac_main_init_later return error code:%d\r\n", ret);
        return OAL_FAIL;
    }
    alg_hmac_sync_param();
    /* 待dmac ko初始化完成后在下发定制化参数 */
    hwifi_hcc_h2d_priv_customize();

    ret = (osal_s32)wal_customize_set_config();
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init::customize init failed [%d]!}", ret);
        return OAL_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_POWERSAVE
    hmac_sta_pm_ctrl_set(OSAL_TRUE);
#endif

    /* 启动完成后，输出打印 */
    wifi_printf("host_module_init_etc:: host_main_init finish!\r\n");

    return OAL_SUCC;
}
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/
td_u8 g_wifi_inited_flag = OSAL_FALSE;
/*****************************************************************************
 功能描述  : 初始化WiFi驱动
 输入参数  : vap_res_num : 驱动支持同时启动的vap数量, 范围[1,3]
             user_res_num: 驱动支持的最大接入用户数量,多vap时共用,范围[1-7]
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_init(const td_u8 vap_res_num, const td_u8 user_res_num)
{
    td_s32 ret = OAL_FAIL;

#ifdef _PRE_WLAN_FEATURE_POWERSAVE
    hal_pm_vote2platform(HAL_PM_WORK);
#endif
    if ((vap_res_num < 1 || vap_res_num > WLAN_SERVICE_VAP_NUM_PER_DEVICE) ||
        (user_res_num < 1 || user_res_num > WIFI_DEFAULT_MAX_NUM_STA)) {
        oam_error_log0(0, OAM_SF_ANY, "wifi initialize fail, vap/user num is wrong.");
        return OAL_FAIL;
    }

    if (g_wifi_inited_flag == OSAL_TRUE) {
        oam_error_log0(0, OAM_SF_ANY, "wifi have inited, donot inited again.");
        return OAL_FAIL;
    }

    wifi_set_init_start_time();
    if (uapi_wifi_plat_init() != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "wifi platform initialize fail.");
        return OAL_FAIL;
    }
    ret = host_module_init_etc();
    if (ret != OAL_SUCC) {
        uapi_wifi_plat_exit();
        oam_error_log0(0, OAM_SF_ANY, "wifi host initialize fail.");
        return OAL_FAIL;
    }

    g_wifi_inited_flag = OSAL_TRUE;

    return OAL_SUCC;
}

OAL_STATIC osal_u32 hmac_main_exit_later(osal_void)
{
    alg_hmac_main_exit();
    hmac_board_exit();
#ifndef _PRE_WLAN_FEATURE_WS63
    hal_hmac_main_exit();
#endif
    return OAL_SUCC;
}

OAL_STATIC void hmac_stop_all_device(void)
{
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    struct netif *netif = netifapi_netif_find_by_name("wlan0");
    if (netif != OSAL_NULL) {
        netifapi_dhcp_stop(netif);
        uapi_wifi_sta_stop();
    }
    netif = netifapi_netif_find_by_name("ap0");
    if (netif != OSAL_NULL) {
#if defined(LWIP_DHCPS) && LWIP_DHCPS
        netifapi_dhcps_stop(netif);
#endif
        uapi_wifi_softap_stop();
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    netif = netifapi_netif_find_by_name("p2p0");
    if (netif != OSAL_NULL) {
        netifapi_dhcp_stop(netif);
#if defined(LWIP_DHCPS) && LWIP_DHCPS
        netifapi_dhcps_stop(netif);
#endif
        uapi_wifi_p2p_stop();
    }
#endif
#else // _PRE_WLAN_USE_CUSTOM_LWIP
    uapi_wifi_sta_stop();
    uapi_wifi_softap_stop();
#ifdef _PRE_WLAN_FEATURE_P2P
    uapi_wifi_p2p_stop();
#endif
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
#endif
}
/*****************************************************************************
 功能描述  : 去初始化WiFi驱动
 返 回 值  : de错误码
*****************************************************************************/
td_s32 uapi_wifi_deinit(td_void)
{
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    td_s32 ret;
#endif
    if (g_wifi_inited_flag == OSAL_FALSE) {
        oam_error_log0(0, OAM_SF_ANY, "wifi have deinited or have not inited.");
        return OAL_FAIL;
    }
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    osal_adapt_wait_destroy(&g_wait_device_ready);
#endif

    hmac_stop_all_device();
    /* WIFI Host Exit */
    uapi_wifi_host_exit();
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
    device_main_exit();
#else
    ret = hcc_send_message(HCC_CHANNEL_AP, H2D_MSG_WLAN_CLOSE, HCC_ACTION_TYPE_TEST); /* close dev */
    if (ret != OAL_SUCC) {
        return  OAL_FAIL;
    }
#endif
    hmac_main_exit_later();
    /* WIFI Plat Exit */
    uapi_wifi_plat_exit();

    g_wifi_inited_flag = OSAL_FALSE;

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 获取当前WiFi驱动初始化状态
 返 回 值  : WiFi驱动初始化状态
*****************************************************************************/
td_u8 uapi_wifi_get_init_status(td_void)
{
    return g_wifi_inited_flag;
}

/*****************************************************************************
 功能描述  : 设置GPIO
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_set_gpio_val(td_s8 *param)
{
    td_s32 ret;
    gpio_param_stru *cfg = (gpio_param_stru *)param;
    oal_net_device_stru *netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);

    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_set_gpio_val: cfg device not fonud.");
        return OAL_FAIL;
    }

    ret = wal_async_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2D_C_CFG_SET_GPIO_VAL,
        (td_u8 *)cfg, sizeof(gpio_param_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_set_gpio_val::send msg failed[%d]!}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 获取关联ap的rssi值
 返 回 值  : rssi值
*****************************************************************************/
td_s32 uapi_wifi_sta_get_ap_rssi(td_void)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    osal_s8 signal_mgmt, signal_mgmt_data, signal;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sta_get_ap_rssi sta device not fonud.");
        return WLAN_RSSI_DUMMY_MARKER;
    }
    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_sta_get_ap_rssi::dev->ml_priv, return null!}");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    signal_mgmt = (osal_s8)oal_get_real_rssi(hmac_vap->query_stats.signal);
    signal_mgmt_data = (osal_s8)oal_get_real_rssi(hmac_vap->query_stats.signal_mgmt_data);
    signal = osal_max(signal_mgmt, signal_mgmt_data);
    /* 上报的rssi都要小于0，否则会导致界面显示的信号格数异常 */
    return (signal < 0) ? signal : -1;
}

/*****************************************************************************
 功能描述  : 获取关联ap的snr值
 返 回 值  : snr值
*****************************************************************************/
td_s32 uapi_wifi_sta_get_ap_snr(td_void)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    hmac_vap_stru *hmac_vap = OSAL_NULL;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sta_get_ap_snr sta device not fonud.");
        return WLAN_RSSI_DUMMY_MARKER;
    }
    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_sta_get_ap_snr::dev->ml_priv, return null!}");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    return hmac_vap->query_stats.snr_ant0;
}

/*****************************************************************************
 功能描述  : 获取softap关联的sta的rssi和rate值
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_ap_get_sta_info(td_char *mac_addr, td_s8 *rssi, td_u32 *best_rate)
{
    hmac_user_stru *hmac_user = OSAL_NULL;
    oal_net_device_stru *netdev = OSAL_NULL;
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    mac_cfg_query_rssi_stru user_info;
    frw_msg cfg_info;
    osal_s32 ret;

    netdev = oal_get_netdev_by_name("ap0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_ap_get_sta_info sta device not fonud.");
        return OAL_FAIL;
    }
    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_ap_get_sta_info::dev->ml_priv, return null!}");
        return OAL_FAIL;
    }

    hmac_user = mac_vap_get_hmac_user_by_addr_etc(hmac_vap, (osal_u8 *)mac_addr);
    if (hmac_user == OSAL_NULL) {
        oam_warning_log1(0, 0, "vap_id[%d] {uapi_wifi_ap_get_sta_info::hmac_user null.}", hmac_vap->vap_id);
        return OAL_FAIL;
    }

    (osal_void)memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    (osal_void)memset_s(&user_info, sizeof(mac_cfg_query_rssi_stru), 0, sizeof(mac_cfg_query_rssi_stru));
    cfg_msg_init((osal_u8 *)mac_addr, EXT_WIFI_MAC_LEN,
        (osal_u8 *)&user_info, OAL_SIZEOF(mac_cfg_query_rssi_stru), &cfg_info);
    ret = send_sync_cfg_to_host(hmac_vap->vap_id, WLAN_MSG_W2H_C_CFG_QUERY_RSSI, &cfg_info);
    if (ret != OAL_SUCC || cfg_info.rsp == OSAL_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_ap_get_sta_info::w2h msg fail, ret_code[%d]!}", ret);
        return OAL_FAIL;
    }

    *rssi = user_info.rssi;
    *best_rate = user_info.best_rate;
    return OAL_SUCC;
}
#endif

td_s32 uapi_wifi_sta_get_last_conn_vendor_ie(td_char *mac_addr, td_s8 *outbuffer, td_u16 out_buffer_len)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    frw_msg cfg_info;
    osal_s32 ret;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sta_get_last_conn_vendor_ie sta device not fonud.");
        return OAL_FAIL;
    }

    (osal_void)memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_msg_init((osal_u8 *)mac_addr, EXT_WIFI_MAC_LEN, (osal_u8 *)outbuffer, out_buffer_len, &cfg_info);
    ret = send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_GET_VENDOR_IE, &cfg_info);
    if (ret != OAL_SUCC || cfg_info.rsp == OSAL_NULL) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_sta_get_last_conn_vendor_ie::w2h msg fail, ret_code[%d]!}", ret);
        return OAL_FAIL;
    }

    if (memcpy_s(outbuffer, out_buffer_len, cfg_info.rsp, cfg_info.rsp_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_sta_get_last_conn_vendor_ie::memcpy_s error!}");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

td_s32 wal_psd_data_report(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    unref_param(hmac_vap);

    osal_u32 psd_data_len = msg->data_len;
    osal_s8 *psd_report_data = (osal_s8 *)msg->data;

    if (g_psd_data_func != NULL) {
        g_psd_data_func(psd_report_data, psd_data_len);
    }
    return OAL_SUCC;
}

td_void wal_csi_register_data_report_cb(wifi_csi_data_cb data_cb)
{
    g_csi_data_func = data_cb;
}

td_s32 wal_csi_data_report(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    unref_param(hmac_vap);
    osal_s32 csi_data_len = 0;
    osal_u8 *csi_report_data = NULL;
    osal_u32 *tmp;
    osal_u8 fcs_fail;
    osal_u8 rate;
    osal_u8 type;
    osal_u8 subtype;
    osal_u8 bw;
    osal_u16 rpt_csi_len;
    osal_u8 ltf_type;
    osal_u8 protocol;

    if (msg == NULL) {
        oam_error_log0(0, 0, "wal_csi_data_report:msg is NULL.");
        return OAL_FAIL;
    }

    csi_data_len = msg->data_len;
    csi_report_data = msg->data;
    tmp = (osal_u32*)(&csi_report_data[4]);                     // 需要打印的有效数据在WORD1 从byte4开始
    fcs_fail = (osal_u8)(((*tmp) >> 31) & 0b1);                 // CSI WORD1 bit31
    rate = (osal_u8)(((*tmp) >> 25) & 0b1111);                  // CSI WORD1 bit25-28
    type = (osal_u8)(((*tmp) >> 19) & 0b11);                    // CSI WORD1 bit19-20
    subtype = (osal_u8)(((*tmp) >> 21) & 0b1111);               // CSI WORD1 bit21-24
    bw = (osal_u8)(((*tmp) >> 15) & 0b1111);                    // CSI WORD1 bit15-18
    rpt_csi_len = (osal_u16)(((*tmp) >> 5) & 0b1111111111);     // CSI WORD1 bit5-15
    ltf_type = (osal_u8)(((*tmp) >> 3) & 0b11);                 // CSI WORD1 bit3-4
    protocol = (osal_u8)(((*tmp) >> 0) & 0b111);                // CSI WORD1 bit0-2

    oam_warning_log4(0, OAM_SF_ANY, "[csi]fcs_fail = %d, rate = 0x%x, type = %d, subtype = 0x%x",
        fcs_fail, rate, type, subtype);
    oam_warning_log4(0, OAM_SF_ANY, "[csi]bw = 0x%x, rpt_len = %d, ltf_type = %d, protocol = %d",
        bw, rpt_csi_len, ltf_type, protocol);
    dft_report_netbuf_cb_etc(csi_report_data, (osal_u16)csi_data_len, SOC_DIAG_MSG_ID_WIFI_RX_CSI);
    if (g_csi_data_func != NULL) {
        g_csi_data_func(csi_report_data, csi_data_len);
    }
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置国家码
 输入参数  : *cc 两个大写字符的国家码，数组大小至少3个字节，带字符串结束符
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_country(const td_char *cc, td_u8 cc_len)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    if (cc == OSAL_NULL) {
        oam_warning_log0(0, 0, "wifi_set_country parameter NULL.");
        return OAL_FAIL;
    }

    if (cc_len < MAC_CONTRY_CODE_LEN) {
        oam_warning_log0(0, 0, "wifi_set_country invalid country code length.");
        return OAL_FAIL;
    }
    /* 国家码两个字符 */
    if (osal_strlen(cc) != 2) { /* 2 国家码2个字符 */
        oam_warning_log0(0, 0, "wifi_set_country invalid country code.");
        return OAL_FAIL;
    }
    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, 0, "wifi_set_country Featureid0 device not fonud.");
        return OAL_FAIL;
    }

    if (wal_regdomain_update(netdev, cc, MAC_CONTRY_CODE_LEN) != OAL_SUCC) {
        oam_warning_log0(0, 0, "wifi_set_country regdomain update failed.");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 获取当前国家码
 输出参数  : [1]cc 两个大写字符的国家码，数组大小至少3个字节
             [2]len 返回字符串长度
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_get_country(td_char *cc, td_u8 *len)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    if ((cc == OSAL_NULL) || (len == OSAL_NULL)) {
        oam_warning_log0(0, 0, "wifi_get_country parameter NULL.");
        return OAL_FAIL;
    }
    if (*len < MAC_CONTRY_CODE_LEN) {
        oam_warning_log0(0, 0, "uapi_wifi_get_country invalid country code length.");
        return OAL_FAIL;
    }
    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, 0, "wifi_get_country Featureid0 device not fonud.");
        return OAL_FAIL;
    }
    if (uapi_ccpriv_getcountry(netdev, cc) != OAL_SUCC) {
        oam_warning_log0(0, 0, "wifi_get_country failed.");
        return OAL_FAIL;
    }

    *len = (td_u8)osal_strlen(cc);
    return OAL_SUCC;
}

#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
/*****************************************************************************
 功能描述  : 设置接口带宽
 输入参数  : [1]ifname
             [2]bw 带宽
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_bandwidth(const td_char *ifname, td_u8 ifname_len, ext_wifi_bw bw)
{
    td_char ac_bw[WAL_BW_STR_MAX_LEN] = {0};
    td_char ifname_cpy[IFNAMSIZ + 1] = {0};

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return OAL_FAIL;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name((td_char *)ifname_cpy);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_bandwidth device not fonud.");
        return OAL_FAIL;
    }
    if (bw != EXT_WIFI_BW_LEGACY_20M) {
        oam_error_log0(0, 0, "uapi_wifi_set_bandwidth invalid bw.");
        return OAL_FAIL;
    }

    strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "20");
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    struct netif *netif_value = netifapi_netif_find_by_name((td_char *)ifname_cpy);
    if (netif_value == OSAL_NULL) {
        oam_error_log0(0, 0, "wal_ccpriv_set_bw netifapi_netif_find return fail.");
        return OAL_FAIL;
    }

    td_u32 flags = netif_value->flags;   /* 记录设置前的netif状态设置后需要还原 */
    if ((flags & NETIF_FLAG_LINK_UP) != 0) {
        netifapi_netif_set_link_down(netif_value);
    }
    if ((flags & NETIF_FLAG_UP) != 0) {
        wal_netif_set_down(netif_value, netdev);
    }
#endif
    if (wal_ccpriv_set_bw(netdev, (td_char *)ac_bw) != OAL_SUCC) {
        return OAL_FAIL;
    }
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    if ((flags & NETIF_FLAG_UP) != 0) {
        wal_netif_set_up(netif_value, netdev);
    }

    if ((flags & NETIF_FLAG_LINK_UP) != 0 &&
        ((osal_strcmp((td_char *)ifname_cpy, "ap0") == 0) || (osal_strcmp((td_char *)ifname_cpy, "mesh0") == 0))) {
        netifapi_netif_set_link_up(netif_value);
    }
#endif
    return OAL_SUCC;
}
#else
td_s32 uapi_wifi_set_bandwidth(const td_char *ifname, td_u8 ifname_len, ext_wifi_bw bw)
{
    td_char ac_bw[WAL_BW_STR_MAX_LEN] = {0};
    td_char ifname_cpy[IFNAMSIZ + 1] = {0};

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return OAL_FAIL;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name((td_char *)ifname_cpy);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_bandwidth device not fonud.");
        return OAL_FAIL;
    }
    if (bw != EXT_WIFI_BW_LEGACY_20M) {
        oam_error_log0(0, 0, "uapi_wifi_set_bandwidth invalid bw.");
        return OAL_FAIL;
    }

    strcpy_s(ac_bw, WAL_BW_STR_MAX_LEN, "20");
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    wal_netif_set_down(netdev);
#endif
    if (wal_ccpriv_set_bw(netdev, (td_char *)ac_bw) != OAL_SUCC) {
        return OAL_FAIL;
    }
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    wal_netif_set_up(netdev);
#endif
    return OAL_SUCC;
}
#endif
/*****************************************************************************
 功能描述  : 获取接口带宽
 输入参数  : [1]ifname
 返 回 值  : ext_wifi_bw 带宽
*****************************************************************************/
ext_wifi_bw uapi_wifi_get_bandwidth(const td_char *ifname, td_u8 ifname_len)
{
    oal_net_device_stru              *netdev = OSAL_NULL;
    hal_channel_assemble_enum_uint8  bw_index  = WLAN_BAND_ASSEMBLE_20M;
    ext_wifi_bw                       bw           = EXT_WIFI_BW_BUTT;
    td_char ifname_cpy[IFNAMSIZ + 1] = {0};

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return EXT_WIFI_BW_BUTT;
    }
    netdev = oal_get_netdev_by_name((td_char *)ifname_cpy);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_get_bandwidth device not fonud.");
        return EXT_WIFI_BW_BUTT;
    }
    if (wal_ccpriv_get_bw(netdev, &bw_index) != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_wifi_get_bandwidth failed.");
        return EXT_WIFI_BW_BUTT;
    }
    if (bw_index == WLAN_BAND_ASSEMBLE_20M) {
        bw = EXT_WIFI_BW_LEGACY_20M;
    }

    return bw;
}

/*****************************************************************************
 功能描述  : 开启/关闭WiFi低功耗模式并配置预期休眠时间
 输入参数  : [1]enable     使能开关
             [2]sleep_time 预期休眠时间
 返 回 值  : 错误码
******************************************************************************/
td_s32 uapi_wifi_set_pm_switch(td_u8 enable, td_u32 sleep_time)
{
    oal_net_device_stru *net_dev = OSAL_NULL;
    td_s32 ret;
    mac_cfg_ps_open_stru sta_pm_open = {0};
    unref_param(sleep_time);
    if ((enable != MAC_STA_PM_DISABLE_FOREVER) && (enable >= MAC_STA_PM_SWITCH_BUTT)) {
        oam_warning_log1(0, 0, "{uapi_wifi_set_pm_switch::enable err [%u]!}\r\n", enable);
        return OAL_FAIL;
    }

    /* 获取STA VAP */
    net_dev = oal_get_netdev_by_type(NL80211_IFTYPE_STATION);
    if (net_dev == OSAL_NULL) {
        oam_warning_log0(0, 0, "{uapi_wifi_set_pm_switch::station vap not fonud.}");
        return OAL_FAIL;
    }

    /* 缓存低功耗标志 */
    set_under_ps(enable == MAC_STA_PM_SWITCH_ON);

    sta_pm_open.pm_enable      = enable;
    sta_pm_open.pm_ctrl_type   = MAC_STA_PM_CTRL_TYPE_HOST;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_STA_PM_ON,
        (osal_u8 *)&sta_pm_open, OAL_SIZEOF(mac_cfg_ps_open_stru));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, 0, "{uapi_wifi_set_pm_switch::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置STA的低功耗参数
 输入参数  : [1]pm_timeout 低功耗定时器周期
             [2]pm_timer_cnt 低功耗定时器重启次数
             [3]bcn_timeout beacon接收超时时间
             [4]mcast_timeout 组播广播接收超时时间
             [5]sleep_time 预期休眠时间
 输出参数  : 无
 返 回 值  : 0-成功 -1-失败
*****************************************************************************/
td_s32 uapi_wifi_sta_set_pm_param(td_u8 pm_timeout, td_u8 pm_timer_cnt, td_u8 bcn_timeout,
    td_u8 mcast_timeout, td_u16 sleep_time)
{
    td_s32 ret;
    mac_cfg_ps_param_stru pm_param;

    /* 仅支持wlan0配置 */
    oal_net_device_stru *netdev = oal_get_netdev_by_type(NL80211_IFTYPE_STATION);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sta_set_pm_param: wlan0 don't exist.");
        return OAL_FAIL;
    }

    pm_param.pm_timer = pm_timeout;
    pm_param.pm_timer_cnt = pm_timer_cnt;
    pm_param.beacon_timeout = bcn_timeout;
    pm_param.mcast_timeout = mcast_timeout;
    pm_param.sleep_time = sleep_time;
    /* 不改变tbtt */
    pm_param.tbtt_offset = 0;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_PSM_PARAM,
        (osal_u8 *)&pm_param, OAL_SIZEOF(mac_cfg_ps_param_stru));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, 0, "{uapi_wifi_sta_set_pm_param::return err code [%u]!}\r\n", ret);
        return ret;
    }
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_POWERSAVE
/*****************************************************************************
 功能描述  : 设置STA的外置tbtt offset值
 输入参数  : [1]pm_timeout 低功耗定时器周期
 输出参数  : 无
 返 回 值  : 0-成功 -1-失败
*****************************************************************************/
int uapi_wifi_sta_set_ext_offset(osal_u16 ext_tbtt_offset)
{
    osal_u32 ret;
    mac_cfg_ps_param_stru pm_param;

    /* 仅支持wlan0配置 */
    oal_net_device_stru *netdev = oal_get_netdev_by_type(NL80211_IFTYPE_STATION);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sta_set_ext_offset: wlan0 don't exist.");
        return OAL_FAIL;
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_EXT_TBTT_OFFSET,
        (osal_u8 *)&ext_tbtt_offset, OAL_SIZEOF(osal_u16));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, 0, "{uapi_wifi_sta_set_ext_offset::return err code [%u]!}\r\n", ret);
        return ret;
    }
    return OAL_SUCC;
}
#endif

td_s32 uapi_wifi_set_psd_enable(psd_option_param *psd_option)
{
    oal_net_device_stru *net_dev = OSAL_NULL;
    td_s32 ret;
    hmac_psd_enable_stru psd_enable_param;

    (osal_void)memset_s(&psd_enable_param, sizeof(hmac_psd_enable_stru), 0, sizeof(hmac_psd_enable_stru));

    psd_enable_param.channel = PSD_CENTER_CHAN;
    psd_enable_param.cycle = psd_option->cycle;
    psd_enable_param.duration = psd_option->duration;
    psd_enable_param.enable = psd_option->enable;
    /* 获取STA VAP */
    net_dev = oal_get_netdev_by_type(NL80211_IFTYPE_STATION);
    if (net_dev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_wifi_set_psd_enable::station vap not fonud.}");
        return OAL_FAIL;
    }

    ret = wal_async_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_PSD_ENABLE,
        (osal_u8 *)&psd_enable_param, OAL_SIZEOF(hmac_psd_enable_stru), FRW_POST_PRI_LOW);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, 0, "{uapi_wifi_set_psd_enable::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

td_s32 uapi_wifi_set_psd_cb(wifi_psd_cb data_cb)
{
    g_psd_data_func = data_cb;

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置Linkloss参数
 输入参数  : [1]probeRequestRatio     单播probe request帧开始发送时间比例 1~10
          [2]linklossThreshold    预期linkloss老化时间（100ms）
 返 回 值  : 错误码
******************************************************************************/
osal_s32 uapi_wifi_set_linkloss_paras(osal_u8 probe_request_ratio, osal_u16 linkloss_threshold)
{
    osal_s32 ret;
    wifi_linkloss_params linkloss_para = {0};

    linkloss_para.send_proberequest_ratio = probe_request_ratio;
    linkloss_para.linkloss_threshold = linkloss_threshold;

    /* 该配置不依赖VAP，任何时候都可以生效，采用配置vap传输消息 */
    ret = wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_SET_LINKLOSS_PARAMS,
        (osal_u8 *)&linkloss_para, OAL_SIZEOF(wifi_linkloss_params));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, 0, "{uapi_wifi_set_linkloss_paras::return err code [%u]!}\r\n", ret);
    }

    return ret;
}
#ifdef _PRE_WLAN_FEATURE_SDP
osal_s32 uapi_wifi_sdp_enable(const osal_char *ifname, osal_s32 enable, osal_s32 ratio)
{
    osal_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    oal_sdp_wakeup_info time_info = {0};

    if (ifname == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sdp_enable parameter NULL.");
        return OAL_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sdp_enable device not fonud.");
        return OAL_FAIL;
    }
    time_info.type = (osal_u8)enable;
    time_info.ratio = (osal_u8)ratio;

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SDP_TEST_SYNC_PRIVATE,
        (osal_u8 *)&time_info, sizeof(time_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_sdp_enable::return err code [%u]!}", ret);
    }

    return ret;
}

osal_s32 uapi_wifi_sdp_subscribe(const osal_char *ifname, osal_char *sdp_subscribe, osal_s32 local_handle)
{
    osal_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    oal_sdp_subscribe_info subscribe_info = {0};

    if (ifname == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sdp_subscribe parameter NULL.");
        return OAL_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_sdp_subscribe device not fonud.");
        return OAL_FAIL;
    }

    if (sdp_subscribe == OSAL_NULL) {
        subscribe_info.type = 4; /* 4 表示 WAL_SDP_TEST_TYPE_SUBSCRIBE_CANCEL */
    } else {
        subscribe_info.type = 3; /* 3 表示 WAL_SDP_TEST_TYPE_SUBSCRIBE_SERVICE */
        wal_sdp_srt_to_hex((osal_s8 *)sdp_subscribe, (osal_u8)osal_strlen(sdp_subscribe),
            (osal_s8 *)subscribe_info.name);
    }

    subscribe_info.local_handle = (osal_u8)local_handle;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SDP_TEST_SYNC_PRIVATE,
        (osal_u8 *)&subscribe_info, sizeof(subscribe_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_sdp_subscribe::return err code [%u]!}", ret);
    }

    return ret;
}
#endif

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 功能描述  : 使能混杂模式
 输入参数  : [1]ifname
             [2]enable
 返 回 值  : 错误码
*****************************************************************************/
osal_s32 uapi_wifi_promis_enable(const osal_char *ifname, osal_s32 enable, const ext_wifi_ptype_filter_stru *filter)
{
    osal_s32 ret;
    osal_u8 filter_value;
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_u8 msg_parma[2] = {0}; /* 2：表示msg参数有两个 */

    if ((ifname == OSAL_NULL) || (filter == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_promis_enable parameter NULL.");
        return OAL_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_promis_enable device not fonud.");
        return OAL_FAIL;
    }
    if ((enable != OSAL_FALSE) && (enable != OSAL_TRUE)) {
        oam_error_log0(0, 0, "uapi_wifi_promis_enable invalid parameter.");
        return OAL_FAIL;
    }
    filter_value = *((osal_u8 *)((osal_void *)filter)) & 0x1F;
    if (enable == OSAL_FALSE) {
        filter_value = 0;
    }

    msg_parma[0] = filter_value;
    msg_parma[1] = (osal_u8)enable;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_MONITOR_EN,
        (osal_u8 *)&msg_parma, sizeof(msg_parma));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_promis_enable::return err code [%u]!}", ret);
    }

    return ret;
}

/*****************************************************************************
 功能描述  : 注册混杂模式收包回调函数
 输入参数  : uapi_wifi_promis_cb data_cb 收包回调函数
 返 回 值  : 错误码
*****************************************************************************/
osal_s32 uapi_wifi_promis_set_rx_callback(wifi_promis_cb data_cb)
{
    wal_register_upload_frame_cb(data_cb);
    return OAL_SUCC;
}
/*****************************************************************************
 功能描述  : 设置信道
 输入参数  : [1]ifname
             [2]channel
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_channel(const td_char *ifname, td_u8 ifname_len, td_s32 channel)
{
    td_s32 ret;
    oal_net_device_stru    *netdev = OSAL_NULL;
    td_char                ifname_cpy[IFNAMSIZ + 1] = {0};

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return OAL_FAIL;
    }
    netdev = oal_get_netdev_by_name((td_char *)ifname_cpy);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_channel device not fonud.");
        return OAL_FAIL;
    }

    if ((channel > MAC_CHANNEL_FREQ_2_BUTT) || (channel <= 0)) {
        oam_error_log0(0, 0, "uapi_wifi_set_channel invalid channel.");
        return OAL_FAIL;
    }
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CURRENT_CHANEL,
        (osal_u8 *)&channel, OAL_SIZEOF(channel));
    if (ret != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_wifi_set_channel failed.");
        return ret;
    }

    return ret;
}

/*****************************************************************************
 功能描述  : 获取当前信道
 输入参数  : [1]ifname
 返 回 值  : 当前信道号
*****************************************************************************/
td_s32 uapi_wifi_get_channel(const td_char *ifname, td_u8 ifname_len)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    td_s32 ret;
    td_s32 channel;
    td_char ifname_cpy[IFNAMSIZ + 1] = {0};
    osal_s32  fill_msg;
    frw_msg    cfg_info;

    if (memcpy_s(ifname_cpy, sizeof(ifname_cpy), ifname, ifname_len) != EOK) {
        return EXT_WIFI_INVALID_CHANNEL;
    }
    netdev = oal_get_netdev_by_name((td_char *)ifname_cpy);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_get_channel device not fonud.");
        return EXT_WIFI_INVALID_CHANNEL;
    }

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = sizeof(fill_msg);
    cfg_info.rsp = (osal_u8 *)&fill_msg;

    ret = send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_CURRENT_CHANEL, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_get_channel::return err code %d!}", ret);
        return EXT_WIFI_INVALID_CHANNEL;
    }

    /* 处理返回消息 */
    channel = *((td_s32 *)(cfg_info.rsp));

    return channel;
}

/*****************************************************************************
 功能描述  : 发送用户定制的报文
 输入参数  : [1]ifname
             [2]data  : 报文内容
             [3]len  : 报文长度
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_send_custom_pkt(const char* ifname, const unsigned char *data, unsigned int len)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_u8 *pkt_data = OSAL_NULL;

    if (ifname == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_send_custom_pkt parameter NULL.");
        return OAL_FAIL;
    }

    /*
    * 申请内存存放用户信息，将内存指针作为事件payload抛下去
    * 此处申请的内存在事件处理函数释放(hmac_config_send_custom_pkt)
    */
    pkt_data = oal_mem_alloc(OAL_MGMT_NETBUF, (osal_u16)len, OAL_NETBUF_PRIORITY_MID);
    if (pkt_data == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_send_pkt::data fail(size:%u), return null!}", len);
        return OAL_FAIL;
    }
    (osal_void)memset_s(pkt_data, len, 0, len);
    (osal_void)memcpy_s(pkt_data, len, data, len);

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_send_custom_pkt get netdev fail}");
        return OAL_FAIL;
    }

    if (wal_send_custom_pkt(netdev, pkt_data, len) != OAL_SUCC) {
        oam_error_log0(0, 0, "uapi_wifi_send_custom_pkt send failed!.");
    }
    /* 释放用户侧数据内存 */
    oal_mem_free(pkt_data, OAL_TRUE);
    return OAL_SUCC;
}

td_u32 uapi_wifi_set_tx_pwr_offset(const td_char *ifname, td_s16 offset)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_u32 config_msg = 0;
    osal_s32 ret;

    if (ifname == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_tx_pwr_offset parameter NULL.");
        return OAL_FAIL;
    }

    netdev = oal_get_netdev_by_name(ifname);
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_set_tx_pwr_offset device not fonud.");
        return OAL_FAIL;
    }

    if ((offset < -100) || (offset > 40)) { /* 40:最大可配置的偏移是 40 -> 4dB,-100:最小可配置的偏移是-100 -> -10dB */
        return OAL_FAIL;
    }
    config_msg = ((WLAN_POW_RATE_POW_CODE_TABLE_2G_LEN - 1) << 16) + (osal_u8)offset; /* 16:左移16位 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev),
        WLAN_MSG_W2H_CFG_RATE_POWER, (osal_u8 *)&config_msg, OAL_SIZEOF(config_msg));
    if (ret != OAL_SUCC) {
        oam_error_log2(0, OAM_SF_ANY, "uapi_wifi_set_tx_pwr_offset msg:0x%x ret:%u.", config_msg, ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置接口发送功率上限
 输入参数  : *ifname 接口名
           power 发送功率
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_txpower_up_limit(const td_char *ifname, td_s32 power)
{
    td_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(ifname);
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_set_txpower_up_limit device not fonud.");
        return OAL_FAIL;
    }
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_TX_POWER,
        (osal_u8 *)&power, OAL_SIZEOF(power));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log1(0, 0, "uapi_wifi_set_txpower_up_limit failed, ret:%d.", ret);
        return ret;
    }
    return ret;
}

/*****************************************************************************
 功能描述  : 获取接口发送功率上限
 输入参数  : *ifname 接口名
 返 回 值  : 发送功率
*****************************************************************************/
td_s32 uapi_wifi_get_txpower_up_limit(const td_char *ifname)
{
    td_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_s32 txpower;

    netdev = oal_get_netdev_by_name(ifname);
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_get_txpower_up_limit device not fonud.");
        return 0;
    }
    frw_msg cfg_info;
    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = sizeof(txpower);
    cfg_info.rsp = (osal_u8 *)&txpower;
    ret = send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_TX_POWER, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_wifi_get_txpower_up_limit:: send_sync_cfg_to_host failed!}");
        return 0;
    }

    return txpower;
}
#endif

/*****************************************************************************
 功能描述  : 设置参数默认值
 输入参数  : [1]scan_param
 返 回 值  : 无
*****************************************************************************/
static td_void uapi_wifi_set_scan_param_default(ext_wifi_scan_param_stru *scan_param)
{
    td_u32 scan_param_map = 1;
    while ((scan_param_map < ALL_SCAN_PARAM_DEFAULT_ENABLE) && (scan_param_map > 0)) {
        switch (scan_param->default_enable & scan_param_map) {
            case WIFI_SCAN_CNT_DEFAULT_ENABLE:
                /* Default scan_cnt is 2. */
                scan_param->scan_cnt = (scan_param->scan_cnt == 0 ? SCAN_CNT_DEFAULT : scan_param->scan_cnt);
                break;
            case WIFI_SCAN_TIME_DEFAULT_ENABLE:
                /* Default scan_time is 20. */
                scan_param->scan_time = (scan_param->scan_time == 0 ? SCAN_TIME_DEFAULT : scan_param->scan_time);
                break;
            case WIFI_SCAN_CHANNEL_INTERVAL_DEFAULT_ENABLE:
                /* Default scan_channel_interval is 6. */
                scan_param->scan_channel_interval = (scan_param->scan_channel_interval == 0 ?
                    SCAN_CHANNEL_INTERVAL_DEFAULT : scan_param->scan_channel_interval);
                break;
            case WIFI_WORK_TIME_ON_HOME_CHANNEL_DEFAULT_ENABLE:
                /* Default work_time_on_home_channel is 110. */
                scan_param->work_time_on_home_channel = (scan_param->work_time_on_home_channel == 0 ?
                    WORK_TIME_ON_HOME_CHANNEL_DEFAULT : scan_param->work_time_on_home_channel);
                break;
            case WIFI_SINGLE_PROBE_REQ_SEND_TIMES_DEFAULT_ENABLE:
                /* Default single_probe_send_times is 1. */
                scan_param->single_probe_send_times = (scan_param->single_probe_send_times == 0 ?
                    SINGLE_PROBE_REQ_SEND_TIMES_DEFAULT : scan_param->single_probe_send_times);
                break;
            default:
                break;
        }
        scan_param_map = (scan_param_map << 1);
    }
}

static td_s32 uapi_wifi_update_scan_param(ext_wifi_scan_param_stru *scan_param)
{
    // scan_cnt = 2次, scan_time = 20秒，scan_channel_interval间隔6个信道，work_time_on_home_channel初始化为120秒
    static ext_wifi_scan_param_stru ext_wifi_scan_param = {0, 2, 20, 6, 120, 1, {0, 0, 0}};

    /* 设置配置命令参数 */
    /* 设置有效参数 default_enable:0B0000~0B1111 */
    if (scan_param->default_enable > ALL_SCAN_PARAM_DEFAULT_ENABLE) {
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: default_enable set failed.}");
        return OAL_FAIL;
    }

    /* 设置默认值 */
    if (scan_param->default_enable != 0) {
        uapi_wifi_set_scan_param_default(scan_param);
    }

    /* 设置配置命令参数 */
    if (scan_param->scan_cnt == 0) {
        scan_param->scan_cnt = ext_wifi_scan_param.scan_cnt;
    } else if (scan_param->scan_cnt < 1 || scan_param->scan_cnt > 10) { /* 设置有效参数 scan_cnt:0, 1~10 */
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: scan_cnt set failed.}");
        return OAL_FAIL;
    }

    if (scan_param->scan_time == 0) {
        scan_param->scan_time = ext_wifi_scan_param.scan_time;
    } else if (scan_param->scan_time < 20 || scan_param->scan_time > 120) { /* 设置有效参数 scan_time:0, 20~120 */
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: scan_time set failed.}");
        return OAL_FAIL;
    }

    if (scan_param->scan_channel_interval == 0) {
        scan_param->scan_channel_interval = ext_wifi_scan_param.scan_channel_interval;
    } else if (scan_param->scan_channel_interval < 1 ||
        scan_param->scan_channel_interval > 6) { /* 设置有效参数 scan_channel_interval:0, 1~6 */
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: scan_channel_interval set failed.}");
        return OAL_FAIL;
    }

    if (scan_param->work_time_on_home_channel == 0) {
        scan_param->work_time_on_home_channel = ext_wifi_scan_param.work_time_on_home_channel;
    } else if (scan_param->work_time_on_home_channel < 30 || /* 设置有效参数 work_time_on_home_channel:0, 30~120 */
        scan_param->work_time_on_home_channel > 120) { /* 设置有效参数 work_time_on_home_channel:0, 30~120 */
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: work_time_on_home_channel set failed.}");
        return OAL_FAIL;
    }

    if (scan_param->single_probe_send_times == 0) {
        scan_param->single_probe_send_times = ext_wifi_scan_param.single_probe_send_times;
    } else if (scan_param->single_probe_send_times < 1 || /* 设置有效参数 single_probe_send_times:0, 1~3 */
        scan_param->single_probe_send_times > 3) { /* 设置有效参数 single_probe_send_times:0, 1~3 */
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: single_probe_send_times set failed.}");
        return OAL_FAIL;
    }

    if (memcpy_s(&ext_wifi_scan_param, sizeof(ext_wifi_scan_param_stru),
        scan_param, sizeof(ext_wifi_scan_param_stru)) != EOK) {
        oam_error_log0(0, 0, "{uapi_wifi_update_scan_param memcpy_s failed.}");
    }
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置扫描次数
 输入参数  : [1]ifname
           [2]scan_param
 返 回 值  :  发送成功返回OAL_SUCC，否则返回其他值
*****************************************************************************/
td_s32 uapi_wifi_set_scan_param(const td_char *ifname, ext_wifi_scan_param_stru *scan_param)
{
    mac_cfg_scan_param_stru sta_scan_param;
    td_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;

    /* 判断网络设备是否存在 */
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param device not fonud.}");
        return OAL_FAIL;
    }

    if (scan_param == OSAL_NULL) {
        oam_error_log0(0, 0, "{uapi_wifi_set_scan_param:: scan_param is null}");
        return OAL_FAIL;
    }

    ret = uapi_wifi_update_scan_param(scan_param);
    if (ret != OAL_SUCC) {
        return OAL_FAIL;
    }

    sta_scan_param.scan_cnt = scan_param->scan_cnt;
    sta_scan_param.scan_time = scan_param->scan_time;
    sta_scan_param.scan_channel_interval = scan_param->scan_channel_interval;
    sta_scan_param.work_time_on_home_channel = scan_param->work_time_on_home_channel;
    sta_scan_param.single_probe_send_times = scan_param->single_probe_send_times;

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_SCAN_PARAMS,
        (osal_u8 *)&sta_scan_param, OAL_SIZEOF(mac_cfg_scan_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_wifi_set_scan_param::return err code %d!}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 停止扫描
 返 回 值  :  发送成功返回OAL_SUCC，否则返回其他值
*****************************************************************************/
td_u32 uapi_wifi_force_scan_complete(void)
{
    oal_net_device_stru            *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_force_scan_complete sta device not fonud.");
        return WLAN_RSSI_DUMMY_MARKER;
    }

    return wal_force_scan_complete(netdev);
}

/*****************************************************************************
 功能描述  : 立即清除扫描结果
 返 回 值  :  发送成功返回OAL_SUCC，否则返回其他值
*****************************************************************************/
td_u32 uapi_wifi_scan_results_clear(void)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    td_u32 ret;
    td_u32 expire_time = 0;

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_force_scan_complete sta device not fonud.");
        return OAL_FAIL;
    }

    ret = (osal_u32)wal_async_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CLEAN_SCAN_RESULT,
        (osal_u8 *)&expire_time, OAL_SIZEOF(expire_time), FRW_POST_PRI_LOW);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_scan_result_clean_timeout_fn::return err code [%u]!}\r\n", ret);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/*****************************************************************************
 输入参数  : [1] en 1-开启，0-关闭
             [2] ip ip地址，注意必须是32bit主机字节序
 返 回 值  : 成功返回0，失败返回-1

 修改历史      :
  1.日    期   : 2019年10月17日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 uapi_wifi_dhcp_offload_setting(const td_char *ifname, td_u8 en, td_u32 ip)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    mac_ip_addr_config_stru ip_addr_cfg = {0};

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_dhcp_offload_setting:: device not fonud.");
        return OAL_FAIL;
    }

    if (en >= MAC_IP_OPER_BUTT) {
        oam_error_log0(0, 0, "uapi_wifi_dhcp_offload_setting:: en error.");
        return OAL_FAIL;
    }

    ip_addr_cfg.type = MAC_CONFIG_IPV4;
    ip_addr_cfg.oper = en;
    ip_addr_cfg.ip.ipv4 = ip;
    /* 打印IPV4地址[2]和[3] */
    oam_info_log3(0, 0, "{uapi_wifi_dhcp_offload_setting:: oper: %d, ip[xx.xx.%d.%d] succ.}",
        en, *(((td_char *)&ip_addr_cfg.ip.ipv4) + 2), *(((td_char *)&ip_addr_cfg.ip.ipv4) + 3)); /* 2/3 偏置 */

    /***************************************************************************
         抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

td_u32 uapi_wifi_nd_offload_setting(const td_char *ifname, td_u8 en, td_u8 *ip6)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    mac_ip_addr_config_stru ip_addr_cfg = {0};

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_arp_offload_setting:: device not fonud.");
        return OAL_FAIL;
    }

    if (en >= MAC_IP_OPER_BUTT) {
        oam_error_log0(0, 0, "uapi_wifi_arp_offload_setting:: en error.");
        return OAL_FAIL;
    }

    ip_addr_cfg.type = MAC_CONFIG_IPV6;
    ip_addr_cfg.oper = en;
    if (memcpy_s(&ip_addr_cfg.ip.ipv6, OAL_IP_ADDR_MAX_SIZE, ip6, OAL_IP_ADDR_MAX_SIZE) != EOK) {
        return OAL_FAIL;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}
#endif

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
/*****************************************************************************
 输入参数  : [1]type 重传设置类型：1、设置数据帧重传 2、设置管理帧重传 3、设置时间重传
           [2]limit 重传限制：重传次数0~15，重传时间0~200
 输出参数  : 无
 返 回 值: 是否成功
*****************************************************************************/
td_u32 uapi_wifi_set_retry_params(const td_char *ifname, td_u8 type, td_u8 limit)
{
    oal_net_device_stru *netdev = OSAL_NULL;

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_retry_params:: device not fonud.");
        return OAL_FAIL;
    }

    if (type >= MAC_CFG_RETRY_TYPE_BUTT) {
        oam_error_log0(0, 0, "uapi_wifi_set_retry_params:: type is invaid.");
        return OAL_FAIL;
    }

    if (type == MAC_CFG_RETRY_DATA || type == MAC_CFG_RETRY_MGMT) {
        if (limit > EXT_WIFI_RETRY_MAX_NUM) {
            oam_error_log0(0, 0, "uapi_wifi_set_retry_params:: tpye is date or mgmt,limit is invaid.");
            return OAL_FAIL;
        }
    }

    if (type == MAC_CFG_RETRY_TIMEOUT) {
        if (limit > EXT_WIFI_RETRY_MAX_TIME) {
            oam_error_log0(0, 0, "uapi_wifi_set_retry_params:: tpye is timeout,limit is invaid.");
            return OAL_FAIL;
        }
    }
    return OAL_SUCC;
}

td_u32 uapi_wifi_set_cca_threshold(td_u8 mode, td_s8 threshold)
{
    mac_ioctl_alg_param_stru    alg_param = {0};
    mac_ioctl_alg_param_stru    alg_param_cca_disable = {0};
    osal_u32                      ret;
    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        wifi_printf("under ps mode,can not exec cmd");
        return OAL_FAIL;
    }

    if (threshold == MAC_API_AUTO_CCA) {
        alg_param.alg_cfg  = MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE;
        alg_param.value = WLAN_CCA_OPT_ENABLE;
        ret = (osal_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_ALG_PARAM,
            (osal_u8 *)&alg_param, OAL_SIZEOF(mac_ioctl_alg_param_stru));
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_set_cca_threshold::wal sync fail, ret:[%d]!}", ret);
            return ret;
        }
        return OAL_SUCC;
    }
    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    if (mode == 0) {
        alg_param.alg_cfg = MAC_ALG_CFG_CCA_OPT_SET_CCA_ED20;
    } else if (mode == 1) {
        alg_param.alg_cfg = MAC_ALG_CFG_CCA_OPT_SET_CCA_ED40;
    } else {
        wifi_printf("{uapi_wifi_set_cca_threshold:invalid mode.}");
        return OAL_FAIL;
    }
    alg_param.value = (osal_u32)threshold;

    /* 先关闭CCA算法，再设置阈值 */
    alg_param_cca_disable.alg_cfg  = MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE;
    alg_param_cca_disable.value = WLAN_CCA_OPT_DISABLE;
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_ALG_PARAM,
        (osal_u8 *)&alg_param_cca_disable, OAL_SIZEOF(mac_ioctl_alg_param_stru));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_set_cca_threshold::MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE wal sync fail, ret:[%d]!}", ret);
        return ret;
    }

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_ALG_PARAM,
        (osal_u8 *)&alg_param, OAL_SIZEOF(mac_ioctl_alg_param_stru));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_set_cca_threshold::WLAN_MSG_W2H_CFG_ALG_PARAM wal sync fail, ret:[%d]!}", ret);
        return ret;
    }
    return OAL_SUCC;
}
#endif

td_u32 uapi_wifi_set_tpc(td_u32 tpc_value)
{
    td_u32               mode;

    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        wifi_printf("under ps mode,can not exec cmd");
        return OAL_FAIL;
    }

    if (tpc_value > MAC_API_TPC_AUTO_MODE) {
        wifi_printf("{uapi_wifi_set_tpc::tpc_value invalide!}");
        return OAL_FAIL;
    }
    if (tpc_value == MAC_API_TPC_OFF_MODE) {
        mode = WLAN_TPC_WORK_MODE_DISABLE;
        wal_set_tpc_mode_add_3db(OAL_FALSE, OAL_TRUE);
        wal_set_tpc_mode(mode);
    } else if (tpc_value == MAC_API_TPC_AUTO_MODE) {
        mode = WLAN_TPC_WORK_MODE_ENABLE;
        wal_set_tpc_mode_add_3db(OAL_TRUE, OAL_TRUE);
        wal_set_tpc_mode(mode);
    } else {
        mode = WLAN_TPC_WORK_MODE_ENABLE;
        wal_set_tpc_mode_add_3db(OAL_TRUE, OAL_FALSE);
        wal_set_tpc_mode(mode);
    }

    return OAL_SUCC;
}

td_u32 uapi_wifi_set_rts_threshold(td_u8 mode, td_u16 pkt_length)
{
    mac_ioctl_alg_param_stru    alg_param = {0};
    mac_cfg_rts_threshold_stru  threshold = {0};
    td_u32                      ret;

    if (mode > MAC_API_RTS_OFF_MODE) {
        wifi_printf("uapi_wifi_set_rts_threshold:: parameter invalid.");
        return OAL_FAIL;
    }
    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    /* 0:自动模式（默认） 1:超过配置报文长度发送RTS,未超过不发 2:不发RTS. */
    if (mode == MAC_API_RTS_AUTO_MODE) {
        alg_param.alg_cfg = MAC_ALG_CFG_RTS_MODE;
        alg_param.value = ALG_RTS_MODE_RATE0_DYNAMIC;
    } else if (mode == MAC_API_RTS_OFF_MODE) {
        alg_param.alg_cfg = MAC_ALG_CFG_RTS_MODE;
        alg_param.value = ALG_RTS_MODE_ALL_DISABLE;
    } else {
        alg_param.alg_cfg = MAC_ALG_CFG_RTS_MODE;
        alg_param.value = ALG_RTS_MODE_THRESHOLD;
        threshold.rts_threshold = pkt_length;
    }
    ret = (td_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_ALG_PARAM,
        (osal_u8 *)&alg_param, OAL_SIZEOF(mac_ioctl_alg_param_stru));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_set_rts_threshold::wal sync fail, ret:[%d]!}", ret);
        return ret;
    }

    if (mode == 1) {
        ret = (td_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_SET_RTS_THRESHOLD,
            (osal_u8 *)&threshold, OAL_SIZEOF(threshold));
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_set_rts_threshold::wal sync fail, ret:[%d]!}", ret);
            return ret;
        }
    }
    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
OAL_STATIC td_u32 uapi_wifi_intrf_mode_process_second(td_u8 enable, mac_alg_intrf_mode_enum_uint8 intrf_mode,
    td_u16 cfg_flag, wlan_wme_ac_type_enum_uint8  *mode, mac_ioctl_alg_intrf_mode_param_stru  *intrf_mode_info)
{
    switch (intrf_mode) {
        case MAC_ALG_INTRF_11N:
            intrf_mode_info->alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_11N_SWITCH;
            intrf_mode_info->value = (enable == 0) ? 0 : ((cfg_flag & BIT4) != 0) ? 1 : 0;
            break;
        case MAC_ALG_INTRF_NO_11B:
            intrf_mode_info->alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_NO_11B_SWITCH;
            intrf_mode_info->value = (enable == 0) ? 0 : ((cfg_flag & BIT5) != 0) ? 1 : 0;
            break;
        case MAC_ALG_INTRF_LONG_RANGE:
            intrf_mode_info->alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_LONG_RANGE_INTRF_SWITCH;
            intrf_mode_info->value = (enable == 0) ? 0 : ((cfg_flag & BIT6) != 0) ? 1 : 0;
            break;
        case MAC_ALG_INTRF_VO:
            if (((cfg_flag & BIT1) != 0) && (enable == 1)) {
                *mode = WLAN_WME_AC_VO;
            } else {
                *mode = WLAN_WME_AC_BUTT;
            }
            break;
        default:
            wifi_printf("uapi_wifi_enable_intrf_mode_process:: parameter invalid.");
            return OAL_FAIL;
    }
    return OAL_SUCC;
}
OAL_STATIC osal_u32 uapi_wifi_enable_intrf_mode_process(oal_net_device_stru *netdev, osal_u8 enable,
    mac_alg_intrf_mode_enum_uint8 intrf_mode, osal_u16 cfg_flag)
{
    osal_u32                      ret = OAL_SUCC;
    mac_ioctl_alg_intrf_mode_param_stru  intrf_info = {0};
    wlan_wme_ac_type_enum_uint8  mode;

    switch (intrf_mode) {
        case MAC_ALG_INTRF_EDCA:
            intrf_info.alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_EDCA_SWITCH;
            intrf_info.value = (enable == 0) ? 0 : ((cfg_flag & BIT0) != 0) ? 1 : 0;
            break;
        case MAC_ALG_INTRF_CCA:
            intrf_info.alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_CCA_SWITCH;
            intrf_info.value = (enable == 0) ? 0 : ((cfg_flag & BIT2) != 0) ? 1 : 0;
            break;
        case MAC_ALG_INTRF_11B:
            intrf_info.alg_intrf_mode_cfg = MAC_ALG_INTRF_MODE_11B_SWITCH;
            intrf_info.value = (enable == 0) ? 0 : ((cfg_flag & BIT3) != 0) ? 1 : 0;
            break;
        default:
            if (uapi_wifi_intrf_mode_process_second(enable, intrf_mode, cfg_flag, &mode, &intrf_info) != OAL_SUCC) {
                wifi_printf("uapi_wifi_enable_intrf_mode_process:: error");
                return OAL_FAIL;
            }
    }
    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    if (intrf_mode == MAC_ALG_INTRF_VO) { /* VO队列消息 */
        ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_AC_MODE,
            (osal_u8 *)&mode, OAL_SIZEOF(mode));
    } else {
        ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_ALG_INTRF_MODE_CFG,
            (osal_u8 *)&intrf_info, OAL_SIZEOF(intrf_info));
    }
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_enable_intrf_mode_process::wal_sync_post2hmac_no_rsp err [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

td_u32 uapi_wifi_enable_intrf_mode(const td_char *ifname, td_u8 enable, td_u16 flag)
{
    oal_net_device_stru        *netdev = OSAL_NULL;
    mac_ioctl_alg_intrf_mode_todmac_stru intrf_dmac = {0};
    osal_u32                      ret;
    mac_alg_intrf_mode_enum_uint8 index;

    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        wifi_printf("uapi_wifi_enable_intrf_mode:: device not fonud.");
        return OAL_FAIL;
    }
    if (enable != 0 && enable != 1) {
        wifi_printf("uapi_wifi_enable_intrf_mode:: parameter invalid.");
        return OAL_FAIL;
    }
    intrf_dmac.enable = enable;
    intrf_dmac.value = flag;
    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    for (index = 0; index < MAC_ALG_INTRF_BUTT; index++) {
        ret = uapi_wifi_enable_intrf_mode_process(netdev, enable, index, flag);
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_enable_intrf_mode::intrf_mode_process err [%d]!, index [%d]\r\n}",
                ret, index);
            return ret;
        }
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_ALG_INTRF_MODE_DMAC_CFG,
        (osal_u8 *)&intrf_dmac, OAL_SIZEOF(intrf_dmac));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_enable_intrf_mode::intrf_mode_process INTRF_MODE_DMAC err [%d]!\r\n}",
            ret, index);
        return ret;
    }
    return OAL_SUCC;
}
#endif

td_s32 uapi_wifi_mac_phy_reset(td_void)
{
    osal_s32 ret;

    ret = wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_RESET_MAC_PHY, OSAL_NULL, 0);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_mac_phy_reset::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

td_s16 uapi_wifi_get_mac_status_code(td_void)
{
    td_s16 mac_status_code = -1;
    td_u16 *ptr_mac_status_code = wal_get_mac_status_code();
    td_u16 actual_code;
    if (*ptr_mac_status_code == 0xffff) {
        mac_status_code = -1;
    } else {
        actual_code = *ptr_mac_status_code;
        actual_code = (actual_code >= MAC_STATUS_ASSOC_MAX) ? (actual_code - MAC_STATUS_ASSOC_MAX) :
                ((actual_code >= MAC_STATUS_AUTH_MAX && actual_code < MAC_STATUS_ASSOC_MAX) ?
                 (actual_code - MAC_STATUS_AUTH_MAX) : actual_code);
        actual_code = (actual_code >= WLAN_MAC_EXT_AUTH_FAIL) ? (actual_code - WLAN_MAC_EXT_AUTH_FAIL) : 0xffff;
        mac_status_code = (td_s16)actual_code;
    }
    *ptr_mac_status_code = 0xffff;
    return mac_status_code;
}

td_s16 uapi_wifi_get_mac_rx_reason_code(td_void)
{
    td_s16 mac_reason_code = -1;
    td_u16 *ptr_mac_reason_code = wal_get_mac_reason_code();
    if (*ptr_mac_reason_code == 0xffff) {
        return mac_reason_code;
    }

    if (*ptr_mac_reason_code >= MAC_REASON_MAX) {
        mac_reason_code = (td_s16)(*ptr_mac_reason_code - MAC_REASON_MAX); /* 只上报rx disassoc的reason_code */
    }
    *ptr_mac_reason_code = 0xffff;
    return mac_reason_code;
}

td_u32 uapi_wifi_set_evm_para(td_u8 pa_bias, const ext_wifi_xldo_voltage *osc_drv_vals, td_u8 size)
{
    if (osc_drv_vals == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_evm_para:: osc_drv_vals is null.");
        return OAL_FAIL;
    }
    if (pa_bias > 1) {
        oam_error_log0(0, 0, "uapi_wifi_set_evm_para:: pa_bias is larger than 1.");
        return OAL_FAIL;
    }
    if (size != WIFI_24G_CHANNEL_NUMS) {
        oam_error_log0(0, 0, "uapi_wifi_set_evm_para:: size is not 14.");
        return OAL_FAIL;
    }

    return 0;
}

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/*****************************************************************************
 功能描述  : 使能WOW特性
 输入参数  : [1] 1 开启wow  0 关闭wow
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_wow_switch(td_u8 enable)
{
    oal_net_device_stru *net_dev = OSAL_NULL;
    td_u32 ret;

    if ((enable < MAC_WOW_DISABLE) || (enable >= MAC_WOW_EN_BUTT)) {
        oam_error_log1(0, 0, "uapi_wifi_set_wow_switch parameter err [%d].", enable);
        return OAL_FAIL;
    }

    net_dev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (net_dev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_wow_switch Featureid0 device not found.");
        return OAL_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_DYNAMIC_OFFLOAD
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_WOW_LOAD, (td_u8 *)&enable,
        OAL_SIZEOF(td_u8));
#else
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_HOST_SLEEP_EN,
        (osal_u8 *)&enable, OAL_SIZEOF(osal_u8));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, 0, "uapi_wifi_set_wow_switch:send msg ret [%d]", ret);
        return OAL_FAIL;
    }
#endif

    return ret;
}

/*****************************************************************************
 功能描述  : 获取add wow pattern的参数
*****************************************************************************/
static td_u32 wow_update_pattern(td_u8 *param, mac_cfg_wow_pattern_param_stru *wow_param)
{
    td_u8 idx, high, low, pattern_len;

    if (param == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "wow_update_pattern:pattern null");
        return OAL_FAIL;
    }

    pattern_len = osal_strlen(param);
    /* pattern长度为0 或 2:pattern长度为奇数校验 */
    if ((pattern_len == 0) || (pattern_len % 2 == 1) || (pattern_len > WOW_NETPATTERN_MAX_LEN)) {
        oam_warning_log1(0, OAM_SF_ANY, "wow_update_pattern:pattern len err [%d]", pattern_len);
        return OAL_FAIL;
    }

    /* 转化 netpattern 格式 */
    for (idx = 0; idx < pattern_len; idx += 2) { /* 2:每2个字符转换为一个字节pattern */
        if (oal_isxdigit(*(param + idx)) && oal_isxdigit(*(param + idx + 1))) {
            high = oal_isdigit(param[idx]) ?
                (param[idx] - '0') : (hi_tolower((td_u8)param[idx]) - 'a' + 10); /* 10: 偏移量 */
            low  = oal_isdigit(param[idx + 1]) ?
                (param[idx + 1] - '0') : (hi_tolower((td_u8)param[idx + 1]) - 'a' + 10); /* 10: 偏移量 */
            if ((td_u8)high > 0xF || (td_u8)low > 0xF) {
                oam_warning_log0(0, OAM_SF_ANY, "{wal_wow_update_pattern::invalid wow value}");
                return OAL_FAIL;
            }
            /* 2:每2个字符转换一次 4: 左移4位 */
            wow_param->pattern_value[idx / 2] = (((td_u8)high & 0xF) << 4) | ((td_u8)low & 0xF);
        } else {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_wow_update_pattern::invalid wow value}");
            return OAL_FAIL;
        }
    }
    /* 更新pattern value长度 */
    wow_param->pattern_len = (pattern_len >> 1);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 配置wow pattern
 输入参数  : [1] 操作类型 0 add 增加一条pattern;1 del 删除一条pattern;2 clr 清除保存的pattern
            [2] 下标 0-3 与add del配合使用
            [3] pattern：与add配合使用
 返 回 值  : 错误码
*****************************************************************************/
td_s32 uapi_wifi_set_wow_pattern(td_u8 type, td_u8 index, td_u8 *pattern)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    mac_cfg_wow_pattern_param_stru cfg_wow_param = {0};
    td_u32 ret;

    if ((type < MAC_WOW_PATTERN_ADD) || (type >= MAC_WOW_PATTERN_BUTT)) {
        oam_error_log1(0, 0, "uapi_wifi_set_wow_pattern:parameter err [%d].", type);
        return OAL_FAIL;
    }

    if ((index < 0) || (index >= WOW_NETPATTERN_MAX_NUM)) {
        oam_error_log1(0, 0, "uapi_wifi_set_wow_pattern:pattern index err [%d].", index);
        return OAL_FAIL;
    }

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_wow_switch Featureid0 device not found.");
        return OAL_FAIL;
    }

    switch (type) {
        case MAC_WOW_PATTERN_ADD:
            if (pattern == OSAL_NULL) {
                oam_error_log0(0, 0, "uapi_wifi_set_wow_pattern:pattern null");
                return OAL_FAIL;
            }
            cfg_wow_param.pattern_option = MAC_WOW_PATTERN_ADD;
            cfg_wow_param.pattern_index = index;
            ret = wow_update_pattern(pattern, &cfg_wow_param);
            if (ret != OAL_SUCC) {
                oam_error_log1(0, 0, "uapi_wifi_set_wow_pattern:add pattern ret [%d]", ret);
                return OAL_FAIL;
            }
            break;
        case MAC_WOW_PATTERN_DEL:
            cfg_wow_param.pattern_option = MAC_WOW_PATTERN_DEL;
            cfg_wow_param.pattern_index = index;
            break;
        case MAC_WOW_PATTERN_CLR:
            cfg_wow_param.pattern_option = MAC_WOW_PATTERN_CLR;
            break;
        default:
            return OAL_FAIL;
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_WOW_PATTERN,
        (td_u8 *)&cfg_wow_param, OAL_SIZEOF(mac_cfg_wow_pattern_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, 0, "uapi_wifi_set_wow_pattern:send msg ret [%d]", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif

osal_u32 uapi_alg_ar_fix_cfg_etc(osal_u32 value)
{
    osal_s32                       ret;
    alg_cfg_tx_rate_param_stru tx_rate_param = {0};

    tx_rate_param.alg_cfg = MAC_ALG_CFG_AUTORATE_RATE_MODE;
    tx_rate_param.fix_rate_mode = (osal_u8)value;

    ret = wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_SET_TX_RATE, (osal_u8 *)&tx_rate_param,
        sizeof(alg_cfg_tx_rate_param_stru));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_alg_ar_fix_cfg_etc::wal_sync_post2hmac_no_rsp err_code [%d]!}", ret);
    }
    return (osal_u32)ret;
}

/*****************************************************************************
 功能描述  : api配置固定速率
*****************************************************************************/
OAL_STATIC osal_u32 uapi_wifi_config_set_udata_fix_rate(osal_u8 auto_rate, osal_u32 param)
{
    osal_u32 ret;
    osal_u32 param_tmp = param;
    alg_cfg_tx_rate_param_stru tx_rate_param = {0};
    tx_rate_param.alg_cfg = MAC_ALG_CFG_AUTORATE_FIX_RATE;

    if (auto_rate == 1) {
        ret = uapi_alg_ar_fix_cfg_etc(0);
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_config_set_udata_fix_rate::uapi_alg_ar_fix_cfg_etc return[%u]!}\n", ret);
            return OAL_FAIL;
        }
    } else {
        ret = uapi_alg_ar_fix_cfg_etc(1);
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_config_set_udata_fix_rate::uapi_alg_ar_fix_cfg_etc return[%u]!}\n", ret);
            return OAL_FAIL;
        }
        /* 低4bit表示速率等级 */
        tx_rate_param.rate_index = param_tmp & 0xF;
        param_tmp >>= 4; /* 右移4位 */
        /* bit4~bit6表示协议模式 */
        tx_rate_param.protocol = param_tmp & 0x7;
        param_tmp >>= 3; /* 右移3位 */
        /* bit7表示带宽 */
        tx_rate_param.bw = ((param_tmp & 0x1) == 1) ? WLAN_BAND_ASSEMBLE_40M : WLAN_BAND_ASSEMBLE_20M;

        ret = (osal_u32)wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_SET_TX_RATE, (osal_u8 *)&tx_rate_param,
            sizeof(alg_cfg_tx_rate_param_stru));
        if (ret != OAL_SUCC) {
            wifi_printf("{uapi_wifi_config_set_udata_fix_rate::wal_sync_post2hmac_no_rsp err_code [%d]!}", ret);
            return OAL_FAIL;
        }
    }

    return OAL_SUCC;
}
/*****************************************************************************
 功能描述  : 设置固定速率api接口，调用uapi_wifi_config_set_udata_fix_rate
*****************************************************************************/
uint32_t uapi_wifi_set_phy_txrate(td_u8 auto_rate, ext_alg_param_stru *alg_param)
{
    uint32_t ret;
    if (auto_rate > 1) {
        wifi_printf("uapi_wifi_set_phy_txrate:: invalid param");
        return OAL_FAIL;
    }

    /* 固定速率 */
    ret = uapi_wifi_config_set_udata_fix_rate(auto_rate, alg_param->rate_value);
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_wifi_set_phy_txrate:: get user rate[%d]!}\r\n", ret);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

td_s32 uapi_wifi_get_tx_params(const td_char *mac_addr, td_u8 mac_len, td_u32 *tx_best_rate)
{
    td_u32 ret = OAL_SUCC;
    td_u32 netdev_index;
    td_u16 user_idx = 0;
    hmac_user_stru *hmac_user = OSAL_NULL;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_char *dev_name[] = {"wlan0", "ap0", "p2p0"};
    unref_param(mac_len);
    for (netdev_index = 0; netdev_index < WLAN_VAP_NUM_PER_BOARD; netdev_index++) {
        netdev = oal_get_past_net_device_by_index(netdev_index);
        if ((netdev != OSAL_NULL) &&
            /* 0:wlan0 */
            ((osal_adapt_strncmp((const td_char*)netdev->name, dev_name[0], osal_strlen(dev_name[0])) == 0) ||
            /* 1:ap0 */
            (osal_adapt_strncmp((const td_char*)netdev->name, dev_name[1], osal_strlen(dev_name[1])) == 0) ||
            /* 2:p2p0 */
            (osal_adapt_strncmp((const td_char*)netdev->name, dev_name[2], osal_strlen(dev_name[2])) == 0))) {
            ret = wal_get_user_by_mac(netdev, (osal_s8 *)mac_addr, &user_idx);
            if (ret == OAL_SUCC) {
                break;
            }
        }
    }
    if (netdev_index == WLAN_VAP_NUM_PER_BOARD) {
        wifi_printf("{uapi_wifi_get_tx_params:: get user failed[%d][%s]!}\r\n", ret, mac_addr);
    }
    hmac_user = mac_res_get_hmac_user_etc(user_idx);
    if (osal_unlikely(hmac_user == OSAL_NULL)) {
        wifi_printf("{uapi_wifi_get_tx_params::hmac_user null!}\r\n");
        return OAL_FAIL;
    }

    ret = wal_get_tx_params(hmac_user, tx_best_rate);
    if (osal_unlikely(ret != OAL_SUCC)) {
        wifi_printf("{uapi_wifi_get_tx_params::no such user!}\r\n");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

osal_s32 uapi_wifi_get_chan_intrf_info(wifi_channel_info_stru *chan_info, osal_u8 max_len, osal_u8 *chan_num)
{
    frw_msg msg = {0};
    wifi_channel_info_stru *chan_info_ptr = NULL;
    osal_u16 info_len = (osal_u16)(sizeof(wifi_channel_info_stru) * MAC_CHANNEL_FREQ_2_BUTT);
    osal_u16 info_len_total = (osal_u16)(info_len + sizeof(osal_u8));
    osal_s32 ret;

    msg.rsp = osal_kmalloc(info_len_total, OSAL_GFP_KERNEL);
    if (msg.rsp == NULL) {
        return OAL_FAIL;
    }
    (osal_void)memset_s(msg.rsp, info_len_total, 0, info_len_total);
    msg.rsp_buf_len = info_len_total;

    ret = send_sync_cfg_to_host(0, WLAN_MSG_W2H_CFG_GET_INTRF_INFO, &msg);
    if (ret != OAL_SUCC) {
        osal_kfree(msg.rsp);
        return ret;
    }

    chan_info_ptr = (wifi_channel_info_stru *)msg.rsp;
    *chan_num = *(msg.rsp + info_len);

    if (memcpy_s(chan_info, max_len, chan_info_ptr, info_len) != EOK) {
        wifi_printf("{uapi_wifi_get_chan_intrf_info::memcpy_s fail!}\r\n");
    }

    osal_kfree(msg.rsp);
    return ret;
}

osal_s32 uapi_wifi_set_mgmt_report(wifi_rx_mgmt_cb data_cb, uint8_t mode)
{
    char *dev_name[] = {"wlan0", "ap0"};
    uint32_t index = 0;
    oal_net_device_stru *netdev = OSAL_NULL;
    int32_t ret;
    uint8_t enable_flag = (data_cb == OSAL_NULL) ? OSAL_FALSE : mode;

    for (index = 0; index < OAL_ARRAY_SIZE(dev_name); ++index) {
        netdev = oal_get_netdev_by_name(dev_name[index]);
        if (netdev != OSAL_NULL) {
            break;
        }
    }
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_mgmt_report Featureid0 device not found.");
        return OAL_FAIL;
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_MGMT_FRAME_REPORT,
        (td_u8 *)&enable_flag, OAL_SIZEOF(enable_flag));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, 0, "uapi_wifi_set_mgmt_report:send msg ret [%d]", ret);
        return OAL_FAIL;
    }
    uapi_wlan_register_report_mgmt_frame(data_cb);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置重传报文的最大重传次数
*****************************************************************************/
td_s32 uapi_wifi_set_pkt_retry_policy(td_u8 frame_type, td_u8 retry_count)
{
    td_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_u8 retry_param[2] = {0};

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_pkt_retry_policy:: netdev is NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }

    retry_param[0] = frame_type;
    retry_param[1] = retry_count;
    ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2D_C_CFG_SET_FRAME_SOFT_RETRY_TIMES,
        (td_u8 *)&retry_param, sizeof(retry_param));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_wifi_set_pkt_retry_policy::send msg failed[%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

td_s32 uapi_set_low_current_boot_mode(td_u16 flag)
{
    switch (flag) {
        case LOW_CURRENT_BOOT_MODE_DEFAULT:
        case LOW_CURRENT_BOOT_MODE_WITHOUT_PWR_CALI:
        case LOW_CURRENT_BOOT_MODE_WITH_ONCE_PWR_CALI:
        case LOW_CURRENT_BOOT_MODE_WITHOUT_CALI:
            hwifi_set_low_current_boot_mode(flag);
            break;
        default:
            oam_error_log1(0, 0, "uapi_set_low_current_boot_mode:: flag:%u is invalid.", flag);
            return OAL_FAIL;
    }
    return OAL_SUCC;
}

td_s32 uapi_wifi_send_pkt(osal_void *dev, const data_segment_t *seg, uint8_t seg_len)
{
    struct sk_buff *skb;
    oal_net_device_stru *netdev = (oal_net_device_stru *)dev;
    td_s32 index = 0;
    size_t total_len = 0;
    td_s32 step_len = 0;

    if (netdev == NULL) {
        oam_error_log0(0, 0, "uapi_wifi_send_pkt::device not found.");
        return OAL_FAIL;
    }

    for (index = 0; index < seg_len; index++) {
        total_len += seg[index].len;
    }

    skb = hwal_lwip_skb_alloc(netdev, (uint16_t)total_len);
    if (skb == NULL || skb->data == NULL) {
        oam_error_log0(0, 0, "uapi_wifi_send_pkt::skb alloc fail.");
        return OAL_FAIL;
    }

    for (index = 0; index < seg_len; index++) {
        if (seg[index].data != NULL) {
            oal_netbuf_put(skb, seg[index].len);
            if (memcpy_s(oal_netbuf_data(skb) + step_len, seg[index].len, seg[index].data, seg[index].len) != EOK) {
                oal_netbuf_free(skb);
                oam_error_log0(0, 0, "{uapi_wifi_send_pkt::mem safe function err!}");
                return OAL_FAIL;
            }
            step_len += (td_s32)seg[index].len;
        }
    }

    frw_host_post_data(FRW_NETBUF_W2H_DATA_FRAME, wal_util_get_vap_id(netdev), skb);
    oam_info_log0(0, 0, "{uapi_wifi_send_pkt::pkt send succ}");
    return OAL_SUCC;
}

td_s32 uapi_wifi_set_brctl(const char *oper, const char *if_name)
{
#if defined(_PRE_WLAN_FEATURE_SINGLE_PROXYSTA) && defined(_PRE_WLAN_FEATURE_LOCAL_BRIDGE)
    mac_cfg_brctl_stru brctl_cmd;
    td_s32 ret;

    if (strcpy_s(brctl_cmd.cmd_name, BRIDGE_CMD_NAME_MAX_LEN, oper) != EOK) {
        return OAL_FAIL;
    }
    if (if_name != OSAL_NULL) {
        if (strcpy_s(brctl_cmd.if_name, BRIDGE_CMD_NAME_MAX_LEN, if_name) != EOK) {
            return OAL_FAIL;
        }
    }

    ret = wal_sync_post2hmac_no_rsp(0, WLAN_MSG_W2H_CFG_SET_BRCTL, (td_u8 *)&brctl_cmd, sizeof(mac_cfg_brctl_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "uapi_wifi_set_brctl::send msg ret %d", ret);
    }
    return ret;
#else
    unref_param(oper);
    unref_param(if_name);
    return OAL_SUCC;
#endif
}

td_u32 uapi_wifi_sta_config_conn_paras(ext_wifi_config_conn_paras *para)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_s32 ret;

    netdev = oal_get_netdev_by_name("wlan0");
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_sta_config_conn_paras device not fonud.");
        return OAL_FAIL;
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_STA_CONN_PARAS,
        (osal_u8 *)para, OAL_SIZEOF(ext_wifi_config_conn_paras));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "uapi_wifi_sta_config_conn_paras ret:%d", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

td_u32 uapi_wifi_sta_data_frame_rssi(td_s8 *rssi)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    frw_msg cfg_info;
    osal_s8 config_msg = 0;
    osal_s32 ret;

    netdev = oal_get_netdev_by_name("wlan0");
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, 0, "uapi_wifi_sta_data_frame_rssi device not fonud.");
        return OAL_FAIL;
    }

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = (osal_u16)sizeof(config_msg);
    cfg_info.rsp = (osal_u8 *)&config_msg;
    ret = send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_DATA_FRAME_RSSI, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_wifi_sta_data_frame_rssi:: get data frame rssi failed!}");
        return OAL_FAIL;
    }

    *rssi = *(osal_s8 *)cfg_info.rsp;
    return OAL_SUCC;
}
wlan_protocol_enum_uint8 uapi_wifi_sta_conn_get_protocol_mode(void)
{
    oal_net_device_stru *net_dev = OSAL_NULL;
    mac_cfg_mode_param_stru *protocol_mode_cfg = OSAL_NULL;
    osal_s32 ret;
    osal_s32 fill_msg;
    frw_msg cfg_info;

    net_dev = oal_get_netdev_by_name("wlan0");
    if (net_dev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "uapi_wifi_sta_conn_get_protocol_mode device not fonud.");
        return WLAN_PROTOCOL_BUTT;
    }

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = (osal_u16)sizeof(fill_msg);
    cfg_info.rsp = (osal_u8 *)&fill_msg;

    ret = send_sync_cfg_to_host(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_GET_MODE, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_sta_conn_get_protocol_mode::return err code %d!}", ret);
        return WLAN_PROTOCOL_BUTT;
    }
    protocol_mode_cfg = (mac_cfg_mode_param_stru *)cfg_info.rsp;

    return protocol_mode_cfg->protocol;
}

#ifdef _PRE_SUPPORT_SET_STA_MODE
td_s32 uapi_wifi_set_sta_wireless_mode(const char *param)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    const wal_ioctl_mode_map_stru mode_map[] = {
        {"11b",             WLAN_LEGACY_11B_MODE,       WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
        {"11bg",            WLAN_MIXED_ONE_11G_MODE,    WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
        {"11bgn",           WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
#ifdef _PRE_WLAN_FEATURE_11AX
        {"11bgnax",         WLAN_HE_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
        {"11ax2g20",        WLAN_HE_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
#endif
        {"11g2g20",         WLAN_MIXED_TWO_11G_MODE,    WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
        {"11n2g20",         WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M, {0}},
        {"11n2g40",         WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_40M, {0}},
        {"11n2g40plus",     WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_40PLUS, {0}},
        {"11n2g40minus",    WLAN_HT_MODE,               WLAN_BAND_2G,   WLAN_BAND_WIDTH_40MINUS, {0}},
        {OSAL_NULL}
    };
 
    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_set_sta_wireless_mode:: netdev is NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }
 
    return wal_set_mode(netdev, mode_map, (const osal_s8 *)param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_MAC_STATISTICS
td_s32 uapi_wifi_get_mac_frame_trx_info(mac_frame_trx_info *info)
{
    hal_mac_tx_mpdu_statis_info_stru mac_tx_statis = {0};
    osal_u8 vap_id;
    osal_s32 ret;
    frw_msg msg_to_device = {0};
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_u32 device_vap_stat[DEVICE_VAP_STAT_NUM] = {0};
    dft_device_statis_cnt_stru device_hw_stat = {0};
 
    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_get_mac_frame_trx_info:: netdev is NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }
 
    vap_id = wal_util_get_vap_id(netdev);
    /* 获取dmac侧hw统计结果 */
    frw_msg_init(&vap_id, sizeof(vap_id), (osal_u8 *)&device_hw_stat,
        sizeof(dft_device_statis_cnt_stru), &msg_to_device);
    ret = frw_send_msg_to_device(vap_id, WLAN_MSG_W2D_C_CFG_GET_HW_STAT, &msg_to_device, OSAL_TRUE);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_wifi_get_mac_frame_trx_info::get hw stat fail=%d!", ret);
        return OAL_FAIL;
    }
 
    /* 获取dmac侧vap统计结果 */
    frw_msg_init(&vap_id, sizeof(vap_id), (osal_u8 *)device_vap_stat,
        DEVICE_VAP_STAT_NUM * sizeof(osal_u32), &msg_to_device);
    ret = frw_send_msg_to_device(vap_id, WLAN_MSG_W2D_C_CFG_GET_VAP_STAT, &msg_to_device, OSAL_TRUE);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_wifi_get_mac_frame_trx_info::get vap stat fail=%d!", ret);
        return OAL_FAIL;
    }
 
    hal_get_mac_tx_statistics_data(&mac_tx_statis);
 
    // 所有接收帧总数
    info->rx_total_num = device_hw_stat.rx_from_hw_mpdu_cnt;
 
    // 接收数据帧总数
    info->rx_data_num = device_vap_stat[RX_DEVICE_D_FROM_HW_MPDU - HOST_PKTS_CNT_TYPE_END];
 
    // 接收丢弃总数=dmac接收总数-接收数据帧总数-接收管理帧总数
    info->rx_droped_num = info->rx_total_num - info->rx_data_num -
        device_vap_stat[RX_DEVICE_M_FROM_HW_MPDU - HOST_PKTS_CNT_TYPE_END];
 
    // 所有发送帧总数
    info->tx_total_num = mac_tx_statis.tx_complete_cnt;
 
    // 发送数据帧总数
    info->tx_data_num = mac_tx_statis.tx_normal_priority_mpdu_cnt + mac_tx_statis.tx_ampdu_inmpdu_cnt;
 
    // 发送丢弃总数, 发送方向mac不丢包
    info->tx_droped_num = 0;
 
    return OAL_SUCC;
}
 
td_s32 uapi_wifi_mac_frame_trx_info_clear(void)
{
    frw_msg msg_to_device = {0};
    osal_u8 hw_stat_enable = OSAL_TRUE;
    dft_vap_statis_command_stru vap_statis_command = {0};
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_u8 vap_id;
    osal_s32 ret;
 
    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "uapi_wifi_mac_frame_trx_info_clear:: netdev is NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }
 
    vap_id = wal_util_get_vap_id(netdev);
    /* hw统计清零，重复使能会清零 */
    frw_msg_init(&hw_stat_enable, sizeof(osal_u8), OAL_PTR_NULL, 0, &msg_to_device);
    ret = frw_send_msg_to_device(vap_id, WLAN_MSG_W2D_C_CFG_ENABLE_HW_STAT, &msg_to_device, OSAL_TRUE);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_wifi_mac_frame_trx_info_clear::enable hw stat fail=%d!", ret);
        return OAL_FAIL;
    }
 
    /* vap统计清零，重复使能会清零 */
    vap_statis_command.enable = OSAL_TRUE;
    vap_statis_command.vap_id = vap_id;
    frw_msg_init((osal_u8 *)&vap_statis_command, sizeof(dft_vap_statis_command_stru), OAL_PTR_NULL, 0, &msg_to_device);
    ret = frw_send_msg_to_device(vap_id, WLAN_MSG_W2D_C_CFG_ENABLE_VAP_STAT, &msg_to_device, OSAL_TRUE);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_wifi_mac_frame_trx_info_clear::enable vap stat fail=%d!", ret);
        return OAL_FAIL;
    }
 
    hal_hw_stat_clear();
    return OAL_SUCC;
}
#endif
 
td_s32 uapi_wifi_set_tx_power_db_offset(int8_t protocol, int8_t rate, int8_t offset)
{
    oal_net_device_stru *netdev = OSAL_NULL;
    /* 根据dev_name找到dev */
    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_at_entry::get netdev by name return null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    return wal_set_rate_power(netdev, protocol, rate, offset);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
