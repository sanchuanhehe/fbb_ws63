/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * @if Eng
 * @brief Demonstrates the allocation, validation, failure handling, and release of OSAL dynamic memory.
 * @else
 * @brief 演示 OSAL 动态内存的分配、验证、失败处理与释放方法。
 * @endif
 */

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define OSAL_MEMORY_TEST_COUNT 7U
#include "osal_addr.h"

#define OSAL_MEMORY_TASK_PRIO 24
#define OSAL_MEMORY_TASK_STACK_SIZE 0x1000
#define OSAL_MEMORY_NORMAL_SIZE 256U
#define OSAL_MEMORY_LARGE_SIZE 8192U
#define OSAL_MEMORY_FAILURE_SIZE (1024U * 1024U)
#define OSAL_MEMORY_ALIGNMENT 32U
#define OSAL_MEMORY_PATTERN_SEED 0x5AU

#define OSAL_MEMORY_LOG "[osal memory]"

/**
 * @if Eng
 * @brief Stores the execution statistics of all dynamic memory test cases.
 * @else
 * @brief 保存全部动态内存测试项的执行统计。
 * @endif
 */
typedef struct {
    uint32_t passed;
    uint32_t failed;
    uint32_t allocated;
    uint32_t freed;
} osal_memory_stats_t;

/**
 * @if Eng
 * @brief Records an allocation failure and prints the safe-degradation result.
 * @else
 * @brief 记录内存分配失败，并打印安全降级结果。
 * @endif
 */
static bool osal_memory_handle_alloc_failure(const char *name, uint32_t size)
{
    osal_printk("%s %s size=%u allocation failed, skip test\r\n", OSAL_MEMORY_LOG, name, size);
    return false;
}

/**
 * @if Eng
 * @brief Writes a deterministic pattern, reads it back, and returns its checksum.
 * @else
 * @brief 写入确定性数据并回读，返回数据校验和。
 * @endif
 */
static bool osal_memory_fill_and_verify(uint8_t *buffer, uint32_t size, uint32_t *checksum)
{
    uint32_t write_checksum = 0;
    uint32_t read_checksum = 0;

    for (uint32_t index = 0; index < size; index++) {
        buffer[index] = (uint8_t)(OSAL_MEMORY_PATTERN_SEED + index);
        write_checksum += buffer[index];
    }
    for (uint32_t index = 0; index < size; index++) {
        read_checksum += buffer[index];
    }

    *checksum = read_checksum;
    return write_checksum == read_checksum;
}

/**
 * @if Eng
 * @brief Checks whether every byte in a newly allocated zeroed buffer is zero.
 * @else
 * @brief 检查清零分配得到的缓冲区是否每个字节都为零。
 * @endif
 */
static bool osal_memory_verify_zero(const uint8_t *buffer, uint32_t size)
{
    for (uint32_t index = 0; index < size; index++) {
        if (buffer[index] != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @if Eng
 * @brief Tests ordinary heap allocation with osal_kmalloc and osal_kfree.
 * @else
 * @brief 测试 osal_kmalloc 与 osal_kfree 普通堆内存分配流程。
 * @endif
 */
static bool osal_memory_test_kmalloc(osal_memory_stats_t *stats)
{
    uint32_t checksum = 0;
    uint8_t *buffer = (uint8_t *)osal_kmalloc(OSAL_MEMORY_NORMAL_SIZE, OSAL_GFP_KERNEL);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("kmalloc", OSAL_MEMORY_NORMAL_SIZE);
    }
    stats->allocated++;

    bool passed = osal_memory_fill_and_verify(buffer, OSAL_MEMORY_NORMAL_SIZE, &checksum);
    osal_kfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s kmalloc size=%u checksum=0x%x %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_NORMAL_SIZE, checksum,
                passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Tests zeroed heap allocation with osal_kzalloc and osal_kfree.
 * @else
 * @brief 测试 osal_kzalloc 与 osal_kfree 清零堆内存分配流程。
 * @endif
 */
static bool osal_memory_test_kzalloc(osal_memory_stats_t *stats)
{
    uint8_t *buffer = (uint8_t *)osal_kzalloc(OSAL_MEMORY_NORMAL_SIZE, OSAL_GFP_KERNEL);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("kzalloc", OSAL_MEMORY_NORMAL_SIZE);
    }
    stats->allocated++;

    bool passed = osal_memory_verify_zero(buffer, OSAL_MEMORY_NORMAL_SIZE);
    osal_kfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s kzalloc size=%u zero_check %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_NORMAL_SIZE,
                passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Tests aligned heap allocation with osal_kmalloc_align and osal_kfree.
 * @else
 * @brief 测试 osal_kmalloc_align 与 osal_kfree 对齐堆内存分配流程。
 * @endif
 */
static bool osal_memory_test_kmalloc_align(osal_memory_stats_t *stats)
{
    uint32_t checksum = 0;
    uint8_t *buffer = (uint8_t *)osal_kmalloc_align(OSAL_MEMORY_NORMAL_SIZE, OSAL_GFP_KERNEL, OSAL_MEMORY_ALIGNMENT);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("kmalloc_align", OSAL_MEMORY_NORMAL_SIZE);
    }
    stats->allocated++;

    bool aligned = ((uintptr_t)buffer % OSAL_MEMORY_ALIGNMENT) == 0;
    bool verified = osal_memory_fill_and_verify(buffer, OSAL_MEMORY_NORMAL_SIZE, &checksum);
    bool passed = aligned && verified;
    osal_kfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s kmalloc_align size=%u align=%u checksum=0x%x %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_NORMAL_SIZE,
                OSAL_MEMORY_ALIGNMENT, checksum, passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Tests aligned zeroed allocation with osal_kzalloc_align and osal_kfree.
 * @else
 * @brief 测试 osal_kzalloc_align 与 osal_kfree 对齐清零分配流程。
 * @endif
 */
static bool osal_memory_test_kzalloc_align(osal_memory_stats_t *stats)
{
    uint8_t *buffer = (uint8_t *)osal_kzalloc_align(OSAL_MEMORY_NORMAL_SIZE, OSAL_GFP_KERNEL, OSAL_MEMORY_ALIGNMENT);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("kzalloc_align", OSAL_MEMORY_NORMAL_SIZE);
    }
    stats->allocated++;

    bool aligned = ((uintptr_t)buffer % OSAL_MEMORY_ALIGNMENT) == 0;
    bool passed = aligned && osal_memory_verify_zero(buffer, OSAL_MEMORY_NORMAL_SIZE);
    osal_kfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s kzalloc_align size=%u align=%u zero_check %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_NORMAL_SIZE,
                OSAL_MEMORY_ALIGNMENT, passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Tests a large buffer allocation with osal_vmalloc and osal_vfree.
 * @else
 * @brief 测试 osal_vmalloc 与 osal_vfree 较大缓冲区分配流程。
 * @endif
 */
static bool osal_memory_test_vmalloc(osal_memory_stats_t *stats)
{
    uint32_t checksum = 0;
    uint8_t *buffer = (uint8_t *)osal_vmalloc(OSAL_MEMORY_LARGE_SIZE);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("vmalloc", OSAL_MEMORY_LARGE_SIZE);
    }
    stats->allocated++;

    bool passed = osal_memory_fill_and_verify(buffer, OSAL_MEMORY_LARGE_SIZE, &checksum);
    osal_vfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s vmalloc size=%u checksum=0x%x %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_LARGE_SIZE, checksum,
                passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Tests a large zeroed buffer allocation with osal_vzalloc and osal_vfree.
 * @else
 * @brief 测试 osal_vzalloc 与 osal_vfree 较大清零缓冲区分配流程。
 * @endif
 */
static bool osal_memory_test_vzalloc(osal_memory_stats_t *stats)
{
    uint8_t *buffer = (uint8_t *)osal_vzalloc(OSAL_MEMORY_LARGE_SIZE);
    if (buffer == NULL) {
        return osal_memory_handle_alloc_failure("vzalloc", OSAL_MEMORY_LARGE_SIZE);
    }
    stats->allocated++;

    bool passed = osal_memory_verify_zero(buffer, OSAL_MEMORY_LARGE_SIZE);
    osal_vfree(buffer);
    buffer = NULL;
    stats->freed++;

    osal_printk("%s vzalloc size=%u zero_check %s\r\n", OSAL_MEMORY_LOG, OSAL_MEMORY_LARGE_SIZE,
                passed ? "PASS" : "FAIL");
    return passed;
}

/**
 * @if Eng
 * @brief Requests 1 MiB to trigger an expected allocation failure and verifies safe handling.
 * @else
 * @brief 申请 1 MiB 内存以触发预期分配失败，并验证失败分支能够安全处理。
 * @endif
 */
static bool osal_memory_test_expected_failure(osal_memory_stats_t *stats)
{
    uint8_t *buffer = (uint8_t *)osal_kmalloc(OSAL_MEMORY_FAILURE_SIZE, OSAL_GFP_KERNEL);
    if (buffer == NULL) {
        osal_printk("%s expected_failure size=%u return=NULL handled PASS\r\n", OSAL_MEMORY_LOG,
                    OSAL_MEMORY_FAILURE_SIZE);
        return true;
    }

    stats->allocated++;
    osal_kfree(buffer);
    buffer = NULL;
    stats->freed++;
    osal_printk("%s expected_failure size=%u return=NON-NULL unexpected FAIL\r\n", OSAL_MEMORY_LOG,
                OSAL_MEMORY_FAILURE_SIZE);
    return false;
}

/**
 * @if Eng
 * @brief Updates the number of passed and failed test cases.
 * @else
 * @brief 更新通过和失败的测试项数量。
 * @endif
 */
static void osal_memory_record_result(osal_memory_stats_t *stats, bool passed)
{
    if (passed) {
        stats->passed++;
    } else {
        stats->failed++;
    }
}

/**
 * @if Eng
 * @brief Runs all OSAL dynamic memory allocation tests and prints the summary.
 * @else
 * @brief 执行全部 OSAL 动态内存分配测试并打印汇总结果。
 * @endif
 */
static void *osal_memory_test_task(const char *arg)
{
    unused(arg);
    osal_memory_stats_t stats = {0};

    osal_printk("%s sample start\r\n", OSAL_MEMORY_LOG);
    osal_memory_record_result(&stats, osal_memory_test_kmalloc(&stats));
    osal_memory_record_result(&stats, osal_memory_test_kzalloc(&stats));
    osal_memory_record_result(&stats, osal_memory_test_kmalloc_align(&stats));
    osal_memory_record_result(&stats, osal_memory_test_kzalloc_align(&stats));
    osal_memory_record_result(&stats, osal_memory_test_vmalloc(&stats));
    osal_memory_record_result(&stats, osal_memory_test_vzalloc(&stats));
    osal_memory_record_result(&stats, osal_memory_test_expected_failure(&stats));

    uint32_t leak = stats.allocated - stats.freed;
    osal_printk("%s summary passed=%u failed=%u alloc=%u free=%u leak=%u\r\n", OSAL_MEMORY_LOG, stats.passed,
                stats.failed, stats.allocated, stats.freed, leak);
    osal_printk("%s %s\r\n", OSAL_MEMORY_LOG,
                (stats.passed == OSAL_MEMORY_TEST_COUNT && stats.failed == 0 && leak == 0) ? "ALL TESTS PASS"
                                                                                           : "TESTS FAILED");
    return NULL;
}

/**
 * @if Eng
 * @brief Creates the OSAL dynamic memory sample task.
 * @else
 * @brief 创建 OSAL 动态内存案例任务。
 * @endif
 */
static void osal_memory_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)osal_memory_test_task, 0, "OsalMemoryTask",
                                      OSAL_MEMORY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, OSAL_MEMORY_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(osal_memory_entry);
