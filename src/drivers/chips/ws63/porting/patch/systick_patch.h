/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides systick driver patch header. \n
 *
 * History: \n
 * 2023-11-06, Create file. \n
 */

#ifndef SFC_PATCH_H
#define SFC_PATCH_H

#include "systick_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static inline uint64_t convert_s_2_count_fix(uint64_t sec)
{
    return sec * systick_clock_get();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif