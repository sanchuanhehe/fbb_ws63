/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2024-03-01, Create file. \n
 */
#include "tiot_pm_wakelock.h"
#include "tiot_board_misc_port.h"
#include "tiot_board_log.h"
#include "tiot_osal.h"

/* 所有controller共享同一个wakelock, 只有所有controller未持锁时才允许睡眠. */
static tiot_board_wakelock g_top_wakelock;
static osal_spinlock g_top_wakelock_spinlock;
static uint16_t g_top_lock_count;

int32_t tiot_pm_wake_lock_init(void)
{
    if (tiot_board_wake_lock_init(&g_top_wakelock) != ERRCODE_TIOT_SUCC) {
        return ERRCODE_TIOT_PM_WAKE_LOCK_INIT_FAIL;
    }
    if (osal_spin_lock_init(&g_top_wakelock_spinlock) != OSAL_SUCCESS) {
        tiot_board_wake_lock_destroy(&g_top_wakelock);
        return ERRCODE_TIOT_SPIN_LOCK_INIT_FAIL;
    }
    g_top_lock_count = 0;
    return ERRCODE_TIOT_SUCC;
}

void tiot_pm_wake_lock(void)
{
    unsigned long flags;
    osal_spin_lock_irqsave(&g_top_wakelock_spinlock, &flags);
    if (g_top_lock_count == 0) {
        tiot_board_wake_lock(&g_top_wakelock);
    }
    g_top_lock_count++;
    osal_spin_unlock_irqrestore(&g_top_wakelock_spinlock, &flags);
}

void tiot_pm_wake_unlock(void)
{
    unsigned long flags;
    osal_spin_lock_irqsave(&g_top_wakelock_spinlock, &flags);
    if (g_top_lock_count > 0) {
        g_top_lock_count--;
        /* 防止count为0时多次调用unlock接口。 */
        if (g_top_lock_count == 0) {
            tiot_board_wake_unlock(&g_top_wakelock);
        }
    }
    osal_spin_unlock_irqrestore(&g_top_wakelock_spinlock, &flags);
}

void tiot_pm_wake_lock_destroy(void)
{
    /* 确保destroy时一定释放。 */
    unsigned long flags;
    osal_spin_lock_irqsave(&g_top_wakelock_spinlock, &flags);
    if (g_top_lock_count > 0) {
        tiot_print_err("[TIoT][pm]g_top_lock_count should be 0, but is %d\n", g_top_lock_count);
        g_top_lock_count = 0;
        tiot_board_wake_unlock(&g_top_wakelock);
    }
    osal_spin_unlock_irqrestore(&g_top_wakelock_spinlock, &flags);

    tiot_board_wake_lock_destroy(&g_top_wakelock);
    osal_spin_lock_destroy(&g_top_wakelock_spinlock);
}

