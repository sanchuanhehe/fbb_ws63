/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: OAL模块初始化.
 */

#include "oal_main.h"
#include "oal_schedule.h"
#include "oal_net.h"
#include "oam_ext_if.h"
#include "wal_main.h"
#include "frw_hmac_hcc_adapt.h"
#include "wlan_thruput_debug.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_OAL_MAIN_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

/* OAL模块初始化总入口，包含OAL模块内部所有特性的初始化 */
osal_s32 oal_main_init_etc(osal_void)
{
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
    hcc_hmac_wifi_msg_register(D2H_MSG_WLAN_READY, (osal_void *)device_init_ready);
#endif

    stat_timer_enable();

    return OAL_SUCC;
}

/* OAL模块卸载 */
osal_void oal_main_exit_etc(osal_void)
{
    return ;
}

osal_u32  oal_chip_get_version_etc(osal_void)
{
    return WLAN_CHIP_VERSION_WF51V100H;
}

/* 检查单chip中mac device使能个数 */
OAL_STATIC osal_u8  oal_device_check_enable_num(osal_void)
{
    return (osal_u8)WLAN_SERVICE_DEVICE_MAX_NUM_PER_CHIP;
}

/* 获取device num */
osal_u8 oal_chip_get_device_num_etc(osal_u32   chip_ver)
{
    (void)chip_ver;
    return oal_device_check_enable_num();
}
/* 获取board上的业务vap其实idx */
WIFI_TCM_TEXT osal_u8 oal_board_get_service_vap_start_id(osal_void)
{
    osal_u8   device_num_per_chip = oal_device_check_enable_num();

    /* 配置vap个数 = mac device个数,vap idx分配先配置vap,后业务vap */
    return (osal_u8)(WLAN_CHIP_MAX_NUM_PER_BOARD * device_num_per_chip);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

