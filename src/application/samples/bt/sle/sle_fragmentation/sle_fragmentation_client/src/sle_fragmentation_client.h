/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Hello Client Config. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_FRAGMENTATION_CLIENT_H
#define SLE_FRAGMENTATION_CLIENT_H

#include "sle_ssap_client.h"

void sle_fragmentation_client_init(ssapc_notification_callback notification_cb,
                                   ssapc_indication_callback indication_cb,
                                   ssapc_read_cfm_callback read_cfm_cb,
                                   ssapc_write_cfm_callback write_cfm_cb);

void sle_fragmentation_client_send_write_req(uint16_t conn_id);

void sle_fragmentation_client_start_scan(void);

#endif
