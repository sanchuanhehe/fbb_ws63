/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: npl timer config
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _NPL_TIMER_H_
#define _NPL_TIMER_H_

#include "time_pri.h"
#include "time.h"
#include "limits.h"
#include "signal.h"
#include "los_swtmr_pri.h"
#include "adapter.h"
#include "nimble/nimble_npl.h"
#include "nimble/nimble_npl_os.h"


typedef int clockid_t;
typedef void *timer_t;

int npl_timer_create(clockid_t clockId, struct sigevent *evp, timer_t *timerId);
int npl_timer_delete(timer_t timerId);
int npl_timer_settime(timer_t timerId, int flags, const struct itimerspec *value, /* new value */
    struct itimerspec *oldValue);
int npl_timer_gettime(timer_t timerId, struct itimerspec *value);
#endif /* _NPL_TIMER_H_ end */