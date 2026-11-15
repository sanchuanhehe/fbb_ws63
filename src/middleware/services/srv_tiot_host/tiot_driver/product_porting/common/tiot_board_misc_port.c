/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Board Misc port. \n
 *
 * History: \n
 * 2023-12-04, Create file. \n
 */
#include "tiot_board_misc_port.h"
#include "tiot_board_misc_port_config.h"
#include "soc_osal.h"
#include "product.h"
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
#include "pm_veto.h"
#endif

int32_t tiot_board_wake_lock_init(tiot_board_wakelock *lock)
{
    tiot_unused(lock);
    return 0;
}

void tiot_board_wake_lock(tiot_board_wakelock *lock)
{
    tiot_unused(lock);
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    (void)uapi_pm_add_sleep_veto(TIOT_WAKEUP_LOCK_ID);
#endif
}

void tiot_board_wake_unlock(tiot_board_wakelock *lock)
{
    tiot_unused(lock);
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    (void)uapi_pm_remove_sleep_veto(TIOT_WAKEUP_LOCK_ID);
#endif
}

void tiot_board_wake_lock_destroy(tiot_board_wakelock *lock)
{
    tiot_unused(lock);
}

void tiot_board_udelay(uint32_t us)
{
    osal_udelay(us);
}