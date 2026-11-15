/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides timer driver patch header. \n
 *
 * History: \n
 * 2023-11-07, Create file. \n
 */

#ifndef TIMER_PATCH_H
#define TIMER_PATCH_H

#include <stdbool.h>
#include "timer_porting.h"
#include "timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// driver
#define MS_PER_S    1000
#define US_PER_MS   1000

typedef enum timer_flag {
    TIMER_FLAG_NORMAL,
    TIMER_FLAG_PERMANENT
} timer_flag_t;

typedef struct timer_info {
    timer_handle_t      key;
    timer_index_t       index;
    bool                enable;
    bool                is_run;
    timer_flag_t        flag;
    uint32_t            time_us;
    timer_callback_t    callback;
    uintptr_t           data;
} timer_info_t;

typedef struct timers_manager {
    uint32_t            last_time_load_us;
    timer_info_t        *timers;
    uint32_t            hw_timer_irq_id;
    uint16_t            hw_timer_int_priority;
    bool                in_timer_irq;
    uint8_t             soft_time_num;
} timers_manager_t;

static inline uint32_t convert_cycle_2_us(uint64_t cycle)
{
    uint32_t clock_value = timer_porting_clock_value_get();
    return (uint32_t)((cycle) / (clock_value / (MS_PER_S * US_PER_MS)));
}

extern hal_timer_funcs_t *g_hal_timer_func[CONFIG_TIMER_MAX_NUM];
extern timers_manager_t g_timers_manager[CONFIG_TIMER_MAX_NUM];
extern uint32_t timer_get_next_timer(timer_index_t index);
extern bool timer_process_timers(timer_index_t index);
extern void timer_set_next_timer_interrupt(timer_index_t index);
extern bool const g_hal_timer_width[TIMER_MAX_NUM + 1];

// new func
errcode_t uapi_timer_read(timer_handle_t timer, uint32_t *time_us);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif