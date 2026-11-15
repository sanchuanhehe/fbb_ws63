/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides SFC sample source \n
 *
 * History: \n
 * 2024-03-04, Create file. \n
 */
#include "soc_osal.h"
#include "securec.h"
#include "watchdog.h"
#include "tcxo.h"
#include "trng.h"
#include "partition.h"
#include "partition_resource_id.h"
#include "sfc.h"
#include "sfc_porting.h"
#include "app_init.h"
#include "memory_config_common.h"

#define SFC_TASK_PRIO                   24
#define SFC_TASK_STACK_SIZE             0x2000

#define SFC_FLASH_TEST_LOOP_CNT 10
#define SFC_FLASH_TEST_ERASE 0
#define SFC_FLASH_TEST_WRITE 1
#define SFC_FLASH_TEST_READ  2
#define SFC_FLASH_TEST_ERASE_CHECK_SIZE 0x1000
#define SFC_FLASH_TEST_MAX_SIZE 0x10000
static uint8_t g_flash_write_buf[SFC_FLASH_TEST_MAX_SIZE] = {0};
static uint8_t g_flash_read_buf[SFC_FLASH_TEST_MAX_SIZE] = {0};
static uint32_t g_flash_test_addr;
static uint32_t g_flash_test_max_size;
typedef struct {
    uint32_t test_size;
    uint64_t erase_time;
    uint64_t erase_max_time;
    uint64_t erase_min_time;
    uint64_t write_time;
    uint64_t write_max_time;
    uint64_t write_min_time;
    uint64_t read_time;
    uint64_t read_max_time;
    uint64_t read_min_time;
} sfc_flash_test_t;
#define UINT64_T_MAX 0xFFFFFFFFFFFFFFFF
static sfc_flash_test_t g_flash_test_group[] = {
    {0x1000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},   /* 4K */
    {0x2000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},   /* 8K */
    {0x8000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},   /* 32K */
    {0x10000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},  /* 64K */
    {0x20000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},  /* 128K */
    {0x40000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},  /* 256K */
    {0x80000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX},  /* 512K */
    {0x100000, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX, 0, 0, UINT64_T_MAX}, /* 1M */
};

static uint64_t sfc_demo_get_timestamp(void)
{
    return uapi_tcxo_get_us();
}

static errcode_t sfc_demo_get_test_addr(void)
{
    partition_information_t info;
    if (uapi_partition_get_info(PARTITION_FOTA_DATA, &info) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if ((info.part_info.addr_info.addr == 0) || (info.part_info.addr_info.size == 0)) {
        osal_printk("Flash test param invalid:addr=0x%x, size=0x%x\r\n", info.part_info.addr_info.addr,
            info.part_info.addr_info.size);
        return ERRCODE_FAIL;
    }
    g_flash_test_addr = info.part_info.addr_info.addr;
    g_flash_test_max_size = info.part_info.addr_info.size;
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_flash_erase_test(sfc_flash_test_t *single_test)
{
    uint64_t cur_stamp = 0;
    uint64_t old_stamp = 0;
    uint32_t test_size = 0;
    uint32_t test_offset = 0;
    uint8_t erase_buf[SFC_FLASH_TEST_ERASE_CHECK_SIZE];
    sfc_flash_test_t *t = single_test;
    old_stamp = sfc_demo_get_timestamp();
    if (uapi_sfc_reg_erase(g_flash_test_addr, t->test_size) != ERRCODE_SUCC) {
        osal_printk("Flash test erase failed, size:%d\r\n", t->test_size);
        return ERRCODE_FAIL;
    }
    cur_stamp = sfc_demo_get_timestamp();
    t->erase_time += cur_stamp - old_stamp;
    t->erase_max_time = t->erase_max_time > (cur_stamp - old_stamp) ? t->erase_max_time : (cur_stamp - old_stamp);
    t->erase_min_time = t->erase_min_time < (cur_stamp - old_stamp) ? t->erase_min_time : (cur_stamp - old_stamp);
    /* 回读确认是否擦除成功 */
    memset_s(erase_buf, SFC_FLASH_TEST_ERASE_CHECK_SIZE, -1, SFC_FLASH_TEST_ERASE_CHECK_SIZE);
    test_size = single_test->test_size < SFC_FLASH_TEST_ERASE_CHECK_SIZE ? \
        single_test->test_size : SFC_FLASH_TEST_ERASE_CHECK_SIZE;
    while (test_offset < single_test->test_size) {
        if (uapi_sfc_reg_read(g_flash_test_addr + test_offset, g_flash_read_buf, test_size) != ERRCODE_SUCC) {
            osal_printk("erase read back failed, size:%d\r\n", test_size);
            return ERRCODE_FAIL;
        }
        if (memcmp(erase_buf, g_flash_read_buf, test_size) != 0) {
            osal_printk("erase read check failed, size:%d\r\n", test_offset);
            return ERRCODE_FAIL;
        }
        test_offset += test_size;
        if ((test_offset + test_size) > single_test->test_size) {
            test_size = single_test->test_size - test_offset;
        }
    }
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_flash_write_test(sfc_flash_test_t *single_test, uint32_t offset, uint32_t size)
{
    uint64_t cur_stamp = 0;
    uint64_t old_stamp = 0;
    sfc_flash_test_t *t = single_test;
    old_stamp = sfc_demo_get_timestamp();
    if (uapi_sfc_reg_write(g_flash_test_addr + offset, g_flash_write_buf, size) != ERRCODE_SUCC) {
        osal_printk("Flash test write failed, offset:%d\r\n", offset);
        return ERRCODE_FAIL;
    }
    cur_stamp = sfc_demo_get_timestamp();
    t->write_time += cur_stamp - old_stamp;
    t->write_max_time = t->write_max_time > (cur_stamp - old_stamp) ? t->write_max_time : (cur_stamp - old_stamp);
    t->write_min_time = t->write_min_time < (cur_stamp - old_stamp) ? t->write_min_time : (cur_stamp - old_stamp);
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_flash_read_test(sfc_flash_test_t *single_test, uint32_t offset, uint32_t size)
{
    uint64_t cur_stamp = 0;
    uint64_t old_stamp = 0;
    sfc_flash_test_t *t = single_test;
    old_stamp = sfc_demo_get_timestamp();
    if (uapi_sfc_reg_read(g_flash_test_addr + offset, g_flash_read_buf, size) != ERRCODE_SUCC) {
        osal_printk("Flash test read failed, size:%d\r\n", size);
        return ERRCODE_FAIL;
    }
    cur_stamp = sfc_demo_get_timestamp();
    t->read_time += cur_stamp - old_stamp;
    t->read_max_time = t->read_max_time > (cur_stamp - old_stamp) ? t->read_max_time : (cur_stamp - old_stamp);
    t->read_min_time = t->read_min_time < (cur_stamp - old_stamp) ? t->read_min_time : (cur_stamp - old_stamp);
    if (memcmp(g_flash_write_buf, g_flash_read_buf, size) != 0) {
        osal_printk("Flash test cmp failed, size:%d\r\n", size);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_flash_single_test(sfc_flash_test_t *single_test)
{
    uint32_t test_size = 0;
    uint32_t test_offset = 0;

    if (sfc_sample_flash_erase_test(single_test) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    test_size = single_test->test_size < SFC_FLASH_TEST_MAX_SIZE ? single_test->test_size : SFC_FLASH_TEST_MAX_SIZE;
    while (test_offset < single_test->test_size) {
        if (sfc_sample_flash_write_test(single_test, test_offset, test_size) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        if (sfc_sample_flash_read_test(single_test, test_offset, test_size) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        test_offset += test_size;
        if ((test_offset + test_size) > single_test->test_size) {
            test_size = single_test->test_size - test_offset;
        }
    }
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_start_flash_test(void)
{
    uint32_t i, j;
    uint32_t irq_sts;
    if (uapi_drv_cipher_trng_get_random_bytes(g_flash_write_buf, SFC_FLASH_TEST_MAX_SIZE) != ERRCODE_SUCC) {
        osal_printk("Set random write buf faild\r\n");
        return ERRCODE_FAIL;
    }
    if (memset_s(g_flash_read_buf, SFC_FLASH_TEST_MAX_SIZE, 0, SFC_FLASH_TEST_MAX_SIZE) != EOK) {
        osal_printk("Flash test init faild\r\n");
        return ERRCODE_FAIL;
    }
    for (i = 0; i < sizeof(g_flash_test_group) / sizeof(sfc_flash_test_t); i++) {
        if (g_flash_test_group[i].test_size > g_flash_test_max_size) {
            osal_printk("Test size too large, size:%d, max:%d\r\n", g_flash_test_group[i].test_size,
                g_flash_test_max_size);
            continue;
        }
        for (j = 0; j < SFC_FLASH_TEST_LOOP_CNT; j++) {
            uapi_watchdog_kick();
            irq_sts = osal_irq_lock();
            if (sfc_sample_flash_single_test(&(g_flash_test_group[i])) != ERRCODE_SUCC) {
                osal_irq_restore(irq_sts);
                return ERRCODE_FAIL;
            }
            osal_irq_restore(irq_sts);
        }
        osal_printk("Finish flash test [%d]\r\n", g_flash_test_group[i].test_size);
    }
    return ERRCODE_SUCC;
}

static void sfc_sample_flash_test_result(void)
{
    uint32_t i;
    osal_printk("||=========================================Flash test result, loop cnt: %02d"
                "==========================================||\r\n", SFC_FLASH_TEST_LOOP_CNT);
    osal_printk("|| Size(Byte) ||             READ(us)           ||            WRITE(us)           "
                "||            ERASE(us)           ||\r\n");
    osal_printk("||            ||    AVE   |    MAX   |    MIN   ||    AVE   |    MAX   |    MIN   ||    AVE   "
                "|    MAX   |    MIN   ||\r\n");
    for (i = 0; i < sizeof(g_flash_test_group) / sizeof(sfc_flash_test_t); i++) {
        if (g_flash_test_group[i].test_size > g_flash_test_max_size) {
            continue;
        }
        osal_printk("||  0x%06X  || %08llu | %08llu | %08llu || %08llu | %08llu | %08llu || %08llu | %08llu | %08llu "
            "||\r\n",
            g_flash_test_group[i].test_size,
            g_flash_test_group[i].read_time / SFC_FLASH_TEST_LOOP_CNT,
            g_flash_test_group[i].read_max_time, g_flash_test_group[i].read_min_time,
            g_flash_test_group[i].write_time / SFC_FLASH_TEST_LOOP_CNT,
            g_flash_test_group[i].write_max_time, g_flash_test_group[i].write_min_time,
            g_flash_test_group[i].erase_time / SFC_FLASH_TEST_LOOP_CNT,
            g_flash_test_group[i].erase_max_time, g_flash_test_group[i].erase_min_time);
    }
    osal_printk("||================================================Flash test result======="
                "==========================================||\r\n");
}

static void *sfc_flash_task(const char *arg)
{
    unused(arg);
    osal_printk("Start flash test sample...\r\n");
    if (sfc_demo_get_test_addr() != ERRCODE_SUCC) {
        osal_printk("get flash test add failed\r\n");
        return NULL;
    }
    if (sfc_sample_start_flash_test() != ERRCODE_SUCC) {
        return NULL;
    }
    sfc_sample_flash_test_result();
    return NULL;
}


static void sfc_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sfc_flash_task, 0, "SFCFlashTask", SFC_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SFC_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(sfc_entry);