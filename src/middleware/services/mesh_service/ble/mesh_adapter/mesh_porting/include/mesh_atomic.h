/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: atomic config
 *
 * History:
 * 2025-8-21, Create file.
 */
#ifndef __MESH_ATOMIC_H__
#define __MESH_ATOMIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef  int32_t atomic_t;
typedef  int32_t atomic_val_t;

atomic_val_t atomic_fetch_or(atomic_t *target, atomic_val_t value);
atomic_val_t atomic_fetch_and(atomic_t *target, atomic_val_t value);
atomic_val_t __atomic_load_n(const atomic_t *target, int memorder);
atomic_val_t __atomic_exchange_n(atomic_t *target, atomic_val_t value, int memorder);
atomic_val_t __atomic_fetch_or(atomic_t *target, atomic_val_t value, int memorder);
atomic_val_t __atomic_fetch_xor(atomic_t *target, atomic_val_t value, int memorder);
atomic_val_t __atomic_fetch_and(atomic_t *target, atomic_val_t value, int memorder);
atomic_val_t __atomic_fetch_add(atomic_t *target, atomic_val_t value, int memorder);
atomic_val_t __atomic_fetch_sub(atomic_t *target, atomic_val_t value, int memorder);

void bt_mesh_atomic_mutex_new(void);
void bt_mesh_atomic_mutex_free(void);

#ifdef __cplusplus
}
#endif

#endif /* __MESH_ATOMIC_H__ */
