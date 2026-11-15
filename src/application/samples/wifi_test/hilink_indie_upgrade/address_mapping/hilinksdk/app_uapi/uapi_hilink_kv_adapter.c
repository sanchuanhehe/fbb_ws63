/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Source file for HiLink adaptation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "app_call.h"
#include "hilink_kv_adapter.h"

int HILINK_KVStoreInit(const char *path, const char *key[], unsigned int num)
{
    app_call3(APP_CALL_HILINK_KVSTORE_INIT, HILINK_KVStoreInit, int,
        const char *, path, const char *[], key, unsigned int, num);
    return 0;
}

int HILINK_SetValue(const char *key, unsigned int offset, const unsigned char *value, unsigned int len)
{
    app_call4(APP_CALL_HILINK_SET_VALUE, HILINK_SetValue, int, const char *, key, unsigned int, offset,
        const unsigned char *, value, unsigned int, len);
    return 0;
}

int HILINK_GetValue(const char *key, unsigned int offset, unsigned char *value, unsigned int len)
{
    app_call4(APP_CALL_HILINK_GET_VALUE, HILINK_GetValue, int, const char *, key, unsigned int, offset,
        unsigned char *, value, unsigned int, len);
    return 0;
}

void HILINK_DeleteValue(const char * key)
{
    app_call1_ret_void(APP_CALL_HILINK_DELETE_VALUE, HILINK_DeleteValue, const char *, key);
}

int HILINK_GetFileName(const char *key, char *out, unsigned int len)
{
    app_call3(APP_CALL_HILINK_GET_FILE_NAME, HILINK_GetFileName, int,
        const char *, key, char *, out, unsigned int, len);
    return 0;
}
