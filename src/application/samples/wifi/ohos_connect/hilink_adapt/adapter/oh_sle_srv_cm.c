/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Connection manager module.
 */

/**
 * @defgroup oh_sle_connection_manager Connection manager API
 * @ingroup  SLE
 * @{
 */
#include "oh_sle_common.h"
#include "sle_connection_manager.h"
#include "oh_sle_connection_manager.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OH_GLE_SRV_CONNECTION_MANAGER

ErrCodeType SleConnectRemoteDevice(const SleAddr *addr)
{
    return sle_connect_remote_device((sle_addr_t *)addr);
}

ErrCodeType SleDisconnectRemoteDevice(const SleAddr *addr)
{
    return sle_disconnect_remote_device((sle_addr_t *)addr);
}

ErrCodeType SleUpdateConnectParam(SleConnectionParamUpdate *params)
{
    return sle_update_connect_param((sle_connection_param_update_t *)params);
}

ErrCodeType SlePairRemoteDevice(const SleAddr *addr)
{
    return sle_pair_remote_device((sle_addr_t *)addr);
}

ErrCodeType SleRemovePairedRemoteDevice(const SleAddr *addr)
{
    return sle_remove_paired_remote_device((sle_addr_t *)addr);
}

ErrCodeType SleRemoveAllPairs(void)
{
    return sle_remove_all_pairs();
}

ErrCodeType SleGetPairedDevicesNum(uint16_t *number)
{
    return  sle_get_paired_devices_num(number);
}

ErrCodeType SleGetPairedDevices(SleAddr *addr, uint16_t *number)
{
    return sle_get_paired_devices((sle_addr_t *)addr, number);
}

ErrCodeType SleGetPairState(const SleAddr *addr, uint8_t *State)
{
    return sle_get_pair_state((sle_addr_t *)addr, State);
}

ErrCodeType SleReadRemoteDeviceRssi(uint16_t connId)
{
    return sle_read_remote_device_rssi(connId);
}

ErrCodeType SleConnectionRegisterCallbacks(SleConnectionCallbacks *func)
{
    sle_connection_callbacks_t cbk = {0};
    cbk.connect_state_changed_cb = (sle_connect_state_changed_callback)func->connectStateChangedCb;
    cbk.connect_param_update_req_cb = (sle_connect_param_update_req_callback)func->connectParamUpdateReqCb;
    cbk.connect_param_update_cb = (sle_connect_param_update_callback)func->connectParamUpdateCb;
    cbk.auth_complete_cb = (sle_auth_complete_callback)func->authCompleteCb;
    cbk.pair_complete_cb = (sle_pair_complete_callback)func->pairCompleteCb;
    cbk.read_rssi_cb = (sle_read_rssi_callback)func->readRssiCb;
    cbk.low_latency_cb = (sle_low_latency_callback)func->lowLatencCb;
    cbk.set_phy_cb = (sle_set_phy_callback)func->setPhyCb;
    return sle_connection_register_callbacks(&cbk);
}