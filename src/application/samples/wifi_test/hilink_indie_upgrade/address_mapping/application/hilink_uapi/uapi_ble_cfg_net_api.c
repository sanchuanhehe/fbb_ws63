  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: ble cfg net api \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "hilink_call.h"
#include "ble_cfg_net_api.h"

int BLE_CfgNetInit(const BLE_InitPara *para, const BLE_CfgNetCb *callback)
{
    hilink_call2(HILINK_CALL_BLE_CFG_NET_INIT, BLE_CfgNetInit, int,
        const BLE_InitPara *, para, const BLE_CfgNetCb *, callback);
    return 0;
}

int BLE_CfgNetDeInit(const BLE_GattHandleList *handleList, unsigned int flag)
{
    hilink_call2(HILINK_CALL_BLE_CFG_NET_DE_INIT, BLE_CfgNetDeInit, int,
        const BLE_GattHandleList *, handleList, unsigned int, flag);
    return 0;
}

int BLE_CfgNetAdvCtrl(unsigned int advSecond)
{
    hilink_call1(HILINK_CALL_BLE_CFG_NET_ADV_CTRL, BLE_CfgNetAdvCtrl, int, unsigned int, advSecond);
    return 0;
}

int BLE_CfgNetAdvUpdate(const BLE_AdvInfo *advInfo)
{
    hilink_call1(HILINK_CALL_BLE_CFG_NET_ADV_UPDATE, BLE_CfgNetAdvUpdate, int, const BLE_AdvInfo *, advInfo);
    return 0;
}

int BLE_CfgNetDisConnect(void)
{
    hilink_call0(HILINK_CALL_BLE_CFG_NET_DIS_CONNECT, BLE_CfgNetDisConnect, int);
    return 0;
}

int BLE_SendCustomData(BLE_DataType dataType, const unsigned char *buff, unsigned int len)
{
    hilink_call3(HILINK_CALL_BLE_SEND_CUSTOM_DATA, BLE_SendCustomData, int,
        BLE_DataType, dataType, const unsigned char *, buff, unsigned int, len);
    return 0;
}

int BLE_GetAdvType(void)
{
    hilink_call0(HILINK_CALL_BLE_GET_ADV_TYPE, BLE_GetAdvType, int);
    return 0;
}

void BLE_SetAdvType(int type)
{
    hilink_call1_ret_void(HILINK_CALL_BLE_SET_ADV_TYPE, BLE_SetAdvType, int, type);
}

int BLE_SetAdvNameMpp(const unsigned char *mpp, unsigned int len)
{
    hilink_call2(HILINK_CALL_BLE_SET_ADV_NAME_MPP, BLE_SetAdvNameMpp, int,
        const unsigned char *, mpp, unsigned int, len);
    return 0;
}

int BLE_NearDiscoveryInit(const BLE_NearDiscoveryCb *cb)
{
    hilink_call1(HILINK_CALL_BLE_NEAR_DISCOVERY_INIT, BLE_NearDiscoveryInit, int, const BLE_NearDiscoveryCb *, cb);
    return 0;
}

int BLE_NearDiscoveryEnable(unsigned long waitTime)
{
    hilink_call1(HILINK_CALL_BLE_NEAR_DISCOVERY_ENABLE, BLE_NearDiscoveryEnable, int, unsigned long, waitTime);
    return 0;
}

int HILINK_BT_GetTaskStackSize(const char *name, unsigned long *stackSize)
{
    hilink_call2(HILINK_CALL_HILINK_BT_GET_TASK_STACK_SIZE, HILINK_BT_GetTaskStackSize, int,
        const char *, name, unsigned long *, stackSize);
    return 0;
}

int HILINK_BT_SetTaskStackSize(const char *name, unsigned long stackSize)
{
    hilink_call2(HILINK_CALL_HILINK_BT_SET_TASK_STACK_SIZE, HILINK_BT_SetTaskStackSize, int,
        const char *, name, unsigned long, stackSize);
    return 0;
}