/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: atomic adapter source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include "mutex_adapter.h"
#include "adapter.h"
#include "atomic_adapter.h"
#include "mesh_atomic.h"

#define ATOMIC_SEM_MAX_TIMEOUT 0xffffffffUL

static uint32_t atomic_lock;

void bt_mesh_atomic_mutex_free(void)
{
    bt_mesh_mux_delete(atomic_lock);
}

void bt_mesh_atomic_mutex_new(void)
{
    bt_mesh_mux_create(&atomic_lock);
}

atomic_val_t atomic_fetch_or(atomic_t *target, atomic_val_t value)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target |= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t atomic_fetch_and(atomic_t *target, atomic_val_t value)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target &= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_load_n(const atomic_t *target, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_exchange_n(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target = value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_fetch_or(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target |= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_fetch_xor(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target ^= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_fetch_and(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target &= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_fetch_add(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target += value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}

atomic_val_t __atomic_fetch_sub(atomic_t *target, atomic_val_t value, int memorder)
{
    atomic_val_t ret = 0;

    bt_mesh_mux_lock(atomic_lock, ATOMIC_SEM_MAX_TIMEOUT);

    ret = *target;
    *target -= value;

    bt_mesh_mux_unlock(atomic_lock);

    return ret;
}