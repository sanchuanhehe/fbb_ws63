/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink独立升级入口
 */
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
#include <stdio.h>
#include <stdbool.h>
#include "hilink.h"
#include "app_call_entry.h"
#include "memory_config_common.h"
#include "sfc.h"
#include "securec.h"

#define ONE_BYTE_HEXIFY_LEN     2
#define HEXIFY_LEN(len)         ((len) * ONE_BYTE_HEXIFY_LEN)
#define BIN_HASH_LEN            32
/* 需减去固件包头长度0x300 */
#define APP_BIN_ADDR            (APP_PROGRAM_ORIGIN - FLASH_START - 0x300)
#define CODE_AREA_HASH_OFFSET   0x128

bool hexify(const unsigned char *inBuf, unsigned int inBufLen, char *outBuf, unsigned int outBufLen)
{
    if (outBufLen < HEXIFY_LEN(inBufLen)) {
        return false;
    }

    unsigned char high;
    unsigned char low;
    while (inBufLen > 0) {
        /* 对16取模得到高位 */
        high = *inBuf / 16;
        /* 对16取余得到低位 */
        low = *inBuf % 16;
        /* 数字10表示16进制字符a与其代表10进制数字的值 */
        if (high < 10) {
            *outBuf++ = '0' + high;
        } else {
            /* 数字10表示16进制字符a与其代表10进制数字的值 */
            *outBuf++ = 'A' + high - 10;
        }
        /* 数字10表示16进制字符a与其代表10进制数字的值 */
        if (low < 10) {
            *outBuf++ = '0' + low;
        } else {
            /* 数字10表示16进制字符a与其代表10进制数字的值 */
            *outBuf++ = 'A' + low - 10;
        }

        ++inBuf;
        inBufLen--;
    }

    return true;
}

int read_app_bin_hash(char *hash_str, unsigned int *size)
{
    if (hash_str == NULL || size == NULL || *size <= HEXIFY_LEN(BIN_HASH_LEN)) {
        printf("param err\n");
        return -1;
    }
    unsigned char bin_hash[BIN_HASH_LEN] = { 0 };
    errcode_t err = uapi_sfc_reg_read(APP_BIN_ADDR + CODE_AREA_HASH_OFFSET, bin_hash, BIN_HASH_LEN);
    if (err != ERRCODE_SUCC) {
        printf("read bin hash fail:%u\n", err);
        return -1;
    }
    (void)memset_s(hash_str, HEXIFY_LEN(BIN_HASH_LEN) + 1, 0, HEXIFY_LEN(BIN_HASH_LEN) + 1);
    if (!hexify(bin_hash, BIN_HASH_LEN, hash_str, *size)) {
        printf("hexify bin hash fail\n");
        return -1;
    }
    *size = HEXIFY_LEN(BIN_HASH_LEN);
    return 0;
}

int hilink_indie_upgrade_main(void)
{
    hilink_func_map_init();

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        printf("sdkAttr is null");
        return -1;
    }
    sdkAttr->deviceMainTaskStackSize = 0x4000;
    sdkAttr->otaCheckTaskStackSize = 0x3250;
    sdkAttr->otaUpdateTaskStackSize = 0x3250;
    HILINK_SetSdkAttr(*sdkAttr);

    return 0;
}
#endif