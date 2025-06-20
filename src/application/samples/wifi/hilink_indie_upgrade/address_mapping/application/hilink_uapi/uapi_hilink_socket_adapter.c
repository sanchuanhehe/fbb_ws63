  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hilink socket adapter
 *
 * History: \n
 * 2024-05-28, Create file.
 */
#include "hilink_call.h"
#include "hilink_socket_adapter.h"

int HILINK_RegisterErrnoCallback(GetErrno cb)
{
    hilink_call1(HILINK_CALL_HILINK_REGISTER_ERRNO_CALLBACK, HILINK_RegisterErrnoCallback, int, GetErrno, cb);
    return 0;
}