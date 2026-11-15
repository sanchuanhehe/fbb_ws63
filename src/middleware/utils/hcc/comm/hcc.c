/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc module completion.
 * Author: CompanyName
 * Create: 2021-05-13
 */

#include "securec.h"
#include "soc_osal.h"
#include "osal_def.h"
#include "soc_errno.h"
#include "common_def.h"
#include "hcc_if.h"
#include "hcc_list.h"
#include "hcc_adapt.h"
#include "hcc_cfg_comm.h"
#include "hcc_comm.h"
#include "hcc_bus.h"
#include "hcc_bus_types.h"
#include "hcc_flow_ctrl.h"
#include "hcc_channel.h"
#include "hcc_service.h"
#include "hcc_test.h"
#include "hcc_dfx.h"
#include "hcc_ipc_adapt.h"
#include "hcc_rom_callback.h"
#include "hcc.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

enum {
    HCC_TASK_EXIT,
    HCC_TASK_INIT,
    HCC_TASK_WAIT,
    HCC_TASK_RUN,
};

td_s8 g_loglevel[BUS_LOG_SYMBOL_NUM][BUS_LOG_SYMBOL_SIZE] = { "[E]", "[W]", "[I]", "[D]", "[V]" };

td_u16 hcc_get_head_len(td_void)
{
    return sizeof(hcc_header);
}

td_void hcc_enable_rx_thread(td_u8 chl, td_bool enable)
{
    if (hcc_get_handler(chl) != TD_NULL) {
        hcc_printf("enable/disable rx thread: %d\r\n", enable);
        hcc_get_handler(chl)->hcc_resource.rx_thread_enable = enable;
    }
}

td_void hcc_enable_tx_thread(td_u8 chl, td_bool enable)
{
    if (hcc_get_handler(chl) != TD_NULL) {
        hcc_printf("enable/disable tx thread: %d\r\n", enable);
        hcc_get_handler(chl)->hcc_resource.tx_thread_enable = enable;
    }
}

STATIC td_bool hcc_sched_cond_check(hcc_handler *hcc, hcc_trans_queue *queue)
{
#if defined(CONFIG_HCC_SUPPORT_FLOW_CONTRL) && defined(CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE)
    return hcc_flow_ctrl_sched_check(hcc, queue);
#else
    uapi_unused(hcc);
    if (hcc_is_list_empty(&queue->queue_info)) {
        return TD_FALSE;
    } else {
        return TD_TRUE;
    }
#endif
}

td_s32 hcc_stop_xfer(td_u8 chl)
{
    td_u8 i;
    hcc_trans_queue *queue = TD_NULL;
    hcc_handler *hcc = hcc_get_handler(chl);
    hcc_queue_cfg *q_cfg;
    hcc_unc_struc *send_head = TD_NULL;
    hcc_service_type que_serv_type = HCC_SERVICE_TYPE_MAX;

    if (hcc == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    q_cfg = hcc->que_cfg;

    if (hcc->bus && hcc->bus->bus_ops && hcc->bus->bus_ops->stop_xfer) {
        hcc->bus->bus_ops->stop_xfer();
    }
    hcc_enable_switch(chl, TD_FALSE);
    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (q_cfg[i].queue_id >= hcc->que_max_cnt) {
            hcc_printf_err_log("hcc stop xfer q:%d\r\n", q_cfg[i].queue_id);
            continue;
        }
        que_serv_type = hcc_fuzzy_trans_queue_2_service(hcc, q_cfg[i].queue_id);
        queue = &hcc->hcc_resource.hcc_queues[q_cfg[i].dir][q_cfg[i].queue_id];
        hcc_list_free(hcc, &queue->queue_info);
        send_head = hcc_list_dequeue(&queue->send_head);
        if (send_head != TD_NULL) {
            /* service type不一致, 说明此头节点是dscr unc节点, 不释放, 反之是数据unc节点, 需要释放 */
            if (send_head->service_type == que_serv_type) {
                hcc_adapt_mem_free(hcc, send_head);
            }
            hcc_list_free(hcc, &queue->send_head);
        }
    }
    return EXT_ERR_SUCCESS;
}

STATIC td_bool hcc_check_queue_cond(hcc_handler *hcc, td_u8 dir, td_u8 q_id)
{
    hcc_trans_queue *queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
    hcc_printf("%s-%d, qid:%d-%d, qlen:%d\r\n",
        __FUNCTION__, __LINE__, dir, q_id, hcc->hcc_resource.hcc_queues[dir][q_id].queue_info.qlen);
    if (hcc_sched_cond_check(hcc, queue) == TD_TRUE) {
#ifdef CONFIG_HCC_SUPPORT_IPC_HOST
        if ((hcc->bus->bus_type == HCC_BUS_IPC) && hcc_ipc_tx_dma_is_busy(dir, queue)) {
            return TD_FALSE;
        }
#endif
        hcc_printf("queue not empty, condition true, dir-%d id -%d, len - %d\r\n",
                   dir, queue->queue_id, hcc_list_len(&queue->queue_info));
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

STATIC int hcc_thread_wait_queue_cond(TD_CONST td_void *param)
{
    td_u8 i;
    hcc_handler *hcc = (hcc_handler *)param;
    hcc_queue_cfg *queue_cfg;

    if (osal_kthread_should_stop() != 0) {
        return TD_TRUE;
    }

    if (hcc->bus == TD_NULL || hcc->hcc_state != HCC_ON || hcc->bus->state == TD_FALSE) {
        return TD_FALSE;
    }
    if (hcc->hcc_resource.task_run == HCC_TASK_EXIT) {
        return TD_TRUE;
    }
#if defined CONFIG_HCC_SUPPORT_NON_OS
    if (hcc->hcc_resource.tx_fail_cnt > hcc->hcc_resource.tx_fail_num_limit) {
        return TD_FALSE;
    }
#endif
    queue_cfg = hcc->que_cfg;
    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (hcc_check_queue_cond(hcc, queue_cfg[i].dir, queue_cfg[i].queue_id)) {
            return TD_TRUE;
        }
    }
    return TD_FALSE;
}

td_bool hcc_check_header_vaild(TD_CONST hcc_handler *hcc, TD_CONST hcc_header *hdr)
{
    if (hdr == TD_NULL) {
        hcc_printf_err_log("hcc check header hdr is null\r\n");
        return TD_FALSE;
    }

    if (hdr->service_type >= hcc->srv_max_cnt ||
        hdr->queue_id >= hcc->que_max_cnt) {
        hcc_printf_err_log("hcc_check_header_vaild S-%d, Q-%d failed\r\n", hdr->service_type, hdr->queue_id);
        return TD_FALSE;
    }

    return TD_TRUE;
}

hcc_service_type hcc_queue_id_2_service_type(hcc_handler *hcc, hcc_queue_dir dir, hcc_queue_type queue_id)
{
    if (hcc == TD_NULL) {
        return HCC_SERVICE_TYPE_MAX;
    }

    if (hcc->hcc_resource.hcc_queues[dir][queue_id].queue_ctrl == TD_NULL) {
        return HCC_SERVICE_TYPE_MAX;
    }

    return hcc->hcc_resource.hcc_queues[dir][queue_id].queue_ctrl->service_type;
}

hcc_service_type hcc_fuzzy_trans_queue_2_service(hcc_handler *hcc, hcc_queue_type queue_id)
{
    hcc_service_type serv = hcc_queue_id_2_service_type(hcc, HCC_DIR_TX, queue_id);
    if (serv < hcc->srv_max_cnt) {
        return serv;
    }
    return hcc_queue_id_2_service_type(hcc, HCC_DIR_RX, queue_id);
}

td_u32 hcc_get_transfer_packet_num(hcc_handler *hcc)
{
    if (hcc == TD_NULL) {
        return 0;
    }
    return hcc->hcc_resource.cur_trans_pkts;
}

td_u16 hcc_tx_queue_proc(hcc_handler *hcc, hcc_trans_queue *queue)
{
    hcc_data_queue *head = TD_NULL;
    hcc_bus *bus = TD_NULL;
    td_u16 max_send_pkt_nums;
    td_u16 remain_pkt_nums;
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc txproc is null\r\n");
        return 0;
    }
    head = &queue->queue_info;
    if (hcc_is_list_empty(head)) {
        return 0;
    }
    bus = hcc->bus;
    hcc_printf("txproc list len:%d - burst limit:%d\r\n", hcc_list_len(head), queue->queue_ctrl->burst_limit);
    max_send_pkt_nums = (td_u16)osal_min(hcc_list_len(head), (td_u32)queue->queue_ctrl->burst_limit);
    remain_pkt_nums = max_send_pkt_nums;

    if ((queue->queue_ctrl->fc_enable != 0) && (queue->queue_ctrl->flow_type == HCC_FLOWCTRL_LIMIT_TX)) {
        remain_pkt_nums = hcc_flow_ctrl_update_tx_limit(hcc, queue, remain_pkt_nums);
    }

    hcc_printf("txproc remain pkt nums: %d\r\n", remain_pkt_nums);
    while (remain_pkt_nums > 0) {
#if defined(CONFIG_HCC_SUPPORT_FLOW_CONTRL) && defined(CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE)
        if (hcc_flow_ctrl_process(hcc, queue) != EXT_ERR_HCC_FC_PROC_UNBLOCK) {
            break;
        }
#endif
        if (hcc_bus_tx_proc(bus, queue, &remain_pkt_nums) != EXT_ERR_SUCCESS) {
            hcc->hcc_resource.tx_fail_cnt++;
            break;
        }
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
        hcc_adapt_tx_start_subq(hcc, queue);
#endif
    }
    hcc_dfx_queue_total_pkt_increase(hcc, hcc_fuzzy_trans_queue_2_service(hcc, queue->queue_id), HCC_DIR_TX,
        queue->queue_id, (td_u8)(max_send_pkt_nums - remain_pkt_nums));
    return max_send_pkt_nums - remain_pkt_nums;
}

STATIC td_void hcc_rx_proc(hcc_handler *hcc, hcc_header *hcc_head, td_u8 *buf, td_u8 *user_param,
                           hcc_adapt_priv_rx_process rx_proc)
{
    hcc_rx_proc_cb rx_proc_cb = TD_NULL;
    rx_proc_cb = (hcc_rx_proc_cb)hcc_get_rom_cb(HCC_CB_RX_PROC);
    hcc_dfx_service_rx_cnt_increase(hcc, hcc_head->service_type, hcc_head->queue_id);
    if (rx_proc_cb != TD_NULL) {
        rx_proc_cb(hcc_head, buf, user_param, rx_proc);
    } else {
        rx_proc(hcc_head->queue_id, hcc_head->sub_type, buf, hcc_head->pay_len, user_param);
    }
    hcc_dfx_service_total_increase(hcc);
}

td_u16 hcc_rx_queue_proc(hcc_handler *hcc, hcc_trans_queue *queue)
{
    td_s32 ret = EXT_ERR_SUCCESS;
    hcc_unc_struc *unc_buf = TD_NULL;
    hcc_header *hcc_head = TD_NULL;
    hcc_serv_info *serv_info = TD_NULL;
    hcc_data_queue *head = &queue->queue_info;
    td_u16 rx_queue_cnt = 0;
    hcc_rx_data_cb rx_data_cb = TD_NULL;

    if (hcc_is_list_empty(head)) {
        return 0;
    }
    rx_data_cb = (hcc_rx_data_cb)hcc_get_rom_cb(HCC_CB_RX_DATA);
    if (rx_data_cb != TD_NULL) {
        if (rx_data_cb(hcc, queue, &rx_queue_cnt) != EXT_ERR_HCC_ROMBLE_HOOK_CONTINUE) {
            return rx_queue_cnt;
        }
    }

    unc_buf = hcc_list_dequeue(head);
    /* 依次处理队列中每个netbuf */
    while (unc_buf != TD_NULL) {
        hcc_head = (hcc_header *)unc_buf->buf;
        serv_info = hcc_get_serv_info(hcc, hcc_head->service_type);
        ret = EXT_ERR_FAILURE;
        if (serv_info == TD_NULL || serv_info->adapt == TD_NULL || !hcc_check_header_vaild(hcc, hcc_head)) {
            hcc_printf_err_log("hcc queue rx srv[%d] null\r\n", hcc_head->service_type);
            break;
        }

        ret = EXT_ERR_SUCCESS;
        hcc_dfx_service_exp_rx_cnt_increase(hcc, hcc_head->service_type);
        hcc_printf("[HCC]Rx Que-%d, srv-%d, len-%d\r\n", queue->queue_id, hcc_head->service_type, hcc_head->pay_len);
        if (serv_info->adapt->rx_proc != TD_NULL) {
            hcc_rx_proc(hcc, hcc_head, unc_buf->buf, unc_buf->user_param, serv_info->adapt->rx_proc);
            hcc_free_unc_buf(hcc, unc_buf);
        } else {
            hcc_adapt_mem_free(hcc, unc_buf);
            hcc_dfx_service_rx_err_cnt_increase(hcc, hcc_head->service_type);
            hcc_dfx_queue_loss_pkt_increase(hcc, HCC_DIR_RX, queue->queue_id);
        }
        if (rx_queue_cnt++ >= queue->queue_ctrl->burst_limit) {
            break;
        }
        unc_buf = hcc_list_dequeue(head);
    }

    if (ret != EXT_ERR_SUCCESS) {
        hcc_adapt_mem_free(hcc, unc_buf);
        hcc_dfx_queue_loss_pkt_increase(hcc, HCC_DIR_RX, queue->queue_id);
    }
    return rx_queue_cnt;
}

td_void hcc_change_state(osal_atomic *atomic, td_u32 state)
{
    td_s32 old_state;
    td_s32 new_state;
    if (atomic == TD_NULL) {
        return;
    }
    old_state = osal_atomic_read(atomic);
    osal_atomic_set(atomic, (td_s32)state);
    new_state = osal_atomic_read(atomic);
    if (old_state != new_state) {
        hcc_printf("state [%d]=>[%d]\r\n", old_state, new_state);
    }
}

td_u32 hcc_check_overrun(hcc_handler *hcc)
{
#if defined CONFIG_HCC_SUPPORT_NON_OS
    if (hcc->hcc_resource.cur_trans_pkts >= hcc->hcc_resource.max_proc_packets_per_loop) {
        hcc_proc_overnum_inc((td_void*)hcc);
        return EXT_ERR_FAILURE;
    }
#else
    uapi_unused(hcc);
#endif
    return EXT_ERR_SUCCESS;
}

STATIC td_void hcc_queue_process(hcc_handler *hcc)
{
    td_u8 i;
    hcc_trans_queue *queue = TD_NULL;
    hcc_queue_cfg *q_cfg = hcc->que_cfg;
    td_u16 pkt_proc = 0;

    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (q_cfg[i].queue_id >= hcc->que_max_cnt) {
            hcc_printf_err_log("hcc thread err qid:%d\r\n", q_cfg[i].queue_id);
            continue;
        }

        queue = &hcc->hcc_resource.hcc_queues[q_cfg[i].dir][q_cfg[i].queue_id];
        hcc_printf("proc: dir - %d, qID - %d\r\n", q_cfg[i].dir, q_cfg[i].queue_id);
        switch (q_cfg[i].dir) {
            case HCC_DIR_TX:
                pkt_proc = hcc_tx_queue_proc(hcc, queue);
                break;

            case HCC_DIR_RX:
                pkt_proc = hcc_rx_queue_proc(hcc, queue);
                break;

            default:
                pkt_proc = 0;
                hcc_printf_err_log("hcc thread dir err qid:%d\r\n", q_cfg[i].dir);
                break;
        }
        hcc->hcc_resource.cur_trans_pkts += pkt_proc;
    }
}

td_s32 hcc_resume_xfer(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);

    if (hcc->bus && hcc->bus->bus_ops && hcc->bus->bus_ops->resume_xfer) {
        hcc->bus->bus_ops->resume_xfer();
    }
    hcc_enable_switch(chl, TD_TRUE);
    return EXT_ERR_SUCCESS;
}

#if defined CONFIG_HCC_SUPPORT_NON_OS
td_void hcc_handle_tx_fail(hcc_handler *hcc)
{
    if (hcc->hcc_resource.tx_fail_cnt > hcc->hcc_resource.tx_fail_num_limit) {
        hcc_tx_fail_inc((td_void*)hcc);
    }
}
#endif

td_void hcc_list_free_by_service(hcc_handler *hcc, hcc_service_type serv_type, td_bool is_filter_by_serv_type)
{
    td_u8 i;
    hcc_trans_queue *queue = TD_NULL;
    hcc_queue_cfg *q_cfg;
    hcc_unc_struc *send_head = TD_NULL;
    hcc_service_type que_serv_type = HCC_SERVICE_TYPE_MAX;

    if (hcc == TD_NULL) {
        return;
    }
    q_cfg = hcc->que_cfg;

    for (i = 0; i < (hcc->que_max_cnt << 1); i++) {
        if (q_cfg[i].queue_id >= hcc->que_max_cnt) {
            hcc_printf_err_log("hcc_list_free_by_service q:%d\r\n", q_cfg[i].queue_id);
            continue;
        }
        que_serv_type = hcc_fuzzy_trans_queue_2_service(hcc, q_cfg[i].queue_id);
        if (is_filter_by_serv_type == TD_TRUE && serv_type != que_serv_type) {
            continue;
        }
        queue = &hcc->hcc_resource.hcc_queues[q_cfg[i].dir][q_cfg[i].queue_id];
        hcc_list_free(hcc, &queue->queue_info);
        send_head = hcc_list_dequeue(&queue->send_head);
        if (send_head != TD_NULL) {
            /* service type不一致, 说明此头节点是dscr unc节点, 不释放, 反之是数据unc节点, 需要释放 */
            if (send_head->service_type == que_serv_type) {
                hcc_adapt_mem_free(hcc, send_head);
            }
            hcc_list_free(hcc, &queue->send_head);
        }
    }
    return;
}

td_s32 hcc_transfer_thread(td_void *data)
{
    hcc_handler *hcc = (hcc_handler *)data;
    hcc_thread_watchdog watchdog = hcc_get_rom_cb(HCC_CB_WATCHDOG);
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc thread null\r\n");
        return EXT_ERR_FAILURE;
    }
    hcc->hcc_resource.tx_fail_cnt = 0;
    hcc->hcc_resource.cur_trans_pkts = 0;
    while (1) {
        hcc->hcc_resource.task_run = HCC_TASK_WAIT;
        if (watchdog != TD_NULL) {
            watchdog();
        }
#if defined CONFIG_HCC_SUPPORT_NON_OS
        if (hcc_thread_wait_queue_cond(hcc) == TD_FALSE) {
            hcc_handle_tx_fail(hcc);
            return 0;
        }
#else
        osal_wait_interruptible(&hcc->hcc_resource.hcc_transfer_wq, hcc_thread_wait_queue_cond, hcc);
        if (osal_kthread_should_stop() != 0) {
            break;
        }
#endif

        hcc_printf("[INFO] - %s, evt wait success\r\n", __FUNCTION__);
        hcc->hcc_resource.task_run = HCC_TASK_RUN;
        hcc_queue_process(hcc);
        if (hcc_check_overrun(hcc) != EXT_ERR_SUCCESS) {
            return 0;
        }
    }
    hcc_list_free_by_service(hcc, 0, TD_FALSE);
    hcc_printf("hcc transfer thread done!\r\n");
    return EXT_ERR_SUCCESS;
}

ext_errno hcc_message_register(td_u8 chl, td_u8 rsv, hcc_rx_msg_type msg_id, hcc_msg_rx cb, td_u8 *cb_data)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    struct bus_msg_stru *message = TD_NULL;
    if (bus == TD_NULL || msg_id >= HCC_RX_MAX_MESSAGE || cb == TD_NULL) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    uapi_unused(rsv);
    message = &bus->msg[msg_id];
    message->msg_rx = cb;
    message->data = cb_data;
    hcc_printf("[success] hcc_message_register - %d\r\n", msg_id);
    osal_atomic_set(&message->count, 0);
    return EXT_ERR_SUCCESS;
}

ext_errno hcc_message_unregister(td_u8 chl, td_u8 rsv, hcc_rx_msg_type msg_id)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    struct bus_msg_stru *message = TD_NULL;
    if (bus == TD_NULL || msg_id >= HCC_RX_MAX_MESSAGE) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    uapi_unused(rsv);
    message = &bus->msg[msg_id];
    message->msg_rx = TD_NULL;
    message->data = TD_NULL;
    return EXT_ERR_SUCCESS;
}

STATIC hcc_queue_ctrl *hcc_get_queue_ctrl_from_cfg(hcc_handler *hcc, hcc_queue_dir dir, hcc_queue_type q_id)
{
    td_u8 index;
    hcc_queue_cfg *que_cfg = hcc->que_cfg;
    for (index = 0; index < (hcc->que_max_cnt << 1); index++) {
        if (que_cfg[index].dir != dir || que_cfg[index].queue_id != q_id) {
            continue;
        }

        return &que_cfg[index].queue_ctrl;
    }
    return TD_NULL;
}

STATIC td_bool hcc_check_queue_cfg(TD_CONST hcc_queue_cfg *queue_cfg, td_u32 queue_len)
{
    td_u32 que_cnt[HCC_DIR_COUNT] = {0};
    td_u32 i;

    for (i = 0; i < queue_len; i++) {
        if (queue_cfg[i].dir < HCC_DIR_COUNT) {
            que_cnt[queue_cfg[i].dir]++;
        } else {
            return TD_FALSE;
        }
    }
    return (que_cnt[HCC_DIR_TX] == que_cnt[HCC_DIR_RX]);
}

STATIC td_s32 hcc_transfer_queue_init(hcc_handler *hcc, hcc_queue_cfg *queue_cfg, td_u32 queue_len)
{
    td_u8 dir;
    td_u8 q_id;
    hcc_trans_queue *hcc_queue = TD_NULL;
    td_u32 queue_size;

    hcc->que_cfg = queue_cfg;
    hcc->que_max_cnt = (td_u8)(queue_len >> 1);
    queue_size = (td_u32)(sizeof(hcc_trans_queue) * hcc->que_max_cnt);
    if (!hcc_check_queue_cfg(queue_cfg, queue_len)) {
        return EXT_ERR_FAILURE;
    }
    hcc->hcc_resource.hcc_queues[HCC_DIR_TX] = osal_kzalloc(queue_size, OSAL_GFP_KERNEL);
    hcc->hcc_resource.hcc_queues[HCC_DIR_RX] = osal_kzalloc(queue_size, OSAL_GFP_KERNEL);
    if ((hcc->hcc_resource.hcc_queues[HCC_DIR_TX] == TD_NULL) || hcc->hcc_resource.hcc_queues[HCC_DIR_RX] == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    for (dir = 0; dir < HCC_DIR_COUNT; dir++) {
        for (q_id = 0; q_id < hcc->que_max_cnt; q_id++) {
            hcc_queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
            hcc_list_head_init(&hcc_queue->queue_info);
            hcc_queue->queue_id = q_id;
            hcc_queue->queue_ctrl = hcc_get_queue_ctrl_from_cfg(hcc, dir, q_id);
            if (hcc_queue->queue_ctrl == TD_NULL) {
                return EXT_ERR_FAILURE;
            }

            if (hcc_list_head_init(&hcc_queue->send_head) != EXT_ERR_SUCCESS) {
                return EXT_ERR_FAILURE;
            }
        }
    }
    return EXT_ERR_SUCCESS;
}

STATIC td_void hcc_transfer_queue_deinit(hcc_handler *hcc)
{
    td_u8 dir;
    td_u8 q_id;
    hcc_trans_queue *hcc_queue = TD_NULL;

    if ((hcc->hcc_resource.hcc_queues[HCC_DIR_TX] != TD_NULL) && hcc->hcc_resource.hcc_queues[HCC_DIR_RX] == TD_NULL) {
        for (dir = 0; dir < HCC_DIR_COUNT; dir++) {
            for (q_id = 0; q_id < hcc->que_max_cnt; q_id++) {
                hcc_queue = &hcc->hcc_resource.hcc_queues[dir][q_id];
                hcc_list_head_deinit(&hcc_queue->queue_info);
                hcc_list_head_deinit(&hcc_queue->send_head);
            }
        }
    } else {
        if (hcc->hcc_resource.hcc_queues[HCC_DIR_TX] != TD_NULL) {
            osal_kfree(hcc->hcc_resource.hcc_queues[HCC_DIR_TX]);
            hcc->hcc_resource.hcc_queues[HCC_DIR_TX] = TD_NULL;
        }
        if (hcc->hcc_resource.hcc_queues[HCC_DIR_RX] != TD_NULL) {
            osal_kfree(hcc->hcc_resource.hcc_queues[HCC_DIR_TX]);
            hcc->hcc_resource.hcc_queues[HCC_DIR_RX] = TD_NULL;
        }
    }
}

STATIC td_void hcc_service_resource_init(hcc_handler *hcc)
{
    OSAL_INIT_LIST_HEAD(&hcc->hcc_serv.service_list);
    hcc->hcc_serv.service_info = TD_NULL;
}

ext_errno hcc_sched_transfer(hcc_handler *hcc)
{
#ifndef CONFIG_HCC_SUPPORT_NON_OS
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc sched is null\r\n");
        return EXT_ERR_FAILURE;
    }
    hcc_printf("sched hcc transfer\r\n");
    osal_wait_wakeup(&hcc->hcc_resource.hcc_transfer_wq);
#else
    uapi_unused(hcc);
#endif
    return EXT_ERR_SUCCESS;
}

td_void hcc_enable_switch(td_u8 chl, td_bool enable)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc != TD_NULL) {
        hcc->hcc_state = (td_u8)enable;
    }
}

#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
STATIC ext_errno hcc_set_unc_pool(hcc_data_queue *unc_pool, td_u32 unc_pool_size)
{
    td_u32 i;
    hcc_unc_struc *unc;

    hcc_list_head_init(unc_pool);
    unc = (hcc_unc_struc *)(unc_pool + 1);

    for (i = 0; i < unc_pool_size; i++) {
        hcc_list_add_tail(unc_pool, &unc[i]);
    }
    hcc_printf("%s success\r\n", __FUNCTION__);
    return EXT_ERR_SUCCESS;
}

STATIC td_void hcc_release_unc_pool(hcc_data_queue *unc_pool)
{
    if (unc_pool == TD_NULL) {
        return;
    }
    osal_spin_lock_destroy(&unc_pool->data_queue_lock);
}
#endif

STATIC ext_errno hcc_task_init(hcc_handler *hcc, td_char *task_name, td_u8 task_pri)
{
#ifdef CONFIG_HCC_SUPPORT_NON_OS
    /* non os 不需要创建task */
    hcc->hcc_resource.rx_thread_enable = TD_FALSE;
    hcc->hcc_resource.tx_thread_enable = TD_FALSE;
    hcc->hcc_resource.task_run = HCC_TASK_INIT;
    uapi_unused(task_name);
    uapi_unused(task_pri);
#else
    td_char *hcc_task_name;
    if (task_name == TD_NULL || strlen(task_name) > HCC_TASK_NAME_MAX_LEN) {
        hcc_task_name = "hcc_task";
    } else {
        hcc_task_name = task_name;
    }
    if (osal_wait_init(&hcc->hcc_resource.hcc_transfer_wq) != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("hcc task wait err\r\n");
        return EXT_ERR_FAILURE;
    }

    /* 在任务创建之前赋值 */
    hcc->hcc_resource.task_run = HCC_TASK_INIT;
    hcc->hcc_resource.hcc_transfer_thread_handler = osal_kthread_create(
        hcc_transfer_thread, (td_void *)hcc, hcc_task_name, HCC_TRANS_THREAD_TASK_STACK_SIZE);
    if (hcc->hcc_resource.hcc_transfer_thread_handler == TD_NULL) {
        hcc_printf_err_log("hcc thread create failed!\r\n");
        return EXT_ERR_FAILURE;
    }
    osal_kthread_set_priority(hcc->hcc_resource.hcc_transfer_thread_handler, task_pri);
    /* 默认使用hcc task处理队列中的数据 */
    hcc->hcc_resource.rx_thread_enable = TD_TRUE;
    hcc->hcc_resource.tx_thread_enable = TD_TRUE;
#endif
    return EXT_ERR_SUCCESS;
}

STATIC td_void hcc_task_exit(hcc_handler *hcc)
{
    hcc->hcc_resource.task_run = HCC_TASK_EXIT;
#ifdef CONFIG_HCC_SUPPORT_NON_OS
    return;
#else
    osal_kthread_destroy(hcc->hcc_resource.hcc_transfer_thread_handler, TD_TRUE);
    osal_wait_destroy(&hcc->hcc_resource.hcc_transfer_wq);
#endif
}

STATIC td_void hcc_module_exit(hcc_handler *hcc)
{
    hcc_task_exit(hcc);
    hcc_debug("hcc_task_exit...\r\n");
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    hcc_flow_ctrl_module_deinit(hcc);
#endif
    hcc_transfer_queue_deinit(hcc);
#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
    hcc_release_unc_pool(hcc->unc_pool_head);
#endif
    hcc_delete_handler(hcc->channel_id);
    hcc_printf("[success] - %s\r\n", __FUNCTION__);
}

STATIC hcc_handler *hcc_module_init_error(hcc_handler *hcc, hcc_module_init_errno num)
{
    // 初始化过程导致函数过长，使用errno替换goto标签，并记录错误码
    switch (num) {
        case HCC_MODULE_INIT_FAILED_ADD_HANDLER:
            hcc_task_exit(hcc);
            break;
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
        case HCC_MODULE_INIT_FAILED_INIT_TASK:
            hcc_flow_ctrl_module_deinit(hcc);
            break;
#endif
        case HCC_MODULE_INIT_FAILED_INIT_FLOW_CTRL:
            hcc_transfer_queue_deinit(hcc);
            break;
#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
        case HCC_MODULE_INIT_FAILED_INIT_QUEUE:
            hcc_release_unc_pool(hcc->unc_pool_head);
            break;
#endif
        case HCC_MODULE_INIT_FAILED_INIT_UNC_POOL:
            break;
        case HCC_MODULE_INIT_FAILED_SET_STATE:
            break;
        case HCC_MODULE_INIT_FAILED_INIT_STATE:
            break;
        default:
            break;
    }
    hcc_printf_err_log("hcc module init err:%d\r\n", num);
    osal_kfree((td_u8 *)hcc);
    return TD_NULL;
}

STATIC hcc_handler *hcc_module_mem_alloc(td_u32 unc_pool_size, td_u8 bus_type)
{
    hcc_handler *hcc;
    size_t total_len = 0;
#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
    total_len = (td_u32)(sizeof(hcc_unc_struc) * unc_pool_size +
        sizeof(hcc_data_queue) + sizeof(hcc_handler));
    hcc = (hcc_handler *)osal_kzalloc(total_len, OSAL_GFP_KERNEL);
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc module malloc err\r\n");
        return hcc;
    }
    hcc->unc_pool_head = (hcc_data_queue *)(hcc + 1);
#else
    uapi_unused(unc_pool_size);
    hcc = (hcc_handler *)osal_kzalloc(sizeof(hcc_handler), OSAL_GFP_KERNEL);
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc module malloc err\r\n");
        return hcc;
    }
#endif

    hcc->channel_id = bus_type;
    return hcc;
}

STATIC hcc_handler *hcc_module_init(hcc_channel_param *init)
{
    hcc_handler *hcc = hcc_module_mem_alloc(init->unc_pool_size, init->bus_type);
    if (hcc == TD_NULL) {
        return hcc;
    }

    hcc->hcc_state = HCC_OFF;
#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
    if (hcc_set_unc_pool(hcc->unc_pool_head, init->unc_pool_size) != EXT_ERR_SUCCESS) {
        return hcc_module_init_error(hcc, HCC_MODULE_INIT_FAILED_INIT_UNC_POOL);
    }
    hcc->unc_low_limit = init->unc_pool_low_limit;
#endif
    if (hcc_transfer_queue_init(hcc, init->queue_cfg, init->queue_len) != EXT_ERR_SUCCESS) {
        return hcc_module_init_error(hcc, HCC_MODULE_INIT_FAILED_INIT_QUEUE);
    }

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    if (hcc_flow_ctrl_module_init(hcc) != EXT_ERR_SUCCESS) {
        return hcc_module_init_error(hcc, HCC_MODULE_INIT_FAILED_INIT_FLOW_CTRL);
    }
#endif
    hcc_service_resource_init(hcc);
    if (hcc_task_init(hcc, init->task_name, init->task_pri) != EXT_ERR_SUCCESS) {
        return hcc_module_init_error(hcc, HCC_MODULE_INIT_FAILED_INIT_TASK);
    }

    if (hcc_add_handler(hcc) != EXT_ERR_SUCCESS) {
        return hcc_module_init_error(hcc, HCC_MODULE_INIT_FAILED_ADD_HANDLER);
    }
    hcc->hcc_state = HCC_ON;
    hcc->srv_max_cnt = init->service_max_cnt;
    hcc->hcc_resource.max_proc_packets_per_loop = init->max_proc_packets_per_loop;
    hcc->hcc_resource.tx_fail_num_limit = init->tx_fail_num_limit;
    hcc_printf("[success] - %s\r\n", __FUNCTION__);
    return hcc;
}

td_void hcc_deinit(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return;
    }
    hcc_bus_unload(hcc->bus);
    hcc_module_exit(hcc);
    hcc_debug("hcc deinit succ\r\n");
}

STATIC ext_errno hcc_adapt_tx_param_check(hcc_handler *hcc, TD_CONST td_u8 *buf, td_u16 len,
    TD_CONST hcc_transfer_param *param)
{
    hcc_serv_info *serv_info = hcc_get_serv_info(hcc, param->service_type);
    if (hcc->bus == TD_NULL || serv_info == TD_NULL) {
        hcc_printf_err_log("hcc tx check srv[%d] or bus chanid[%d] null\r\n", param->service_type, hcc->channel_id);
        return EXT_ERR_FAILURE;
    }

    if (param->service_type >= hcc->srv_max_cnt ||
        hcc_fuzzy_trans_queue_2_service(hcc, param->queue_id) != param->service_type) {
        hcc_printf_err_log("hcc tx check srv t-%d, q-%d\r\n", param->service_type, param->queue_id);
        return EXT_ERR_FAILURE;
    }

    if ((param->fc_flag & (~HCC_FC_ALL)) != 0) {
        hcc_printf_err_log("hcc tx check fc-%d\r\n", param->fc_flag);
        return EXT_ERR_FAILURE;
    }

    if (param->queue_id >= hcc->que_max_cnt) {
        hcc_printf_err_log("hcc tx check qid-%d\r\n", param->queue_id);
        return EXT_ERR_FAILURE;
    }

    if (osal_atomic_read(&serv_info->service_enable) != HCC_ENABLE) {
        hcc_printf_err_log("hcc tx check srv[%d] null\r\n", param->service_type);
        return EXT_ERR_FAILURE;
    }

    if (!osal_is_aligned((uintptr_t)buf, hcc->bus->addr_align)) {
        return EXT_ERR_FAILURE;
    }

    if (len <= hcc_get_head_len() || !osal_is_aligned(len, hcc->bus->len_align) ||
        len > hcc->bus->max_trans_size) {
        hcc_printf_err_log("hcc tx check err: len[%d] min[%d] max[%d] aligned[%d]\r\n",
                           len, hcc_get_head_len(), hcc->bus->max_trans_size, hcc->bus->len_align);
        return EXT_ERR_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void hcc_header_init(hcc_header *head, td_u16 len, hcc_transfer_param *param)
{
    /* fill the hcc header */
    head->service_type = (td_u8)param->service_type;
    head->sub_type = (td_u8)param->sub_type;
    head->queue_id = (td_u8)param->queue_id;
    head->pay_len = len;
}

td_u8 hcc_init(hcc_channel_param *init)
{
    td_u32 ret = EXT_ERR_SUCCESS;
    hcc_handler *hcc;

    if (init == TD_NULL) {
        return HCC_CHANNEL_INVALID;
    }
    hcc = hcc_get_bus_handler(init->bus_type);
    if ((hcc != TD_NULL) || (init->queue_cfg == TD_NULL) || (init->queue_len == 0)) {
        hcc_printf_err_log("hcc init param err\r\n");
        return HCC_CHANNEL_INVALID;
    }
    hcc = hcc_module_init(init);
    if (hcc == TD_NULL) {
        hcc_printf_err_log("hcc init module init err\r\n");
        return HCC_CHANNEL_INVALID;
    }
#ifdef CONFIG_HCC_SUPPORT_DFX
    ret = hcc_dfx_init(hcc);
    if (ret != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("hcc init dfx init, ret %x\r\n", ret);
        return HCC_CHANNEL_INVALID;
    }
#endif
    ret = hcc_bus_load(init->bus_type, hcc);
    if (ret != EXT_ERR_SUCCESS) {
        hcc_module_exit(hcc);
        hcc_printf_err_log("hcc init bus load fail\r\n");
        return HCC_CHANNEL_INVALID;
    }
#ifdef CONFIG_HCC_SUPPORT_TEST
    hcc_test_msg_init();
#endif
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
    fc_msg_init(hcc->channel_id);
#endif
    hcc_debug("hcc init succ\r\n");
    return hcc->channel_id;
}

td_void hcc_init_unc_buf(hcc_unc_struc *unc_buf, td_u8* buf, td_u32 len, hcc_transfer_param *param)
{
    unc_buf->list.next = TD_NULL;
    unc_buf->list.prev = TD_NULL;
    unc_buf->buf = buf;
    unc_buf->length = len;
    unc_buf->user_param = param->user_param;
    unc_buf->service_type = param->service_type;
    unc_buf->sub_type = param->sub_type;
    unc_buf->queue_id = param->queue_id;
}

STATIC ext_errno hcc_tx_proc(hcc_handler *hcc, hcc_trans_queue *queue, hcc_transfer_param *param,
    td_u8 *buf, td_u16 buf_size)
{
    hcc_tx_queue_cb tx_queue_func;

#ifdef CONFIG_HCC_SUPPORT_UNC_POOL
    if (hcc->unc_pool_head->qlen <= hcc->unc_low_limit) {
        return EXT_ERR_HCC_TX_BUF_ERR;
    }
#endif
    hcc_unc_struc *unc = hcc_alloc_unc_buf(hcc);
    if (unc == TD_NULL) {
        hcc_dfx_unc_alloc_cnt_increase(hcc, HCC_DIR_TX, TD_FALSE);
        return EXT_ERR_HCC_BUILD_TX_BUF_ERR;
    }
    hcc_dfx_unc_alloc_cnt_increase(hcc, HCC_DIR_TX, TD_TRUE);
    hcc_init_unc_buf(unc, buf, buf_size, param);
    /* 插入hcc队列之前返回失败都由业务释放
     * 插入hcc队列之后异常都由HCC释放
     */
    hcc_list_add_tail(&queue->queue_info, unc);

    tx_queue_func = (hcc_tx_queue_cb)hcc_get_rom_cb(HCC_CB_TX_QUEUE);
    if (tx_queue_func != TD_NULL) {
        return tx_queue_func(hcc, queue, param);
    }
    if (hcc->hcc_resource.tx_thread_enable == TD_TRUE) {
        hcc_sched_transfer(hcc);
    } else {
        /* 提高NONOS的聚合度,在大循环中去发送 */
#ifdef CONFIG_HCC_SUPPORT_NON_OS
        if (fc_below_low_waterline(queue) && hcc_check_pre_req_queue(queue)) {
            return EXT_ERR_SUCCESS;
        }
#endif
        hcc_tx_queue_proc(hcc, queue);
    }
    return EXT_ERR_SUCCESS;
}

STATIC ext_errno hcc_bt_tx_proc(hcc_handler *hcc, td_u8 *buf, td_u16 buf_size,
    hcc_transfer_param *param)
{
    hcc_trans_queue *queue;
    if (hcc->hcc_state != HCC_ON) {
        hcc_printf_err_log("hcc bt tx proc state:%d\r\n", hcc->hcc_state);
        return EXT_ERR_HCC_STATE_OFF;
    }

    if (hcc_adapt_tx_param_check(hcc, buf, buf_size, param) != EXT_ERR_SUCCESS) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_TX][param->queue_id];
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    if (hcc_flow_ctrl_pre_proc(hcc, param, queue) != EXT_ERR_SUCCESS) {
        return EXT_ERR_HCC_FC_PRE_PROC_ERR;
    }
#endif

    return hcc_tx_proc(hcc, queue, param, buf, buf_size);
}

ext_errno hcc_bt_tx_data(td_u8 chl, td_u8 *data_buf, td_u16 len, hcc_transfer_param *param)
{
    ext_errno ret;
    hcc_handler *hcc = hcc_get_handler(chl);
    size_t buf_size;
    td_u8 *buf = TD_NULL;
    td_u8 *user_param = TD_NULL;
    hcc_serv_info *serv_info = TD_NULL;
    hcc_tx_data_cb tx_data_func = TD_NULL;

    if (hcc == TD_NULL || data_buf == TD_NULL || param == TD_NULL) {
        hcc_printf_err_log("hcc bt tx params null!\r\n");
        return EXT_ERR_HCC_PARAM_ERR;
    }
    tx_data_func = (hcc_tx_data_cb)hcc_get_rom_cb(HCC_CB_BT_TX);
    if (tx_data_func != TD_NULL) {
        ret = tx_data_func(chl, buf, len, param);
        if (ret != EXT_ERR_HCC_ROMBLE_HOOK_CONTINUE) {
            return ret;
        }
    }

    buf_size = align_next((len + sizeof(hcc_header)), hcc->bus->len_align);
    serv_info = hcc_get_serv_info(hcc, param->service_type);
    if (serv_info == TD_NULL || serv_info->adapt == TD_NULL) {
        hcc_printf_err_log("hcc bt tx srv err q-%d, serv-%d\r\n", param->queue_id, param->service_type);
        return EXT_ERR_FAILURE;
    }

    if (serv_info->adapt->alloc == TD_NULL || serv_info->adapt->free == TD_NULL) {
        hcc_printf_err_log("hcc bt tx srv-%d alloc or free  null\r\n", param->service_type);
        return EXT_ERR_FAILURE;
    }

    ret = serv_info->adapt->alloc(param->queue_id, buf_size, &buf, &user_param);
    if (ret != EXT_ERR_SUCCESS) {
        hcc_printf_err_log("hcc bt tx srv alloc fail\r\n");
        return ret;
    }
    (td_void)memset_s(buf, buf_size, 0, buf_size);
    /* hcc header init */
    hcc_header_init((hcc_header *)buf, len, param);
    ret = memcpy_s(buf + sizeof(hcc_header), len, data_buf, len);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    ret = hcc_bt_tx_proc(hcc, buf, (td_u16)buf_size, param);
    if (ret == EXT_ERR_SUCCESS) {
        return ret;
    }
    serv_info->adapt->free(param->queue_id, buf, user_param);
    return ret;
}

ext_errno hcc_tx_data(td_u8 chl, td_u8 *buf, td_u16 len, hcc_transfer_param *param)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    hcc_trans_queue *queue = TD_NULL;
    hcc_tx_data_cb tx_data_func;
    ext_errno ret;
    if (hcc == TD_NULL || buf == TD_NULL || param == TD_NULL) {
        hcc_printf_err_log("hcc tx_data params null!\r\n");
        return EXT_ERR_HCC_PARAM_ERR;
    }
    tx_data_func = (hcc_tx_data_cb)hcc_get_rom_cb(HCC_CB_TX);
    if (tx_data_func != TD_NULL) {
        ret = tx_data_func(chl, buf, len, param);
        if (ret != EXT_ERR_HCC_ROMBLE_HOOK_CONTINUE) {
            return ret;
        }
    }
    if (hcc->hcc_state != HCC_ON) {
        hcc_printf_err_log("hcc tx_state:%d\r\n", hcc->hcc_state);
        return EXT_ERR_HCC_STATE_OFF;
    }

    if (hcc_adapt_tx_param_check(hcc, buf, len, param) != EXT_ERR_SUCCESS) {
        return EXT_ERR_HCC_PARAM_ERR;
    }
    queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_TX][param->queue_id];
#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    if (hcc_flow_ctrl_pre_proc(hcc, param, queue) != EXT_ERR_SUCCESS) {
        return EXT_ERR_HCC_FC_PRE_PROC_ERR;
    }
#endif
    hcc_header_init((hcc_header *)buf, len, param);
    return hcc_tx_proc(hcc, queue, param, buf, len);
}

#if defined(_PRE_FEATURE_WS53_DEVICE_MODE) || defined(_PRE_FEATURE_SYSCHANNEL_LITEOS)
STATIC td_u32 hcc_get_tx_queue_len(td_u8 channel_name, td_u32 queue_id)
{
    hcc_handler *hcc = hcc_get_handler(channel_name);
    hcc_trans_queue *queue = TD_NULL;
    if (hcc == TD_NULL || queue_id >= hcc->que_max_cnt) {
        return 0;
    }

    queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_TX][queue_id];
    return queue->queue_info.qlen;
}

STATIC td_u32 hcc_get_tx_sched_len(td_u8 channel_name, td_u32 queue_id)
{
    hcc_handler *hcc = hcc_get_handler(channel_name);
    hcc_trans_queue *queue = TD_NULL;
    if (hcc == TD_NULL || queue_id >= hcc->que_max_cnt) {
        return 0;
    }

    queue = &hcc->hcc_resource.hcc_queues[HCC_DIR_TX][queue_id];
    return queue->queue_ctrl->high_waterline;  // high 8 low 4 tid最多給hcc送8個包，hcc低於4個就喚醒
}

td_u32 hcc_get_tx_sched_num(osal_void)
{
    td_u32 total_num = 0;
    td_u32 allow_num = 0;

    total_num = hcc_get_tx_queue_len(HCC_CHANNEL_AP, DATA_LO_QUEUE);
    allow_num = hcc_get_tx_sched_len(HCC_CHANNEL_AP, DATA_LO_QUEUE);
    return (allow_num > total_num) ? (allow_num - total_num) : 0;
}

td_void hcc_tx_sched_wait(osal_wait_condition_func func, const void *param, unsigned long wait_time)
{
    td_s32 ret;
    hcc_handler *hcc = hcc_get_handler(HCC_CHANNEL_AP);

    if (hcc == TD_NULL) {
        return;
    }
    ret = osal_wait_timeout_interruptible(&hcc->hcc_resource.hcc_fc_wq, func, param, wait_time);
    if (ret == 0) {
        hcc_printf("[WARN]hcc flow control wait event timeout! too much time locked\r\n");
    }
}
#endif

osal_module_export(hcc_message_register);
osal_module_export(hcc_message_unregister);
osal_module_export(hcc_get_head_len);
osal_module_export(hcc_init);
osal_module_export(hcc_deinit);
osal_module_export(hcc_enable_switch);
osal_module_export(hcc_enable_rx_thread);
osal_module_export(hcc_enable_tx_thread);
osal_module_export(hcc_tx_data);
osal_module_export(hcc_bt_tx_data);
#if defined(_PRE_FEATURE_WS53_DEVICE_MODE) || defined(_PRE_FEATURE_SYSCHANNEL_LITEOS)
osal_module_export(hcc_get_tx_sched_num);
osal_module_export(hcc_tx_sched_wait);
#endif