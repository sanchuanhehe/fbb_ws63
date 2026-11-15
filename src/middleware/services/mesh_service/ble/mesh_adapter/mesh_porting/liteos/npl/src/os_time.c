/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os time source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "osal_task.h"
#include "osal_timer.h"

#include "nimble/nimble_npl.h"
#include "os_time.h"

#define MS_PRE_S 1000
#define US_PRE_MS 1000

/**
 * Return ticks since system start as uint32_t.
 */
ble_npl_time_t ble_npl_time_get(void)
{
    return ble_npl_time_ms_to_ticks32(ble_npl_time_get_ms());
}

/**
 * Return ms since system start as uint32_t.
 */
uint32_t ble_npl_time_get_ms(void)
{
    osal_timeval now;
    
    osal_gettimeofday(&now);
    return now.tv_sec * MS_PRE_S + now.tv_usec / US_PRE_MS;
}


ble_npl_error_t ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    *out_ticks = osal_msecs_to_jiffies(ms);
    
    return BLE_NPL_OK;
}


ble_npl_error_t ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    *out_ms = osal_jiffies_to_msecs(ticks);

    return BLE_NPL_OK;
}

ble_npl_time_t ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return osal_msecs_to_jiffies(ms);
}

uint32_t ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return osal_jiffies_to_msecs(ticks);
}

void ble_npl_time_delay(ble_npl_time_t ticks)
{
    uint32_t ms = ble_npl_time_ticks_to_ms32(ticks);
    osal_msleep(ms);
}

void ble_npl_time_mdelay(uint32_t ms)
{
    osal_msleep(ms);
}

