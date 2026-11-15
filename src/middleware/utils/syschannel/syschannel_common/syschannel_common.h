/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Author: IoT software develop group
 * Create: 2023-05-01
 */

#ifndef __SYSCHANNEL_COMMON_H__
#define __SYSCHANNEL_COMMON_H__

#include "syschannel_api.h"
#include "hcc_if.h"
#include "osal_types.h"
#include "soc_osal.h"
#include "securec.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MALLOC_SIZE_ZERO          0
#define SYSCHANNEL_DATA_ALIGN     32
#define SYSCHANNEL_ADDR_ALIGN     4
#define SYSCHANNEL_NAME_MAX_LEN   15
#define SYSCHANNEL_HDR_LEN        4        /* 4字节，用来记录帧的真实长度 */
#define SYSCHANNEL_MSG_TYPE_LEN   1        /* 管理消息头的长度 */
#define SYSCHANNEL_HB_INTERVAL    1000     /* 默认心跳发包周期1秒 */
#define HB_TIMEOUT_THRESHOLD      3        /* 默认心跳超时次数 */
#define HB_ENABLE                 1        /* 心跳使能 */
#define HB_DISABLE                0        /* 心跳去使能 */

#define channel_round_up(_old_len, _align)   ((((_old_len) + ((_align) - 1)) / (_align)) * (_align))
#define channel_round_down(_old_len, _align) ((_old_len) & (~((_align)-1)))
#ifndef unref_param
#define unref_param(P)  ((P) = (P))
#endif

#ifdef _PRE_FEATURE_SYSCHANNEL_LITEOS
#define SYSCHANNEL_TCM_TEXT
#else
#define SYSCHANNEL_TCM_TEXT __attribute__((section(".syschannel.tcm.text")))    /* RAM代码段 */
#endif

#ifdef BUILD_UT
#define STATIC
#else
#define STATIC static
#endif

/* 大小端转换 */
#if defined(SYSCHANNEL_BIG_ENDIAN) && (SYSCHANNEL_BIG_ENDIAN == SYSCHANNEL_ENDIAN) /* BIG_ENDIAN */
#define syschannel_ntohs(value) (value)
#define syschannel_ntohl(value) (value)
#define syschannel_htons(value) (value)
#define syschannel_htonl(value) (value)
#else /* LITTLE_ENDIAN */
#define syschannel_ntohs(value)            ((((value) & 0xFF) << 8) + (((value) & 0xFF00) >> 8))
#define syschannel_ntohl(value)       \
    ((osal_u32)(((value) & 0x000000FF) << 24) + \
     (osal_u32)(((value) & 0x0000FF00)  << 8) +  \
     (osal_u32)(((value) & 0x00FF0000)  >> 8) +  \
     (osal_u32)(((value) & 0xFF000000)  >> 24))
#define syschannel_htons(value)            ((((value) & 0xFF) << 8) + (((value) & 0xFF00) >> 8))
#define syschannel_htonl(value)       \
    ((osal_u32)(((value) & 0x000000FF) << 24) + \
     (osal_u32)(((value) & 0x0000FF00)  << 8) +  \
     (osal_u32)(((value) & 0x00FF0000)  >> 8) +  \
     (osal_u32)(((value) & 0xFF000000)  >> 24))
#endif

/* Syschannel通道控制结构体 */
typedef struct {
    osal_u8 inuse;  /* 是否使用 */
    osal_u8 hcc_id; /* hcc通道id */
    osal_u8 bus_type; /* 总线类型 */
    osal_u8 heartbeat_cnt; /* 心跳检测超时计数 */
    osal_timer heartbeat_timer; /* 心跳定时器 */
    osal_u8 enablehb; /* 是否使能心跳检测 */
    osal_u8 hb_timeout_threshold;  /* 心跳检测超时阈值 */
    osal_u16 hb_interval; /* 心跳检测报文发送周期 */
    syschannel_timeout_func timeout_func; /* 消息接收处理回调函数 */
    syschannel_rx_func rx_func; /* 消息接收处理回调函数 */
    char descrp[SYSCHANNEL_NAME_MAX_LEN + 1]; /* 通道描述 */
}syschannel_handler;

typedef enum {
    SYSCHANNEL_SUB_TYPE_MSG_MNT = 0,        /* Syschannel组件的管理消息 */
    SYSCHANNEL_SUB_TYPE_MSG_APP = 1,       /* 用户业务消息 */
    SYSCHANNEL_SUB_TYPE_MSG_MAX,
} syschannel_sub_type_msg;

typedef enum {
    SYSCHANNEL_HEARTBEAT_ENABLE = 0,        /* Syschannel心跳使能消息 */
    SYSCHANNEL_SDIO_READY = 1,              /* SDIO检测成功消息 */
    SYSCHANNEL_HEARTBEAT_ENABLE_ACK = 2,    /* Syschannel心跳使能消息ack */
    SYSCHANNEL_HOST_SYNC_MAC_ADDR = 3,      /* Syschannel_host同步dev mac地址 */
    SYSCHANNEL_HOST_SYNC_IP_ADDR = 4,       /* Syschannel_host同步dev IP地址 */
    SYSCHANNEL_MSG_MAX = 0xff,
} syschannel_msg_mnt;

#define OAL_IPV4_ADDR_SIZE 4
typedef struct {
    osal_u8                     ip_addr[OAL_IPV4_ADDR_SIZE];
    osal_u8                     mask_addr[OAL_IPV4_ADDR_SIZE];
    osal_u8                     gw_addr[OAL_IPV4_ADDR_SIZE];
} ip_addr_sync_stru;

typedef struct {
    osal_u8 enablehb; /* 是否使能心跳检测 */
    osal_u8 hb_timeout_threshold;  /* 心跳检测超时阈值 */
    osal_u16 hb_interval; /* 心跳检测报文发送周期 */
} heartbeat_ctrl_stru;

typedef struct {
    osal_u16 date_len;           /* 保存真实长度 */
    osal_s8 pad_offset;          /* 记录地址4字节对齐的偏移 */
    osal_u8 resv;
} syschannel_hdr_stru;

hcc_queue_cfg *syschannel_get_queue_cfg(osal_u8 *arr_len);
void syschannel_handler_register_rx_cb(syschannel_handler *handler, syschannel_rx_func rx_func);
void syschannel_handler_register_timeout_cb(syschannel_handler *handler, syschannel_timeout_func timeout_func);
osal_void *syschannel_alloc_msg_buf(osal_u32 size);
osal_u32 syschannel_adapt_alloc_msg_buf(hcc_queue_type queue_id, osal_u32 len, osal_u8 **buf,
    osal_u8 **user_param);
osal_void syschannel_adapt_free_msg_buf(hcc_queue_type queue_id, osal_u8 *buf, osal_u8 *user_param);
osal_u32 syschannel_tx_msg_adapt(osal_u8 hcc_id, osal_u8 *buf, osal_u32 len, syschannel_service_type type,
    osal_u32 sub_type);
osal_void syschannel_set_default_heartbeat_param(syschannel_handler* handler);
osal_void syschannel_config_hcc_flowctrl_type(osal_u8 data_type, osal_u16 *fc_flag, osal_u8 *queue_id);
osal_void syschannel_init_hcc_service_hdr(hcc_transfer_param *param, osal_u32 main_type, osal_u32 sub_type,
    osal_u8 queue_id, osal_u16 fc_flag);
osal_u32 syschannel_get_unused_handler(const syschannel_handler* handler, osal_u8 num, osal_u8* unuse_handler);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif