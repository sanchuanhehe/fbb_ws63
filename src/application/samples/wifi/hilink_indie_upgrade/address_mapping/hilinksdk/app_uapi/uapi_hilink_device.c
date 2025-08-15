/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Source file for HiLink adaptation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_device.h"

#include "hilink_stdio_adapter.h"
int HILINK_GetDevInfo(HILINK_DevInfo *devinfo)
{
    app_call1(APP_CALL_HILINK_GET_DEV_INFO, HILINK_GetDevInfo, int, HILINK_DevInfo *, devinfo);
    return 0;
}

int HILINK_GetSvcInfo(HILINK_SvcInfo *svcInfo[], unsigned int size)
{
    app_call2(APP_CALL_HILINK_GET_SVC_INFO, HILINK_GetSvcInfo, int, HILINK_SvcInfo *[], svcInfo, unsigned int, size);
    return 0;
}

unsigned char *HILINK_GetAutoAc(void)
{
    app_call0(APP_CALL_HILINK_GET_AUTO_AC, HILINK_GetAutoAc, unsigned char *);
    return NULL;
}

int HILINK_PutCharState(const char *svcId, const char *payload, unsigned int len)
{
    app_call3(APP_CALL_HILINK_PUT_CHAR_STATE, HILINK_PutCharState, int,
        const char *, svcId, const char *, payload, unsigned int, len);
    return 0;
}

int HILINK_ControlCharState(const char *payload, unsigned int len)
{
    app_call2(APP_CALL_HILINK_CONTROL_CHAR_STATE, HILINK_ControlCharState, int,
        const char *, payload, unsigned int, len);
    return 0;
}

int HILINK_GetCharState(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen)
{
    app_call5(APP_CALL_HILINK_GET_CHAR_STATE, HILINK_GetCharState, int, const char *, svcId,
        const char *, in, unsigned int, inLen, char **, out, unsigned int *, outLen);
    return 0;
}

int HILINK_GetPinCode(void)
{
    app_call0(APP_CALL_HILINK_GET_PIN_CODE, HILINK_GetPinCode, int);
    return 0;
}

void HILINK_NotifyDevStatus(int status)
{
    app_call1_ret_void(APP_CALL_HILINK_NOTIFY_DEV_STATUS, HILINK_NotifyDevStatus, int, status);
}

int HILINK_ProcessBeforeRestart(int flag)
{
    app_call1(APP_CALL_HILINK_PROCESS_BEFORE_RESTART, HILINK_ProcessBeforeRestart, int, int, flag);
    return 0;
}
