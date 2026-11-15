/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc channel.
 * Author: CompanyName
 * Create: 2021-09-11
 */

#include "hcc_channel.h"
#include "hcc_bus.h"
#include "hcc_if.h"
#include "osal_def.h"
#include "common_def.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_CHANNEL_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

typedef struct _hcc_handler_list_ {
    struct osal_list_head  handler_list;
    hcc_handler *handler;
} hcc_handler_list;

STATIC hcc_handler_list g_hcc_handler_list = {
    {&g_hcc_handler_list.handler_list, &g_hcc_handler_list.handler_list}, TD_NULL
};

#ifndef CONFIG_HCC_SUPPORT_MULTI_CHANNEL
hcc_handler *hcc_get_handler(td_u8 chl)
{
    return (g_hcc_handler_list.handler == TD_NULL ? TD_NULL :
            (g_hcc_handler_list.handler->channel_id != chl ? TD_NULL :
             g_hcc_handler_list.handler));
}

hcc_handler *hcc_get_bus_handler(td_u8 bus_type)
{
    if (g_hcc_handler_list.handler != TD_NULL) {
        if (g_hcc_handler_list.handler->bus != TD_NULL) {
            if (g_hcc_handler_list.handler->bus->bus_type == bus_type) {
                return g_hcc_handler_list.handler;
            }
        }
    }
    return TD_NULL;
}

td_s32 hcc_add_handler(hcc_handler *hcc)
{
    if (g_hcc_handler_list.handler != TD_NULL) {
        return EXT_ERR_HCC_HANDLER_REPEAT;
    }
    g_hcc_handler_list.handler = hcc;
    return EXT_ERR_SUCCESS;
}

td_void hcc_delete_handler(td_u8 chl)
{
    uapi_unused(chl);
    osal_kfree(g_hcc_handler_list.handler);
    g_hcc_handler_list.handler = TD_NULL;
}

#else
typedef td_void *(hcc_traverse_func)(hcc_handler_list *handler_list, td_u8 data);
STATIC td_void *hcc_traverse_handler_list(td_u8 data, hcc_traverse_func func)
{
    hcc_handler_list *handler_list = (hcc_handler_list *)(void *)g_hcc_handler_list.handler_list.next;
    td_void *res = TD_NULL;
    while (&handler_list->handler_list != &g_hcc_handler_list.handler_list) {
        if (handler_list->handler == TD_NULL) {
            handler_list = (hcc_handler_list *)(void *)handler_list->handler_list.next;
            continue;
        }

        res = func(handler_list, data);
        if (res != TD_NULL) {
            return res;
        }
        handler_list = (hcc_handler_list *)(void *)handler_list->handler_list.next;
    }
    return TD_NULL;
}

STATIC td_void *hcc_traverse_func_get_handler_list(hcc_handler_list *handler_list, td_u8 chl)
{
    if (handler_list == TD_NULL) {
        return TD_NULL;
    }

    if (handler_list->handler == TD_NULL) {
        return TD_NULL;
    }

    if (handler_list->handler->channel_id == chl) {
        return handler_list;
    }
    return TD_NULL;
}

STATIC td_void *hcc_traverse_func_get_bus_list(hcc_handler_list *handler_list, td_u8 bus_type)
{
    if (handler_list == TD_NULL) {
        return TD_NULL;
    }

    if (handler_list->handler == TD_NULL || handler_list->handler->bus == TD_NULL) {
        return TD_NULL;
    }

    if (handler_list->handler->bus->bus_type == bus_type) {
        return handler_list;
    }
    return TD_NULL;
}

hcc_handler *hcc_get_handler(td_u8 chl)
{
    hcc_handler_list *handler_list =
        (hcc_handler_list *)hcc_traverse_handler_list(chl, hcc_traverse_func_get_handler_list);
    if (handler_list != TD_NULL) {
        return handler_list->handler;
    }
    return TD_NULL;
}

hcc_handler *hcc_get_bus_handler(td_u8 bus_type)
{
    hcc_handler_list *handler_list =
        (hcc_handler_list *)hcc_traverse_handler_list(bus_type, hcc_traverse_func_get_bus_list);
    if (handler_list != TD_NULL) {
        return handler_list->handler;
    }
    return TD_NULL;
}

td_s32 hcc_add_handler(hcc_handler *hcc)
{
    hcc_handler_list *new = TD_NULL;
    if (hcc_get_handler(hcc->channel_id) != TD_NULL) {
        return EXT_ERR_HCC_HANDLER_REPEAT;
    }

    new = (hcc_handler_list *)osal_kmalloc(sizeof(hcc_handler_list), OSAL_GFP_KERNEL);
    if (new == TD_NULL) {
        return EXT_ERR_MALLOC_FAILUE;
    }

    new->handler_list.next = new->handler_list.prev = &new->handler_list;
    new->handler = hcc;
    osal_list_add_tail(&new->handler_list, &g_hcc_handler_list.handler_list);
    return EXT_ERR_SUCCESS;
}

td_void hcc_delete_handler(td_u8 chl)
{
    hcc_handler_list *handler_list =
        (hcc_handler_list *)hcc_traverse_handler_list(chl, hcc_traverse_func_get_handler_list);
    if (handler_list == TD_NULL) {
        return;
    }
    osal_list_del(&handler_list->handler_list);
    osal_kfree(handler_list->handler);
    handler_list->handler = TD_NULL;
    osal_kfree(handler_list);
}
#endif

td_bool hcc_get_state(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return TD_FALSE;
    }
    return (hcc->hcc_state == HCC_ON);
}

td_bool hcc_chan_is_busy(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return TD_FALSE;
    }

    if (hcc_bus_is_busy(hcc->bus, HCC_DIR_TX) || hcc_bus_is_busy(hcc->bus, HCC_DIR_RX)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

osal_module_export(hcc_chan_is_busy);
osal_module_export(hcc_get_state);
osal_module_export(hcc_get_handler);
