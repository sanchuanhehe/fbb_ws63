/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BEACON_H__
#define __BEACON_H__

#include "os/os_mbuf.h"

typedef void (* beacon_recv)(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf);

void bt_mesh_beacon_enable(void);
void bt_mesh_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf);

int bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
                           struct os_mbuf *buf, bool priv);

void bt_mesh_beacon_init(void);
void bt_mesh_beacon_deinit(void);
void bt_mesh_beacon_update(struct bt_mesh_subnet *sub);
#if CONFIG_BLE_MESH_PTS_MODE
int bt_mesh_cmd_secure_beacon_send(uint8_t key_refresh);
#endif
void bt_mesh_beacon_register_cbks(beacon_recv func);
void bt_mesh_beacon_set_unprov_beacon_flag(bool flag);
#endif
