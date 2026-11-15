/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc dfx completion.
 * Author: CompanyName
 * Create: 2021-05-13
 */

#include "hcc_dfx.h"
#include "hcc_comm.h"
#include "soc_osal.h"
#include "securec.h"
#include "securectype.h"
#include "hcc_cfg_comm.h"
#include "hcc_list.h"
#include "hcc.h"
#include "hcc_channel.h"
#include "hcc_bus.h"
#include "hcc_if.h"
#include "hcc_flow_ctrl.h"
#include "common_def.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_DFX_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

ext_errno hcc_dfx_init(hcc_handler *hcc)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *tmp_hcc_dfx = (hcc_dfx *)osal_kzalloc(sizeof(hcc_dfx), OSAL_GFP_KERNEL);
    if (tmp_hcc_dfx == TD_NULL) {
        return EXT_ERR_MALLOC_FAILURE;
    }
    size_t que_stat_len = sizeof(hcc_queue_stat) * hcc->que_max_cnt;
    tmp_hcc_dfx->queue_stat[HCC_DIR_TX] = (hcc_queue_stat *)osal_kzalloc(que_stat_len, OSAL_GFP_KERNEL);
    if (tmp_hcc_dfx->queue_stat[HCC_DIR_TX] == TD_NULL) {
        osal_kfree(tmp_hcc_dfx);
        return EXT_ERR_MALLOC_FAILURE;
    }
    tmp_hcc_dfx->queue_stat[HCC_DIR_RX] = (hcc_queue_stat *)osal_kzalloc(que_stat_len, OSAL_GFP_KERNEL);
    if (tmp_hcc_dfx->queue_stat[HCC_DIR_RX] == TD_NULL) {
        osal_kfree(tmp_hcc_dfx);
        osal_kfree(tmp_hcc_dfx->queue_stat[HCC_DIR_TX]);
        return EXT_ERR_MALLOC_FAILURE;
    }
#ifdef CONFIG_HCC_SUPPORT_DFX_SRV
    size_t srv_stat_len = sizeof(hcc_service_stat) * hcc->srv_max_cnt;
    tmp_hcc_dfx->service_stat = (hcc_service_stat *)osal_kzalloc(srv_stat_len, OSAL_GFP_KERNEL);
    if (tmp_hcc_dfx->service_sta == TD_NULL) {
        osal_kfree(tmp_hcc_dfx->queue_stat[HCC_DIR_TX]);
        osal_kfree(tmp_hcc_dfx->queue_stat[HCC_DIR_RX]);
        osal_kfree(tmp_hcc_dfx);
        return EXT_ERR_MALLOC_FAILURE;
    }
#endif
    osal_spin_lock_init(&tmp_hcc_dfx->hcc_dfx_lock);
    tmp_hcc_dfx->que_max_cnt = hcc->que_max_cnt;
    tmp_hcc_dfx->srv_max_cnt = hcc->srv_max_cnt;
    hcc->dfx_handle = tmp_hcc_dfx;
#else
    uapi_unused(hcc);
#endif
    return EXT_ERR_SUCCESS;
}

#ifdef CONFIG_HCC_SUPPORT_DFX
STATIC hcc_dfx *hcc_get_dfx(hcc_handler *hcc)
{
    return hcc->dfx_handle;
}
#endif

td_void hcc_dfx_queue_total_pkt_increase(hcc_handler *hcc, hcc_service_type serv_type,
                                         hcc_queue_dir dir, hcc_queue_type q_id, td_u8 cnt)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (q_id >= hcc_dfx_stat->que_max_cnt || serv_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->queue_stat[dir][q_id].total_pkts += cnt;
#ifdef CONFIG_HCC_SUPPORT_DFX_SRV
    if (dir == HCC_DIR_TX) { /* 此处只记录tx的数量，rx的可以使用其他值 */
        hcc_dfx_stat->service_stat[serv_type].bus_tx_succ += cnt;
    }
#endif
#else
    uapi_unused(hcc);
    uapi_unused(serv_type);
    uapi_unused(dir);
    uapi_unused(q_id);
    uapi_unused(cnt);
#endif
}

td_void hcc_dfx_queue_loss_pkt_increase(hcc_handler *hcc, hcc_queue_dir dir, hcc_queue_type q_id)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (q_id >= hcc_dfx_stat->que_max_cnt) {
        return;
    }
    hcc_dfx_stat->queue_stat[dir][q_id].loss_pkts++;
#else
    uapi_unused(hcc);
    uapi_unused(dir);
    uapi_unused(q_id);
#endif
}

td_void hcc_dfx_service_alloc_cb_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].alloc_cb_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}

td_void hcc_dfx_service_alloc_cnt_increase(hcc_handler *hcc, hcc_service_type service_type,
                                           hcc_queue_type queue_id, td_bool success)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (queue_id >= hcc_dfx_stat->que_max_cnt || service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    if (success) {
        hcc_dfx_stat->queue_stat[HCC_DIR_RX][queue_id].alloc_succ_cnt++;
    } else {
        hcc_dfx_stat->queue_stat[HCC_DIR_RX][queue_id].alloc_fail_cnt++;
    }
#ifdef CONFIG_HCC_SUPPORT_DFX_SRV
    if (success) {
        hcc_dfx_stat->service_stat[service_type].alloc_succ_cnt++;
    } else {
        hcc_dfx_stat->service_stat[service_type].alloc_fail_cnt++;
    }
#endif
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
    uapi_unused(queue_id);
    uapi_unused(success);
#endif
}

td_void hcc_dfx_service_free_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].free_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}

td_void hcc_dfx_unc_alloc_cnt_increase(hcc_handler *hcc, hcc_queue_dir direction, td_bool success)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (success) {
        hcc_dfx_stat->inner_stat.unc_alloc_succ[direction]++;
    } else {
        hcc_dfx_stat->inner_stat.unc_alloc_fail[direction]++;
    }
#else
    uapi_unused(hcc);
    uapi_unused(direction);
    uapi_unused(success);
#endif
}

td_void hcc_dfx_mem_free_cnt_increase(hcc_handler *hcc)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    hcc_dfx_stat->inner_stat.mem_free_cnt++;
#else
    uapi_unused(hcc);
#endif
}

td_void hcc_dfx_unc_free_cnt_increase(hcc_handler *hcc)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    hcc_dfx_stat->inner_stat.unc_free_cnt++;
#else
    uapi_unused(hcc);
#endif
}

td_void hcc_dfx_service_total_increase(hcc_handler *hcc)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    hcc_dfx_stat->inner_stat.srv_rx_cnt++;
#else
    uapi_unused(hcc);
#endif
}

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
td_void hcc_dfx_service_startsubq_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].start_subq_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}

td_void hcc_dfx_service_stopsubq_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].stop_subq_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}
#endif

td_void hcc_dfx_service_rx_cnt_increase(hcc_handler *hcc, hcc_service_type service_type, hcc_queue_type queue_id)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (queue_id >= hcc_dfx_stat->que_max_cnt || service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
#ifdef CONFIG_HCC_SUPPORT_DFX_SRV
    hcc_dfx_stat->service_stat[service_type].rx_cnt++;
#endif
    hcc_dfx_stat->queue_stat[HCC_DIR_RX][queue_id].total_pkts++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
    uapi_unused(queue_id);
#endif
}

td_void hcc_dfx_service_exp_rx_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].exp_rx_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}

td_void hcc_dfx_service_rx_err_cnt_increase(hcc_handler *hcc, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_dfx *hcc_dfx_stat = hcc->dfx_handle;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }
    hcc_dfx_stat->service_stat[service_type].rx_err_cnt++;
#else
    uapi_unused(hcc);
    uapi_unused(service_type);
#endif
}

#ifdef CONFIG_HCC_SUPPORT_DFX
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
STATIC td_void hcc_dfx_queue_fc_info_print(TD_CONST hcc_trans_queue *queue)
{
    if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_DATA) {
        hcc_dfx_print("fc:%d\r\n", queue->fc_para.fc_back_para.flow_ctrl_open);
        hcc_dfx_print("fc flag:%d\r\n", queue->fc_para.fc_back_para.is_stopped);
        hcc_dfx_print("fc_on_cnt: %d\r\n", queue->fc_para.fc_back_para.fc_on_cnt);
        hcc_dfx_print("fc_off_cnt: %d\r\n", queue->fc_para.fc_back_para.fc_off_cnt);
    }
    if (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_CREDIT) {
        hcc_dfx_print("credit: %d, bottom value: %d\r\n",
            queue->fc_para.credit, queue->queue_ctrl->credit_bottom_value);
    }
}

STATIC td_void hcc_dfx_queue_ctrl_info_print(TD_CONST hcc_trans_queue *queue)
{
    uapi_unused(queue);
    hcc_dfx_print("service_type: %d\r\n", queue->queue_ctrl->service_type);
    hcc_dfx_print("transfer_mode: %d\r\n", queue->queue_ctrl->transfer_mode);
    hcc_dfx_print("fc_enable: %d\r\n", queue->queue_ctrl->fc_enable);
    hcc_dfx_print("flow_type: %d\r\n", queue->queue_ctrl->flow_type);
    hcc_dfx_print("low_waterline: %d\r\n", queue->queue_ctrl->low_waterline);
    hcc_dfx_print("high_waterline: %d\r\n", queue->queue_ctrl->high_waterline);
    hcc_dfx_print("burst_limit: %d\r\n", queue->queue_ctrl->burst_limit);
    hcc_dfx_print("credit_bottom_value: %d\r\n", queue->queue_ctrl->credit_bottom_value);
}
#endif
#endif

td_void hcc_dfx_queue_info_print(td_u8 chl, hcc_queue_dir dir, hcc_queue_type q_id)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_dfx *hcc_dfx_stat = hcc_get_dfx(hcc_get_handler(chl));
    hcc_trans_queue *queue;
    hcc_queue_stat *queue_stat;
    uapi_unused(queue_stat);
    if (q_id >= hcc_dfx_stat->que_max_cnt || dir > HCC_DIR_COUNT) {
        return;
    }

    queue = &(hcc_get_handler(chl)->hcc_resource.hcc_queues[dir][q_id]);
    queue_stat = &hcc_dfx_stat->queue_stat[dir][q_id];
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    hcc_dfx_queue_ctrl_info_print(queue);
    hcc_dfx_queue_fc_info_print(queue);
#endif
    hcc_dfx_print("current queue_len: %d\r\n", hcc_list_len(&queue->queue_info));
    hcc_dfx_print("total_pkts: %d\r\n", queue_stat->total_pkts);
    hcc_dfx_print("loss_pkts: %d\r\n", queue_stat->loss_pkts);
    hcc_dfx_print("alloc_succ_cnt: %d\r\n", queue_stat->alloc_succ_cnt);
    hcc_dfx_print("alloc_fail_cnt: %d\r\n", queue_stat->alloc_fail_cnt);
#else
    uapi_unused(chl);
    uapi_unused(dir);
    uapi_unused(q_id);
#endif
}

td_void hcc_proc_overnum_inc(td_void *hcc)
{
    hcc_handler *handle = (hcc_handler*)hcc;

    if (handle->dfx_handle) {
        handle->dfx_handle->inner_stat.proc_packet_overnum_cnt++;
    }
}

td_void hcc_tx_fail_inc(td_void *hcc)
{
    hcc_handler *handle = (hcc_handler*)hcc;

    if (handle->dfx_handle) {
        handle->dfx_handle->inner_stat.tx_fail_exit_cnt++;
    }
}

td_void hcc_dfx_service_info_print(td_u8 chl, hcc_service_type service_type)
{
#if defined(CONFIG_HCC_SUPPORT_DFX) && defined(CONFIG_HCC_SUPPORT_DFX_SRV)
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return;
    }
    hcc_dfx *hcc_dfx_stat = hcc_get_dfx(hcc);
    hcc_service_stat *serv_stat;
    if (service_type >= hcc_dfx_stat->srv_max_cnt) {
        return;
    }

    uapi_unused(serv_stat);
    serv_stat = &hcc_dfx_stat->service_stat[service_type];
    hcc_dfx_print("alloc_succ_cnt: %d\r\n", serv_stat->alloc_succ_cnt);
    hcc_dfx_print("alloc_cb_cnt: %d\r\n", serv_stat->alloc_cb_cnt);
    hcc_dfx_print("alloc_fail_cnt: %d\r\n", serv_stat->alloc_fail_cnt);
    hcc_dfx_print("free_cnt: %d\r\n", serv_stat->free_cnt);

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    hcc_dfx_print("start_subq_cnt: %d\r\n", serv_stat->start_subq_cnt);
    hcc_dfx_print("stop_subq_cnt: %d\r\n", serv_stat->stop_subq_cnt);
#endif
    hcc_dfx_print("exp_rx_cnt: %d\r\n", serv_stat->exp_rx_cnt);
    hcc_dfx_print("rx_cnt: %d\r\n", serv_stat->rx_cnt);
    hcc_dfx_print("bus_tx_succ: %d\r\n", serv_stat->bus_tx_succ);

    hcc_dfx_print("rx_err_cnt: %d\r\n", serv_stat->rx_err_cnt);
#else
    uapi_unused(chl);
    uapi_unused(service_type);
#endif
}

td_void hcc_bus_dfx_statics_print(td_u8 chl)
{
#ifdef CONFIG_HCC_SUPPORT_DFX
    hcc_handler *hcc = hcc_get_handler(chl);
    hcc_bus_dfx_f func;
    hcc_trans_queue *queue = TD_NULL;
    if (hcc == TD_NULL) {
        return;
    }
    hcc_inner_stat *inner_stat = &(hcc_get_dfx(hcc)->inner_stat);
    td_u8 i;
    hcc_queue_cfg *q_cfg;
    td_u32 queue_len;

    func = hcc->bus->hcc_bus_dfx;
    q_cfg = hcc->que_cfg;

    hcc_dfx_print("alloc_unc ok tx[%u],rx[%u]\r\n", inner_stat->unc_alloc_succ[HCC_DIR_TX],
        inner_stat->unc_alloc_succ[HCC_DIR_RX]);
    hcc_dfx_print("alloc_unc err tx[%u],rx[%u]\r\n", inner_stat->unc_alloc_fail[HCC_DIR_TX],
        inner_stat->unc_alloc_fail[HCC_DIR_RX]);
    hcc_dfx_print("free unc: %u\r\n", inner_stat->unc_free_cnt);
    hcc_dfx_print("free_mem: %u\r\n", inner_stat->mem_free_cnt);
    hcc_dfx_print("rx_srv: %u\r\n", inner_stat->srv_rx_cnt);
    hcc_dfx_print("tx_fail: %u, pkt_overnum_cnt:%u\r\n",
        inner_stat->tx_fail_exit_cnt, inner_stat->proc_packet_overnum_cnt);
    hcc_dfx_print("task_state: %d\r\n", hcc->hcc_resource.task_run);
    uapi_unused(inner_stat);

    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (q_cfg[i].queue_id >= hcc->que_max_cnt) {
            hcc_printf_err_log("hcc dfx q:%d\r\n", q_cfg[i].queue_id);
            continue;
        }

        queue = &hcc->hcc_resource.hcc_queues[q_cfg[i].dir][q_cfg[i].queue_id];
        queue_len = queue->queue_info.qlen;
        if (queue_len > 0) {
            hcc_dfx_print("queue id[%u] dir[%u] len[%u]\r\n", q_cfg[i].queue_id, q_cfg[i].dir, queue_len);
        }
        queue_len = queue->send_head.qlen;
        if (queue_len > 0) {
            hcc_dfx_print("send id[%u] len[%u]\r\n", q_cfg[i].queue_id, queue_len);
        }
    }
    if (func) {
        func();
    }
#else
    uapi_unused(chl);
#endif
}
