/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: 11d interface function.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_event_msg.h"
#include "wal_main.h"
#include "wal_regdb.h"
#include "wal_ioctl.h"
#include "wal_cfg80211.h"
#include "wal_wpa_ioctl.h"
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "soc_customize_wifi.h"
#endif
#include "wal_11d.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_11D_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
const wal_dfs_domain_entry_stru g_ast_dfs_domain_table[] = {
    {"AE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"AL", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"AM", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"AN", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"AR", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"AT", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"AU", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"AZ", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BA", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BG", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BH", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BL", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"BN", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BO", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BR", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"BY", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"BZ", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"CA", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"CH", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"CL", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"CN", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"CO", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"CR", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"CS", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"CY", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"CZ", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"DE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"DK", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"DO", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"DZ", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"EC", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"EE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"EG", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"ES", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"FI", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"FR", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"GB", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"GE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"GR", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"GT", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"HK", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"HN", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"HR", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"HU", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"ID", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"IE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"IL", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"IN", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"IQ", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"IR", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"IS", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"IT", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"JM", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"JO", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"JP", MAC_DFS_DOMAIN_MKK, {0, 0, 0}},
    {"KP", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"KR", MAC_DFS_DOMAIN_KOREA, {0, 0, 0}},
    {"KW", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"KZ", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"LB", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"LI", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"LK", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"LT", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"LU", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"LV", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"MA", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"MC", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"MK", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"MO", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"MT", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"MX", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"MY", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"NG", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"NL", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"NO", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"NP", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"NZ", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"OM", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PA", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PE", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PG", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PH", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PK", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"PL", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"PR", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"PT", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"QA", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"RO", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"RU", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"SA", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"SE", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"SG", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"SI", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"SK", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"SV", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"SY", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"TH", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"TN", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"TR", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"TT", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"TW", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"UA", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"US", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"UY", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"UZ", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"VE", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"VN", MAC_DFS_DOMAIN_ETSI, {0, 0, 0}},
    {"YE", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
    {"ZA", MAC_DFS_DOMAIN_FCC, {0, 0, 0}},
    {"ZW", MAC_DFS_DOMAIN_NULL, {0, 0, 0}},
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 功能描述  : 获取一个管制类的起始频带
 输入参数  : start_freq: 起始频率
             end_freq  : 结束频率
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_015CHANNEL_EXP
static inline td_u8 wal_regdomain_get_band(td_u32 start_freq, td_u32 end_freq)
{
    if (start_freq > 2372 && end_freq < 2524) { /* 2372 起始频率的最小值 2524 结束频率的最大值 */
        return MAC_RC_START_FREQ_2;
    }
    return MAC_RC_START_FREQ_BUTT;
}
#else
static inline td_u8 wal_regdomain_get_band(td_u32 start_freq, td_u32 end_freq)
{
    if (start_freq > 2400 && end_freq < 2500) { /* 2400 起始频率的最小值 2500 结束频率的最大值 */
        return MAC_RC_START_FREQ_2;
    }
    return MAC_RC_START_FREQ_BUTT;
}
#endif

/*****************************************************************************
 功能描述  : 获取一个管制类的带宽
 输入参数  : bw: linux管制类中的带宽值
*****************************************************************************/
static inline td_u8 wal_regdomain_get_bw(td_u8 bw)
{
    switch (bw) {
        case 40: /* 40 带宽为40MHZ */
            return MAC_CH_SPACING_40MHZ;
        case 20: /* 20 带宽为20MHZ */
            return MAC_CH_SPACING_20MHZ;
        default:
            return MAC_CH_SPACING_BUTT;
    }
}

/*****************************************************************************
 功能描述  : 获取管制类信道位图，信道在2g频段上
 输入参数  : start_freq: 起始频率
             end_freq  : 结束频率
*****************************************************************************/
static td_u32 wal_regdomain_get_channel_2g(td_u32 start_freq, td_u32 end_freq)
{
    td_u32 freq;
    td_u32 i;
    td_u32 ch_bmap = 0;
    mac_freq_channel_map_stru freq_map_2g;
#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* 2.4G频段 信道与中心频率映射 */
    const mac_freq_channel_map_stru ast_freq_map_2g[MAC_CHANNEL_FREQ_2_BUTT] = {
        {2412, 1, 0},
        {2417, 2, 1},
        {2422, 3, 2},
        {2427, 4, 3},
        {2432, 5, 4},
        {2437, 6, 5},
        {2442, 7, 6},
        {2447, 8, 7},
        {2452, 9, 8},
        {2457, 10, 9},
        {2462, 11, 10},
        {2467, 12, 11},
        {2472, 13, 12},
        {2484, 14, 13},
#ifdef _PRE_WLAN_FEATURE_015CHANNEL_EXP
        {2512, 15, 14},
        {2384, 16, 15},
#endif
    };
#endif

    for (freq = start_freq + 10; freq <= (end_freq - 10); freq++) { /* 10 带宽 */
        for (i = 0; i < MAC_CHANNEL_FREQ_2_BUTT; i++) {
            freq_map_2g = ast_freq_map_2g[i];
            if (freq == freq_map_2g.freq) {
                ch_bmap |= (1 << i);
            }
        }
    }

    return ch_bmap;
}

/*****************************************************************************
 功能描述  : 获取1个管制类的信道位图
 输入参数  : band      : 频段
             start_freq: 起始频率
             end_freq  : 中止频率
*****************************************************************************/
static inline td_u32 wal_regdomain_get_channel(td_u8 band, td_u32 start_freq, td_u32 end_freq)
{
    if (band == MAC_RC_START_FREQ_2) {
        return wal_regdomain_get_channel_2g(start_freq, end_freq);
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : wal_get_dfs_domain
 功能描述  : 根据国家码，获取对应的雷达检测标准
 输入参数  : mac_regdom: 管制域指针
             pc_country    : 国家码
 输出参数  : mac_regdom: 管制域指针
 返 回 值  : 无
*****************************************************************************/
static inline td_void wal_get_dfs_domain(mac_regdomain_info_stru *mac_regdom, const td_char *pc_country)
{
    td_u32    u_idx;

    for (u_idx = 0; u_idx < uapi_array_size(g_ast_dfs_domain_table); u_idx++) {
        if (osal_strcmp(g_ast_dfs_domain_table[u_idx].pc_country, pc_country) == 0) {
            mac_regdom->dfs_domain = g_ast_dfs_domain_table[u_idx].dfs_domain;
            return;
        }
    }

    mac_regdom->dfs_domain = MAC_DFS_DOMAIN_NULL;
}

/*****************************************************************************
 功能描述  : 填充管制下发的管制域信息
 输入参数  : regdom    : 指向linux的管制域信息
             mac_regdom: 指向要下发的管制域信息
*****************************************************************************/
static td_u32 wal_regdomain_fill_info(const oal_ieee80211_regdomain_stru *regdom,
    mac_regdomain_info_stru *mac_regdom)
{
    td_u32  i;
    td_u32  start;
    td_u32  end;
    td_u8   band;
    td_u8   bw;

    /* 复制国家字符串 */
    mac_regdom->ac_country[0] = (osal_s8)regdom->alpha2[0];
    mac_regdom->ac_country[1] = (osal_s8)regdom->alpha2[1];
    mac_regdom->ac_country[2] = 0; /* 2 国家码的第3位 */
    /* 初始化管制类数量为0 */
    mac_regdom->regclass_num = 0;
    /* 获取DFS认证标准类型 */
    wal_get_dfs_domain(mac_regdom, regdom->alpha2);
    /* 填充管制类信息 */
    for (i = 0; i < regdom->n_reg_rules; i++) {
        /* 填写管制类的频段(2.4G或5G) */
        start = regdom->reg_rules[i].freq_range.start_freq_khz / 1000; /* 1000 频率单位转换 */
        end   = regdom->reg_rules[i].freq_range.end_freq_khz / 1000; /* 1000 频率单位转换 */
        band  = wal_regdomain_get_band(start, end);
        mac_regdom->regclass_num++;
        if (mac_regdom->regclass_num > WLAN_MAX_RC_NUM) {
            oam_warning_log1(0, OAM_SF_CFG, "wal_regdomain_fill_info: regclass num[%d] overflow.",
                             mac_regdom->regclass_num);
            return OAL_FAIL;
        }
        mac_regdom->regclass[i].start_freq = band;
        /* 填写管制类允许的最大带宽 */
        bw = (td_u8)(regdom->reg_rules[i].freq_range.max_bandwidth_khz / 1000); /* 1000 频率单位转换 */
        mac_regdom->regclass[i].ch_spacing = wal_regdomain_get_bw(bw);
        /* 填写管制类信道位图 */
        mac_regdom->regclass[i].channel_bmap = wal_regdomain_get_channel(band, start, end);
        /* 标记管制类行为 */
        mac_regdom->regclass[i].behaviour_bmap = 0;
        if ((regdom->reg_rules[i].flags & NL80211_RRF_DFS) != 0) {
            mac_regdom->regclass[i].behaviour_bmap |= MAC_RC_DFS;
        }
        /* 填充覆盖类和最大发送功率 */
        mac_regdom->regclass[i].coverage_class = 0;
        mac_regdom->regclass[i].max_reg_tx_pwr =
            (td_u8)(regdom->reg_rules[i].power_rule.max_eirp / 100); /* 100 单位转换 */
        mac_regdom->regclass[i].max_tx_pwr =
            (td_u16)(regdom->reg_rules[i].power_rule.max_eirp / 10); /* 10 单位转换 */
    }
    return OAL_SUCC;
}

/* 根据bitmap刷新可用信道列表 */
static osal_void wal_regdomain_update_wiphy_channel_list_num_rc(osal_u32 channel_bit_map)
{
    osal_u8 i;
    oal_ieee80211_supported_band *wifi_2ghz_band_info = wal_get_wifi_2ghz_band();
    for (i = 0; i < MAC_CHANNEL_FREQ_2_BUTT; i++) {
        if (((1 << i) & channel_bit_map) != 0) {
            wifi_2ghz_band_info->channels[i].flags &= ~EXT_CHAN_DISABLED;
        }
    }
}

/* 更新管制域信息, 用于给supplicnt同步信息 */
static osal_void wal_regdomain_update_wiphy_channel_list_num(mac_regdomain_info_stru *mac_regdom)
{
    osal_u8 rc_idx;
    osal_u8 ch_idx;
    oal_ieee80211_supported_band *wifi_2ghz_band_info = wal_get_wifi_2ghz_band();

    /* 先将每个信道都标记为无效 */
    for (ch_idx = 0; ch_idx < MAC_CHANNEL_FREQ_2_BUTT; ch_idx++) {
        wifi_2ghz_band_info->channels[ch_idx].flags |= EXT_CHAN_DISABLED;
    }

    /* 根据每个管制类更新可用信道信息 */
    for (rc_idx = 0; rc_idx < mac_regdom->regclass_num; rc_idx++) {
        wal_regdomain_update_wiphy_channel_list_num_rc(mac_regdom->regclass[rc_idx].channel_bmap);
    }
}

/*****************************************************************************
 功能描述  : 下发配置管制域信息
 输入参数  : net_dev: net_device
             country : 要设置的国家字符串
*****************************************************************************/
td_u32 wal_regdomain_update(oal_net_device_stru *netdev, const td_char *country, td_u8 country_code_len)
{
    osal_s32 ret;
    unref_param_prv(country_code_len);

    const oal_ieee80211_regdomain_stru *regdom = wal_regdb_find_db_etc((const osal_s8 *)country);
    osal_s8 old_region = hwifi_get_region((const osal_s8 *)hmac_regdomain_get_country_etc());
    osal_s8 new_region = hwifi_get_region((const osal_s8 *)country);

    if (regdom == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_regdomain_update::no regdomain db was found!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (old_region != new_region) {
        hwifi_set_nvram_tag_by_region_index(new_region);
        hwifi_force_refresh_rf_params(netdev);
    }
    /* 申请内存存放管制域信息，将内存指针作为事件payload抛下去 */
    /* 此处申请的内存在事件处理函数释放(hmac_config_set_country) */
    td_u16 size = (td_u16)sizeof(mac_regdomain_info_stru);
    mac_regdomain_info_stru *mac_regdom = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, size, OAL_TRUE);
    if (mac_regdom == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_regdomain_update::alloc regdom mem fail, return null ptr!}");
        return OAL_FAIL;
    }

    if (wal_regdomain_fill_info(regdom, mac_regdom) != OAL_SUCC) {
        oal_mem_free(mac_regdom, OAL_FALSE);
        return OAL_FAIL;
    }

    /* 填写WID对应的参数 */
    mac_cfg_country_stru param = {0};
    param.mac_regdom = mac_regdom;
    /* 同步更新supplicnt管制域信息 */
    wal_regdomain_update_wiphy_channel_list_num(mac_regdom);
    /* 发送消息 */
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_COUNTRY,
        (osal_u8 *)&param, OAL_SIZEOF(param));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_regdomain_update::return err code %d!}", ret);
        oal_mem_free(mac_regdom, OAL_FALSE);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_DFS
/*****************************************************************************
 功能描述  : 下发雷达过滤配置
*****************************************************************************/
td_u32 wal_regdomain_update_for_dfs(oal_net_device_stru *netdev, td_char *country)
{
    const oal_ieee80211_regdomain_stru *regdom;
    td_u16                              size;
    mac_regdomain_info_stru                *mac_regdom;
    td_s32                               ret;

    regdom = wal_regdb_find_db_etc(country);
    if (regdom == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs::no regdomain db was found!}");
        return -OAL_EINVAL;
    }

    size = (osal_u16)(OAL_SIZEOF(mac_regclass_info_stru) * regdom->n_reg_rules + MAC_RD_INFO_LEN);

    /* 申请内存存放管制域信息,在本函数结束后释放 */
    mac_regdom = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, size, OAL_TRUE);
    if (mac_regdom == OAL_PTR_NULL) {
        oam_error_log1(0, OAM_SF_ANY,
            "{wal_regdomain_update_for_dfs::alloc regdom mem fail, return null ptr!size[%d]}", size);
        return -OAL_ENOMEM;
    }

    wal_regdomain_fill_info(regdom, mac_regdom);
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_COUNTRY_FOR_DFS,
        &(mac_regdom->dfs_domain), OAL_SIZEOF(mac_dfs_domain_enum_uint8));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        /* mac_regdom内存，此处释放 */
        oal_mem_free(mac_regdom, OAL_TRUE);
        oam_warning_log1(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs::return err code %d!}", ret);
        return ret;
    }
    /* mac_regdom内存，此处释放 */
    oal_mem_free(mac_regdom, OAL_TRUE);

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : sta关联成功后自动设置国家码,
 与手动配置不同的地方在于,此处使用异步消息,如使用同步消息,会造成死锁
*****************************************************************************/
td_s32 wal_util_set_country_code(oal_net_device_stru *net_dev, osal_s8 *country_code)
{
    const oal_ieee80211_regdomain_stru *regdom = wal_regdb_find_db_etc(country_code);
    mac_cfg_country_stru param = {0};
    td_s32 ret;
    td_u16 size;

    if (regdom == OAL_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_util_set_country_code::no regdomain db was found!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    size = (td_u16)sizeof(mac_regdomain_info_stru);
    mac_regdomain_info_stru *mac_regdom = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, size, OAL_TRUE);
    if (mac_regdom == OAL_PTR_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_util_set_country_code::alloc regdom mem fail, return null ptr!}");
        return OAL_FAIL;
    }

    if (wal_regdomain_fill_info(regdom, mac_regdom) != OAL_SUCC) {
        oal_mem_free(mac_regdom, OAL_FALSE);
        return OAL_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_DFS
    /* 配置雷达过滤参数 */
    ret = wal_async_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_COUNTRY_FOR_DFS,
        &(mac_regdom->dfs_domain), OAL_SIZEOF(mac_dfs_domain_enum_uint8));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oal_mem_free(mac_regdom, OAL_TRUE);
        oam_warning_log1(0, OAM_SF_ANY, "{wal_util_set_country_code::cfg dfs return err code %d!}", ret);
        return ret;
    }
#endif
    /* 填写对应的参数  下发国家码配置 */
    param.mac_regdom = mac_regdom;

    ret = wal_async_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_COUNTRY,
        (osal_u8 *)&param, OAL_SIZEOF(param), FRW_POST_PRI_LOW);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_util_set_country_code::cfg cc return err code %d!}", ret);
        oal_mem_free(mac_regdom, OAL_FALSE);
        return ret;
    }
    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

