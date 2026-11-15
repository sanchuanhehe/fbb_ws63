/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Create Date : 2026.05
 * Description: enter_net.h 入网sample
 */
#ifndef ENTER_NET_H
#define ENTER_NET_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "chip_core_irq.h"
#include "timer.h"
#include "led_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef unused
#define unused(P) (void)(P)
#endif

#define SAMPLE_ADDR_IND_0                           0
#define SAMPLE_ADDR_IND_4                           4
#define SAMPLE_ADDR_IND_5                           5

/* 硬件定时器 */
#define UH_HW_TIMER_IRQN                            TIMER_2_IRQN    /* 硬件定时器timer-2中断位 */
#define UH_HW_TIMER_INDEX                           TIMER_INDEX_2   /* 硬件定时器2索引 */
#define TIMER_PRIO                                  2               /* 硬件定时器优先级 */

#define MESH_SCENE_SLE_SLOT_WIN                     125
#define MESH_SAMPLE_SLOT_TO_MS                      8               /* 将slot时隙转换成毫秒 */
#define SAMPLE_SYNC_CLOCK_DEFAULT_SLOT              500            /* 默认时隙 */
#define SAMPLE_TIMER_IGNORED_TIME_US              (80 * 1000)            /* 定时器里默认忽略的时间-微秒 */

/* 开关开关UUID */
#define LIGHT_PROPERTY_SWITCH_UUID 0x301

/* 时钟同步 */
#define MESH_SCENE_MAX_SLOT125_CLOCK ((1L << 30) - 1)
#define MESH_SCENE_CLK_125_ADD(clock_a, clock_b) ((uint32_t)((uint32_t)((clock_a) + \
                                                  (clock_b)) & MESH_SCENE_MAX_SLOT125_CLOCK))
#define MESH_SCENE_CLK_125_SUB(clock_a, clock_b) ((uint32_t)((uint32_t)((clock_a) - \
                                                  (clock_b)) & MESH_SCENE_MAX_SLOT125_CLOCK))

#define MESH_SCENE_CLK_125_DIFF(clock_a, clock_b) \
    ((MESH_SCENE_CLK_125_SUB((clock_b), (clock_a)) > ((MESH_SCENE_MAX_SLOT125_CLOCK + 1) >> 1)) ? \
     (-(int32_t)((MESH_SCENE_CLK_125_SUB((clock_a), (clock_b))))) : \
     ((int32_t)((MESH_SCENE_CLK_125_SUB((clock_b), (clock_a))))))

/* AT指令参数：不同模式解析不同字段 */
#pragma pack(1)
typedef struct {
    uint8_t onoff_status;            /* 设置开关灯状态 */
    uint32_t exec_time;              /* MF/MT开始执行时间戳 */
} demo_ctrl_data_t;
#pragma pack()

int sle_enter_net_sample_init(void);

void sle_mesh_led_demo_ctrl(void);

void sample_hardware_timer_init(void);

void sle_mesh_led_demo_task(void);

uint32_t sample_adapt_get_sys_time(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of enter_net.h */
