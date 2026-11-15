/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: mutex adapter config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __MUTEX_ADAPTER_H__
#define __MUTEX_ADAPTER_H__

#include "los_mux.h"

static inline uint32_t bt_mesh_mux_create(uint32_t *muxHandle)
{
    return LOS_MuxCreate(muxHandle);
}

static inline uint32_t bt_mesh_mux_delete(uint32_t muxHandle)
{
    return LOS_MuxDelete(muxHandle);
}

static inline uint32_t bt_mesh_mux_lock(uint32_t muxHandle, uint32_t timeout)
{
    return LOS_MuxPend(muxHandle, timeout);
}

static inline uint32_t bt_mesh_mux_unlock(uint32_t muxHandle)
{
    return LOS_MuxPost(muxHandle);
}

#endif