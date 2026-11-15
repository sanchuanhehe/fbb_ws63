/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: init adapter config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __INIT_ADAPTER_H__
#define __INIT_ADAPTER_H__

#include "mesh/glue.h"
#include "mesh_common.h"

#ifndef EOK
#define EOK 0
#endif

void mesh_initialized(void);
void mesh_deinitialized(void);
int adapter_init(void);
int adapter_deinit(void);
int ble_hs_hci_rand(void *dst, int length);
bool ble_get_adapter_init_status(void);
#endif    // __INIT_ADAPTER_H__