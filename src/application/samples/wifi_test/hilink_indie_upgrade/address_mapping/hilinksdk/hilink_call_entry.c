/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink call entry. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "hilink_call_entry.h"
#include "hilink_function_mapping.h"
#include "func_call.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#endif

#define BITS_PER_BYTES  8
#define CRC32_TBL_SIZE  256

unsigned int g_crc32_tbl[CRC32_TBL_SIZE] = { 0 };

static api_get g_app_api_get = NULL;

void *get_app_api_addr(unsigned int index, const char *prototype)
{
    return g_app_api_get == NULL ? NULL : g_app_api_get(index, prototype);
}

static void init_crc32_table(void)
{
    for (unsigned int i = 0; i < CRC32_TBL_SIZE; i++) {
        unsigned int tmp = i;
        for (unsigned char bit = 0; bit < BITS_PER_BYTES; bit++) {
            if ((tmp & 1) != 0) {
                /* 0xEDB88320 为 CRC32 的生成多项式的反转 */
                tmp = (tmp >> 1) ^ 0xEDB88320;
            } else {
                tmp = tmp >> 1;
            }
        }
        g_crc32_tbl[i] = tmp;
    }
}

static unsigned int prototype_cal_crc32(const char *input)
{
    unsigned int checksum = 0xFFFFFFFF;
    for (unsigned int i = 0; input[i] != 0; i++) {
        if (input[i] == ' ') {
            continue;
        }
        checksum = (checksum >> BITS_PER_BYTES) ^ (g_crc32_tbl[(checksum ^ input[i]) & 0xFF]);
    }
    return ~checksum;
}

/* copy ram */
static void copy_bin_to_ram(unsigned int *start_addr,
    const unsigned int *const load_addr, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size / sizeof(unsigned int); i++) {
        *(start_addr + i) = *(load_addr + i);
    }
}

/* init ram value */
static void init_mem_value(unsigned int *start_addr,
    const unsigned int *const end_addr, unsigned int init_val)
{
    unsigned int *dest = start_addr;

    while (dest < end_addr) {
        *dest = init_val;
        dest++;
    }
}

static void *hilink_api_get(unsigned int index, const char *prototype)
{
    const struct mapping_tbl *hilink_call_tbl = get_hilink_mapping_tbl();
    unsigned int size = get_hilink_mapping_tbl_size();
    if (hilink_call_tbl == NULL || size == 0) {
        return NULL;
    }

    unsigned int checksum = prototype_cal_crc32(prototype);
    if ((index < size) && (hilink_call_tbl[index].checksum == checksum)) {
        return hilink_call_tbl[index].addr;
    }
    /* 对应校验和不匹配，尝试全部匹配 */
    for (unsigned int i = 0; i < size; i++) {
        if (hilink_call_tbl[i].checksum == checksum) {
            return hilink_call_tbl[i].addr;
        }
    }
    return NULL;
}

static void hilink_info_entry(api_get *hilink_get, api_get app_get)
{
    if (hilink_get == NULL || app_get == NULL) {
        return;
    }
#ifndef CHIP_EDA
    /* copy sram_text from flash to SRAM */
    copy_bin_to_ram(&__sram_text_begin__, &__sram_text_load__, (unsigned int)&__sram_text_size__);
    /* copy data from flash to SRAM */
    copy_bin_to_ram(&__data_begin__, &__data_load__, (unsigned int)&__data_size__);
    /* clear bss on SRAM */
    init_mem_value(&__bss_begin__, &__bss_end__, 0);
#endif

    *hilink_get = hilink_api_get;
    g_app_api_get = app_get;
    init_crc32_table();
}

__attribute__ ((used, section(".hilink_info"))) struct hilink_info_stru hilink_info = {
    hilink_info_entry,
};
