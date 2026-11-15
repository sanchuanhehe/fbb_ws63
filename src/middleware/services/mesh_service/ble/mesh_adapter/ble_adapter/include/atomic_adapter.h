/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: atomic adapter config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __ATOMIC_ADAPTER_H__
#define __ATOMIC_ADAPTER_H__

#include "los_atomic.h"
#include "mesh_atomic.h"
#include "mesh/glue.h"
#include "mesh/atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define atomic_inc(target) LOS_AtomicInc(target)

#define atomic_dec(target) LOS_AtomicDec(target)

#define atomic_get(target) LOS_AtomicRead(target)

static inline atomic_val_t atomic_set_inner(atomic_t *target, atomic_val_t value)
{
    LOS_AtomicSet(target, value);
    return 0;
}

#define atomic_set(target, value) atomic_set_inner(target, value)


/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __ATOMIC_ADAPTER_H__ */
