/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
* Description: Header file of main.c.
*/

/*****************************************************************************
    头文件包含
*****************************************************************************/
#include "stdio.h"

#include <securec.h>
#include "soc_wifi_api.h"
#include "oam_ext_if.h"
#include "oal_types.h"
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
#include "lwip/tcpip.h"
#endif
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
#ifdef BOARD_FPGA_WIFI
#include "abb_config.h"
#include "mpw0_poweron.h"
#endif
#endif
#include "hal_phy.h"
#include "hal_mac.h"
#include "oal_util_hcm.h"
#ifdef CONFIG_SLE_BASE_STATION_MASTER
#include "syschannel_host/main.h"
#endif
#ifdef CONFIG_SLE_BASE_STATION_SLAVE
#include "syschannel_dev_adapt.h"
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WIFI_MAIN_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

int wifi_host_task(void *param)
{
    td_s32 ret = OAL_SUCC;
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
#ifdef BOARD_FPGA_WIFI
    // 共核编译的直接在host初始化abb 其他情况在device初始化中完成
    hh503_abb5_init_pre();
    mpw0_poweron();
#endif
#endif
    unref_param(param);
    wifi_printf("-->wifi_host_task enter.\n");

    ret = uapi_wifi_init(1, 1);
    if (ret != OAL_SUCC) {
        wifi_printf("Fail to uapi_wifi_init!\n");
        return OAL_FAIL;
    }
#ifndef _PRE_WLAN_USE_CUSTOM_LWIP
    tcpip_init(NULL, NULL);
#endif
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
#ifdef CONFIG_SLE_BASE_STATION_MASTER
    /* lwip初始化后初始化syshannel */
    wlan_init();
#elif defined(CONFIG_SLE_BASE_STATION_SLAVE)
    syschannel_dev_create_task(SPI_TYPE);
#endif
#endif
    wifi_printf("-->wifi_host_task finish.\n");
    return OAL_SUCC;
}

int wifi_init_task(void)
{
    osal_task *result = NULL;
    /* stack 大小: 8 * 1024 */
    result = osal_adapt_kthread_create(wifi_host_task, OSAL_NULL, "wifi_host", (8 * 1024));
    if (result == NULL) {
        return OAL_FAIL;
    }
    return OAL_SUCC;
}
