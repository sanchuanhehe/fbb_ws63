/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: pm_temp.c LiteOS pm模块缺失全局变量与接口补充
 * Create: 2021-08-11
 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_ext_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/
osal_u32 wlan_close(osal_void)
{
    return OAL_SUCC;
}

osal_u32 wlan_close_ex(osal_void)
{
    return OAL_SUCC;
}

struct wifi_srv_callback_handler* wlan_pm_get_wifi_srv_handler_etc(osal_void)
{
    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

