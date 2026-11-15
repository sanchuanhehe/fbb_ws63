/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_H
#define __BT_MESH_H

#include <stddef.h>
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"

#include "glue.h"
#include "msg.h"
#include "access.h"
#include "main.h"
#include "cfg.h"
#include "cfg_srv.h"
#include "health_srv.h"
#include "cfg_cli.h"
#include "health_cli.h"
#include "proxy.h"
#include "cdb.h"
#include "cfg.h"
#include "heartbeat.h"
#include "../src/app_keys.h"
#include "../src/net.h"

void bt_mesh_reprovision(uint16_t addr);
void bt_mesh_dev_key_cand(const uint8_t *key, uint8_t len);
void bt_mesh_dev_key_cand_remove(void);
void bt_mesh_dev_key_cand_activate(void);

int bt_mesh_comp_get_page_0(struct os_mbuf *buf);

#endif /* __BT_MESH_H */
