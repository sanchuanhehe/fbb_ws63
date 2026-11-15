/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: Function related to wifi aware(NAN) feature.
 * Create: 2021-04-16
 */

#include "wal_config_sdp.h"
#include "hmac_sdp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_CONFIG_SDP_C
#ifdef _PRE_WLAN_FEATURE_SDP
/* 接收底层上报的SDP数据信息传递给用户层回调函数，包括接收到的publish和follow up报文 */
osal_s32 wal_sdp_process_rx_data(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    oal_sdp_info *sdp_param = OAL_PTR_NULL;
    unref_param(hmac_vap);

    oam_info_log0(0, OAM_SF_SDP, "wal_sdp_process_rx_data come in.");

    /* 获取用户数据结构体指针 */
    sdp_param = (oal_sdp_info *)msg->data;

    /* 目前无明确应用，直接释放由HMAC申请的内存 */
    if (sdp_param->data != OAL_PTR_NULL) {
        oal_mem_free(sdp_param->data, OAL_TRUE);
        sdp_param->data = OAL_PTR_NULL;
    }
    return OAL_SUCC;
}

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
