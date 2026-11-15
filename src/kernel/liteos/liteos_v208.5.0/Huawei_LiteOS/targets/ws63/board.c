/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: liteos platform config files \n
 *
 * History: \n
 * 2023-05-25, Create file. \n
 */

#include "los_typedef.h"
#include "los_printf.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_init_pri.h"
#include "los_init.h"
#include "los_tick_pri.h"
// #include "asm/dma.h"
#include "asm/memmap_config.h"

#ifdef LOSCFG_DRIVERS_BASE
#include "los_driverbase.h"
#endif
#include "nmi_adapt.h"

#include "los_exc_pri.h"
#include "los_cpup_pri.h"
#include "los_sem_pri.h"
#include "board_ws63.h"

#ifdef LOSCFG_MEM_TASK_STAT
#include "los_memstat_pri.h"
#endif

#ifdef LOSCFG_MEM_LEAKCHECK_CUSTOM
#include "los_memory_pri.h"
#endif

#ifdef LOSCFG_COMPAT_POSIX
#include <sys/uio.h>
#endif

// Watchdog interrupt type.
#define NMI_WDT_MCAUSE 0x8000000C
#define NMI_WDT_CCAUSE 2
#define NMI_WDT_PROC_RETURN     0
#define NMI_WDT_PROC_CONTINUE   1

#define TASK_NAME_FMT "%-23s0x%-18.*lx0x%-5x"
#define TASK_CPU_FMT "0x%04x  %4d   "
#define TASK_STACK_FMT "%-11u%-13s0x%-11x0x%-11x  0x%-18.*lx   0x%-18.*lx   0x%-11x"
#define TASK_EVENT_FMT "0x%-6x"
#define TASK_CPUP_FMT " %4u.%1u%7u.%1u%9u.%1u   "
#define TASK_MEM_FMT "       %-11u"
#define INVALID_SEM_ID  0xFFFFFFFF
#define TASK_NAME_MAX 23

extern const CHAR *OsTskStatusConvertStr(UINT16 taskStatus);
extern LITE_OS_SEC_BSS UINT32 g_taskMaxNum;
extern uint32_t read_from_flash(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

#ifndef LOSCFG_LIB_CONFIGURABLE
UINT32 gSysClock = OS_SYS_CLOCK_DEFAULT;
#else
UINT32 gSysClock = OS_SYS_CLOCK_CONFIG_DEFAULT;
#endif /* LOSCFG_LIB_CONFIGURABLE */

#ifdef LOSCFG_POSIX_STUB_VFS
ssize_t write(int fd, const void *buf, size_t nbytes)
{
    return 0;
}
int close(int fd)
{
    return 0;
}
off_t lseek(int fd, off_t offset, int whence)
{
    return 0;
}
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    return 0;
}
int dl_iterate_phdr(int (*callback)(void *info, size_t size, void *data), void *data)
{
    return 0;
}
#endif

#ifndef LOSCFG_LIB_STDIO
FILE *const stderr;

int fputs(const char *restrict s, FILE *restrict f)
{
    return 0;
}

int fputc(int c, FILE *f)
{
    return 0;
}
#endif

VOID LOS_SetSysClosk(UINT32 clock)
{
    gSysClock = clock;
}

UINT32 LOS_GetSysClosk(VOID)
{
    return gSysClock;
}

static LITE_OS_SEC_TEXT VOID LOS_NMIHandler(ExcContext *excBufAddr)
{
    (void)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
    do_hard_fault_handler(excBufAddr);
}

VOID LOS_PrepareMainTask(VOID)
{
    ArchSetNMIHook((NMI_PROC_FUNC)LOS_NMIHandler);
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
#if defined(LOSCFG_MEM_LEAKCHECK_CUSTOM) && defined(LOSCFG_MEM_LEAKCHECK)
    LOS_HookReg(LOS_HOOK_MEMLEAK_CUSTOM, ArchBackTraceCustom);
#endif
}

void dma_cache_clean(unsigned int start, unsigned int end)
{
    ArchDCacheCleanByAddr(start, end);
}

void dma_cache_inv(unsigned int start, unsigned int end)
{
    ArchDCacheFlushByAddr(start, end);
}

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

__attribute__((weak)) VOID BoardConfig(VOID)
{
    /* config start memory before startup usb_init */

    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}

UINT32 oal_get_sleep_ticks(VOID)
{
    UINT32 intSave;
    intSave = LOS_IntLock();
    UINT32 taskTimeout = OsSortLinkGetNextExpireTime(&OsPercpuGet()->taskSortLink);
#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrTimeout = OsSortLinkGetNextExpireTime(&OsPercpuGet()->swtmrSortLink);
    if (swtmrTimeout < taskTimeout) {
        taskTimeout = swtmrTimeout;
    }
#endif
    LOS_IntRestore(intSave);
    return taskTimeout;
}

VOID oal_ticks_restore(UINT32 ticks)
{
    UINT32 intSave;
    intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += ticks;
    OsSortLinkUpdateExpireTime(ticks, &OsPercpuGet()->taskSortLink);
#ifdef LOSCFG_BASE_CORE_SWTMR
    OsSortLinkUpdateExpireTime(ticks, &OsPercpuGet()->swtmrSortLink);
#endif
    LOS_IntRestore(intSave);
}

VOID crashinfo_taskinfo_title_print(VOID)
{
    PRINTK("\r\nName                   TaskEntryAddr       TID    ");
#ifdef LOSCFG_KERNEL_SMP
    PRINTK("Affi    CPU    ");
#endif
    PRINTK("Priority   Status       "
                   "StackSize    WaterLine      StackPoint             TopOfStack             SemID        EventMask");

#ifdef LOSCFG_MEM_TASK_STAT
    PRINTK("   MEMUSE");
#endif
    PRINTK("\n");
    PRINTK("----                   -------------       ---    ");
#ifdef LOSCFG_KERNEL_SMP
    PRINTK("-----   ----   ");
#endif
    PRINTK("--------   --------     "
           "---------    ----------     ----------             ----------             ----------   ---------");
#ifdef LOSCFG_MEM_TASK_STAT
    PRINTK("   ------");
#endif
    PRINTK("\n");
}

VOID crashinfo_taskinfo_print(const LosTaskCB *allTaskArray, UINT32 *water_line_info_arr, UINT32 flash_save_offset,
    UINT32 task_num)
{
    const LosTaskCB *taskCB = NULL;
    UINT32 loop;
    UINT32 semId = 0;
    UINT32 save_offset = flash_save_offset + sizeof(UINT32);
    UINT32 task_name_len = 0;
    CHAR task_name[TASK_NAME_MAX] = {0};

    for (loop = 0; loop < task_num; ++loop) {
        taskCB = allTaskArray + loop;
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }
        read_from_flash(save_offset, (UINT8 *)(uintptr_t)&semId, sizeof(UINT32));
        save_offset += sizeof(UINT32);
        read_from_flash(save_offset, (UINT8 *)(uintptr_t)&task_name_len, sizeof(UINT32));
        save_offset += sizeof(UINT32);
        read_from_flash(save_offset, (UINT8 *)task_name, task_name_len + 1);
        save_offset += task_name_len + 1;

        PRINTK(TASK_NAME_FMT, task_name, OS_HEX_ADDR_WIDTH, (UINTPTR)taskCB->taskEntry, taskCB->taskId);
        if (water_line_info_arr != NULL) {
            PRINTK(TASK_STACK_FMT, taskCB->priority,
                OsTskStatusConvertStr(taskCB->taskStatus), taskCB->stackSize,
                water_line_info_arr[taskCB->taskId],
                OS_HEX_ADDR_WIDTH, (UINTPTR)taskCB->stackPointer, OS_HEX_ADDR_WIDTH, taskCB->topOfStack, semId);
        }

        PRINTK(TASK_EVENT_FMT, taskCB->eventMask);
#ifdef LOSCFG_MEM_TASK_STAT
        PRINTK(TASK_MEM_FMT, OsMemTaskUsage(taskCB->taskId));
#endif
        PRINTK("\n");
    }
}

CHAR *OsCurTaskNameGetExt(VOID)
{
    return OsCurTaskNameGet();
}

#ifdef LOSCFG_MEM_LEAKCHECK_CUSTOM
#include "los_exc_pri.h"
#define ROM_START  0x109000
#define ROM_LENGTH 0x43000
#define ROM_END    (ROM_START + ROM_LENGTH)
#define RAM_END    0xA90000
/* 放在SRAM运行的代码的text&rodata段在初始阶段需要从flash拷贝到ram */
static bool check_txt_addr_range(uint32_t pc, uint32_t text_start, uint32_t text_end)
{
    if (pc >= text_start && pc < text_end) {
        return true;
    } else {
        return false;
    }
}
 
static bool is_valid_txt_addr(uint32_t pc)
{
    /* flash text */
    if (check_txt_addr_range(pc, (uintptr_t)&__text_begin__, (uintptr_t)&__text_end__)) {
        return true;
    }
 
    /* tcm text */
    if (check_txt_addr_range(pc, (uintptr_t)&__tcm_text_begin__, (uintptr_t)&__tcm_text_end__)) {
        return true;
    }
 
    /* sram text */
    if (check_txt_addr_range(pc, (uintptr_t)&__sram_text_begin__, (uintptr_t)&__sram_text_end__)) {
        return true;
    }
 
    /* rom */
    if (check_txt_addr_range(pc, ROM_START, ROM_START + ROM_LENGTH)) {
        return true;
    }
 
    return false;
}
 
#define  USER_STACK_OFFSET_PER   4
#define  USER_STACK_PRINT_DEPTH  128
#define get_temp_sp(temp_sp) __asm volatile("mv %0, sp" : "=r"(temp_sp))
LITE_OS_SEC_TEXT void ArchBackTraceCustom(UINTPTR *array, UINTPTR arry_len)
{
    unsigned long back_sp;
    get_temp_sp(back_sp);
    uint32_t count = 0;
    while ((back_sp != 0) && (back_sp < RAM_END)) {
        if (is_valid_txt_addr(*((uint32_t *)(uintptr_t)(back_sp))) != 0) {
            if (count < arry_len) {
                array[count] =  *((uint32_t *)(back_sp));
                count++;
            } else {
                break;
            }
        }
        back_sp = back_sp + USER_STACK_OFFSET_PER;
    }
}
#endif