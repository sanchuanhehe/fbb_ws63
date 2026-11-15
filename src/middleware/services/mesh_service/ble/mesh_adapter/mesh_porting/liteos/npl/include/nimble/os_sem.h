/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os sem config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef _NPL_OS_SEM_H
#define _NPL_OS_SEM_H

#include <assert.h>
#include <errno.h>
#include "semaphore.h"

#include "os/os.h"
#include "nimble/nimble_npl.h"

ble_npl_error_t ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens);
ble_npl_error_t ble_npl_sem_deinit(struct ble_npl_sem *sem);
ble_npl_error_t ble_npl_sem_release(struct ble_npl_sem *sem);
ble_npl_error_t ble_npl_sem_pend(struct ble_npl_sem *sem, uint32_t timeout);
uint16_t ble_npl_sem_get_count(struct ble_npl_sem *sem);
ble_npl_error_t ble_npl_sem_pend_ms(struct ble_npl_sem *sem, uint32_t timeout);
#endif // _NPL_OS_SEM_H