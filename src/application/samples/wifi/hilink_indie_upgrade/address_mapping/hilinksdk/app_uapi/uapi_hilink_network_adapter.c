/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Network adaptation implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_GetLocalIp(char *localIp, unsigned char len)
{
    app_call2(APP_CALL_HILINK_GET_LOCAL_IP, HILINK_GetLocalIp, int, char *, localIp, unsigned char, len);
    return 0;
}

int HILINK_GetMacAddr(unsigned char *mac, unsigned char len)
{
    app_call2(APP_CALL_HILINK_GET_MAC_ADDR, HILINK_GetMacAddr, int, unsigned char *, mac, unsigned char, len);
    return 0;
}

int HILINK_GetWiFiSsid(char *ssid, unsigned int *ssidLen)
{
    app_call2(APP_CALL_HILINK_GET_WI_FI_SSID, HILINK_GetWiFiSsid, int, char *, ssid, unsigned int *, ssidLen);
    return 0;
}

int HILINK_SetWiFiInfo(const char *ssid, unsigned int ssidLen, const char *pwd, unsigned int pwdLen)
{
    app_call4(APP_CALL_HILINK_SET_WI_FI_INFO, HILINK_SetWiFiInfo, int,
        const char *, ssid, unsigned int, ssidLen, const char *, pwd, unsigned int, pwdLen);
    return 0;
}

void HILINK_ReconnectWiFi(void)
{
    app_call0_ret_void(APP_CALL_HILINK_RECONNECT_WI_FI, HILINK_ReconnectWiFi);
}

int HILINK_ConnectWiFi(void)
{
    app_call0(APP_CALL_HILINK_CONNECT_WI_FI, HILINK_ConnectWiFi, int);
    return 0;
}

int HILINK_GetNetworkState(int *state)
{
    app_call1(APP_CALL_HILINK_GET_NETWORK_STATE, HILINK_GetNetworkState, int, int *, state);
    return 0;
}

int HILINK_GetWiFiBssid(unsigned char *bssid, unsigned char *bssidLen)
{
    app_call2(APP_CALL_HILINK_GET_WI_FI_BSSID, HILINK_GetWiFiBssid, int,
        unsigned char *, bssid, unsigned char *, bssidLen);
    return 0;
}

int HILINK_GetWiFiRssi(signed char *rssi)
{
    app_call1(APP_CALL_HILINK_GET_WI_FI_RSSI, HILINK_GetWiFiRssi, int, signed char *, rssi);
    return 0;
}