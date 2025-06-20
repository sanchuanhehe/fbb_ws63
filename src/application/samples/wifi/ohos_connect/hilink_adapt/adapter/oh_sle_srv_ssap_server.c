/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE ssap server manager module.
 */

/**
 * @defgroup sle_srv_ssap_server ssap server manager API
 * @ingroup  SLE
 * @{
 */

#include "sle_ssap_server.h"
#include "oh_sle_ssap_stru.h"
#include "oh_sle_ssap_server.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SRV_SERVICE_ACCESS_PROTOTOL_SERVER

#define OCTET_BIT_LEN 8

#define INVALID_ITEM_HANDLE 0xFFFF
#define INVALID_CONN_ID 0xFFFF
#define INVALID_SERVER_ID 0xFF

#define SSAPS_MAX_SERVICE_NUM 64
#define SSAPS_MAX_SERVER_NUM 1
#define SSAPS_MAX_HANLDE_NUM 0xFFFF

errcode_t ssapsRegisterServer(SleUuid *appUuid, uint8_t *serverId)
{
    return ssaps_register_server((sle_uuid_t *)appUuid, serverId);
}

errcode_t SsapsUnregisterServer(uint8_t serverId)
{
    return ssaps_unregister_server(serverId);
}

errcode_t SsapsAddServiceSync(uint8_t serverId, SleUuid *serviceUuid, bool isPrimary, uint16_t *handle)
{
    return ssaps_add_service_sync(serverId, (sle_uuid_t *)serviceUuid, isPrimary, handle);
}

errcode_t SsapsAddPropertySync(uint8_t serverId, uint16_t serviceHandle, SsapsPropertyInfo *property,
    uint16_t *handle)
{
    return ssaps_add_property_sync(serverId, serviceHandle, (ssaps_property_info_t *)property, handle);
}

errcode_t SsapsAddDescriptorSync(uint8_t serverId, uint16_t serviceHandle, uint16_t propertyHandle,
    SsapsDescInfo *descriptor)
{
    return ssaps_add_descriptor_sync(serverId, serviceHandle, propertyHandle, (ssaps_desc_info_t *)descriptor);
}

errcode_t SsapsStartService(uint8_t serverId, uint16_t serviceHandle)
{
    return ssaps_start_service(serverId, serviceHandle);
}

errcode_t SsapsDeleteAllServices(uint8_t serverId)
{
    return ssaps_delete_all_services(serverId);
}

errcode_t SsapsSendResponse(uint8_t serverId, uint16_t connId, SsapsSendRsp *param)
{
    return ssaps_send_response(serverId, connId, (ssaps_send_rsp_t *)param);
}

errcode_t SsapsNotifyIndicate(uint8_t serverId, uint16_t connId, SsapsNtfInd *param)
{
    return ssaps_notify_indicate(serverId, connId, (ssaps_ntf_ind_t *)param);
}

errcode_t SsapsNotifyIndicateByUuid(uint8_t serverId, uint16_t connId, SsapsNtfIndByUuid *param)
{
    return ssaps_notify_indicate_by_uuid(serverId, connId, (ssaps_ntf_ind_by_uuid_t *)param);
}

errcode_t SsapsSetInfo(uint8_t serverId, SsapcExchangeInfo *info)
{
    return ssaps_set_info(serverId, (ssap_exchange_info_t *)info);
}

errcode_t SsapsRegisterCallbacks(SsapsCallbacks *func)
{
    ssaps_callbacks_t cbk = {0};
    cbk.add_service_cb = (ssaps_add_service_callback)(func->addServiceCb);
    cbk.add_property_cb = (ssaps_add_property_callback)(func->addPropertyCb);
    cbk.add_descriptor_cb = (ssaps_add_descriptor_callback)(func->addDescriptorCb);
    cbk.start_service_cb = (ssaps_start_service_callback)(func->startServiceCb);
    cbk.delete_all_service_cb = (ssaps_delete_all_service_callback)(func->deleteAllServiceCb);
    cbk.read_request_cb = (ssaps_read_request_callback)(func->readRequestCb);
    cbk.write_request_cb = (ssaps_write_request_callback)(func->writeRequestCb);
    cbk.indicate_cfm_cb = (ssaps_indicate_cfm_callback)(func->indicateCfmCb);
    cbk.mtu_changed_cb = (ssaps_mtu_changed_callback)(func->mtuChangedCb);
    return ssaps_register_callbacks(&cbk);
}