/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sdio device interface
 * Author: CompanyName
 * Create: 2023-05-17
 */

#ifndef __HCC_SDIO_DEVICE_ROM_H__
#define __HCC_SDIO_DEVICE_ROM_H__

#ifdef CONFIG_HCC_SUPPORT_SDIO

#include "td_base.h"
#include "hcc_sdio_comm.h"
#include "hcc_queue.h"
#include "hcc_bus_types.h"
#include "sdio_slave.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct _sdio_error_status {
    /* rx assem error */
    td_u8 sdio_rx_assem_reset_fail;
    td_u8 sdio_wr_start_len_align_err;
    td_u8 sdio_wr_start_pen_len_err;
    td_u8 sdio_wr_over_align_err;
    td_u8 sdio_wr_over_hdr_len_err;
    td_u8 sdio_wr_over_total_len_err;
    td_u8 sdio_queue_switch_type_err;
    td_u8 reserved;

    td_u32 sdio_wr_start_len_err;
    td_u32 sdio_rx_single_pkt_miss;
} sdio_error_status;

typedef struct {
    td_u32 sdio_rx_ctrl_pkt;         /* 控制消息个数 */
    td_u32 sdio_rx_buf_cnt;          /* 收包个数 */
    td_u32 sdio_send_data_cnt;       /* SDIO调用send data次数统计 */
    td_u32 sdio_h2d_pkt_cnt;         /* Host To Device 有效数据包个数 */
    td_u32 sdio_d2h_pkt_cnt;         /* Host To Device 有效数据包个数 */
    td_u32 sdio_rx_pkt_miss;         /* SDIO层内存分配失败Rx丢包统计 */

    td_u32 sdio_wr_start_cnt;           /* SDIO接收开始中断数统计 */
    td_u32 sdio_rd_start_cnt;           /* SDIO发送开始中断数统计 */
    td_u32 sdio_wr_over_cnt;            /* SDIO接收完成中断数统计 */
    td_u32 sdio_rd_over_cnt;            /* SDIO发送完成中断数统计 */
    td_u32 sdio_rd_error_cnt;           /* SDIO发送错误中断数统计 */

    sdio_error_status error_status;
} sdio_stats_stru;

/* This descr buff is reversed in device,
  the callback function can't cost much time,
  just for transfer sdio buff message */
struct hcc_descr_header {
    td_u32 queue_id;
};

/* 64 - hcc - cb - mac */
td_void set_sdio_packet_alloc_max_size(td_u32 size);

enum _hcc_descr_type {
    HCC_DESCR_ASSEM_RESET = 0,
    HCC_NETBUF_QUEUE_SWITCH = 1,
    HCC_DESCR_TYPE_BUTT
};

/* 接收结构体 */
typedef struct _sdio_rx_stru {
    hcc_unc_struc *buf;
    td_u32 rx_pkt_bytes; /* 发送数据字节数 ,包括HCC头+PayLoad */
} sdio_rx_stru;

/* 发送结构体 */
typedef struct _sdio_tx_stru {
    hcc_unc_struc *buf;
    td_u32 tx_pkt_bytes; /* 发送数据字节数 */
} sdio_tx_stru;

/* 发送方向信息结构体,Device To Host */
typedef struct _sdio_tx_info_stru {
    sdio_tx_stru sdio_tx_aggr[HISDIO_DEV2HOST_SCATT_MAX];   /* TX方向聚合帧数据 */
    td_u16 aggr_tx_num;                                     /* TX方向聚合帧数量 */
    td_u32 tx_aggr_total_len;                               /* TX方向聚合帧Pad之前的总长度 */
} sdio_tx_info_stru;

/* 接收方向信息结构体, Host To Device */
typedef struct _sdio_rx_info_stru {
    sdio_rx_stru sdio_rx_aggr[HISDIO_HOST2DEV_SCATT_MAX + 1];   /* RX方向聚合帧数据,留1个用于放置Padding数据 */
    td_u32 rx_aggr_total_len;                                   /* TX方向聚合帧Pad之前的总长度 */
    td_u16 aggr_rx_num;                                         /* RX方向聚合帧数量 */
    td_u16 trans_len;                                           /* RX方向传输报文长度 */
} sdio_rx_info_stru;
#define BUS_RESERVE_BUFF    2560
typedef td_void (*sdio_descr_ctrl_func)(td_void);
typedef struct _sdio_data_ctl_stru {
    hcc_bus sdio_bus;
    sdio_bus_t dev_bus;
    sdio_descr_ctrl_func descr_control_func[HCC_DESCR_TYPE_BUTT];
    td_u32 device_panic_flag : 1;
    td_u32 current_queue_id : 8; // = HCC_QUEUE_COUNT;
    td_u32 rsv : 23;
    td_u32 sdio_panic_addr;
    /* Used to receive the first assem info descr. */
    volatile td_u8 h2d_assem_descr[HISDIO_HOST2DEV_SCATT_SIZE];
    /* 收发方向全局变量结构体 */
    volatile sdio_tx_info_stru tx_aggr_info;
    volatile sdio_rx_info_stru rx_aggr_info;
    sdio_stats_stru sdio_device_stats;
    hcc_unc_struc sdio_hcc_unc_rsv_buf;
    td_u8 rsv_data_buf[BUS_RESERVE_BUFF];
    bus_dev_ops sdio_bus_ops;
} sdio_data_ctl_stru;

td_void hcc_sdio_msg_callback(td_u32 msg);
td_void hcc_sdio_readerr_callback(td_void);
td_void hcc_sdio_device_info_dump(td_void);
sdio_data_ctl_stru* get_sdio_data_ctl(td_void);
td_u32 hcc_sdio_readover_callback(td_void);
td_u32 hcc_sdio_readstart_callback(td_u32 len, td_u8 *dma_tbl);
td_u32 hcc_sdio_writestart_callback(td_u32 trans_len, td_u8 *dma_tbl);
td_u32 hcc_sdio_writeover_callback(td_void);

td_s32 hcc_sdio_soft_res_init(bus_dev_ops *sdio_bus_ops);
typedef td_void (*hcc_list_restore_callback)(hcc_data_queue *dst_queue, hcc_data_queue *src_queue);
typedef td_u32 (*sdio_queue_sched)(hcc_bus *bus, hcc_data_queue *head, hcc_queue_type queue_id);
td_u32 sdio_wifi_tx_queue_sched(hcc_bus *bus, hcc_data_queue *head, hcc_queue_type queue_id);
td_u32 sdio_tx_proc(hcc_bus *bus, hcc_trans_queue *queue, td_u16 *remain_pkt_nums,
    sdio_queue_sched func, hcc_list_restore_callback restore_func);
ext_errno hcc_sdio_send_message(TD_CONST hcc_bus *bus, td_u32 msg);
td_bool hcc_sdio_is_busy(hcc_queue_dir chan);
td_void hcc_sdio_update_credit(TD_CONST hcc_bus *bus, td_u32 free_cnt);
td_void hcc_sdio_dump_reg_and_panic(td_void);
td_void hcc_sdio_rx_assem_info_reset(td_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* CONFIG_HCC_SUPPORT_SDIO */
#endif /* HCC_SDIO_DEVICE_HEADER */
