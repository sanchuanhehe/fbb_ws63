/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: syschannel 公共代码
 * Date: 2023-03-17
 */


#include "syschannel_common.h"
#include "hcc_flow_ctrl.h"
#include "hcc_cfg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_HCC_CONFIG_DATA_FC
#define SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE 40
#else
#define SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE 20
#endif
#ifdef CONFIG_HCC_CONFIG_DATA_FC
#define SYSCHAN_DATA_FLOWCTRL_LOW_WATER_LINE ((SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE) - 8)
#else
#define SYSCHAN_DATA_FLOWCTRL_LOW_WATER_LINE ((SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE) - 12)
#endif
#define SYSCHAN_DATA_FLOWCTRL_BURST_LIMIT ((SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE) + 8)

static hcc_queue_cfg g_syschannel_queue_cfg[] = {
    { HCC_DIR_RX, SYSCH_MSG_QUEUE,
      .queue_ctrl.service_type = SYSCHANNEL_SERVICE_TYPE_MSG,
      .queue_ctrl.transfer_mode = HCC_SINGLE_MODE,
      .queue_ctrl.fc_enable = HCC_DISABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
      .queue_ctrl.low_waterline = HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE,
      .queue_ctrl.high_waterline = HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE,
      .queue_ctrl.burst_limit = HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = 0,
    },
    { HCC_DIR_TX, SYSCH_MSG_QUEUE,
      .queue_ctrl.service_type = SYSCHANNEL_SERVICE_TYPE_MSG,
      .queue_ctrl.transfer_mode = HCC_SINGLE_MODE,
      .queue_ctrl.fc_enable = HCC_DISABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
      .queue_ctrl.low_waterline = HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE,
      .queue_ctrl.high_waterline = HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE,
      .queue_ctrl.burst_limit = HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = 0,
    },
    { HCC_DIR_RX, SYSCH_DATA_QUEUE,
      .queue_ctrl.service_type = SYSCHANNEL_SERVICE_TYPE_PKT,
      .queue_ctrl.transfer_mode = HCC_ASSEMBLE_MODE,
      .queue_ctrl.fc_enable = HCC_DISABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
      .queue_ctrl.low_waterline = HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE,
      .queue_ctrl.high_waterline = HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE,
      .queue_ctrl.burst_limit = HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = HCC_DATA_QUEUE_BUF_LEN,
    },
    { HCC_DIR_TX, SYSCH_DATA_QUEUE,
      .queue_ctrl.service_type = SYSCHANNEL_SERVICE_TYPE_PKT,
      .queue_ctrl.transfer_mode = HCC_ASSEMBLE_MODE,
#ifdef CONFIG_HCC_CONFIG_DATA_FC
      .queue_ctrl.fc_enable = HCC_ENABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
#else
      .queue_ctrl.fc_enable = HCC_DISABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
#endif
      .queue_ctrl.low_waterline = SYSCHAN_DATA_FLOWCTRL_LOW_WATER_LINE,
      .queue_ctrl.high_waterline = SYSCHAN_DATA_FLOWCTRL_HI_WATER_LINE,
      .queue_ctrl.burst_limit = SYSCHAN_DATA_FLOWCTRL_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = HCC_DATA_QUEUE_BUF_LEN,
    },
    { HCC_DIR_RX, SYSCH_TEST_QUEUE,
      .queue_ctrl.service_type = HCC_ACTION_TYPE_TEST,
      .queue_ctrl.transfer_mode = HCC_ASSEMBLE_MODE,
      .queue_ctrl.fc_enable = HCC_ENABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
      .queue_ctrl.low_waterline = HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE,
      .queue_ctrl.high_waterline = HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE,
      .queue_ctrl.burst_limit = HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = HCC_DATA_QUEUE_BUF_LEN,
    },
    { HCC_DIR_TX, SYSCH_TEST_QUEUE,
      .queue_ctrl.service_type = HCC_ACTION_TYPE_TEST,
      .queue_ctrl.transfer_mode = HCC_ASSEMBLE_MODE,
      .queue_ctrl.fc_enable = HCC_ENABLE,
      .queue_ctrl.flow_type = HCC_FLOWCTRL_DATA,
      .queue_ctrl.low_waterline = HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE,
      .queue_ctrl.high_waterline = HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE,
      .queue_ctrl.burst_limit = HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT,
      .queue_ctrl.credit_bottom_value = HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE,
      .queue_ctrl.extend = HCC_DATA_QUEUE_BUF_LEN,
    },
};

hcc_queue_cfg *syschannel_get_queue_cfg(osal_u8 *arr_len)
{
    size_t len = sizeof(g_syschannel_queue_cfg) / sizeof(g_syschannel_queue_cfg[0]);
    *arr_len = (osal_u8)len;
    return g_syschannel_queue_cfg;
}

void syschannel_handler_register_rx_cb(syschannel_handler *handler, syschannel_rx_func rx_func)
{
    if (handler == OSAL_NULL) {
        return;
    }
    handler->rx_func = rx_func;
}

void syschannel_handler_register_timeout_cb(syschannel_handler *handler, syschannel_timeout_func timeout_func)
{
    if (handler == OSAL_NULL) {
        return;
    }
    handler->timeout_func = timeout_func;
}

osal_void *syschannel_alloc_msg_buf(osal_u32 size)
{
    osal_u32 size_align = channel_round_up(size, SYSCHANNEL_DATA_ALIGN);
    if (size_align == MALLOC_SIZE_ZERO) {
        return OSAL_NULL;
    }
    return osal_kmalloc(size_align, 0);
}

osal_u32 syschannel_adapt_alloc_msg_buf(hcc_queue_type queue_id, osal_u32 len, osal_u8 **buf,
    osal_u8 **user_param)
{
    unref_param(queue_id);
    if (buf == OSAL_NULL || user_param == OSAL_NULL) {
        return OSAL_NOK;
    }

    *buf = syschannel_alloc_msg_buf(len);
    *user_param = OSAL_NULL;
    if (*buf == OSAL_NULL) {
        return OSAL_NOK;
    }
    return OSAL_OK;
}

osal_void syschannel_adapt_free_msg_buf(hcc_queue_type queue_id, osal_u8 *buf, osal_u8 *user_param)
{
    unref_param(queue_id);
    unref_param(user_param);
    if (buf == OSAL_NULL) {
        return;
    }

    osal_kfree(buf);
}

osal_void syschannel_set_default_heartbeat_param(syschannel_handler* handler)
{
    /* 设置结构体的默认周期1000ms */
    /* 设置结构体的默认心跳次数为3次心跳报文 */
    handler->hb_interval = SYSCHANNEL_HB_INTERVAL;
    handler->hb_timeout_threshold = HB_TIMEOUT_THRESHOLD;
}

SYSCHANNEL_TCM_TEXT osal_void syschannel_config_hcc_flowctrl_type(osal_u8 data_type, osal_u16 *fc_flag,
    osal_u8 *queue_id)
{
    switch (data_type) {
        case SYSCHANNEL_SERVICE_TYPE_MSG:    /* 管理帧 */
            *fc_flag = HCC_FC_WAIT;      /* 阻塞等待 */
            *queue_id = SYSCH_MSG_QUEUE;
            break;
        case SYSCHANNEL_SERVICE_TYPE_PKT:    /* 数据帧 */
#ifdef CONFIG_HCC_CONFIG_DATA_FC
            *fc_flag = HCC_FC_NET;
#else
            *fc_flag = HCC_FC_WAIT | HCC_FC_DROP;
#endif
            *queue_id = SYSCH_DATA_QUEUE;
            break;
        default:
            *fc_flag = HCC_FC_NONE;
            *queue_id = SYSCH_DATA_QUEUE;
    }
}

SYSCHANNEL_TCM_TEXT osal_void syschannel_init_hcc_service_hdr(hcc_transfer_param *param, osal_u32 main_type,
    osal_u32 sub_type, osal_u8 queue_id, osal_u16 fc_flag)
{
    param->service_type = (osal_u8)main_type;
    param->sub_type = (osal_u8)sub_type;
    param->queue_id = queue_id;
    param->fc_flag = fc_flag;
}

/*****************************************************************************
 函数命名  ：syschannel_tx_msg_adapt
 功能描述  : 发送消息的函数入口
 输入参数  : hcc_id:通道对应的hcc_id，buf:内存指针，len:消息长度，type:服务类型
            sub_type:服务子类型
 返 回 值  : OSAL_OK：0表示成功
             OSAL_NOK：1表示失败
*****************************************************************************/
osal_u32 syschannel_tx_msg_adapt(osal_u8 hcc_id, osal_u8 *buf, osal_u32 len, syschannel_service_type type,
    osal_u32 sub_type)
{
    hcc_transfer_param param;
    osal_u16 fc_flag;
    osal_u8 queue_id;
    osal_u32 len_align = 0;
    osal_u16 msg_len = 0;
    osal_u8 *cfg_buf = OSAL_NULL;
    syschannel_hdr_stru *syschannel_hdr = OSAL_NULL;
    osal_u32 payload_len = len + hcc_get_head_len() + SYSCHANNEL_HDR_LEN;
    if ((len == 0) || (buf == OSAL_NULL)) {
        return OSAL_NOK;
    }

    /* 申请内存添加hcc头和syschannel头，进行一次拷贝 */
    cfg_buf = syschannel_alloc_msg_buf(payload_len);
    if (cfg_buf == OSAL_NULL) {
        return OSAL_NOK;
    }

    if (memcpy_s((cfg_buf + hcc_get_head_len() + SYSCHANNEL_HDR_LEN), len, buf, len) != OSAL_OK) {
        osal_kfree(cfg_buf);
        return OSAL_NOK;
    }
    syschannel_hdr = (syschannel_hdr_stru *)(cfg_buf + hcc_get_head_len()); /* 偏移到syschannel头 */
    /* 转下字节序, 2字节记录帧体真实长度,其余位保留 */
    msg_len = (osal_u16)len;
    syschannel_hdr->date_len = syschannel_htons(msg_len);
    syschannel_config_hcc_flowctrl_type(type, &fc_flag, &queue_id);
    syschannel_init_hcc_service_hdr(&param, type, sub_type, queue_id, fc_flag);

    len_align = channel_round_up(payload_len, SYSCHANNEL_DATA_ALIGN);
    param.user_param = OSAL_NULL;
    if (hcc_tx_data(hcc_id, cfg_buf, (td_u16)len_align, &param) != OSAL_OK) {
        osal_kfree(cfg_buf);
        return OSAL_NOK;
    }

    return OSAL_OK;
}

osal_u32 syschannel_get_unused_handler(const syschannel_handler* handler, osal_u8 num, osal_u8* unuse_handler)
{
    if ((handler == OSAL_NULL) || (unuse_handler == OSAL_NULL)) {
        return OSAL_NOK;
    }

    for (osal_u8 i = 0; i < num; i++) {
        if (handler[i].inuse == OSAL_FALSE) {
            *unuse_handler = i;
            return OSAL_OK;
        }
    }

    return OSAL_NOK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif