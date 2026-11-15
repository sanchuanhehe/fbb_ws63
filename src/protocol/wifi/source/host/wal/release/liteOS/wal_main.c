/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: WAL module initialization and uninstallation.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "frw_ext_if.h"
#include "frw_timer.h"
#include "hmac_ext_if.h"
#include "wlan_msg.h"
#include "wal_ioctl.h"
#include "wal_ccpriv.h"
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#include "wal_cfg80211.h"
#endif
#include "wal_net.h"
#include "wal_config.h"
#include "wal_event_msg.h"
#include "soc_customize_wifi.h"
#include "soc_wifi_driver_api.h"
#include "soc_wifi_api.h"
#include "mac_addr.h"
#ifdef _PRE_WLAN_FEATURE_WS73
#include "efuse_opt.h"
#endif
#include "wal_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_MAIN_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

#define WAL_HAL_INTERRUPT_COUNT   4
/*****************************************************************************
  3 函数实现
*****************************************************************************/
static osal_void wal_event_init(osal_void)
{
    frw_msg_hook_register(WLAN_MSG_H2W_SCAN_COMP_STA, wal_scan_comp_proc_sta);
    frw_msg_hook_register(WLAN_MSG_H2W_ASOC_COMP_STA, wal_asoc_comp_proc_sta);
    frw_msg_hook_register(WLAN_MSG_H2W_DISASOC_COMP_STA, wal_disasoc_comp_proc_sta);
    frw_msg_hook_register(WLAN_MSG_H2W_STA_CONNECT_AP, wal_connect_new_sta_proc_ap);
    frw_msg_hook_register(WLAN_MSG_H2W_STA_DISCONNECT_AP, wal_disconnect_sta_proc_ap);
    frw_msg_hook_register(WLAN_MSG_H2W_RX_MGMT, wal_send_mgmt_to_host);
#ifdef _PRE_WLAN_FEATURE_P2P
    frw_msg_hook_register(WLAN_MSG_H2W_LISTEN_EXPIRED, wal_p2p_listen_timeout);
#endif
    frw_msg_hook_register(WLAN_MSG_H2W_MGMT_TX_STATUS, wal_cfg80211_mgmt_tx_status);
    frw_msg_hook_register(WLAN_MSG_H2W_STA_CONN_RESULT, wal_report_sta_assoc_info);
    frw_msg_hook_register(WLAN_MSG_H2W_RX_CSA_DONE, wal_report_csa_done);
#ifdef _PRE_WLAN_FEATURE_WPA3
    frw_msg_hook_register(WLAN_MSG_H2W_EXTERNAL_AUTH, wal_trigger_external_auth_sta);
    frw_msg_hook_register(WLAN_MSG_H2W_STA_OWE_INFO, wal_cfg80211_report_owe_info);
#endif
#ifdef _PRE_WLAN_FEATURE_ROAM
    frw_msg_hook_register(WLAN_MSG_H2W_ROAM_COMP_STA, wal_roam_comp_proc_sta_etc);
#ifdef _PRE_WLAN_FEATURE_11R
    frw_msg_hook_register(WLAN_MSG_H2W_FT_EVENT_STA, wal_ft_event_proc_sta_etc);
#endif
#endif
    frw_msg_hook_register(WLAN_MSG_H2W_RX_CSI, wal_csi_data_report);
    frw_msg_hook_register(WLAN_MSG_H2W_RX_PSD, wal_psd_data_report);
    frw_msg_hook_register(WLAN_MSG_H2W_UPLOAD_RX_MGMT, wal_upload_rx_mgmt_frame);
    frw_msg_hook_register(WLAN_MSG_H2W_UPLOAD_PROMIS, wal_upload_rx_promis_frame);
}

/*****************************************************************************
 功能描述  : WAL模块初始化总入口，包含WAL模块内部所有特性的初始化。
 返 回 值  : 初始化返回值，成功或失败原因
*****************************************************************************/
td_u32 wal_main_init(td_void)
{
    frw_init_enum_uint16  init_state;

    init_state = frw_get_init_state_etc();
    /* WAL模块初始化开始时，说明HMAC肯定已经初始化成功 */
    if ((init_state == FRW_INIT_STATE_BUTT) || (init_state < FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_main_init::init_state has a invalid value [%d]!}", init_state);
        frw_timer_exit();
        return OAL_FAIL;
    }
    wal_cfg_init();
    wal_event_init();

    /* 在host侧如果WAL初始化成功，即为全部初始化成功 */
    frw_set_init_state_etc(FRW_INIT_STATE_ALL_SUCC);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 卸载前删除所有vap
*****************************************************************************/
OAL_STATIC td_void wal_destroy_all_vap(td_void)
{
    oal_net_device_stru* netdev = OSAL_NULL;
    td_char param[10] = {0}; /* 元素个数为10 */
    td_u8 vap_id = WLAN_SERVICE_VAP_START_ID;

    /* 删除业务vap */
    for (; vap_id < WLAN_SERVICE_VAP_NUM_PER_DEVICE; vap_id++) {
        netdev = hmac_vap_get_net_device_etc(vap_id);
        if (netdev != OSAL_NULL) {
            oal_net_device_close(netdev);
            wal_ccpriv_del_vap(netdev, (td_char *)param);
            frw_event_process_all_event_etc(0);
        }
    }
    frw_event_process_all_event_etc(0);
    return;
}

/*****************************************************************************
 功能描述  : WAL模块卸载
 返 回 值  : 模块卸载返回值，成功或失败原因
*****************************************************************************/
td_void wal_main_exit(td_void)
{
    if (frw_get_init_state_etc() != FRW_INIT_STATE_ALL_SUCC) {
        oam_info_log0(0, 0, "{wal_main_exit::frw state wrong.\n");
        return;
    }
    /* down掉所有的vap */
    wal_destroy_all_vap();
    /* 卸载成功时，将初始化状态置为HMAC初始化成功 */
    frw_set_init_state_etc(FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC);
}

/*****************************************************************************
 功能描述  : Host部分 加载初始化
*****************************************************************************/
td_u32 uapi_wifi_host_init(td_void)
{
    td_u32  ret;

    ret = (td_u32)hmac_main_init_etc();
    if (ret != OAL_SUCC) {
        oam_info_log1(0, 0, "uapi_wifi_host_init: hmac_main_init return error code: %d", ret);
        goto hmac_main_init_fail;
    }

    ret = wal_main_init();
    if (ret != OAL_SUCC) {
        oam_info_log1(0, 0, "uapi_wifi_host_init: wal_main_init return error code: %d", ret);
        goto wal_main_init_fail;
    }

    return OAL_SUCC;

wal_main_init_fail:
    hmac_main_exit_etc();
hmac_main_init_fail:

    return ret;
}

/*****************************************************************************
 功能描述  : Host部分 卸载
*****************************************************************************/
td_void uapi_wifi_host_exit(td_void)
{
    wal_main_exit();
    hmac_main_exit_etc();

    wifi_printf("wifi host exit successfully!\n");
    return;
}


/*****************************************************************************
 功能描述  : 平台初始化函数总入口
 输入参数  : vap_num : 最大支持的同时工作的vap数量
             user_num: 最大支持接入的用户数量,多vap时共享
*****************************************************************************/
td_u32 uapi_wifi_plat_init(td_void)
{
    if (oal_main_init_etc() != OAL_SUCC) {
        oam_error_log0(0, 0, "plat_init: oal_main_init return error code.");
        goto oal_main_init_fail;
    }
    if (frw_main_init_etc() != OAL_SUCC) {
        oam_error_log0(0, 0, "plat_init: frw_main_init_etc return error code.");
        goto frw_main_init_fail;
    }
    return OAL_SUCC;

frw_main_init_fail:
    oal_main_exit_etc();
oal_main_init_fail:
    wal_customize_exit();

    return OAL_FAIL;
}

/*****************************************************************************
 功能描述  : 平台卸载函数总入口
*****************************************************************************/
td_void uapi_wifi_plat_exit(td_void)
{
    if (frw_get_init_state_etc() != FRW_INIT_STATE_FRW_SUCC) {
        oam_error_log0(0, 0, "{uapi_wifi_plat_exit:: frw init state error.}");
        return;
    }
    frw_main_exit_etc();
    oal_main_exit_etc();
    wal_customize_exit();

    wifi_printf("wifi platform exit successfully!\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

