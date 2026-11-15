/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Os detailed information.
 * Author: Huawei LiteOS Team
 * Create: 2022-01-28
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "los_typedef.h"
#include "los_printf.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_tick_pri.h"
#include "los_init_pri.h"
#ifdef LOSCFG_DEBUG_RESOURCE_INFO
#include "los_resource.h"
#endif
#if (defined(LOSCFG_MEM_LEAKCHECK_CUSTOM) && defined(LOSCFG_MEM_LEAKCHECK)) || defined(LOSCFG_MEM_DEBUG)
#include "los_memory_pri.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SYS_PRINT(fmt, args...) printf(fmt, ##args)

static VOID sys_run_time_show(VOID)
{
    #define SEC_PER_DAY 86400
    #define SEC_PER_HOUR 3600
    #define SEC_PER_MIN 60
    UINT32 tot_sec = LOS_Tick2MS((UINT32)(LOS_TickCountGet())) / OS_SYS_MS_PER_SECOND;
    UINT32 run_day, run_hour, run_min, run_sec;
    run_day = tot_sec / SEC_PER_DAY;
    run_hour = tot_sec % SEC_PER_DAY / SEC_PER_HOUR;
    run_min = tot_sec % SEC_PER_HOUR / SEC_PER_MIN;
    run_sec = tot_sec % SEC_PER_MIN;
    SYS_PRINT("runtime: %dd-%02dh-%02dm-%02ds\r\n", run_day, run_hour, run_min, run_sec);
}

static VOID sys_mem_info_show(VOID)
{
    UINT32 total = LOS_MemPoolSizeGet(m_aucSysMem0);
    LOS_MEM_POOL_STATUS pool_status = {0};
    UINT32 ret = LOS_MemInfoGet(m_aucSysMem0, &pool_status);
    if (ret != LOS_OK) {
        return;
    }

    SYS_PRINT("Mem: total %u, used %u, free %u, max free blk size %u, free blk num %u\r\n", total,
        pool_status.uwTotalUsedSize, pool_status.uwTotalFreeSize, pool_status.uwMaxFreeNodeSize,
        pool_status.uwFreeNodeNum);
}

#ifdef LOSCFG_DEBUG_RESOURCE_INFO
static VOID print_resource_info(const char *name, const ResourceInfo *info)
{
    SYS_PRINT("%-7s %-5u  %-5u  %-5u  %-5u\n", name, info->maxNum, info->peak, info->used, info->unused);
}

static VOID sys_resource_info_show(VOID)
{
    ResourceInfo info;
    SYS_PRINT("resource_info:\n");
    SYS_PRINT("Name    Max    Peak   Used   Unused\n");
    SYS_PRINT("----    -----  -----  -----  -------\n");
    LOS_GetSwtmrInfo(&info);
    print_resource_info("Swtmr", &info);
    LOS_GetTaskInfo(&info);
    print_resource_info("Task", &info);
    LOS_GetMuxInfo(&info);
    print_resource_info("Mux", &info);
    LOS_GetSemInfo(&info);
    print_resource_info("Sem", &info);
    LOS_GetQueueInfo(&info);
    print_resource_info("Queue", &info);
    SYS_PRINT("\n");
}
#endif

static VOID sys_task_show(VOID)
{
    SYS_PRINT("task_info:\r\n");
    OsDbgTskInfoGet(OS_ALL_TASK_MASK);
}

VOID sys_mem_show(VOID)
{
#ifdef LOSCFG_MEM_DEBUG
    OsCheckPool((void*)m_aucSysMem0);
#endif
 
#if defined(LOSCFG_MEM_LEAKCHECK_CUSTOM) && defined(LOSCFG_MEM_LEAKCHECK)
    OsMemUsedNodeShow((void*)m_aucSysMem0);
#endif
}

VOID sys_info_show(VOID)
{
    sys_run_time_show();
    sys_mem_info_show();
#ifdef LOSCFG_DEBUG_RESOURCE_INFO
    sys_resource_info_show();
#endif
    sys_task_show();
    sys_mem_show();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
