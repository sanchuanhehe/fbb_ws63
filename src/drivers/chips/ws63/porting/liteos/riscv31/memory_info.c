/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  LiteOs Heap and Stack info
 *
 * Create: 2025-07-2
 */

#include <stdint.h>
#include "los_task_pri.h"
#include "los_memory.h"
#include "chip_io.h"
#include "osal_debug.h"
#include "memory_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void print_os_task_id_and_name(void)
{
    TSK_INFO_S taskinfo;
    uint32_t ret = 0;
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = LOS_TaskInfoGet(loop, &taskinfo);
        if (ret != LOS_OK) {
            continue;
        }
        osal_printk("task_id: %d, task_name: %s\n", taskinfo.uwTaskID, taskinfo.acName);
    }
}

void print_stack_waterline_riscv(void)
{
    TSK_INFO_S taskinfo;
    uint32_t ret = 0;

    osal_printk("task_id  taskName          stackTop   stackLen   peakUsage    sp       peakRatio\r\n");
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = LOS_TaskInfoGet(loop, &taskinfo);
        if (ret != LOS_OK) {
            continue;
        }
        if ((taskinfo.usTaskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        osal_printk("%02d      %-18s 0x%08X 0x%08X 0x%08X 0x%08X %02d%%\r\n", \
            taskinfo.uwTaskID, taskinfo.acName, \
            taskinfo.uwTopOfStack, taskinfo.uwStackSize, taskinfo.uwPeakUsed, (uint32_t)((uintptr_t)(taskinfo.uwSP)), \
            taskinfo.uwPeakUsed * 100 / taskinfo.uwStackSize); // * 100 for calculate percent.
    }
}

void print_heap_statistics_riscv(void)
{
    LOS_MEM_POOL_STATUS status;

    LOS_MemInfoGet(OS_SYS_MEM_ADDR, &status);
    osal_printk("[SysHeap stat] total:0x%x, used:0x%x, current free:0x%x, peak usage:0x%x, peak free:0x%x\r\n", \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize), \
                (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize), \
                (uint32_t)(status.uwUsageWaterLine), \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize - status.uwUsageWaterLine));

    /* print all task heap usage info */
    osal_printk("Idx     TaskName        current malloc  peak malloc\r\n");
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        if (idx == g_taskMaxNum) {
            osal_printk("---------non-task alloc(e.g. startup stage, interrupt)----------\r\n");
        }
        LOS_TaskMemInfoShow((void *)OS_SYS_MEM_ADDR, idx, osal_printk);
    }
    osal_printk("Done\r\n");
}

void print_os_sys_task_heap(uint8_t taskid)
{
    LOS_MemTaskHeapInfoGet((void *)OS_SYS_MEM_ADDR, taskid, osal_printk);
}

void print_os_all_sys_task_heap(void)
{
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        print_os_sys_task_heap(idx);
    }
}

void print_os_dfx_info(void)
{
    print_stack_waterline_riscv();
    print_heap_statistics_riscv();
    print_os_all_sys_task_heap();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */