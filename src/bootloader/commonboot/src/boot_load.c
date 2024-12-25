/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: load
 *
 * Create: 2023-01-09
 */

#include "boot_def.h"
#include "boot_ymodem.h"
#include "boot_serial.h"
#include "boot_flash.h"
#include "boot_watchdog.h"
#include "securec.h"
#ifdef PROVISION_WRITE_WITH_CMD
#include "boot_flash.h"
#endif
#include "boot_load.h"

static uint8_t g_ymodem_buf[READ_SIZE] = { 0 };

static uint32_t receive_file(uint32_t length, uint32_t erased_size, uintptr_t addr, uint32_t *size)
{
    uint32_t read_len;
    uint32_t file_length = length;
    uintptr_t store_addr = addr;
    while (file_length > 0) {
        uint32_t read_size = (uint32_t)min(file_length, READ_SIZE);
        read_len = ymodem_read(g_ymodem_buf, read_size);
        if (read_len == 0 || file_length < read_len) {
            return ERRCODE_FAIL;
        }

#ifdef PROVISION_WRITE_WITH_INTERFACE
        unused(erased_size);
        flash_cmd_func *flash_ctr = boot_get_flash_funcs();
        uint32_t ret = flash_ctr->write(store_addr - FLASH_START, read_len, g_ymodem_buf, 0);
        if (ret != ERRCODE_SUCC) {
            boot_msg1("flash write fail", ret);
            return ERRCODE_FAIL;
        }
#else
        if (erased_size == 0) {
            if (memcpy_s((void *)store_addr, (uint32_t)file_length, g_ymodem_buf, read_len) != EOK) {
                return ERRCODE_FAIL;
            }
        } else {
            if (memcpy_s((void *)store_addr, (uint32_t)file_length, g_ymodem_buf, read_len) != EOK) {
                boot_msg0("flash write fail");
                return ERRCODE_FAIL;
            }
        }
#endif
        file_length -= read_len;
        (*size) += read_len;
        store_addr += (uint32_t)read_len;
        boot_wdt_kick();
    }

    (void)ymodem_read(g_ymodem_buf, READ_SIZE);
    return ERRCODE_SUCC;
}

uint32_t loader_serial_ymodem(uint32_t offset, uint32_t erased_size, uint32_t min, uint32_t max)
{
    uint32_t size = 0;
    uintptr_t store_addr = offset;
    uint32_t file_length, ret;

    ret = ymodem_open();
    if (ret != ERRCODE_SUCC) {
        goto err;
    }

    file_length = ymodem_get_length();
    if (file_length <= min || file_length > max) {
        boot_msg1("file length err : ", file_length);
        goto err;
    }

    if ((erased_size != 0) && (erased_size < file_length)) {
        boot_msg0("file_size > erase_size");
        goto err;
    }
    if (receive_file(file_length, erased_size, store_addr, &size) != ERRCODE_SUCC) {
        goto err;
    }
    if ((uint32_t)size == file_length) {
        boot_msg1("total size:", (uint32_t)size);
        ymodem_close();
        return ERRCODE_SUCC;
    }

err:
    ymodem_close();
    return ERRCODE_FAIL;
}