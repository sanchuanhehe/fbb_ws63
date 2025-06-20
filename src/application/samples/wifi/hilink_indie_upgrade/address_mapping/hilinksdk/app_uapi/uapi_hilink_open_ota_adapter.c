/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: OTA Adaptation Implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include <stdbool.h>
#include "app_call.h"

bool HILINK_OtaAdapterFlashInit(void)
{
    app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_INIT, HILINK_OtaAdapterFlashInit, bool);
    return false;
}

unsigned int HILINK_OtaAdapterGetUpdateIndex(void)
{
    app_call0(APP_CALL_HILINK_OTA_ADAPTER_GET_UPDATE_INDEX, HILINK_OtaAdapterGetUpdateIndex, unsigned int);
    return 0;
}

int HILINK_OtaAdapterFlashErase(unsigned int size)
{
    app_call1(APP_CALL_HILINK_OTA_ADAPTER_FLASH_ERASE, HILINK_OtaAdapterFlashErase, int, unsigned int, size);
    return 0;
}

int HILINK_OtaAdapterFlashWrite(const unsigned char *buf, unsigned int bufLen)
{
    app_call2(APP_CALL_HILINK_OTA_ADAPTER_FLASH_WRITE, HILINK_OtaAdapterFlashWrite, int,
        const unsigned char *, buf, unsigned int, bufLen);
    return 0;
}

int HILINK_OtaAdapterFlashRead(unsigned int offset, unsigned char *buf, unsigned int bufLen)
{
    app_call3(APP_CALL_HILINK_OTA_ADAPTER_FLASH_READ, HILINK_OtaAdapterFlashRead, int,
        unsigned int, offset, unsigned char *, buf, unsigned int, bufLen);
    return 0;
}

bool HILINK_OtaAdapterFlashFinish(void)
{
    app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_FINISH, HILINK_OtaAdapterFlashFinish, bool);
    return false;
}

unsigned int HILINK_OtaAdapterFlashMaxSize(void)
{
    app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_MAX_SIZE, HILINK_OtaAdapterFlashMaxSize, unsigned int);
    return 0;
}

void HILINK_OtaAdapterRestart(int flag)
{
    app_call1_ret_void(APP_CALL_HILINK_OTA_ADAPTER_RESTART, HILINK_OtaAdapterRestart, int, flag);
}

int HILINK_OtaStartProcess(int type)
{
    app_call1(APP_CALL_HILINK_OTA_START_PROCESS, HILINK_OtaStartProcess, int, int, type);
    return 0;
}

int HILINK_OtaEndProcess(int status)
{
    app_call1(APP_CALL_HILINK_OTA_END_PROCESS, HILINK_OtaEndProcess, int, int, status);
    return 0;
}

int HILINK_GetRebootFlag(void)
{
    app_call0(APP_CALL_HILINK_GET_REBOOT_FLAG, HILINK_GetRebootFlag, int);
    return 0;
}
