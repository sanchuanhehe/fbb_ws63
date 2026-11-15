/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Wal_ccpriv configuration command.
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "wal_ccpriv.h"
#include "wifi_main.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "hmac_ext_if.h"
#include "wlan_msg.h"
#include "wal_common.h"
#include "wal_ccpriv_common.h"
#include "wal_11d.h"
#include "wal_net.h"
#include "oal_util.h"
#include "oal_net.h"
#include "hmac_al_tx_rx.h"
#include "hmac_thruput_test.h"
#include "oam_struct.h"
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
#include "wal_cfg80211_apt.h"
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/netifapi.h"
#endif
#endif
#include "wal_main.h"
#include "wal_ioctl.h"
#include "wal_event_msg.h"
#include "wal_config.h"

#include "soc_wifi_api.h"
#include "soc_wifi_mfg_test_if.h"

#include "soc_wifi_driver_wpa_if.h"
#include "hmac_scan.h"
#include "soc_customize_wifi.h"
#include "hmac_config.h"
#include "wal_liteos_sdp.h"
#ifdef _PRE_WLAN_FEATURE_ROAM
#include "hmac_roam_main.h"
#include "hmac_11v.h"
#endif

#include "hmac_m2u.h"
#ifdef _PRE_WLAN_FEATURE_DFX_CHR
#include "hmac_chr.h"
#endif
#include "hmac_sdp.h"
#include "hmac_feature_dft.h"
#ifdef _PRE_WLAN_FEATURE_FTM
#include "hmac_ftm.h"
#endif
#include "hmac_csa_ap.h"
#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
#include "hmac_wow.h"
#include "msg_wow_rom.h"
#endif
#include "msg_dft_rom.h"
#include "msg_alg_rom.h"
#ifndef _PRE_WLAN_FEATURE_WS73
#include "debug_print.h"
#endif
#include "hmac_bsrp_nfrp.h"
#ifdef SUPPORT_HCC
#include "hcc_if.h"
#include "hcc_ipc_adapt.h"
#endif
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
#include "hmac_al_tx_rx.h"
#endif
#ifdef AT_COMMAND
#include "at.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_HIPRIV_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#define EXT_WIFI_FAIL    (-1)

/* TPC CH0 功率值设置的上下限，RF1.0版本调试稳定后需同接口 wal_ccpriv_dscr_set_tpc_ch0（）一起移除 */
#define TPC_CH0_LOW_LIMIT 4
#define TPC_CH0_UP_LIMIT 118
#define CHANNEL_ARRAY_LEN 14

#define WAL_RXINFO_RSP_BUFF_LEN 256
#ifdef AT_COMMAND
#define CCPRIV_CONSOLE_PRINT uapi_at_print
#else
#define CCPRIV_CONSOLE_PRINT wifi_printf_always
#endif

typedef enum {
    WAL_DSCR_PARAM_TXRTS_ANTENNA        = 0,
    WAL_DSCR_PARAM_RXCTRL_ANTENNA       = 1,
    WAL_DSCR_PARAM_DATA_RATE0           = 2,
    WAL_DSCR_PARAM_DATA_RATE1           = 3,
    WAL_DSCR_PARAM_DATA_RATE2           = 4,
    WAL_DSCR_PARAM_DATA_RATE3           = 5,
    WAL_DSCR_PARAM_POWER                = 6,
    WAL_DSCR_PARAM_PREAMBLE_MODE        = 7,
    WAL_DSCR_PARAM_RTSCTS               = 8,
    WAL_DSCR_PARAM_LSIGTXOP             = 9,
    WAL_DSCR_PARAM_SMOOTH               = 10,
    WAL_DSCR_PARAM_SOUNDING             = 11,
    WAL_DSCR_PARAM_TXBF                 = 12,
    WAL_DSCR_PARAM_STBC                 = 13,
    WAL_DSCR_PARAM_GET_ESS              = 14,
    WAL_DSCR_PARAM_DYN_BW               = 15,
    WAL_DSCR_PARAM_DYN_BW_EXIST         = 16,
    WAL_DSCR_PARAM_CH_BW_EXIST          = 17,
    WAL_DSCR_PARAM_MCS                  = 18,
    WAL_DSCR_PARAM_NSS                  = 19,
    WAL_DSCR_PARAM_BW                   = 20,
    WAL_DSCR_PARAM_LTF                  = 21,
    WLA_DSCR_PARAM_GI                   = 22,
    WLA_DSCR_PARAM_TXCHAIN              = 23,
    WLA_DSCR_PARAM_FEC                  = 24,
    WLA_DSCR_PARAM_DCM                  = 25,
    WLA_DSCR_PARAM_PE                   = 26,
    WAL_DSCR_PARAM_PROTOCOL             = 27,
    WAL_DSCR_PARAM_UPLINK_FLAG          = 28,
    WAL_DSCR_PARAM_BUTT
}wal_dscr_param_enum;

/*****************************************************************************
  1. 宏定义
*****************************************************************************/
#define ALG_RTS_CFG_RANK_SHIFT 3
#define ALG_RTS_CFG_RATE_SHIFT 6
#define AL_TXRX_TEMP_CHANNEL 6
/*****************************************************************************
  2. 结构体定义
*****************************************************************************/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

static td_bool g_under_ps = OSAL_FALSE; /* 是否处于低功耗状态 */
#ifdef CONFIG_FACTORY_TEST_MODE
td_u8 g_factory_status = 0;    /* 产测bin启动状态 */
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/* 返回协议低功耗状态 */
td_bool is_under_ps(osal_void)
{
    return g_under_ps;
}

td_void set_under_ps(td_bool under_ps)
{
    g_under_ps = under_ps;
}

/*****************************************************************************
 功能描述  : 开启或关闭ampdu发送功能

 修改历史      :
  1.日    期   : 2013年8月27日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32  uapi_ccpriv_ampdu_tx_on(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    td_u32                          off_set;
    td_s8                         ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32                          ret;
    td_u8                           aggr_tx_on;
    td_s32                          send_ret;

    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_ampdu_tx_on::wal_get_cmd_one_arg err[%u]!}\r\n", ret);
        return ret;
    }

    aggr_tx_on = (td_u8)atoi((const osal_char *)ac_name);

    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_AMPDU_TX_ON,
        (td_u8 *)&aggr_tx_on, OAL_SIZEOF(td_u8));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_ampdu_tx_on::return err code [%d]!}", send_ret);
        return (osal_u32)send_ret;
    }
    uapi_at_printf("OK\r\n");
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_DFT_STAT
/* VAP USER 信息通过HSO维测打印,关闭维测后删除接口 */
/*****************************************************************************
 功能描述  : 打印vap的所有参数信息
*****************************************************************************/
td_u32 uapi_ccpriv_vap_info(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    td_u32                      ret;
    osal_s32 value = 0;

    if (*pc_param != '\0') {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_vap_info::cmd len error}");
        return OAL_FAIL;
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_VAP_INFO, (osal_u8 *)&value,
        OAL_SIZEOF(value));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_vap_info::return err code [%u]!}", ret);
        return ret;
    }
    uapi_at_printf("OK\r\n");

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 打印user的所有参数信息
*****************************************************************************/
osal_u32 uapi_ccpriv_user_info(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    osal_u32                          ret;
    osal_u8                       mac_addr[6] = {0};    /* 临时保存获取的use的mac地址信息 */
    osal_u16                      user_idx;

    /* 规则6.6：禁止使用内存操作类危险函数 例外(1)对固定长度的数组进行初始化，或对固定长度的结构体进行内存初始化 */
    if (*pc_param == ' ') {
        pc_param++;
    }
    oal_strtoaddr((const osal_char *)pc_param, (osal_u8 *)mac_addr);

    /* 建议5.5 这里强制类型转换会不会出问题 */
    if (wal_macaddr_check((const osal_u8 *)mac_addr) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "uapi_ccpriv_user_info:: Mac address invalid!");
        return OAL_FAIL;
    }

    pc_param += WLAN_MAC_ADDR_BYTE_LEN;
    /* 判断命令长度是否有问题 */
    if (*pc_param != '\0') {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_user_info::cmd len error!}\r\n");
        return OAL_FAIL;
    }

    /* 根据mac地址找用户 */
    hmac_vap_stru *mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    ret = hmac_vap_find_user_by_macaddr_etc(mac_vap, (const osal_u8 *)mac_addr, &user_idx);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_user_info::no such user!}\r\n");
        return ret;
    }
    mac_cfg_user_info_param_stru user_info;
    (osal_void)memset_s(&user_info, OAL_SIZEOF(user_info), 0, OAL_SIZEOF(user_info));

    /* 设置配置命令参数 */
    user_info.user_idx = user_idx;
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(mac_vap->vap_id, WLAN_MSG_W2H_CFG_USER_INFO,
        (osal_u8 *)&user_info, OAL_SIZEOF(user_info));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_user_info::return err code [%u]!}", ret);
        return ret;
    }
    uapi_at_printf("OK\r\n");

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WIFI_DEBUG
static osal_u8 g_wal_tx_datatype_debug_on = OSAL_FALSE;
WIFI_HMAC_TCM_TEXT WIFI_TCM_TEXT osal_u8 wal_tx_data_type_debug_on(osal_void)
{
    return g_wal_tx_datatype_debug_on;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_set_tx_pkt_trace_debug
 功能描述  : 维测命令，控制tx pkt trace打印开关
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
static td_u32 uapi_ccpriv_set_tx_pkt_trace_debug(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_u32                          off_set;
    td_s8                         ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32                          ret;
    td_u8                           debug_on;

    ret = wal_get_cmd_one_arg(param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{uapi_ccpriv_set_tx_pkt_trace_debug::wal_get_cmd_one_arg return err_code [%u]!}\r\n", ret);
        return ret;
    }

    debug_on = (td_u8)atoi((const osal_char *)ac_name);
    if (debug_on > 1) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_tx_pkt_trace_debug::debug_on[%d] is err!}", debug_on);
        return OAL_FAIL;
    }
    g_wal_tx_datatype_debug_on = debug_on;
    /***************************************************************************
                             抛事件到wal层处理 DATA_TYPE
    ***************************************************************************/
    ret = (osal_u32)wal_async_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_SET_TX_DATA_TYPE_DEBUG,
        (td_u8 *)&debug_on, OAL_SIZEOF(td_u8), FRW_POST_PRI_LOW);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_tx_pkt_trace_debug::return err code [%d]!}", ret);
        return (osal_u32)ret;
    }

    wifi_printf("OK\r\n");
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 函 数 名  : uapi_ccpriv_send_pkt
 功能描述  : 发送任意报文
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
td_u32  uapi_ccpriv_send_pkt(oal_net_device_stru *net_dev, osal_s8 *param)
{
    td_u32 ret;
    td_u32 off_set = 0;
    osal_s8 *pkt_name;
    osal_u8 *pkt_data = OSAL_NULL;
    osal_u32 pkt_len, tmp_len;
    osal_s8 *temp = (osal_s8 *)param;
    if (net_dev == OSAL_NULL || param == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "uapi_ccpriv_send_pkt input net_dev or param is NULL.");
        return OAL_FAIL;
    }
    /* 数组申请内存 */
    pkt_name = (osal_s8 *)oal_mem_alloc(OAL_MGMT_NETBUF, WAL_CCPRIV_CMD_PKT_NAME_MAX_LEN, OAL_NETBUF_PRIORITY_MID);
    if (pkt_name == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_send_pkt::pkt_name malloc fail!}");
        return OAL_FAIL;
    }
    (osal_void)memset_s(pkt_name, WAL_CCPRIV_CMD_PKT_NAME_MAX_LEN, 0, WAL_CCPRIV_CMD_PKT_NAME_MAX_LEN);
    /* 获取第一个参数 data	*/
    ret = wal_get_cmd_one_arg(temp, pkt_name, WAL_CCPRIV_CMD_PKT_NAME_MAX_LEN, &off_set);
    /* 传入的是十六进制字符，长度为name真实长度的2倍 */
    tmp_len = (osal_u32)osal_strlen((const osal_char *)pkt_name);
    if ((ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "{wal_get_cmd_one_arg_etc_pkt::err %d!, strlen(%d).}", ret, tmp_len);
        oal_mem_free(pkt_name, OAL_TRUE);
        return OAL_FAIL;
    }
    /* 获取报文长度等于字符串长度除以2 */
    pkt_len = tmp_len / 2;
    /* 判断最大长度时,避免输入长度为奇数除2后将最后一字节截掉的情况,直接用输入的原始字符串长度比较 */
    if ((pkt_len < WAL_CCPRIV_CMD_PKT_MIN_LEN) || (tmp_len > WAL_CCPRIV_CMD_PKT_MAX_LEN * 2)) {
        oam_error_log1(0, OAM_SF_ANY, "uapi_ccpriv_send_pkt invalid input len[%d].", tmp_len);
        oal_mem_free(pkt_name, OAL_TRUE);
        return OAL_FAIL;
    }
    /*
    * 申请内存存放用户信息，将内存指针作为事件payload抛下去
    * 此处申请的内存在事件处理函数释放(hmac_config_send_custom_pkt)
    */
    pkt_data = oal_mem_alloc(OAL_MGMT_NETBUF, (osal_u16)pkt_len, OAL_NETBUF_PRIORITY_MID);
    if (pkt_data == OSAL_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_send_pkt::data fail(size:%u), return null!}", pkt_len);
        oal_mem_free(pkt_name, OAL_TRUE);
        return OAL_FAIL;
    }
    (osal_void)memset_s(pkt_data, pkt_len, 0, pkt_len);
    wal_ccpriv_str_to_hex(pkt_name, (td_s16)tmp_len, pkt_data);
    /* 打印构造的数据包信息 */
    oam_warning_log4(0, OAM_SF_ANY, "{uapi_ccpriv_send_pkt::create pkt:len[%d]::%02x xxxxxxx %02x %02x}", pkt_len,
        pkt_data[0], pkt_data[pkt_len - 2], pkt_data[pkt_len - 1]); /* 打印数据包后1 2个字节的信息 */
    /* 释放数组内存 */
    oal_mem_free(pkt_name, OAL_TRUE);
    /* 抛事件到wal层处理 */
    ret = wal_send_custom_pkt(net_dev, pkt_data, pkt_len);
    if (osal_unlikely(ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY, "uapi_ccpriv_send_pkt failed ret[%d].", ret);
    }
    /* 释放用户侧数据内存 */
    oal_mem_free(pkt_data, OAL_TRUE);
    return ret;
}

td_u32 wal_send_custom_pkt(oal_net_device_stru *net_dev, osal_u8 *data, osal_u32 len)
{
    td_u32 ret;

    if (data == NULL) {
        oam_error_log0(0, 0, "wal_send_custom_pkt data NULL.");
        return OAL_FAIL;
    }

    /* 打印构造的数据包信息 */
    oam_warning_log4(0, OAM_SF_ANY, "{wal_send_custom_pkt:: len[%d]::%02x xxxxxxx %02x %02x}", len,
        data[0], data[1], data[len - 1]); /* 打印数据包后1 2个字节的信息 */

    ret = (osal_u32)wal_async_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_SEND_CUSTOM_PKT,
        data, len, FRW_POST_PRI_LOW);
    if (osal_unlikely(ret != OAL_SUCC)) {
        oam_error_log1(0, OAM_SF_ANY, "wal_send_custom_pkt failed ret[%d].", ret);
    }
    return ret;
}


/*****************************************************************************
 函 数 名  : wal_ccpriv_str_to_hex
 功能描述  : 字符串转16进制
*****************************************************************************/
osal_void wal_ccpriv_str_to_hex(osal_s8 *param, td_s16 len, osal_u8 *value)
{
    td_s16 index;
    if (param == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wal_ccpriv_str_to_hex input param is NULL.");
        return;
    }
    for (index = 0; index < len; index++) {
        /* 每2位字符转为一个十六进制，第一位乘以16 */
        value[index / 2] = (osal_u8)(value[index / 2] * 16 * (index % 2) + oal_strtohex(param));
        param++;
    }
}

/*****************************************************************************
 功能描述  : 读取国家码
*****************************************************************************/
td_u32 uapi_ccpriv_getcountry(oal_net_device_stru *netdev, td_char *pc_param)
{
#ifdef _PRE_WLAN_FEATURE_11D
    td_u32 ret;
    osal_u8 value = 0;
    frw_msg cfg_info;
    mac_cfg_get_country_stru fill_msg;

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_msg_init(&value, sizeof(value), (osal_u8 *)&fill_msg, sizeof(fill_msg), &cfg_info);

    ret = (osal_u32)send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_COUNTRY, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_getcountry:: alloc_cfg_event failed!}");
        return ret;
    }
    /* 处理返回消息 */
    pc_param[0] = (td_char)cfg_info.rsp[0];
    pc_param[1] = (td_char)cfg_info.rsp[1];
    pc_param[2] = 0; /* 2: 第3位 */

    wifi_printf("uapi_ccpriv_getcountry:: %c%c\r\n", cfg_info.rsp[0], cfg_info.rsp[1]);

#else
    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_getcountry::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置管制域国家码

 修改历史      :
  1.日    期   : 2015年1月16日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 uapi_ccpriv_setcountry(oal_net_device_stru *netdev, td_char *param)
{
#ifdef _PRE_WLAN_FEATURE_11D
    td_u32                        ret;
    td_u32                        off_set;
    osal_char                    arg[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_char                      *para = OAL_PTR_NULL;

    if (wal_dev_is_running() == OAL_TRUE) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::device is busy}");
    }
    /* 获取国家码字符串 */
    if (*param == ' ') {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::cmd error!}\r\n");
        return OAL_FAIL;
    }
    ret = wal_get_cmd_one_arg((const osal_s8 *)param, (osal_s8 *)arg, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::wal_get_cmd_one_arg return err_code [%u]!}\r\n", ret);
        return ret;
    }
    para = &arg[0];

    param += off_set;
    /* 判断命令长度是否有问题 */
    if (*param != '\0') {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_user_info::cmd len error!}\r\n");
        return OAL_FAIL;
    }
    if (osal_strlen(para) > (MAC_CONTRY_CODE_LEN - 1)) {
        return OAL_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_DFS
    // 下发雷达过滤参数
    ret = wal_regdomain_update_for_dfs(netdev, para);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::wal_regdomain_update_for_dfs err %u!}\r\n",
            ret);
        return ret;
    }
#endif
    ret = wal_regdomain_update(netdev, para, MAC_CONTRY_CODE_LEN);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::regdomain_update return err code %u!}\r\n", ret);
        return ret;
    }
#else
    oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_setcountry::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 删除vap
*****************************************************************************/
td_u32 wal_ccpriv_del_vap(oal_net_device_stru *netdev, td_char *pc_param)
{
    unref_param_prv(pc_param);
    /* 设备在up状态不允许删除，必须先down */
    if (OAL_UNLIKELY((OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(netdev)) != 0)) {
        oam_error_log1(0, OAM_SF_ANY, "{wal_ccpriv_del_vap::netdev status[%d] is busy, please down it first!}",
            OAL_NETDEVICE_FLAGS(netdev));
        return OAL_ERR_CODE_CONFIG_BUSY;
    }
    /* 去注册 */
    oal_net_unregister_netdev(netdev);
    oal_net_free_netdev(netdev);
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 获取带宽

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_ccpriv_get_bw(oal_net_device_stru *netdev, hal_channel_assemble_enum_uint8  *pen_bw_index)
{
    unref_param(netdev);
    unref_param(pen_bw_index);
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置带宽
 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_ccpriv_set_bw(oal_net_device_stru *netdev, td_char *pc_param)
{
    unref_param(netdev);
    unref_param(pc_param);
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 数据常发

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_always_tx(oal_net_device_stru *netdev, td_u8 tx_flag)
{
    td_u32                ret;
    mac_cfg_tx_comp_stru set_bcast_param = {0};

    /* 解析并设置配置命令参数 */
    set_bcast_param.param = tx_flag;
    set_bcast_param.tpc_code = 0xff;    // 采用tpc算法结果 不使用固定档位
    if (tx_flag == 0) {
        set_bcast_param.payload_flag = RF_PAYLOAD_ALL_ZERO;
        set_bcast_param.payload_len = 0;
    } else {
        set_bcast_param.payload_flag = RF_PAYLOAD_ALL_AA;
        set_bcast_param.payload_len = 1000; /* 1000:常发payload长度 */
        set_bcast_param.al_tx_max = 0;
    }

    wifi_printf("{uapi_ccpriv_always_tx:: tx_flag=[%d] payload_len=[%d]} \r\n", tx_flag, set_bcast_param.payload_len);

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_SET_ALWAYS_TX,
        (osal_u8 *)&set_bcast_param, OAL_SIZEOF(mac_cfg_tx_comp_stru));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "{uapi_ccpriv_always_tx:: return Err=%u, tx_flag=%d}", ret, tx_flag);
    } else {
        if ((tx_flag == HAL_ALWAYS_TX_RF) || (tx_flag == HAL_ALWAYS_TX_DC)) {
            uapi_at_printf("OK\r\n");
        }
    }

    return ret;
}

/*****************************************************************************
 功能描述  : 混杂模式命令参数解析
 修改历史      :
  1.日    期   : 2021年4月21日
    修改内容   : 新生成函数
*****************************************************************************/
OAL_STATIC osal_u32 wal_ccpriv_set_monitor_cmd_proc(osal_s8 *param, osal_u8 *cmd)
{
    osal_u8 cur_para;
    osal_s32 i;
    osal_u32 ret;
    osal_u32 off_set = 0;
    osal_s8 arg_list[4][WAL_CCPRIV_CMD_NAME_MAX_LEN]; /* 4:用于存储四种帧类型的开关参数 */
 
    for (i = 0; i < 4; ++i) { /* 第1/2/3/4个参数分别是:mdata,udata,mmgmt,umgmt */
        param += off_set;
        ret = wal_get_cmd_one_arg(param, &(arg_list[i][0]), WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
        if (ret != OAL_SUCC) {
            oam_warning_log2(0, OAM_SF_ANY, "{wal_ccpriv_set_monitor_cmd_proc::parse cmd err=%d, i=%d}", ret, i);
            return OAL_CONTINUE;
        }
        cur_para = (osal_u8)oal_atoi((const osal_s8 *)arg_list[i]);
        if ((cur_para == 0) || (cur_para == 1)) {
            *cmd |= (cur_para << (osal_u32)i);
        } else {
            oam_error_log2(0, OAM_SF_ANY, "{wal_ccpriv_set_monitor_cmd_proc::invalid cur_para=%d, i=%d}", cur_para, i);
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    return OAL_SUCC;
}

/* 功能描述  : 混杂模式命令 */
td_u32 uapi_ccpriv_set_monitor_switch(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    osal_u8            msg_parma[2] = {0}; /* 2：表示msg参数有两个 */
    osal_u8            cmd_para = 0;
    osal_u32            cmd_para_switch = 0;
    osal_u32           ret;

    ret = wal_cmd_get_digit_with_check_max(&pc_param, WAL_MONITER_SWITCH_MODE, &cmd_para_switch);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_monitor_switch::return err_code[%u]!}", ret);
        return ret;
    }

    if (cmd_para_switch == 0) {
        /* 关闭混杂模式 */
        cmd_para = 0;
    } else if (cmd_para_switch == 1) {
        /* 开启混杂模式 */
        ret = wal_ccpriv_set_monitor_cmd_proc(pc_param, &cmd_para);
        if (ret != OAL_SUCC && ret != OAL_CONTINUE) {
            uapi_at_printf("Error: get switch para fail, ret[%d]!\r\n", ret);
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_monitor_switch::cmd proc err [%d]}", ret);
            return ret;
        }
    }

    oam_warning_log2(0, OAM_SF_ANY, "{uapi_ccpriv_set_monitor_switch::switch=[%d], cmd_para=[%d]}",
                     cmd_para_switch, cmd_para);

    /***************************************************************************
                                抛事件到wal层处理
    ***************************************************************************/
    msg_parma[0] = cmd_para;
    msg_parma[1] = (osal_u8)cmd_para_switch;
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_MONITOR_EN,
        (osal_u8 *)&msg_parma, sizeof(msg_parma));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        uapi_at_printf("Error: send set_monitor_switch message to device error!\r\n");
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_monitor_switch::return err code [%u]!}", ret);
        return ret;
    }

    uapi_at_printf("OK\r\n");
    return OAL_SUCC;
}

OAL_STATIC osal_u32 wal_ccpriv_always_tx_get_ratio_param(oal_net_device_stru *net_dev, osal_s8 *param, osal_u32 len)
{
    osal_u32 ret;
    osal_s32 duty_ratio;
    osal_u32 rate;
    al_tx_back_off_time_stru back_off_time_stru;

    /* 常发占空比设置分为1~10档 */
    ret = (osal_u32)wal_cmd_get_digit_with_range(&param, WAL_AL_TX_RATIO_LOW, WAL_AL_TX_RATIO_HIGH, &duty_ratio);
    if (ret != OAL_SUCC && ret != OAL_ERR_CODE_PTR_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_always_tx_get_ratio_param::set param error}");
        return ret;
    }

    if (ret == OAL_ERR_CODE_PTR_NULL) {
        duty_ratio = 7; /* 客户下发命令不设置占空比参数时，设置默认占空比为第7档 */
    }
    hmac_set_al_tx_duty_rate((osal_u8)(osal_u32)duty_ratio);
    rate = uapi_get_al_tx_rate(hmac_get_al_tx_protocol(), hmac_get_al_tx_mcs());
    back_off_time_stru.back_off_time = uapi_get_back_off_time(rate, len, (osal_u32)duty_ratio);

    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_ALWAYS_TX_RATIO,
        (osal_u8 *)&back_off_time_stru, OAL_SIZEOF(al_tx_back_off_time_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_always_tx_get_ratio_param::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_CONTINUE;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_always_tx_ccpriv
 功能描述  : 数据常发
*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_always_tx_ccpriv(oal_net_device_stru *net_dev, td_s8 *param)
{
    osal_u32 ret;
    mac_cfg_tx_comp_stru set_bcast_param = {0};
    osal_u32 tx_flag;
    mac_rf_payload_enum_uint8 payload_flag = RF_PAYLOAD_RAND;
    osal_u32 len = 1000;

    /* 获取常发模式开关标志 */
    ret = wal_cmd_get_digit_with_check_max(&param, HAL_ALWAYS_TX_RF, &tx_flag);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_always_tx_ccpriv::tx flag fail, return err_code[%u]!}", ret);
        return ret;
    }

    /* 关闭的情况下不需要解析后面的参数 */
    if (tx_flag != HAL_ALWAYS_TX_DISABLE) {
        ret = wal_ccpriv_always_tx_get_param(&param, &payload_flag, &len, &set_bcast_param);
        if (ret != OAL_CONTINUE) {
            return ret;
        }

        ret = wal_ccpriv_always_tx_get_ratio_param(net_dev, param, len);
        if (ret != OAL_CONTINUE) {
            return ret;
        }
    }

    set_bcast_param.payload_flag = payload_flag;
    set_bcast_param.payload_len = len;
    set_bcast_param.param = (osal_u8)tx_flag;

    oam_warning_log3(0, OAM_SF_ANY, "uapi_ccpriv_always_tx_ccpriv tx_flag=[%d] payload_flag=[%d] len=[%d]\r\n",
        set_bcast_param.param, set_bcast_param.payload_flag, set_bcast_param.payload_len);

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_SET_ALWAYS_TX,
        (osal_u8 *)&set_bcast_param, OAL_SIZEOF(mac_cfg_tx_comp_stru));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_always_tx_ccpriv::return err code [%d]!}", ret);
        return ret;
    }

    if ((set_bcast_param.payload_flag == RF_PAYLOAD_DATA) && (set_bcast_param.payload != OSAL_NULL)) {
        oal_mem_free(set_bcast_param.payload, OAL_TRUE);
        set_bcast_param.payload = OSAL_NULL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 数据常收

 修改历史      :
  1.日    期   : 2014年3月28日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_always_rx(oal_net_device_stru *netdev, td_u8 rx_flag)
{
    td_u32                ret;

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_C_CFG_SET_ALWAYS_RX,
        &rx_flag, OAL_SIZEOF(rx_flag));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "{uapi_ccpriv_always_rx::return err code=%u, rx_flag=%d}", ret, rx_flag);
    } else {
        if (rx_flag == 1) {
            uapi_at_printf("OK\r\n");
        }
    }

    return ret;
}
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
OAL_STATIC osal_s32 uapi_ccpriv_rx_info_detail(hmac_vap_stru *hmac_vap, osal_char *out_buff, osal_u16 out_buff_len)
{
    osal_s32 ret;
    frw_msg cfg_info = {0};
    mac_cfg_al_rx_info_stru rx_info = {1, 1};   // 每次查询清零
    cfg_msg_init((osal_u8 *)&rx_info, sizeof(rx_info), (osal_u8 *)out_buff, (osal_u32)out_buff_len, &cfg_info);
    ret = send_sync_cfg_to_host(hmac_vap->vap_id, WLAN_MSG_W2H_CFG_GET_RX_PPDU_INFO, &cfg_info);
    if (ret != OAL_SUCC) {
        oam_error_log0(hmac_vap->vap_id, OAM_SF_ANY, "uapi_ccpriv_al_rx_info_detail::send_sync_cfg_to_host err!");
        return ret;
    }

    if (cfg_info.rsp_len < out_buff_len) {
        out_buff[cfg_info.rsp_len] = '\0';
    }

    return OAL_SUCC;
}
#endif
/*****************************************************************************
 功能描述  : 打印对应vap的接收FCS的信息

 修改历史      :
  1.日    期   : 2019年5月5日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 uapi_ccpriv_rx_fcs_info(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32                  ret;
    hmac_vap_stru            *mac_vap = OSAL_NULL;
    hmac_vap_stru           *hmac_vap = OSAL_NULL;
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    osal_char *out_buff = OSAL_NULL;
#endif

    unref_param_prv(pc_param);
    mac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (mac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_rx_fcs_info::dev->ml_priv, return null!}");
        return EXT_ERR_FAILURE;
    }
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(mac_vap->vap_id);
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_rx_fcs_info::hmac_vap_get_vap_stru, return null!}");
        return EXT_ERR_FAILURE;
    }

    ret = (osal_u32)wal_get_rx_pckg(hmac_vap, 0, 0);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_rx_fcs_info::return err code [%u]!}\r\n", ret);
    }

    CCPRIV_CONSOLE_PRINT("+RXINFO::rx succ num[mpdu,ampdu]:[%u,%u] fail num:%u rssi:%d\r\n",
        hmac_vap->atcmdsrv_get_status.rx_pkct_succ_num, hmac_vap->atcmdsrv_get_status.rx_ampdu_succ_num,
        hmac_vap->atcmdsrv_get_status.rx_fail_num, hmac_vap->atcmdsrv_get_status.s_rx_rssi);

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    out_buff = (osal_char *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, WAL_RXINFO_RSP_BUFF_LEN, OAL_TRUE);
    if (out_buff == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_rx_fcs_info::malloc fail!}");
        return OAL_FAIL;
    }
    (osal_void)memset_s(out_buff, WAL_RXINFO_RSP_BUFF_LEN, 0, WAL_RXINFO_RSP_BUFF_LEN);

    ret = (td_u32)uapi_ccpriv_rx_info_detail(hmac_vap, out_buff, WAL_RXINFO_RSP_BUFF_LEN);
    if (ret != OAL_SUCC) {
        goto exit;
    }

    CCPRIV_CONSOLE_PRINT(out_buff);
    ret = OAL_SUCC;
exit:
    if (out_buff != OSAL_NULL) {
        oal_mem_free(out_buff, OAL_TRUE);
        out_buff = OSAL_NULL;
    }
#endif

    return ret;
}

static td_u32 wal_check_al_txrx_channel(oal_net_device_stru *netdev, td_s32 protocol)
{
    td_u32 ret;
    frw_msg cfg_info;
    td_s32 fill_msg;
    td_u8 channel;

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = sizeof(fill_msg);
    cfg_info.rsp = (td_u8 *)&fill_msg;

    ret = (td_u32)send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_GET_CURRENT_CHANEL, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log1(0, OAM_SF_ANY, "{check_protocol_channel::return err code %d!}", ret);
        return EXT_WIFI_INVALID_CHANNEL;
    }

    channel = *(cfg_info.rsp);
    oam_warning_log1(0, OAM_SF_ANY, "{check_protocol_channel::last channel is [%d]!}", channel);
    /* 不满足条件就先设置信道6，后面解析信道参数设置时会覆盖当前设置 */
    if ((protocol == 5 && channel > hmac_chan_get_bw_40m_max_chan_idx()) || /* 5:代表模式11n2g40plus */
        (protocol == 6 && channel < HMAC_BW_40M_MIN_CHAN_IDX)) { /* 6:代表模式11n2g40minus */
        channel = AL_TXRX_TEMP_CHANNEL;
        ret = (td_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CURRENT_CHANEL,
            &channel, OAL_SIZEOF(channel));
    }
    return ret;
}

OAL_STATIC td_u32 wal_set_protocol(oal_net_device_stru *netdev, td_char *protocol_param,
    td_s32 len, td_s32 *protocol_value)
{
    td_s32 protocol = strtol(protocol_param, NULL, 10); /* 10进制 */
    td_u32 ret;

    if (len != 1) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_set_protocol:: protocol len [%d] error!}\r\n", len);
        return OAL_FAIL;
    }

    switch (protocol) {
        case 0: /* 0:代表模式11bgn */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11bgn");
            break;
        case 1: /* 1:代表模式11bg */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11bg");
            break;
        case 2: /* 2:代表模式11b */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11b");
            break;
        case 3: /* 3:代表模式11bgnax */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11bgnax");
            break;
        case 4: /* 4:代表模式11n2g40 */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11n2g40");
            break;
        case 5: /* 5:代表模式11n2g40plus */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11n2g40plus");
            break;
        case 6: /* 6:代表模式11n2g40minus */
            /* 安全编程规则6.6例外(5) 源内存全部是静态字符串常量 */
            strcpy_s(protocol_param, WAL_CCPRIV_CMD_NAME_MAX_LEN, "11n2g40minus");
            break;
        default:
            oam_warning_log0(0, OAM_SF_ANY, "{wal_set_protocol:: protocol error!}\r\n");
            return OAL_FAIL;
    }

    /* 频宽为40M上偏或者下偏时需要校验当前信道是否满足条件 */
    if (protocol > 4) {
        ret = wal_check_al_txrx_channel(netdev, protocol);
        if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_check_rx_param::set channel return err code [%d]!}", ret);
        }
    }

    ret = uapi_ccpriv_set_mode(netdev, (td_s8 *)protocol_param);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "wal_set_protocol::set_mode return err_code [%d]!", ret);
        return ret;
    }

    *protocol_value = protocol;
    if (protocol > 3) { // 3以上都是11n
        *protocol_value = 0;
    }
    return OAL_SUCC;
}

OAL_STATIC td_u32 wal_ccpriv_get_value(const td_char *pc_param, td_char *ac_arg, td_u32 *off_set,
    td_u32 first_elem_flag)
{
    td_u32 ret;
    td_u32 check_offset = (first_elem_flag == 0) ? 0 : 1;

    if ((*(pc_param + check_offset)) == ' ') {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_get_value::cmd error}");
        return OAL_FAIL;
    }

    ret = wal_get_cmd_one_arg((const osal_s8 *)pc_param, (osal_s8 *)ac_arg, WAL_CCPRIV_CMD_NAME_MAX_LEN, off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_get_value::wal_get_cmd_one_arg return err_code[%u]}", ret);
        return ret;
    }

    return OAL_SUCC;
}

OAL_STATIC td_u32 wal_ccpriv_check_switch(const td_char *ac_arg, const td_char *pc_param, td_bool is_tx)
{
    /* 如果是关闭常收，则直接关闭，无需解析后面的参数 */
    if (osal_strcmp("0", ac_arg) == 0) {
        /* 判断命令长度是否有问题 */
        if (*pc_param != '\0') {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_check_switch::cmd len error}");
            return OAL_FAIL;
        }

        wifi_printf("OK\r\n");
        return OAL_SUCC;
    }

    /* 判断开关字符是否有问题,1:打开长发;2:tx_dc */
    if ((osal_strcmp("1", ac_arg) != 0) && !(is_tx && (osal_strcmp("2", ac_arg) == 0))) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_check_switch::osal_strcmp err_code}");
        return OAL_FAIL;
    }

    if (is_tx == OSAL_TRUE) {
        if (osal_strcmp("2", ac_arg) == 0) {
            hmac_set_al_tx_frame_type_switch(1); /* 2:常发速率使能开关 */
            return OAL_SUCC;
        } else {
            hmac_set_al_tx_frame_type_switch(0); /* 2:常发速率使能开关 */
        }
    }
    return OAL_CONTINUE;
}

OAL_STATIC td_u32 wal_ccpriv_check_rx_param(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32 ret, off_set;
    td_s32  protocol_value = 0;
    td_char ac_arg[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_char *ac_arg_ptr = ac_arg;
    int32_t channel;
    td_bool flag;

    for (td_u32 i = 0; i < 5; i++) { /* 常收命令有5个参数 */
        ret = wal_ccpriv_get_value(pc_param, ac_arg_ptr, &off_set, ((i == 0) ? 0 : 1));
        if (ret != OAL_SUCC) {
            return ret;
        }

        pc_param += off_set;

        if (i == 0) {       /* 0: 常收使能开关 */
            ret = wal_ccpriv_check_switch(ac_arg_ptr, pc_param, OSAL_FALSE);
            if (ret != OAL_CONTINUE) {
                return ret;
            }
        } else if (i == 1) { /* 1: 设置协议类型 */
            ret = wal_set_protocol(netdev, ac_arg_ptr, (osal_s32)osal_strlen(ac_arg_ptr), &protocol_value);
        } else if (i == 2) { /* 2: 设置带宽 */
            if ((protocol_value != 0) && (atoi(ac_arg_ptr) == 40)) { /* 0:11n 40:bw */
                oam_error_log1(0, OAM_SF_ANY, "wal_ccpriv_check_rx_param::set_bw return err_code [%d]}", ret);
                return OAL_FAIL;
            }
            ret = wal_ccpriv_set_bw(netdev, ac_arg_ptr);
        } else if (i == 3) { /* 3: 设置信道频点 */
            channel = atoi(ac_arg_ptr);
            ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CURRENT_CHANEL,
                (osal_u8 *)&channel, OAL_SIZEOF(channel));
            /* 14信道仅支持11b模式 */
            if ((protocol_value != 2) && (channel == 14)) { /* 2:11b模式,14:信道 */
                return OAL_FAIL;
            }
        }

        flag = (i == 1) || (i == 2) || (i == 3); /* 参数1/2/3的异常分支 */
        if (flag && (ret != OAL_SUCC)) {
            oam_error_log2(0, OAM_SF_ANY, "wal_ccpriv_check_rx_param::fun[i=%d] return err_code [%d]}", i, ret);
            return ret;
        }
    }

    /* 判断命令长度是否有问题 */
    if (*pc_param != '\0') {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_al_rx::cmd len error}");
        return OAL_FAIL;
    }

    /* 打开常收 */
    return uapi_ccpriv_always_rx(netdev, 1);
}

/*****************************************************************************
 功能描述      : 设置常收参数

 修改历史      :
  1.日    期   : 2019年5月5日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_ccpriv_rx_proc(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32 ret;

#ifdef _PRE_WLAN_FEATURE_POWERSAVE
    /* 常收前关闭低功耗 */
    osal_u8 value = 0;
    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2D_C_CFG_SET_SLEEP,
        &value, sizeof(osal_u8));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{wal_ccpriv_rx_proc::send msg failed[%d]!}", ret);
        return OAL_FAIL;
    }
#endif
    /*  产测每次常收前，关常收 */
    ret = uapi_ccpriv_always_rx(netdev, 0);
    if (ret != OAL_SUCC) {
        return ret;
    }

#ifdef _PRE_WLAN_FEATURE_WS53
    hcc_ipc_clear_premem();
#endif
    /* 设置常收的5个参数 */
    ret = wal_ccpriv_check_rx_param(netdev, pc_param);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "wal_ccpriv_rx_proc::set_rx_param return err_code [%d]}", ret);
        return ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_RF_CALI_DEBUG
osal_u32 uapi_ccpriv_signle_tone_type_other(oal_net_device_stru *net_dev, osal_s8 *param, osal_u32 *data)
{
    osal_u32                      off_set = 0;
    osal_u32                      ret;
    osal_u8                       flag = 0;
    osal_u32                      para3 = 0;
    osal_u32                      para4 = 0;
    osal_s8                       name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    unref_param(net_dev);
    /* 参数2:使能 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_signle_tone::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    flag = (osal_u8)oal_atoi((const osal_s8 *)name);
    *data |= (flag << SINGLE_TONE_TEST_8BITS);
    /* 参数3:值1 */
    param = param + off_set;
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_signle_tone::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    para3 = (osal_u32)oal_atoi((const osal_s8 *)name);
    *data |= (para3 << SINGLE_TONE_TEST_16BITS);

    /* 参数4:值2 */
    param = param + off_set;
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_signle_tone::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    para4 = (osal_u32)oal_atoi((const osal_s8 *)name);
    *data |= (para4 << SINGLE_TONE_TEST_24BITS);
    wifi_printf("\tType other, enable[%d]para3[%d]para4[%d]\n", flag, para3, para4);
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_signle_tone
 功能描述  : 单音测试
 输入参数  : 参数类型 使能 值1 值2
*****************************************************************************/
osal_u32 uapi_ccpriv_signle_tone(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 off_set = 0;
    osal_u32 ret;
    osal_u32 data;
    osal_u32 type;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    /* 参数1:参数类型 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_signle_tone::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    type = (osal_u32)oal_atoi((const osal_s8 *)name);
    data = type;
    param = param + off_set;

    ret = uapi_ccpriv_signle_tone_type_other(net_dev, param, &data);
    ret |= (osal_s32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SINGLE_TONE_TEST,
        (osal_u8 *)&data, sizeof(data));
    if (ret != OAL_SUCC) {
        wifi_printf("{uapi_ccpriv_signle_tone::return err code [%d]!}", ret);
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_signle_tone::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_txdc_step_type(oal_net_device_stru *net_dev, osal_s8 *param, osal_u32 *data)
{
    osal_u32                      para3 = 0;
    osal_u32                      para4 = 0;
    osal_u32                      off_set = 0;
    osal_u32                      ret;
    osal_s8                       name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    unref_param(net_dev);
    /* 参数3:使能 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_txdc_step_type::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    para3 = (osal_u8)oal_atoi((const osal_s8 *)name);
    *data |= (para3 << SINGLE_TONE_TEST_16BITS);
    param = param + off_set;

    /* 参数4:使能 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_txdc_step_type::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    para4 = (osal_u8)oal_atoi((const osal_s8 *)name);
    *data |= (para4 << SINGLE_TONE_TEST_24BITS);
    param = param + off_set;
    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_txdc_step
 功能描述  : txdc
 输入参数  : 参数类型 值1 值2 值3
*****************************************************************************/
osal_u32 uapi_ccpriv_txdc_step(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32                      off_set = 0;
    osal_u32                      ret;
    osal_u32                      data;
    osal_u32                      type;
    osal_u32                      para2 = 0;
    osal_s8                       name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    /* 参数1:参数类型 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_txdc_step::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    type = (osal_u32)oal_atoi((const osal_s8 *)name);
    data = type;
    param = param + off_set;

    /* 参数2:使能 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_txdc_step::wal_get_cmd_one_arg err_code [%d]!}", ret);
        return ret;
    }
    para2 = (osal_u8)oal_atoi((const osal_s8 *)name);
    data |= (para2 << SINGLE_TONE_TEST_8BITS);
    param = param + off_set;

    ret = uapi_ccpriv_txdc_step_type(net_dev, param, &data);
    if (ret != OAL_SUCC) {
        return ret;
    }

    ret = (osal_s32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_TXDC_STEP_TEST,
        (osal_u8 *)&data, sizeof(data));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_txdc_step::return err code [%d]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC td_u32 wal_ccpriv_check_tx_param(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32  ret, off_set;
    td_s32  protocol_value, channel;
    td_char ac_arg[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_char *ac_arg_ptr = ac_arg;
    td_u8   tx_flag = 0;

    for (td_u32 i = 0; i < 4; i++) { /* 常发命令有4个参数 */
        if (wal_ccpriv_get_value(pc_param, ac_arg_ptr, &off_set, ((i == 0) ? 0 : 1)) != OAL_SUCC) {
            return OAL_FAIL;
        }

        pc_param += off_set;

        if (i == 0) {        /* 0: 常发使能开关 */
            ret = wal_ccpriv_check_switch(ac_arg_ptr, pc_param, OSAL_TRUE);
            if (ret != OAL_CONTINUE) {
                return ret;
            }
            tx_flag = (unsigned char)atoi(ac_arg_ptr);
        } else if (i == 1) { /* 1: 设置协议类型 */
            if (wal_set_protocol(netdev, ac_arg_ptr, (osal_s32)osal_strlen(ac_arg_ptr), &protocol_value) != OAL_SUCC) {
                return OAL_FAIL;
            }
        } else if (i == 2) { /* 2: 设置带宽 */
            if ((protocol_value != 0) && (atoi(ac_arg_ptr) == 40)) { /* 0:11n,40:40M */
                return OAL_FAIL;
            }
        } else if (i == 3) { /* 3: 设置信道频点 */
            channel = atoi(ac_arg_ptr);
            ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CURRENT_CHANEL,
                (osal_u8 *)&channel, OAL_SIZEOF(channel));
            /* 14信道仅支持11b模式 */
            if ((ret != OAL_SUCC) || ((protocol_value != 2) && (channel == 14))) { /* 2:11b模式,14:信道 */
                return OAL_FAIL;
            }
        }
    }

    /* 判断命令长度是否有问题 */
    if (*pc_param != '\0') {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_check_tx_param::cmd len error}");
        return OAL_FAIL;
    }

    /* 打开常发 */
    return uapi_ccpriv_always_tx(netdev, tx_flag);
}

/*****************************************************************************
 功能描述      : 设置常发参数

 修改历史      :
  1.日    期   : 2019年5月5日
    修改内容   : 新生成函数
*****************************************************************************/
td_u32 wal_ccpriv_tx_proc(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32 ret;

    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        oam_warning_log0(0, 0, "under ps mode,can not exec cmd");
        return OAL_FAIL;
    }
    /* 产测每次常发前，关常发 */
    ret = uapi_ccpriv_always_tx(netdev, 0);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "wal_ccpriv_tx_proc::always_tx set 0 return err_code [%d]}", ret);
        return ret;
    }
    /* 设置常发的4个参数 */
    ret = wal_ccpriv_check_tx_param(netdev, pc_param);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "wal_ccpriv_tx_proc::set_tx_param return err_code [%d]}", ret);
        return ret;
    }

    return OAL_SUCC;
}

OAL_STATIC td_u32 wal_at_setcountry(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32 ret = uapi_ccpriv_setcountry(netdev, pc_param);
    if (ret == OAL_SUCC) {
        wifi_printf("OK\r\n");
    }

    return ret;
}

/*****************************************************************************
 功能描述  : 处理私有配置命令
 输入参数  : pc_cmd: 命令
 返 回 值  : 错误码
*****************************************************************************/
static td_u32 wal_at_cmd(oal_net_device_stru *netdev, osal_char *cmd, td_u32 cmd_type)
{
    td_u32 ret = OAL_SUCC;

    if (cmd_type == EXT_AT_AL_TX) {
        ret = wal_ccpriv_tx_proc(netdev, cmd);
    } else if (cmd_type == EXT_AT_AL_RX) {
        ret = wal_ccpriv_rx_proc(netdev, cmd);
    } else if (cmd_type == EXT_AT_RX_INFO) {
        ret = uapi_ccpriv_rx_fcs_info(netdev, cmd);
    } else if (cmd_type == EXT_AT_SET_COUNTRY) {
        ret = wal_at_setcountry(netdev, cmd);
    } else if (cmd_type == EXT_AT_GET_COUNTRY) {
        ret = uapi_ccpriv_getcountry(netdev, cmd);
        if (ret == OAL_SUCC) {
            wifi_printf("+CC:%c%c\r\n", cmd[0], cmd[1]);
            wifi_printf("OK\r\n");
        }
    } else if (cmd_type == EXT_AT_SET_WLAN0_BW) {
        ret = wal_ccpriv_set_bw(netdev, cmd);
    } else if (cmd_type == EXT_AT_SET_AP0_BW) {
        ret = wal_ccpriv_set_bw(netdev, cmd);
    } else if (cmd_type == EXT_AT_SET_MESH0_BW) {
        ret = wal_ccpriv_set_bw(netdev, cmd);
#ifdef _PRE_WLAN_DFT_STAT
    } else if (cmd_type == EXT_AT_GET_VAP_INFO) {
        ret = uapi_ccpriv_vap_info(netdev, (osal_s8 *)cmd);
    } else if (cmd_type == EXT_AT_GET_USR_INFO) {
        ret = uapi_ccpriv_user_info(netdev, (osal_s8 *)cmd);
#endif
    } else if (cmd_type == EXT_AT_SET_RATE_POWER) {
        ret = uapi_ccpriv_set_rate_power(netdev, (osal_s8 *)cmd);
    } else if (cmd_type == EXT_AT_CAL_TONE) {
        ret = uapi_ccpriv_set_cal_tone(netdev, (osal_s8 *)cmd);
    } else {
        return OAL_FAIL;
    }

    return ret;
}

/*****************************************************************************
 功能描述  : ccpriv命令入口
 输入参数  : *pc_buffer 命令参数
            count 命令长度
 返 回 值  : 字节数，0表示ccpriv失败
*****************************************************************************/
osal_u32 wal_at_entry(osal_char *buffer, osal_u32 cmd_type)
{
    td_u32               ret;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_char             *dev_name        = OSAL_NULL;
    td_char              net_device_licon[] = "Featureid0";
    td_char              *cmd_buff = buffer;
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
    td_char              net_wlan0[]     = "wlan0";
    td_char              net_ap0[]       = "ap0";
    td_char              net_mesh0[]     = "mesh0";
    td_char              ac_arg[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_char              *ac_arg_ptr = ac_arg;
    td_u32               off_set = 0;

    if ((cmd_type == EXT_AT_SET_COUNTRY) || (cmd_type == EXT_AT_GET_COUNTRY) || (cmd_type == EXT_AT_SET_ARLOG)) {
        dev_name = net_device_licon;
    } else if (cmd_type == EXT_AT_SET_AP0_BW) {
        dev_name = net_ap0;
    } else if (cmd_type == EXT_AT_SET_MESH0_BW) {
        dev_name = net_mesh0;
    } else if ((cmd_type == EXT_AT_GET_VAP_INFO) || (cmd_type == EXT_AT_GET_USR_INFO)) {
        /* 第一个参数为 wlan0 或者 ap0 */
        if (wal_ccpriv_get_value(cmd_buff, ac_arg_ptr, &off_set, 0) != OAL_SUCC) {
            return OAL_FAIL;
        }
        dev_name = ac_arg_ptr;
        cmd_buff += off_set;
    } else {
        dev_name = net_wlan0;
    }
#else
    dev_name = net_device_licon;
#endif
    /* 根据dev_name找到dev */
    netdev = oal_get_netdev_by_name(dev_name);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_at_entry::get netdev by name return null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ret = wal_at_cmd(netdev, cmd_buff, cmd_type);
    return ret;
}

osal_u32 uapi_wifi_at_start(osal_s32 argc, const osal_char *argv[], osal_u32 cmd_type)
{
    td_u32      total_len = 0;
    td_s32      index;
    td_char     temp_buffer[3]; /* 3 存储buffer */
    td_char    *buffer = NULL;
    td_char    *buffer_temp = NULL;
    td_char    *buffer_index = NULL;

#ifdef CONFIG_FACTORY_TEST_MODE
    /* 产测BIN进行常发常收测试时,如STA未正常启动直接返回失败 */
    if ((g_factory_status == OSAL_FALSE) && ((cmd_type == EXT_AT_AL_TX) || (cmd_type == EXT_AT_AL_RX))) {
        return OAL_FAIL;
    }
#endif
    /* EXT_AT_RX_INFO和EXT_AT_GET_COUNTRY场景 */
    if (argc == 0) {
        temp_buffer[0] = '\0';
        buffer_temp = temp_buffer;
    } else {
        for (index = 0; index < argc; index++) {
            total_len += osal_strlen((td_char *)argv[index]) + 1;
        }

        buffer = malloc(total_len);
        if (buffer == NULL) {
            oam_error_log0(0, OAM_SF_ANY, "uapi_wifi_at_start: malloc failed!");
            return EXT_ERR_FAILURE;
        }

        buffer_index = buffer;
        for (index = 0; index < argc; index++) {
            td_u32 len = osal_strlen((td_char *)argv[index]);
            if (memcpy_s(buffer_index, len, argv[index], len) != EOK) {
                oam_error_log0(0, 0, "{uapi_wifi_at_start::mem safe function err!}");
                free(buffer);
                return EXT_ERR_FAILURE;
            }
            buffer_index[len] = ' ';
            buffer_index += (len + 1);
        }
        buffer[total_len - 1] = '\0';
        buffer_temp = buffer;
    }

    if (wal_at_entry(buffer_temp, cmd_type) != OAL_SUCC) {
        if (buffer != NULL) {
            free(buffer);
        }
        return EXT_ERR_FAILURE;
    }

    if (buffer != NULL) {
        free(buffer);
    }
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
 功能描述  : 对不同协议场景、不用速率分别做功率补偿，传递补偿事件给hmac
 输入参数  : [1]protol 协议序号
             [2]rate 速率
             [3]val 补偿值
 输出参数  : 无
 返 回 值  : 对不同协议场景、不用速率分别做功率补偿，传递补偿事件给hmac 是否成功的结果
*****************************************************************************/
td_u32 wal_set_cal_rate_power(td_u8 protol, td_u8 rate, td_s32 val)
{
    unref_param(protol);
    unref_param(rate);
    unref_param(val);
    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        oam_warning_log0(0, 0, "under ps mode,can not exec cmd");
        return OAL_FAIL;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "wal_set_cal_rpower:Featureid0 device not fonud.");
        return OAL_FAIL;
    }

    /* 抛事件到wal层处理 */
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 进行常温频偏功率补偿，传递补偿事件给hmac
 输入参数  : [1]freq_offset 补偿值
 输出参数  : 无
 返 回 值  : 进行常温频偏功率补偿，传递补偿事件给hmac 是否成功的结果
*****************************************************************************/
td_u32 wal_set_cal_freq(td_s32 freq_offset)
{
    unref_param(freq_offset);
    /* 低功耗状态下不允许执行 */
    if (is_under_ps()) {
        oam_warning_log0(0, 0, "under ps mode,can not exec cmd");
        return OAL_FAIL;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "wal_set_cal_freq:Featureid0 device not fonud.");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 读取校准数据
 返 回 值  : 错误码
*****************************************************************************/
td_u32 wal_get_cal_data(td_void)
{
    oal_net_device_stru *netdev;

    netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "wal_get_cal_data::sta device not fonud.");
        return OAL_FAIL;
    }

    /***************************************************************************
                                抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MESH
/*****************************************************************************
 功能描述  : 读取Mesh节点信息
*****************************************************************************/
td_u32 wal_get_mesh_dev_info(td_char *dev_name, mac_cfg_mesh_nodeinfo_stru *mesh_node_info)
{
    oal_net_device_stru         *netdev = OSAL_NULL;

    /* 根据dev_name找到dev */
    netdev = oal_get_netdev_by_name(dev_name);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_at_entry::get netdev by name return null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
                                抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

td_u32 uapi_wifi_get_mesh_node_info(mac_cfg_mesh_nodeinfo_stru *mesh_node_info)
{
    td_u32 ret;
    /* 先查询wlan0 */
    if (mesh_node_info == OSAL_NULL) {
        return EXT_ERR_FAILURE;
    }
    ret = wal_get_mesh_dev_info("wlan0", mesh_node_info);
    if (ret != OAL_SUCC) {
        /* 再查询mesh0 */
        ret = wal_get_mesh_dev_info("mesh0", mesh_node_info);
        if (ret != OAL_SUCC) {
            mesh_node_info->node_type = MAC_EXT_MESH_UNSPEC;
        }
    }
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
 功能描述  : 设置驱动上报lwip 重传次数的相关限制参数

 修改历史      :
  1.日    期   : 2019年3月9日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_report_times_limit(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              ret;
    td_u8               times_limit;

    /*
        命令格式
        ccpriv wlan0 set_times_limit X
    */
    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG,
                         "{wal_ccpriv_set_report_times_limit::wal_get_cmd_one_arg return err_code %d!}\r\n", ret);
        return ret;
    }

    times_limit = (td_u8)atoi(ac_name);

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置驱动上报lwip 重传次数的相关限制参数

 修改历史      :
  1.日    期   : 2019年3月9日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_report_cnt_limit(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              ret;
    td_u8               cnt_limit;

    /*
        命令格式
        ccpriv wlan0 set_cnt_limit X
    */
    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG,
                         "{wal_ccpriv_set_report_cnt_limit::wal_get_cmd_one_arg return err_code %d!}\r\n", ret);
        return ret;
    }

    cnt_limit = (td_u8)atoi(ac_name);

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置mesh vap是否为MBR节点

 修改历史      :
  1.日    期   : 2019年4月28日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_en_mbr(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              ret;
    td_u8               mbr;

    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{wal_ccpriv_set_en_mbr:wal_get_cmd_one_arg return err_code %d!}\r\n", ret);
        return ret;
    }
    if (osal_strcmp("0", ac_name) == 0) {
        mbr = OSAL_FALSE;
    } else if (osal_strcmp("1", ac_name) == 0) {
        mbr = OSAL_TRUE;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_set_en_mbr::command param is error!}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置mesh vap的mnid

 修改历史      :
  1.日    期   : 2019年4月28日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_mnid(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              ret;
    td_u8               mnid;

    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG, "{wal_ccpriv_set_mnid::wal_get_cmd_one_arg return err_code %d!}\r\n", ret);
        return ret;
    }

    mnid = (td_u8)atoi(ac_name);

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置beacon priority(调试接口)

 修改历史      :
  1.日    期   : 2019年6月3日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_beacon_priority(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              beacon_prio;

    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_CFG,
                         "{wal_ccpriv_set_beacon_priority::wal_get_cmd_one_arg return err_code %d!}\r\n", ret);
        return ret;
    }

    beacon_prio = (td_u32)atoi(ac_name);
    if (beacon_prio > WLAN_MESH_BEACON_PRIO_MAX) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_set_beacon_priority::invalid beacon prio!!}\r\n");
        return OAL_FAIL;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置Mesh Accept STA标志位(调试接口)

 修改历史      :
  1.日    期   : 2019年6月20日
    修改内容   : 新生成函数
*****************************************************************************/
static td_u32 wal_ccpriv_set_mesh_accept_sta(oal_net_device_stru *netdev, td_char *pc_param)
{
    td_u32              off_set;
    td_char             ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32              ret;
    td_u8               accept_sta;

    /* 获取RSSI门限开关 */
    ret = wal_get_cmd_one_arg(pc_param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_ccpriv_set_mesh_accept_sta::wal_get_cmd_one_arg return err_code [%u]!}\r\n", ret);
        return ret;
    }

    if ((osal_strcmp("0", ac_name) == 0)) {
        accept_sta = 0;
    } else if ((osal_strcmp("1", ac_name) == 0)) {
        accept_sta = 1;
    } else {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_set_mesh_accept_sta::the mod switch command is error [%p]!}\r\n",
                         (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    return OAL_SUCC;
}
#endif

OAL_STATIC td_u32 uapi_ccpriv_set_soft_retry_num(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_u32 offset;
    td_u32 ret;
    td_s8  cmd[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_s32 value = 0;
    mac_cfg_soft_retry_times_stru retry = {0};

    ret = (osal_u32)wal_get_cmd_one_arg_digit(param, cmd, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset, &value);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_soft_retry_num::return err_code [%d]!}", ret);
        return ret;
    }
    retry.data_retry_times = (td_u8)value;

    param = param + offset;
    ret = (osal_u32)wal_get_cmd_one_arg_digit(param, cmd, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset, &value);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_soft_retry_num::return err_code [%d]!}", ret);
        return ret;
    }

    retry.mgmt_retry_times = (td_u8)value;

    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_SOFT_RETRY_TIMES,
        (td_u8 *)&retry, sizeof(retry));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_ccpriv_set_soft_retry_num::send msg failed[%d]!}", ret);
    }
    return ret;
}

static inline oal_bool_enum_uint8 check_unsigned(osal_s8 *value)
{
    osal_u32 i;
    /* 无符号数字符串仅允许纯数字字符 */
    if (osal_strlen((const osal_char *)value) == 0) {
        return OAL_FALSE;
    }
    for (i = 0; i < osal_strlen((const osal_char *)value); i++) {
        if (oal_isdigit(value[i]) == 0) {
            return OAL_FALSE;
        }
    }
    return OAL_TRUE;
}

oal_bool_enum_uint8 check_unsigned_range(osal_s8 *value, osal_u32 min, osal_u32 max)
{
    osal_u32 num;
    if ((osal_strlen((const osal_char *)value) == 0) || (check_unsigned(value) != OAL_TRUE)) {
        return OAL_FALSE;
    }
    num = (osal_u32)oal_atoi((const osal_s8 *)value);
    if ((num < min) || (num > max)) {
        return OAL_FALSE;
    }
    return OAL_TRUE;
}

#ifdef _PRE_WLAN_FEATURE_11V
OAL_STATIC td_u32 uapi_ccpriv_11v_tx_query(oal_net_device_stru *net_dev, td_s8 *pc_param)
{
    td_s32                      l_ret;
    td_u32                      ret;
    td_u32                      off_set;
    td_s8                       name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    hmac_trigger_11v_msg_stru   msg;

    (osal_void)memset_s(&msg, OAL_SIZEOF(hmac_trigger_11v_msg_stru), 0, OAL_SIZEOF(hmac_trigger_11v_msg_stru));
    /* 获取mac地址 */
    ret = wal_get_cmd_one_arg(pc_param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_11v_tx_query:: get arg 1 return err_code [%d]!}", ret);
        return ret;
    }
    pc_param += off_set;
    oal_strtoaddr((const osal_char *)name, (osal_u8 *)&msg.mac_addr_list_0);

    /* 获取mac地址 */
    ret = wal_get_cmd_one_arg(pc_param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_11v_tx_query:: get arg 2 return err_code [%d]!}", ret);
        return ret;
    }
    pc_param += off_set;
    msg.chl_num_list_0 = (osal_u8)atoi((const osal_char *)name);

    /* 获取mac地址 */
    ret = wal_get_cmd_one_arg(pc_param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_11v_tx_query:: get arg 3 return err_code [%d]!}", ret);
        return ret;
    }
    pc_param += off_set;
    oal_strtoaddr((const osal_char *)name, (osal_u8 *)&msg.mac_addr_list_1);

    /* 获取mac地址 */
    ret = wal_get_cmd_one_arg(pc_param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_11v_tx_query:: get arg 4 return err_code [%d]!}", ret);
        return ret;
    }
    pc_param += off_set;
    msg.chl_num_list_1 = (osal_u8)atoi((const osal_char *)name);

    /***************************************************************************
                                抛事件到wal层处理
    ***************************************************************************/
    l_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_11V_TX_QUERY,
        (osal_u8 *)&msg, OAL_SIZEOF(msg));
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_ccpriv_11v_tx_query::return err code [%d]!}", l_ret);
        return (osal_u32)l_ret;
    }

    return OAL_SUCC;
}
#endif // _PRE_WLAN_FEATURE_11V

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/*****************************************************************************
功能描述: 提供WOW_OFFLOAD模块清空收发包统计的接口配置
创建时间: 2022年9月8日
*****************************************************************************/
osal_u32 uapi_ccpriv_clear_wow_offload_info(oal_net_device_stru *net_dev, osal_s8 *pc_param)
{
    osal_u32 ul_ret;
    osal_s32 l_ret;
    osal_u32 off_set;
    oal_switch_enum_uint8 delete_switch;
    osal_s8 ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    ul_ret = wal_get_cmd_one_arg(pc_param, ac_name, OAL_SIZEOF(ac_name), &off_set);
    if (ul_ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_CFG, "uapi_ccpriv_clear_wow_offload_info: %d", ul_ret);
        return ul_ret;
    }
    delete_switch = (oal_switch_enum_uint8)oal_atoi((const osal_s8 *)ac_name);
    l_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_CLEAR_WOW_OFFLOAD_INFO,
        (osal_u8 *)&delete_switch, OAL_SIZEOF(delete_switch));
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_ccpriv_clear_wow_offload_info:return err code[%d]!}", l_ret);
    }

    return l_ret;
}

/*****************************************************************************
 功能描述  : DHCP offload DFX打印
*****************************************************************************/
td_u32 uapi_ccpriv_show_dhcpoffload_info(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_s32 ret;
    td_u32 off_set;
    td_s8 cmd_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_u8 dhcp_offload_type;

    ret = wal_get_cmd_one_arg(param, cmd_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_show_dhcpoffload_info: return err_code [%d]!}", ret);
        return ret;
    }

    dhcp_offload_type = (td_u8)oal_atoi((const osal_s8 *)cmd_name);
    ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SHOW_DHCPOFFLOAD_INFO,
        (td_u8 *)&dhcp_offload_type, OAL_SIZEOF(dhcp_offload_type));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_ccpriv_show_dhcpoffload_info::return err code [%d]!}", ret);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : DHCP offload 设置ip地址命, set_dhcpoffload_info 0-del/1-add [ipv4_addr]
*****************************************************************************/
td_u32 uapi_ccpriv_set_dhcpoffload_info(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_u32 off_set;
    td_s8  cmd_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_s32 ret;
    td_u32 ipv4_addr = 0;
    td_u8 oper;
    mac_ip_addr_config_stru offload_cfg;
    (td_void)memset_s(&offload_cfg, OAL_SIZEOF(offload_cfg), 0, OAL_SIZEOF(offload_cfg));

    ret = wal_get_cmd_one_arg(param, cmd_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_dhcpoffload_info: return err_code [%d]!}", ret);
        return ret;
    }
    oper = (td_u8)oal_atoi((const osal_s8 *)cmd_name);

    param = param + off_set;
    ret = wal_get_cmd_one_arg(param, cmd_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_dhcpoffload_info: return err_code [%d]!}", ret);
        return ret;
    }
    ret = wal_ccpriv_parse_ipv4(cmd_name, &ipv4_addr);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_dhcpoffload_info: return err_code [%d]!}", ret);
        return ret;
    }
    offload_cfg.type = MAC_CONFIG_IPV4;
    offload_cfg.oper = oper;
    offload_cfg.ip.ipv4 = ipv4_addr;
    ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_DHCPOFFLOAD_INFO,
        (td_u8 *)&offload_cfg, OAL_SIZEOF(offload_cfg));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_ccpriv_show_dhcpoffload_info::return err code [%d]!}", ret);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_11D
/*****************************************************************************
 功能描述  : 获取当前管制域内的信道列表,需要带一个任意参数,如get_channel_list,0
*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_get_current_channel_list(oal_net_device_stru *netdev, td_s8 *param)
{
    td_s32  ret;
    frw_msg cfg_info;
    mac_vendor_cmd_channel_list_stru fill_msg;
    mac_vendor_cmd_channel_list_stru *rsp_info = OAL_PTR_NULL;
    td_u8 index;
    unref_param(param);
    if (netdev == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_UNLIKELY(netdev->ml_priv == OAL_PTR_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_get_current_channel_list::NET_DEV_PRIV is NULL.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    memset_s(&cfg_info, sizeof(cfg_info), 0, sizeof(cfg_info));
    cfg_info.rsp_buf_len = sizeof(fill_msg);
    cfg_info.rsp = (osal_u8 *)&fill_msg;

    ret = send_sync_cfg_to_host(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_VENDOR_CMD_GET_CHANNEL_LIST, &cfg_info);
    if ((ret != OAL_SUCC) || (cfg_info.rsp == OAL_PTR_NULL)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_get_current_channel_list:: return err code %d!}", ret);
        return (osal_u32)ret;
    }

    rsp_info = (mac_vendor_cmd_channel_list_stru *)(cfg_info.rsp);
    oam_warning_log0(0, OAM_SF_ANY, "2G channel list");
    for (index = 0; index < rsp_info->channel_num_2g; index++) {
        oam_warning_log1(0, OAM_SF_ANY, "%d", rsp_info->channel_list_2g[index]);
    }
    oam_warning_log0(0, OAM_SF_ANY, "5G channel list");
    for (index = 0; index < rsp_info->channel_num_5g; index++) {
        oam_warning_log1(0, OAM_SF_ANY, "%d", rsp_info->channel_list_5g[index]);
    }
    return OAL_SUCC;
}

/*****************************************************************************
读取管制域最大发送功率 单位dBm
*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_get_regdomain_pwr_priv(oal_net_device_stru *netdev, td_s8 *param)
{
    td_u32 ret;
    unref_param(param);
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev),
        WLAN_MSG_W2H_CFG_GET_REGDOMAIN_PWR, OAL_PTR_NULL, 0);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_get_regdomain_pwr_priv:: alloc_cfg_event failed!}");
        return ret;
    }
    return OAL_SUCC;
}

#endif

#ifdef _PRE_WLAN_FEATURE_DFX_CHR
td_u32 uapi_ccpriv_chr_test(oal_net_device_stru *netdev, td_s8 *param)
{
    td_u32 off_set = 0;
    td_u32 ret;
    td_s32 ret1;
    td_u8 err_code;
    hmac_vap_stru *hmac_vap = OSAL_NULL;

    hmac_vap = (hmac_vap_stru *)netdev->ml_priv;
    if (hmac_vap == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_chr_test::mac_vap is null ptr}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ret = wal_cmd_get_u8_with_check_max(&param, CHR_ERR_CODE_BUTT - 1, &err_code);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG, "{uapi_ccpriv_chr_test::switch_value invalid.}");
        return ret;
    }
    ret1 = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CHR_ERR_CODE,
        (osal_u8 *)&err_code, OAL_SIZEOF(err_code));
    if (OAL_UNLIKELY(ret1 != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_chr_test::return err code[%d]!}", ret1);
        return (osal_u32)ret1;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_DFT_STAT
/*****************************************************************************
 函 数 名  : uapi_ccpriv_get_hw_stat
 功能描述  : 查询hw实时状况信息
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_get_hw_stat(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_u32 ret;
    td_u8 vap_id = wal_util_get_vap_id(net_dev);
    unref_param(param);

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(vap_id, WLAN_MSG_W2H_CFG_GET_HW_STAT, OSAL_NULL, 0);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "vap_id[%d] {uapi_ccpriv_get_hw_stat::return err code [%d]!}",
                         vap_id, ret);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_hw_stat_enable
 功能描述  : 使能是否查询hw收发包信息
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
OAL_STATIC td_u32 uapi_ccpriv_hw_stat_enable(oal_net_device_stru *net_dev, td_s8 *param)
{
    td_u32 ret;
    td_u8 enable_switch = 0;
    unref_param(param);

    /* 获取帧方向 */
    ret = wal_cmd_get_u8_with_check_max(&param, 1, &enable_switch);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG, "{uapi_ccpriv_hw_stat_enable::switch_value invalid.}");
        return ret;
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_HW_STAT_ENABLE,
        (td_u8 *)&enable_switch, OAL_SIZEOF(enable_switch));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_hw_stat_enable::return err code[%d]!}", ret);
        return (td_u32)ret;
    }

    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_get_vap_aggre_info(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 ret;
    osal_u8 vap_id = wal_util_get_vap_id(net_dev);
    unref_param(param);
    ret = hmac_query_tx_vap_aggre_info(vap_id);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "vap_id[%d] {uapi_ccpriv_get_vap_aggre_info::return err code [%d]!}",
                         vap_id, ret);
    }

    return (osal_u32)ret;
}

osal_u32 *g_wifi_external_record_enable = OSAL_NULL;
osal_u16 g_record_max_num;
osal_void wal_record_wifi_external_log(const osal_u32 addr)
{
    static osal_u16 line_index = 0;

    if (g_wifi_external_record_enable == OSAL_NULL) {
        return;
    }
    if (line_index == g_record_max_num) {
        line_index = 0;
    }
    line_index %= g_record_max_num;
    g_wifi_external_record_enable[line_index] = addr;
    wifi_printf("wal_record_wifi_external_log, addr:0x%08x.\r\n", addr);
    ++line_index;
    return;
}

OAL_STATIC osal_u32 wal_set_external_record_enable(osal_u8 enable_value, osal_u16 record_max_num)
{
    if (enable_value == 0) {
        if (g_wifi_external_record_enable == OSAL_NULL) {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_set_external_record_enable::before not malloc memory.}");
        } else {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_set_external_record_enable::free the memory.}");
            oal_mem_free(g_wifi_external_record_enable, OAL_TRUE);
            g_wifi_external_record_enable = OSAL_NULL;
        }
    } else {
        if (g_wifi_external_record_enable != OSAL_NULL) {
            oal_mem_free(g_wifi_external_record_enable, OAL_TRUE);
            g_wifi_external_record_enable = OSAL_NULL;
        }

        g_record_max_num = record_max_num;
        g_wifi_external_record_enable = (osal_u32 *)oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL,
            (sizeof(osal_u32) * record_max_num), OAL_TRUE);
        if (g_wifi_external_record_enable == OSAL_NULL) {
            oam_warning_log0(0, OAM_SF_CFG, "{wal_set_external_record_enable malloc memory fail}");
            return OAL_FAIL;
        }
        (osal_void)memset_s((osal_u8 *)g_wifi_external_record_enable,
            (sizeof(osal_u32) * record_max_num), 0, (sizeof(osal_u32) * record_max_num));
        oam_warning_log0(0, OAM_SF_CFG, "{wal_set_external_record_enable::malloc the memory.}");
    }

    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_external_record_enable(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 ret, off_set, record_max_num;
    osal_u8 enable_switch = 0;
    osal_s8 cmd_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    unref_param(net_dev);
    /* 获取帧方向 */
    ret = wal_cmd_get_u8_with_check_max(&param, WAL_EXTERNAL_RECORD_ENABLE_ON, &enable_switch);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG, "{uapi_ccpriv_external_record_enable::enable_switch invalid.}");
        return ret;
    }

    /* 获取最大能存多少条记录 */
    ret = wal_get_cmd_one_arg(param, cmd_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_thruput_test::get type failed [%d]!}", ret);
        return ret;
    }
    param += off_set;
    record_max_num = (osal_u16)oal_atoi((const osal_s8 *)cmd_name);
    wal_set_external_record_enable(enable_switch, (osal_u16)record_max_num);
    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_get_external_record(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u16 i;
    unref_param(net_dev);
    unref_param(param);
    if (g_wifi_external_record_enable == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_CFG,
            "{uapi_ccpriv_get_external_record::external_record_enable is off}");
        return OAL_FAIL;
    }
    wifi_printf("==========external record info max_num:%d==============\r\n", g_record_max_num);
    for (i = 0; i < g_record_max_num; ++i) {
        if (g_wifi_external_record_enable[i] == 0) {
            continue;
        }
        wifi_printf("the : %d record function addr is 0x%08x\r\n", i, g_wifi_external_record_enable[i]);
    }
    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_DFT_STAT
OAL_STATIC osal_u32 uapi_ccpriv_get_vap_packet_info(oal_net_device_stru *net_dev, osal_s8 *param)
{
    oam_stat_info_stru *oam_stat;
    oam_vap_stat_info_stru *oam_vap_stat;
    osal_u32 rx_errors, rx_dropped, tx_errors, tx_dropped;
    osal_u8 vap_id = wal_util_get_vap_id(net_dev);

    unref_param(param);
    oam_stat = OAM_STAT_GET_STAT_ALL();
    oam_vap_stat = &(oam_stat->vap_stat_info[vap_id]);
    rx_errors = oam_vap_stat->rx_defrag_process_dropped;
    rx_dropped = oam_vap_stat->rx_no_buff_dropped + oam_vap_stat->rx_da_check_dropped;
    tx_errors = oam_vap_stat->tx_abnormal_msdu_dropped + oam_vap_stat->tx_security_check_faild;
    tx_dropped = 0;

    wifi_printf("============Packets and bytes stats=============\r\n");
    wifi_printf("Rx packets:%u errors:%u dropped:%u\r\n", oam_vap_stat->rx_pkt_to_lan, rx_errors, rx_dropped);
    wifi_printf("Tx packets:%u errors:%u dropped:%u\r\n", oam_vap_stat->tx_pkt_num_from_lan, tx_errors, tx_dropped);
    wifi_printf("Rx bytes:%u  Tx bytes:%u\r\n", oam_vap_stat->rx_bytes_to_lan, oam_vap_stat->tx_bytes_from_lan);
    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_get_vap_stat_new(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 l_ret;
    dft_vap_statis_command_stru vap_statis_command = {0};
    osal_u8 vap_id = wal_util_get_vap_id(net_dev);
    unref_param(param);
    vap_statis_command.vap_id = vap_id;

    l_ret = hmac_get_vap_stat_new(&vap_statis_command);
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log2(0, OAM_SF_ANY, "vap_id[%d] {uapi_ccpriv_get_vap_stat_new::return err code [%d]!}",
                         vap_id, l_ret);
        return (osal_u32)l_ret;
    }
    /* 打印出vap的收发包状态 */
    hmac_vap_stat_tx_display(vap_id);
    hmac_vap_stat_rx_display(vap_id);
    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_vap_stat_enable(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 ret;
    dft_vap_statis_command_stru vap_statis_command = {0};

    /* 获取使能参数 */
    ret = wal_cmd_get_u8_with_check_max(&param, 1, &vap_statis_command.enable);
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG, "{wal_ccpriv_user_rate_info_enable::switch_value invalid.}");
        return ret;
    }

    vap_statis_command.vap_id = wal_util_get_vap_id(net_dev);

    hmac_config_vap_stat_enable(&vap_statis_command);

    return OAL_SUCC;
}

#endif

OAL_STATIC osal_u32 uapi_ccpriv_reg_info(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 ret;

    osal_u32 param_ret;
    osal_s8 *param_copy = param;
    osal_u32 off_set;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_char *end = OSAL_NULL;
    uintptr_t start_addr;
    osal_u8 reg32 = OSAL_TRUE;
    param_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
    if (param_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_info::get bit width return err_code[%d]!}", param_ret);
        return param_ret;
    }
    param_copy = param_copy + off_set;

    if (osal_strcmp((const osal_char *)name, "16") == 0) {
        reg32 = OSAL_FALSE;
    }

    param_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
    if (param_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_info::get reg type return err_code[%d]!}", param_ret);
        return param_ret;
    }
    param_copy = param_copy + off_set;

    /* 读host寄存器(内存) */
    if (osal_strcmp((const osal_char *)name, "host") == 0) {
        param_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
        if (param_ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_info::get reg start addr return err[%d]!}", param_ret);
            return param_ret;
        }
        param_copy = param_copy + off_set;
        start_addr = (osal_u32)osal_strtol((const osal_char *)name, (osal_char **)&end, 16); /* 16表示将字符串转化成16进制数 */

        param_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
        if (param_ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_info::get reg number return err_code[%d]!}", param_ret);
            return param_ret;
        }
        /* 10表示将字符串转化成10进制数 */
        wal_mem_read(reg32, start_addr, (osal_u32)osal_strtol((const osal_char *)name, &end, 10));

        return OAL_SUCC;
    }
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_REG_INFO,
        (osal_u8 *)param, (osal_u32)(osal_strlen((const osal_char *)param) + 1));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_info::return err code [%d]!}", ret);
        return (osal_u32)ret;
    }

    return OAL_SUCC;
}

OAL_STATIC osal_void wal_mem_write(osal_u8 reg32, uintptr_t start_addr, osal_u32 value)
{
    if (reg32 != 0) {
        OAL_REG_WRITE32(start_addr, value);
    } else {
        OAL_REG_WRITE16(start_addr, (osal_u16)value);
    }
}

OAL_STATIC osal_u32 uapi_ccpriv_reg_write(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 ret;

    osal_u32 arg_ret;
    osal_s8 *param_copy = param;
    osal_u32 off_set;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_char *end = OSAL_NULL;
    uintptr_t start_addr;
    osal_u8 reg32 = OSAL_TRUE;

    arg_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
    if (arg_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_write::get bit width return err_code[%d]!}", arg_ret);
        return arg_ret;
    }
    param_copy = param_copy + off_set;

    if (osal_strcmp((const osal_char *)name, "16") == 0) {
        reg32 = OSAL_FALSE;
    }

    arg_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
    if (arg_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_write::get reg type return err_code[%d]!}", arg_ret);
        return arg_ret;
    }
    param_copy = param_copy + off_set;

    /* 读host寄存器(内存) */
    if (osal_strcmp((const osal_char *)name, "host") == 0) {
        arg_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
        if (arg_ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_write::get reg start addr return err[%d]!}", arg_ret);
            return arg_ret;
        }
        param_copy = param_copy + off_set;
        start_addr = (osal_u32)osal_strtol((const osal_char *)name, &end, 16); /* 16表示将字符串转化成16进制数 */

        arg_ret = wal_get_cmd_one_arg(param_copy, name, OAL_SIZEOF(name), &off_set);
        if (arg_ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_write::get set value return err_code[%d]!}", arg_ret);
            return arg_ret;
        }
        /* 16表示将字符串转化成16进制数 */
        wal_mem_write(reg32, start_addr, (osal_u32)osal_strtol((const osal_char *)name, &end, 16));

        return OAL_SUCC;
    }

    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_REG_WRITE,
        (osal_u8 *)param, osal_strlen((const osal_char *)param) + 1);
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_reg_write::return err code [%d]!}", ret);
        return (osal_u32)ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_BSLE_GATEWAY
OAL_STATIC osal_u32 uapi_ccpriv_bsle_listen(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 ul_ret;
    osal_u32 enable;

    ul_ret = wal_cmd_get_digit_with_check_max(&param, 1, &enable);
    if (ul_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_bsle_listen::wal_cmd_get_digit_with_check_max err [%u]!}",
            ul_ret);
        return ul_ret;
    }

    ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_C_CFG_SET_BSLE_LISTEN_ENABLE,
        (osal_u8 *)&enable, OAL_SIZEOF(enable));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_bsle_listen::wal_sync_send2device_no_rsp err [%d]!}", ret);
        return (osal_u32)ret;
    }
    return OAL_SUCC;
}
#endif

OAL_STATIC osal_u32 uapi_ccpriv_aifsn_cfg(oal_net_device_stru *net_dev, osal_s8 *param)
{
    mac_edca_cfg_stru edca_cfg;
    osal_u32 ret;
    osal_s32 l_ret;
    osal_u32 val;

    memset_s(&edca_cfg, OAL_SIZEOF(edca_cfg), 0, OAL_SIZEOF(edca_cfg));

    /* 获取配置开关 */
    ret = wal_cmd_get_digit_with_check_max(&param, 1, &val);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa switch fail, return err_code[%u]!}", ret);
        return ret;
    }
    edca_cfg.switch_code = val;

    /* 获取ac */
    ret = wal_cmd_get_digit_with_check_max(&param, 1, &val);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa ac fail, return err_code[%u]!}", ret);
        return ret;
    }
    edca_cfg.ac = (osal_u8)val;

    if (edca_cfg.switch_code == OAL_TRUE) {
        /* 获取配置值 */
        ret = wal_cmd_get_digit_with_check_max(&param, 255, &val);  // 最大值255
        if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa val fail, return err_code[%u]!}", ret);
            return ret;
        }
        edca_cfg.val = (osal_u16)val;
    }
    l_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_WFA_CFG_AIFSN,
        (osal_u8 *)&edca_cfg, OAL_SIZEOF(edca_cfg));
    if (l_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::return err code[%d]!}", l_ret);
        return (osal_u32)l_ret;
    }
    return OAL_SUCC;
}

OAL_STATIC osal_u32  uapi_ccpriv_cw_cfg(oal_net_device_stru *net_dev, osal_s8 *param)
{
    mac_edca_cfg_stru edca_cfg;
    osal_s32 ret;
    osal_u32 ul_ret;
    osal_u32 val;

    memset_s(&edca_cfg, OAL_SIZEOF(edca_cfg), 0, OAL_SIZEOF(edca_cfg));

    /* 获取配置开关 */
    ul_ret = wal_cmd_get_digit_with_check_max(&param, 1, &val);
    if (ul_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa switch fail, return err_code[%u]!}", ul_ret);
        return ul_ret;
    }
    edca_cfg.switch_code = val;

    /* 获取ac */
    ul_ret = wal_cmd_get_digit_with_check_max(&param, 1, &val);
    if (ul_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa ac fail, return err_code[%u]!}", ul_ret);
        return ul_ret;
    }
    edca_cfg.ac = (osal_u8)val;

    if (edca_cfg.switch_code == OAL_TRUE) {
        /* 获取配置值 */
        ul_ret = wal_cmd_get_digit_with_check_max(&param, 255, &val);   // 最大值255
        if (ul_ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::get wfa val fail, return err_code[%u]!}", ul_ret);
            return ul_ret;
        }
        edca_cfg.val = (osal_u16)val;
    }
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_WFA_CFG_CW,
        (osal_u8 *)&edca_cfg, OAL_SIZEOF(edca_cfg));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_aifsn_cfg::return err code[%d]!}", ret);
        return (osal_u32)ret;
    }
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_SNIFFER
OAL_STATIC osal_void wal_ccpriv_wifi_sniffer_on(const oal_net_device_stru *netdev)
{
    OAL_STATIC osal_u8 proc_create_flag = 0;
    osal_u32 ul_ret;

    if (OAL_UNLIKELY(netdev == OAL_PTR_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_on::netdev is null}");
        return;
    }

    if (proc_create_flag != 1) {
        oam_warning_log4(0, OAM_SF_ANY,
            "wal_ccpriv_wifi_sniffer_on:: sniffer_save_file on, [cur,max,num,len]=[%d %d %d %d]",
            g_sniffer_st.cur_file, g_sniffer_st.data_packet_max_len, g_sniffer_st.max_file_num,
            g_sniffer_st.max_file_len);
        ul_ret = wal_wifi_sniffer_proc_file_sec();
        if (ul_ret == OAL_SUCC) {
            proc_create_flag = 1;
        }
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_on::proc files already created}");
    }

    proc_handle_command_sniffer_enable(1);
}

OAL_STATIC osal_void wal_ccpriv_wifi_sniffer_off(oal_net_device_stru *netdev)
{
    proc_handle_command_sniffer_enable(0);
    proc_handle_command_save_file();
    oam_warning_log0(0, OAM_SF_ANY, "wal_ccpriv_wifi_sniffer_off:: sniffer_save_file off");
}

OAL_STATIC osal_u32 wal_ccpriv_wifi_sniffer_config(const osal_s8 *param)
{
    osal_u32 ret;
    osal_u32 off_set;
    osal_s32  ret_l;
    osal_s32  file_num;
    osal_s32  file_size;
    osal_s32  proc_file_size;
    osal_s8   name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_config::get file num fail, return [%d]!}", ret);
        return ret;
    }
    param += off_set;
    file_num = oal_atoi((const osal_s8 *)name);
    if (file_num < PCAP_FILE_NUM_MIN || file_num > PCAP_FILE_NUM_MAX) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_config::invalid file_num[%d]}", file_num);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    ret = wal_get_cmd_one_arg(param, name, sizeof(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_config::get file size fail, return [%d]!}", ret);
        return ret;
    }

    file_size = oal_atoi((const osal_s8 *)name);
    if (file_size < (PCAP_FILE_LEN_MIN / (1024 * 1024)) || /* 1024 * 1024 Mb */
        file_size > (PCAP_FILE_LEN_MAX / (1024 * 1024))) { /* 1024 * 1024 Mb */
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_wifi_sniffer_config::invalid file_size[%d]}", file_size);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    proc_file_size = file_size * 1024 * 1024; /* file_size's unit is Mbit(1024 * 1024) */
    oam_warning_log2(0, OAM_SF_ANY,
        "wal_ccpriv_wifi_sniffer_config:: sniffer_save_file config, file_num = %d, file_len = %d(Mb)",
        file_num, file_size);
    ret_l = proc_sniffer_reset_file_num_len(file_num, proc_file_size);
    if (ret_l) {
        oam_warning_log1(0, OAM_SF_ANY,
            "{wal_ccpriv_wifi_sniffer_config::proc_sniffer_reset_file_num_len, return [%d]!}", ret_l);
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    return OAL_SUCC;
}

OAL_STATIC osal_u32 uapi_ccpriv_wifi_sniffer(oal_net_device_stru *netdev, osal_s8 *param)
{
    osal_u8   sniffer_switch;
    osal_u32  ul_ret;
    osal_u32  off_set;
    osal_u8   ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    ul_ret = wal_get_cmd_one_arg(param, ac_name, OAL_SIZEOF(ac_name), &off_set);
    if (ul_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_wifi_sniffer::get arg return err_code %d!}", ul_ret);
        return ul_ret;
    }
    param += off_set;

    sniffer_switch = (osal_u8)oal_atoi((const osal_s8 *)ac_name);
    if (sniffer_switch == 0) {           /* sniffer_save_file 0 */
        wal_ccpriv_wifi_sniffer_off(netdev);
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_wifi_sniffer::sniffer_save_file off}");
    } else if (sniffer_switch == 1) {    /* sniffer_save_file 1 [FILE_NUM] [FILE_SIZE] */
        ul_ret = wal_ccpriv_wifi_sniffer_config(param);
        if (ul_ret != OAL_SUCC) {
            oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_wifi_sniffer::config FAIL}");
            return ul_ret;
        }
        wal_ccpriv_wifi_sniffer_on(netdev);
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_wifi_sniffer::sniffer_save_file on}");
    } else {
        oam_error_log0(0, OAM_SF_ANY, "{uapi_ccpriv_wifi_sniffer::invalid cmd}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_DFR_STAT
OAL_STATIC osal_u32 wal_ccpriv_destroy_all_vap_etc(osal_void)
{
    osal_s32 ret;
    osal_u8 vap_id;
    oal_net_device_stru *net_dev = OAL_PTR_NULL;
    osal_s8 param[10] = {0};

    /* 删除业务vap，双芯片id从2开始，增加编译宏表示板级业务vap起始id 后续业务vap的处理，采用此宏 */
    for (vap_id = oal_board_get_service_vap_start_id(); vap_id < WLAN_VAP_SUPPORT_MAX_NUM_LIMIT; vap_id++) {
        net_dev = hmac_vap_get_net_device_etc(vap_id);
        if (net_dev != OAL_PTR_NULL) {
            oal_net_close_dev(net_dev);
            ret = uapi_ccpriv_del_vap_etc(net_dev, (osal_s8 *)param);
            if (ret != OAL_SUCC) {
                oam_error_log2(0, OAM_SF_CFG, "{wal_ccpriv_destroy_all_vap_etc vapid [%u]:: err_code [%d]!}",
                    vap_id, ret);
                return (osal_u32)ret;
            }
            frw_event_process_all_event_etc(0);
        }
    }

    return ret;
}

OAL_STATIC osal_u32 uapi_ccpriv_set_queue_reset(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 offset;
    mac_cfg_queue_reset_stru *queue_reset_param = OAL_PTR_NULL;
    osal_s8 arg[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_s32 value = 0;
    mac_cfg_queue_reset_stru queue_reset;
    (osal_void)memset_s(&queue_reset, OAL_SIZEOF(queue_reset), 0, OAL_SIZEOF(queue_reset));
    queue_reset_param = &queue_reset;

    ret = wal_get_cmd_one_arg_digit((osal_s8 *)param, arg, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset, &value);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_CFG, "{uapi_ccpriv_set_queue_reset err_code [%d]!}", ret);
        return (osal_u32)ret;
    }

    /* 1.获取复位类型 0:FRW, 1:TID, 2:Device */
    if ((value < 0) || (value > 2)) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_ccpriv_set_queue_reset::param:index error [%d].}", value);
        return OAL_FAIL;
    }
    param += offset;
    queue_reset_param->index = (osal_u8)value;

    /* 2.当index为2时表示为Deive复位 */
    if (queue_reset_param->index == 2) {
        oam_warning_log0(0, OAM_SF_CFG, "{uapi_ccpriv_set_queue_reset::device reset.}");
        wal_ccpriv_destroy_all_vap_etc();
        return OAL_SUCC;
    }

    /* 3.当index为0时获取的是 FRW_EVENT_TYPE， 当index为1时获取的是user_id,WLAN_USER_MAX_USER_LIMIT */
    ret = wal_get_cmd_one_arg_digit((osal_s8 *)param, arg, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset, &value);
    if ((ret != OAL_SUCC) || (value < 0)) {
        oam_error_log1(0, OAM_SF_CFG,
                       "{uapi_ccpriv_set_queue_reset::param:mode error [%d]!}", value);
        return OAL_FAIL;
    }
    param = param + offset;
    queue_reset_param->mode = (osal_u8)value;
    ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_QUEUE_RESET,
        (osal_u8 *)&queue_reset, OAL_SIZEOF(queue_reset));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_queue_reset::return err code [%d]!}", ret);
        return (osal_u32)ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_SMPS
/*****************************************************************************
 函 数 名  : uapi_ccpriv_set_smps_mode
 功能描述  : 设置device的SMPS能力
*****************************************************************************/
static uint32_t  uapi_ccpriv_set_smps_mode(oal_net_device_stru *net_dev, int8_t *param)
{
    uint32_t                   smps_mode;
    uint32_t                   off_set;
    int8_t                     name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    int32_t                    l_ret;
    uint32_t                   ret;

    /* 此处将解析出"1"、"2"或"3"存入name */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SMPS,
                         "{uapi_ccpriv_set_smps_mode::wal_get_cmd_one_arg return err_code[%d]}", ret);
        return ret;
    }

    /* 针对解析出的不同命令，设置不同SMPS模式 */
    if ((osal_strcmp("1", (const osal_char *)name)) == 0) {
        smps_mode = 1;
    } else if ((osal_strcmp("2", (const osal_char *)name)) == 0) {
        smps_mode = 2;    /* 字符2转为数字2 */
    } else if ((osal_strcmp("3", (const osal_char *)name)) == 0) {
        smps_mode = 3;    /* 字符3转为数字3 */
    } else {
        oam_error_log1(0, OAM_SF_SMPS, "{uapi_ccpriv_set_smps_mode::error cmd[%d],input 1/2/3!}", name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    l_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SMPS_MODE,
        (uint8_t *)&smps_mode, OAL_SIZEOF(smps_mode));
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SMPS, "{uapi_ccpriv_set_smps_mode::return err code [%d]!}", l_ret);
        return (uint32_t)l_ret;
    }
    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_FEATURE_PK_MODE
/*****************************************************************************
 函 数 名  : uapi_ccpriv_pk_mode_th_reset
 功能描述  : ccpriv命令 设置pk模式开关阈值
****************************************************************************/
static uint32_t  uapi_ccpriv_pk_mode_th_reset(oal_net_device_stru *net_dev, int8_t *param)
{
    mac_cfg_pk_mode_th_reset_stru    pk_mode_param;
    uint32_t                   ret;
    osal_u32                   val;

    /* 获取低门限pk阈值存入args */
    ret = wal_cmd_get_digit_with_check_max(&param, 255, &val);  // 最大值255
    if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_pk_mode_th_reset::return err_code[%u]!}", ret);
            return ret;
        }
    pk_mode_param.low_th = val;
    /* 获取高门限pk阈值存入args */
    ret = wal_cmd_get_digit_with_check_max(&param, 255, &val);  // 最大值255
    if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_pk_mode_th_reset::return err_code[%u]!}", ret);
            return ret;
        }
    pk_mode_param.high_th = val;

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_PK_MODE_TH,
        (uint8_t *)&pk_mode_param, OAL_SIZEOF(mac_cfg_pk_mode_th_reset_stru));
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_pk_mode_th_reset:: return err code %d!}", ret);
        return ret;
    }
    return OAL_SUCC;
}
#endif
/*****************************************************************************
 函 数 名  : uapi_ccpriv_remove_app_ie
 功能描述  : ccpriv命令 移除或者恢复某个IE
****************************************************************************/
static uint32_t  uapi_ccpriv_remove_app_ie(oal_net_device_stru *net_dev, int8_t *param)
{
    cfg_ie_info_stru           ie_info;
    int32_t                    l_ret;
    uint32_t                   ret;
    uint32_t                   off_set;
    int8_t                     name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    /* 获取移除的类型，类型暂只支持0/1 移除或者恢复 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_remove_app_ie::wal_get_cmd_one_arg type return err_code %d!}",
                         ret);
        return ret;
    }
    ie_info.ie_type = (uint8_t)oal_atoi((const int8_t *)name);

    /* 获取操作的EID */
    param += off_set;
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_remove_app_ie::wal_get_cmd_one_arg eid return err_code %d!}",
                         ret);
        return ret;
    }
    ie_info.ie_id = (uint8_t)oal_atoi((const int8_t *)name);
    l_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_REMOVE_APP_IE,
        (uint8_t *)&ie_info, OAL_SIZEOF(ie_info));
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_CFG, "{uapi_ccpriv_remove_app_ie::return err code [%d]!}", l_ret);
        return (uint32_t)l_ret;
    }
    return OAL_SUCC;
}

static uint32_t uapi_ccpriv_set_extend_ie_ext(oal_net_device_stru *net_dev, int8_t *param,
    app_ie_type_uint8 ie_index, uint8_t ie_bitmap, uint32_t ie_len)
{
    uint32_t off_set = 0;
    uint32_t ret;
    int32_t  rst;
    int8_t   args[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    oal_app_ie_stru extend_ie = {0};
    int32_t temp_ie;
    uint32_t i;

    extend_ie.app_ie_type = ie_index;
    extend_ie.ie_type_bitmap = ie_bitmap;
    extend_ie.ie_len = ie_len;

    for (i = 0; i < ie_len; i++) {
        ret = (osal_u32)wal_get_cmd_one_arg_digit(param, args, OAL_SIZEOF(args), &off_set, &temp_ie);
        if (ret != OAL_SUCC) {
            oam_error_log1(0, OAM_SF_ANY,
                           "{uapi_ccpriv_set_extend_ie_ext::rwal_get_cmd_one_arg_digit err [%d]!}", ret);
            return (uint32_t)ret;
        }
        if ((temp_ie > 0xff) || (temp_ie < 0)) {
            oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie_ext::ie[%d] out of range!!}", temp_ie);
            return OAL_ERR_CODE_CONFIG_UNSUPPORT;
        }
        extend_ie.ie[i] = (osal_u8)temp_ie;
        param = param + off_set;
    }

    /***************************************************************************
        抛事件到wal层处理
    ***************************************************************************/
    rst = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_VENDOR_IE,
        (uint8_t *)&extend_ie, OAL_SIZEOF(oal_app_ie_stru));
    if (rst != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie_ext:: return err code %d!}", rst);
        return (osal_u32)rst;
    }
    return OAL_SUCC;
}

static uint32_t  uapi_ccpriv_set_extend_ie(oal_net_device_stru *net_dev, int8_t *param)
{
    app_ie_type_uint8 ie_index;
    uint8_t ie_bitmap;
    uint32_t ie_len;
    uint32_t off_set;
    uint32_t ret;
    int8_t args[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_bool flag;

    /* ccpriv “wlan0 set_extended_ie 1~4 0~7 ie_total_len IE_tag(221) IE_LEN(ie_total_len - 2) IE IE …… IE” */
    /* 获取第一个参数，支持扩展IE的index下标，1~4 */
    ret = wal_get_cmd_one_arg(param, args, OAL_SIZEOF(args), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::parse arg failed [%d]!}", ret);
        return ret;
    }
    if (args[0] < '1' || args[0] > '4') {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::extend ie_index [%c] is out of range!}", args[0]);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    ie_index = (app_ie_type_uint8)oal_atoi((const int8_t *)args) + OAL_APP_REASSOC_REQ_IE;
    param = param + off_set;

    /* 获取第二个参数，支持扩展IE的类型 bitmap，1(b) beacon 10(b) probe req 100(b) probe rsp 转换为10进制 0~7(dec) */
    ret = wal_get_cmd_one_arg(param, args, OAL_SIZEOF(args), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::parse arg failed [%d]!}", ret);
        return ret;
    }
    if (args[0] > '7' || args[0] < '0') {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::extend ie_bitmap [%c] is out of range!}", args[0]);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    ie_bitmap = (uint8_t)oal_atoi((const int8_t *)args);
    param = param + off_set;

    /* 获取第三个参数，IE字段的长度 */
    ret = wal_get_cmd_one_arg(param, args, OAL_SIZEOF(args), &off_set);
    flag = oal_isdigit(args[0]);
    if (ret != OAL_SUCC || (flag == 0)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::parse arg failed [%d]!}", ret);
        return ret;
    }
    ie_len = (uint32_t)oal_atoi((const int8_t *)args);
    if (ie_len > WLAN_WPS_IE_MAX_SIZE || ie_len == 0) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_extend_ie::extend ie_len [%d] is out of range!}", ie_len);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    param = param + off_set;
    /* 获取完整IE payload，并发送消息 */
    ret = uapi_ccpriv_set_extend_ie_ext(net_dev, param, ie_index, ie_bitmap, ie_len);

    return ret;
}

#ifdef _DEBUG_HEAP_MEM_MGR
/*****************************************************************************
 函 数 名  : uapi_ccpriv_heap_dump
 功能描述  : 堆内存维测
 输入参数  : N/A
*****************************************************************************/
osal_u32 uapi_ccpriv_heap_dump(oal_net_device_stru *net_dev, osal_s8 *param)
{
    td_u32 ret;
    td_u8 value = 0;
    unref_param(param);
    ret = wal_async_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_HEAP_DUMP,
        (td_u8 *)&value, sizeof(value));
    wifi_printf("uapi_ccpriv_heap_dump:: ret:%d\r\n", ret);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_ccpriv_heap_dump::send msg failed[%d]!}", ret);
        return OAL_FAIL;
    }
    wifi_printf("OK\r\n");
    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_POWERSAVE
td_u32 uapi_ccpriv_set_sleep(oal_net_device_stru *net_dev, osal_s8 *pc_param)
{
    osal_u32                           off_set;
    osal_s8                            cmd[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_u32                           ret;
    osal_u8                            value;
    ret = wal_get_cmd_one_arg(pc_param, cmd, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_sleep::return err_code [%d]!}", ret);
        return ret;
    }
    value = (osal_u8)atoi((const osal_char *)cmd);
    if (value > 3) { /* 参数范围是0-3 0不允许深睡 1允许深睡 BIT1置位打印睡眠次数 */
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_set_sleep::invalid switch value}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_SLEEP,
        (osal_u8 *)&value, sizeof(osal_u8));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_PWR, "{uapi_ccpriv_set_sleep::send msg failed[%d]!}", ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 函 数 名  : uapi_ccpriv_set_11b_pm
 功能描述  : set_11b
 输入参数  : N/A
*****************************************************************************/
osal_u32 uapi_ccpriv_set_11b_pm(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32                          off_set;
    osal_s8                           ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_s8                           ac_index[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32                          ret;
    osal_u8                           set_11b_param[2]; /* 2：用于储存11b命令的两个参数 */
    osal_s32                          send_ret;

    ret = wal_get_cmd_one_arg(param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_11b_pm::wal_get_cmd_one_arg err[%u]!}\r\n", ret);
        return ret;
    }

    set_11b_param[0] = (osal_u8)atoi((const osal_char *)ac_name); /* 第一个参数为11b rx beacon的开关 */
    if (set_11b_param[0] > 1) { /* 参数范围是0/1 */
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_set_11b_pm::invalid switch value}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    param += off_set;
    ret = wal_get_cmd_one_arg(param, ac_index, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_11b_pm::wal_get_cmd_one_arg err[%u]!}\r\n", ret);
        return ret;
    }
    set_11b_param[1] = (osal_u8)atoi((const osal_char *)ac_index); /* 第二个参数为11b rx beacon max_skip_cnt的大小 */
    if (set_11b_param[1] == 0) { /* 参数范围大于0 */
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_set_11b_pm::invalid switch value}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    send_ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_11B_ON,
        (osal_u8 *)&set_11b_param, OAL_SIZEOF(set_11b_param));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_11b_pm::return err code [%d]!}", send_ret);
        return (osal_u32)send_ret;
    }
    uapi_at_printf("OK\r\n");
    return OAL_SUCC;
}
// rf低功耗开关
osal_u32 uapi_ccpriv_set_rf_pm(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32                          off_set;
    osal_s8                           ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32                          ret;
    osal_u8                           flag;
    osal_s32                          send_ret;

    ret = wal_get_cmd_one_arg(param, ac_name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_rf_pm::wal_get_cmd_one_arg err[%u]!}\r\n", ret);
        return ret;
    }

    flag = (osal_u8)atoi((const osal_char *)ac_name); // rf低功耗开关
    if (flag > 1) { /* 参数范围是0/1 */
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_set_rf_pm::invalid switch value}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                             抛事件到wal层处理
    ***************************************************************************/
    send_ret = wal_sync_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_RF_PM,
        (osal_u8 *)&flag, OAL_SIZEOF(flag));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_rf_pm::return err code [%d]!}", send_ret);
        return (osal_u32)send_ret;
    }
    uapi_at_printf("OK\r\n");
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : 设置随机mac addr扫描是否开启开关
*****************************************************************************/
static td_u32 uapi_ccpriv_set_random_mac_addr_scan(oal_net_device_stru *netdev, td_s8 *pc_param)
{
    td_u32                      off_set;
    td_s8                     ac_arg[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    td_u32                      ret;
    td_u8                       rand_mac_addr_scan_switch;

    /* sh ccpriv.sh "Featureid0 random_mac_addr_scan 0|1(开关)" */
    /* 获取帧方向 */
    ret = wal_get_cmd_one_arg(pc_param, ac_arg, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_random_mac_addr_scan::get switch return err_code[%d]!}", ret);
        return ret;
    }
    rand_mac_addr_scan_switch = (td_u8)atoi((const osal_char *)ac_arg);
    /* 开关的取值范围为0|1,做参数合法性判断 */
    if (rand_mac_addr_scan_switch >= SCAN_RANDOM_MAX) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_random_mac_addr_scan::param is error, switch_value[%d]!}",
            rand_mac_addr_scan_switch);
        return OAL_FAIL;
    }
    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_SET_RANDOM_MAC_ADDR_SCAN,
        (osal_u8 *)&rand_mac_addr_scan_switch, OAL_SIZEOF(rand_mac_addr_scan_switch));
    if (OAL_UNLIKELY(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_random_mac_addr_scan::return err code[%u]!}", ret);
        return ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_CFGID_DEBUG
static osal_u32 uapi_ccpriv_pkt_debug(oal_net_device_stru *cfg_net_dev, osal_s8 *param)
{
    osal_u32 off_set;
    osal_s32 err;
    osal_s8 ac_name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    mac_pkt_debug_ctl_stru debug_ctl = {0};

    if (wal_get_cmd_one_arg(param, ac_name, OAL_SIZEOF(ac_name), &off_set) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_CFG,
            "{uapi_ccpriv_pkt_debug::'wlan0 pkt_debug [on|off|clear|dump|trace|stats|delay]'!}");
        return OAL_FAIL;
    }

    if (osal_strcmp("dump", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.dump = 1;
    } else if (osal_strcmp("off", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.off = 1;
    } else if (osal_strcmp("trace", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.trace = 1;
    } else if (osal_strcmp("icmp", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.icmp = 1;
    } else if (osal_strcmp("auth", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.auth = 1;
    } else if (osal_strcmp("arp", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.arp = 1;
    } else if (osal_strcmp("eapol", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.eapol = 1;
    } else if (osal_strcmp("assoc", (const osal_char *)ac_name) == 0) {
        debug_ctl.bits.assoc = 1;
    } else {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_pkt_debug:: please check second arg!}");
        return OAL_FAIL;
    }

    err = wal_sync_send2device_no_rsp(wal_util_get_vap_id(cfg_net_dev), WLAN_MSG_W2D_C_CFG_DEVICE_PKT_DBG,
        (osal_u8 *)&debug_ctl, OAL_SIZEOF(mac_pkt_debug_ctl_stru));
    if (err != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_pkt_debug::return err code [%d]!}", err);
    }

    return (osal_u32)err;
}

/*****************************************************************************
 功能描述  : 设置性能峰值测试类型开关
*****************************************************************************/
static osal_u32 uapi_ccpriv_set_thruput_test(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 ret;
    osal_s32 cfg_rst;
    osal_u32 off_set = 0;
    osal_u8 thruput_type;
    osal_u8 value;
    osal_s8 *param_tmp = param;
    mac_cfg_thruput_test_stru thruput_info;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    (osal_void)memset_s(&thruput_info, OAL_SIZEOF(thruput_info), 0, OAL_SIZEOF(thruput_info));

    /* 获取性能测试类型 */
    ret = wal_get_cmd_one_arg(param_tmp, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_thruput_test::get type failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    thruput_type = (osal_u8)oal_atoi((const osal_s8 *)name);

    /* 获取设定置 */
    ret = wal_get_cmd_one_arg(param_tmp, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_thruput_test::get value failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    value = (osal_u8)oal_atoi((const osal_s8 *)name);

    thruput_info.thruput_type   = thruput_type;
    thruput_info.value = value;

    cfg_rst = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_THRUPUT_TEST,
        (osal_u8 *)&thruput_info, OAL_SIZEOF(thruput_info));
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_thruput_test::send event failed [%d]!}", cfg_rst);
        return (osal_u32)cfg_rst;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 功能描述  : 设置性能峰值测试类型开关
*****************************************************************************/
#define WAL_PERFORMANCE_ARG_SIZE 4
#define WAL_PERFORMANCE_FIX_AGGR 1
static osal_u32 uapi_ccpriv_set_performace_mode(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 ret;
    osal_s32 cfg_rst;
    osal_u32 off_set = 0;
    osal_s8 *param_tmp = param;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_u8 pf_info[WAL_PERFORMANCE_ARG_SIZE] = {0};

    /* 获取性能测试类型 */
    ret = wal_get_cmd_one_arg(param_tmp, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_performace_mode::get type failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    pf_info[0] = (osal_u8)oal_atoi((const osal_s8 *)name);

    /* 获取设定置 */
    ret = wal_get_cmd_one_arg(param_tmp, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_performace_mode::get value failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    pf_info[1] = (osal_u8)oal_atoi((const osal_s8 *)name);

    cfg_rst = wal_async_send2device_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2D_C_CFG_SET_PERFORMANCE_MODE,
        (osal_u8 *)&pf_info, OAL_SIZEOF(pf_info));
    if (cfg_rst != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_performace_mode::send event failed [%d]!}", cfg_rst);
        return (osal_u32)cfg_rst;
    }

    /* 同步状态到hmac */
    if (pf_info[0] == WAL_PERFORMANCE_FIX_AGGR) {
        hmac_set_thruput_test(THRUPUT_FIX_AGGR, pf_info[1]);
    }

    return OAL_SUCC;
}
#endif

OSAL_STATIC osal_u32 uapi_ccpriv_disable_clear_ipaddr(oal_net_device_stru *net_dev, osal_s8 *param)
{
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    osal_u32 off_set;
    osal_u32 ret;
    osal_u8 disable_clear_ip;

    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_disable_clear_ipaddr::wal_get_cmd_one_arg err[%u]!}\r\n", ret);
        return ret;
    }

    disable_clear_ip = (osal_u8)atoi((const osal_char *)name);
    if ((disable_clear_ip != 0) && (disable_clear_ip != 1)) {
        /* 0:断开连接清理ip;1：断开连接不清理ip */
        return OAL_FAIL;
    }

    hmac_vap = (hmac_vap_stru *)net_dev->ml_priv;
    hmac_vap->disable_clear_ip = disable_clear_ip;
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11AX
OSAL_STATIC osal_u32 uapi_ccpriv_set_11ax_softap_para(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 off_set;
    osal_u32 ret;
    osal_u16 softap_para;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN];

    /* 设置11ax softap参数     */
    /* 获取使能参数配置开关 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_11ax_softap_para::wal_get_cmd_one_arg err_code %d}", ret);
        return ret;
    }
    softap_para = (osal_u8)oal_atoi((const osal_s8 *)name);
    param = param + off_set;

    /* 获取bss color配置参数 */
    ret = wal_get_cmd_one_arg(param, name, OAL_SIZEOF(name), &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_set_11ax_softap_para::wal_get_cmd_one_arg err_code %d}", ret);
        return ret;
    }
    softap_para = softap_para << 8; // 8: softap_param高8位标识是否使能参数，低8位标识bss color
    softap_para |= (osal_u8)oal_atoi((const osal_s8 *)name);
    return (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SET_11AX_SOFTAP_PARAM,
        (osal_u8 *)&softap_para, OAL_SIZEOF(softap_para));
}
#endif

/* ccpriv命令统一格式:  ccpriv [vap_name] [cmd_name] [param1] [...] */
static const wal_ccpriv_cmd_entry_stru  g_ast_ccpriv_cmd[] = {
#ifdef _PRE_WLAN_FEATURE_11AX
    {"set_11ax_softap_para",    uapi_ccpriv_set_11ax_softap_para},
#endif
    {"al_tx_ccpriv",        uapi_ccpriv_always_tx_ccpriv},     /* 使用 AT+CCPRIV=Featureid0,al_tx_ccpriv,data */
    {"mode",                uapi_ccpriv_set_mode},            /* 设置AP 协议模式: mode [mode_type] */
    {"show_frag_threshold", uapi_ccpriv_show_frag_threshold}, /* 打印分片门限 */
    {"send_custom_pkt",     uapi_ccpriv_send_pkt},  /* 发送任意报文命令为:       "wlan0 send_custom_pkt data" */

    {"set_soft_retry_num",  uapi_ccpriv_set_soft_retry_num},     /* AT+CCPRIV=Featureid0,set_soft_retry_num,3,5 */
    {"reginfo",                 uapi_ccpriv_reg_info},
    {"regwrite",                uapi_ccpriv_reg_write},
    {"set_monitor",             uapi_ccpriv_set_monitor_switch}, /* 开关monitor模式:  set_monitor [0|1|2|3|4] */
#ifdef _PRE_WLAN_FEATURE_WS92_MERGE
#ifdef _PRE_WLAN_DFT_STAT
    {"info",                uapi_ccpriv_vap_info}, /* 打印vap的所有参数信息：ccpriv wlan0/ap0 info */
    {"userinfo",            uapi_ccpriv_user_info}, /* 打印指定mac地址user的所有参数信息 ccpriv ap0 userinfo usermac */
#endif
    /* 手工删除user配置:"ap0 kick_user XX:XX:XX:XX:XX:XX(16进制oal_strtohex)", 此处ap0 根据vap名字修改 */
    {"kick_user",           uapi_ccpriv_kick_user},
#endif
#ifdef _PRE_WLAN_FEATURE_FTM
    {"ftm_debug",              uapi_ccpriv_ftm},
#endif
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    /* 设置VAP工作模式通知: ccpriv "vap0 set_opmode_notify 0/1"  0-不支持; 1-支持 */
    {"set_opmode_notify", uapi_ccpriv_set_opmode_notify},
#endif
#ifdef _PRE_WLAN_FEATURE_11V
    {"11v_tx_query",        uapi_ccpriv_11v_tx_query},
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
    {"show_dhcpoffload_info",   uapi_ccpriv_show_dhcpoffload_info},
    {"set_dhcpoffload_info",    uapi_ccpriv_set_dhcpoffload_info}, /* 设置dhcp offload ip地址 */
    {"wow_event",              uapi_ccpriv_set_wow},                /* wow事件配置: wow_event_set [value] */
    {"wow_pattern",            uapi_ccpriv_set_wow_pattern},        /* 设置wow模式: wow_pattern [clr|add|del] [index] */
    /* 设置WOW_OFFLOAD模块是否打印或者清除收发包信息统计信息,0表示打印，1表示清空 "WLAN0 set_clear_wow_offload_info 0/1" */
    {"clear_wow_offload_info", uapi_ccpriv_clear_wow_offload_info},
#endif
#ifdef _PRE_WLAN_FEATURE_11D
    {"get_channel_list",    uapi_ccpriv_get_current_channel_list}, /* 获取当前管制域内的信道列表 */
    {"get_regdomain_pwr_p", uapi_ccpriv_get_regdomain_pwr_priv}, /* 读取管制域最大发送功率 单位dBm */
    {"get_nv_power", uapi_ccpriv_get_nvram_power},
#endif
#ifdef _PRE_WLAN_FEATURE_BLACKLIST
    {"blacklist_add",       uapi_ccpriv_blacklist_add},          /* 添加黑白名单 */
    {"blacklist_del",       uapi_ccpriv_blacklist_del},          /* 删除黑白名单 */
    {"blacklist_show",      uapi_ccpriv_blacklist_show},         /* 查看黑白名单列表 */
#endif
#ifdef _PRE_WLAN_FEATURE_DFX_CHR
    /* CHR上报测试命令:  chr_err_code [err_code] */
    {"chr_err_code",                uapi_ccpriv_chr_test},
#endif
#if defined (_PRE_WLAN_FEATURE_M2U) && defined (_PRE_WLAN_CFGID_DEBUG)
    {"m2u_snoop_enable",        uapi_ccpriv_m2u_snoop_enable},
    {"m2u_snoop_list",          uapi_ccpriv_m2u_snoop_list},
    {"m2u_snoop_deny_table",    uapi_ccpriv_m2u_snoop_deny_table},
    {"m2u_snoop_send_igmp",     uapi_ccpriv_m2u_snoop_send_igmp},
#endif
    {"set_scan_param",          uapi_ccpriv_set_scan_param},
    {"random_mac_scan",         uapi_ccpriv_set_random_mac_addr_scan},   /* 随机mac扫描开关 random_mac_addr_scan [0|1] */
#ifdef _PRE_WLAN_CFGID_DEBUG
    {"pf_mode",                 uapi_ccpriv_set_performace_mode},
    {"random_mac_oui",          uapi_ccpriv_send_random_mac_oui},               /* 随机mac扫描开关 random_mac_oui */
    {"pkt_debug",               uapi_ccpriv_pkt_debug},
    {"thruput_test",            uapi_ccpriv_set_thruput_test},
    {"tx_pkt_trace",            uapi_ccpriv_set_tx_pkt_trace_debug},
    {"multi_bssid_enable",      uapi_ccpriv_multi_bssid_enable},
    {"ampdu_tx_on",             uapi_ccpriv_ampdu_tx_on},    /* 开关ampdu: ampdu_tx_on [0\1] */
    {"ampdu_amsdu",             uapi_ccpriv_set_ampdu_amsdu}, /* 开关amsdu ampdu联合聚合: ampdu_amsdu [0|1] */
    {"set_keepalive_mode",      uapi_ccpriv_set_keepalive_mode},
    {"common_debug",            uapi_ccpriv_common_debug},
    {"set_aging_time",          uapi_ccpriv_set_ap_user_aging_time}, /* 设置keepalive老化时间 */
    {"2040_coexistence",        uapi_ccpriv_set_2040_coext_support},
    {"dhcp_debug",              uapi_ccpriv_dhcp_debug},
    {"service_control_set",     uapi_ccpriv_service_control_set},
    {"service_control_get",     uapi_ccpriv_service_control_get},
#ifdef _PRE_WLAN_SMOOTH_PHASE
    {"set_smooth_phase_en",     uapi_ccpriv_set_smooth_phase_en},
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_SDP
    {"adjust_tx_power",         uapi_ccpriv_adjust_tx_power},
    {"sdp_enable",              uapi_ccpriv_sdp_init},
    {"sdp_start_subscribe",     uapi_ccpriv_sdp_start_subscribe},
    {"sdp_cancel_subscribe",    uapi_ccpriv_sdp_cancle_subscribe},
    {"sdp_send_data",           uapi_ccpriv_sdp_send_data},
    {"sdp_test",                uapi_ccpriv_sdp_test},    /* sdp发送私有帧 */
    {"sdp_start_subscribe_svr", uapi_ccpriv_sdp_start_subscribe_svr},
    {"sdp_cancel_subscribe_svr", uapi_ccpriv_sdp_cancel_subscribe_svr},
#endif
#ifdef _PRE_WLAN_FEATURE_PK_MODE
    {"pk_mode_th_set",         uapi_ccpriv_pk_mode_th_reset},   /* 重设pk模式阈值 */
#endif
#ifdef _PRE_WLAN_DFT_STAT
    {"get_hw_stat_info",         uapi_ccpriv_get_hw_stat}, /* 查询hw实时状况信息 */
    {"enable_hw_stat_info",      uapi_ccpriv_hw_stat_enable}, /* 使能是否查询hw收发包信息 */
    {"get_vap_aggre_info",        uapi_ccpriv_get_vap_aggre_info}, /* 查询vap实时聚合信息 */
    {"enable_external_record",    uapi_ccpriv_external_record_enable},   /* 使能WiFi对外交互记录日志开关 */
    {"get_external_record",       uapi_ccpriv_get_external_record},   /* 使能WiFi对外交互记录日志开关 */
    {"get_vap_stat_info",        uapi_ccpriv_get_vap_stat_new}, /* 查询收发包的状况信息 */
    {"enable_vap_stat_info",     uapi_ccpriv_vap_stat_enable}, /* 使能收发包的能力 */
    {"get_vap_sniffer_info",     uapi_ccpriv_get_vap_sniffer_result}, /* 查询VAP空口实时状况信息 */
    {"get_vap_packet_info",      uapi_ccpriv_get_vap_packet_info}, /* 查询VAP packet包个数和字节数情况 */
#endif
    {"get_tx_params",          uapi_ccpriv_get_tx_params},
#ifdef _PRE_BSLE_GATEWAY
    {"set_bsle_listen",        uapi_ccpriv_bsle_listen},
#endif
    {"aifsn_cfg",               uapi_ccpriv_aifsn_cfg},
    {"cw_cfg",                  uapi_ccpriv_cw_cfg},
#ifdef _PRE_WLAN_FEATURE_SNIFFER
    {"sniffer_save_file",       uapi_ccpriv_wifi_sniffer},
#endif
#ifdef _PRE_WLAN_DFR_STAT
    {"reset_hw",                uapi_ccpriv_set_hw_reset},
    {"reset_queue",             uapi_ccpriv_set_queue_reset},
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
    /* 配置device的smps模式 'wlan0 smps_mode      1/2/3' static/dynamic/disable */
    {"smps_mode",               uapi_ccpriv_set_smps_mode},
#endif
    /* 通过eid移除用户态下发的某个IE "wlan0 remove_app_ie 0/1 eid" 0恢复该ie,1屏蔽该ie */
    {"remove_app_ie",           uapi_ccpriv_remove_app_ie},
    {"set_extended_ie",         uapi_ccpriv_set_extend_ie},
#ifdef _PRE_WLAN_FEATURE_11AX
    /* 设置时空流，单双用户，及使用的Ng, 命令为: echo "wlan0 sts_sung_mung [sts] [sung] [mung]" > /sys/hisys/ccpriv" */
    {"sts_sung_mung",             uapi_ccpriv_set_sts_sung_mung},
#endif
    {"rssi_limit",      uapi_ccpriv_rssi_limit},
#ifdef _PRE_WLAN_RF_CALI_DEBUG
    /* 单音测试 */
    {"tone_test",                   uapi_ccpriv_signle_tone},
    {"txdc_step",                   uapi_ccpriv_txdc_step},
#endif
#ifdef _DEBUG_HEAP_MEM_MGR
    {"heap_dump",            uapi_ccpriv_heap_dump},
#endif
#ifdef _PRE_WLAN_FEATURE_POWERSAVE
    {"set_sleep",           uapi_ccpriv_set_sleep},
    {"set_11b_pm",          uapi_ccpriv_set_11b_pm},
    {"set_rf_pm",          uapi_ccpriv_set_rf_pm},
#endif
    {"disable_clear_ip",         uapi_ccpriv_disable_clear_ipaddr},
};

/*****************************************************************************
 功能描述  : 获取cmd id
 输入参数  : pc_cmd: 命令字符串
 输出参数  : ppst_net_dev: 得到net_device
             pul_off_set: 取net_device偏移的字节
 返 回 值  : 错误码
*****************************************************************************/
static osal_u32 wal_ccpriv_parse_cmd(const osal_char *cmd_name, osal_u8 *ccpriv_cmd_id, wal_ccpriv_cmd_func *func)
{
    osal_u8 cmd_id;
#if defined(_PRE_WLAN_CFGID_DEBUG) && defined(_PRE_WLAN_FEATURE_WS53)
    osal_u32 ret;
#endif

    /* 根据命令名找到命令枚举 */
    for (cmd_id = 0; cmd_id < uapi_array_size(g_ast_ccpriv_cmd); cmd_id++) {
        if (osal_strcmp(g_ast_ccpriv_cmd[cmd_id].cmd_name, cmd_name) == 0) {
            *ccpriv_cmd_id = cmd_id;
            *func = g_ast_ccpriv_cmd[cmd_id].func;
            return OAL_SUCC;
        }
    }
#if defined(_PRE_WLAN_CFGID_DEBUG) && defined(_PRE_WLAN_FEATURE_WS53)
    ret = wal_ccpriv_parse_cmd_debug(cmd_name, ccpriv_cmd_id, func);
    if (ret == OAL_SUCC) {
        return OAL_SUCC;
    }
#endif
    return OAL_FAIL;
}

/*****************************************************************************
 功能描述  : 处理私有配置命令
 输入参数  : pc_cmd: 命令
 返 回 值  : 错误码
*****************************************************************************/
static td_u32 wal_ccpriv_process_cmd(oal_net_device_stru *netdev, td_char *param)
{
    td_u8                   cmd;
    td_u32                  off_set = 0;
    td_u32                  ret;
    wal_ccpriv_cmd_func     cmd_func;
    hmac_ccpriv_para info = {0};

    if (OAL_UNLIKELY(param == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "{wal_ccpriv_process_cmd::pc_cmd null ptr error!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /***************************************************************************
        cmd格式约束
        网络设备名 命令      参数   eg. Featureid0 create vap0
        1~15Byte   1~15Byte
    **************************** ***********************************************/

    if (wal_get_cmd_one_arg((const osal_s8 *)param, info.name, sizeof(info.name), &off_set) != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_process_cmd::wal_get_cmd_one_arg return err_code!}");
        return OAL_FAIL;
    }

    param += off_set;
    ret = wal_ccpriv_parse_cmd((const td_char *)info.name, &cmd, &cmd_func);
    if (ret == OAL_SUCC) {
#ifdef _PRE_WLAN_DFT_STAT
        wal_record_wifi_external_log((td_u32)cmd_func);
#endif
        /* 调用命令对应的函数 */
        ret = cmd_func(netdev, (osal_s8 *)param);
        if (ret != OAL_SUCC) {
            oam_warning_log0(0, OAM_SF_ANY, "{wal_ccpriv_process_cmd::cmd func process failed!}");
        }
        return ret;
    }
    /* 执行单个int类型入参命令通用处理函数 */
#ifdef _PRE_WLAN_DFT_STAT
    wal_record_wifi_external_log((osal_u32)__builtin_return_address(0));
#endif
    ret = uapi_ccpriv_set_one_arg_s32(netdev, (osal_s8 *)param, (const osal_char *)info.name);
    if (ret == OAL_ERR_CODE_CCPRIV_CMD_NOT_FOUND) {
        info.param = (const osal_s8 *)param;
        ret = (osal_u32)wal_sync_send2device_no_rsp(wal_util_get_vap_id(netdev), WLAN_MSG_W2H_CFG_CCPRIV_CMD,
            (osal_u8 *)&info, OAL_SIZEOF(info));
    }
    return ret;
}

OAL_STATIC td_u32 wifi_ccpriv_check_param(td_s32 argc, const td_char *argv[])
{
    if ((argc < 2) || (argc > HI_WIFI_CCPRIV_ARGC_MAX)) {   /* 2: 至少包含一个netdev name和命令字 */
        oam_warning_log1(0, OAM_SF_ANY, "uapi_wifi_ccpriv: argc[%d]!", argc);
        return OAL_FAIL;
    }

#ifdef SUPPORT_HCC
    if ((argv[0] != OSAL_NULL) && (osal_strcmp(argv[0], HCC_TEST_CMD_STR) == 0)) {
        (void)hcc_test_cmd_proc(argc - 1, (char **)(&argv[1]));
        return OAL_SUCC;
    }
#else
    unref_param(argv);
#endif
    return OAL_CONTINUE;
}

/*****************************************************************************
 功能描述  : ccpriv命令入口
 输入参数  : *pc_buffer 命令参数
            count 命令长度
 返 回 值  : 字节数，0表示ccpriv失败
*****************************************************************************/
osal_u32 uapi_wifi_ccpriv(osal_s32 argc, const osal_char *argv[])
{
    td_char *pc_cmd = OSAL_NULL;
    td_char *pc_cmd_tmp = OSAL_NULL;
    oal_net_device_stru *netdev = OSAL_NULL;
    td_u32 ret;
    td_s32 index;
    td_u32 total_len = 0;

    ret = wifi_ccpriv_check_param(argc, argv);
    if (ret != OAL_CONTINUE) {
        return ret;
    }

    /* 第一个参数为netdev name */
    netdev = oal_get_netdev_by_name(argv[0]);
    if (netdev == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_wifi_ccpriv::netdev isn't exits.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    for (index = 1; index < argc; index++) {    /* 1:第一个参数netdv name已经处理 */
        if (argv[index] == OSAL_NULL) {
            oam_error_log0(0, OAM_SF_ANY, "{uapi_wifi_ccpriv::Omitting Parameters is not supported in ccpriv!}");
            return OAL_ERR_CODE_PTR_NULL;
        }
        total_len += osal_strlen(argv[index]) + 1;
    }
    if (total_len > WAL_CCPRIV_CMD_MAX_LEN) {
        oam_error_log1(0, OAM_SF_ANY, "{uapi_wifi_ccpriv::param len overflow, len[%u]!}", total_len);
        return OAL_FAIL;
    }
    /* 复制后续命令字段 */
    pc_cmd = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, (osal_u16)total_len, OAL_TRUE);
    if (OAL_UNLIKELY(pc_cmd == OSAL_NULL)) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_wifi_ccpriv::alloc mem return null ptr!}");
        return OAL_FAIL;
    }
    pc_cmd_tmp = pc_cmd;
    for (index = 1; index < argc; index++) {
        td_u32 str_len = osal_strlen((td_char *)argv[index]);
        if (memcpy_s(pc_cmd_tmp, str_len, argv[index], str_len) != EOK) {   /* 长度已经校验此处直接复制 */
            oam_error_log0(0, 0, "{uapi_wifi_ccpriv::mem safe function err!}");
            continue;
        }
        pc_cmd_tmp[str_len] = ' ';  /* ccpriv使用空格作为分隔符 */
        pc_cmd_tmp += str_len + 1;
    }
    pc_cmd[total_len - 1] = '\0';     /* 添加结束符 */
    /* 处理命令 */
    ret = wal_ccpriv_process_cmd(netdev, pc_cmd);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_wifi_ccpriv::parse cmd return err code[%d]!}", ret);
    }
    oal_mem_free(pc_cmd, OAL_TRUE);
    return ret;
}

#ifdef CONFIG_FACTORY_TEST_MODE
td_u32 wal_add_cfg_vap(void)
{
    /* 已经添加过vap,直接返回成功,产测软件可能会重复下,不能返回失败 */
    if (g_factory_status == OSAL_TRUE) {
        return OAL_SUCC;
    }
    oal_net_device_stru *netdev = oal_get_netdev_by_name(WLAN_CFG_VAP_NAME);
    if (netdev == OSAL_NULL) {
        oam_error_log0(0, 0, "wal_add_cfg_vap: cfg device not fonud.");
        return OAL_FAIL;
    }
    td_u32 ret = wal_setup_vap(netdev);
    if (ret == OAL_SUCC) {
        g_factory_status = OSAL_TRUE;
    } else {
        g_factory_status = OSAL_FALSE;
    }
    return ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
