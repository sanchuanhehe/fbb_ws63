/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: SoftAP Adaptation Implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen)
{
    app_call2(APP_CALL_HILINK_START_SOFT_AP, HILINK_StartSoftAp, int, const char *, ssid, unsigned int, ssidLen);
    return 0;
}

int HILINK_StopSoftAp(void)
{
    app_call0(APP_CALL_HILINK_STOP_SOFT_AP, HILINK_StopSoftAp, int);
    return 0;
}