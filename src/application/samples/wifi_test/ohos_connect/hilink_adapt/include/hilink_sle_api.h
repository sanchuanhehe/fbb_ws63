/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: 蓝牙Sdk sle接口头文件
 */

#ifndef HILINK_SLE_API_H
#define HILINK_SLE_API_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void HILINK_EnableSle(void);

bool IsSleEnable(void);

#ifdef __cplusplus
}
#endif

#endif