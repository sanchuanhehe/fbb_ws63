/* *
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: os types config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef _NPL_OS_TYPES_H
#define _NPL_OS_TYPES_H

#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include "semaphore.h"
#include "osal_semaphore.h"
#include "osal_list.h"

typedef uint32_t ble_npl_time_t;
typedef int32_t ble_npl_stime_t;

struct ble_npl_event;
typedef void ble_npl_event_fn(struct ble_npl_event *ev);

struct ble_npl_event {
    uint8_t ev_queued;
    ble_npl_event_fn *ev_cb;
    void *ev_arg;
    struct osal_list_head entry;
};

struct ble_npl_eventq {
    struct osal_list_head  event_queue;
    uint8_t size;
    uint8_t inited;
};

struct ble_npl_callout {
    struct ble_npl_event c_ev;          /* 事件 */
    struct ble_npl_eventq *c_evq;       /* 事件队列 */
    ble_npl_time_t c_ticks;             /* 定时器ticks */
    timer_t c_timer;                    /* 定时器句柄 */
    bool c_active;                      /* 激活标记 */
};

struct ble_npl_sem {
    osal_semaphore lock;
};

struct ble_npl_mutex {
    int mu;
};

#endif // _NPL_OS_TYPES_H
