/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Upgrade and adaptation of the external MCU. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_GetMcuVersion(char *version, unsigned int inLen, unsigned int *outLen)
{
    app_call3(APP_CALL_HILINK_GET_MCU_VERSION, HILINK_GetMcuVersion, int,
        char *, version, unsigned int, inLen, unsigned int *, outLen);
    return 0;
}

int HILINK_NotifyOtaStatus(int flag, unsigned int len, unsigned int type)
{
    app_call3(APP_CALL_HILINK_NOTIFY_OTA_STATUS, HILINK_NotifyOtaStatus, int,
        int, flag, unsigned int, len, unsigned int, type);
    return 0;
}

int HILINK_NotifyOtaData(const unsigned char *data, unsigned int len, unsigned int offset)
{
    app_call3(APP_CALL_HILINK_NOTIFY_OTA_DATA, HILINK_NotifyOtaData, int,
        const unsigned char *, data, unsigned int, len, unsigned int, offset);
    return 0;
}