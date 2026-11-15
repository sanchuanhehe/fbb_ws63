/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE CHBA SERVER.
 */
#ifndef SLE_CHBA_SERVER_H
#define SLE_CHBA_SERVER_H

#include "sle_common.h"
#include "soc_diag_log.h"
#include "stdbool.h"
#include "sle_chba_netif_mng.h"
#include "nv_common_cfg.h"

#define CHBA_INDEX_0 0
#define CHBA_INDEX_1 1
#define CHBA_INDEX_2 2
#define CHBA_INDEX_3 3
#define CHBA_INDEX_4 4
#define CHBA_INDEX_5 5

#define CHBA_INVALID_CONN_ID 0xFFFF
#define SLE_CHBA_LINK_NUM_MAX 4
#define LOG_ID 0

#define chba_err_log(fmt, args...)        printf(fmt, ##args)
#define chba_warn_log(fmt, args...)       printf(fmt, ##args)
#define chba_info_log(fmt, args...)       printf(fmt, ##args)
#define chba_dbg_log(fmt, args...)        printf(fmt, ##args)

#define chba_log_print(fmt, args...)      printf(fmt, ##args)

enum sle_chba_status {
    STATUS_IDLE,
    STATUS_CHBA_READY,
    STATUS_CONN_UPDATING,
    STATUS_PHY_UPDATING,
    STATUS_ENCRYPTING,
};

typedef struct {
    uint16_t interval;
    uint8_t phy;
    uint8_t mcs;
    uint8_t latency;
} sle_chba_link_param_t;

typedef struct {
    uint16_t conn_id;
    sle_addr_t remote_addr;
    enum sle_chba_status status;
    sle_chba_link_param_t link_param;
    bool link_ready;
} sle_chba_user_link_t;

typedef struct {
    uint16_t data_len;
    uint8_t *data_ptr;
} sle_chba_recv_pkt_t;

extern chba_cfg_t g_chba_cfg;

sle_chba_user_link_t *sle_chba_user_get_new_linkinfo(void);
sle_chba_user_link_t *sle_chba_user_get_linkinfo_by_connid(uint16_t conn_id);
void sle_chba_user_del_linkinfo_by_connid(uint16_t conn_id);
void sle_chba_user_links_clean(void);
#endif