/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK提供的demo示例代码（此文件为DEMO，需集成方适配修改）
 */
#include <stdio.h>
#include <stddef.h>
#include "hilink.h"
#include "cmsis_os2.h"
#include "hilink_socket_adapter.h"
#include "hilink_sal_defines.h"
#include "hal_reboot.h"
#include "hilink_device.h"
#include "hilink_network_adapter.h"

#ifdef SUPPORT_QUICK_NETCFG
#include "hilink_quick_netcfg_demo.h"
#include "hilink_quick_netcfg_api.h"
#endif

static int GetAcV2Func(unsigned char *acKey, unsigned int acLen)
{
    /* key文件在开发者平台获取 */
    return -1;
}

static unsigned int GetWifiRecType(void)
{
    return (0x01 | 0x02);
}

static int HardReboot(void)
{
    hal_reboot_chip();
    return 0;
}

int hilink_wifi_main(void)
{
    /* 注册ACKey函数 */
    HILINK_RegisterGetAcV2Func(GetAcV2Func);

    /* 设置配网方式 */
    HILINK_SetNetConfigMode(HILINK_NETCONFIG_WIFI);

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    if (HILINK_RegisterErrnoCallback(get_os_errno) != 0) {
        HILINK_SAL_NOTICE("reg errno cb err\r\n");
        return -1;
    }
#endif

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        HILINK_SAL_NOTICE("sdkAttr is null");
        return -1;
    }
    sdkAttr->monitorTaskStackSize = 0x600;
    sdkAttr->rebootHardware = HardReboot;
    sdkAttr->rebootSoftware = HardReboot;
    HILINK_SetSdkAttr(*sdkAttr);

#ifdef SUPPORT_QUICK_NETCFG
    HILINK_EnableQuickNetCfg();
#endif

    /* 注册极简配网WIFI相关函数 */
    WiFiRecoveryApi wifiCb;
    wifiCb.getWifiRecoveryType = GetWifiRecType;
    wifiCb.scanAP = HILINK_ScanAP;
    wifiCb.getAPScanResult = HILINK_GetAPScanResult;
    wifiCb.restartWiFi = HILINK_RestartWiFi;
    wifiCb.connectWiFiByBssid = HILINK_ConnectWiFiByBssid;
    wifiCb.lastConnResult = HILINK_GetLastConnectResult;
    if (HILINK_RegWiFiRecoveryCallback((const WiFiRecoveryApi *)&wifiCb, sizeof(wifiCb)) != 0) {
        HILINK_SAL_NOTICE("reg wifi recovery failed!\r\n");
        return -1;
    }
    HILINK_SAL_NOTICE("RegWiFiRecoveryCallback finish!\n");

    /* 启动Hilink SDK */
    if (HILINK_Main() != 0) {
        HILINK_SAL_NOTICE("HILINK_Main start error");
        return -1;
    }

#ifdef SUPPORT_QUICK_NETCFG
    StartQuickNetCfg();
#endif

    return 0;
}
