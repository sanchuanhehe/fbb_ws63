/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: wifi管理帧极速配网对外通用接口api头文件
 */

#ifndef HILINK_QUICK_NETCFG_API_H
#define HILINK_QUICK_NETCFG_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DEVICE_TYPE = 0,     /* 待配设备 */
    PROVIDER_TYPE = 1,   /* 主配设备 */
} DevType;

typedef enum {
    DISABLE_SET_CHAN = 0,
    ENABLE_SET_CHAN = 1,
} SetChanSwitch;

/**
 * @brief 获取设备mac地址
 *
 * @param mac [OUT] 设备mac地址
 * @param len [IN] 设备mac地址长度
 * @return 成功返回0，失败返回非0
 */
typedef int (*GetDevMacCallback)(unsigned char *mac, unsigned int len);

/**
 * @brief 生成设备配网注册信息(中枢提供)
 *
 * @param payload [OUT] 返回payload数据，json格式如下
 {
    "devId":"xxxx",
    "psk":"xxxx",
    "activeCode":"xxxx",
    "url":"xxxx",
 }
 * @param len     [IN] payload数据长度
 * @return 成功返回0，失败返回非0
 */
typedef int (*GenRegInfoCallback)(char *payload, unsigned int len);

/**
 * @brief 设置配网信息, 作为设备端必须设置，主配设备端不需要
 *
 * @param info [OUT] 配网信息数据
 * @return 返回0表示设置成功，其他表示设置失败(-2表示HiLink未处于接收配网数据状态)
 */
typedef int (*SetNetConfigInfo)(const char *info);

/**
 * @brief 获取连接路由信息(ssid、pwd)(中枢提供)
 *
 * @param payload [OUT] 设备连接路由信息json格式如下
 {
    "ssid":"xxxx",
    "pwd":"xxxx",
 }
 * @param len     [IN] payload数据长度
 * @return 返回：设置失败返回-1，成功返回0
 */
typedef int (*GetWifiInfo)(char *payload, unsigned int len);

/**
 * @brief 当设备配置为主配设备时，通过此接口向（中枢或云）申请设备注册信息，
          异步接口，通过HILINK_QuickCfgCmdParse接口返回数据
 *
 * @param regNum [IN] 表示要申请的配网注册的数量
 * @return 0表示设置成功，其他表示设置失败
 */
typedef int (*RequestRegInfo)(unsigned int regNum);

typedef struct {
    int type;                       /* 0 device 1 provider */
    int cmdMaxLen;                  /* 主配设备与app报文交互最大长度 */
    SetChanSwitch enable;           /* 开启：主配设备在触发其他wifi设备配网时会切换到其他wifi设备信道发送数据 */
    GetDevMacCallback getDevMac;
    GetWifiInfo getWifiInfo;
    GenRegInfoCallback genRegInfo; /* device no need */
    SetNetConfigInfo setNetInfo;   /* provider no need */
    RequestRegInfo reqRegInfo;     /* device no need */
} QuickCfgCommonLoader;

/**
 * @brief 注册设备极速配网通用接口回调函数
 *
 * @param loader     [IN] 设备极速配网通用接口回调
 * @param loaderSize [IN] 回调函数结构体大小sizeof(QuickCfgCommonLoader)
 * @return 设置失败返回-1，成功返回0
 */
int HILINK_SetQuickCfgCommonLoader(QuickCfgCommonLoader *loader, unsigned int loaderSize);

/**
 * @brief 启动极速配网
 *
 * @return 设置失败返回-1，成功返回0
 */
int HILINK_StartQuickCfg(void);

/**
 * @brief 开启Wifi混杂模式时注册此函数
 *
 * @param frame [IN] 管理帧数据
 * @param len   [IN] 管理帧数据长度
 * @return 设置失败返回-1，成功返回0
 * @attention 报文要求: 支持收802.11 标准MAC帧，包含数据帧(组播、广播)和管理帧(beacon、 probe request)等
 */
int HILINK_FrameParse(const unsigned char *frame, unsigned int len);

/**
 * @brief 解析APP/中枢/云下发的极速配网数据报文
 *
 * @param payload [IN] 配置json数据
 * @param len   [IN] json数据长度
 * @return 设置失败返回-1，成功返回0
 */
int HILINK_QuickCfgCmdParse(const char *payload, unsigned int len);

/**
 * @brief 切换配网设备模式
 *
 * @param type [IN] type 0:待配设备 1：主配设备
 * @return 设置失败返回-1，成功返回0
 */
int HILINK_SetDeviceType(DevType type);

/**
 * @brief 获取主配设备与app报文交互最大长度
 *
 * @return 设置失败返回-1，成功返回最大长度
 */
int HILINK_GetCmdMaxLen(void);
void HILINK_EnableQuickNetCfg(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_QUICK_NETCFG_API_H */