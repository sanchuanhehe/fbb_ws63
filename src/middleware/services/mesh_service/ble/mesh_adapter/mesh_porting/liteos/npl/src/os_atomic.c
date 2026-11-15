/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os atomic source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include <stdint.h>
#include "mesh_common.h"
#include "osal_mutex.h"
#include "nimble/nimble_npl.h"

static osal_mutex g_mutex;
static uint8_t s_mutex_inited = 0;

uint32_t ble_npl_hw_enter_critical(void)
{
    if (!s_mutex_inited) {
        if (osal_mutex_init(&g_mutex) == 0) {
            s_mutex_inited = 1;
        }
    }

    return osal_mutex_lock(&g_mutex);
}

void ble_npl_hw_exit_critical(uint32_t ctx)
{
    osal_mutex_unlock(&g_mutex);
}