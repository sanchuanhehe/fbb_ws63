/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: hmac_wapi_sms4.c的头文件.
 */

#ifndef __HMAC_WAPI_SMS4_H__
#define __HMAC_WAPI_SMS4_H__

#include "oal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_WAPI_SMS4_H
/*****************************************************************************/
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define left_one(_B) ((_B) ^ rotl(_B, 2) ^ rotl(_B, 10) ^ rotl(_B, 18) ^ rotl(_B, 24))

#define left_two(_B) ((_B) ^ rotl(_B, 13) ^ rotl(_B, 23))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_void hmac_sms4_crypt_etc(const osal_u8 *input, osal_u8 *output, const osal_u32 *rk);
osal_void hmac_sms4_keyext_etc(osal_u8 *key, osal_u32 *rk);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hmac_wapi_sms4.h */