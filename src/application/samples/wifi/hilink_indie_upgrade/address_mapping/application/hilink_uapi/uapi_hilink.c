  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: HiLink function adaption \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "hilink_call.h"
#include "hilink.h"

int HILINK_RegisterBaseCallback(const HiLinkBaseCallback *cb, unsigned int cbSize)
{
    hilink_call2(HILINK_CALL_HILINK_REGISTER_BASE_CALLBACK, HILINK_RegisterBaseCallback, int,
        const HiLinkBaseCallback *, cb, unsigned int, cbSize);
    return 0;
}

int HILINK_Main(void)
{
    hilink_call0(HILINK_CALL_HILINK_MAIN, HILINK_Main, int);
    return 0;
}

void HILINK_Reset(void)
{
    hilink_call0_ret_void(HILINK_CALL_HILINK_RESET, HILINK_Reset);
}

int HILINK_SetSdkAttr(HILINK_SdkAttr sdkAttr)
{
    hilink_call1(HILINK_CALL_HILINK_SET_SDK_ATTR, HILINK_SetSdkAttr, int, HILINK_SdkAttr, sdkAttr);
    return 0;
}

HILINK_SdkAttr *HILINK_GetSdkAttr(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_SDK_ATTR, HILINK_GetSdkAttr, HILINK_SdkAttr *);
    return NULL;
}

int HILINK_RestoreFactorySettings(void)
{
    hilink_call0(HILINK_CALL_HILINK_RESTORE_FACTORY_SETTINGS, HILINK_RestoreFactorySettings, int);
    return 0;
}

int HILINK_GetDevStatus(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_DEV_STATUS, HILINK_GetDevStatus, int);
    return 0;
}

const char *HILINK_GetSdkVersion(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_SDK_VERSION, HILINK_GetSdkVersion, const char *);
    return NULL;
}

int HILINK_ReportCharState(const char *svcId, const char *payload, unsigned int len)
{
    hilink_call3(HILINK_CALL_HILINK_REPORT_CHAR_STATE, HILINK_ReportCharState, int,
        const char *, svcId, const char *, payload, unsigned int, len);
    return 0;
}

int HILINK_IsRegister(void)
{
    hilink_call0(HILINK_CALL_HILINK_IS_REGISTER, HILINK_IsRegister, int);
    return 0;
}

int HILINK_GetNetworkingMode(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_NETWORKING_MODE, HILINK_GetNetworkingMode, int);
    return 0;
}

int HILINK_GetRegisterStatus(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_REGISTER_STATUS, HILINK_GetRegisterStatus, int);
    return 0;
}

int HILINK_SetScheduleInterval(unsigned long interval)
{
    hilink_call1(HILINK_CALL_HILINK_SET_SCHEDULE_INTERVAL, HILINK_SetScheduleInterval, int, unsigned long, interval);
    return 0;
}

int HILINK_SetMonitorScheduleInterval(unsigned long interval)
{
    hilink_call1(HILINK_CALL_HILINK_SET_MONITOR_SCHEDULE_INTERVAL, HILINK_SetMonitorScheduleInterval, int,
        unsigned long, interval);
    return 0;
}

int HILINK_SetNetConfigMode(enum HILINK_NetConfigMode netConfigMode)
{
    hilink_call1(HILINK_CALL_HILINK_SET_NET_CONFIG_MODE, HILINK_SetNetConfigMode, int,
        enum HILINK_NetConfigMode, netConfigMode);
    return 0;
}

enum HILINK_NetConfigMode HILINK_GetNetConfigMode(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_NET_CONFIG_MODE, HILINK_GetNetConfigMode, enum HILINK_NetConfigMode);
    return HILINK_NETCONFIG_BUTT;
}

void HILINK_SetNetConfigTimeout(unsigned long netConfigTimeout)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_SET_NET_CONFIG_TIMEOUT, HILINK_SetNetConfigTimeout,
        unsigned long, netConfigTimeout);
}

int HILINK_SetOtaBootTime(unsigned int bootTime)
{
    hilink_call1(HILINK_CALL_HILINK_SET_OTA_BOOT_TIME, HILINK_SetOtaBootTime, int, unsigned int, bootTime);
    return 0;
}

void HILINK_EnableKitframework(void)
{
    hilink_call0_ret_void(HILINK_CALL_HILINK_ENABLE_KITFRAMEWORK, HILINK_EnableKitframework);
}

void HILINK_EnableBatchControl(bool flag)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_ENABLE_BATCH_CONTROL, HILINK_EnableBatchControl, bool, flag);
}

void HILINK_EnableProcessDelErrCode(int enable)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_ENABLE_PROCESS_DEL_ERR_CODE, HILINK_EnableProcessDelErrCode, int, enable);
}

void HILINK_UnbindDevice(int type)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_UNBIND_DEVICE, HILINK_UnbindDevice, int, type);
}

int HILINK_SetDeviceInstallType(int type)
{
    hilink_call1(HILINK_CALL_HILINK_SET_DEVICE_INSTALL_TYPE, HILINK_SetDeviceInstallType, int, int, type);
    return 0;
}

SetupType HILINK_GetDevSetupType(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_DEV_SETUP_TYPE, HILINK_GetDevSetupType, SetupType);
    return SETUP_TYPE_UNREGISTER;
}

void HILINK_EnableDevIdInherit(bool isEnbale)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_ENABLE_DEV_ID_INHERIT, HILINK_EnableDevIdInherit, bool, isEnbale);
}

void HILINK_NotifyNetworkAvailable(bool status)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_NOTIFY_NETWORK_AVAILABLE, HILINK_NotifyNetworkAvailable, bool, status);
}
