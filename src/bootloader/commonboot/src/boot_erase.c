/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: boot erase
 *
 * Create: 2023-01-09
 */
#include "boot_def.h"
#include "boot_watchdog.h"
#include "boot_transfer.h"
#include "boot_delay.h"
#include "securec.h"
#include "boot_flash.h"
#include "sfc_porting.h"
#include "boot_cmd_loop.h"
#include "boot_erase.h"

static bool g_flash_all_erased = false;
bool loader_download_is_flash_all_erased(void)
{
    return g_flash_all_erased;
}

void loader_download_set_flash_all_erase(bool all_erased)
{
    g_flash_all_erased = all_erased;
}

static uint32_t loader_get_erasing_writing(const uart_ctx *cmd_ctx, bool *need_erase_all, bool *need_write)
{
    uint32_t file_len = *(uint32_t *)(&cmd_ctx->packet.payload[4]); /* offset 4 is file length */
    uint32_t erase_size = *(uint32_t *)(&cmd_ctx->packet.payload[8]); /* offset 8 is erase size */

    if (cmd_ctx->packet.head.type != CMD_DL_IMAGE) {
        return ERRCODE_FAIL;
    }

    *need_erase_all = false;
    *need_write = false;
    if (erase_size == FLASH_CHIP_ERASE_SIZE) {
        *need_erase_all = true;
    }

    if (file_len != FLASH_CHIP_ERASE_FILE_LEN) {
        *need_write = true;
    }
    return ERRCODE_SUCC;
}

uint32_t loader_erase_all_process(const uart_ctx *cmd_ctx, bool *write_flash)
{
    uint32_t ret;
    bool need_erase_all = false;
    bool need_write = false;

    ret = loader_get_erasing_writing(cmd_ctx, &need_erase_all, &need_write);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (need_erase_all) {
        flash_cmd_func *flash = boot_get_flash_funcs();
        if (flash == NULL) {
            return ERRCODE_FAIL;
        }

        boot_wdt_kick();
        ret = flash->erase(0, FLASH_CHIP_ERASE_SIZE);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        loader_download_set_flash_all_erase(true);
    }

    *write_flash = need_write;
    return ERRCODE_SUCC;
}