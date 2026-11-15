/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of RSA Encryption and Decryption Adaptation Layer Interfaces. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_rsa.h"

HiLinkRsaContext HILINK_SAL_RsaInit(HiLinkRsaPkcs1Mode padding, HiLinkMdType md)
{
    app_call2(APP_CALL_HILINK_SAL_RSA_INIT, HILINK_SAL_RsaInit, HiLinkRsaContext,
        HiLinkRsaPkcs1Mode, padding, HiLinkMdType, md);
    return NULL;
}

void HILINK_SAL_RsaFree(HiLinkRsaContext ctx)
{
    app_call1_ret_void(APP_CALL_HILINK_SAL_RSA_FREE, HILINK_SAL_RsaFree, HiLinkRsaContext, ctx);
}

int HILINK_SAL_RsaParamImport(HiLinkRsaContext ctx, const HiLinkRsaParam *param)
{
    app_call2(APP_CALL_HILINK_SAL_RSA_PARAM_IMPORT, HILINK_SAL_RsaParamImport, int,
        HiLinkRsaContext, ctx, const HiLinkRsaParam *, param);
    return 0;
}

int HILINK_RsaPkcs1Verify(HiLinkRsaContext ctx, HiLinkMdType md, const unsigned char *hash,
    unsigned int hashLen, const unsigned char *sig, unsigned int sigLen)
{
    app_call6(APP_CALL_HILINK_RSA_PKCS1_VERIFY, HILINK_RsaPkcs1Verify, int, HiLinkRsaContext, ctx, HiLinkMdType, md,
        const unsigned char *, hash, unsigned int, hashLen, const unsigned char *, sig, unsigned int, sigLen);
    return 0;
}

int HILINK_RsaPkcs1Decrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int *len)
{
    app_call3(APP_CALL_HILINK_RSA_PKCS1_DECRYPT, HILINK_RsaPkcs1Decrypt, int, const HiLinkRsaCryptParam *, param,
        unsigned char *, buf, unsigned int *, len);
    return 0;
}

int HILINK_RsaPkcs1Encrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int len)
{
    app_call3(APP_CALL_HILINK_RSA_PKCS1_ENCRYPT, HILINK_RsaPkcs1Encrypt, int, const HiLinkRsaCryptParam *, param,
        unsigned char *, buf, unsigned int, len);
    return 0;
}