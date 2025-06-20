/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the Message Digest Algorithm Adaptation Layer Interface. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_md.h"

int HILINK_SAL_MdCalc(HiLinkMdType type, const unsigned char *inData, unsigned int inLen,
    unsigned char *md, unsigned int mdLen)
{
    app_call5(APP_CALL_HILINK_SAL_MD_CALC, HILINK_SAL_MdCalc, int, HiLinkMdType, type,
        const unsigned char *, inData, unsigned int, inLen, unsigned char *, md, unsigned int, mdLen);
    return 0;
}

int HILINK_SAL_HmacCalc(const HiLinkHmacParam *param, unsigned char *hmac, unsigned int hmacLen)
{
    app_call3(APP_CALL_HILINK_SAL_HMAC_CALC, HILINK_SAL_HmacCalc, int, const HiLinkHmacParam *, param,
        unsigned char *, hmac, unsigned int, hmacLen);
    return 0;
}

HiLinkMdContext HILINK_SAL_MdInit(HiLinkMdType type)
{
    app_call1(APP_CALL_HILINK_SAL_MD_INIT, HILINK_SAL_MdInit, HiLinkMdContext, HiLinkMdType, type);
    return NULL;
}

int HILINK_SAL_MdUpdate(HiLinkMdContext ctx, const unsigned char *inData, unsigned int inLen)
{
    app_call3(APP_CALL_HILINK_SAL_MD_UPDATE, HILINK_SAL_MdUpdate, int,
        HiLinkMdContext, ctx, const unsigned char *, inData, unsigned int, inLen);
    return 0;
}

int HILINK_SAL_MdFinish(HiLinkMdContext ctx, unsigned char *outData, unsigned int outLen)
{
    app_call3(APP_CALL_HILINK_SAL_MD_FINISH, HILINK_SAL_MdFinish, int,
        HiLinkMdContext, ctx, unsigned char *, outData, unsigned int, outLen);
    return 0;
}

void HILINK_SAL_MdFree(HiLinkMdContext ctx)
{
    app_call1_ret_void(APP_CALL_HILINK_SAL_MD_FREE, HILINK_SAL_MdFree, HiLinkMdContext, ctx);
}