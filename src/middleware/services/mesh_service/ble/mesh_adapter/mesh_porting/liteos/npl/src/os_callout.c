/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os callout source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "npl_timer.h"
#include "nimble/nimble_npl.h"

#include "os_time.h"

void ble_npl_callout_init(
    struct ble_npl_callout *callout, struct ble_npl_eventq *evq, ble_npl_event_fn *ev_cb, void *ev_arg)
{
    struct sigevent event;
    if (callout == NULL) {
        BT_ERR("callout is null.");
        return;
    }

    memset_s(callout, sizeof(*callout), 0, sizeof(*callout));
    callout->c_ev.ev_cb = ev_cb;
    callout->c_ev.ev_arg = ev_arg;
    callout->c_evq = evq;
    callout->c_active = false;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_value.sival_ptr = callout;  // put callout obj in signal args
    event.sigev_notify_function = ev_cb;    // 超时函数, 该函数中需做删除timer处理;
    event.sigev_notify_attributes = NULL;

    int ret = npl_timer_create(CLOCK_REALTIME, &event, &callout->c_timer);
    if (ret != 0) {
        BT_ERR("[ERROR] npl_timer_create err. ret is %d.", ret);
    }
}

bool ble_npl_callout_is_active(struct ble_npl_callout *callout)
{
    return callout != NULL && callout->c_active;
}

int ble_npl_callout_inited(struct ble_npl_callout *callout)
{
    return (callout != NULL && callout->c_timer != NULL);
}

ble_npl_error_t ble_npl_callout_reset(struct ble_npl_callout *callout, ble_npl_time_t ticks)
{
    ble_npl_time_t remaining_ticks = ticks;
    struct itimerspec its;
    if (callout == NULL) {
        BT_ERR("callout is null.");
        return BLE_NPL_INVALID_PARAM;
    }

    if (remaining_ticks < 0) {
        return BLE_NPL_EINVAL;
    }

    if (remaining_ticks == 0) {
        remaining_ticks = 1;
    }

    callout->c_ticks = ble_npl_time_get() + remaining_ticks;

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;  // one shot
    its.it_value.tv_sec = (ble_npl_time_ticks_to_ms32(remaining_ticks) / MS_PER_SEC);
    its.it_value.tv_nsec = (ble_npl_time_ticks_to_ms32(remaining_ticks) % MS_PER_SEC) * MS_PER_SEC * MS_PER_SEC;
    its.it_value.tv_nsec %= MS_PER_SEC * MS_PER_SEC * MS_PER_SEC;
    callout->c_active = true;
    npl_timer_settime(callout->c_timer, 0, &its, NULL);

    return BLE_NPL_OK;
}

int ble_npl_callout_delete(timer_t tmr)
{
    return npl_timer_delete(tmr);
}

void ble_npl_callout_stop(struct ble_npl_callout *callout)
{
    if (!ble_npl_callout_inited(callout)) {
        return;
    }

    struct itimerspec its;
    (void)memset_s(&its, sizeof(struct itimerspec), 0, sizeof(struct itimerspec));

    npl_timer_settime(callout->c_timer, 0, &its, NULL);
    callout->c_active = false;
}

ble_npl_time_t ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    if (co == NULL) {
        return 0;
    }

    return co->c_ticks;
}

void ble_npl_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    if (co == NULL) {
        return;
    }
    co->c_ev.ev_arg = arg;
}

uint32_t ble_npl_callout_remaining_ticks(struct ble_npl_callout *co, ble_npl_time_t now)
{
    ble_npl_time_t rt;
    if (co == NULL) {
        return BLE_NPL_EINVAL;
    }
    uint32_t exp = co->c_ticks;

    if (exp > now) {
        rt = exp - now;
    } else {
        rt = 0;
    }

    return rt;
}
