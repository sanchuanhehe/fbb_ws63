/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc flow ctrl module completion.
 * Author: CompanyName
 * Create: 2021-06-30
 */

#include "hcc_flow_ctrl.h"
#include "soc_osal.h"
#include "osal_def.h"
#include "common_def.h"

#include "hcc_comm.h"
#include "hcc_service.h"
#include "hcc_list.h"
#include "hcc.h"
#include "hcc_adapt.h"
#include "hcc_bus.h"
#include "hcc_dfx.h"
#include "hcc_channel.h"
#include "hcc_if.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_FLOW_CTRL_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

#define HCC_FLOWCTRL_DEFAULT_CREDIT_VALUE 20

td_bool fc_below_low_waterline(hcc_trans_queue *hcc_queue)
{
    return hcc_list_len(&hcc_queue->queue_info) < hcc_queue->queue_ctrl->low_waterline;
}

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
td_void fc_msg_init(td_u8 chl)
{
    hcc_message_register(chl, 0, FC_MSG_FLOWCTRL_ON, hcc_flowctrl_on_proc, (td_u8 *)hcc_get_handler(chl));
    hcc_message_register(chl, 0, FC_MSG_FLOWCTRL_OFF, hcc_flowctrl_off_proc, (td_u8 *)hcc_get_handler(chl));
    hcc_message_register(chl, 0, FC_MSG_FLOWCTRL_CHECK, hcc_flowctrl_check_proc, (td_u8 *)hcc_get_handler(chl));
}

td_bool hcc_flow_ctrl_sched_check(hcc_handler *hcc, hcc_trans_queue *queue)
{
    if (hcc_is_list_empty(&queue->queue_info)) {
        return TD_FALSE;
    }
    if (queue->queue_ctrl->fc_enable != 0) {
        if ((queue->queue_ctrl->flow_type == HCC_FLOWCTRL_DATA) &&
            (queue->fc_para.fc_back_para.flow_ctrl_open == 1)) {
            return TD_FALSE;
        }
        if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_CREDIT &&
            queue->fc_para.credit < queue->queue_ctrl->credit_bottom_value) {
            td_u32 credit;
            hcc_bus_get_credit(hcc->bus, &credit);
            queue->fc_para.credit = (td_u16)credit;
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

ext_errno hcc_flow_ctrl_process(hcc_handler *hcc, hcc_trans_queue *queue)
{
    if (queue->queue_ctrl->fc_enable == 0) {
        return EXT_ERR_HCC_FC_PROC_UNBLOCK;
    }

    if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_DATA &&
        (queue->fc_para.fc_back_para.flow_ctrl_open != 0)) {
        hcc_send_message(hcc->channel_id, FC_MSG_FLOWCTRL_CHECK, 0);
        hcc_printf("can't send data, flow ctrl data effect\r\n");
        return EXT_ERR_HCC_FC_PROC_BLOCK;
    } else if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_CREDIT) {
        td_u32 credit;
        if (hcc_bus_get_credit(hcc->bus, &credit) != EXT_ERR_SUCCESS) {
            return EXT_ERR_HCC_FC_PROC_BLOCK;
        } else {
            queue->fc_para.credit = (td_u16)credit;
        }

        if (queue->fc_para.credit < queue->queue_ctrl->credit_bottom_value) {
            hcc_printf("can't send data, flow ctrl credit effect\r\n");
            return EXT_ERR_HCC_FC_PROC_BLOCK;
        }
    }
    return EXT_ERR_HCC_FC_PROC_UNBLOCK;
}

td_u32 hcc_flowctrl_on_proc(td_u8 *data)
{
    hcc_handler *hcc = (hcc_handler *)data;
    td_u32 dir;
    td_u8 q_id;
    hcc_trans_queue *hcc_queue;
    unsigned long flags;
    hcc_printf("rx msg flow on\r\n");
    for (dir = 0; dir < HCC_DIR_COUNT; dir++) {
        for (q_id = 0; q_id < hcc->que_max_cnt; q_id++) {
            hcc_queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
            if (hcc_queue->queue_ctrl->flow_type != HCC_FLOWCTRL_DATA ||
                (hcc_queue->queue_ctrl->fc_enable == 0)) {
                continue;
            }
            if (hcc_queue->fc_para.fc_back_para.flow_ctrl_open == 0) {
                osal_spin_lock_irqsave(&hcc_queue->queue_info.data_queue_lock, &flags);
                hcc_queue->fc_para.fc_back_para.flow_ctrl_open = TD_TRUE;
                hcc_queue->fc_para.fc_back_para.fc_off_cnt++;
                osal_spin_unlock_irqrestore(&hcc_queue->queue_info.data_queue_lock, &flags);
                hcc_printf("flowctrl_flag on -> off, can't send\r\n");
            }
        }
    }
    return EXT_ERR_SUCCESS;
}

td_u32 hcc_flowctrl_off_proc(td_u8 *data)
{
    hcc_handler *hcc = (hcc_handler *)data;
    td_u32 dir;
    td_u8 q_id;
    hcc_trans_queue *hcc_queue;
    unsigned long flags;
    hcc_printf("rx msg flow off\r\n");
    for (dir = 0; dir < HCC_DIR_COUNT; dir++) {
        for (q_id = 0; q_id < hcc->que_max_cnt; q_id++) {
            hcc_queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
            if (hcc_queue->queue_ctrl->flow_type != HCC_FLOWCTRL_DATA ||
                (hcc_queue->queue_ctrl->fc_enable == 0)) {
                continue;
            }
            if (hcc_queue->fc_para.fc_back_para.flow_ctrl_open != 0) {
                osal_spin_lock_irqsave(&hcc_queue->queue_info.data_queue_lock, &flags);
                hcc_queue->fc_para.fc_back_para.flow_ctrl_open = TD_FALSE;
                hcc_queue->fc_para.fc_back_para.fc_on_cnt++;
                osal_spin_unlock_irqrestore(&hcc_queue->queue_info.data_queue_lock, &flags);
                hcc_printf("flowctrl_flag off -> on, start send\r\n");
                hcc_sched_transfer(hcc);
            }
        }
    }
    return EXT_ERR_SUCCESS;
}

td_u32 hcc_flowctrl_check_proc(td_u8 *data)
{
    hcc_handler *hcc = (hcc_handler *)data;
    if (hcc->bus != TD_NULL && hcc->bus->bus_ops != TD_NULL && hcc->bus->bus_ops->flow_off != TD_NULL) {
        hcc->bus->bus_ops->flow_off(hcc->bus, TD_TRUE);
    }
    return EXT_ERR_SUCCESS;
}
#endif

STATIC td_bool fc_above_high_waterline(hcc_trans_queue *hcc_queue)
{
    return hcc_list_len(&hcc_queue->queue_info) > hcc_queue->queue_ctrl->high_waterline;
}

STATIC td_s32 fc_wait_queue_cond(TD_CONST td_void *param)
{
    hcc_trans_queue *hcc_queue = (hcc_trans_queue *)param;
    return fc_below_low_waterline(hcc_queue);
}

STATIC td_void fc_tx_stop_subq(hcc_handler *hcc, hcc_trans_queue *hcc_queue)
{
    hcc_serv_info *serv_info = hcc_get_serv_info(hcc, hcc_queue->queue_ctrl->service_type);
    if (serv_info == TD_NULL) {
        return;
    }

    if (fc_above_high_waterline(hcc_queue) &&
        (hcc_queue->fc_para.fc_back_para.is_stopped == 0)) {
        td_ulong flags = 0;
        osal_spin_lock_irqsave(&hcc_queue->queue_info.data_queue_lock, &flags);
        hcc_printf("fc stop!\r\n");
        if (serv_info->adapt != TD_NULL) {
            if (serv_info->adapt->stop_subq != TD_NULL) {
                serv_info->adapt->stop_subq(hcc_queue->queue_id);
                hcc_dfx_service_stopsubq_cnt_increase(hcc, hcc_queue->queue_ctrl->service_type);
            }
            hcc_queue->fc_para.fc_back_para.is_stopped = TD_TRUE;
        }
        osal_spin_unlock_irqrestore(&hcc_queue->queue_info.data_queue_lock, &flags);
    }
}

td_void hcc_adapt_tx_start_subq(hcc_handler *hcc, hcc_trans_queue *hcc_queue)
{
    td_ulong flags = 0;
    hcc_serv_info *serv_info = hcc_get_serv_info(hcc, hcc_queue->queue_ctrl->service_type);
    hcc_flowctl_start_subq start_subq = TD_NULL;
    if (serv_info == TD_NULL) {
        return;
    }

    if (serv_info->adapt == TD_NULL) {
        return;
    }

    start_subq = serv_info->adapt->start_subq;
    osal_spin_lock_irqsave(&hcc_queue->queue_info.data_queue_lock, &flags);
    if (fc_below_low_waterline(hcc_queue)) {
        if (hcc_queue->fc_para.fc_back_para.is_stopped != 0) {
            hcc_queue->fc_para.fc_back_para.is_stopped = TD_FALSE;
            if (start_subq != TD_NULL) {
                start_subq(hcc_queue->queue_id);
                hcc_dfx_service_startsubq_cnt_increase(hcc, hcc_queue->queue_ctrl->service_type);
            }
        }
#ifndef CONFIG_HCC_SUPPORT_NON_OS
        osal_wait_wakeup(&hcc->hcc_resource.hcc_fc_wq);
#endif
    }
    osal_spin_unlock_irqrestore(&hcc_queue->queue_info.data_queue_lock, &flags);
}

#if (defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
ext_errno hcc_flow_ctrl_pre_proc(hcc_handler *hcc, hcc_transfer_param *param, hcc_trans_queue *hcc_queue)
#else
ext_errno hcc_flow_ctrl_pre_proc(hcc_handler *hcc, TD_CONST hcc_transfer_param *param, hcc_trans_queue *hcc_queue)
#endif
{
    if (param->fc_flag == HCC_FC_NONE) {
        return EXT_ERR_SUCCESS;
    }

#if (defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
    if (((param->fc_flag & HCC_FC_WAIT) != 0) && (osal_in_interrupt() != 0)) {
        /* when in interrupt,can't sched! */
        param->fc_flag &= ~HCC_FC_WAIT;
    }
#endif

#ifndef CONFIG_HCC_SUPPORT_NON_OS
    /* flow control process */
    /* Block if fc */
    if (((param->fc_flag & HCC_FC_WAIT) != 0) && (fc_wait_queue_cond(hcc_queue) == 0)) {
        td_s32 ret = osal_wait_timeout_interruptible(
            &hcc->hcc_resource.hcc_fc_wq, fc_wait_queue_cond, hcc_queue, HCC_FC_PRE_PROC_WAIT_TIME);
        if (ret == 0) {
            hcc_printf("[WARN]hcc flow control wait event timeout! too much time locked\r\n");
        } else if (ret < 0) {
            hcc_printf("wifi task was interrupted by a signal\r\n");
            return EXT_ERR_FAILURE;
        }
    }
#endif
    /* control net layer if fc */
    if ((param->fc_flag & HCC_FC_NET) != 0) {
        fc_tx_stop_subq(hcc, hcc_queue);
    }

    /* control net layer if fc */
    if ((param->fc_flag & HCC_FC_DROP) != 0) {
        /* 10 netbufs to buff */
        if (fc_above_high_waterline(hcc_queue)) {
            hcc_dfx_queue_loss_pkt_increase(hcc, HCC_DIR_TX, hcc_queue->queue_id);
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

ext_errno hcc_flow_ctrl_module_init(hcc_handler *hcc)
{
    td_u8 dir;
    td_u8 q_id;
    hcc_trans_queue *queue;
    if (hcc == TD_NULL) {
        return EXT_ERR_FAILURE;
    }

    for (dir = 0; dir < HCC_DIR_COUNT; dir++) {
        for (q_id = 0; q_id < hcc->que_max_cnt; q_id++) {
            queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
            queue->fc_para.fc_back_para.is_stopped = TD_FALSE;
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
            if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_DATA) {
                queue->fc_para.fc_back_para.flow_ctrl_open = TD_FALSE;
            }
            if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_CREDIT) {
                queue->fc_para.credit = HCC_FLOWCTRL_DEFAULT_CREDIT_VALUE;
            }
#endif
        }
    }

#ifndef CONFIG_HCC_SUPPORT_NON_OS
    if (osal_wait_init(&hcc->hcc_resource.hcc_fc_wq) != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("hcc fc init wait err\r\n");
        return EXT_ERR_FAILURE;
    }
#endif
    return EXT_ERR_SUCCESS;
}

td_void hcc_flow_ctrl_module_deinit(hcc_handler *hcc)
{
#ifdef CONFIG_HCC_SUPPORT_NON_OS
    uapi_unused(hcc);
#else
    osal_wait_destroy(&hcc->hcc_resource.hcc_fc_wq);
#endif
}

ext_errno hcc_flow_ctrl_set_water_line(td_u8 chl, hcc_queue_dir direction, td_u8 q_id,
    td_u8 low_line, td_u8 high_line)
{
    td_ulong flags;
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return EXT_ERR_HCC_HANDLER_ERR;
    }

    if (direction >= HCC_DIR_COUNT || q_id >= hcc->que_max_cnt) {
        return EXT_ERR_HCC_PARAM_ERR;
    }

    flags = 0;
    osal_spin_lock_irqsave(&hcc->hcc_resource.hcc_queues[direction][q_id].queue_info.data_queue_lock, &flags);
    hcc->hcc_resource.hcc_queues[direction][q_id].queue_ctrl->low_waterline  = low_line;
    hcc->hcc_resource.hcc_queues[direction][q_id].queue_ctrl->high_waterline = high_line;
    osal_spin_unlock_irqrestore(&hcc->hcc_resource.hcc_queues[direction][q_id].queue_info.data_queue_lock, &flags);
    return EXT_ERR_SUCCESS;
}

ext_errno hcc_flow_ctrl_get_water_line(td_u8 chl, hcc_queue_dir direction, td_u8 q_id,
    td_u8 *low_line, td_u8 *high_line)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return EXT_ERR_HCC_HANDLER_ERR;
    }

    if (direction >= HCC_DIR_COUNT || q_id >= hcc->que_max_cnt || low_line == TD_NULL || high_line == TD_NULL) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    *low_line = hcc->hcc_resource.hcc_queues[direction][q_id].queue_ctrl->low_waterline;
    *high_line = hcc->hcc_resource.hcc_queues[direction][q_id].queue_ctrl->high_waterline;
    return EXT_ERR_SUCCESS;
}

osal_module_export(hcc_flow_ctrl_set_water_line);
osal_module_export(hcc_flow_ctrl_get_water_line);
#endif

td_u32 hcc_flowctrl_check_with_off(td_u8 *data)
{
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
    hcc_handler *hcc = (hcc_handler *)data;
    if (hcc == TD_NULL || hcc->bus == TD_NULL || hcc->bus->bus_ops == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    if (hcc->bus != TD_NULL && hcc->bus->bus_ops != TD_NULL && hcc->bus->bus_ops->flow_off != TD_NULL) {
        hcc->bus->bus_ops->flow_off(hcc->bus, TD_FALSE);
    }
#endif
#endif
    unused(data);
    return EXT_ERR_SUCCESS;
}

hcc_flow_ctrl_tx_limit_cb g_hcc_flow_ctrl_tx_limit_fn = TD_NULL;
td_void hcc_flow_ctrl_tx_limit_register(hcc_flow_ctrl_tx_limit_cb cb)
{
    g_hcc_flow_ctrl_tx_limit_fn = cb;
}
 
td_void hcc_flow_ctrl_tx_limit_unregister(td_void)
{
    g_hcc_flow_ctrl_tx_limit_fn = TD_NULL;
}
 
td_u16 hcc_flow_ctrl_update_tx_limit(hcc_handler *hcc, hcc_trans_queue *queue, td_u16 remain_pkt_nums)
{
    static td_u16 tx_limit_max = 0;
    td_u16 tx_limit;
    hcc_flow_ctrl_tx_limit_cb callback = (hcc_flow_ctrl_tx_limit_cb)g_hcc_flow_ctrl_tx_limit_fn;
 
    unused(hcc);
    unused(queue);
    if (callback == TD_NULL) {
        return remain_pkt_nums;
    }
 
    if (tx_limit_max <= 1) {
        tx_limit_max = (td_u16)callback();
    }
 
    tx_limit = (remain_pkt_nums < tx_limit_max) ? remain_pkt_nums : tx_limit_max;
    tx_limit_max -= tx_limit;
    return tx_limit;
}
