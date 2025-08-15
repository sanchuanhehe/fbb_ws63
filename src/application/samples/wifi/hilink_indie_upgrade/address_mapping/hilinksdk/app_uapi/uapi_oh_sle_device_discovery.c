/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of the sle device discovery. \n
 *
 * History: \n
 * 2024-11-13, Create file. \n
 */

#include "app_call.h"
#include "oh_sle_device_discovery.h"
#include "oh_sle_errcode.h"

ErrCodeType EnableSle(void)
{
    app_call0(APP_CALL_ENABLE_SLE, EnableSle, ErrCodeType);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType DisableSle(void)
{
    app_call0(APP_CALL_DISABLE_SLE, DisableSle, ErrCodeType);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleGetLocalAddr(SleAddr *addr)
{
    app_call1(APP_CALL_SLE_GET_LOCAL_ADDR, SleGetLocalAddr, ErrCodeType,
        SleAddr *, addr);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleSetLocalName(const uint8_t *name, uint8_t len)
{
    app_call2(APP_CALL_SLE_SET_LOCAL_NAME, SleSetLocalName, ErrCodeType,
        const uint8_t *, name, uint8_t, len);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleSetAnnounceData(uint8_t announceId, const SleAnnounceData *data)
{
    app_call2(APP_CALL_SLE_SET_ANNOUNCE_DATA, SleSetAnnounceData, ErrCodeType,
        uint8_t, announceId, const SleAnnounceData *, data);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleSetAnnounceParam(uint8_t announceId, const SleAnnounceParam *param)
{
    app_call2(APP_CALL_SLE_SET_ANNOUNCE_PARAM, SleSetAnnounceParam, ErrCodeType,
        uint8_t, announceId, const SleAnnounceParam *, param);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleStartAnnounce(uint8_t announceId)
{
    app_call1(APP_CALL_SLE_START_ANNOUNCE, SleStartAnnounce, ErrCodeType,
        uint8_t, announceId);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleStopAnnounce(uint8_t announceId)
{
    app_call1(APP_CALL_SLE_STOP_ANNOUNCE, SleStopAnnounce, ErrCodeType,
        uint8_t, announceId);
    return OH_ERRCODE_SLE_MAX;
}

ErrCodeType SleAnnounceSeekRegisterCallbacks(SleAnnounceSeekCallbacks *func)
{
    app_call1(APP_CALL_SLE_ANNOUNCE_SEEK_REGISTER_CALLBACKS, SleAnnounceSeekRegisterCallbacks, ErrCodeType,
        SleAnnounceSeekCallbacks *, func);
    return OH_ERRCODE_SLE_MAX;
}
