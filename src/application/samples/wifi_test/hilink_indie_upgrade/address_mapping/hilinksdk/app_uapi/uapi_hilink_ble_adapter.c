/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the ble adapter, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include <stdbool.h>
#include "app_call.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt_server.h"

BdAddr* GetLocalAddress(void)
{
    app_call0(APP_CALL_GET_LOCAL_ADDRESS, GetLocalAddress, BdAddr*);
    return NULL;
}

bool GetLocalName(unsigned char *localName, unsigned char *length)
{
    app_call2(APP_CALL_GET_LOCAL_NAME, GetLocalName, bool, unsigned char *, localName, unsigned char *, length);
    return false;
}

bool SetLocalName(unsigned char *localName, unsigned char length)
{
    app_call2(APP_CALL_SET_LOCAL_NAME, SetLocalName, bool, unsigned char *, localName, unsigned char, length);
    return false;
}

bool BluetoothFactoryReset(void)
{
    app_call0(APP_CALL_BLUETOOTH_FACTORY_RESET, BluetoothFactoryReset, bool);
    return false;
}

int GetBtScanMode(void)
{
    app_call0(APP_CALL_GET_BT_SCAN_MODE, GetBtScanMode, int);
    return 0;
}

bool SetBtScanMode(int mode, int duration)
{
    app_call2(APP_CALL_SET_BT_SCAN_MODE, SetBtScanMode, bool, int, mode, int, duration);
    return false;
}

int ReadBtMacAddr(unsigned char *mac, unsigned int len)
{
    app_call2(APP_CALL_READ_BT_MAC_ADDR, ReadBtMacAddr, int, unsigned char *, mac, unsigned int, len);
    return 0;
}

bool GetPariedDevicesNum(unsigned int *number)
{
    app_call1(APP_CALL_GET_PARIED_DEVICES_NUM, GetPariedDevicesNum, bool, unsigned int *, number);
    return false;
}

int GetPairState(void)
{
    app_call0(APP_CALL_GET_PAIR_STATE, GetPairState, int);
    return 0;
}

bool RemovePair(const BdAddr addr)
{
    app_call1(APP_CALL_REMOVE_PAIR, RemovePair, bool, const BdAddr, addr);
    return false;
}

bool RemoveAllPairs(void)
{
    app_call0(APP_CALL_REMOVE_ALL_PAIRS, RemoveAllPairs, bool);
    return false;
}

bool ReadRemoteRssiValue(const BdAddr *bdAddr, int transport)
{
    app_call2(APP_CALL_READ_REMOTE_RSSI_VALUE, ReadRemoteRssiValue, bool, const BdAddr *, bdAddr, int, transport);
    return false;
}

bool IsAclConnected(BdAddr *addr)
{
    app_call1(APP_CALL_IS_ACL_CONNECTED, IsAclConnected, bool, BdAddr *, addr);
    return false;
}

bool DisconnectRemoteDevice(BdAddr *addr)
{
    app_call1(APP_CALL_DISCONNECT_REMOTE_DEVICE, DisconnectRemoteDevice, bool, BdAddr *, addr);
    return false;
}

bool ConnectRemoteDevice(BdAddr *addr)
{
    app_call1(APP_CALL_CONNECT_REMOTE_DEVICE, ConnectRemoteDevice, bool, BdAddr *, addr);
    return false;
}

int InitBtStack(void)
{
    app_call0(APP_CALL_INIT_BT_STACK, InitBtStack, int);
    return 0;
}

int EnableBtStack(void)
{
    app_call0(APP_CALL_ENABLE_BT_STACK, EnableBtStack, int);
    return 0;
}

int DisableBtStack(void)
{
    app_call0(APP_CALL_DISABLE_BT_STACK, DisableBtStack, int);
    return 0;
}

int SetDeviceName(const char *name, unsigned int len)
{
    app_call2(APP_CALL_SET_DEVICE_NAME, SetDeviceName, int, const char *, name, unsigned int, len);
    return 0;
}

int BleSetAdvData(int advId, const BleConfigAdvData *data)
{
    app_call2(APP_CALL_BLE_SET_ADV_DATA, BleSetAdvData, int, int, advId, const BleConfigAdvData *, data);
    return 0;
}

int BleStartAdv(int advId, const BleAdvParams *param)
{
    app_call2(APP_CALL_BLE_START_ADV, BleStartAdv, int, int, advId, const BleAdvParams *, param);
    return 0;
}

int BleStopAdv(int advId)
{
    app_call1(APP_CALL_BLE_STOP_ADV, BleStopAdv, int, int, advId);
    return 0;
}

int BleUpdateAdv(int advId, const BleAdvParams *param)
{
    app_call2(APP_CALL_BLE_UPDATE_ADV, BleUpdateAdv, int, int, advId, const BleAdvParams *, param);
    return 0;
}

int BleSetSecurityIoCap(BleIoCapMode mode)
{
    app_call1(APP_CALL_BLE_SET_SECURITY_IO_CAP, BleSetSecurityIoCap, int, BleIoCapMode, mode);
    return 0;
}

int BleSetSecurityAuthReq(BleAuthReqMode mode)
{
    app_call1(APP_CALL_BLE_SET_SECURITY_AUTH_REQ, BleSetSecurityAuthReq, int, BleAuthReqMode, mode);
    return 0;
}

int BleGattSecurityRsp(BdAddr bdAddr, bool accept)
{
    app_call2(APP_CALL_BLE_GATT_SECURITY_RSP, BleGattSecurityRsp, int, BdAddr, bdAddr, bool, accept);
    return 0;
}

int BleScanFilterParamSetup(BleAdvScanFilterParam *param)
{
    app_call1(APP_CALL_BLE_SCAN_FILTER_PARAM_SETUP, BleScanFilterParamSetup, int, BleAdvScanFilterParam *, param);
    return 0;
}

int BleScanFilterAddRemove(BleAdvScanFilterCondition *param)
{
    app_call1(APP_CALL_BLE_SCAN_FILTER_ADD_REMOVE, BleScanFilterAddRemove, int, BleAdvScanFilterCondition *, param);
    return 0;
}

int BleScanFilterClear(int clientId, int filterIndex)
{
    app_call2(APP_CALL_BLE_SCAN_FILTER_CLEAR, BleScanFilterClear, int, int, clientId, int, filterIndex);
    return 0;
}

int BleScanFilterEnable(int clientId, bool enable)
{
    app_call2(APP_CALL_BLE_SCAN_FILTER_ENABLE, BleScanFilterEnable, int, int, clientId, bool, enable);
    return 0;
}

int BleSetScanParameters(int clientId, BleScanParams *param)
{
    app_call2(APP_CALL_BLE_SET_SCAN_PARAMETERS, BleSetScanParameters, int, int, clientId, BleScanParams *, param);
    return 0;
}

int BleStartScan(void)
{
    app_call0(APP_CALL_BLE_START_SCAN, BleStartScan, int);
    return 0;
}

int BleStopScan(void)
{
    app_call0(APP_CALL_BLE_STOP_SCAN, BleStopScan, int);
    return 0;
}

int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    app_call1(APP_CALL_BLE_GATT_REGISTER_CALLBACKS, BleGattRegisterCallbacks, int, BtGattCallbacks *, func);
    return 0;
}

int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam)
{
    app_call3(APP_CALL_BLE_START_ADV_EX, BleStartAdvEx, int, int *, advId,
        const StartAdvRawData, rawData, BleAdvParams, advParam);
    return 0;
}

int BleGattsRegister(BtUuid appUuid)
{
    app_call1(APP_CALL_BLE_GATTS_REGISTER, BleGattsRegister, int, BtUuid, appUuid);
    return 0;
}

int BleGattsUnRegister(int serverId)
{
    app_call1(APP_CALL_BLE_GATTS_UN_REGISTER, BleGattsUnRegister, int, int, serverId);
    return 0;
}

int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId)
{
    app_call3(APP_CALL_BLE_GATTS_DISCONNECT, BleGattsDisconnect, int, int, serverId, BdAddr, bdAddr, int, connId);
    return 0;
}

int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number)
{
    app_call4(APP_CALL_BLE_GATTS_ADD_SERVICE, BleGattsAddService, int, int, serverId, BtUuid, srvcUuid,
        bool, isPrimary, int, number);
    return 0;
}

int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions)
{
    app_call5(APP_CALL_BLE_GATTS_ADD_CHARACTERISTIC, BleGattsAddCharacteristic, int, int, serverId, int, srvcHandle,
        BtUuid, characUuid, int, properties, int, permissions);
    return 0;
}

int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions)
{
    app_call4(APP_CALL_BLE_GATTS_ADD_DESCRIPTOR, BleGattsAddDescriptor, int, int, serverId, int, srvcHandle,
        BtUuid, descUuid, int, permissions);
    return 0;
}

int BleGattsStartService(int serverId, int srvcHandle)
{
    app_call2(APP_CALL_BLE_GATTS_START_SERVICE, BleGattsStartService, int, int, serverId, int, srvcHandle);
    return 0;
}

int BleGattsStopService(int serverId, int srvcHandle)
{
    app_call2(APP_CALL_BLE_GATTS_STOP_SERVICE, BleGattsStopService, int, int, serverId, int, srvcHandle);
    return 0;
}

int BleGattsDeleteService(int serverId, int srvcHandle)
{
    app_call2(APP_CALL_BLE_GATTS_DELETE_SERVICE, BleGattsDeleteService, int, int, serverId, int, srvcHandle);
    return 0;
}

int BleGattsClearServices(int serverId)
{
    app_call1(APP_CALL_BLE_GATTS_CLEAR_SERVICES, BleGattsClearServices, int, int, serverId);
    return 0;
}

int BleGattsSendResponse(int serverId, GattsSendRspParam *param)
{
    app_call2(APP_CALL_BLE_GATTS_SEND_RESPONSE, BleGattsSendResponse, int, int, serverId, GattsSendRspParam *, param);
    return 0;
}

int BleGattsSendIndication(int serverId, GattsSendIndParam *param)
{
    app_call2(APP_CALL_BLE_GATTS_SEND_INDICATION, BleGattsSendIndication, int,
        int, serverId, GattsSendIndParam *, param);
    return 0;
}

int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct)
{
    app_call2(APP_CALL_BLE_GATTS_SET_ENCRYPTION, BleGattsSetEncryption, int, BdAddr, bdAddr, BleSecAct, secAct);
    return 0;
}

int BleGattsRegisterCallbacks(BtGattServerCallbacks *func)
{
    app_call1(APP_CALL_BLE_GATTS_REGISTER_CALLBACKS, BleGattsRegisterCallbacks, int, BtGattServerCallbacks *, func);
    return 0;
}

int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo)
{
    app_call2(APP_CALL_BLE_GATTS_START_SERVICE_EX, BleGattsStartServiceEx, int, int *, srvcHandle,
        BleGattService *, srvcInfo);
    return 0;
}

int BleGattsStopServiceEx(int srvcHandle)
{
    app_call1(APP_CALL_BLE_GATTS_STOP_SERVICE_EX, BleGattsStopServiceEx, int, int, srvcHandle);
    return 0;
}
