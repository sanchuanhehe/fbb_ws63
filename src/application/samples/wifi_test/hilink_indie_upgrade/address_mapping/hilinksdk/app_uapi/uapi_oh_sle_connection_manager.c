/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of the sle connection manager. \n
 *
 * History: \n
 * 2024-11-13, Create file. \n
 */

#include "app_call.h"
#include "oh_sle_common.h"
#include "oh_sle_connection_manager.h"
#include "oh_sle_errcode.h"

ErrCodeType SleDisconnectRemoteDevice(const SleAddr *addr)
{
    app_call1(APP_CALL_SLE_DISCONNECT_REMOTE_DEVICE, SleDisconnectRemoteDevice, ErrCodeType,
        const SleAddr *, addr);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleConnectionRegisterCallbacks(SleConnectionCallbacks *func)
{
    app_call1(APP_CALL_SLE_CONNECTION_REGISTER_CALLBACKS, SleConnectionRegisterCallbacks, ErrCodeType,
        SleConnectionCallbacks *, func);
    return OH_ERRCODE_SLE_MAX;
}
