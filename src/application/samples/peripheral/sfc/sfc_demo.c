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
#include "sfc.h"
#include "sfc_porting.h"
#include "app_init.h"
#include "memory_config_common.h"

#define SFC_TASK_PRIO                   24
#define SFC_TASK_STACK_SIZE             0x1000
#define SFC_SAMPLE_LEN                  0x80000
#define SFC_PRINT_BUFF_LEN              32

static uint8_t g_full_ff_buff[SFC_PRINT_BUFF_LEN] = {0};
static uint8_t g_read_data_buff[SFC_PRINT_BUFF_LEN] = {0};
static uint8_t g_write_data_buff[SFC_PRINT_BUFF_LEN] = {0};

static void sfc_err_data_print(uint32_t start_addr, uint32_t data_len, uint8_t *expected_data_buf)
{
    osal_printk("addr=0x%x, len=%u, flash data error!\r\n", start_addr, data_len);
    osal_printk("expected data: ");
    for (uint8_t i = 0; i < data_len; i++) {
        osal_printk("%02x ", expected_data_buf[i]);
    }
    osal_printk("\r\n");

    osal_printk("  actual data: ");
    for (uint8_t i = 0; i < data_len; i++) {
        osal_printk("%02x ", g_read_data_buff[i]);
    }
    osal_printk("\r\n");
}

static errcode_t sfc_read_test_single(uint8_t *expected_data_buf)
{
    uint32_t remained_len = CONFIG_SFC_SAMPLE_USER_SIZE;
    uint32_t start_addr = CONFIG_SFC_SAMPLE_USER_ADDR;

    while (remained_len > 0) {
        uint32_t cur_len = remained_len > SFC_PRINT_BUFF_LEN ? SFC_PRINT_BUFF_LEN : remained_len;
        uapi_sfc_reg_read(start_addr, g_read_data_buff, cur_len);
        if (memcmp(expected_data_buf, g_read_data_buff, cur_len) != 0) {
            sfc_err_data_print(start_addr, cur_len, expected_data_buf);
            return ERRCODE_FAIL;
        }
        // The flash region is full of 0xFF. Write data for next read test.
        if (expected_data_buf == g_full_ff_buff) {
            uapi_sfc_reg_write(start_addr, g_write_data_buff, cur_len);
        }

        start_addr += cur_len;
        remained_len -= cur_len;
    }
    return ERRCODE_SUCC;
}

static errcode_t sfc_sample_start_api_test(void)
{
    /* Erase User space */
    osal_printk("Erasing for API sample...\r\n");
    errcode_t ret = uapi_sfc_reg_erase(CONFIG_SFC_SAMPLE_USER_ADDR, CONFIG_SFC_SAMPLE_USER_SIZE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("flash erase failed! ret = 0x%x\r\n", ret);
        return ERRCODE_FAIL;
    }

    osal_printk("Start API read sample...\r\n");
    if (sfc_read_test_single(g_full_ff_buff) != ERRCODE_SUCC) {   // read back after erase
        return ERRCODE_FAIL;
    }
    if (sfc_read_test_single(g_write_data_buff) != ERRCODE_SUCC) {   // read back after write
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static void *sfc_task(const char *arg)
{
    unused(arg);
    for (uint8_t i = 0; i < SFC_PRINT_BUFF_LEN; i++) {
        g_write_data_buff[i] = i;
        g_full_ff_buff[i] = 0xFF;
    }

    errcode_t ret = sfc_sample_start_api_test();
    if (ret != ERRCODE_SUCC) {
        osal_printk("sfc sample failed!\r\n");
    } else {
        osal_printk("sfc sample succ!\r\n");
    }

    return NULL;
}

static void sfc_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sfc_task, 0, "SFCTask", SFC_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SFC_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(sfc_entry);