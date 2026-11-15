/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Linux ioctl configuration command.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hmac_ext_if.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_main.h"
#include "wal_event_msg.h"
#include "wal_ccpriv.h"
#include "wal_net.h"
#include "soc_customize_wifi.h"
#include "mac_addr.h"
#include "frw_util_notifier.h"
#include "wal_ioctl.h"
#if defined (_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/netifapi.h"
#endif // #ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#endif
#endif
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
#include "nv.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_IOCTL_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#define MAX_PRIV_CMD_SIZE   4096

/*****************************************************************************
  私有命令函数表. 私有命令格式:
         设备名 命令名 参数
  ccpriv "Featureid0 create vap0"
*****************************************************************************/
/* Android private command strings */
#define CMD_SET_AP_WPS_P2P_IE   "SET_AP_WPS_P2P_IE"
#define CMD_P2P_SET_NOA         "P2P_SET_NOA"
#define CMD_P2P_SET_PS          "P2P_SET_PS"
#define CMD_SET_POWER_ON        "SET_POWER_ON"
#define CMD_SET_POWER_MGMT_ON   "SET_POWER_MGMT_ON"
#define CMD_COUNTRY             "COUNTRY"
#define CMD_SET_QOS_MAP         "SET_QOS_MAP"
#define CMD_TX_POWER            "TX_POWER"
#define CMD_WPAS_GET_CUST       "WPAS_GET_CUST"
#define CMD_SET_SSID            "SET_SSID"

/*****************************************************************************
  2 函数声明
*****************************************************************************/
/* 静态函数声明 */
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*****************************************************************************
  协议模式字符串定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : uapi_ccpriv_set_mode
 功能描述  : 设置模式: 包括协议、频段、带宽
*****************************************************************************/
td_u32 uapi_ccpriv_set_mode(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    td_char            ac_mode_str[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};     /* 预留协议模式字符串空间 */
    td_u8              prot_idx;
    td_u32             off_set = 0;
    mac_cfg_mode_param_stru mode_info = {0};
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

    if (OAL_UNLIKELY((netdev == OSAL_NULL) || (pc_param == OSAL_NULL))) {
        oam_error_log2(0, OAM_SF_ANY, "{uapi_ccpriv_set_mode::error %p %p!}", (uintptr_t)netdev, (uintptr_t)pc_param);
        return OAL_ERR_CODE_WAL_FAILURE;
    }

    /* pc_param指向传入模式参数, 将其取出存放到ac_mode_str中 */
    td_u32 ret = wal_get_cmd_one_arg(pc_param, (osal_s8 *)ac_mode_str, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_mode::mode get failed [%d]!}\r\n", ret);
        return ret;
    }
    ac_mode_str[sizeof(ac_mode_str) - 1] = '\0';    /* 确保以null结尾 */

    for (prot_idx = 0; mode_map[prot_idx].pc_name != OAL_PTR_NULL; prot_idx++) {
        if (osal_strcmp(mode_map[prot_idx].pc_name, (const osal_char *)ac_mode_str) == 0) {
            break;
        }
    }

    if (mode_map[prot_idx].pc_name == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_set_mode::unrecognized protocol string!}\r\n");
        return OAL_FAIL;
    }

    mode_info.protocol = mode_map[prot_idx].mode;
    mode_info.band = mode_map[prot_idx].band;
    mode_info.en_bandwidth = mode_map[prot_idx].en_bandwidth;

    ret = (td_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_MODE,
        (osal_u8 *)&mode_info, OAL_SIZEOF(mode_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_mode::wal_sync_post2hmac_no_rsp return err code %u!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_ioctl_set_essid
 功能描述  : 设置ssid
 输入参数  : [1]net_dev
             [2]pc_param
 输出参数  : 无
 返 回 值  : static td_u32
*****************************************************************************/
td_u32  wal_ioctl_set_essid(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_ssid[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    hmac_vap_stru       *mac_vap = (hmac_vap_stru *)netdev->ml_priv;

    if (mac_vap == NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ioctl_set_essid::mac_vap is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((mac_vap->vap_mode == WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
    || (mac_vap->vap_mode == WLAN_VAP_MODE_MESH)
#endif
    ) {
        /* 设备在up状态且是AP时，不允许配置，必须先down */
        if ((OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(netdev)) != 0) {
            oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
                "{wal_ioctl_set_essid::device is busy, please down it firste %d!}", OAL_NETDEVICE_FLAGS(netdev));
            return OAL_FAIL;
        }
    }

    /* pc_param指向传入模式参数, 将其取出存放到ac_mode_str中 */
    td_u32 ret = wal_get_cmd_one_arg((osal_s8 *)pc_param, (osal_s8 *)ac_ssid, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ioctl_set_essid::wal_get_cmd_one_arg vap name return err_code %d!}", ret);
        return ret;
    }

    osal_char *pc_ssid  = (osal_char *)oal_strim((osal_s8 *)ac_ssid); /* 去掉字符串开始结尾的空格 */
    td_u8    ssid_len = (td_u8)osal_strlen(pc_ssid);
    oam_info_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_ioctl_set_essid:: ssid length %d!}", ssid_len);
    ssid_len = (ssid_len > WLAN_SSID_MAX_LEN - 1) ? (WLAN_SSID_MAX_LEN - 1) : ssid_len; /* -1为\0预留空间 */
    oam_info_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_ioctl_set_essid:: ssid length is %d!}", ssid_len);

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    ret = (osal_u32)wal_set_ssid(netdev, (osal_u8 *)pc_ssid, ssid_len);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_ioctl_set_essid:: wal_set_ssid Err=%u}", ret);
        return ret;
    }

    return OAL_SUCC;
}

td_u32  wal_ioctl_set_txpower(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_s32                   l_pwer;
    td_u32                   off_set;
    td_char                  ac_val[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32                   ret;

    ret = wal_get_cmd_one_arg((osal_s8 *)pc_param, (osal_s8 *)ac_val, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ioctl_set_txpower::wal_get_cmd_one_arg vap name return err_code %d!}\r\n",
            ret);
        return ret;
    }

    l_pwer = atoi(ac_val);
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_TX_POWER,
        (osal_u8 *)&l_pwer, OAL_SIZEOF(l_pwer));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ioctl_set_txpower::return err code %u!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_set_mac_to_mib
 功能描述  : 设置wlan接口mac地址
*****************************************************************************/
td_u32 wal_set_mac_to_mib(oal_net_device_stru *netdev)
{
    td_u32                        ret;
    mac_cfg_staion_id_param_stru staion_id;

    (osal_void)memset_s(&staion_id, OAL_SIZEOF(staion_id), 0, OAL_SIZEOF(staion_id));
    oam_info_log0(0, OAM_SF_ANY, "{wal_set_mac_to_mib::enter!}");

    /* 设置配置命令参数 */
    if (memcpy_s(staion_id.station_id, WLAN_MAC_ADDR_LEN, netdev->dev_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_set_mac_to_mib::mem safe function err!}");
        return OAL_FAIL;
    }

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_STATION_ID,
        (osal_u8 *)&staion_id, OAL_SIZEOF(staion_id));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_mac_to_mib::return err code [%u]!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置WPS p2p信息元素

 修改历史      :
  1.日    期   : 2014年11月26日
    修改内容   : 新生成函数

*****************************************************************************/
td_u32 wal_ioctl_set_wps_p2p_ie(oal_net_device_stru *netdev, const td_u8 *buf, td_u32 len,
    app_ie_type_uint8 type)
{
    oal_app_ie_stru                 wps_p2p_ie;
    osal_u32 ret;

    if (len > WLAN_WPS_IE_MAX_SIZE) {
        oam_error_log1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie:: wrong len: [%u]!}\r\n", len);
        return OAL_FAIL;
    }

    /* 安全编程规则6.6例外(1) 固定长度的结构体进行内存初始化 */
    memset_s(&wps_p2p_ie, sizeof(wps_p2p_ie), 0, sizeof(wps_p2p_ie));

    switch (type) {
        case OAL_APP_BEACON_IE:
        case OAL_APP_PROBE_RSP_IE:
        case OAL_APP_ASSOC_RSP_IE:
            wps_p2p_ie.app_ie_type = type;
            wps_p2p_ie.ie_len      = len;
            if (memcpy_s(wps_p2p_ie.ie, sizeof(wps_p2p_ie.ie), buf, len) != EOK) {
                oam_error_log0(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie::mem safe function err!}");
                return OAL_FAIL;
            }
            break;
        default:
            oam_error_log1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie:: wrong type: [%x]!}\r\n", type);
            return OAL_FAIL;
    }

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_WPS_IE,
        (osal_u8 *)&wps_p2p_ie, OAL_SIZEOF(wps_p2p_ie));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_ioctl_set_wps_p2p_ie:: wal_sync_post2hmac_no_rsp err %u!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 函 数 名  : wal_ioctl_set_p2p_noa
 功能描述  : 设置p2p noa节能参数
****************************************************************************/
td_u32 wal_ioctl_set_p2p_noa(oal_net_device_stru *netdev, const mac_cfg_p2p_noa_param_stru *p2p_noa_param)
{
    td_u32             ret;
    mac_cfg_p2p_noa_param_stru param = {0};

    if (p2p_noa_param != OSAL_NULL) {
        if (memcpy_s(&param, sizeof(param),
            p2p_noa_param, sizeof(mac_cfg_p2p_noa_param_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa::mem safe function err!}");
            return OAL_FAIL;
        }
    }

    /* 发送消息 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_P2P_PS_NOA,
        (osal_u8 *)&param, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_P2P,
            "{wal_ioctl_set_p2p_noa:: wal_sync_post2hmac_no_rsp return err code %u!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_ioctl_set_p2p_ops
 功能描述  : 设置p2p ops节能参数
****************************************************************************/
td_u32 wal_ioctl_set_p2p_ops(oal_net_device_stru *netdev, const mac_cfg_p2p_ops_param_stru *p2p_ops_param)
{
    td_u32             ret;
    mac_cfg_p2p_ops_param_stru param = {0};

    if (p2p_ops_param != OSAL_NULL) {
        if (memcpy_s(&param, sizeof(param),
            p2p_ops_param, sizeof(mac_cfg_p2p_ops_param_stru)) != EOK) {
            oam_error_log0(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops::mem safe function err!}");
            return OAL_FAIL;
        }
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_P2P_PS_OPS,
        (osal_u8 *)&param, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_P2P,
            "{wal_ioctl_set_p2p_ops:: wal_sync_post2hmac_no_rsp return err code %u!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : wal_start_vap
*****************************************************************************/
td_u32 wal_start_vap(oal_net_device_stru *netdev)
{
    td_u32                  ret;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 p2p_mode;
    oal_wireless_dev       *wdev = OSAL_NULL;
#endif
    mac_cfg_start_vap_param_stru vap_info;
    (osal_void)memset_s(&vap_info, OAL_SIZEOF(vap_info), 0, OAL_SIZEOF(vap_info));

    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_start_vap::net_dev is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    vap_info.net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    wdev = netdev->ieee80211_ptr;
    if (OAL_UNLIKELY(wdev == OSAL_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_start_vap::wdev is null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(wdev->iftype);
    if (WLAN_P2P_BUTT == p2p_mode) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_start_vap::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        return OAL_FAIL;
    }
    vap_info.p2p_mode = p2p_mode;

    oam_warning_log1(0, OAM_SF_ANY, "{wal_start_vap::p2p_mode:%d}\r\n", p2p_mode);
#endif
    vap_info.mgmt_rate_init_flag = OSAL_TRUE;
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_START_VAP,
        (osal_u8 *)&vap_info, OAL_SIZEOF(vap_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_start_vap::wal_sync_post2hmac_no_rsp return err code %u!}\r\n", ret);
        wifi_printf("{wal_start_vap::wal_sync_post2hmac_no_rsp return err code %u!}\r\n", ret);
        return ret;
    }
    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) == 0) {
        OAL_NETDEVICE_FLAGS(netdev) |= OAL_IFF_RUNNING;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_stop_vap
 功能描述  : 停用vap
*****************************************************************************/
td_u32 wal_stop_vap(oal_net_device_stru *netdev)
{
    td_u32                  ret;

    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_stop_vap::net_dev is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 如果不是up状态，不能直接返回成功,防止netdevice状态与VAP状态不一致的情况 */
    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) == 0) {
        /* 解决vap状态与netdevice状态不一致，无法down vap的问题 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_stop_vap::vap is already down,continue to reset hmac vap state.}\r\n");
    }

    ret = (osal_u32)wal_down_vap(netdev, wal_util_get_vap_id(netdev));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_stop_vap::wal_down_vap return err code %d!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}

OAL_STATIC td_u32 wal_get_vap_p2p_mode(const oal_wireless_dev *wdev,
    wlan_vap_mode_enum_uint8 *vap_mode, wlan_p2p_mode_enum_uint8 *p2p_mode)
{
    unref_param_prv(p2p_mode);

    switch (wdev->iftype) {
        case NL80211_IFTYPE_STATION:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            break;
        case NL80211_IFTYPE_AP:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            break;
#ifdef _PRE_WLAN_FEATURE_P2P
        case NL80211_IFTYPE_P2P_CLIENT:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            *vap_mode = WLAN_VAP_MODE_BSS_AP;
            *p2p_mode = WLAN_P2P_GO_MODE;
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            *vap_mode = WLAN_VAP_MODE_BSS_STA;
            *p2p_mode = WLAN_P2P_DEV_MODE;
            break;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
            *vap_mode = WLAN_VAP_MODE_MESH;
            break;
#endif
        default:
            oam_warning_log1(0, OAM_SF_ANY, "{wal_get_vap_p2p_mode::iftype[%d] is not supported!}", wdev->iftype);
            return EXT_ERR_SUCCESS;
    }
    return OAL_CONTINUE;
}
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#if defined(LWIP_NETIF_EXT_STATUS_CALLBACK) && LWIP_NETIF_EXT_STATUS_CALLBACK
static netif_ext_callback_t callback;
static td_void wal_get_ip_status_from_lwip(struct netif *netif, netif_nsc_reason_t reason,
    const netif_ext_callback_args_t *args)
{
    td_s32 ret;
    td_u8 ip_stat = 0;
    oal_net_device_stru *netdev = OSAL_NULL;
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
    unref_param(args);
    if (netif == OSAL_NULL) {
        return;
    }

    netdev = oal_get_netdev_by_name("wlan0");
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_get_ip_status_from_lwip::getdev by name failed}");
        return;
    }

    if (((reason & LWIP_NSC_IPV4_ADDRESS_CHANGED) != 0) &&
        ((reason & LWIP_NSC_IPV4_NETMASK_CHANGED) != 0) &&
        ((reason & LWIP_NSC_IPV4_GATEWAY_CHANGED) != 0) &&
        ((reason & LWIP_NSC_IPV4_SETTINGS_CHANGED) != 0)) {
        if ((reason & LWIP_NSC_IPV4_ADDR_VALID) != 0) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_get_ip_status_from_lwip::connected, reason=0x%x}", reason);
            netifapi_netif_get_addr(netdev->lwip_netif, &ipaddr, &netmask, &gw);
            frw_util_notifier_notify(WLAN_UTIL_NOTIFIER_EVENT_INETADDR_NOTIFIER_UP, &ipaddr);
            ip_stat = 1;
        } else {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_get_ip_status_from_lwip::disconnected, reason=0x%x}", reason);
            ip_stat = 0;
        }
    }

    ret = wal_async_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_IP_STATUS,
        (osal_u8 *)&ip_stat, OAL_SIZEOF(ip_stat), FRW_POST_PRI_LOW);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_get_ip_status_from_lwip::send W2H msg failed}");
        return;
    }
}
#endif
#endif

static osal_u32 wal_netifapi_netif_add_ext_callback(osal_void)
{
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#if defined(LWIP_NETIF_EXT_STATUS_CALLBACK) && LWIP_NETIF_EXT_STATUS_CALLBACK
    if (netifapi_netif_add_ext_callback(&callback, wal_get_ip_status_from_lwip) != ERR_OK) {
        oam_error_log0(0, OAM_SF_CFG, "wal_netifapi_netif_add_ext_callback:: netifapi_netif_add_ext_callback failed");
        return OAL_FAIL;
    }
#endif
#endif

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MFG_ONLY
td_u32 init_addr_adapter(oal_net_device_stru *netdev)
{
    return wlan_get_mac(netdev->dev_addr, sizeof(netdev->dev_addr), netdev);
}
#endif

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
static void wal_add_vap_set_cust_nv_customize(hmac_cfg_add_vap_param_stru *add_vap_param)
{
    uint8_t priv_value = 0;
    uint16_t priv_value_len = 0;
    uint32_t nv_ret = OAL_FAIL;

    nv_ret = uapi_nv_read(NV_ID_ER_SU_DISABLE, sizeof(priv_value), &priv_value_len, &priv_value);
    if (nv_ret == OAL_SUCC) {
        add_vap_param->add_vap.er_su_disable = (priv_value == 0) ? 0 : 1;
    }

    nv_ret = uapi_nv_read(NV_ID_DCM_CONSTELLATION_TX, sizeof(priv_value), &priv_value_len, &priv_value);
    if (nv_ret == OAL_SUCC) {
        add_vap_param->add_vap.dcm_constellation_tx = (priv_value > 0x3) ? 0x3 : priv_value;
    }

    nv_ret = uapi_nv_read(NV_ID_BANDWIDTH_EXTENDED_RANGE, sizeof(priv_value), &priv_value_len, &priv_value);
    if (nv_ret == OAL_SUCC) {
        add_vap_param->add_vap.bandwidth_extended_range = (priv_value == 0) ? 0 : 1;
    }
}
#endif

/*****************************************************************************
 函 数 名  : wal_init_wlan_vap
 功能描述  : 初始化wlan0,p2p0的vap
*****************************************************************************/
td_u32 wal_init_wlan_vap(oal_net_device_stru *netdev)
{
    wlan_vap_mode_enum_uint8  vap_mode;
    wlan_p2p_mode_enum_uint8  p2p_mode = WLAN_LEGACY_VAP_MODE;
    hmac_cfg_add_vap_param_stru vap_info;
    osal_u32 ret;

#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    if ((netdev->ml_priv) != OSAL_NULL) {
        return OAL_SUCC;
    }

    oal_wireless_dev *wdev = OAL_NETDEVICE_WDEV(netdev);
    oal_net_device_stru *netdev_cfg = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if ((wdev == OSAL_NULL) || (netdev_cfg == OSAL_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 仅用于WIFI和AP打开时创建VAP */
    if (wal_get_vap_p2p_mode(wdev, &vap_mode, &p2p_mode) == EXT_ERR_SUCCESS) {
        return EXT_ERR_SUCCESS;
    }
#else
    oal_net_device_stru   *netdev_cfg = netdev;
    if (init_addr_adapter(netdev)) {
        oam_error_log0(0, OAM_SF_CFG, "wal_init_wlan_vap:: get_dev_addr fail.");
        return OAL_FAIL;
    }
    vap_mode = WLAN_VAP_MODE_BSS_STA;
#endif

    if (wal_netifapi_netif_add_ext_callback() != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_CFG, "wal_init_wlan_vap:: wal_netifapi_netif_add_ext_callback failed");
        return OAL_FAIL;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    /* 填写消息 */
    (osal_void)memset_s(&vap_info, OAL_SIZEOF(vap_info), 0, OAL_SIZEOF(vap_info));

    vap_info.net_dev = netdev;
    vap_info.add_vap.vap_mode  = vap_mode;
    vap_info.add_vap.cfg_vap_indx = WLAN_CFG_VAP_ID;
    vap_info.add_vap.p2p_mode  = p2p_mode;
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    vap_info.add_vap.disable_capab_2ght40 = ((wlan_customize_stru *)hwifi_wlan_customize_etc())->disable_capab_2ght40;
    wal_add_vap_set_cust(&vap_info);
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    wal_add_vap_set_cust_nv_customize(&vap_info);
#endif
#endif

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(WLAN_CFG_VAP_ID, WLAN_MSG_W2H_CFG_ADD_VAP,
        (osal_u8 *)&vap_info, OAL_SIZEOF(vap_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(WLAN_CFG_VAP_ID, OAM_SF_ANY, "{wal_init_wlan_vap::return err code %u!}", ret);
        wifi_printf("{wal_init_wlan_vap::return err code %u!}\r\n", ret);
        return ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : wal_deinit_wlan_vap
 功能描述  : 仅用于WIFI和AP关闭时删除VAP
*****************************************************************************/
td_u32 wal_deinit_wlan_vap(oal_net_device_stru *netdev)
{
    hmac_vap_stru                *mac_vap = OSAL_NULL;
    td_u32                       ret;
    hmac_cfg_del_vap_param_stru vap_info;

    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_vap::del_vap_param null ptr !}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_deinit_wlan_vap::mac_vap is already null}\r\n");
        return OAL_SUCC;
    }

    (osal_void)memset_s(&vap_info, OAL_SIZEOF(vap_info), 0, OAL_SIZEOF(vap_info));
    /* 删除vap 时需要将参数赋值 */
    vap_info.net_dev = netdev;
#ifdef _PRE_WLAN_FEATURE_P2P
    vap_info.add_vap.p2p_mode = mac_vap->p2p_mode;
#endif
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_DEL_VAP,
        (osal_u8 *)&vap_info, OAL_SIZEOF(vap_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_deinit_wlan_vap::return error code %d}\r\n", ret);
        wifi_printf("{wal_deinit_wlan_vap::return error code %d}\r\n", ret);
    }

    netdev->ml_priv = NULL;

    return ret;
}

/*****************************************************************************
 函 数 名  : wal_get_mac_addr
 功能描述  : 判断mac地址是否合法
*****************************************************************************/
td_u32 wal_macaddr_check(const td_u8 *mac_addr)
{
    if ((hmac_addr_is_zero_etc((osal_u8 *)mac_addr) == OSAL_TRUE) || ((mac_addr[0] & 0x1) == 0x1)) {
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : wal侧通过change virtual iftype创建vap的接口
 修改历史      :
  1.日    期   : 2015年12月2日
    修改内容   : 新生成函数
  2.    日    期   : 2019年2月12日
    修改内容   : 修改函数名为wal_setup_vap，将iftype赋值移到函数外

*****************************************************************************/
td_u32  wal_setup_vap(oal_net_device_stru *netdev)
{
    td_u32 ret = OAL_FAIL;
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    if ((OAL_NETDEVICE_FLAGS(netdev) & OAL_IFF_RUNNING) != 0) {
        /* 切换到Mesh前如果网络设备处于UP状态，需要先down 网络设备 */
        oam_info_log0(0, 0, "wal_setup_vap:stop netdevice.");
        wal_netdev_stop(netdev);

        ret = wal_deinit_wlan_vap(netdev);
        if (ret != OAL_SUCC) {
            oam_info_log0(0, 0, "wal_setup_vap:wlan_deinit_wlan_vap failed");
            return ret;
        }
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#if defined(LWIP_IPV6) && LWIP_IPV6
    struct netif *netif = netifapi_netif_find_by_name((const td_char *)netdev->name);
    if (netif != OSAL_NULL) {
        /* 设置IPv6 linklocal address(SLAAC) */
        if ((netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_AP) ||
            (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_MESH_POINT)) {
            (td_void)netifapi_set_ip6_autoconfig_disabled(netdev->lwip_netif);
#ifdef _PRE_WLAN_FEATURE_LWIP_ND6_ROUTER
            (td_void)netifapi_set_accept_ra(netif, OSAL_FALSE);
            (td_void)netifapi_set_ipv6_forwarding(netif, OSAL_TRUE);
            (td_void)netifapi_set_ra_enable(netif, OSAL_TRUE);
#endif
            (td_void)netifapi_netif_add_ip6_linklocal_address(netif, OSAL_TRUE);
        }
    }
#endif // LWIP_IPV6
#endif // _PRE_WLAN_USE_CUSTOM_LWIP
#endif
    ret = wal_init_wlan_vap(netdev);

    return ret;
}

td_u32 uapi_ccpriv_show_frag_threshold(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_s32 ret;
    td_u32 data = 0;
    unref_param(param);
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SHOW_FRAG_THRESHOLD_REG,
        (td_u8 *)&data, OAL_SIZEOF(data));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_show_frag_threshold::return err code [%d]!}", ret);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

