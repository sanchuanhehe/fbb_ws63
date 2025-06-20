/*
  * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */
#undef THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SRV_SERVICE_ACCESS_PROTOTOL_CLIENT

#include "osal_list.h"
#include "oh_sle_errcode.h"
#include "oh_sle_common.h"
#include "sle_ssap_client.h"
#include "oh_sle_ssap_client.h"

errcode_t SsapcRegisterClient(SleUuid *appUuid, uint8_t *clientId)
{
    return ssapc_register_client((sle_uuid_t *)appUuid, clientId);
}

errcode_t SsapcUnregisterClient(uint8_t clientId)
{
    return ssapc_unregister_client(clientId);
}

errcode_t SsapcFindStructure(uint8_t clientId, uint16_t connId, SsapcFindStructureParam *param)
{
    return ssapc_find_structure(clientId, connId, (ssapc_find_structure_param_t*)param);
}

errcode_t SsapcReadReqByUuid(uint8_t clientId, uint16_t connId, SsapcReadReqByUuidParam *param)
{
    return ssapc_read_req_by_uuid(clientId, connId, (ssapc_read_req_by_uuid_param_t *)param);
}

errcode_t SsapcReadReq(uint8_t clientId, uint16_t connId, uint16_t handle, uint8_t type)
{
    return ssapc_read_req(clientId, connId, handle, type);
}
 

errcode_t SsapWriteReq(uint8_t clientId, uint16_t connId, SsapcWriteParam *param)
{
    return ssapc_write_req(clientId, connId, (ssapc_write_param_t *)param);
}

errcode_t SsapcWriteCmd(uint8_t clientId, uint16_t connId, SsapcWriteParam *param)
{
    return ssapc_write_cmd(clientId, connId, (ssapc_write_param_t *)param);
}

errcode_t SsapcExchangeInfoReq(uint8_t clientId, uint16_t connId, SsapcExchangeInfo* param)
{
    return ssapc_exchange_info_req(clientId, connId, (ssap_exchange_info_t*)param);
}
 
errcode_t SsapcRegisterCallbacks(SsapcCallbacks *func)
{
    ssapc_callbacks_t cbk = {0};
    cbk.find_structure_cb = (ssapc_find_structure_callback)(func->findStructureCb);
    cbk.ssapc_find_property_cbk = (ssapc_find_property_callback)(func->ssapcFindPropertyCbk);
    cbk.find_structure_cmp_cb = (ssapc_find_structure_complete_callback)(func->findStructureCmpCb);
    cbk.read_cfm_cb = (ssapc_read_cfm_callback)(func->readCfmCb);
    cbk.read_by_uuid_cmp_cb = (ssapc_read_by_uuid_complete_callback)(func->readByUuidCmpCb);
    cbk.write_cfm_cb = (ssapc_write_cfm_callback)(func->writeCfmCb);
    cbk.exchange_info_cb = (ssapc_exchange_info_callback)(func->exchangeInfoCb);
    cbk.notification_cb = (ssapc_notification_callback)(func->notificationCb);
    cbk.indication_cb = (ssapc_indication_callback)(func->indicationCb);
    return ssapc_register_callbacks(&cbk);
}