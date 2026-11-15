/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hcc service.
 * Author: CompanyName
 * Create: 2021-09-11
 */

#include "securec.h"
#include "osal_def.h"
#include "common_def.h"
#include "hcc_comm.h"
#include "hcc_bus.h"
#include "hcc.h"
#include "hcc_channel.h"
#include "hcc_list.h"
#include "hcc_if.h"
#include "hcc_service.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_SERVICE_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

STATIC hcc_service_list *hcc_get_serv_list(hcc_handler *hcc, hcc_service_type serv_type)
{
    hcc_service_list *serv_list = TD_NULL;
    if (hcc == TD_NULL) {
        return TD_NULL;
    }

    serv_list = (hcc_service_list *)(void *)hcc->hcc_serv.service_list.next;
    while (&serv_list->service_list != &hcc->hcc_serv.service_list) {
        if (serv_list->service_info == TD_NULL) {
            serv_list = (hcc_service_list *)(void *)serv_list->service_list.next;
            continue;
        }

        if (serv_list->service_info->service_type == serv_type) {
            return serv_list;
        }
        serv_list = (hcc_service_list *)(void *)serv_list->service_list.next;
    }
    return TD_NULL;
}

hcc_serv_info *hcc_get_serv_info(hcc_handler *hcc, hcc_service_type serv_type)
{
    hcc_service_list *serv_list = hcc_get_serv_list(hcc, serv_type);
    if (serv_list == TD_NULL) {
        return TD_NULL;
    }
    return serv_list->service_info;
}

ext_errno hcc_service_init(td_u8 chl, hcc_service_type service_type, hcc_adapt_ops *adapt)
{
    hcc_serv_info *serv_info;
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc srv init null\r\n");
        return EXT_ERR_HCC_INIT_ERR;
    }
    if (adapt == TD_NULL) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    if (hcc_get_serv_info(hcc, service_type) != TD_NULL) {
        return EXT_ERR_SUCCESS;
    }

    serv_info = hcc_add_service_list(hcc, service_type);
    if (serv_info == TD_NULL) {
        hcc_printf_err_log("hcc srv add-%d\r\n", service_type);
        return EXT_ERR_FAILURE;
    }
    serv_info->adapt = adapt;
    serv_info->service_type = service_type;
    if ((td_u32)service_type + 1 > hcc->srv_max_cnt) {
        hcc->srv_max_cnt = (td_u8)service_type + 1;
    }
    osal_atomic_set(&serv_info->service_enable, HCC_ENABLE);
    if ((hcc->bus != TD_NULL) && (hcc->bus->hcc_srv_hook != TD_NULL)) {
        hcc->bus->hcc_srv_hook(hcc->bus, service_type);
    }
    hcc_printf("[success] - %s, %d\r\n", __FUNCTION__, service_type);
    return EXT_ERR_SUCCESS;
}

ext_errno hcc_service_deinit(td_u8 chl, hcc_service_type service_type)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    hcc_serv_info *serv_info;
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc srv deinit null\r\n");
        return EXT_ERR_HCC_INIT_ERR;
    }
    serv_info = hcc_get_serv_info(hcc, service_type);
    if (serv_info == TD_NULL) {
        return EXT_ERR_SUCCESS;
    }
    hcc_list_free_by_service(hcc, service_type, TD_TRUE);
    serv_info->adapt = TD_NULL;
    hcc_del_service_list(hcc, service_type);
    return EXT_ERR_SUCCESS;
}

#ifdef CONFIG_HCC_SUPPORT_EXTEND_INTERFACE
td_bool hcc_service_is_busy(td_u8 chl, hcc_service_type service_type)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    td_u8 index;
    hcc_trans_queue *tx_queue = TD_NULL;
    hcc_trans_queue *rx_queue = TD_NULL;
    if (hcc == TD_NULL) {
        return TD_FALSE;
    }

    for (index = 0; index < hcc->que_max_cnt; index++) {
        tx_queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_TX][index];
        rx_queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_RX][index];
        if ((tx_queue->queue_ctrl->service_type == service_type && !hcc_is_list_empty(&tx_queue->queue_info)) ||
            (rx_queue->queue_ctrl->service_type == service_type && !hcc_is_list_empty(&rx_queue->queue_info))) {
            return TD_TRUE;
        }
    }
    return TD_FALSE;
}

td_void hcc_service_enable_switch(td_u8 chl, hcc_service_type service_type, td_bool enable)
{
    hcc_serv_info *serv_info = hcc_get_serv_info(hcc_get_handler(chl), service_type);
    if (serv_info != TD_NULL) {
        hcc_change_state(&serv_info->service_enable, enable);
    }
}

td_s32 hcc_service_get_credit(td_u8 chl, hcc_service_type serv, td_u32 *credit)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    if (bus == TD_NULL || credit == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    uapi_unused(serv);
    return hcc_bus_get_credit(bus, credit);
}

osal_module_export(hcc_service_get_credit);
osal_module_export(hcc_service_is_busy);
osal_module_export(hcc_service_enable_switch);
#endif

td_void hcc_service_update_credit(td_u8 chl, hcc_service_type serv, td_u32 credit)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    if (bus == TD_NULL) {
        return;
    }
    uapi_unused(serv);
    hcc_bus_update_credit(bus, credit);
}

hcc_serv_info *hcc_add_service_list(hcc_handler *hcc, hcc_service_type serv_type)
{
    hcc_service_list *service_new = TD_NULL;
    if (hcc_get_serv_info(hcc, serv_type) != TD_NULL) {
        return TD_NULL;
    }

    service_new = (hcc_service_list *)osal_kmalloc(sizeof(hcc_service_list) + sizeof(hcc_serv_info), OSAL_GFP_KERNEL);
    if (service_new == TD_NULL) {
        return TD_NULL;
    }
    service_new->service_info = (hcc_serv_info *)(service_new + 1);
    memset_s(service_new->service_info, sizeof(hcc_serv_info), 0, sizeof(hcc_serv_info));
    OSAL_INIT_LIST_HEAD(&service_new->service_list);
    osal_list_add_tail(&service_new->service_list, &hcc->hcc_serv.service_list);
    return service_new->service_info;
}

td_void hcc_del_service_list(hcc_handler *hcc, hcc_service_type serv_type)
{
    hcc_service_list *del = hcc_get_serv_list(hcc, serv_type);
    if (del == TD_NULL) {
        return;
    }
    osal_list_del(&del->service_list);
    del->service_info = TD_NULL;
    osal_kfree(del);
}

osal_module_export(hcc_service_update_credit);
osal_module_export(hcc_service_init);
osal_module_export(hcc_service_deinit);
