/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: boot Cmd
 *
 * Create: 2024-01-15
 */
#if defined(CONFIG_LOADERBOOT_SUPPORT_SW_HASH)
#include "sha256/sha256.h"
#else
#include "drv_rom_cipher.h"
#endif
#include "efuse_porting.h"
#include "efuse.h"
#include "boot_serial.h"
#include "string.h"
#include "soc_errno.h"
#include "boot_cmd_loop.h"
#include "boot_load.h"

#define SHA_256_LENGTH          32
#define THREE_BITS_OFFSET       3

typedef struct {
    uint8_t hash[SHA_256_LENGTH];     /* hash of configuration file. */
    uint8_t stru_ver;     /* default 0. */
    uint8_t stru_size;    /* sizeof(struct otp_config_header). */
    uint16_t number;      /* Item number to be burn. */
    uint32_t file_size;   /* Configuration file size. */
    uint32_t reserved[2]; /* Reserved 2 u32. */
    uint8_t data[0];      /* Item: size = file_size - stru_size. */
} efuse_config_header;

typedef struct {
    uint8_t stru_ver;     /* default 0. */
    uint8_t stru_size;    /* sizeof(struct otp_config_item) */
    uint16_t start_bit;   /* Start bit of OTP */
    uint16_t bit_width;   /* Bit width */
    uint16_t value_len;   /* Length of value Byte(s), 4-byte-aligned. */
    uint8_t value[0];     /* Item, offset: stru_size. */
} efuse_config_item;

static uint32_t efuse_cfg_verify(uintptr_t file_addr, uint32_t file_len)
{
    uint8_t hash[SHA_256_LENGTH] = {0};
    efuse_config_header *header = (efuse_config_header *)file_addr;
#if defined(CONFIG_LOADERBOOT_SUPPORT_SW_HASH)
    sha256_context_t md;
    (void)sha256_init(&md);
    (void)SHA256Update(&md, (unsigned char *)((uintptr_t)&(header->stru_ver)), (file_len - SHA_256_LENGTH));
    (void)sha256_final(&md, (unsigned char *)hash, SHA256_HASH_SIZE);
#else
    uint32_t ret = (uint32_t)drv_rom_cipher_init();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("drv_rom_cipher_init !!! ret = ", ret);
        return ERRCODE_FAIL;
    }

    ret = (uint32_t)drv_rom_cipher_sha256(&(header->stru_ver), file_len - SHA_256_LENGTH, hash, SHA_256_LENGTH);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("drv_rom_cipher_sha256 !!! ret = ", ret);
        return ERRCODE_FAIL;
    }
    ret = (uint32_t)drv_rom_cipher_deinit();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("drv_rom_cipher_deinit !!! ret = ", ret);
        return ERRCODE_FAIL;
    }
#endif

    if (memcmp(header->hash, hash, SHA_256_LENGTH) != EOK) {
        boot_msg0("hash commpare fail!!!");
        return ERRCODE_FAIL;
    }

    if (header->number > (EFUSE_IDX_MAX)) {
        boot_msg0("exceed efuse index!!!");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static uint32_t efuse_write_bits(uint16_t start_bit, uint16_t size, const uint8_t *key_data)
{
    uint16_t i;
    uint16_t j;
    uint16_t a;
    uint16_t b;
    uint32_t ret;
    uint16_t zero_count = 0;

    /* j: the current bit offset of key_data. */
    for (i = start_bit, j = 0; i < (start_bit + size); i++, j++) {
        a = j >> THREE_BITS_OFFSET; /* subscript of receive array. */
        b = j & 0x7;                /* bit offset in one byte. */
        if ((key_data[a] & (uint8_t)(1 << b)) != 0) {
            ret = uapi_efuse_write_bit((i >> THREE_BITS_OFFSET), (i & 0x7));
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        } else {
            zero_count++;
        }
    }
    return ERRCODE_SUCC;
}

static uint32_t efuse_burn(uintptr_t file_addr, uint32_t file_len)
{
    uint32_t ret = efuse_cfg_verify(file_addr, file_len);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("Efuse config file invalid");
        return ERRCODE_FAIL;
    }
    efuse_config_header *header = (efuse_config_header *)file_addr;
    efuse_config_item *item = (efuse_config_item *)(file_addr + header->stru_size);
    for (uint8_t i = 0; i < header->number; i++) {
        if (item == NULL) {
            return ERRCODE_FAIL;
        }
        ret = efuse_write_bits(item->start_bit, item->bit_width, item->value);
        if (ret != ERRCODE_SUCC) {
            boot_msg1("efuse write error, index:", i);
            serial_puts("Start bit: ");
            serial_puthex(item->start_bit, 1);
            serial_puts(" len(bits)=");
            serial_puthex(item->bit_width, 1);
            serial_puts("\n");
        }

        item = (efuse_config_item *)((uintptr_t)item + item->stru_size + item->value_len);
    }
    return ERRCODE_SUCC;
}

static uint32_t loady_file(uintptr_t ram_addr)
{
    uint32_t ret;

    loader_ack(ACK_SUCCESS);
    ret = loader_serial_ymodem(ram_addr, 0, sizeof(efuse_config_header), EFUSE_CFG_MAX_LEN);
    return ret;
}

uint32_t loader_burn_efuse(const uart_ctx *cmd_ctx)
{
    uint32_t ret;
    uint32_t file_len = *(uint32_t *)(&cmd_ctx->packet.payload[0]);
    if (file_len <= EFUSE_CFG_MIN_LEN || file_len > EFUSE_CFG_MAX_LEN) {
        boot_msg1("File length error : ", file_len);
        return ERRCODE_FAIL;
    }
    ret = loady_file((uintptr_t)BURN_EFUSE_BIN_ADDR);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("Loady efuse file failed:", ret);
        return ret;
    }
    ret = efuse_burn((uintptr_t)BURN_EFUSE_BIN_ADDR, file_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

uint32_t loader_read_efuse(const uart_ctx *cmd_ctx)
{
    uint32_t ret;
    uint16_t efuse_id = *(uint16_t *)(&cmd_ctx->packet.payload[0]);
    uint8_t data[EFUSE_READ_MAX_BYTE] = { 0 };
    boot_msg0("Efuse read");
    serial_puts("Start bit: ");
    serial_puthex(efuse_id, 1);
    serial_puts("\r\n");
    if (efuse_id >= EFUSE_IDX_MAX) {
        boot_msg0("Params err");
        return ERRCODE_FAIL;
    }
    ret = efuse_read_item(efuse_id, data, EFUSE_READ_MAX_BYTE);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    for (uint8_t i = 0; i < EFUSE_READ_MAX_BYTE; i++) {
        serial_puthex(data[i], 1);
        serial_puts("  ");
    }
    return ERRCODE_SUCC;
}