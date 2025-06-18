/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 *
 * Description: SLE CHBA NET DEVICE PUBLIC API module.
 */
#ifndef SLE_IP_SERVER_H
#define SLE_IP_SERVER_H

#include "sle_common.h"

enum sle_chba_role {
    CHBA_ROLE_AP,
    CHBA_ROLE_STA,
};

typedef struct {
    uint8_t conn_id;
    sle_addr_t remote_addr;
    uint32_t tx_pkts_cnt;
    uint32_t tx_bytes;
    uint32_t rx_pkts_cnt;
    uint32_t rx_bytes;
} sle_ip_link_info;

errcode_t sle_chba_netdev_create(uint8_t chba_mode, uint8_t chba_role);

errcode_t sle_chba_netdev_destroy(void);

errcode_t sle_chba_netdev_add_link(uint16_t conn_id, const sle_addr_t *remote_addr);

errcode_t sle_chba_netdev_del_link(uint16_t conn_id, const sle_addr_t *remote_addr);

errcode_t sle_chba_netdev_get_linkinfo(uint16_t conn_id, sle_ip_link_info *link);

#endif