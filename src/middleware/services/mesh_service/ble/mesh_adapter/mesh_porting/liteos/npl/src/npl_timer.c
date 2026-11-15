/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: npl timer source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include "npl_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int npl_timer_create(clockid_t clockId, struct sigevent *evp, timer_t *timerId)
{
    BT_DBG("[DEBUG] npl_timer.c npl_timer_create start.");
    LosSwtmrCB *swtmr = NULL;
    UINT32 ret;
    UINT16 swtmrId;

    if ((clockId != CLOCK_REALTIME) || (timerId == NULL) || (evp == NULL)) {
        errno = EINVAL;
        BT_INFO("[DEBUG] npl_timer.c npl_timer_create err.");
        return -1;
    }

    if ((evp->sigev_notify == SIGEV_SIGNAL) || (evp->sigev_notify == SIGEV_NONE)) {
        errno = ENOTSUP;
        BT_INFO("[DEBUG] npl_timer.c npl_timer_create err.");
        return -1;
    } else if (evp->sigev_notify != SIGEV_THREAD) {
        errno = EINVAL;
        BT_INFO("[DEBUG] npl_timer.c npl_timer_create err.");
        return -1;
    }

    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)evp->sigev_notify_function, &swtmrId,
        (UINTPTR)evp->sigev_value.sival_int);
    if (ret != LOS_OK) {
        errno = (ret == LOS_ERRNO_SWTMR_MAXSIZE) ? EAGAIN : EINVAL;
        BT_INFO("[DEBUG] npl_timer.c npl_timer_create ret = %d. LOS_ERRNO_SWTMR_MAXSIZE = %d", ret,
            LOS_ERRNO_SWTMR_MAXSIZE);
        return -1;
    }

    swtmr = OS_SWT_FROM_SWTID(swtmrId);
    *timerId = swtmr;
    BT_DBG("[DEBUG] npl_timer.c npl_timer_create end.");

    return 0;
}

int npl_timer_delete(timer_t timerId)
{
    uint32_t ret = 0;
    LosSwtmrCB *swtmr = (LosSwtmrCB *)timerId;
    if (OS_INT_ACTIVE || (timerId == NULL)) {
        goto ERROUT;
    }

    ret = LOS_SwtmrDelete(swtmr->timerId);
    if (ret != LOS_OK) {
        BT_ERR("LOS_SwtmrDelete error:%d.", ret);
        goto ERROUT;
    }

ERROUT:
    errno = EINVAL;
    return -1;
}

int npl_timer_settime(timer_t timerId, int flags, const struct itimerspec *value, /* new value */
    struct itimerspec *oldValue)    /* old value to return, always 0 */
{
    LosSwtmrCB *swtmr = (LosSwtmrCB *)timerId;
    UINT32 interval, expiry, ret;
    UINT32 intSave;

    (VOID) flags;
    if ((value == NULL) || OS_INT_ACTIVE || (timerId == NULL)) {
        goto ERROUT;
    }

    if (!ValidTimespec(&value->it_value) || !ValidTimespec(&value->it_interval)) {
        goto ERROUT;
    }

    if (oldValue != NULL) {
        (VOID)npl_timer_gettime(timerId, oldValue);
    }

    ret = LOS_SwtmrStop(swtmr->timerId);
    if (ret == LOS_ERRNO_SWTMR_ID_INVALID) {
        goto ERROUT;
    }

    expiry = OsTimespec2Tick(&value->it_value);
    interval = OsTimespec2Tick(&value->it_interval);
    if (expiry == 0) {
        /*
         * 1) when expiry is 0, means timer should be stopped.
         * 2) If timer is ticking, stopping timer is already done before.
         * 3) If timer is created but not ticking, return 0 as well.
         */
        return 0;
    } else {
        LOS_SpinLockSave(&g_swtmrSpin, &intSave);
        if (interval == 0) {
            swtmr->mode = LOS_SWTMR_MODE_NO_SELFDELETE;
        } else {
            swtmr->mode = LOS_SWTMR_MODE_OPP;
        }
    }

    swtmr->expiry = expiry;
    swtmr->interval = interval;
    swtmr->overrun = 0;
    LOS_SpinUnlockRestore(&g_swtmrSpin, intSave);
    if (LOS_SwtmrStart(swtmr->timerId)) {
        goto ERROUT;
    }

    return 0;
ERROUT:
    errno = EINVAL;
    return -1;
}

int npl_timer_gettime(timer_t timerId, struct itimerspec *value)
{
    UINT32 tick = 0;
    LosSwtmrCB *swtmr = NULL;
    UINT32 ret;

    swtmr = (LosSwtmrCB *)timerId;

    /* expire time */
    if ((value == NULL) || (swtmr == NULL)) {
        errno = EINVAL;
        return -1;
    }
    /* get expire time */
    ret = LOS_SwtmrTimeGet(swtmr->timerId, &tick);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        errno = EINVAL;
        return -1;
    }

    OsTick2TimeSpec(&value->it_value, tick);
    OsTick2TimeSpec(&value->it_interval, swtmr->interval);
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */