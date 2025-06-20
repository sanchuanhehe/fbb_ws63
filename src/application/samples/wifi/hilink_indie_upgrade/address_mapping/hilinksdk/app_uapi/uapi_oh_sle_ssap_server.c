/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of the sle ssap server. \n
 *
 * History: \n
 * 2024-11-13, Create file. \n
 */

#include "app_call.h"
#include "oh_sle_ssap_server.h"
#include "errcode.h"

errcode_t ssapsRegisterServer(SleUuid *appUuid, uint8_t *serverId)
{
    app_call2(APP_CALL_SSAPS_REGISTER_SERVER, ssapsRegisterServer, errcode_t,
        SleUuid *, appUuid, uint8_t *, serverId);
    return ERRCODE_FAIL;
}

errcode_t SsapsAddServiceSync(uint8_t serverId, SleUuid *serviceUuid, bool isPrimary, uint16_t *handle)
{
    app_call4(APP_CALL_SSAPS_ADD_SERVICE_SYNC, SsapsAddServiceSync, errcode_t,
        uint8_t, serverId, SleUuid *, serviceUuid, bool, isPrimary, uint16_t *, handle);
    return ERRCODE_FAIL;
}

errcode_t SsapsAddPropertySync(uint8_t serverId, uint16_t serviceHandle, SsapsPropertyInfo *property,
    uint16_t *handle)
{
    app_call4(APP_CALL_SSAPS_ADD_PROPERTY_SYNC, SsapsAddPropertySync, errcode_t,
        uint8_t, serverId, uint16_t, serviceHandle, SsapsPropertyInfo *, property, uint16_t *, handle);
    return ERRCODE_FAIL;
}

errcode_t SsapsAddDescriptorSync(uint8_t serverId, uint16_t serviceHandle, uint16_t propertyHandle,
    SsapsDescInfo *descriptor)
{
    app_call4(APP_CALL_SSAPS_ADD_DESCRIPTOR_SYNC, SsapsAddDescriptorSync, errcode_t,
        uint8_t, serverId, uint16_t, serviceHandle, uint16_t, propertyHandle, SsapsDescInfo *, descriptor);
    return ERRCODE_FAIL;
}

errcode_t SsapsStartService(uint8_t serverId, uint16_t serviceHandle)
{
    app_call2(APP_CALL_SSAPS_START_SERVICE, SsapsStartService, errcode_t,
        uint8_t, serverId, uint16_t, serviceHandle);
    return ERRCODE_FAIL;
}

errcode_t SsapsDeleteAllServices(uint8_t serverId)
{
    app_call1(APP_CALL_SSAPS_DELETE_ALL_SERVICES, SsapsDeleteAllServices, errcode_t,
        uint8_t, serverId);
    return ERRCODE_FAIL;
}

errcode_t SsapsSendResponse(uint8_t serverId, uint16_t connId, SsapsSendRsp *param)
{
    app_call3(APP_CALL_SSAPS_SEND_RESPONSE, SsapsSendResponse, errcode_t,
        uint8_t, serverId, uint16_t, connId, SsapsSendRsp *, param);
    return ERRCODE_FAIL;
}

errcode_t SsapsNotifyIndicate(uint8_t serverId, uint16_t connId, SsapsNtfInd *param)
{
    app_call3(APP_CALL_SSAPS_NOTIFY_INDICATE, SsapsNotifyIndicate, errcode_t,
        uint8_t, serverId, uint16_t, connId, SsapsNtfInd *, param);
    return ERRCODE_FAIL;
}

errcode_t SsapsRegisterCallbacks(SsapsCallbacks *func)
{
    app_call1(APP_CALL_SSAPS_REGISTER_CALLBACKS, SsapsRegisterCallbacks, errcode_t,
        SsapsCallbacks *, func);
    return ERRCODE_FAIL;
}