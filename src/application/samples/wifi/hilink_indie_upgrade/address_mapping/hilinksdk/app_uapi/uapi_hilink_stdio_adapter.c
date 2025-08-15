/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the standard output interface of the system adaptation layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include <stddef.h>
#include <stdarg.h>
#include "app_call.h"

int HILINK_Vprintf(const char *format, va_list ap)
{
    app_call2(APP_CALL_HILINK_VPRINTF, HILINK_Vprintf, int, const char *, format, va_list, ap);
    return 0;
}

int HILINK_Printf(const char *format, ...)
{
    if (format == NULL) {
        return 0;
    }
    va_list ap;
    va_start(ap, format);
    int ret = HILINK_Vprintf(format, ap);
    va_end(ap);

    return ret;
}

int HILINK_Rand(unsigned char *input, unsigned int len)
{
    app_call2(APP_CALL_HILINK_RAND, HILINK_Rand, int, unsigned char *, input, unsigned int, len);
    return 0;
}

int HILINK_Trng(unsigned char *input, unsigned int len)
{
    app_call2(APP_CALL_HILINK_TRNG, HILINK_Trng, int, unsigned char *, input, unsigned int, len);
    return 0;
}