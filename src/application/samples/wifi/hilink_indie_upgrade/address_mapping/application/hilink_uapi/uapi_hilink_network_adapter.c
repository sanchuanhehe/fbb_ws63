  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hilink network adapter
 *
 * History: \n
 * 2024-05-28, Create file.
 */
#include "hilink_call.h"
#include "hilink_network_adapter.h"

int HILINK_RegWiFiRecoveryCallback(const WiFiRecoveryApi *cb, unsigned int cbSize)
{
    hilink_call2(HILINK_CALL_HILINK_REG_WI_FI_RECOVERY_CALLBACK, HILINK_RegWiFiRecoveryCallback, int,
        const WiFiRecoveryApi *, cb, unsigned int, cbSize);
    return 0;
}
