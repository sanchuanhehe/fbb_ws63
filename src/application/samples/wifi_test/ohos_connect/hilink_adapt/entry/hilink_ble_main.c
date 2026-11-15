/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: 蓝牙SDK提供demo示例代码（此文件为DEMO，需集成方适配修改）
 */
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cJSON.h"
#include "hilink_bt_api.h"
#include "hilink_bt_function.h"
#include "ble_cfg_net_api.h"
#include "hilink_device.h"
#include "hilink_sal_defines.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_gatt_server.h"
#include "cmsis_os2.h"
#include "hal_reboot.h"
#include "device_profile.h"
#include "hilink_custom.h"
#include "hilink_mem_adapter.h"
#include "hilink_network_adapter.h"
#include "hilink_socket_adapter.h"
#include "hilink_device.h"
#include "wifi_device.h"
#include "securec.h"
#include "hilink_entry.h"

#define min_len(a, b) (((a) < (b)) ? (a) : (b))

#define SWITCH_REPORT   "{\"data\":{\"on\":%d},\"sid\":\"switch\"}"
#define UPDATE_REPORT   "{\"data\":{\"autoUpdate\":%d},\"sid\":\"update\"}"
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
#define CHECKSUM_SID    "checkSum"
#define CHECKSUM_REPORT "{\"data\":{\"checkSum\":\"%s\"},\"sid\":\"checkSum\"}"
#endif

static bool g_autoUpdate = 0;
static HILINK_BT_DevInfo g_btDevInfo = {0};

/*
 * 功能: 获取设备sn号
 * 参数[in]: len sn的最大长度, 39字节
 * 参数[out]: sn 设备sn
 * 注意: sn指针的字符串长度为0时将使用设备mac地址作为sn
 */
void HILINK_GetDeviceSn(unsigned int len, char *sn)
{
    if (sn == NULL) {
        return;
    }
    if (strcpy_s(sn, len, GetMacSn()) != EOK) {
        return;
    }
}

/*
 * 获取设备的子型号，长度固定两个字节
 * subProdId为保存子型号的缓冲区，len为缓冲区的长度
 * 如果产品定义有子型号，则填入两字节子型号，并以'\0'结束, 返回0
 * 没有定义子型号，则返回-1
 * 该接口需设备开发者实现
 */
int HILINK_GetSubProdId(char *subProdId, int len)
{
    if (subProdId == NULL || len <= 0) {
        return -1;
    }
    if (strcpy_s(subProdId, len, SUB_PRODUCT_ID) != EOK) {
        return -1;
    }
    return 0;
}

/*
 * 获取设备表面的最强点信号发射功率强度，最强点位置的确定以及功率测试方
 * 法，参照hilink认证蓝牙靠近发现功率设置及测试方法指导文档，power为出参,
 * 单位dbm，下一次发送广播时生效
 */
int HILINK_BT_GetDevSurfacePower(char *power)
{
    if (power == NULL) {
        return -1;
    }
    *power = ADV_TX_POWER;
    return 0;
}

/* 获取蓝牙SDK设备相关信息 */
HILINK_BT_DevInfo *HILINK_BT_GetDevInfo(void)
{
    HILINK_SAL_NOTICE("HILINK_BT_GetDevInfo\n");
    g_btDevInfo.manuName = MANUAFACTURER;
    g_btDevInfo.devName = DEVICE_TYPE_NAME;
    g_btDevInfo.productId = PRODUCT_ID;
    g_btDevInfo.mac = (char *)GetMacStr();
    g_btDevInfo.model = DEVICE_MODEL;
    g_btDevInfo.devType = DEVICE_TYPE;
    g_btDevInfo.hiv = DEVICE_HIVERSION;
    g_btDevInfo.protType = DEVICE_PROT_TYPE;
    g_btDevInfo.sn = (char *)GetMacSn();
    return &g_btDevInfo;
}
/*
 * 若厂商发送广播类型为BLE_ADV_CUSTOM时才需适配此接口
 * 获取厂商定制信息，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetCustomInfo(char *customInfo, unsigned int len)
{
    if (customInfo == NULL || len == 0) {
        return -1;
    }
    if (strcpy_s(customInfo, len, BT_CUSTOM_INFO) != EOK) {
        return -1;
    }

    return 0;
}

/*
 * 若厂商发送广播类型为BLE_ADV_CUSTOM时才需适配此接口
 * 获取厂家ID，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetManuId(char *manuId, unsigned int len)
{
    if (manuId == NULL || len == 0) {
        return -1;
    }
    if (strcpy_s(manuId, len, DEVICE_MANU_ID) != EOK) {
        return -1;
    }

    return 0;
}

/*
 * 获取蓝牙mac地址，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_BT_GetMacAddr(unsigned char *mac, unsigned int len)
{
    (void)mac;
    (void)len;
    return 0;
}

/* 填写固件、软件和硬件版本号，APP显示版本号以及OTA版本检查与此相关 */
int getDeviceVersion(char* *firmwareVer, char* *softwareVer, char* *hardwareVer)
{
    if (firmwareVer == NULL || softwareVer == NULL || hardwareVer == NULL) {
        return -1;
    }
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    static char fwvWithSwv[64] = "";
    if (sprintf_s(fwvWithSwv, sizeof(fwvWithSwv), "%s_%s", FIRMWARE_VER, SOFTWARE_VER) <= 0) {
        return -1;
    }
    *firmwareVer = fwvWithSwv;
#else
    *firmwareVer = FIRMWARE_VER;
#endif
    *softwareVer = (char *)SOFTWARE_VER;
    *hardwareVer = HARDWARE_VER;
    return 0;
}

static void ReporSwitchStatus(void)
{
    char buff[128] = { 0 };
    int ret = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, SWITCH_REPORT, GetSwitch());
    if (ret <= 0) {
        HILINK_SAL_NOTICE("ReporSwitchStatus snprintf_s ret:%d\r\n", ret);
        return;
    }
    unsigned int buffLen = strlen(buff);
    ret = BLE_SendCustomData(CUSTOM_SEC_DATA, (const unsigned char *)buff, buffLen);
    HILINK_SAL_NOTICE("BLE_SendCustomData:%s len:%u,ret:%d\r\n", buff, buffLen, ret);
}

static void ReporAutoUpdateStatus(void)
{
    char buff[128] = { 0 };
    int ret = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, UPDATE_REPORT, g_autoUpdate);
    if (ret <= 0) {
        HILINK_SAL_NOTICE("ReporAutoUpdateStatus snprintf_s ret:%d\r\n", ret);
        return;
    }
    unsigned int buffLen = strlen(buff);
    ret = BLE_SendCustomData(CUSTOM_SEC_DATA, (const unsigned char *)buff, buffLen);
    HILINK_SAL_NOTICE("BLE_SendCustomData:%s len:%u,ret:%d\r\n", buff, buffLen, ret);
}

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
static void ReporFwvCheckSum(void)
{
    char fwvCheckSum[65] = { 0 };
    unsigned int size = sizeof(fwvCheckSum);
    int ret = read_app_bin_hash(fwvCheckSum, &size);
    if (ret < 0) {
        HILINK_SAL_NOTICE("ReporFwvCheckSum get fwv checksum ret:%d\r\n", ret);
        return;
    }
    char buff[128] = { 0 };
    ret = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, CHECKSUM_REPORT, fwvCheckSum);
    if (ret <= 0) {
        HILINK_SAL_NOTICE("ReporFwvCheckSum snprintf_s ret:%d\r\n", ret);
        return;
    }
    unsigned int buffLen = strlen(buff);
    ret = BLE_SendCustomData(CUSTOM_SEC_DATA, (const unsigned char *)buff, buffLen);
    HILINK_SAL_NOTICE("BLE_SendCustomData:%s len:%u,ret:%d\r\n", buff, buffLen, ret);
}
#endif

static void HILINK_BT_StateChangeHandler(HILINK_BT_SdkStatus event, const void *param)
{
    (void)param;
    /* ble sdk初始化完成后，发送广播让设备被手机发现 */
    if (event == HILINK_BT_SDK_STATUS_SVC_RUNNING) {
        /*
         * 由于 sle 与 ble 协议栈中的 mac 会在使能协议栈之后恢复默认，
         * 当前海思未释放星闪 hilink 适配层代码，所以放在此处设置 mac，
         * 保证能在使能之后起广播前能设置正确的 mac 到协议栈
         */
        if (SetBleAndSleAddrToStackFromNv() != 0) {
            HILINK_SAL_ERROR("set addr err\n");
        }
        /* 设置蓝牙广播格式，包括靠近发现、碰一碰等，下一次发送广播生效 */
        BLE_SetAdvType(BLE_ADV_NEARBY_V0);

        /* BLE配网广播控制：参数代表广播时间，0:停止；0xFFFFFFFF:一直广播，其他：广播指定时间后停止，单位秒 */
        (void)BLE_CfgNetAdvCtrl(BLE_ADV_TIME);
    }
}

static BLE_ConfPara g_isBlePair = {
    .isBlePair = 0,
};

static BLE_InitPara g_bleInitParam = {
    .confPara = &g_isBlePair,
    /* advInfo为空表示使用ble sdk默认广播参数及数据 */
    .advInfo  = NULL,
    .gattList = NULL,
};

static int BleHandleCustomDataGet(const char *sid)
{
    HILINK_SAL_NOTICE("get %s svc\r\n", sid);
    if (strcmp(sid, "switch") == 0) {
        ReporSwitchStatus();
        return 0;
    } else if (strcmp(sid, "update") == 0) {
        ReporAutoUpdateStatus();
        return 0;
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    } else if (strcmp(sid, CHECKSUM_SID) == 0) {
        ReporFwvCheckSum();
        return 0;
#endif
    }
    return -1;
}

// 参考链接 https://device.harmonyos.com/cn/docs/devicepartner/DevicePartner-Guides/device-development-ble-specifications-control-0000001482432154
static int BleHandleCustomData(const char *buff, unsigned int len)
{
    if (strcmp((char *)buff, "{}") == 0) {
        /* 上报全量数据 */
        ReporSwitchStatus();
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
        ReporFwvCheckSum();
#endif
        return 0;
    }
    int ret = -1;

    cJSON *json = cJSON_Parse(buff);
    if (json == NULL) {
        return ret;
    }

    do {
        cJSON *sidItem = cJSON_GetObjectItem(json, "sid");
        cJSON *dataItem = cJSON_GetObjectItem(json, "data");
        if (sidItem == NULL || !cJSON_IsString(sidItem) || (sidItem->valuestring == NULL)) {
            break;
        }

        if (dataItem == NULL) {
            ret = BleHandleCustomDataGet(sidItem->valuestring);
            break;
        }

        if (strcmp(sidItem->valuestring, "allservices") == 0) {
            /* H5 连接上时会给设备发送allservices， 设备需要给H5同步全量状态 */
            HILINK_SAL_NOTICE("sync dev status\r\n");
            ReporSwitchStatus();
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
            ReporFwvCheckSum();
#endif
            ret = 0;
            break;
        } else if (strcmp(sidItem->valuestring, "currentTime") == 0) {
            // 参考格式 {"sid":"currentTime", "data":{"currentTime":"1730692041"}}
            cJSON *item = cJSON_GetObjectItem(dataItem, "currentTime");
            if (item != NULL) {
                HILINK_SAL_NOTICE("sync time %d\r\n", item->valueint);
            }
            ret = 0;
            break;
        } else if (strcmp(sidItem->valuestring, "switch") == 0) {
            // 参考格式 {"sid":"switch", "data":{"on":1}}
            cJSON *item = cJSON_GetObjectItem(dataItem, "on");
            if (item != NULL) {
                HILINK_SAL_NOTICE("DEMO: Hilink receive a switch cmd, on = %d\r\n", item->valueint);
                SetSwitch(item->valueint);
            }
            ReporSwitchStatus();
            ret = 0;
            break;
        } else if (strcmp(sidItem->valuestring, "update") == 0) {
            // 参考格式 {"sid":"update", "data":{"autoUpdateOn":1}}
            cJSON *item = cJSON_GetObjectItem(dataItem, "autoUpdateOn");
            if (item != NULL) {
                g_autoUpdate = item->valueint != 0 ? true : false;
                HILINK_SAL_NOTICE("DEMO: Hilink receive a update cmd, on = %d\r\n", g_autoUpdate);
            }
            ReporAutoUpdateStatus();
            ret = 0;
            break;
        }
    } while (0);

    cJSON_Delete(json);
    return ret;
}

/* APP下发自定义指令时调用此函数，需处理自定义数据，返回0表示处理成功 */
static int BleRcvCustomData(unsigned char *buff, unsigned int len)
{
    if (buff == NULL || len == 0) {
        HILINK_SAL_NOTICE("buff is NULL\r\n");
        return -1;
    }
    HILINK_SAL_NOTICE("custom data, len: %u, data: %s", len, buff);

    /* 处理自定义数据 */
    if (BleHandleCustomData((const char *)buff, len) != 0) {
        HILINK_SAL_NOTICE("BleHandleCustomData fail");
        return -1;
    }

    return 0;
}

int CfgNetProcess(BLE_CfgNetStatus status)
{
    HILINK_SAL_NOTICE("status: %d", status);
    if (status == CFG_NET_BLE_DIS_CONNECT) {
        BLE_CfgNetAdvUpdate(NULL);
    }
    return 0;
}

static BLE_CfgNetCb g_bleCfgNetCb = {
    .rcvCustomDataCb = BleRcvCustomData,
    .cfgNetProcessCb = CfgNetProcess,
};

static int HardReboot(void)
{
    hal_reboot_chip();
    return 0;
}

unsigned int GetWifiRecoveryType(void)
{
    return (0x01 | 0x02);
}

int hilink_ble_main(void)
{
    int ret = 0;

    HILINK_SetProtType(DEVICE_PROT_TYPE);
    ret = HILINK_SetNetConfigMode(HILINK_NETCONFIG_OTHER);
    if (ret != 0) {
        HILINK_SAL_NOTICE("SetNetConfigMode failed\r\n");
        return -1;
    }

    /* 设备按需设置，例如接入蓝牙网关时，设置广播类型标志及心跳间隔 */
    unsigned char mpp[] = { 0x02, 0x3c, 0x00 };
    ret = BLE_SetAdvNameMpp(mpp, sizeof(mpp));
    if (ret != 0) {
        HILINK_SAL_NOTICE("set adv name mpp failed\r\n");
        return -1;
    }

    /* 注册SDK状态接收函数，可在初始化完成后发送广播 */
    ret = HILINK_BT_SetSdkEventCallback(HILINK_BT_StateChangeHandler);
    if (ret != 0) {
        HILINK_SAL_NOTICE("set event callback failed\r\n");
        return -1;
    }

    /* 设置广播方式为靠近发现 */
    BLE_SetAdvType(BLE_ADV_NEARBY_V0);

    /* 初始化ble sdk */
    ret = BLE_CfgNetInit(&g_bleInitParam, &g_bleCfgNetCb);
    if (ret != 0) {
        HILINK_SAL_NOTICE("ble sdk init fail\r\n");
        return -1;
    }

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        HILINK_SAL_NOTICE("sdkAttr is null");
        return -1;
    }
    sdkAttr->monitorTaskStackSize = 0x600;  /* 示例代码 推荐栈大小为0x400 */
    sdkAttr->rebootHardware = HardReboot;
    sdkAttr->rebootSoftware = HardReboot;
    HILINK_SetSdkAttr(*sdkAttr);

    /* 注册极简配网WIFI相关函数 */
    WiFiRecoveryApi wifiCb;
    wifiCb.getWifiRecoveryType = GetWifiRecoveryType;
    wifiCb.scanAP = HILINK_ScanAP;
    wifiCb.getAPScanResult = HILINK_GetAPScanResult;
    wifiCb.restartWiFi = HILINK_RestartWiFi;
    wifiCb.connectWiFiByBssid = HILINK_ConnectWiFiByBssid;
    wifiCb.lastConnResult = HILINK_GetLastConnectResult;
    if (HILINK_RegWiFiRecoveryCallback((const WiFiRecoveryApi *)&wifiCb, sizeof(wifiCb)) != 0) {
        HILINK_SAL_NOTICE("reg wifi recovery failed!\r\n");
        return -1;
    }
    HILINK_SAL_NOTICE("RegWiFiRecoveryCallback finish!\n");

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    if (HILINK_RegisterErrnoCallback(get_os_errno) != 0) {
        HILINK_SAL_NOTICE("reg errno cb err\r\n");
        return -1;
    }
#endif

    /* 启动Hilink SDK */
    if (HILINK_Main() != 0) {
        HILINK_SAL_NOTICE("HILINK_Main start error");
        return -1;
    }

    return 0;
}