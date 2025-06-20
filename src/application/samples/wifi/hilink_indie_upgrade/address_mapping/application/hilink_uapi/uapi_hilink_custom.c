  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hilink sle api
 *
 * History: \n
 * 2024-11-08, Create file.
 */
#include "hilink_call.h"
#include "hilink_sle_api.h"

void HILINK_SetProtType(const int protType)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_SET_PROT_TYPE, HILINK_SetProtType, const int, protType);
}

void HILINK_EnablePrescan(void)
{
    hilink_call0_ret_void(HILINK_CALL_HILINK_ENABLE_PRESCAN, HILINK_EnablePrescan);
}

int HILINK_SetNetConfigInfo(const char *info)
{
    hilink_call1(HILINK_CALL_HILINK_SET_NET_CONFIG_INFO, HILINK_SetNetConfigInfo, int, const char *, info);
    return 0;
}

int HILINK_SetSoftAPMode(void)
{
    hilink_call0(HILINK_CALL_HILINK_SET_SOFT_APMODE, HILINK_SetSoftAPMode, int);
    return 0;
}

int HILINK_RequestRegInfo(unsigned int regInfoNums)
{
    hilink_call1(HILINK_CALL_HILINK_REQUEST_REG_INFO, HILINK_RequestRegInfo, int, unsigned int, regInfoNums);
    return 0;
}

int HILINK_DiagnosisInfoRecord(int errCode, const char *param)
{
    hilink_call2(HILINK_CALL_HILINK_DIAGNOSIS_INFO_RECORD, HILINK_DiagnosisInfoRecord, int,
        int, errCode, const char *, param);
    return 0;
}