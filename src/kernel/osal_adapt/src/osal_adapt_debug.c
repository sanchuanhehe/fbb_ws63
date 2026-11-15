/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt atomic source file.
 */
#ifdef OSAL_IRQ_RECORD_DEBUG
#include "securec.h"
#include "osal_inner.h"
#include "osal_adapt_debug.h"
#include "hal_tcxo.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static td_u8 g_irq_lock_enable = 0;
static debug_irq_record g_irq_record = {0};
static td_u32 g_irq_record_index = 0;
static td_u32 g_irq_latest_record = 0;
static td_u32 g_irq_lock_caller = 0;
#ifdef OSAL_IRQ_RECORD_STATUS
static td_u32 g_irq_lock_status = 0;
#endif
#ifdef OSAL_IRQ_RECORD_INTTER
static td_u64 g_irq_enter_time = 0;
static td_u64 g_irq_exit_time = 0;
#endif

#define OSAL_IRQ_RECORD_MAX_TICK     0xFFFFFFFFull

void osal_irq_record_flag_set(td_u8 flag)
{
    g_irq_lock_enable = flag;
}

td_u8 osal_irq_record_flag_get(void)
{
    return g_irq_lock_enable;
}

void OsIntConsumeTimeEnter(td_u32 irq_num, td_u32 func)
{
    unused(irq_num);
    unused(func);
#ifdef OSAL_IRQ_RECORD_INTTER
    if (irq_num == 0x7) {
        return;
    }
    if ((g_irq_lock_enable & OSAL_IRQ_RECORD_INTTER_CONSUME) == 0) {
        return;
    }

    g_irq_enter_time = hal_tcxo_get_funcs()->get();
    osal_irq_record(IRQ_ENTER, func, 0, 0);
#endif
}

void OsIntConsumeTimeExit(td_u32 irq_num, td_u32 func)
{
    unused(irq_num);
    unused(func);
#ifdef OSAL_IRQ_RECORD_INTTER
    if (irq_num == 0x7) {
        return;
    }
    if ((g_irq_lock_enable & OSAL_IRQ_RECORD_INTTER_CONSUME) == 0) {
        return;
    }

    g_irq_exit_time = hal_tcxo_get_funcs()->get();
    td_u64 cosume_time = g_irq_exit_time - g_irq_enter_time;
    osal_irq_record(IRQ_EXIT, func, 0, cosume_time);
#endif
}

static void osal_irq_do_record(irq_type_enum type, td_u32 caller, td_u32 stauts, td_u64 consume_time)
{
    unused(stauts);
    g_irq_record.caller[type][g_irq_record_index] = caller;
#ifdef OSAL_IRQ_RECORD_STATUS
    g_irq_record.stauts[type][g_irq_record_index] = stauts;
#endif
    /* IRQ_UNLOCK或IRQ_RESTORE 时consume_time才会大于0 */
    if (consume_time > 0) {
        g_irq_record.consume_time[g_irq_record_index] = consume_time;
        g_irq_latest_record = g_irq_record_index;
        g_irq_record_index++;
    }
    return;
}

void osal_irq_record(irq_type_enum type, td_u32 caller, td_u32 stauts, td_u64 consume_time)
{
    if (type >= IRQ_TYPE_BUFF || g_irq_lock_enable == 0 || consume_time > OSAL_IRQ_RECORD_MAX_TICK) {
        return;
    }

    unused(stauts);
    /* 记录信息直到已满 */
    if (g_irq_record_index < RECORD_CNT_MAX) {
        osal_irq_do_record(type, caller, stauts, consume_time);
        return;
    }

    if (consume_time == 0) {
        /* 信息已满，找到消耗时间最小的中断记录 */
        td_u64 min_time = g_irq_record.consume_time[0];
        td_u32 min_cnt = 0;
        for (td_u32 cnt = 0; cnt < RECORD_CNT_MAX; cnt++) {
            if (min_time > g_irq_record.consume_time[cnt]) {
                min_time = g_irq_record.consume_time[cnt];
                min_cnt = cnt;
            }
        }
        g_irq_lock_caller = caller;
#ifdef OSAL_IRQ_RECORD_STATUS
        g_irq_lock_status = stauts;
#endif
        g_irq_latest_record = min_cnt;
        /* IRQ_LOCK暂时不记录信息 */
        return;
    }

    /* 当前消耗时间大于最小消耗时间，则替换中断记录 */
    if (consume_time > g_irq_record.consume_time[g_irq_latest_record]) {
        g_irq_record.consume_time[g_irq_latest_record] = consume_time;
        if (type == IRQ_UNLOCK || type == IRQ_RESTORE) {
            g_irq_record.caller[IRQ_LOCK][g_irq_latest_record] = g_irq_lock_caller;
#ifdef OSAL_IRQ_RECORD_STATUS
            g_irq_record.stauts[IRQ_LOCK][g_irq_latest_record] = g_irq_lock_status;
#endif
        } else {
#ifdef OSAL_IRQ_RECORD_INTTER
            g_irq_record.caller[IRQ_ENTER][g_irq_latest_record] = g_irq_lock_caller;
#endif
#ifdef OSAL_IRQ_RECORD_STATUS
            g_irq_record.stauts[IRQ_ENTER][g_irq_latest_record] = g_irq_lock_status;
#endif
        }

        g_irq_record.caller[type][g_irq_latest_record] = caller;
#ifdef OSAL_IRQ_RECORD_STATUS
        g_irq_record.stauts[type][g_irq_latest_record] = stauts;
#endif
    }
}

debug_irq_record *osal_get_irq_record(void)
{
    return &g_irq_record;
}

void osal_clear_irq_record(void)
{
    memset_s(&g_irq_record, sizeof(g_irq_record), 0, sizeof(g_irq_record));
}

void osal_print_irq_record(void)
{
    osal_printk("=======> type[0:lock, 1:unlock, 2:restore 3:enter 4:exit]\r\n");
    td_u32 cnt;
    for (cnt = 0; cnt < RECORD_CNT_MAX; cnt++) {
        for (td_u32 type = 0; type < IRQ_TYPE_BUFF; type++) {
#ifdef OSAL_IRQ_RECORD_STATUS
            osal_printk("irq[%d] type[%d], caller: 0x%x, stauts: 0x%x\r\n", cnt, type,
                        g_irq_record.caller[type][cnt], g_irq_record.stauts[type][cnt]);
#else
            osal_printk("irq[%d] type[%d], caller: 0x%x\r\n", cnt, type,
                        g_irq_record.caller[type][cnt]);
#endif
        }
        osal_printk("irq[%d] cunsume_timer: %d ticks\r\n", cnt, (td_u32)g_irq_record.consume_time[cnt]);
    }
    osal_printk("latest irq is: %d.\r\n", g_irq_latest_record);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // #ifdef OSAL_IRQ_RECORD_DEBUGus