/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PROVISIONER_H_
#define _PROVISIONER_H_
#include "prov_bearer.h"

int bt_mesh_pb_adv_open(const uint8_t uuid[16], uint16_t net_idx, uint16_t addr,
			uint8_t attention_duration);

int bt_mesh_pb_rmt_open(const uint8_t *uuid, uint16_t addr, uint8_t attention_durationconst,
    const struct prov_bearer *bearer, void *bearer_cb_data);

int bt_mesh_provisioner_enable(uint16_t addr, uint16_t net_idx, uint8_t flags, uint32_t iv_index);
int bt_mesh_provisioner_deinit(void);

#endif    // _PROVISIONER_H_