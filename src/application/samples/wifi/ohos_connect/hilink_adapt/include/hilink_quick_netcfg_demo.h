/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: 设备通过80211 wifi管理帧实现demo头文件
 */

#ifndef QUICK_NETCFG_DEMO_H
#define QUICK_NETCFG_DEMO_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 启动批量配网任务
 * 注意：为兼容softap配网在HILINK_Main启动后再启动次任务
 */
int StartQuickNetCfg(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* QUICK_NETCFG_DEMO_H */
