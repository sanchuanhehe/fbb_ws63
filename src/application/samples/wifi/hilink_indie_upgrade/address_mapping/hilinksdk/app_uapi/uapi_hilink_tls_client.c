/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the TLS client, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_tls_client.h"

HiLinkTlsClient *HILINK_TlsClientCreate(const char *custom)
{
    app_call1(APP_CALL_HILINK_TLS_CLIENT_CREATE, HILINK_TlsClientCreate, HiLinkTlsClient *, const char *, custom);
    return NULL;
}

int HILINK_SetTlsClientOption(HiLinkTlsClient *ctx, HiLinkTlsOption option, const void *value, unsigned int len)
{
    app_call4(APP_CALL_HILINK_SET_TLS_CLIENT_OPTION, HILINK_SetTlsClientOption, int,
        HiLinkTlsClient *, ctx, HiLinkTlsOption, option, const void *, value, unsigned int, len);
    return 0;
}

int HILINK_TlsClientConnect(HiLinkTlsClient *ctx)
{
    app_call1(APP_CALL_HILINK_TLS_CLIENT_CONNECT, HILINK_TlsClientConnect, int, HiLinkTlsClient *, ctx);
    return 0;
}

int HILINK_TlsClientGetContextFd(HiLinkTlsClient *ctx)
{
    app_call1(APP_CALL_HILINK_TLS_CLIENT_GET_CONTEXT_FD, HILINK_TlsClientGetContextFd, int, HiLinkTlsClient *, ctx);
    return 0;
}

int HILINK_TlsClientRead(HiLinkTlsClient *ctx, unsigned char *buf, size_t len)
{
    app_call3(APP_CALL_HILINK_TLS_CLIENT_READ, HILINK_TlsClientRead, int,
        HiLinkTlsClient *, ctx, unsigned char *, buf, size_t, len);
    return 0;
}

int HILINK_TlsClientWrite(HiLinkTlsClient *ctx, const unsigned char *buf, size_t len)
{
    app_call3(APP_CALL_HILINK_TLS_CLIENT_WRITE, HILINK_TlsClientWrite, int,
        HiLinkTlsClient *, ctx, const unsigned char *, buf, size_t, len);
    return 0;
}

bool HILINK_TlsClientIsValidCert(HiLinkTlsClient *ctx)
{
    app_call1(APP_CALL_HILINK_TLS_CLIENT_IS_VALID_CERT, HILINK_TlsClientIsValidCert, bool, HiLinkTlsClient *, ctx);
    return false;
}

int HILINK_TlsClientFreeResource(HiLinkTlsClient *ctx)
{
    app_call1(APP_CALL_HILINK_TLS_CLIENT_FREE_RESOURCE, HILINK_TlsClientFreeResource, int, HiLinkTlsClient *, ctx);
    return 0;
}