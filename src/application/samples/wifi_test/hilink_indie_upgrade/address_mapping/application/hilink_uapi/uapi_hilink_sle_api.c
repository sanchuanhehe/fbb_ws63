  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hilink sle api
 *
 * History: \n
 * 2024-11-08, Create file.
 */
#include "hilink_call.h"
#include "hilink_sle_api.h"

void HILINK_EnableSle(void)
{
    hilink_call0_ret_void(HILINK_CALL_HILINK_ENABLE_SLE, HILINK_EnableSle);
}