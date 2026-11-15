/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink产品基本信息
 */
#ifndef DEVICE_PROFILE_H
#define DEVICE_PROFILE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 设备基本信息根据设备实际情况填写
 * 与hilink sdk相同定义，双模组模式只需一份，已提供给第三方厂家，暂不按编程规范整改
 */
#define MINIMALIST_MODE 0 // 默认配网方式：极简配网
#define SOFTAP_MODE 1 // SOFTAP配网
#define QUICK_NETCFG_MODE 2 // 蓝牙辅助配网
#define BLE_ANCILLAY_MODE 3 // 快速配网

#define PRODUCT_ID                  GetProId()
#define DEVICE_MODEL                GetDevModel()
#define DEVICE_TYPE                 GetDevType()
#define MANUAFACTURER               GetManuaFac()
#define DEVICE_PROT_TYPE            GetDevProType()
#define DEVICE_TYPE_NAME            GetDevTypeName()
#define MANUAFACTURER_NAME          GetManuaName()

/* 设备固件版本号 */
#define FIRMWARE_VER "1.0.0"
/* 设备硬件版本号 */
#define HARDWARE_VER "1.0.0"
/* 设备软件版本号 */
#define SOFTWARE_VER HILINK_GetSdkVersion()
#define DEVICE_HIVERSION "1.0.0"

/* 蓝牙sdk单独使用的定义 */
#define SUB_PRODUCT_ID "00"
#define ADV_TX_POWER 0xF8
#define BLE_ADV_TIME 0xFFFFFFFF

/* 厂商自定义蓝牙广播，设备型号信息 */
#define BT_CUSTOM_INFO "12345678"
#define DEVICE_MANU_ID "017"

char *GetDevType(void);
char *GetDevModel(void);
char *GetProId(void);
char *GetDevTypeName(void);
char *GetManuaFac(void);
char *GetManuaName(void);
int GetDevProType(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DEVICE_PROFILE_H */