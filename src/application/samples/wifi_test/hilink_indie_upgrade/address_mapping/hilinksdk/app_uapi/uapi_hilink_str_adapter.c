/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the character string interface at the system adaptation layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

unsigned int HILINK_Strlen(const char *src)
{
    app_call1(APP_CALL_HILINK_STRLEN, HILINK_Strlen, unsigned int, const char *, src);
    return 0;
}

char *HILINK_Strchr(const char *str, int ch)
{
    app_call2(APP_CALL_HILINK_STRCHR, HILINK_Strchr, char *, const char *, str, int, ch);
    return NULL;
}

char *HILINK_Strrchr(const char *str, int ch)
{
    app_call2(APP_CALL_HILINK_STRRCHR, HILINK_Strrchr, char *, const char *, str, int, ch);
    return NULL;
}

int HILINK_Atoi(const char *str)
{
    app_call1(APP_CALL_HILINK_ATOI, HILINK_Atoi, int, const char *, str);
    return 0;
}

char *HILINK_Strstr(const char *str1, const char *str2)
{
    app_call2(APP_CALL_HILINK_STRSTR, HILINK_Strstr, char *, const char *, str1, const char *, str2);
    return NULL;
}

int HILINK_Strcmp(const char *str1, const char *str2)
{
    app_call2(APP_CALL_HILINK_STRCMP, HILINK_Strcmp, int, const char *, str1, const char *, str2);
    return 0;
}

int HILINK_Strncmp(const char *str1, const char *str2, unsigned int len)
{
    app_call3(APP_CALL_HILINK_STRNCMP, HILINK_Strncmp, int, const char *, str1, const char *, str2, unsigned int, len);
    return 0;
}