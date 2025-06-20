/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the ble main, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_bt_api.h"

void HILINK_GetDeviceSn(unsigned int len, char *sn)
{
    app_call2_ret_void(APP_CALL_HILINK_GET_DEVICE_SN, HILINK_GetDeviceSn, unsigned int, len, char *, sn);
}

int HILINK_GetSubProdId(char *subProdId, int len)
{
    app_call2(APP_CALL_HILINK_GET_SUB_PROD_ID, HILINK_GetSubProdId, int, char *, subProdId, int, len);
    return 0;
}

int HILINK_BT_GetDevSurfacePower(char *power)
{
    app_call1(APP_CALL_HILINK_BT_GET_DEV_SURFACE_POWER, HILINK_BT_GetDevSurfacePower, int, char *, power);
    return 0;
}

HILINK_BT_DevInfo *HILINK_BT_GetDevInfo(void)
{
    app_call0(APP_CALL_HILINK_BT_GET_DEV_INFO, HILINK_BT_GetDevInfo, HILINK_BT_DevInfo *);
    return NULL;
}

int HILINK_GetCustomInfo(char *customInfo, unsigned int len)
{
    app_call2(APP_CALL_HILINK_GET_CUSTOM_INFO, HILINK_GetCustomInfo, int, char *, customInfo, unsigned int, len);
    return 0;
}

int HILINK_GetManuId(char *manuId, unsigned int len)
{
    app_call2(APP_CALL_HILINK_GET_MANU_ID, HILINK_GetManuId, int, char *, manuId, unsigned int, len);
    return 0;
}

int HILINK_BT_GetMacAddr(unsigned char *mac, unsigned int len)
{
    app_call2(APP_CALL_HILINK_BT_GET_MAC_ADDR, HILINK_BT_GetMacAddr, int, unsigned char *, mac, unsigned int, len);
    return 0;
}

int getDeviceVersion(char* *firmwareVer, char* *softwareVer, char* *hardwareVer)
{
    app_call3(APP_CALL_GET_DEVICE_VERSION, getDeviceVersion, int, char* *, firmwareVer,
        char* *, softwareVer, char* *, hardwareVer);
    return 0;
}