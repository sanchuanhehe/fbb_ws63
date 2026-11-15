/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: adapter config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __ADAPTER_H__
#define __ADAPTER_H__

#include <stdint.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"
#include "bts_def.h"
#include "mesh/glue.h"
#include "mesh/porting.h"
#include "mesh/slist.h"
#include "securec.h"
#include "atomic_adapter.h"
#include "access.h"
#include "os/endian.h"
#include "nimble/os_sem.h"
#include "nimble/os_types.h"
#include "init_adapter.h"

/* BD ADDR length */

#ifndef NIMBLE_ENUM
#define NIMBLE_ENUM 0

#endif
uint8_t ble_hs_hci_get_hci_version(void);
void ble_transport_init(void);
void *ble_npl_get_current_task_id(void);
int mem_init_mbuf_pool(
    void *mem, struct os_mempool *mempool, struct os_mbuf_pool *mbuf_pool, int num_blocks, int block_size, char *name);
int adapter_init(void);
int adapter_deinit(void);
void mesh_initialized(void);
void mesh_deinitialized(void);
void k_work_schedule_periodic(struct k_work_delayable *w, uint32_t ms);
struct k_work_delayable *k_work_delayable_from_work(struct ble_npl_event *work);
int ble_mesh_adapter_init(void);
int ble_mesh_adapter_deinit(void);

#endif /* __ADAPTER_H__ */