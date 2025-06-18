/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the AES Encryption and Decryption Adaptation Layer Interfaces. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_aes.h"

int HILINK_SAL_AesGcmEncrypt(const HiLinkAesGcmParam *param, unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    app_call4(APP_CALL_HILINK_SAL_AES_GCM_ENCRYPT, HILINK_SAL_AesGcmEncrypt, int,
        const HiLinkAesGcmParam *, param, unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
    return 0;
}

int HILINK_SAL_AesGcmDecrypt(const HiLinkAesGcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    app_call4(APP_CALL_HILINK_SAL_AES_GCM_DECRYPT, HILINK_SAL_AesGcmDecrypt, int,
        const HiLinkAesGcmParam *, param, const unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
    return 0;
}

int HILINK_SAL_AddPadding(HiLinkPaddingMode mode, unsigned char *out, unsigned int outLen, unsigned int dataLen)
{
    app_call4(APP_CALL_HILINK_SAL_ADD_PADDING, HILINK_SAL_AddPadding, int,
        HiLinkPaddingMode, mode, unsigned char *, out, unsigned int, outLen, unsigned int, dataLen);
    return 0;
}

int HILINK_SAL_GetPadding(HiLinkPaddingMode mode, const unsigned char *input,
    unsigned int inputLen, unsigned int *dataLen)
{
    app_call4(APP_CALL_HILINK_SAL_GET_PADDING, HILINK_SAL_GetPadding, int,
        HiLinkPaddingMode, mode, const unsigned char *, input, unsigned int, inputLen, unsigned int *, dataLen);
    return 0;
}

int HILINK_SAL_AesCbcEncrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    app_call2(APP_CALL_HILINK_SAL_AES_CBC_ENCRYPT, HILINK_SAL_AesCbcEncrypt, int,
        const HiLinkAesCbcParam *, param, unsigned char *, buf);
    return 0;
}

int HILINK_SAL_AesCbcDecrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    app_call2(APP_CALL_HILINK_SAL_AES_CBC_DECRYPT, HILINK_SAL_AesCbcDecrypt, int,
        const HiLinkAesCbcParam *, param, unsigned char *, buf);
    return 0;
}

int HILINK_SAL_AesCcmDecrypt(const HiLinkAesCcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    app_call4(APP_CALL_HILINK_SAL_AES_CCM_DECRYPT, HILINK_SAL_AesCcmDecrypt, int,
        const HiLinkAesCcmParam *, param, const unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
    return 0;
}

int HILINK_SAL_AesCcmEncrypt(const HiLinkAesCcmParam *param, unsigned char *tag, unsigned int tagLen,
    unsigned char *buf)
{
    app_call4(APP_CALL_HILINK_SAL_AES_CCM_ENCRYPT, HILINK_SAL_AesCcmEncrypt, int, const HiLinkAesCcmParam *, param,
        unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
    return 0;
}