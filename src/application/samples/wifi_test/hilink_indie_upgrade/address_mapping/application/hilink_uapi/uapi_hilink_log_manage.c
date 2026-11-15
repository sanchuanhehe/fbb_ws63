 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: HiLink log management \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "hilink_log_manage.h"
#include "hilink_call.h"

#include <stdio.h>

void HILINK_SetLogLevel(HiLinkLogLevel level)
{
    hilink_call1_ret_void(HILINK_CALL_HILINK_SET_LOG_LEVEL, HILINK_SetLogLevel, HiLinkLogLevel, level);
}

HiLinkLogLevel HILINK_GetLogLevel(void)
{
    hilink_call0(HILINK_CALL_HILINK_GET_LOG_LEVEL, HILINK_GetLogLevel, HiLinkLogLevel);
    return HILINK_LOG_INVALID;
}