 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: app call entry. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call_entry.h"
#include <stdio.h>
#include "app_function_mapping.h"
#include "func_call.h"

#define BITS_PER_BYTES  8
#define CRC32_TBL_SIZE  256

unsigned int g_crc32_tbl[CRC32_TBL_SIZE] = { 0 };

static api_get g_hilink_api_get = NULL;

static struct hilink_info_stru *g_hilink_info = (struct hilink_info_stru *)&hilink_info_addr;

void *get_hilink_api_addr(unsigned int index, const char *prototype)
{
    return g_hilink_api_get == NULL ? NULL : g_hilink_api_get(index, prototype);
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

static void *app_api_get(unsigned int index, const char *prototype)
{
    const struct mapping_tbl *app_call_tbl = get_app_mapping_tbl();
    unsigned int size = get_app_mapping_tbl_size();
    if (app_call_tbl == NULL || size == 0) {
        return NULL;
    }

    unsigned int checksum = prototype_cal_crc32(prototype);
    if ((index < size) && (app_call_tbl[index].checksum == checksum)) {
        return app_call_tbl[index].addr;
    }
    /* 对应校验和不匹配，尝试全部匹配 */
    for (unsigned int i = 0; i < size; i++) {
        if (app_call_tbl[i].checksum == checksum) {
            return app_call_tbl[i].addr;
        }
    }
    return NULL;
}

void hilink_func_map_init(void)
{
    printf("%s %d, 0x%x\r\n", __FUNCTION__, __LINE__, g_hilink_info);

    init_crc32_table();
    if (g_hilink_info->entry != NULL) {
        printf("%s %d, 0x%x\r\n", __FUNCTION__, __LINE__, g_hilink_info->entry);
        g_hilink_info->entry(&g_hilink_api_get, app_api_get);
    }
}