/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:  Entry, initialization, and external interface definition of the algorithm module, depending on DMAC
 */

/******************************************************************************
  1 头文件包含
******************************************************************************/
#include "alg_main.h"
#include "alg_schedule_if.h"
#include "alg_autorate.h"
#include "alg_txbf.h"
#include "alg_edca_opt.h"
#include "alg_temp_protect.h"
#include "alg_probe_common.h"
#include "dmac_ext_if_hcm.h"
#ifdef _PRE_WLAN_FEATURE_CCA_OPT
#include "alg_cca_optimize.h"
#endif
#ifdef _PRE_WLAN_FEATURE_INTF_DET
#include "alg_intf_det.h"
#endif
#ifdef _PRE_WLAN_FEATURE_ANTI_INTERF
#include "alg_anti_interference.h"
#endif
#ifdef _PRE_WLAN_FEATURE_RTS
#include "alg_rts.h"
#endif
#include "alg_tpc.h"
#include "alg_dbac_hmac.h"
#include "alg_aggr.h"
#include "hmac_alg_notify.h"
#include "alg_traffic_ctl.h"
#include "alg_tx_tb_hmac.h"
#include "alg_gla.h"
#include "msg_alg_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_ALG_MAIN_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/******************************************************************************
  2 全局变量定义
******************************************************************************/
OAL_STATIC alg_internal_hook_stru g_alg_internal_hook = {};
OAL_STATIC osal_u32 g_alg_mem_stat[ALG_MEM_BUTT] = {0};

typedef struct {
    osal_s16 noise;       /* 底噪 */
    osal_u16 busy_ratio;  /* 繁忙度 */
    osal_u16 coch_ratio;  /* 同频干扰 */
    osal_u8  channel;
    osal_u8  resv;
} wifi_channel_info_stru;
/******************************************************************************
  3 函数实现
******************************************************************************/
__attribute__((weak)) osal_s32 alg_hmac_txbf_init(osal_void);
__attribute__((weak)) osal_void alg_hmac_txbf_exit(osal_void);
__attribute__((weak)) osal_void alg_temp_protect_init(osal_void);
__attribute__((weak)) osal_void alg_temp_protect_exit(osal_void);
__attribute__((weak)) osal_s32 alg_anti_intf_init(osal_void);
__attribute__((weak)) osal_void alg_anti_intf_exit(osal_void);
__attribute__((weak)) osal_s32 alg_edca_opt_init(osal_void);
__attribute__((weak)) osal_void alg_edca_opt_exit(osal_void);
__attribute__((weak)) osal_s32 alg_cca_opt_init(osal_void);
__attribute__((weak)) osal_void alg_cca_opt_exit(osal_void);
alg_internal_hook_stru* hmac_alg_get_notify_if(osal_void)
{
    return &g_alg_internal_hook;
}
/*****************************************************************************
 函 数 名  : hmac_alg_cfg_intf_det_pk_mode_notify
 功能描述  : 干扰检测算法PK模式通知函数
*****************************************************************************/
osal_u32 hmac_alg_cfg_intf_det_pk_mode_notify(hmac_vap_stru *hmac_vap, oal_bool_enum_uint8 is_pk_mode)
{
    alg_internal_hook_stru* callback = hmac_alg_get_notify_if();
    if (osal_unlikely(hmac_vap == OSAL_NULL)) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_alg_cfg_intf_det_pk_mode_notify:: OSAL_NULL == hmac_vap");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (callback->cfg_intf_det_pk_mode_notify_func) {
        callback->cfg_intf_det_pk_mode_notify_func(hmac_vap, is_pk_mode);
    }

    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : hmac_alg_cfg_intf_det_edca_notify
 功能描述  : 干扰检测算法通知EDCA进行碰撞统计函数
*****************************************************************************/
osal_u32 hmac_alg_cfg_intf_det_edca_notify(osal_void)
{
    alg_internal_hook_stru* callback = hmac_alg_get_notify_if();
    if (callback->cfg_intf_det_edca_notify_func) {
        callback->cfg_intf_det_edca_notify_func();
    }
    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : hmac_alg_cfg_intf_det_cca_notify
 功能描述  : 干扰检测算法通知CCA干扰状态变化函数
*****************************************************************************/
osal_u32 hmac_alg_cfg_intf_det_cca_notify(hal_to_dmac_device_stru *hal_device, osal_u8 old_intf_mode,
    osal_u8 cur_intf_mode)
{
    alg_internal_hook_stru* callback = hmac_alg_get_notify_if();
    if (callback->cfg_intf_det_cca_notify_func) {
        callback->cfg_intf_det_cca_notify_func(hal_device, old_intf_mode, cur_intf_mode);
    }
    return OAL_SUCC;
}
/*****************************************************************************
 函 数 名  : hmac_alg_cfg_intf_det_cca_notify
 功能描述  : 干扰检测算法通知CCA干扰状态变化函数
*****************************************************************************/
osal_u32 hmac_alg_cfg_intf_det_cca_chk_noise_notify(osal_s8 avg_rssi_20, oal_bool_enum_uint8 coch_intf_state_sta,
    osal_u8 cur_intf)
{
    alg_internal_hook_stru* callback = hmac_alg_get_notify_if();
    if (callback->cfg_intf_det_cca_chk_noise_notify_func) {
        callback->cfg_intf_det_cca_chk_noise_notify_func(avg_rssi_20, coch_intf_state_sta, cur_intf);
    }
    return OAL_SUCC;
}
OAL_STATIC osal_u32 alg_para_sync_hmac_to_dmac(alg_param_sync_stru *data, osal_u16 len)
{
    osal_u32 ret = OAL_SUCC;
    frw_msg msg = {0};
    msg.data = (osal_u8 *)data;
    msg.data_len = len;
    /* 通知各个算法填充配置参数 */
    hmac_alg_para_sync_notify(data);
    /* 抛事件至Device侧alg */
    ret = (osal_u32)frw_send_msg_to_device(0, WLAN_MSG_H2D_C_CFG_ALG_PARAM_SYNC, &msg, OSAL_TRUE);
    if (ret != OAL_SUCC) {
        oam_error_log1(0, OAM_SF_CFG, "{alg_para_sync_hmac_to_dmac:: frw_send_msg_to_device fail [%d].}", ret);
    }
    return ret;
}

WIFI_TCM_TEXT osal_s32 hmac_alg_sync_distance_info(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    osal_u32 ret = OAL_SUCC;
    osal_u32 index;
    alg_internal_hook_stru *alg_internal_hook = &g_alg_internal_hook;
    dmac_to_hmac_syn_distance_stru *sync = OAL_PTR_NULL;
    if (osal_unlikely((msg == OAL_PTR_NULL) || (hmac_vap == OAL_PTR_NULL))) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_syn_info_event::msg or hmac_vap is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    sync = (dmac_to_hmac_syn_distance_stru *)msg->data;
    for (index = 0; index < ALG_DISTANCE_NOTIFY_BUTT; ++index) {
        if (alg_internal_hook->pa_distance_notify_func[index] != NULL) {
            ret = alg_internal_hook->pa_distance_notify_func[index](hmac_vap->hal_device, sync->distance);
        }
    }

    return (osal_s32)ret;
}

#define DUTY_CYC_RATIO_SHIFT 10 /* 计算邻频干扰繁忙度的扩大因子 */
OSAL_STATIC osal_u16 hmac_conn_intf_info_get_duty_cyc_ratio(const wlan_scan_chan_stats_stru *chan_result, osal_u8 idx)
{
    osal_u16 duty_cyc_ratio = 0;

    if (chan_result[idx].total_stats_time_us == 0) {
        return duty_cyc_ratio;
    }

    duty_cyc_ratio = (osal_u16)(((chan_result[idx].total_free_time_20m_us +
        chan_result[idx].total_recv_time_us + chan_result[idx].total_send_time_us) >=
        chan_result[idx].total_stats_time_us) ? 0 : (((chan_result[idx].total_stats_time_us -
        chan_result[idx].total_free_time_20m_us - chan_result[idx].total_recv_time_us -
        chan_result[idx].total_send_time_us) << DUTY_CYC_RATIO_SHIFT) /
        (chan_result[idx].total_stats_time_us)));
    return duty_cyc_ratio;
}

OSAL_STATIC osal_u16 hmac_conn_intf_info_get_nondir_duty(const wlan_scan_chan_stats_stru *chan_result,
    const hal_ch_mac_statics_stru *mac_stats, osal_u8 idx)
{
    osal_u32 rx_nondir_time;
    osal_u32 total_stats_time_ms;
    osal_u32 total_recv_time_us = chan_result[idx].total_recv_time_us;
    osal_u16 rx_nondir_duty_cyc = 0;

    total_stats_time_ms = chan_result[idx].total_stats_time_us >> DUTY_CYC_RATIO_SHIFT;
    if (total_stats_time_ms == 0) {
        return rx_nondir_duty_cyc;
    }

    if (total_recv_time_us < mac_stats[idx].rx_direct_time) {
        rx_nondir_time = mac_stats[idx].rx_nondir_time;
    } else {
        rx_nondir_time = osal_max((total_recv_time_us - mac_stats[idx].rx_direct_time), mac_stats[idx].rx_nondir_time);
        /* 为避免计算出的rx nondir time与mac统计的时间相差较大，导致误检测，做一个约束处理 */
        if ((mac_stats[idx].rx_nondir_time > 1000) && /* 1000:大于1000 */
            /* 2:左移2 */
            ((total_recv_time_us - mac_stats[idx].rx_direct_time) >= (mac_stats[idx].rx_nondir_time << 2))) {
            rx_nondir_time = mac_stats[idx].rx_nondir_time << 1;
        }
    }
    rx_nondir_duty_cyc = (osal_u16)(rx_nondir_time / total_stats_time_ms);

    return rx_nondir_duty_cyc;
}

OAL_STATIC osal_void hmac_chan_intf_info_report(wifi_channel_info_stru *chan_info, size_t info_len, osal_u8 *chan_num)
{
    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(0);
    hal_to_dmac_device_stru *hal_dev = hal_chip_get_hal_device();
    hmac_scan_record_stru *scan_record = OSAL_NULL;
    wlan_scan_chan_stats_stru *chan_stats = OSAL_NULL;
    hal_ch_mac_statics_stru *mac_ch_stats = OSAL_NULL;
    alg_intf_det_stru *intf_det = OSAL_NULL;
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    osal_u16 duty_cyc_ratio_20;
    osal_u16 nondir_duty_cyc;
    osal_s8 intf_det_avg_rssi_20_scan;
    osal_u8 idx;

    /* 获取intf det优化的算法结构体 */
    if (hmac_alg_get_device_priv_stru(hal_dev, HAL_ALG_DEVICE_STRU_ID_INTF_DET, (osal_void **)&intf_det) != OAL_SUCC) {
        return;
    }

    scan_record = &(hmac_device->scan_mgmt.scan_record_mgmt);
    chan_stats = scan_record->chan_results;
    mac_ch_stats = scan_record->mac_ch_stats;

    /* 检测本次扫描是否开启了信道测量，如果没有直接返回 */
    if (chan_stats[0].stats_valid == 0) {
        return;
    }

    /* 如果本次扫描检测信道数大于入参数组长度，直接返回 */
    if (scan_record->chan_numbers > info_len) {
        return;
    }

    *chan_num = scan_record->chan_numbers;
    (osal_void)mac_device_find_up_vap_etc(hmac_device, &hmac_vap);

    /* 信道测量结果 */
    for (idx = 0; idx < scan_record->chan_numbers; idx++) {
        chan_info[idx].channel = chan_stats[idx].channel_number;
        if ((hmac_vap != OSAL_NULL) && (chan_info[idx].channel == hmac_vap->channel.chan_number)) {
            chan_info[idx].busy_ratio = intf_det->adjch_intf_stat.duty_cyc_ratio_20;
            chan_info[idx].coch_ratio = intf_det->coch_intf_stat.rx_nondir_duty_cyc;
            chan_info[idx].noise = intf_det->adjch_intf_stat.intf_det_avg_rssi_20;
            continue;
        }

        /* 获取统计的信道结果:底噪平均值,如果统计计数为0则不做除法 */
        if (chan_stats[idx].free_power_cnt == 0) {
            intf_det_avg_rssi_20_scan = 0;
        } else {
            intf_det_avg_rssi_20_scan =
                (osal_s8)(chan_stats[idx].free_power_stats_20m / (osal_s16)chan_stats[idx].free_power_cnt);
        }

        duty_cyc_ratio_20 = hmac_conn_intf_info_get_duty_cyc_ratio(chan_stats, idx);

        nondir_duty_cyc = hmac_conn_intf_info_get_nondir_duty(chan_stats, mac_ch_stats, idx);

        chan_info[idx].busy_ratio = duty_cyc_ratio_20;
        chan_info[idx].coch_ratio = nondir_duty_cyc;
        chan_info[idx].noise = intf_det_avg_rssi_20_scan;
    }
}

/*****************************************************************************
 功能描述  : 同步参数设置固定速率
*****************************************************************************/
OSAL_STATIC osal_s32 hmac_config_get_intrf_info(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    wifi_channel_info_stru wifi_channel_info[MAC_CHANNEL_FREQ_2_BUTT] = {0};
    osal_u32 offset = 0;
    osal_u8 chan_num = 0;
    unref_param(hmac_vap);

    if (osal_unlikely(msg == OSAL_NULL || msg->rsp == OSAL_NULL)) {
        return OAL_FAIL;
    }

    hmac_chan_intf_info_report(wifi_channel_info, osal_array_size(wifi_channel_info), &chan_num);

    if (memcpy_s(msg->rsp, msg->rsp_buf_len, wifi_channel_info, sizeof(wifi_channel_info)) != EOK) {
        return OAL_FAIL;
    }
    offset += (osal_u32)sizeof(wifi_channel_info);
    if (memcpy_s(msg->rsp + offset, msg->rsp_buf_len - offset, &chan_num, sizeof(osal_u8)) != EOK) {
        return OAL_FAIL;
    }
    offset += (osal_u32)sizeof(osal_u8);
    msg->rsp_len = (osal_u16)offset;
    return OAL_SUCC;
}

/******************************************************************************
 功能描述  : ALG模块初始化总入口，包含ALG模块内部所有特性的初始化。
******************************************************************************/
osal_void alg_hmac_main_init(osal_void)
{
    /* 调用各个子算法的初始化 */
    alg_probe_common_init();
#ifdef _PRE_WLAN_FEATURE_SCHEDULE
    alg_schedule_init();
#endif
#ifdef _PRE_WLAN_FEATURE_AUTORATE
    alg_hmac_autorate_init();
#endif
#ifdef _PRE_WLAN_FEATURE_TXBF
    alg_hmac_txbf_init();
#endif
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT
    alg_edca_opt_init();
#endif
#ifdef _PRE_WLAN_FEATURE_TEMP_PROTECT
    alg_temp_protect_init();
#endif
#ifdef _PRE_WLAN_FEATURE_CCA_OPT
    alg_cca_opt_init();
#endif
#ifdef _PRE_WLAN_FEATURE_INTF_DET
    alg_intf_det_init();
#endif
#ifdef _PRE_WLAN_FEATURE_ANTI_INTERF
    alg_anti_intf_init();
#endif
#ifdef _PRE_WLAN_FEATURE_TRAFFIC_CTL
    alg_hmac_traffic_ctl_init();
#endif
#ifdef _PRE_WLAN_FEATURE_RTS
    alg_host_rts_init();
#endif
#ifdef _PRE_WLAN_FEATURE_TPC
    alg_tpc_hmac_init();
#endif
#ifdef _PRE_WLAN_FEATURE_DBAC
    alg_dbac_hmac_init();
#endif
#ifdef _PRE_WLAN_FEATURE_AUTOAGGR
    alg_aggr_hmac_init();
#endif
    alg_tx_tb_hmac_init();
    alg_hmac_gla_init();
    hmac_alg_register_distance_notify_func(ALG_DISTANCE_NOTIFY_DMAC_MODULE, hmac_alg_distance_notify_hook);

    frw_msg_hook_register(WLAN_MSG_D2H_C_ALG_DISTANCE_SYN, hmac_alg_sync_distance_info);
    frw_msg_hook_register(WLAN_MSG_W2H_CFG_GET_INTRF_INFO, hmac_config_get_intrf_info);
}
osal_void alg_hmac_sync_param(osal_void)
{
    alg_param_sync_stru alg_sync_param = {0};
    /* 通知各算法填充参数并发送至device侧 */
    alg_sync_param.head_magic_num = ALG_MAGIC_NUM;
    alg_sync_param.tail_magic_num = ALG_MAGIC_NUM;
    alg_para_sync_hmac_to_dmac(&alg_sync_param, sizeof(alg_param_sync_stru));
}
/******************************************************************************
 函 数 名  : alg_hmac_main_exit
 功能描述  : ALG模块卸载
******************************************************************************/
osal_void alg_hmac_main_exit(osal_void)
{
    /* 模块卸载成功后，输出打印 */
    /* 拥塞控制必须在调度算法之前exit */
#ifdef _PRE_WLAN_FEATURE_SCHEDULE
    alg_schedule_exit();
#endif

#ifdef _PRE_WLAN_FEATURE_AUTORATE
    alg_hmac_autorate_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_TXBF
    alg_hmac_txbf_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_INTF_DET
    alg_intf_det_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_ANTI_INTERF
    alg_anti_intf_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT
    alg_edca_opt_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_TEMP_PROTECT
    alg_temp_protect_exit();
#endif

#ifdef _PRE_WLAN_FEATURE_CCA_OPT
    alg_cca_opt_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_TPC
    alg_tpc_hmac_exit();
#endif

#ifdef _PRE_WLAN_FEATURE_RTS
    alg_host_rts_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_DBAC
    alg_dbac_hmac_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_AUTOAGGR
    alg_aggr_hmac_exit();
#endif
#ifdef _PRE_WLAN_FEATURE_TRAFFIC_CTL
    alg_hmac_traffic_ctl_exit();
#endif
    alg_tx_tb_hmac_exit();
    alg_probe_common_exit();
    alg_hmac_gla_exit();
    /* 去注册算法配置事件处理钩子 */
    hmac_alg_unregister_distance_notify_func(ALG_DISTANCE_NOTIFY_DMAC_MODULE);
    frw_msg_hook_unregister(WLAN_MSG_D2H_C_ALG_DISTANCE_SYN);
    frw_msg_hook_unregister(WLAN_MSG_W2H_CFG_GET_INTRF_INFO);

    return;
}

/******************************************************************************
 函 数 名  : alg_register_distance_notify_func
 功能描述  : 距离状态改变通知函数
******************************************************************************/
osal_u32 hmac_alg_register_distance_notify_func(alg_distance_notify_enum_uint8 notify_sub_type,
    p_alg_distance_notify_func func)
{
    g_alg_internal_hook.pa_distance_notify_func[notify_sub_type] = func;

    return OAL_SUCC;
}

/******************************************************************************
 函 数 名  : alg_unregister_vap_distance_notify_func
 功能描述  : 距离状态改变通知函数注销
******************************************************************************/
osal_u32 hmac_alg_unregister_distance_notify_func(alg_distance_notify_enum_uint8 notify_sub_type)
{
    g_alg_internal_hook.pa_distance_notify_func[notify_sub_type] = OSAL_NULL;

    return OAL_SUCC;
}

osal_void *alg_mem_alloc(alg_mem_enum_uint8 alg_id, osal_u16 len)
{
    osal_void *buf = OSAL_NULL;
    if (alg_id >= ALG_MEM_BUTT) {
        return OSAL_NULL;
    }
    /* 内存池自动处理是否从pkram申请 */
    buf = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, len, OAL_TRUE);
    if (buf != OSAL_NULL) {
        g_alg_mem_stat[alg_id] += len;
    } else {
        oam_error_log2(0, OAM_SF_ANY, "{alg_mem_alloc:alloc fail.alg_id %d, len %d}", alg_id, len);
    }
    return buf;
}

__attribute__((weak)) osal_s32 alg_hmac_txbf_init(osal_void)
{
    return OAL_SUCC;
}

__attribute__((weak)) osal_void alg_hmac_txbf_exit(osal_void)
{
    return;
}

__attribute__((weak)) osal_s32 alg_anti_intf_init(osal_void)
{
    /* 设置弱干扰免疫默认门限值 */
    hal_set_nav_max_duration(ALG_ANTI_INF_MAX_DUR_VAL, ALG_ANTI_INF_OBSS_MAX_DUR_VAL);
    hal_set_agc_unlock_min_th(ALG_ANTI_INF_UNLOCK_TX_TH_DEFAULT_VAL, ALG_ANTI_INF_UNLOCK_RX_TH_DEFAULT_VAL);
    hal_set_weak_intf_rssi_th(ALG_ANTI_INF_RSSI_TH_DEFAULT_VAL);
    return OAL_SUCC;
}

__attribute__((weak)) osal_void alg_anti_intf_exit(osal_void)
{
    return;
}

__attribute__((weak)) osal_s32 alg_edca_opt_init(osal_void)
{
    return OAL_SUCC;
}

__attribute__((weak)) osal_void alg_edca_opt_exit(osal_void)
{
    return;
}

__attribute__((weak)) osal_s32 alg_cca_opt_init(osal_void)
{
    /* 设置CCA默认门限值 */
    hal_set_ed_high_th(HAL_CCA_OPT_ED_HIGH_20TH_DEF, HAL_CCA_OPT_ED_HIGH_40TH_DEF, OSAL_TRUE);
    hal_set_cca_prot_th(HAL_CCA_OPT_ED_LOW_TH_DSSS_DEF, HAL_CCA_OPT_ED_LOW_TH_OFDM_DEF);
    return OAL_SUCC;
}

__attribute__((weak)) osal_void alg_cca_opt_exit(osal_void)
{
    return;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
