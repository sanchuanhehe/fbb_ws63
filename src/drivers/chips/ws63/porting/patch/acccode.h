/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides anti-channel conflict code header. \n
 *
 * History: \n
 * 2024-08-26, Create file. \n
 */

#ifndef ACCCODE_H
#define ACCCODE_H

#include "stdio.h"
#include "errcode.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
errcode_t write_acccode(uint16_t vendor_code);
#endif
uint32_t uapi_get_unique_code(uint8_t *unique_code);
uint8_t is_acccode_ok(uint16_t vendor_code);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif