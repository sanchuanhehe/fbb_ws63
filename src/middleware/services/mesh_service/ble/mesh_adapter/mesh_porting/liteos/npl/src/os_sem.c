/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os sem source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include <assert.h>
#include "semaphore.h"
#include "osal_semaphore.h"

#include "os/os.h"
#include "nimble/nimble_npl.h"

ble_npl_error_t ble_npl_sem_deinit(struct ble_npl_sem *sem)
{
    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    osal_sem_destroy(&sem->lock);

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    osal_sem_init(&sem->lock, tokens);

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_release(struct ble_npl_sem *sem)
{
    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    osal_sem_up(&sem->lock);

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_pend_ms(struct ble_npl_sem *sem, uint32_t timeout)
{
    int err = 0;
    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    if (timeout == BLE_NPL_TIME_FOREVER) {
        err = osal_sem_down(&sem->lock);
    } else {
        err = osal_sem_down_timeout(&sem->lock, timeout);
        if (err != 0) {
            return BLE_NPL_TIMEOUT;
        }
    }

    return (err != 0) ? BLE_NPL_ERROR : BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    return ble_npl_sem_pend_ms(sem, ble_npl_time_ticks_to_ms32(timeout));
}

uint16_t ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    int count;

    assert(sem);
    assert(&sem->lock);
    sem_getvalue(&sem->lock, &count);

    return count;
}
