/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: SLE Device Discovery, Scan Manager, module.
 */

/**
 * @defgroup sle_device_discovery Device Discovery, Scan Manager API
 * @ingroup  SLE
 * @{
 */

#include "sle_device_discovery.h"
#include "oh_sle_device_discovery.h"

ErrCodeType EnableSle(void)
{
    return enable_sle();
}

ErrCodeType DisableSle(void)
{
    return disable_sle();
}

ErrCodeType SleSetLocalAddr(SleAddr *addr)
{
    return sle_set_local_addr((sle_addr_t*)addr);
}

ErrCodeType SleGetLocalAddr(SleAddr *addr)
{
    return sle_get_local_addr((sle_addr_t*)addr);
}

ErrCodeType SleSetLocalName(const uint8_t *name, uint8_t len)
{
    return sle_set_local_name(name, len);
}

ErrCodeType SleGetLocalName(uint8_t *name, uint8_t *len)
{
    return sle_get_local_name(name, len);
}

ErrCodeType SleSetAnnounceData(uint8_t announceId, const SleAnnounceData *data)
{
    return sle_set_announce_data(announceId, (sle_announce_data_t *)data);
}

ErrCodeType SleSetAnnounceParam(uint8_t announceId, const SleAnnounceParam *param)
{
    return sle_set_announce_param(announceId, (sle_announce_param_t *)param);
}


ErrCodeType SleStartAnnounce(uint8_t announceId)
{
    return sle_start_announce(announceId);
}

ErrCodeType SleStopAnnounce(uint8_t announceId)
{
    return sle_stop_announce(announceId);
}


ErrCodeType SleSetSeekParam(SleSeekParam *param)
{
    return sle_set_seek_param((sle_seek_param_t *)param);
}

ErrCodeType SleStartSeek(void)
{
    return sle_start_seek();
}

ErrCodeType SleStopSeek(void)
{
    return sle_stop_seek();
}

ErrCodeType SleAnnounceSeekRegisterCallbacks(SleAnnounceSeekCallbacks *func)
{
    uint8_t ret = OH_ERRCODE_SLE_SUCCESS;
    sle_announce_seek_callbacks_t announce_cbk = {0};
    announce_cbk.sle_enable_cb = (sle_enable_callback)(func->sleEnableCb);
    announce_cbk.sle_disable_cb = (sle_disable_callback)(func->sleDisableCb);
    announce_cbk.announce_enable_cb = (sle_announce_enable_callback)(func->announceEnableCb);
    announce_cbk.announce_disable_cb = (sle_announce_disable_callback)(func->announceDisableCb);
    announce_cbk.announce_remove_cb = (sle_announce_remove_callback)(func->announceRemoveCb);
    announce_cbk.announce_terminal_cb = (sle_announce_terminal_callback)(func->announceTerminalCb);
    announce_cbk.seek_enable_cb = (sle_start_seek_callback)(func->seekEnableCb);
    announce_cbk.seek_disable_cb = (sle_seek_disable_callback)(func->seekDisableCb);
    announce_cbk.seek_result_cb = (sle_seek_result_callback)(func->seekResultCb);
    announce_cbk.sle_dfr_cb = (sle_dfr_callback)(func->seekDfrCb);
    ret |= sle_announce_seek_register_callbacks(&announce_cbk);
    return ret;
}