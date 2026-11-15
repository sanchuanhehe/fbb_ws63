  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hilink network adapter
 *
 * History: \n
 * 2024-05-28, Create file.
 */
#include "hilink_call.h"
#include "hilink_quick_netcfg_api.h"
#include "hilink_quick_netcfg_adapter.h"

int HILINK_SetQuickCfgCommonLoader(QuickCfgCommonLoader *loader, unsigned int loaderSize)
{
    hilink_call2(HILINK_CALL_HILINK_SET_QUICK_CFG_COMMON_LOADER, HILINK_SetQuickCfgCommonLoader, int,
        QuickCfgCommonLoader *, loader, unsigned int, loaderSize);
    return 0;
}
int HILINK_StartQuickCfg(void)
{
    hilink_call0(HILINK_CALL_HILINK_START_QUICK_CFG, HILINK_StartQuickCfg, int);
    return 0;
}

int HILINK_FrameParse(const unsigned char *frame, unsigned int len)
{
    hilink_call2(HILINK_CALL_HILINK_FRAME_PARSE, HILINK_FrameParse, int,
        const unsigned char *, frame, unsigned int, len);
    return 0;
}

int HILINK_QuickCfgCmdParse(const char *payload, unsigned int len)
{
    hilink_call2(HILINK_CALL_HILINK_QUICK_CFG_CMD_PARSE, HILINK_QuickCfgCmdParse, int,
        const char *, payload, unsigned int, len);
    return 0;
}

int HILINK_SetDeviceType(DevType type)
{
    hilink_call1(HILINK_CALL_HILINK_SET_DEVICE_TYPE, HILINK_SetDeviceType, int,
        DevType, type);
    return 0;
}

int HILINK_SetQuickCfgWifiLoader(QuickCfgWifiLoader *loader, unsigned int loaderSize)
{
    hilink_call2(HILINK_CALL_HILINK_SET_QUICK_CFG_WIFI_LOADER, HILINK_SetQuickCfgWifiLoader, int,
        QuickCfgWifiLoader *, loader, unsigned int, loaderSize);
    return 0;
}

void HILINK_EnableQuickNetCfg(void)
{
    hilink_call0_ret_void(HILINK_CALL_HILINK_ENABLE_QUICK_NET_CFG, HILINK_EnableQuickNetCfg);
}