/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: transfer
 *
 * Create: 2023-01-09
 */

#include "boot_def.h"
#include "boot_serial.h"
#include "boot_load.h"
#include "boot_cmd_loop.h"
#include "boot_crc.h"
#include "boot_flash.h"
#include "boot_delay.h"
#include "boot_watchdog.h"
#include "boot_transfer.h"
#ifdef CONFIG_LOADERBOOT_SUPPORT_UPLOAD_DATA
#include "boot_malloc.h"
#endif
#include "boot_erase.h"

#ifdef CONFIG_LOADERBOOT_SUPPORT_UPLOAD_DATA
upload_context *g_upload_ctx = NULL;
#endif

static uint32_t download_to_flash(uint32_t flash_offset, uint32_t erase_size, uint32_t flash_size)
{
    uint32_t size = erase_size;
    uint32_t ret = ERRCODE_FAIL;
    flash_cmd_func *flash_ctr = boot_get_flash_funcs();
    boot_wdt_kick();
#ifdef CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE
    if ((size == FLASH_CHIP_ERASE_SIZE) && !loader_download_is_flash_all_erased()) {
#else
    if (size == FLASH_CHIP_ERASE_SIZE) {
#endif
        ret = flash_ctr->erase(0, FLASH_CHIP_ERASE_SIZE);
        if (ret != ERRCODE_SUCC) {
            boot_put_errno(ERRCODE_BOOT_FLASH_ERASE_ERR);
            return ERRCODE_FAIL;
        }
    } else if (size != 0) {
        ret = flash_ctr->erase(flash_offset, size);
        if (ret != ERRCODE_SUCC) {
            boot_put_errno(ERRCODE_BOOT_FLASH_ERASE_ERR);
            return ERRCODE_FAIL;
        }
    } else {                                 /* 0x0 indicates not erase */
        size = FLASH_CHIP_ERASE_SIZE;
    }
    boot_wdt_kick();
    boot_msg0("Ready for download");
    loader_ack(ACK_SUCCESS);
    ret = loader_serial_ymodem(flash_offset, size, 0, flash_size);
    return ret;
}

uint32_t download_image(uint32_t addr, uint32_t erase_size, uint32_t flash_size, uint8_t burn_efuse)
{
    uint32_t ret;
    unused(burn_efuse);

    ret = download_to_flash(addr, erase_size, flash_size);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

uint32_t download_factory_image(uint32_t addr, uint32_t erase_size, uint32_t flash_size, uint8_t burn_efuse)
{
    uint32_t ret;
    unused(burn_efuse);
    ret = download_to_flash(addr, erase_size, flash_size);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_UPLOAD_DATA
static uint32_t upload_malloc_init(void)
{
    if (g_upload_ctx == NULL) {
        g_upload_ctx = (upload_context *)boot_malloc(sizeof(upload_context));
    }

    if (g_upload_ctx == NULL) {
        return ERRCODE_FAIL;
    }

    (void)memset_s(g_upload_ctx, sizeof(upload_context), 0, sizeof(upload_context));

    return ERRCODE_SUCC;
}

static uint32_t upload_malloc_deinit(void)
{
    return ERRCODE_SUCC;
}

static void upload_send_file_info(void)
{
    upload_context *ctx = g_upload_ctx;
    uint8_t name_length = UPLOAD_FILE_NAME_LEN;
    uint8_t *data = NULL;
    int32_t temp_length = (int32_t)ctx->file_length;
    int32_t count = 0;
    uint8_t temp_char;
    uint16_t crc;
    int32_t i;

    if (ctx->retry > 0) {
        serial_put_buf((const char *)ctx->buffer, SOH_MSG_TOTAL_LEN);
        return;
    }

    (void)memset_s(ctx->buffer, sizeof(ctx->buffer), 0, sizeof(ctx->buffer));
    ctx->buffer[0] = MODEM_SOH;
    ctx->buffer[1] = ctx->seq;
    ctx->buffer[2] = (uint8_t)~ctx->buffer[1]; /* buffer[2] is buffer[1] invert val */

    if (memcpy_s(ctx->buffer + MSG_START_LEN, SOH_MSG_LEN, ctx->file_name, name_length) != EOK) {
        return;
    }

    data = ctx->buffer + MSG_START_LEN + name_length;

    while (temp_length > 0) {
        data[count++] = (uint8_t)((uint8_t)'0' + (temp_length % DECIMAL));
        temp_length /= DECIMAL;
    }

    /* 64 bytes, enough for storing 32-bit decimal digits
       CNcomment:64字节足够存放32位10进制数 */
    for (i = 0; i < count / 2; i++) { /* count should be divided by 2 */
        temp_char = data[i];
        data[i] = data[count - i - 1];
        data[count - i - 1] = temp_char;
    }

    crc = crc16_ccitt(0, ctx->buffer + MSG_START_LEN, SOH_MSG_LEN);
    ctx->buffer[131] = (uint8_t)(crc >> 8); /* buffer[131] is crc high 8 bit */
    ctx->buffer[132] = (uint8_t)crc; /* buffer[132] is crc low 8 bit */

    serial_put_buf((const char *)ctx->buffer, SOH_MSG_TOTAL_LEN);

    ctx->status = UPLOAD_WAIT_INIT_ACK;
}

static void upload_send_data(void)
{
    upload_context *ctx = g_upload_ctx;
    uint32_t remain = ctx->file_length - ctx->offset;
    uint16_t crc;

    if (ctx->retry > 0) {
        if (ctx->buffer[0] == MODEM_SOH) {
            serial_put_buf((const char *)ctx->buffer, SOH_MSG_TOTAL_LEN);
        } else {
            serial_put_buf((const char *)ctx->buffer, UPLOAD_BUFF_LEN);
        }
        return;
    }

    ctx->status = UPLOAD_WAIT_FINAL_ACK;
    (void)memset_s(ctx->buffer, sizeof(ctx->buffer), MODEM_EOF, sizeof(ctx->buffer));

    ctx->buffer[0] = MODEM_STX;
    ctx->buffer[1] = ++ctx->seq;
    ctx->buffer[2] = (uint8_t)~ctx->buffer[1];  /* buffer[2] is buffer[1] invert val */

    if (remain <= SOH_MSG_LEN) {
        ctx->buffer[0] = MODEM_SOH;
        if (memcpy_s(ctx->buffer + MSG_START_LEN, SOH_MSG_LEN,
            (const void *)(uintptr_t)(ctx->file_addr + ctx->offset), remain) != EOK) {
            return;
        }

        crc = crc16_ccitt(0, ctx->buffer + MSG_START_LEN, SOH_MSG_LEN);
        ctx->buffer[131] = (uint8_t)(crc >> 8); /* buffer[131] is crc high 8 bit */
        ctx->buffer[132] = (uint8_t)crc; /* buffer[132] is crc low 8 bit */
        serial_put_buf((const char *)ctx->buffer, SOH_MSG_TOTAL_LEN);
    } else {
        if (remain > UPLOAD_DATA_SIZE) {
            remain = UPLOAD_DATA_SIZE;
            ctx->status = UPLOAD_WAIT_INTER_ACK;
        }

        if (memcpy_s(ctx->buffer + MSG_START_LEN, UPLOAD_DATA_SIZE,
            (const void *)(uintptr_t)(ctx->file_addr + ctx->offset), remain) != EOK) {
            return;
        }

        crc = crc16_ccitt(0, ctx->buffer + MSG_START_LEN, UPLOAD_DATA_SIZE);
        ctx->buffer[1027] = (uint8_t)(crc >> 8); /* buffer[1027] is crc high 8 bit */
        ctx->buffer[1028] = (uint8_t)crc; /* buffer[1028] is crc low 8 bit */
        serial_put_buf((const char *)ctx->buffer, UPLOAD_BUFF_LEN);
    }
    ctx->offset += remain;
    boot_wdt_kick();
}

static void upload_send_null_info(void)
{
    upload_context *ctx = g_upload_ctx;
    uint16_t crc;

    (void)memset_s(ctx->buffer, sizeof(ctx->buffer), 0, SOH_MSG_LEN);
    ctx->buffer[0] = MODEM_SOH;
    ctx->buffer[1] = 0;
    ctx->buffer[2] = 0xFF;  /* buffer[2] is buffer[1] invert val */

    crc = crc16_ccitt(0, ctx->buffer + MSG_START_LEN, SOH_MSG_LEN);
    ctx->buffer[131] = (uint8_t)(crc >> 8); /* buffer[131] is crc high 8 bit */
    ctx->buffer[132] = (uint8_t)crc; /* buffer[132] is crc low 8 bit */
    serial_put_buf((const char *)ctx->buffer, SOH_MSG_TOTAL_LEN);

    ctx->status = UPLOAD_WAIT_ZERO_ACK;
}

static int32_t upload_modem_nak_step(uint8_t *status, uint32_t *timeout)
{
    switch (*status) {
        case UPLOAD_WAIT_INIT_ACK:
            *timeout = UPLOAD_WAIT_START_C_TIME;
            upload_send_file_info();
            return 1;
        case UPLOAD_WAIT_INTER_ACK:
        case UPLOAD_WAIT_FINAL_ACK:
            upload_send_data();
            return 1;
        case UPLOAD_WAIT_EOT_C:
            serial_putc(MODEM_EOT);
            *status = UPLOAD_WAIT_EOT_C;
            return 1;
        case UPLOAD_WAIT_ZERO_ACK:
            upload_send_null_info();
            return 1;
        default:
            return 0;
    }
}

static uint32_t upload_modem_c_step(uint8_t status)
{
    switch (status) {
        case UPLOAD_WAIT_START_C:
            upload_send_file_info();
            return 1;
        case UPLOAD_WAIT_TRANS_C:
            upload_send_data();
            return 1;
        case UPLOAD_WAIT_EOT_C:
            upload_send_null_info();
            return 1;
        default:
            return 0;
    }
}

static int32_t upload_modem_ack_step(uint8_t *status, uint32_t *timeout)
{
    switch (*status) {
        case UPLOAD_WAIT_INIT_ACK:
            *timeout = UPLOAD_WAIT_DEFAULT_TIME;
            *status = UPLOAD_WAIT_TRANS_C;
            return 1;
        case UPLOAD_WAIT_INTER_ACK:
            upload_send_data();
            return 1;
        case UPLOAD_WAIT_FINAL_ACK:
            serial_putc(MODEM_EOT);
            *status = UPLOAD_WAIT_EOT_C;
            return 1;
        default:
            return 0;
    }
}

static uint32_t upload_serial_ymodem(void)
{
    upload_context *ctx = g_upload_ctx;
    uint8_t ch;
    uint32_t timeout = UPLOAD_WAIT_START_C_TIME;

    for (;;) {
        uint32_t ret = serial_getc_timeout(timeout, &ch);
        if (ret != ERRCODE_SUCC) {
            continue;
        }

        if (ch == MODEM_C) {
            ctx->can_cnt = 0;
            ctx->retry = 0;
            if (upload_modem_c_step(ctx->status) == 1) {
                continue;
            }
        } else if (ch == MODEM_ACK) {
            ctx->can_cnt = 0;
            ctx->retry = 0;
            if (ctx->status == UPLOAD_WAIT_ZERO_ACK) {
                mdelay(WAIT_ZERO_ACK_DELAY);
                return ERRCODE_SUCC;
            }
            if (upload_modem_ack_step(&ctx->status, (uint32_t *)&timeout) == 1) {
                continue;
            }
        } else if (ch == MODEM_NAK) {
            ctx->can_cnt = 0;
            if (++ctx->retry == RETRY_COUNT) {
                return ERRCODE_FAIL;
            }
            if (upload_modem_nak_step(&ctx->status, &timeout) == 1) {
                continue;
            }
        } else if (ch == MODEM_CAN) {
            ctx->retry = 0;
            if (++ctx->can_cnt == CAN_COUNT) {
                return ERRCODE_FAIL;
            }
            continue;
        }
    }
}

uint32_t upload_data(uint32_t addr, uint32_t length)
{
    if (upload_malloc_init() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    g_upload_ctx->file_addr = addr + FLASH_ADDR_OFFSET;
    g_upload_ctx->file_length = length;
    g_upload_ctx->file_name = UPLOAD_FILE_NAME;
    g_upload_ctx->status = UPLOAD_WAIT_START_C;

    loader_ack(ACK_SUCCESS);

    if (upload_serial_ymodem() == ERRCODE_SUCC) {
        return ERRCODE_SUCC;
    }
    upload_malloc_deinit();
    return ERRCODE_FAIL;
}
#endif