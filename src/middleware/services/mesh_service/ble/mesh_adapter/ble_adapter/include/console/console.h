/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: console config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define console_printf(_fmt, ...) printf(_fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
