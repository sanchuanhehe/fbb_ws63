/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Linux cfg80211 interface.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_scan.h"
#include "wal_main.h"
#include "wal_ioctl.h"
#include "wal_ccpriv.h"
#include "wal_net.h"
#include "soc_customize_wifi.h"
#include "wal_event_msg.h"
#include "wal_wpa_ioctl.h"
#include "wal_cfg80211_apt.h"
#include "oal_net.h"
#include "mac_frame.h"
#include "mac_ie.h"

#include "mac_vap_ext.h"
#include "hmac_user.h"
#include "mac_resource_ext.h"
#include "hmac_feature_dft.h"
#include "hmac_feature_interface.h"
#include "hmac_11r.h"
#include "hmac_wapi.h"
#include "wal_cfg80211.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_CFG80211_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#define MILLISECOND_PER_TICK   10
#define WIFI_G_RATES           (g_wifi_rates + 0)
#define WIFI_G_RATES_SIZE      12

#ifndef hi_makeu16
#define hi_makeu16(a, b)       ((osal_u16)(((osal_u8)(a)) | (((osal_u16)((osal_u8)(b))) << 8)))
#endif

/* 设备支持的速率 */
static oal_ieee80211_rate g_wifi_rates[] = {
    ratetab_ent(10,  0x1,   0),
    ratetab_ent(20,  0x2,   0),
    ratetab_ent(55,  0x4,   0),
    ratetab_ent(110, 0x8,   0),
    ratetab_ent(60,  0x10,  0),
    ratetab_ent(90,  0x20,  0),
    ratetab_ent(120, 0x40,  0),
    ratetab_ent(180, 0x80,  0),
    ratetab_ent(240, 0x100, 0),
    ratetab_ent(360, 0x200, 0),
    ratetab_ent(480, 0x400, 0),
    ratetab_ent(540, 0x800, 0),
};

/* 2.4G 频段 */
oal_ieee80211_channel g_wifi_2ghz_channels[] = {
    chan2g(1, 2412, 0),
    chan2g(2, 2417, 0),
    chan2g(3, 2422, 0),
    chan2g(4, 2427, 0),
    chan2g(5, 2432, 0),
    chan2g(6, 2437, 0),
    chan2g(7, 2442, 0),
    chan2g(8, 2447, 0),
    chan2g(9, 2452, 0),
    chan2g(10, 2457, 0),
    chan2g(11, 2462, 0),
    chan2g(12, 2467, 0),
    chan2g(13, 2472, 0),
    chan2g(14, 2484, 0),
#ifdef _PRE_WLAN_FEATURE_015CHANNEL_EXP
    chan2g(15, 2512, 0),
    chan2g(16, 2384, 0),
#endif
};

/* 2.4G 频段信息 */
static oal_ieee80211_supported_band g_wifi_band_2ghz = {
    .channels   = g_wifi_2ghz_channels,
    .n_channels = sizeof(g_wifi_2ghz_channels) / sizeof(oal_ieee80211_channel),
    .bitrates   = (oal_ieee80211_rate *)WIFI_G_RATES,
    .n_bitrates = WIFI_G_RATES_SIZE,
    .ht_cap = {
        .ht_supported = OSAL_TRUE,
        .cap = IEEE80211_HT_CAP_SGI_20,
    },
};

#ifdef _PRE_WLAN_FEATURE_P2P
td_u8               g_cookie_array_bitmap = 0;   /* 每个bit 表示cookie array 中是否使用，1 - 已使用；0 - 未使用 */
cookie_arry_stru    g_cookie_array[WAL_COOKIE_ARRAY_SIZE];
#endif
/*****************************************************************************
  3 函数实现
*****************************************************************************/
oal_ieee80211_channel* wal_get_g_wifi_2ghz_channels(td_void)
{
    return g_wifi_2ghz_channels;
}

oal_ieee80211_supported_band* wal_get_wifi_2ghz_band(td_void)
{
    return &g_wifi_band_2ghz;
}

/*****************************************************************************
 功能描述  : 解析内核传递过来的beacon信息中的WPA/WPA2 信息元素
*****************************************************************************/
OAL_STATIC td_u32 wal_parse_wpa_wpa2_ie(const oal_beacon_parameters *beacon_info, mac_beacon_param_stru *beacon_param)
{
    const td_u8         *rsn_ie = OSAL_NULL;
    td_u8               *wpa_ie = OSAL_NULL;
    oal_ieee80211_mgmt  *mgmt   = OSAL_NULL;
    td_u16               capability_info;

    /* 判断是否加密 */
    mgmt = (oal_ieee80211_mgmt *)beacon_info->head;
    capability_info = mgmt->u.beacon.capab_info;
    beacon_param->privacy = OSAL_FALSE;
    if ((WLAN_WITP_CAPABILITY_PRIVACY & capability_info) != 0) {
        beacon_param->privacy = OSAL_TRUE;

        /* 查找 RSN 信息元素 */
        rsn_ie = mac_find_ie_etc(MAC_EID_RSN, beacon_info->tail, beacon_info->tail_len);
        if (rsn_ie != OSAL_NULL) {
            if (memcpy_s(beacon_param->rsn_ie, sizeof(beacon_param->rsn_ie), rsn_ie, MAC_MAX_RSN_LEN) != EOK) {
                oam_error_log0(0, OAM_SF_ANY, "{wal_parse_wpa_wpa2_ie::memcpy_s rsn_ie error}");
            }
        }

        /* 查找 WPA 信息元素，并解析出认证类型 */
        wpa_ie = hmac_find_vendor_ie_etc(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA, beacon_info->tail,
            (td_s32)beacon_info->tail_len);
        if (wpa_ie != OSAL_NULL) {
            if (memcpy_s(beacon_param->wpa_ie, sizeof(beacon_param->wpa_ie), wpa_ie, MAC_MAX_RSN_LEN) != EOK) {
                oam_error_log0(0, OAM_SF_ANY, "{wal_parse_wpa_wpa2_ie::memcpy_s wpa_ie error}");
            }
        }
    }
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 功能描述  : 解析内核传递过来beacon信息中的mesh configuration信息元素
*****************************************************************************/
td_u32 wal_parse_mesh_conf_ie(const oal_beacon_parameters *beacon_info, mac_beacon_param_stru *beacon_param)
{
    const td_u8 *mesh_conf_ie = OSAL_NULL;
    td_u8 index = 0;

    /* 查找 mesh conf信息元素 */
    mesh_conf_ie = mac_find_ie_etc(MAC_EID_MESH_CONF, beacon_info->tail, beacon_info->tail_len);
    if (mesh_conf_ie != OSAL_NULL) {
        /* 根据Mesh Conf信息元素解析出mesh配置项 */
        /* 长度若小于2，取后面的值都会异常 */
        if (mesh_conf_ie[1] < MAC_MIN_MESH_CONF_LEN) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_parse_mesh_conf_ie::invalid mesh conf IE len[%d]!}\r\n",
                mesh_conf_ie[1]);
            return OAL_FAIL;
        }

        index += 6; /* 6: 忽略 Mesh conf IE 和 IE 长度 ,选路算法及用塞控制 */
        beacon_param->mesh_auth_protocol = mesh_conf_ie[index++];
        beacon_param->mesh_formation_info = mesh_conf_ie[index++];
        beacon_param->mesh_capability = mesh_conf_ie[index];
        oam_warning_log3(0, OAM_SF_ANY,
            "{wal_parse_mesh_conf_ie::auth_protocol = %d, formation_info = %d, capability = %d!}",
            beacon_param->mesh_auth_protocol, beacon_param->mesh_formation_info, beacon_param->mesh_capability);
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_parse_mesh_conf_ie::mesh vap can't find mesh conf ie!}");
        return OAL_FAIL;
    }
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : wal_check_support_basic_rate_6M
 功能描述  : 判断指数速率集和扩展速率集中是否包含6M速率作为基本速率
 输入参数  : [1]supported_rates_ie
             [2]supported_rates_num
             [3]extended_supported_rates_ie
             [4]extended_supported_rates_num
 输出参数  : 无
 返 回 值  : static td_bool : OSAL_TRUE    支持
                              OSAL_FALSE   不支持
*****************************************************************************/
static td_bool wal_check_support_basic_rate_6m(const td_u8 *supported_rates_ie,
                                               td_u8 supported_rates_num,
                                               const td_u8 *extended_supported_rates_ie,
                                               td_u8 extended_supported_rates_num)
{
    td_u8     loop;
    td_bool support = OSAL_FALSE;
    for (loop = 0; loop < supported_rates_num; loop++) {
        if (supported_rates_ie == OSAL_NULL) {
            break;
        }
        if (supported_rates_ie[2 + loop] == 0x8c) { /* 2：偏移位 */
            support = OSAL_TRUE;
        }
    }

    for (loop = 0; loop < extended_supported_rates_num; loop++) {
        if (extended_supported_rates_ie == OSAL_NULL) {
            break;
        }
        if (extended_supported_rates_ie[2 + loop] == 0x8c) { /* 2：偏移位 */
            support = OSAL_TRUE;
        }
    }

    return support;
}

/*****************************************************************************
 功能描述  : 解析协议模式
*****************************************************************************/
static td_u32 wal_parse_protocol_mode(wlan_channel_band_enum_uint8 band, const oal_beacon_parameters *beacon_info,
    const td_u8 *ht_ie, wlan_protocol_enum_uint8 *pen_protocol)
{
    td_u8   *supported_rates_ie             = OSAL_NULL;
    td_u8   *extended_supported_rates_ie    = OSAL_NULL;
    td_u8   *he_capab_ie                    = OSAL_NULL;
    td_u8    supported_rates_num             = 0;
    td_u8    extended_supported_rates_num    = 0;
    td_u16   offset;

    he_capab_ie = hmac_find_ie_ext_ie(MAC_EID_HE, MAC_EID_EXT_HE_CAP, beacon_info->tail, beacon_info->tail_len);
    if (he_capab_ie != OSAL_NULL) {
        *pen_protocol = WLAN_HE_MODE;
        return OAL_SUCC;
    }

    if (ht_ie != OSAL_NULL) {
        /* 设置AP 为11n 模式 */
        *pen_protocol = WLAN_HT_MODE;
        return OAL_SUCC;
    }

    if (band == WLAN_BAND_2G) {
        offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
        if (beacon_info->head_len > offset) {
            supported_rates_ie = mac_find_ie_etc(MAC_EID_RATES,
                beacon_info->head + 24 + offset, beacon_info->head_len - offset); /* mac header长度24 */
            if (supported_rates_ie != OSAL_NULL) {
                supported_rates_num = supported_rates_ie[1];
            }
        }
        extended_supported_rates_ie = mac_find_ie_etc(MAC_EID_XRATES, beacon_info->tail, beacon_info->tail_len);
        if (extended_supported_rates_ie != OSAL_NULL) {
            extended_supported_rates_num = extended_supported_rates_ie[1];
        }

        if (supported_rates_num + extended_supported_rates_num == 4) { /* 判断总IE长度是否为4，选择模式 */
            *pen_protocol = WLAN_LEGACY_11B_MODE;
            return OAL_SUCC;
        }
        if (supported_rates_num + extended_supported_rates_num == 8) { /* 判断总IE长度是否为8，选择模式 */
            *pen_protocol = WLAN_LEGACY_11G_MODE;
            return OAL_SUCC;
        }
        if (supported_rates_num + extended_supported_rates_num == 12) { /* 判断总IE长度是否为12，选择模式 */
            /* 根据基本速率区分为 11gmix1 还是 11gmix2 */
            /* 如果基本速率集支持 6M , 则判断为 11gmix2 */
            *pen_protocol = WLAN_MIXED_ONE_11G_MODE;
            if (wal_check_support_basic_rate_6m(supported_rates_ie, supported_rates_num,
                                                extended_supported_rates_ie,
                                                extended_supported_rates_num) == OSAL_TRUE) {
                *pen_protocol = WLAN_MIXED_TWO_11G_MODE;
            }
            return OAL_SUCC;
        }
    }
    /* 其他情况，认为配置不合理 */
    *pen_protocol = WLAN_PROTOCOL_BUTT;

    return OAL_FAIL;
}

/*****************************************************************************
 功能描述  : 解析内核传递过来的beacon信息中的ht_vht 信息元素
 输入参数  : oal_beacon_parameters *beacon_info
             mac_beacon_param_stru *beacon_param
*****************************************************************************/
static td_u32 wal_parse_ht_vht_ie(const hmac_vap_stru *mac_vap, const oal_beacon_parameters  *beacon_info,
    mac_beacon_param_stru  *beacon_param)
{
    td_u8 *ht_ie  = mac_find_ie_etc(MAC_EID_HT_CAP, beacon_info->tail, beacon_info->tail_len);

    /* 解析协议模式 */
    td_u32 ret = wal_parse_protocol_mode(mac_vap->channel.band, beacon_info, ht_ie, &beacon_param->protocol);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_parse_ht_vht_ie::return err code!}\r\n", ret);
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    /* 定制化实现P2P GO 2.4G下默认支持11ac 协议模式 */
    if (is_p2p_go(mac_vap) && (WLAN_BAND_2G == mac_vap->channel.band)) {
        beacon_param->protocol = ((OSAL_TRUE == mac_vap->cap_flag.support_11ac2g) ? WLAN_VHT_MODE : WLAN_HT_MODE);
    }
#endif /* _PRE_WLAN_FEATURE_P2P */

    /* 解析short gi能力 */
    if (ht_ie == OSAL_NULL) {
        return OAL_SUCC;
    }

    /* 使用ht cap ie中数据域的2个字节 */
    if (ht_ie[1] < sizeof(mac_frame_ht_cap_stru)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_parse_ht_vht_ie::invalid htcap ie len %d}\n", ht_ie[1]);
        return OAL_SUCC;
    }

    mac_frame_ht_cap_stru *ht_cap = (mac_frame_ht_cap_stru *)(ht_ie + MAC_IE_HDR_LEN);

    beacon_param->shortgi_20 = (td_u8)ht_cap->short_gi_20mhz;
    beacon_param->shortgi_40 = 0;

    if ((mac_vap->channel.en_bandwidth > WLAN_BAND_WIDTH_20M) &&
        (mac_vap->channel.en_bandwidth != WLAN_BAND_WIDTH_BUTT)) {
        beacon_param->shortgi_40 = (td_u8)ht_cap->short_gi_40mhz;
    }

    beacon_param->smps_mode = (td_u8)ht_cap->sm_power_save;
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_set_pmksa
 功能描述  : 添加pmkid 缓存信息
 输入参数  : [1]netdev
             [2]pmkid_params
 输出参数  : td_u32
 返 回 值  : 0:成功,其他:失败
*****************************************************************************/
td_u32 wal_cfg80211_set_pmksa(oal_net_device_stru *netdev, const ext_pmkid_params *pmkid_params)
{
    mac_cfg_pmksa_param_stru payload_params = {0};
    td_u32 ret;

    /* 1.1 入参检查 */
    if ((netdev == OSAL_NULL) || (pmkid_params == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_set_pmksa::Param Check ERROR,netdev, params %p, %p!}",
            (uintptr_t)netdev, (uintptr_t)pmkid_params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1.2 pmkid检查，防止拷贝越界 */
    if (pmkid_params->pmkid == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_pmksa::Param Check ERROR! pmkid[%p]!}\r\n",
            (uintptr_t)pmkid_params->pmkid);
        return OAL_FAIL;
    }

    /* 2.1 消息参数准备 */
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
    if ((memcpy_s(payload_params.bssid, WLAN_MAC_ADDR_LEN, pmkid_params->bssid, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(payload_params.pmkid, WLAN_PMKID_LEN, pmkid_params->pmkid, WLAN_PMKID_LEN) != EOK)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_set_pmksa::mem safe function err!}");
        return OAL_FAIL;
    }
    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_SET_PMKSA,
        (osal_u8 *)&payload_params, OAL_SIZEOF(mac_cfg_pmksa_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_pmksa::return err code [%u]!}", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_remove_pmksa
 功能描述  : 删除pmkid 缓存信息
 输入参数  : [1]netdev
             [2]pmkid_params
 输出参数  : td_u32
 返 回 值  : 0:成功,其他:失败
*****************************************************************************/
td_u32 wal_cfg80211_remove_pmksa(oal_net_device_stru *netdev, const ext_pmkid_params *pmkid_params)
{
    mac_cfg_pmksa_param_stru payload_params = {0};
    td_u32 ret;

    /* 1.1 入参检查 */
    if ((netdev == OSAL_NULL) || (pmkid_params == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_remove_pmksa::Param Check ERROR,netdev, params %p, %p!}",
            (uintptr_t)netdev, (uintptr_t)pmkid_params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1.2 pmkid检查，防止拷贝越界 */
    if (pmkid_params->pmkid == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_remove_pmksa::Param Check ERROR! pmkid[%p]!}\r\n",
            (uintptr_t)pmkid_params->pmkid);
        return OAL_FAIL;
    }

    /* 2.1 消息参数准备 */
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
    if ((memcpy_s(payload_params.bssid, WLAN_MAC_ADDR_LEN, pmkid_params->bssid, WLAN_MAC_ADDR_LEN) != EOK) ||
        (memcpy_s(payload_params.pmkid, WLAN_PMKID_LEN, pmkid_params->pmkid, WLAN_PMKID_LEN) != EOK)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_remove_pmksa::mem safe function err!}");
        return OAL_FAIL;
    }
    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_DEL_PMKSA,
        (osal_u8 *)&payload_params, OAL_SIZEOF(mac_cfg_pmksa_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_remove_pmksa::return err code [%u]!}", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_flush_pmksa
 功能描述  : 删除所有pmkid 缓存信息
 输入参数  : [1]netdev
 输出参数  : td_u32
 返 回 值  : 0:成功,其他:失败
*****************************************************************************/
td_u32 wal_cfg80211_flush_pmksa(oal_net_device_stru *netdev)
{
    td_u32 ret;
    td_u8 params = 0;

    /* 1.1 入参检查 */
    if (netdev == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_flush_pmksa::Param Check ERROR,netdev, params %p!}",
            (uintptr_t)netdev);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_FLUSH_PMKSA,
        (osal_u8 *)&params, sizeof(params));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_flush_pmksa::return err code [%u]!}", ret);
    }
    return ret;
}

#ifdef _PRE_WLAN_FEATURE_WPA3
/*****************************************************************************
 功能描述  : 根据rsn_ie获取PMKID
******************************************************************************/
OAL_STATIC td_u16 wal_get_rsn_pmkid(const td_u8 *rsn_ie)
{
    td_u16  pairwise_count;
    td_u16  akm_count;
    td_u16  pmkid_count;
    td_u16  us_index = 0;

    if (rsn_ie == OSAL_NULL) {
        return 0;
    }
    /*************************************************************************/
    /*                  RSN Element Format                                   */
    /* --------------------------------------------------------------------- */
    /* |Element ID | Length | Version | Group Cipher Suite | Pairwise Cipher */
    /* --------------------------------------------------------------------- */
    /* |     1     |    1   |    2    |         4          |       2         */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* Suite Count| Pairwise Cipher Suite List | AKM Suite Count | AKM Suite List */
    /* --------------------------------------------------------------------- */
    /*            |         4*m                |     2           |   4*n     */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* |RSN Capabilities|PMKID Count|PMKID List|Group Management Cipher Suite */
    /* --------------------------------------------------------------------- */
    /* |        2       |    2      |   16 *s  |               4           | */
    /* --------------------------------------------------------------------- */
    /*                                                                       */
    /*************************************************************************/
    if (rsn_ie[1] < MAC_MIN_RSN_LEN) {
        oam_warning_log1(0, OAM_SF_WPA, "{wal_get_rsn_pmkid::invalid rsn ie len[%d].}", rsn_ie[1]);
        return 0;
    }
    us_index += 8; /* 偏移8 byte，获取pairwise_count */
    pairwise_count = hi_makeu16(rsn_ie[us_index], rsn_ie[us_index + 1]);
    if (pairwise_count > MAC_PAIRWISE_CIPHER_SUITES_NUM) {
        oam_warning_log1(0, OAM_SF_WPA, "{wal_get_rsn_pmkid::invalid pairwise_count[%d].}", pairwise_count);
        return 0;
    }
    us_index += 2 + 4 * (td_u8)pairwise_count; /* 再偏移(2 + 4 * pairwise_count) byte，获取akm_count */
    akm_count = hi_makeu16(rsn_ie[us_index], rsn_ie[us_index + 1]);
    if (akm_count > MAC_AUTHENTICATION_SUITE_NUM) {
        oam_warning_log1(0, OAM_SF_WPA, "{wal_get_rsn_pmkid::invalid akm_count[%d].}", akm_count);
        return 0;
    }
    us_index += 2 + 4 * (td_u8)akm_count; /* 再偏移(2 + 4 * akm_count) byte，获取rsn_capability */
    us_index += 2;                           /* 再偏移rsn_capability的长度 */

    if ((us_index + 2) > (rsn_ie[1] + MAC_IE_HDR_LEN)) { // 2:确保有足够的长度包含PMKID COUNT IE
        pmkid_count = 0;
    } else {
        pmkid_count = hi_makeu16(rsn_ie[us_index], rsn_ie[us_index + 1]);
    }

    return pmkid_count;
}
#endif
/*****************************************************************************
 功能描述  : 内核调用启动扫描的接口函数
*****************************************************************************/
td_u32 wal_cfg80211_scan(oal_cfg80211_scan_request_stru *request, hmac_scan_stru *scan_mgmt)
{
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    oal_net_device_stru *netdev = OSAL_NULL;
    osal_void *fhook = hmac_get_feature_fhook(HMAC_FHOOK_WAPI_CONNECT_CHECK);

    if (request == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::parameter is wrong, return fail!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oam_info_log2(0, OAM_SF_SCAN, "{wal_cfg80211_scan::request to scan, channel number is %d, ssid number is %d}",
        request->n_channels, request->n_ssids);

    netdev = (request->wdev == OSAL_NULL ? OSAL_NULL : request->wdev->netdev);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::acquire netdev fail, return fail!}");
        return OAL_FAIL;
    }

    /* 通过net_device 找到对应hmac_vap_struu 结构 */
    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if ((hmac_vap == NULL) || (((hmac_vap->vap_state == MAC_VAP_STATE_UP) ||
        (hmac_vap->vap_state == MAC_VAP_STATE_PAUSE)) && (hmac_vap->not_scan_flag == 1))) {
        oam_error_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::do not scan, return fail!}");
        return OAL_FAIL;
    }
    if ((fhook != OSAL_NULL) && (is_p2p_scan_req(request)) &&
            (((hmac_wapi_connected_etc_cb)fhook)(hmac_vap->device_id) == OAL_TRUE)) {
        oam_warning_log0(0, OAM_SF_SCAN, "{wal_cfg80211_scan::cann't execute p2p scan under wapi mode, return!}");
        return OAL_FAIL;
    }

    /* 进入扫描 */
    if (wal_start_scan_req(netdev, scan_mgmt) != OAL_SUCC) {
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

td_s32 uapi_wifi_app_service(const td_char *ifname, td_void *buf)
{
    td_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    wifi_app_common_param *param = OSAL_NULL;

    if (buf == NULL) {
        return -1;
    }

    /* 判断网络设备是否存在 */
    netdev = oal_get_netdev_by_name(ifname);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, 0, "{uapi_wifi_app_service device not fonud.}");
        return -1;
    }

    param = (wifi_app_common_param *)buf;

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_WIFI_APP_SERVICE,
        (osal_u8 *)param, param->input_para.length);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_app_service::return err code %d!}", ret);
    }
    return ret;
}

/*****************************************************************************
 功能描述  : 配置wep加密信息
*****************************************************************************/
static td_u32 wal_set_wep_key(mac_conn_param_stru *connect_param,
                              const oal_cfg80211_connect_params_stru *sme)
{
    connect_param->wep_key           = sme->key;
    connect_param->wep_key_len        = sme->key_len;
    connect_param->wep_key_index      = sme->key_idx;
    connect_param->crypto.group_suite = (td_u8)sme->crypto.cipher_group;

    oam_warning_log4(0, 0, "wal_set_wep_key::key_len[%d], key_idx[%d], cipher_group:0x%x, ul_group_suite 0x%x",
                     sme->key_len, sme->key_idx, sme->crypto.cipher_group, connect_param->crypto.group_suite);

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 处理使能PMF STAUT下发n_akm_suites==0的RSN特殊情况
 输入参数  : mac_conn_param_stru   *connect_param
             oal_cfg80211_connect_params_stru    *sme
 返 回 值  : static td_u32
*****************************************************************************/
static td_u32 wal_set_crypto_pmf(mac_conn_param_stru *connect,
    const oal_cfg80211_connect_params_stru *sme, const td_u8 *ie)
{
    /* 设置WPA/WPA2 加密信息 */
    connect->crypto.control_port = (td_u8)sme->crypto.control_port;
    connect->crypto.wpa_versions = (td_u8)sme->crypto.wpa_versions;

    /* 获取group cipher type */
    connect->crypto.group_suite = ie[MAC_IE_HDR_LEN + MAC_RSN_VERSION_LEN + MAC_OUI_LEN];

    /* 获取pairwise cipher cout */
    td_u32 offset = MAC_IE_HDR_LEN + MAC_RSN_VERSION_LEN + MAC_OUI_LEN + MAC_OUITYPE_WPA;
    connect->crypto.n_ciphers_pairwise = ie[offset];
    connect->crypto.n_ciphers_pairwise += (td_u8)(ie[offset + 1] << 8); /* 左移8位 */
    if (connect->crypto.n_ciphers_pairwise > WLAN_PAIRWISE_CIPHER_SUITES) {
        oam_warning_log1(0, 0, "{wal_set_crypto_pmf:invalid ciphers len:%d!}", connect->crypto.n_ciphers_pairwise);
        return OAL_FAIL;
    }
    /* 获取pairwise cipher type */
    offset += MAC_RSN_CIPHER_COUNT_LEN;
    if (connect->crypto.n_ciphers_pairwise != 0) {
        for (td_u8 loop2 = 0; loop2 < connect->crypto.n_ciphers_pairwise; loop2++) {
            connect->crypto.pair_suite[loop2] = (td_u8)ie[offset + MAC_OUI_LEN];
            offset += (MAC_OUITYPE_WPA + MAC_OUI_LEN);
        }
    }

    /* 获取AKM cout */
    connect->crypto.n_akm_suites = ie[offset];
    connect->crypto.n_akm_suites += (td_u8)(ie[offset + 1] << 8); /* 左移8位 */
    if (connect->crypto.n_akm_suites > OAL_NL80211_MAX_NR_AKM_SUITES) {
        oam_warning_log1(0, 0, "{wal_set_crypto_pmf:invalid akm len:%d!}", connect->crypto.n_akm_suites);
        return OAL_FAIL;
    }
    /* 获取AKM type */
    offset += MAC_RSN_CIPHER_COUNT_LEN;
    if (connect->crypto.n_akm_suites != 0) {
        for (td_u8 loop3 = 0; loop3 < connect->crypto.n_akm_suites; loop3++) {
            connect->crypto.akm_suite[loop3] = (td_u8)ie[offset + MAC_OUI_LEN];
            offset += (MAC_OUITYPE_WPA + MAC_OUI_LEN);
        }
    }

    /* 获取PMKID */
#ifdef _PRE_WLAN_FEATURE_WPA3
    offset += MAC_RSN_CAP_LEN;
    td_u16 pmkid_count;
    pmkid_count = ie[offset];
    pmkid_count += (td_u8)(ie[offset + 1] << 8); /* 左移8位 */
    connect->crypto.have_pmkid = (pmkid_count != 0) ? 1 : 0;
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置STA connect 加密信息
*****************************************************************************/
OAL_STATIC td_u32 wal_set_crypto_info(mac_conn_param_stru *connect_param, const oal_cfg80211_connect_params_stru *sme)
{
    td_u8 *ie = mac_find_ie_etc(MAC_EID_RSN, (td_u8 *)sme->ie, (osal_s32)sme->ie_len);

    if ((sme->key_len != 0) && (sme->crypto.n_akm_suites == 0) && (sme->key != OSAL_NULL)) {
        /* 设置wep加密信息 */
        return wal_set_wep_key(connect_param, sme);
    } else if (sme->crypto.n_akm_suites != 0) {
        if ((sme->crypto.n_akm_suites > OAL_NL80211_MAX_NR_AKM_SUITES) ||
            (sme->crypto.n_ciphers_pairwise > OAL_NL80211_MAX_NR_CIPHER_SUITES)) {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_set_crypto_info:invalid suites len!}");
            return OAL_FAIL;
        }
        /* 设置WPA/WPA2 加密信息 */
        connect_param->crypto.wpa_versions       = sme->crypto.wpa_versions;
        connect_param->crypto.group_suite       = sme->crypto.cipher_group;
        connect_param->crypto.n_ciphers_pairwise = (osal_u32)sme->crypto.n_ciphers_pairwise;
        connect_param->crypto.n_akm_suites       = (osal_u32)sme->crypto.n_akm_suites;
        connect_param->crypto.control_port       = sme->crypto.control_port;

        oam_warning_log4(0, OAM_SF_CFG, "wal_set_crypto_info:wpa[%d] group_suite[0x%x] n_ciphers[%d],n_akm_suites[%d]",
                         sme->crypto.wpa_versions, sme->crypto.cipher_group, sme->crypto.n_ciphers_pairwise,
                         sme->crypto.n_akm_suites);

        /* 优先查找 RSN 信息元素 */
        if (ie != OSAL_NULL) {
            hmac_ie_get_rsn_cipher(ie, &connect_param->crypto);
            oam_warning_log4(0, OAM_SF_CFG, "wal_set_crypto_info::rsn_ie pair[0]:0x%x [1]:0x%x, akm[0]:0x%x [1]:0x%x",
                             connect_param->crypto.pair_suite[0], connect_param->crypto.pair_suite[1],
                             connect_param->crypto.akm_suite[0], connect_param->crypto.akm_suite[1]);
        } else {
            /* WPA */
            osal_u8 *ie_tmp = hmac_find_vendor_ie_etc(MAC_WLAN_OUI_MICROSOFT, MAC_OUITYPE_WPA,
                (osal_u8 *)sme->ie, (osal_s32)sme->ie_len);
            if (ie_tmp != OSAL_NULL) {
                hmac_ie_get_wpa_cipher(ie_tmp, &connect_param->crypto);
                oam_warning_log4(0, OAM_SF_CFG, "wal_set_crypto_info:wpa_ie pair[0]:0x%x [1]:0x%x,akm[0]:0x%x [1]:0x%x",
                    connect_param->crypto.pair_suite[0], connect_param->crypto.pair_suite[1],
                    connect_param->crypto.akm_suite[0], connect_param->crypto.akm_suite[1]);
            }
        }

        /* SAE */
#ifdef _PRE_WLAN_FEATURE_WPA3
        if (ie != OSAL_NULL) {
            td_u16 pmkid_count = wal_get_rsn_pmkid(ie);
            connect_param->crypto.have_pmkid = (pmkid_count != 0) ? 1 : 0;
        }
#endif
        return OAL_SUCC;
    } else if (ie != OSAL_NULL) {
        /* 处理使能PMF STAUT下发n_akm_suites==0的RSN特殊情况 */
        return wal_set_crypto_pmf(connect_param, sme, ie);
    } else if (hmac_find_vendor_ie_etc(MAC_WLAN_OUI_MICROSOFT, MAC_WLAN_OUI_TYPE_MICROSOFT_WPS, (td_u8 *)sme->ie,
        (td_s32)(sme->ie_len))) {
        /* 如果使能了WPS，则返回成功 */
        oam_warning_log0(0, OAM_SF_CFG, "{wal_set_crypto_info:connect use wps method!}");

        return OAL_SUCC;
    }

    return OAL_FAIL;
}

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 功能描述  : 判断是否为P2P DEVICE .如果是P2P device，则不允许关联。
*****************************************************************************/
static td_u8 wal_is_p2p_device(const oal_net_device_stru *netdev)
{
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        return OSAL_TRUE;
    }
    if (is_p2p_dev(hmac_vap)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}
#endif

/*****************************************************************************
 功能描述  : 解析内核下发的关联命令，sta启动关联
 修改历史      :
  1.日    期   : 2013年8月27日
    修改内容   : 新生成函数
  2.日    期   : 2013年10月24日
    修改内容   : 增加加密认证相关的处理
*****************************************************************************/
td_u32 wal_cfg80211_connect(oal_net_device_stru *netdev, oal_cfg80211_connect_params_stru *sme)
{
    mac_conn_param_stru mac_cfg80211_connect_param = {0};
    td_u32 ret;
    osal_void *fhook = hmac_get_feature_fhook(HMAC_FHOOK_WAPI_UPDATE_CONNECT_PARAM);

    /* iw接口下发的关联请求，有可能无信道信息，此时会访问空指针，并且此处获取信道号，在入网过程中，并没有用到 */
    /* 解析内核下发的 ssid */
    mac_cfg80211_connect_param.ssid = (td_u8 *)sme->ssid;
    mac_cfg80211_connect_param.ssid_len = (td_u8)sme->ssid_len;

    /* 解析内核下发的 bssid */
    mac_cfg80211_connect_param.bssid = (td_u8 *)sme->bssid;

    /* 解析内核下发的安全相关参数 */
    /* 设置认证类型 */
    mac_cfg80211_connect_param.auth_type = sme->auth_type;

    /* 设置加密能力 */
    mac_cfg80211_connect_param.privacy = sme->privacy;

    /* 获取内核下发的pmf是使能的结果 */
    mac_cfg80211_connect_param.mfp = sme->mfp;

    oam_warning_log4(0, OAM_SF_ANY, "{wal_cfg80211_connect::start new conn,ssid_len=%d,auth_type=%d,privacy=%d,mfp=%d}",
        sme->ssid_len, sme->auth_type, sme->privacy, sme->mfp);

    /* 设置加密参数 */
    if (fhook != OSAL_NULL) {
        mac_cfg80211_connect_param.wapi = ((hmac_wapi_update_connect_param_cb)fhook)(sme->crypto.wpa_versions);
    }
    if (sme->privacy != 0) {
        if (wal_set_crypto_info(&mac_cfg80211_connect_param, sme) != OAL_SUCC) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_connect::connect failed, wal_set_wep_key fail!}");
            return OAL_FAIL;
        }
    }

    /* 设置关联P2P/WPS ie */
    mac_cfg80211_connect_param.ie = (td_u8 *)sme->ie;
    mac_cfg80211_connect_param.ie_len = (td_u32)(sme->ie_len);
#ifdef _PRE_WLAN_FEATURE_WPA3
    mac_cfg80211_connect_param.sae_pwe = (osal_u8)sme->crypto.sae_pwe;
#endif

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_START_CONNECT,
        (osal_u8 *)&mac_cfg80211_connect_param, OAL_SIZEOF(mac_conn_param_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_connect::wal_cfg80211_start_connect fail %u}", ret);
    }
    return ret;
}

/*****************************************************************************
 功能描述  : 发起去关联
*****************************************************************************/
td_u32 wal_cfg80211_disconnect(oal_net_device_stru *netdev, td_u16 reason_code)
{
    mac_cfg_kick_user_param_stru    mac_cfg_kick_user_param = {0};
    td_u32                          ret;
    hmac_user_stru                   *mac_user = OSAL_NULL;
    hmac_vap_stru                    *mac_vap = OSAL_NULL;

    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY,
            "{wal_cfg80211_disconnect::input netdev pointer is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 解析内核下发的去关联原因  */
    mac_cfg_kick_user_param.reason_code = reason_code;

    /* 填写和sta关联的ap mac 地址 */
    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::get mac vap ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    mac_user = mac_res_get_hmac_user_etc(mac_vap->assoc_vap_id);
    if (mac_user == OSAL_NULL) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_disconnect::mac_user_get_user_stru mac_user is null, user idx[%d]!}\r\n",
            mac_vap->assoc_vap_id);
        return OAL_SUCC;
    }

    if (memcpy_s(mac_cfg_kick_user_param.mac_addr, WLAN_MAC_ADDR_LEN,
                 mac_user->user_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::mem safe function err!}");
        return OAL_FAIL;
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_KICK_USER,
        (osal_u8 *)&mac_cfg_kick_user_param, OAL_SIZEOF(mac_cfg_kick_user_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_disconnect::KICK_USER err %d!}", ret);
        wifi_printf("{wal_cfg80211_disconnect::KICK_USER err %d!}\r\n", ret);
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_add_key
 功能描述  : 配置ptk,gtk等密钥到物理层
 输入参数  : [1]wiphy
             [2]netdev
             [3]p_cfg80211_add_key_info
             [4]mac_addr
             [5]params
 输出参数  : td_u32
 返 回 值  : 0:成功,其他:失败
*****************************************************************************/
td_u32 wal_cfg80211_add_key(oal_net_device_stru *netdev, const cfg80211_add_key_info_stru *cfg80211_add_key_info,
    const td_u8 *mac_addr, oal_key_params_stru *params)
{
    mac_addkey_param_stru payload_params = {0};
    td_u32                ret;

    /* 1.1 入参检查 */
    if ((netdev == OSAL_NULL) || (params == OSAL_NULL) || (params->key == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_add_key::Param Check ERROR,netdev, params %p, %p!}",
            (uintptr_t)netdev, (uintptr_t)params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1.2 key长度检查，防止拷贝越界 */
    if ((params->key_len > OAL_WPA_KEY_LEN) || (params->seq_len > OAL_WPA_SEQ_LEN)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_add_key::Param Check ERROR! key_len[%x]  seq_len[%x]!}\r\n",
            (td_s32)params->key_len, (td_s32)params->seq_len);
        return OAL_FAIL;
    }

    /* 2.1 消息参数准备 */
    payload_params.key_index = cfg80211_add_key_info->key_index;
    if (mac_addr != OSAL_NULL) {
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
        if (memcpy_s(payload_params.mac_addr, WLAN_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
            return OAL_FAIL;
        }
    }
    payload_params.pairwise  = cfg80211_add_key_info->pairwise;

    /* 2.2 获取相关密钥值 */
    payload_params.key.key_len = params->key_len;
    payload_params.key.seq_len = params->seq_len;
    payload_params.key.cipher  = params->cipher;
    if (memcpy_s(payload_params.key.key, OAL_WPA_KEY_LEN, params->key, (td_u32)params->key_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
        return OAL_FAIL;
    }

    if (params->seq != OSAL_NULL && params->seq_len != 0) {
        if (memcpy_s(payload_params.key.seq, OAL_WPA_SEQ_LEN, params->seq, (td_u32)params->seq_len) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_add_key::mem safe function err!}");
            return OAL_FAIL;
        }
    }
    oam_info_log3(0, OAM_SF_ANY, "{wal_cfg80211_add_key::key_len:%d, seq_len:%d, cipher:0x%08x!}\r\n",
        params->key_len, params->seq_len, params->cipher);

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_ADD_KEY,
        (osal_u8 *)&payload_params, OAL_SIZEOF(mac_addkey_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_add_key::return err code [%u]!}", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_remove_key
 功能描述  : 把ptk,gtk等密钥从物理层删除
 输入参数  : [1]wiphy
             [2]netdev
             [3]key_index
             [4]pairwise
             [5]mac_addr
 输出参数  : td_u32
 返 回 值  : 0:成功,其他:失败
*****************************************************************************/
td_u32 wal_cfg80211_remove_key(oal_net_device_stru *netdev, td_u8 key_index, td_bool pairwise, const td_u8 *mac_addr)
{
    mac_removekey_param_stru         payload_params  = {0};
    td_u32                           ret;

    /* 1.1 入参检查 */
    if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::Param Check ERROR,netdev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 2.1 消息参数准备 */
    payload_params.key_index = key_index;

    if (mac_addr != OSAL_NULL) {
        /* 不能使用内核下发的mac指针，可能被释放，需要拷贝到本地再使用 */
        if (memcpy_s(payload_params.mac_addr, OAL_MAC_ADDR_LEN, mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::mem safe function err!}");
            return OAL_FAIL;
        }
    }
    payload_params.pairwise  = pairwise;

    oam_info_log2(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::key_index:%d, pairwise:%d!}\r\n", key_index,
        payload_params.pairwise);

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_REMOVE_KEY,
        (osal_u8 *)&payload_params, OAL_SIZEOF(mac_removekey_param_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_remove_key::return err code [%u]!}", ret);
        wifi_printf("{wal_cfg80211_remove_key::return err code [%u]!}\r\n", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_set_default_key
 功能描述  : 使配置的密钥生效
 输入参数  : [1]wiphy
             [2]netdev
             [3]key_index
             [4]unicast
             [5]multicast
 返 回 值  : 0:成功,其他:失败
 修改内容  : 合并设置数据帧默认密钥和设置管理帧默认密钥函数
*****************************************************************************/
td_u32 wal_cfg80211_set_default_key(oal_net_device_stru *netdev, td_u8 key_index, td_bool unicast, td_bool multicast)
{
    unref_param(netdev);
    unref_param(key_index);
    unref_param(unicast);
    unref_param(multicast);
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 启动ap
*****************************************************************************/
static td_u32 wal_cfg80211_set_ssid(oal_net_device_stru *netdev, const td_u8 *ssid_ie, td_u8 ssid_len)
{
    td_u32                       ret;

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_set_ssid(netdev, ssid_ie, ssid_len);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_ssid::return err code [%u]!}", ret);
    }

    return ret;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 功能描述  :设置meshid
 输入参数  : oal_net_device_stru   *netdev
                            td_u8 *ssid_ie
                            td_u8 ssid_len
 返 回 值  : static td_s32
 修改历史      :
  1.日    期   : 2019年3月19日
    修改内容   : 新生成函数

*****************************************************************************/
static td_u32 wal_cfg80211_set_meshid(oal_net_device_stru *netdev, const td_u8 *meshid_ie, td_u8 meshid_len)
{
    mac_cfg_ssid_param_stru      ssid_param = {0};

    /* 2.1 消息参数准备 */
    ssid_param.ssid_len = meshid_len;
    if (memcpy_s(ssid_param.ac_ssid, WLAN_SSID_MAX_LEN, (td_s8 *)meshid_ie, meshid_len) != EOK) {
        oam_error_log0(0, 0, "{wal_cfg80211_set_meshid::mem safe function err!}");
        return OAL_FAIL;
    }

    /* 抛事件给驱动 */
    return OAL_SUCC;
}
#endif

OAL_STATIC td_u32 wal_cfg80211_configuration_beacon(const hmac_vap_stru *mac_vap,
    const oal_beacon_data_stru *beacon_info, mac_beacon_param_stru *beacon_param)
{
    oal_beacon_parameters beacon_info_tmp = {0};

    /*****************************************************************************
        1.安全配置ie消息等
    *****************************************************************************/
    td_u16 beacon_head_len = (td_u16)beacon_info->head_len;
    td_u16 beacon_tail_len = (td_u16)beacon_info->tail_len;
    td_u8  *puc_beacon_info_tmp = (td_u8 *)(malloc(beacon_head_len + beacon_tail_len));
    if (puc_beacon_info_tmp == OSAL_NULL) {
        oam_error_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::memalloc failed.}");
        /* 返回 */
        return OAL_ERR_CODE_PTR_NULL;
    } else {
        /* 复制beacon内容 */
        if ((memcpy_s(puc_beacon_info_tmp, beacon_head_len, beacon_info->head, beacon_head_len) != EOK) ||
            (memcpy_s(puc_beacon_info_tmp + beacon_head_len, beacon_tail_len,
                beacon_info->tail, beacon_tail_len) != EOK)) {
            oam_error_log0(0, 0, "{wal_cfg80211_fill_beacon_param::mem safe function err!}");
            free(puc_beacon_info_tmp);
            return OAL_FAIL;
        }
    }

    beacon_info_tmp.head     = puc_beacon_info_tmp;
    beacon_info_tmp.head_len = beacon_head_len;
    beacon_info_tmp.tail     = puc_beacon_info_tmp + beacon_head_len;
    beacon_info_tmp.tail_len = beacon_tail_len;

    /* 获取 WPA/WPA2 信息元素 */
    td_u32 ret = wal_parse_wpa_wpa2_ie(&beacon_info_tmp, beacon_param);
    if (ret != OAL_SUCC) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::failed to parse WPA/WPA2 ie!}");
        free(puc_beacon_info_tmp);
        return ret;
    }

    /* 此接口仅支持linux，win32不支持 */
    ret = wal_parse_ht_vht_ie(mac_vap, &beacon_info_tmp, beacon_param);
    if (ret != OAL_SUCC) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::failed to parse HT/VHT ie!}");
        free(puc_beacon_info_tmp);
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /* 获取mesh conf信息元素 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        ret = wal_parse_mesh_conf_ie(&beacon_info_tmp, beacon_param);
        if (ret != OAL_SUCC) {
            oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param::Mesh VAP fail parse ie!}");
            free(puc_beacon_info_tmp);
            return ret;
        }
    }
#endif
    /* 释放临时申请的内存 */
    free(puc_beacon_info_tmp);
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 将要下发的修改的beacon帧参数填入到入参结构体中
*****************************************************************************/
static td_u32 wal_cfg80211_fill_beacon_param(hmac_vap_stru *mac_vap,
                                             oal_beacon_data_stru *beacon_info,
                                             mac_beacon_param_stru *beacon_param)
{
    if (beacon_info->tail == OSAL_NULL || beacon_info->head == OSAL_NULL) {
        oam_error_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::beacon frame error tail = %p, head = %p!}",
            (uintptr_t)beacon_info->tail, (uintptr_t)beacon_info->head);
        return OAL_ERR_CODE_PTR_NULL;
    }

    td_u32 ret = wal_cfg80211_configuration_beacon(mac_vap, beacon_info, beacon_param);
    if (ret != OAL_SUCC) {
        return ret;
    }

    oam_warning_log3(mac_vap->vap_id, OAM_SF_ANY,
        "{wal_cfg80211_fill_beacon_param::crypto_mode=%d, group_crypt=%d, protocol=%d!}", beacon_param->crypto_mode,
        beacon_param->group_crypto, beacon_param->protocol);

    oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY,
        "{wal_cfg80211_fill_beacon_param::auth_type[0]=%d, auth_type[1]=%d}", beacon_param->auth_type[0],
        beacon_param->auth_type[1]);

#ifdef _PRE_WLAN_FEATURE_MESH
    /* 打印调试使用 */
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        oam_warning_log2(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::mesh formation info = %d, mesh capability = %d}",
            beacon_param->mesh_formation_info, beacon_param->mesh_capability);
    }
#endif

    /* 对日本14信道作特殊判断，只在11b模式下才能启用14，非11b模式 降为11b */
    if ((mac_vap->channel.chan_number == 14) && (beacon_param->protocol != WLAN_LEGACY_11B_MODE)) { /* 14：代表信道号 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_fill_beacon_param::ch 14 should in 11b, but is %d, change to 11b!}", beacon_param->protocol);
    }
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 修改ap beacon帧配置参数
 输入参数  : oal_net_device_stru     *netdev
             struct cfg80211_beacon_data *info
 返 回 值  : static td_s32
 修改历史      :
  1.日    期   : 2014年12月31日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_cfg80211_change_beacon(oal_net_device_stru *netdev, oal_beacon_data_stru *beacon_info)
{
    mac_beacon_param_stru        beacon_param = {0};  /* beacon info struct */
    hmac_vap_stru                *mac_vap = OSAL_NULL;
    td_u32                       ret;
    /* 参数合法性检查 */
    if ((netdev == OSAL_NULL) || (beacon_info == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY,
            "{wal_cfg80211_change_beacon::netdev = %p, beacon_info = %p!}",
            (uintptr_t)netdev, (uintptr_t)beacon_info);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取vap id */
    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_change_beacon::mac_vap = %p}", (uintptr_t)mac_vap);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 初始化beacon interval 和DTIM_PERIOD 参数 */
    ret = wal_cfg80211_fill_beacon_param(mac_vap, beacon_info, &beacon_param);
    if (ret != OAL_SUCC) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY,
            "{wal_cfg80211_change_beacon::failed to fill beacon param, error[%d]}", ret);
        return ret;
    }

    /* 设置操作类型 */
    beacon_param.operation_type = MAC_SET_BEACON;

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_CONFIG_BEACON,
        (osal_u8 *)&beacon_param, OAL_SIZEOF(mac_beacon_param_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_change_beacon::Failed to start addset beacon, error[%d]!}", ret);
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_set_channel_info
 功能描述  : 设置信道
*****************************************************************************/
static td_u32 wal_cfg80211_set_channel_info(oal_net_device_stru *netdev)
{
    oal_ieee80211_channel *channel = netdev->ieee80211_ptr->preset_chandef.chan;
    td_s32 l_channel      = channel->hw_value;

    /* 判断信道在不在管制域内 */
    osal_u32 ret = hmac_is_channel_num_valid_etc(channel->band, (osal_u8)l_channel);
    if (ret != OAL_SUCC) {
        oam_warning_log3(0, OAM_SF_ANY,
            "{wal_cfg80211_set_channel::channel Err.band=%d,ch=%d,ErrCode=%u}", channel->band, l_channel, ret);
        return ret;
    }

    oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_set_channel::channel=%d,band=%d,bandwidth=%d}",
        channel->hw_value, channel->band, WLAN_BAND_WIDTH_20M);

    /* 抛事件给驱动 */
    ret = (osal_u32)wal_set_channel_info(wal_util_get_vap_id(netdev), (osal_u8)channel->hw_value,
        channel->band, WLAN_BAND_WIDTH_20M);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_channel_info::return err code [%u]!}", ret);
    }

    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_set_auth_mode
 功能描述  : 配置vap的auth类型
*****************************************************************************/
static td_s32 wal_cfg80211_set_auth_mode(oal_net_device_stru *netdev, osal_u8 auth_algs)
{
    td_s32 ret;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_AUTH_MODE,
        (osal_u8 *)&auth_algs, OAL_SIZEOF(auth_algs));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_set_auth_mode::fail to send auth_tpye cfg msg, error[%d]}", ret);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}

static td_s32 wal_cfg80211_start_ap_set_auth_mode(oal_net_device_stru *netdev,
    wlan_auth_alg_mode_enum_uint8 auth_algs)
{
    td_s32 ret;

    if ((auth_algs != WLAN_WITP_AUTH_OPEN_SYSTEM) &&
        (auth_algs != WLAN_WITP_AUTH_SHARED_KEY) &&
#ifdef _PRE_WLAN_FEATURE_WPA3
        (auth_algs != WLAN_WITP_AUTH_SAE) &&
#endif
        (auth_algs != WLAN_WITP_AUTH_AUTOMATIC)) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_start_ap::auth_algs error[%d].}", auth_algs);
        return -OAL_EFAIL;
    }

    ret = wal_cfg80211_set_auth_mode(netdev, auth_algs);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_start_ap::fail to send auth_tpye cfg msg, error[%d]}", ret);
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}

OAL_STATIC osal_void wal_set_beacon_param(mac_beacon_param_stru *beacon_param, oal_ap_settings_stru *ap_settings)
{
    /* 初始化beacon interval 和DTIM_PERIOD 参数 */
    beacon_param->interval = ap_settings->beacon_interval;
    beacon_param->dtim_period = ap_settings->dtim_period;
    beacon_param->hidden_ssid = (ap_settings->hidden_ssid == 1);
#ifdef _PRE_WLAN_FEATURE_WPA3
    beacon_param->sae_pwe = (osal_u8)ap_settings->sae_pwe;
#endif
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_start_ap
 功能描述  : 启动AP,配置AP 参数。
*****************************************************************************/
td_u32 wal_cfg80211_start_ap(oal_net_device_stru *netdev, oal_ap_settings_stru *ap_settings)
{
    mac_beacon_param_stru       beacon_param = {0};  /* beacon info struct */
    /* 参数合法性检查 */
    if ((netdev == OSAL_NULL) || (ap_settings == OSAL_NULL) || (ap_settings->ssid == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_start_ap:: %p, %p!}", (uintptr_t)netdev, (uintptr_t)ap_settings);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取vap id */
    hmac_vap_stru *mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_start_ap::mac_vap = %p}", (uintptr_t)mac_vap);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*****************************************************************************
        1.设置信道
    *****************************************************************************/
    if (wal_cfg80211_set_channel_info(netdev) != OAL_SUCC) {
        return OAL_FAIL;
    }

    /*****************************************************************************
        2.设置ssid等信息
    *****************************************************************************/
    if ((ap_settings->ssid_len > 32) || (ap_settings->ssid_len == 0)) { /* 32: 长度上界 */
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_start_ap: len[%d].}", ap_settings->ssid_len);
        return OAL_FAIL;
    }

    if (wal_cfg80211_set_ssid(netdev, ap_settings->ssid, (td_u8)ap_settings->ssid_len) != OAL_SUCC) {
        return OAL_FAIL;
    }

    /*****************************************************************************
        2.2 设置auth mode信息
    *****************************************************************************/
    if (wal_cfg80211_start_ap_set_auth_mode(netdev, ap_settings->auth_type) != OAL_SUCC) {
        return OAL_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_MESH
    /*****************************************************************************
        2.Mesh设置meshid等信息,与ssid一致
    *****************************************************************************/
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        if (wal_cfg80211_set_meshid(netdev, ap_settings->ssid, (td_u8)ap_settings->ssid_len) != OAL_SUCC) {
            return OAL_FAIL;
        }
    }
#endif

    /*****************************************************************************
        3.设置beacon时间间隔、tim period以及安全配置消息等
    *****************************************************************************/
    wal_set_beacon_param(&beacon_param, ap_settings);

    oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_fill_beacon_param:beacon_interval=%d,dtim_period=%d,hidden_ssid=%d}",
        ap_settings->beacon_interval, ap_settings->dtim_period, ap_settings->hidden_ssid);

    if (wal_cfg80211_fill_beacon_param(mac_vap, &(ap_settings->beacon), &beacon_param) != OAL_SUCC) {
        return OAL_FAIL;
    }

    /* 设置操作类型 */
    beacon_param.operation_type = MAC_ADD_BEACON;

    /* 抛事件给驱动 */
    if (wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_CONFIG_BEACON,
        (osal_u8 *)&beacon_param, OAL_SIZEOF(mac_beacon_param_stru)) != OAL_SUCC) {
        return OAL_FAIL;
    }

    /*****************************************************************************
        4.启动ap
    *****************************************************************************/
    td_u32 ret = wal_start_vap(netdev);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_start_ap::failed to start ap, error[%u]}", ret);
    }

    return ret;
}

td_u32 wal_cfg80211_intf_mode_check(oal_net_device_stru *netdev, nl80211_iftype_uint8 type)
{
    switch (type) {
        case NL80211_IFTYPE_STATION:
            if (netdev->ieee80211_ptr->iftype == NL80211_IFTYPE_AP) {
                /* 结束扫描,以防在20/40M扫描过程中关闭AP */
                wal_force_scan_complete(netdev);

                /* AP关闭切换到STA模式,删除相关vap */
                /* 解决vap状态与netdevice状态不一致，无法删除vap的问题，VAP删除后，上报成功 */
                if (wal_stop_vap(netdev) != OAL_SUCC) {
                    oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::wal_stop_vap enter a error.}");
                }
                if (wal_deinit_wlan_vap(netdev) != OAL_SUCC) {
                    oam_warning_log0(0, OAM_SF_CFG,
                        "{wal_cfg80211_change_virtual_intf::wal_deinit_wlan_vap enter a error.}");
                }
            }
            netdev->ieee80211_ptr->iftype = type; /* P2P BUG P2P_DEVICE 提前创建，不需要通过wpa_supplicant 创建 */
            oam_warning_log0(0, OAM_SF_CFG, "{wal_cfg80211_change_virtual_intf::change to station}\r\n");
            return OAL_SUCC;
#ifdef _PRE_WLAN_FEATURE_MESH
        case NL80211_IFTYPE_MESH_POINT:
#endif
        case NL80211_IFTYPE_AP:
#ifdef _PRE_WLAN_FEATURE_P2P
        case NL80211_IFTYPE_P2P_CLIENT:
        case NL80211_IFTYPE_AP_VLAN:
        case NL80211_IFTYPE_P2P_GO:
        case NL80211_IFTYPE_P2P_DEVICE:
#endif
            break;
        default:
            oam_error_log1(0, OAM_SF_CFG,
                "{wal_cfg80211_change_virtual_intf::currently we do not support this type[%d]}\r\n", type);
            return OAL_ERR_CODE_PTR_NULL;
    }

    return OAL_CONTINUE;
}

OAL_STATIC td_u32 wal_cfg80211_del_send_event(oal_net_device_stru *netdev, const td_u8 *mac_addr,
    hmac_vap_stru *mac_vap)
{
    mac_cfg_kick_user_param_stru kick_user_param;
    td_u32                       ret;

#ifdef _PRE_WLAN_FEATURE_MESH
    if (mac_vap->vap_mode == WLAN_VAP_MODE_MESH) {
        kick_user_param.reason_code = MAC_WPA_KICK_MESH_USER;
    } else {
        kick_user_param.reason_code = MAC_DEAUTH_LV_SS;
    }
#else
    unref_param_prv(mac_vap);
    kick_user_param.reason_code = MAC_DEAUTH_LV_SS;
#endif
    if (memcpy_s(kick_user_param.mac_addr, OAL_MAC_ADDR_LEN, mac_addr, OAL_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_send_event::mem safe function err!}");
        return OAL_FAIL;
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_KICK_USER,
        (osal_u8 *)&kick_user_param, OAL_SIZEOF(mac_cfg_kick_user_param_stru));
    if (ret != OAL_SUCC) {
        /* 由于删除的时候可能用户已经删除，此时再进行用户查找，会返回错误，输出ERROR打印，修改为warning */
        oam_warning_log1(0, OAM_SF_ANY, "{wal_cfg80211_del_send_event::kick_user Err=%d}", ret);
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_del_station
 功能描述  : 删除用户
*****************************************************************************/
td_u32 wal_cfg80211_del_station(oal_net_device_stru *netdev, const td_u8 *mac_addr)
{
    hmac_vap_stru *mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_station::can't get mac vap from netdevice priv data!}\r\n");
        return OAL_FAIL;
    }

    /* 判断是否是AP模式 */
    if ((mac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP)
#ifdef _PRE_WLAN_FEATURE_MESH
        && (mac_vap->vap_mode != WLAN_VAP_MODE_MESH)
#endif
    ) {
        oam_error_log1(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_station::vap_mode=%d Err}", mac_vap->vap_mode);
        return OAL_FAIL;
    }

    if (mac_addr == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_station::Mac addr is null!}");
        return OAL_FAIL;
    } else {
        oam_info_log4(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_del_station::delete user:%02X:%02X:%02X:%02X:XX:XX}",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3]); /* 1, 2, 3: 数组下标 */
    }

    return wal_cfg80211_del_send_event(netdev, mac_addr, mac_vap);
}

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 函 数 名  : wal_del_p2p_group
 功能描述  : 删除P2P group
*****************************************************************************/
td_u32 wal_del_p2p_group(const hmac_device_stru *hmac_device)
{
    hmac_vap_stru        *mac_vap  = OSAL_NULL;
    hmac_vap_stru       *hmac_vap = OSAL_NULL;
    oal_net_device_stru *netdev   = OSAL_NULL;

    for (td_u8 vap_idx = 0; vap_idx < hmac_device->vap_num; vap_idx++) {
        mac_vap = mac_res_get_hmac_vap(hmac_device->vap_id[vap_idx]);
        if (OAL_UNLIKELY(mac_vap == OSAL_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::mac vap Err!vapId=%d}", hmac_device->vap_id[vap_idx]);
            continue;
        }

        hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(hmac_device->vap_id[vap_idx]);
        if (OAL_UNLIKELY(hmac_vap == OSAL_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::get hmac vap resource fail! vap id is %d}",
                hmac_device->vap_id[vap_idx]);
            continue;
        }

        netdev = hmac_vap->net_device;
        if (OAL_UNLIKELY(netdev == OSAL_NULL)) {
            oam_warning_log1(0, OAM_SF_P2P, "{wal_del_p2p_group::netdev Err!vap id=%d}", hmac_device->vap_id[vap_idx]);
            continue;
        }

        if (is_p2p_go(mac_vap) || is_p2p_cl(mac_vap)) {
            oam_warning_log2(mac_vap->vap_id, OAM_SF_P2P, "{wal_del_p2p_group:: vap mode[%d], p2p mode[%d]}\r\n",
                mac_vap->vap_mode, hmac_get_p2p_mode_etc(mac_vap));
            /* 删除已经存在的P2P group */
            wal_force_scan_complete(netdev);
            wal_stop_vap(netdev);
            if (wal_deinit_wlan_vap(netdev) == OAL_SUCC) {
                /* 去注册netdev */
                oal_net_unregister_netdev(netdev);
                oal_net_free_netdev(netdev);
            }
        }
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_remain_on_channel
 功能描述  : 保持在指定信道
*****************************************************************************/
td_u32 wal_cfg80211_remain_on_channel(const oal_wireless_dev *wdev,
    const struct ieee80211_channel *chan, td_u32 duration, osal_u64 *pull_cookie)
{
    /* 1.1 入参检查 */
    if ((wdev == OSAL_NULL) || (chan == OSAL_NULL) || (pull_cookie == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_P2P, "{wal_cfg80211_remain_on_channel:: null param}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_net_device_stru *netdev = wdev->netdev;
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_P2P, "{wal_cfg80211_remain_on_channel::netdev ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_device_stru *mac_device     = (hmac_device_stru *)hmac_res_get_mac_dev_etc(0);
    td_u16           center_freq = chan->center_freq;
    td_s32           l_channel      = (td_s32)oal_ieee80211_frequency_to_channel((td_s32)center_freq);

    mac_remain_on_channel_param_stru remain_on_channel = {0};

    /* 2.1 消息参数准备 */
    remain_on_channel.listen_channel = (td_u8)l_channel;
    remain_on_channel.listen_duration = duration;
    remain_on_channel.listen_channel_para = *chan;
    remain_on_channel.listen_channel_type = WLAN_BAND_WIDTH_20M;

    if (chan->band == OAL_IEEE80211_BAND_2GHZ) {
        remain_on_channel.band = WLAN_BAND_2G;
    } else {
        oam_warning_log1(0, OAM_SF_P2P, "{wal_cfg80211_remain_on_channel::wrong band type[%d]!}\r\n", chan->band);
        return OAL_FAIL;
    }
    /* 将cookie+1值提前，保证驱动侧扫描的cookie值一致避免两次扫描cookie值弄混的现象
     * cookie值上层调用需要判断是否是这次的发送导致的callbacks_pending
     */
    *pull_cookie = ++mac_device->st_p2p_info.ull_last_roc_id;
    if (*pull_cookie == 0) {
        *pull_cookie = ++mac_device->st_p2p_info.ull_last_roc_id;
    }

    /* 保存cookie 值，下发给HMAC 和DMAC */
    remain_on_channel.cookie = mac_device->st_p2p_info.ull_last_roc_id;

    /* 抛事件给驱动 */
    td_u32 ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_REMAIN_ON_CHANNEL,
        (osal_u8 *)&remain_on_channel, OAL_SIZEOF(mac_remain_on_channel_param_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_P2P, "{wal_cfg80211_remain_on_channel::send_cfg_event return error:[%d]}", ret);
        return ret;
    }

    /* 上报暂停在指定信道成功 */
#if (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    cfg80211_ready_on_channel(wdev, cookie, chan, duration, gfp);
#endif
    ret = cfg80211_remain_on_channel(netdev, chan->center_freq, duration);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_P2P, "{wal_cfg80211_remain_on_channel::cfg80211_remain_on_channel fail[%u]}", ret);
        return ret;
    }

    oam_warning_log4(0, OAM_SF_P2P,
        "{wal_cfg80211_remain_on_channel::SUCC! l_channel=%d, duration=%d, cookie 0x%x, band= %d!}",
        l_channel, duration, *pull_cookie, remain_on_channel.band);

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_cfg80211_cancel_remain_on_channel
 功能描述  : 停止保持在指定信道
*****************************************************************************/
td_u32 wal_cfg80211_cancel_remain_on_channel(const oal_wireless_dev *wdev, osal_u64 ull_cookie)
{
    oal_net_device_stru             *netdev                  = OSAL_NULL;
    mac_remain_on_channel_param_stru cancel_remain_on_channel = {0};
    td_u32                           ret;

    unref_param_prv(ull_cookie);
    netdev = wdev->netdev;

    /* 抛事件给驱动 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_CANCEL_REMAIN_ON_CHANNEL,
        (osal_u8 *)&cancel_remain_on_channel, OAL_SIZEOF(cancel_remain_on_channel));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_P2P,
            "{wal_cfg80211_cancel_remain_on_channel::wal_sync_post2hmac_no_rsp return err code:[%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 删除cookie 列表中超时的cookie
 输入参数  : cookie_arry_stru *cookie_array
             td_u32 current_time
*****************************************************************************/
td_void wal_check_cookie_timeout(cookie_arry_stru *cookie_array, td_u8 *cookie_bitmap)
{
    td_u8               loops = 0;
    cookie_arry_stru   *tmp_cookie = OSAL_NULL;

    oam_warning_log0(0, OAM_SF_CFG, "{wal_check_cookie_timeout::time_out!}\r\n");
    for (loops = 0; loops < WAL_COOKIE_ARRAY_SIZE; loops++) {
        tmp_cookie = &cookie_array[loops];

        if (((osal_u32)(osal_adapt_get_jiffies() & 0xffffffff)) >
            tmp_cookie->record_time + WAL_MGMT_TX_TIMEOUT_MSEC / MILLISECOND_PER_TICK) {
            /* cookie array 中保存的cookie 值超时 */
            /* 清空cookie array 中超时的cookie */
            tmp_cookie->record_time = 0;
            tmp_cookie->ull_cookie     = 0;
            /* 清除占用的cookie bitmap位 */
            oal_bit_clear_bit_one_byte(cookie_bitmap, loops);
        }
    }
}

/*****************************************************************************
 功能描述  : 删除指定idx 的cookie
 输入参数  : [1]cookie_array
             [2]cookie_bitmap
             [3]cookie_idx
 返 回 值  : td_u32
*****************************************************************************/
td_void wal_del_cookie_from_array(cookie_arry_stru *cookie_array, td_u8 *cookie_bitmap, td_u8 cookie_idx)
{
    cookie_arry_stru   *tmp_cookie = OSAL_NULL;

    if (cookie_idx >= WAL_COOKIE_ARRAY_SIZE) {
        return;
    }
    /* 清除对应cookie bitmap 位 */
    oal_bit_clear_bit_one_byte(cookie_bitmap, cookie_idx);

    /* 清空cookie array 中超时的cookie */
    tmp_cookie = &cookie_array[cookie_idx];
    tmp_cookie->ull_cookie     = 0;
    tmp_cookie->record_time = 0;
}

/*****************************************************************************
 功能描述  : 添加cookie 到cookie array 中
 输入参数  : [1]cookie_array
             [2]cookie_bitmap
             [3]cookie_idx
             [4]pull_cookie
 返 回 值  : td_u32
*****************************************************************************/
td_u32 wal_add_cookie_to_array(cookie_arry_stru *cookie_array, td_u8 *cookie_bitmap,
                               const osal_u64 *pull_cookie, td_u8 *cookie_idx)
{
    td_u8           idx;
    cookie_arry_stru   *tmp_cookie = OSAL_NULL;
    if (pull_cookie == NULL) {
        return OAL_FAIL;
    }

    if (*cookie_bitmap == 0xFF) {    /* cokie规格 8个 最大到0xFF */
        /* cookie array 满，返回错误 */
        oam_warning_log0(0, OAM_SF_CFG, "{wal_add_cookie_to_array::array full!}\r\n");
        return OAL_FAIL;
    }

    /* 将cookie 添加到array 中 */
    idx = oal_bit_get_num_one_byte(*cookie_bitmap);
    oal_bit_set_bit_one_byte(cookie_bitmap, idx);

    tmp_cookie = &cookie_array[idx];
    tmp_cookie->ull_cookie      = *pull_cookie;
    tmp_cookie->record_time  = (osal_u32)(osal_adapt_get_jiffies() & 0xffffffff);

    *cookie_idx = idx;
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 从cookie array 中查找相应cookie index
 输入参数  : [1]cookie_bitmap
             [2]cookie_idx
 返 回 值  : status td_u32
*****************************************************************************/
static td_u32 wal_check_cookie_from_array(const td_u8 *cookie_bitmap, td_u8 cookie_idx)
{
    /* 从cookie bitmap中查找相应的cookie index，如果位图为0，表示已经被del */
    if (*cookie_bitmap & (bit(cookie_idx))) {
        return OAL_SUCC;
    }
    /* 找不到则返回FAIL */
    return OAL_FAIL;
}

osal_s32 wal_mgmt_do_tx_wait_condition(const void *param)
{
    oal_mgmt_tx_stru *mgmt_tx = (oal_mgmt_tx_stru *)param;
    return (OSAL_TRUE == mgmt_tx->mgmt_tx_complete);
}

/*****************************************************************************
 功能描述  : WAL 层发送从wpa_supplicant  接收到的管理帧
 输入参数  : oal_net_device_stru    *netdev        发送管理帧设备
             mac_mgmt_frame_stru    *mgmt_tx_param 发送管理帧参数
 输出参数  : 无
 返 回 值  : static td_u32 OAL_SUCC 发送成功
                                   OAL_FAIL 发送失败
*****************************************************************************/
static td_u32 wal_mgmt_do_tx(oal_net_device_stru *netdev, const mac_mgmt_frame_stru *mgmt_tx_param)
{
    hmac_vap_stru                    *mac_vap = OSAL_NULL;
    hmac_vap_stru                   *hmac_vap = OSAL_NULL;
    oal_mgmt_tx_stru                *mgmt_tx = OSAL_NULL;
    td_u32                           retval;
    td_s32                           i_leftime;

    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->vap_id);
    mgmt_tx = &(hmac_vap->mgmt_tx);
    mgmt_tx->mgmt_tx_complete = OSAL_FALSE;
    mgmt_tx->mgmt_tx_status  = OSAL_FALSE;

    oam_warning_log2(0, OAM_SF_P2P, "vap_id[%d] {wal_mgmt_do_tx::mgmt frame id=[%d]}", hmac_vap->vap_id,
        mgmt_tx_param->mgmt_frame_id);
    /* 抛事件给驱动 */
    retval = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_MGMT_TX,
        (osal_u8 *)mgmt_tx_param, OAL_SIZEOF(mac_mgmt_frame_stru));
    if (retval != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_mgmt_do_tx::wal_sync_post2hmac_no_rsp return err code:[%d]!}", retval);
        return retval;
    }

    i_leftime = osal_wait_timeout_interruptible(&mgmt_tx->wait_queue, wal_mgmt_do_tx_wait_condition, mgmt_tx,
        WAL_MGMT_TX_TIMEOUT_MSEC);
    if (i_leftime == 0) {
        /* 定时器超时 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx timeout!}\r\n");
        return OAL_FAIL;
    } else if (i_leftime < 0) {
        /* 定时器内部错误 */
        oam_warning_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx timer error!}\r\n");
        return OAL_FAIL;
    } else {
        /* 正常结束  */
        oam_info_log0(0, OAM_SF_ANY, "{wal_mgmt_do_tx::mgmt tx commpleted!}\r\n");
        /* 正常发送结束，返回发送完成状态 */
        return mgmt_tx->mgmt_tx_status;
    }
}

static td_u32 wal_cfg80211_mgmt_tx_parameter_check(oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    const td_u8 *buf, osal_u64 *pull_cookie)
{
    oal_net_device_stru *netdev;

    if ((wdev == OSAL_NULL) || (chan == OSAL_NULL) || (pull_cookie == OSAL_NULL) || (buf == OSAL_NULL)) {
        oam_error_log3(0, OAM_SF_CFG,
            "{wal_cfg80211_mgmt_tx_parameter_check::wdev or chan or cookie or buf ptr is null, error %p, %p, %p!}\r\n",
            (uintptr_t)wdev, (uintptr_t)chan, (uintptr_t)pull_cookie);
        return OAL_ERR_CODE_PTR_NULL;
    }

    netdev = wdev->netdev;
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::netdev ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::can't get mac vap fail!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->vap_id);
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG, "{wal_cfg80211_mgmt_tx_parameter_check::hmac_vap ptr is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return OAL_SUCC;
}
td_void wal_cfg80211_p2p_mgmt_tx_process(oal_wireless_dev *wdev, osal_u64 *pull_cookie,
    mac_mgmt_frame_stru mgmt_tx)
{
    td_u8 retry = 0;
    td_u32 start_time_stamp = (osal_u32)(osal_adapt_get_jiffies() & 0xffffffff);
    td_u32 end_time_stamp = start_time_stamp + 2 * WAL_MGMT_TX_TIMEOUT_MSEC / MILLISECOND_PER_TICK; /* 2: 比例系数 */
    td_u32 ret;
    do {
        ret = wal_mgmt_do_tx(wdev->netdev, &mgmt_tx);
        retry++;
    } while ((ret != DMAC_TX_SUCC) && (ret != DMAC_TX_INVALID) && (retry <= WAL_MGMT_TX_RETRY_CNT)
             && oal_time_before((osal_u32)(osal_adapt_get_jiffies() & 0xffffffff), end_time_stamp));
    /* 请留意，如果是Go端(通过p2p0)在一次会话中第二次(第一次发送已成功)给第三方发送
       Device Discoverability Response等帧时，可能会由于对方不再侦听而超时失败 */
    if (retry > WAL_MGMT_TX_RETRY_CNT) {
        oam_warning_log3(0, OAM_SF_CFG,
            "{wal_cfg80211_mgmt_tx::retry count[%d]>max[%d],tx status[%d],stop tx mgmt}",
            retry, WAL_MGMT_TX_RETRY_CNT, ret);
    }
    if (ret != DMAC_TX_SUCC) {
        wal_del_cookie_from_array(g_cookie_array, &g_cookie_array_bitmap, mgmt_tx.mgmt_frame_id);
        /* 发送失败，处理超时帧的bitmap */
        wal_check_cookie_timeout(g_cookie_array, &g_cookie_array_bitmap);
        cfg80211_mgmt_tx_status(wdev, mgmt_tx.frame, mgmt_tx.len, OSAL_FALSE);
    } else {
        /* 正常结束  */
        *pull_cookie = g_cookie_array[mgmt_tx.mgmt_frame_id].ull_cookie;
        wal_del_cookie_from_array(g_cookie_array, &g_cookie_array_bitmap, mgmt_tx.mgmt_frame_id);
        cfg80211_mgmt_tx_status(wdev, mgmt_tx.frame, mgmt_tx.len, OSAL_TRUE);
    }
    /* 请留意，如果是Go端(通过p2p0)在一次会话中第二次(第一次发送已成功)给第三方发送
       Device Discoverability Response等帧时，可能会由于对方不再侦听而超时失败 */
    oam_warning_log3(0, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::tx status [%d], retry cnt[%d]}, delta_time[%d]",
        ret, retry, (osal_u32)(osal_adapt_get_jiffies() & 0xffffffff) - start_time_stamp);
}
/*****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx
 功能描述  : 发送管理帧
*****************************************************************************/
td_u32 wal_cfg80211_p2p_mgmt_tx(oal_wireless_dev *wdev, oal_ieee80211_channel *chan,
    ext_mlme_data_stru *mlme_data, osal_u64 *pull_cookie)
{
    mac_mgmt_frame_stru              mgmt_tx = {0};
    td_u8                            *buf = mlme_data->data;
    td_u8                            need_offchan = OSAL_FALSE;

    if (wal_cfg80211_mgmt_tx_parameter_check(wdev, chan, buf, pull_cookie) != OAL_SUCC) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    td_u8 cookie_idx;
    hmac_device_stru *mac_dev = (hmac_device_stru *)hmac_res_get_mac_dev_etc(0);
    hmac_vap_stru *mac_vap = (hmac_vap_stru *)wdev->netdev->ml_priv; /* 空指针check函数已校验 */
    hmac_vap_stru *hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->vap_id);
   /* cookie值上层调用需要判断是否是这次的发送导致的callback */
    *pull_cookie = mac_dev->st_p2p_info.ull_send_action_id++;
    if (*pull_cookie == 0) {    /* 不能为0 故再+1 */
        *pull_cookie = mac_dev->st_p2p_info.ull_send_action_id++;
    }
    const oal_ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *)buf;
    if (oal_ieee80211_is_probe_resp(mgmt->frame_control)) {
        *pull_cookie = 0; /* set cookie default value */
        /* host should not send PROE RESPONSE,
           device will send immediately when receive probe request packet */
        cfg80211_mgmt_tx_status(wdev, buf, mlme_data->data_len, OSAL_TRUE);
        return OAL_SUCC;
    }
    if (wal_add_cookie_to_array(g_cookie_array, &g_cookie_array_bitmap, pull_cookie, &cookie_idx) != OAL_SUCC) {
        oam_warning_log0(mac_vap->vap_id, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::Failed to add cookies!}");
        return OAL_FAIL;
    }
    /* 2.1 消息参数准备 */
    mgmt_tx.channel = oal_ieee80211_frequency_to_channel(chan->center_freq);
    mgmt_tx.mgmt_frame_id = cookie_idx;
    mgmt_tx.len        = (td_u16)mlme_data->data_len;
    mgmt_tx.frame     = buf;

    /* APUT妯″紡涓嶈兘roc */
    switch (hmac_vap->vap_mode) {
        case WLAN_VAP_MODE_BSS_AP:
            if ((hmac_vap->channel.chan_number != mgmt_tx.channel) && (hmac_vap->p2p_mode == WLAN_P2P_GO_MODE)) {
                need_offchan = OSAL_TRUE;
            }
            break;
        /* P2P CL DEV */
        case WLAN_VAP_MODE_BSS_STA:
            if ((hmac_vap->p2p_mode == WLAN_LEGACY_VAP_MODE) && (hmac_vap->vap_state == MAC_VAP_STATE_UP)) {
                need_offchan = OSAL_FALSE;
            }
            break;
        default:
            break;
    }
    if ((need_offchan == OSAL_TRUE) && !chan) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::need offchannle but channel is null}");
        return OAL_FAIL;
    }

    wal_cfg80211_p2p_mgmt_tx_process(wdev, pull_cookie, mgmt_tx);
    return OAL_SUCC;
}
#else
/*****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx
 功能描述  : 发送管理帧
*****************************************************************************/
td_u32 wal_cfg80211_mgmt_tx(oal_net_device_stru *netdev, oal_ieee80211_channel *chan, ext_mlme_data_stru *mlme_data)
{
    mac_mgmt_frame_stru              mgmt_tx = {0};
    td_u32                           ret;
    td_u8                            *buf = mlme_data->data;
    hmac_vap_stru                    *hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if ((hmac_vap == OAL_PTR_NULL) || (buf == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::input ptr is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_DFT_STAT
    /* WAL接收协议栈发来的管理帧数目 */
    hmac_pkts_tx_stat(hmac_vap->vap_id, 1, TX_M_FROM_KERNEL_PKTS);
#endif

    /* 2.1 消息参数准备 */
    mgmt_tx.channel = oal_ieee80211_frequency_to_channel(chan->center_freq);
    mgmt_tx.len  = (td_u16)mlme_data->data_len;
    mgmt_tx.frame = buf;
    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CFG80211_MGMT_TX,
        (osal_u8 *)&mgmt_tx, OAL_SIZEOF(mac_mgmt_frame_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_mgmt_tx::send mgmt return err code:[%d]!}", ret);
#ifdef _PRE_WLAN_DFT_STAT
        hmac_pkts_tx_stat(hmac_vap->vap_id, 1, TX_M_SEND_HMAC_FAIL_PKTS);
#endif
        return ret;
    }
#ifdef _PRE_WLAN_DFT_STAT
    /* WAL发送至HMAC管理帧报文成功 */
    hmac_pkts_tx_stat(hmac_vap->vap_id, 1, TX_M_SEND_HMAC_SUCC_PKTS);
#endif
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : wal_cfg80211_mgmt_tx_status
 功能描述  : HMAC抛mgmt tx status到WAL, 唤醒wait queue
*****************************************************************************/
osal_s32 wal_cfg80211_mgmt_tx_status(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    unref_param(hmac_vap);
    unref_param(msg);
#ifdef _PRE_WLAN_FEATURE_P2P
    dmac_tx_status_stru *mgmt_tx_status_param = OSAL_NULL;
    oal_mgmt_tx_stru *mgmt_tx = OSAL_NULL;
    mgmt_tx_status_param = (dmac_tx_status_stru *)(msg->data);
    mgmt_tx = &(hmac_vap->mgmt_tx);
    mgmt_tx->mgmt_tx_complete = OSAL_TRUE;
    mgmt_tx->mgmt_tx_status = mgmt_tx_status_param->dscr_status;
    mgmt_tx->mgmt_frame_id = mgmt_tx_status_param->mgmt_frame_id;

    /* 找不到相应的cookie值，说明已经超时被处理，不需要再唤醒 */
    if (OAL_SUCC == wal_check_cookie_from_array(&g_cookie_array_bitmap, mgmt_tx->mgmt_frame_id)) {
         /* 让编译器优化时保证HI_WAIT_QUEUE_WAKE_UP在最后执行 */
        oal_smp_mb();
        osal_adapt_wait_wakeup(&mgmt_tx->wait_queue);
    }
#endif
    return OAL_SUCC;
}

osal_s32 wal_report_csa_done(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    osal_s32 ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    hmac_csa_chan_switch_done_stru *chan_switch_done;

    unref_param(hmac_vap);
    if ((hmac_vap == OSAL_NULL) || (msg == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_CSA, "{wal_report_csa_done::hmac_vap or msg is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取hmac vap结构体 */
    if (hmac_vap->net_device == OSAL_NULL) {
        oam_warning_log0(hmac_vap->vap_id, OAM_SF_CSA, "{wal_report_csa_done::param is NULL!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    netdev = hmac_vap->net_device;
    chan_switch_done = (hmac_csa_chan_switch_done_stru *)(msg->data);

    /* 通知内核CSA通道切换完成 */
    ret = (osal_s32)cfg80211_csa_channel_switch(netdev, chan_switch_done->freq);
    if (ret != OAL_SUCC) {
        oam_error_log1(hmac_vap->vap_id, OAM_SF_CSA, "{wal_report_csa_done::send event err[%d]!}", ret);
        return ret;
    }
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 获取信道 仅支持2G信道
*****************************************************************************/
oal_ieee80211_channel* wal_cfg80211_get_channel(td_s32 freq)
{
    int i;
    for (i = 0; i < g_wifi_band_2ghz.n_channels; i++) {
        if (g_wifi_band_2ghz.channels[i].center_freq == freq) {
            return &(g_wifi_band_2ghz.channels[i]);
        }
    }
    return OSAL_NULL;
}

#ifdef _PRE_WLAN_FEATURE_ROAM
/*****************************************************************************
 函 数 名  : wal_roam_comp_proc_sta_etc
 功能描述  : STA上报漫游完成事件处理
*****************************************************************************/
osal_s32  wal_roam_comp_proc_sta_etc(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_net_device_stru *net_device;
    hmac_device_stru *hmac_device;
    hmac_roam_rsp_stru *roam_rsp;
    oal_ieee80211_band_enum_uint8 band = OAL_IEEE80211_NUM_BANDS;
    osal_slong freq;
    oal_connet_result_stru connet_result = {0};

    roam_rsp = (hmac_roam_rsp_stru *)msg->data;

    /* 获取net_device */
    net_device = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    /* 获取device id 指针 */
    hmac_device = hmac_res_get_mac_dev_etc(0);
    if ((hmac_device == OAL_PTR_NULL) || (net_device == OAL_PTR_NULL)) {
        oam_error_log3(0, OAM_SF_SCAN,
            "vap_id[%d] {wal_roam_comp_proc_sta_etc::hmac_device[%p] or net_device[%p] is null}",
            hmac_vap->vap_id, (uintptr_t)hmac_device, (uintptr_t)net_device);
        oal_free(roam_rsp->asoc_rsp_ie_buff);
        roam_rsp->asoc_rsp_ie_buff = OAL_PTR_NULL;
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (roam_rsp->st_channel.band >= WLAN_BAND_BUTT) {
        oam_error_log2(0, OAM_SF_ROAM, "vap_id[%d] {wal_asoc_comp_proc_sta_etc::unexpected band[%d]}",
            hmac_vap->vap_id, roam_rsp->st_channel.band);
        oal_free(roam_rsp->asoc_rsp_ie_buff);
        roam_rsp->asoc_rsp_ie_buff = OAL_PTR_NULL;
        return OAL_FAIL;
    }

    band = (roam_rsp->st_channel.band == WLAN_BAND_2G) ? OAL_IEEE80211_BAND_2GHZ : OAL_IEEE80211_BAND_5GHZ;
    freq = oal_ieee80211_channel_to_frequency(roam_rsp->st_channel.chan_number, band);
    if (memcpy_s(connet_result.bssid, WLAN_MAC_ADDR_LEN, roam_rsp->bssid, WLAN_MAC_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ASSOC, "{wal_asoc_comp_proc_sta::mem safe function err!}");
        oal_free(roam_rsp->asoc_rsp_ie_buff);
        roam_rsp->asoc_rsp_ie_buff = OAL_PTR_NULL;
        return OAL_FAIL;
    }
    connet_result.req_ie = roam_rsp->asoc_req_ie_buff;
    connet_result.req_ie_len = roam_rsp->asoc_req_ie_len;
    connet_result.rsp_ie = roam_rsp->asoc_rsp_ie_buff;
    connet_result.rsp_ie_len = roam_rsp->asoc_rsp_ie_len;
    connet_result.freq = (osal_u16)freq;

    cfg80211_connect_result(net_device, &connet_result);

    oam_warning_log4(0, OAM_SF_ASSOC, "{wal_roam_comp_proc_sta_etc::asoc_req_ie[%p] len[%d] asoc_rsp_ie[%p] len[%d]!}",
        roam_rsp->asoc_req_ie_buff, roam_rsp->asoc_req_ie_len,
        roam_rsp->asoc_rsp_ie_buff, roam_rsp->asoc_rsp_ie_len);
    oal_free(roam_rsp->asoc_rsp_ie_buff);
    roam_rsp->asoc_rsp_ie_buff = OAL_PTR_NULL;
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11R
/*****************************************************************************
 函 数 名  : wal_cfg80211_update_ft_ies
 功能描述  : 配置AUTH中FT_IE信息
*****************************************************************************/
osal_s32 wal_cfg80211_update_ft_ies(oal_net_device_stru *netdev, oal_ft_ies_stru *fties)
{
    mac_cfg80211_ft_ies_stru           *mac_ft_ies;
    osal_s32                           ret;
    mac_cfg80211_ft_ies_stru cfg_mac_ft_ies;

    (osal_void)memset_s(&cfg_mac_ft_ies, OAL_SIZEOF(cfg_mac_ft_ies), 0, OAL_SIZEOF(cfg_mac_ft_ies));
    mac_ft_ies = &cfg_mac_ft_ies;

    if ((netdev == OAL_PTR_NULL) || (fties == OAL_PTR_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_update_ft_ies::param is null.}");
        return OAL_FAIL;
    }

    if ((fties->ie == OAL_PTR_NULL) || (fties->ie_len == 0)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_update_ft_ies::unexpect ie or len[%d].}", fties->ie_len);
        return OAL_FAIL;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    mac_ft_ies->mdid = fties->md;
    mac_ft_ies->len  = fties->ie_len;
    if (memcpy_s(mac_ft_ies->ie, sizeof(mac_ft_ies->ie), fties->ie, fties->ie_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_update_ft_ies::memcpy_s error}");
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_FT_IES,
        (osal_u8 *)&cfg_mac_ft_ies, OAL_SIZEOF(cfg_mac_ft_ies));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_cfg80211_update_ft_ies::wal_sync_post2hmac_no_rsp return errcode %d}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : wal_ft_event_proc_sta_etc
 功能描述  : STA上报FT事件处理
*****************************************************************************/
osal_s32 wal_ft_event_proc_sta_etc(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    osal_u16 ie_len;
    osal_u8 *ie_buf_tmp = OSAL_NULL;
    osal_u8 *ie_buf = OSAL_NULL;
    oal_net_device_stru *net_device = OSAL_NULL;
    hmac_roam_ft_stru *ft_event = OSAL_NULL;
    oal_cfg80211_ft_event_stru cfg_ft_event = {0};
    ft_event = (hmac_roam_ft_stru *)msg->data;
    ie_buf = msg->data + sizeof(hmac_roam_ft_stru);

    /* 获取net_device */
    net_device = hmac_vap_get_net_device_etc(hmac_vap->vap_id);
    if (net_device == OAL_PTR_NULL) {
        oam_error_log1(0, OAM_SF_ROAM, "vap_id[%d] {wal_ft_event_proc_sta_etc::net_device null}", hmac_vap->vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ie_len = ft_event->ft_ie_len;
    if (ie_len > WLAN_MEM_NETBUF_SIZE2) {
        oam_error_log3(0, OAM_SF_ROAM, "vap_id[%d] {wal_ft_event_proc_sta_etc::ERROR, ie_len(%d) is larger than %d}",
            hmac_vap->vap_id, ie_len, WLAN_MEM_NETBUF_SIZE2);
        return OAL_FAIL;
    }

    ie_buf_tmp = (osal_u8 *)malloc(ie_len);
    if (ie_buf_tmp == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_ft_event_proc_sta_etc::malloc ie_buf FAILED}");
        return OAL_FAIL;
    }

    if (memcpy_s(ie_buf_tmp, ie_len, ie_buf, ie_len) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_ft_event_proc_sta_etc::copy ie_buf FAILED}");
        free(ie_buf_tmp);
        return OAL_FAIL;
    }

    cfg_ft_event.ies         = ie_buf_tmp;
    cfg_ft_event.ies_len     = ie_len;
    cfg_ft_event.ric_ies     = OAL_PTR_NULL;
    cfg_ft_event.ric_ies_len = 0;

    if (memcpy_s(cfg_ft_event.target_ap, ETH_ADDR_LEN, ft_event->bssid, ETH_ADDR_LEN) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_ft_event_proc_sta_etc::copy bssid FAILED}");
        free(ie_buf_tmp);
        return OAL_FAIL;
    }

    osal_u32 ret = cfg80211_ft_event_proc_sta_etc(net_device, &cfg_ft_event);
    if (ret != OAL_SUCC) {
        if (cfg_ft_event.ies != OSAL_NULL) {
            free(cfg_ft_event.ies);
            cfg_ft_event.ies = NULL;
        }
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif // _PRE_WLAN_FEATURE_11R
#endif // _PRE_WLAN_FEATURE_ROAM

#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
/*****************************************************************************
 函 数 名  : hmac_cfg80211_get_survey_etc
 功能描述  :向hostapd上报信道统计结果
*****************************************************************************/
osal_s32 wal_cfg80211_get_survey_etc(oal_net_device_stru *netdev, osal_u32 ifidx, oal_survey_info_stru *sinfo)
{
    hmac_vap_stru *mac_vap = OAL_PTR_NULL;
    hmac_device_stru *hmac_device = OAL_PTR_NULL;
    wlan_scan_chan_stats_stru *record = OAL_PTR_NULL;
    oal_ieee80211_channel *chan = OAL_PTR_NULL;
    oal_ieee80211_supported_band *bands = wal_get_wifi_2ghz_band(); /* 仅支持2G带宽 */

    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    hmac_device = hmac_res_get_mac_dev_etc(mac_vap->device_id);
    if (hmac_device == OAL_PTR_NULL) {
        oam_warning_log2(0, OAM_SF_SCAN,
            "vap_id[%d] {hmac_cfg80211_get_survey_etc::device id[%d],hmac_device null.}",
            mac_vap->vap_id, mac_vap->device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (hmac_device->scan_mgmt.is_scanning) {
        oam_warning_log0(0, OAM_SF_ANY, "{hmac_cfg80211_get_survey_etc::drop request while scan running}");
        return OAL_CONTINUE;
    }

    record = hmac_device->scan_mgmt.scan_record_mgmt.chan_results;
    sinfo->channel_time = record[ifidx].total_stats_time_us / 1000; /* 1000单位换算 */
    /* 1000单位换算 */
    sinfo->channel_time_busy = (record[ifidx].total_stats_time_us - record[ifidx].total_free_time_20m_us) / 1000;
    sinfo->channel_time_ext_busy = 0;
    sinfo->channel_time_rx = record[ifidx].total_recv_time_us / 1000; /* 1000单位换算 */
    sinfo->channel_time_tx = record[ifidx].total_send_time_us / 1000; /* 1000单位换算 */
    sinfo->filled = SURVEY_HAS_CHAN_TIME | SURVEY_HAS_CHAN_TIME_BUSY | SURVEY_HAS_CHAN_TIME_RX |
        SURVEY_HAS_CHAN_TIME_TX;

    if ((record[ifidx].free_power_cnt != 0) && (record[ifidx].free_power_stats_20m < 0)) {
        sinfo->noise = (osal_s8)(record[ifidx].free_power_stats_20m / record[ifidx].free_power_cnt);
        sinfo->filled |= SURVEY_HAS_NF;
    }

    chan = &(bands->channels[ifidx]);
    if ((chan->flags & EXT_CHAN_DISABLED) == 0) {
        sinfo->channel = chan;
    }

    wifi_printf("wal_cfg80211_get_survey_etc channel:%d, channel_time:%llu," \
        " channel_time_busy:%llu, channel_time_rx:%llu," \
        " channel_time_tx:%llu, filled:%u, noise:%d \r\n", sinfo->channel->center_freq,
        sinfo->channel_time, sinfo->channel_time_busy, sinfo->channel_time_rx,
        sinfo->channel_time_tx, sinfo->filled, sinfo->noise);

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/*****************************************************************************
 函 数 名  : wal_cfg80211_set_rekey_info
 功能描述  : 上层下发的rekey info，抛给wal层处理
*****************************************************************************/
td_u32 wal_cfg80211_set_rekey_info(oal_net_device_stru *netdev, mac_rekey_offload_stru *rekey_offload)
{
    mac_rekey_offload_stru rekey_params;
    td_s32 ret;

    /* 1 参数合法性检查 */
    if ((netdev == OSAL_NULL) || (rekey_offload == OSAL_NULL)) {
        oam_error_log2(0, OAM_SF_ANY, "{wal_cfg80211_set_rekey_info::net_dev = %p, rekey_offload = %p!}",
            (uintptr_t)netdev, (uintptr_t)rekey_offload);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 2 消息参数准备 */
    if (memcpy_s(&rekey_params, sizeof(mac_rekey_offload_stru),
        rekey_offload, sizeof(mac_rekey_offload_stru)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_set_rekey_info::mem safe function err!}");
        return OAL_FAIL;
    }

    /* 抛事件给驱动 */
#ifdef _PRE_WLAN_FEATURE_DYNAMIC_OFFLOAD
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_SET_REKEY,
        (osal_u8 *)&rekey_params, sizeof(mac_rekey_offload_stru));
#else
    ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2D_C_CFG_SET_REKEY,
        (osal_u8 *)&rekey_params, sizeof(mac_rekey_offload_stru));
#endif
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "wal_cfg80211_set_rekey_info::wal_sync_send2device_no_rsp ret [%d]", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif /* _PRE_WLAN_FEATURE_WOW_OFFLOAD */

#ifdef _PRE_WLAN_FEATURE_WPA3
/*****************************************************************************
 函 数 名  : wal_cfg80211_external_auth_status
 功能描述  : 上层下发的ext auth status，抛给wal层处理
 修改历史      :
  1.日    期   : 2020年6月24日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_cfg80211_external_auth_status(oal_net_device_stru *netdev, const ext_external_auth_stru *param)
{
    td_u32       ret;
    hmac_external_auth_stru ext_auth = {0};

    /* 1 参数合法性检查 */
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_external_auth::net_dev is null!");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (memcpy_s(ext_auth.bssid, OAL_MAC_ADDR_LEN, param->bssid, OAL_MAC_ADDR_LEN) != EOK) {
        return OAL_FAIL;
    }
    ext_auth.status = param->status;
    if (param->pmkid != OSAL_NULL) {
        if (memcpy_s(ext_auth.pmkid, WLAN_PMKID_LEN, param->pmkid, WLAN_PMKID_LEN) != EOK) {
            return OAL_FAIL;
        }
        ext_auth.have_pmkid = OSAL_TRUE;
    }
    /* 抛事件给驱动 */
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_EXTERNAL_AUTH_STATUS,
        (osal_u8 *)&ext_auth, OAL_SIZEOF(ext_auth));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_external_auth::wal_sync_post2hmac_no_rsp return err code:[%d]!}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

osal_void wal_cfg80211_update_dh_ie(oal_net_device_stru *netdev, const ext_update_dh_ie_stru *param)
{
    hmac_owe_info info = {0};
    osal_s32 ret;

    info.status = param->status;
    if (memcpy_s(info.peer, sizeof(info.peer), param->peer, sizeof(param->peer)) != EOK) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_update_dh_ie::memcpy_s mac error");
        return;
    }
    info.ie = (osal_u8 *)param->ie;
    info.ie_len = param->ie_len;
    /* must be sync msg for ie point */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_UPDATE_OWE_INFO,
        (osal_u8 *)&info, OAL_SIZEOF(info));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY,
            "{wal_cfg80211_update_dh_ie::wal_sync_post2hmac_no_rsp return err code:[%d]!}", ret);
        return;
    }
    oam_warning_log2(0, OAM_SF_ANY,
        "{wal_cfg80211_update_dh_ie::send owe to hmac, status=%u, ie_len=%u}", info.status, info.ie_len);

    return;
}

#endif

td_u32 wal_cfg80211_del_beacon(oal_net_device_stru *netdev)
{
    hmac_vap_stru *hmac_vap;
    td_s32 l_ret;
    td_u32 vap_id;

    /* 参数合法性检查 */
    if (netdev == OAL_PTR_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_beacon:: netdev is null!}");
        return -OAL_EINVAL;
    }

    /* 获取vap id */
    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OAL_PTR_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_cfg80211_del_beacon::can't get mac vap from netdevice priv data!}");
        return -OAL_EINVAL;
    }

    vap_id = hmac_vap->vap_id;

    /* 判断是否为非ap模式 */
    if (hmac_vap->vap_mode != WLAN_VAP_MODE_BSS_AP) {
        oam_error_log1(0, OAM_SF_ANY, "vap_id[%d] {wal_cfg80211_del_beacon::vap is not in ap mode!}", vap_id);
        return -OAL_EINVAL;
    }

    l_ret = wal_down_vap(netdev, (osal_u8)vap_id);
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY,
                         "vap_id[%d] {wal_cfg80211_del_beacon::failed to stop ap, error[%d]}", vap_id, l_ret);
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

