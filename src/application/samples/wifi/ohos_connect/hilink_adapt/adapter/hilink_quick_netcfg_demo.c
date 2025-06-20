/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: 设备通过80211 wifi管理帧实现文件demo
 */

#ifdef SUPPORT_QUICK_NETCFG

#include "hilink_quick_netcfg_demo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "soc_wifi_api.h"
#include "hilink.h"
#include "hilink_custom.h"
#include "hilink_quick_netcfg_api.h"
#include "hilink_quick_netcfg_adapter.h"
#include "hilink_thread_adapter.h"
#include "hilink_softap_adapter.h"
#include "hilink_str_adapter.h"
#include "hilink_network_adapter.h"
#include "wifi_device_config.h"
#include "securec.h"
#include "nv_common_cfg.h"
#include "nv.h"
#include "hilink_sal_defines.h"
#include "hilink_stdio_adapter.h"

#define QUICK_NETCFG_TASK_SIZE 4096
#define CMD_MAX_LEN 2048
#define QUICK_CFG_DELEAY 1500
#define CHAN_DEFAULT 6
#define SCAN_WIFI_SSID "TestScanAp"

static osal_char g_ifName[WIFI_IFNAME_MAX_SIZE + 1] = "ap0";
static void *g_quickCfgTask = NULL;
static bool g_isRecvReginfo = false;

#define BATCH_CFG    "batchCfg"
#define BATCH_CFG_PRINTF(fmt, args...) HILINK_Printf("[%s][%s:%d] " fmt "\n", \
                                           BATCH_CFG, __FUNCTION__, __LINE__, ##args)

static bool IsWifiExsit(void)
{
    hilink_connect_info_t result = {0};

    uint16_t result_len = 0;
    uint32_t nv_ret = ERRCODE_FAIL;
    nv_ret = uapi_nv_read(NV_ID_HILINK_CONNECT_INFO, sizeof(hilink_connect_info_t), &result_len,
        (uint8_t *)&result);
    if (nv_ret != ERRCODE_SUCC) {
        BATCH_CFG_PRINTF("read hilink connect info  from nv failed");
        return false;
    }
    if ((result.ssid[0] != 0) && (strlen((char *)result.ssid) > 0) &&
        (result.pwd[0] != 0) && (strlen((char *)result.pwd) > 0)) {
        return true;
    }

    return false;
}

static int SetRoutName(WiFiMode mode)
{
    (void)memset_s(g_ifName, sizeof(g_ifName), 0, sizeof(g_ifName));
    if (mode == STA_MODE) {
        if (memcpy_s(g_ifName, sizeof(g_ifName), "wlan0", strlen("wlan0")) != 0) {
            return -1;
        }
    } else if (mode == AP_MODE) {
        if (memcpy_s(g_ifName, sizeof(g_ifName), "ap0", strlen("ap0")) != 0) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

/*
 * 功能：开启Wifi混杂模式时注册此函数
 * 参数: frame: 802.11 frame，len：长度
 * 返回：失败返回-1，成功返回0
 */
static int QuickCfgReceiveFrame(void* frame, int len, signed char rssi)
{
    (void)rssi;
    return HILINK_FrameParse((const unsigned char *)frame, (unsigned int)len);
}

/*
 * 功能：开启wifi帧混杂模式，需注册IEEE 802.11 frames收包回调函数
 * 返回：成功返回0，失败返回非零
 */
static int QuickCfgStartPromisMode(void)
{
    if (wifi_is_sta_enabled() == 1) {
        BATCH_CFG_PRINTF("SetRoutName STA_MODE");
        SetRoutName(STA_MODE);
    } else {
        BATCH_CFG_PRINTF("SetRoutName AP_MODE");
        SetRoutName(AP_MODE);
    }

    ext_wifi_ptype_filter_stru filter = {0};
    filter.mdata_en = 0;
    filter.udata_en = 0;
    filter.mmngt_en = 1;  // 开启管理帧接收
    filter.umngt_en = 1;  // 开启管理帧接收
    filter.custom_en = 0;
    uapi_wifi_promis_set_rx_callback(QuickCfgReceiveFrame);
    int ret = uapi_wifi_promis_enable(g_ifName, 1, &filter);
    if (ret != 0) {
        BATCH_CFG_PRINTF("hi_wifi_promis_enable: set error! ifName: %s, ret: %d", g_ifName, ret);
        return ret;
    }
    BATCH_CFG_PRINTF("Start Promis Mode success, ifName: %s", g_ifName);
    return 0;
}

/* 关闭wifi帧混杂模式 */
static int QuickCfgStopPromisMode(void)
{
    ext_wifi_ptype_filter_stru filter = {0};
    BATCH_CFG_PRINTF("QuickCfgStopPromisMode:: g_ifName: %s", g_ifName);
    int ret = uapi_wifi_promis_enable(g_ifName, 0, &filter);
    if (ret != 0) {
        BATCH_CFG_PRINTF("hi_wifi_promis_disable:: set error. ret: %d", ret);
        return ret;
    }
    BATCH_CFG_PRINTF("Stop Promis Mode success, ifName: %s", g_ifName);
    return 0;
}

/* 切换wifi模式 */
static int QuickCfgSetWifiMode(WiFiMode mode)
{
    int ret;
    BATCH_CFG_PRINTF("[SetWifiMode] set mode=%u\n", mode);
    if (mode == STA_MODE) {
        HILINK_StopSoftAp();
        memset_s(g_ifName, sizeof(g_ifName), 0, sizeof(g_ifName));
        int len = sizeof(g_ifName);
        ret = uapi_wifi_sta_start(g_ifName, &len);
        if (ret != EXT_WIFI_OK) {
            BATCH_CFG_PRINTF("hi_wifi_sta_start fail, ret=%d\n", ret);
            return -1;
        }
    } else {
        if (!IsWifiExsit()) {
            ret = uapi_wifi_sta_stop();
            if (ret != EXT_WIFI_OK) {
                BATCH_CFG_PRINTF("hi_wifi_sta_stop fail, ret=%d\n", ret);
                return ret;
            }
            ret = HILINK_SetSoftAPMode();
            if (ret != 0) {
                BATCH_CFG_PRINTF("Start SoftAP fail, ret=%d\n", ret);
                return ret;
            }
        }
    }
    return 0;
}

/* 切换wifi信道（平台相关） */
static int QuickCfgWiFiSetChannel(int chan)
{
    return uapi_wifi_set_channel(g_ifName, WIFI_IFNAME_MAX_SIZE + 1, chan);
}

/* 获取wifi信道（平台相关） */
static int QuickCfgWiFiGetChannel(int *chan)
{
    if (chan == NULL) {
        return -1;
    }

    *chan = uapi_wifi_get_channel(g_ifName, WIFI_IFNAME_MAX_SIZE + 1);
    return 0;
}

/* IEEE 802.11 frames的发送函数 */
static int QuickCfgSendFrame(const unsigned char *buff, unsigned int len)
{
    return uapi_wifi_send_custom_pkt(g_ifName, buff, len);
}

/* 获取设备mac地址 */
static int QuickCfgGetDevMac(unsigned char *mac, unsigned int len)
{
    return HILINK_GetMacAddr(mac, len);
}

/*
 * 设置配网信息, 作为设备端必须设置，配置端不需要
 * 返回0表示设置成功，其他表示设置失败(-2表示HiLink未处于接收配网数据状态)
 */
static int QuickCfgSetNetConfigInfo(const char *info)
{
    if (info == NULL) {
        BATCH_CFG_PRINTF("[QuickCfgSetNetConfigInfo]Invalid param\n");
        return -1;
    }

    int ret = uapi_wifi_sta_stop();
    if (ret != EXT_WIFI_OK) {
        BATCH_CFG_PRINTF("[QuickCfgSetNetConfigInfo]hi_wifi_sta_stop fail, ret=%d\n", ret);
    }
    ret = HILINK_SetNetConfigInfo(info);
    if (ret != 0) {
        BATCH_CFG_PRINTF("[QuickCfgSetNetConfigInfo]Set NetConfig Info fail, ret=%d\n", ret);
        return ret;
    }

    g_isRecvReginfo = true;
    return 0;
}

/* 获取设备wifi信息 */
static int QuickCfgGetWifiInfo(char *payload, unsigned int len)
{
    hilink_connect_info_t result = {0};
    uint16_t result_len = 0;
    uint32_t nv_ret = ERRCODE_FAIL;
    nv_ret = uapi_nv_read(NV_ID_HILINK_CONNECT_INFO, sizeof(hilink_connect_info_t), &result_len,
        (uint8_t *)&result);
    if (nv_ret != ERRCODE_SUCC) {
        BATCH_CFG_PRINTF("read hilink connect info  from nv failed");
        return -1;
    }

    BATCH_CFG_PRINTF("[QuickCfgGetWifiInfo]ssid: %s, pwd: %s. ret=%d\n", result.ssid, result.pwd);
    int ret = sprintf_s(payload, len, "{\"ssid\":\"%s\",\"pwd\":\"%s\"}", result.ssid, result.pwd);
    if (ret <= 0) {
        BATCH_CFG_PRINTF("[QuickCfgGetWifiInfo]sBATCH_CFG_PRINTF_s fail, ret=%d\n", ret);
        return -1;
    }
    return 0;
}

/* 启动扫描wifi信息 */
static int QuickCfgScanWifi(void)
{
    HILINK_APScanParam param;
    (void)memset_s(&param, sizeof(HILINK_APScanParam), 0, sizeof(HILINK_APScanParam));
    if (strcpy_s(param.ssid, sizeof(param.ssid), SCAN_WIFI_SSID) != 0) {
        BATCH_CFG_PRINTF("[QuickCfgScanWifi]copy ssid failed");
        return -1;
    }
    param.ssidLen = HILINK_Strlen(SCAN_WIFI_SSID);
    int ret = HILINK_ScanAP(&param);
    if (ret != 0) {
        BATCH_CFG_PRINTF("[QuickCfgScanWifi]wifi scan failed, ret=%d", ret);
        return -1;
    }
    return 0;
}

static void QuickCfgProcess(void *arg)
{
    /* 批量配网注册wifi驱动相关接口 */
    QuickCfgWifiLoader wifiLoader = {
        QuickCfgStartPromisMode,
        QuickCfgStopPromisMode,
        QuickCfgWiFiGetChannel,
        QuickCfgWiFiSetChannel,
        QuickCfgSetWifiMode,
        QuickCfgSendFrame,
        QuickCfgScanWifi,
    };
    HILINK_SetQuickCfgWifiLoader(&wifiLoader, sizeof(QuickCfgWifiLoader));

    /* 批量配网注册通用相关接口 */
    QuickCfgCommonLoader commonLoader = {
        DEVICE_TYPE,
        CMD_MAX_LEN,
        DISABLE_SET_CHAN,
        QuickCfgGetDevMac,
        QuickCfgGetWifiInfo,
        NULL,
        QuickCfgSetNetConfigInfo,
        HILINK_RequestRegInfo,
    };
    HILINK_SetQuickCfgCommonLoader(&commonLoader, sizeof(QuickCfgCommonLoader));

    /* 延时等待HILINK_Main完全启动后启动批量配网 */
    HILINK_MilliSleep(QUICK_CFG_DELEAY);
    /* 如果设备已注册不启动批量配网 */
    if (HILINK_IsRegister() != 0 || IsWifiExsit()) {
        return;
    }

    HILINK_StartQuickCfg();
}

/*
 * 启动批量配网任务
 * 注意：为兼容softap配网在HILINK_Main启动后再启动次任务
 */
int StartQuickNetCfg(void)
{
    HiLinkTaskParam taskParam = {
        (HiLinkTaskEntryFunc)QuickCfgProcess,
        HILINK_TASK_PRIORITY_MID,
        QUICK_NETCFG_TASK_SIZE,
        NULL,
        "QuickCfgTask",
    };
    g_quickCfgTask = HILINK_CreateTask(&taskParam);
    if (g_quickCfgTask == NULL) {
        BATCH_CFG_PRINTF("[StartQuickNetCfg]Create TestWifiProcess task failed");
        return -1;
    }
    BATCH_CFG_PRINTF("[StartQuickNetCfg]Create g_quickCfgTask ok");
    return 0;
}
#endif