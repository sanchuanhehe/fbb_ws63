/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of secure random number Adaptation Layer Interfaces. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_drbg.h"

HiLinkDrbgContext HILINK_SAL_DrbgInit(const char *custom)
{
    app_call1(APP_CALL_HILINK_SAL_DRBG_INIT, HILINK_SAL_DrbgInit, HiLinkDrbgContext, const char *, custom);
    return NULL;
}

void HILINK_SAL_DrbgDeinit(HiLinkDrbgContext ctx)
{
    app_call1_ret_void(APP_CALL_HILINK_SAL_DRBG_DEINIT, HILINK_SAL_DrbgDeinit, HiLinkDrbgContext, ctx);
}

int HILINK_SAL_DrbgRandom(HiLinkDrbgContext ctx, unsigned char *out, unsigned int outLen)
{
    app_call3(APP_CALL_HILINK_SAL_DRBG_RANDOM, HILINK_SAL_DrbgRandom, int,
        HiLinkDrbgContext, ctx, unsigned char *, out, unsigned int, outLen);
    return 0;
}