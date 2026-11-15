/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: 设备极速配网wifi管理帧驱动接口适配接口（此文件为DEMO，需集成方适配修改）
 */

#ifndef HILINK_QUICK_NETCFG_ADAPTER_H
#define HILINK_QUICK_NETCFG_ADAPTER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    STA_MODE = 0,  /* wifi sta 模式 */
    AP_MODE = 1,   /* wifi ap 模式 */
} WiFiMode;

/**
 * @brief 开启wifi帧混杂模式，需注册IEEE802.11 frames收包回调函数
 *
 * @return 设置失败返回-1，成功返回0
 */
typedef int (*StartWifiPromisCallback)(void);

/**
 * @brief 关闭wifi帧混杂模式
 *
 * @return 设置失败返回-1，成功返回0
 */
typedef int (*StopWifiPromisCallback)(void);

/**
 * @brief 获取wifi信道
 *
 * @param channel [OUT] wifi信道号
 * @return 成功返回0，失败返回非0
 */
typedef int (*GetWifiChannelCallback)(int *channel);

/**
 * @brief 切换wifi信道
 *
 * @param channel [IN] wifi信道号
 * @return 成功返回0，失败返回非0
 */
typedef int (*SetWifiChannelCallback)(int channel);

/**
 * @brief 切换wifi模式
 *
 * @param mode [IN] 0 sta 模式 1 ap 模式
 * @return 成功返回0，失败返回非0
 */
typedef int (*SetWifiModeCallback)(WiFiMode mode);

/**
 * @brief IEEE802.11 wifi管理帧的发送函数
 *
 * @param data [IN] 发送数据
 * @param len  [IN] 发送数据长度
 * @return 成功返回0，失败返回非0
 */
typedef int (*SendWifiFrameCallback)(const unsigned char *data, unsigned int len);

/**
 * @brief 触发启动扫描wifi信息
 *
 * @return 成功返回0，失败返回非0
 */
typedef int (*ScanfWifiCallback)(void);

typedef struct {
    StartWifiPromisCallback startPromis;
    StopWifiPromisCallback stopPromis;
    GetWifiChannelCallback getChannel;
    SetWifiChannelCallback setChannel; /* provider no need */
    SetWifiModeCallback setWifiMode;   /* provider no need */
    SendWifiFrameCallback sendFrame;
    ScanfWifiCallback scanWifi;
} QuickCfgWifiLoader;

/**
 * @brief 注册设备极速配网wifi管理帧驱动相关接口回调函数
 *
 * @param loader     [IN] 设备极速配网wifi管理帧驱动相关接口回调
 * @param loaderSize [IN] 回调函数结构体大小sizeof(QuickCfgWifiLoader)
 * @return 成功返回0，失败返回非0
 */
int HILINK_SetQuickCfgWifiLoader(QuickCfgWifiLoader *loader, unsigned int loaderSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_QUICK_NETCFG_ADAPTER_H */