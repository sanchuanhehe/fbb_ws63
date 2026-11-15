/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: struction of AT command and public function
 */

#ifndef AT_WIFI_TEST_TALBE_H
#define AT_WIFI_TEST_TALBE_H

#include "at.h"
#include "td_base.h"

#ifdef CONFIG_SUPPORT_MBEDTLS_SAMPLE
typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /* ip */
    uint8_t                 *para2; /* 端口 */
    int32_t                 para3; /* 证书0-3 */
    int32_t                 para4; /* 加密曲线0-12 */
    int32_t                 para5; /* 加密套件 */
} mbe_args_t;

at_ret_t cmd_mbedtls(const mbe_args_t *args);

const at_para_parse_syntax_t mbe_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 17,
        .offset = offsetof(mbe_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 6,
        .offset = offsetof(mbe_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(mbe_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 13,
        .offset = offsetof(mbe_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(mbe_args_t, para5)
    },
};
#endif

const at_cmd_entry_t at_wifi_test_table[] = {
#ifdef CONFIG_SUPPORT_MBEDTLS_SAMPLE
    {
        "MBEDTLS",
        8,
        0,
        mbe_syntax,
        NULL,
        (at_set_func_t)cmd_mbedtls,
        NULL,
        NULL,
    },
#endif
};
td_void los_at_wifi_test_cmd_register(td_void);

#endif  /* AT_WIFI_TEST_TALBE_H */