/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt aes header.
 *
 * Create: 2024-07-10
*/

#ifndef AES_ALT_H
#define AES_ALT_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>

typedef struct mbedtls_aes_context {
    uint8_t key[32];
    uint32_t key_len;
} mbedtls_aes_context;

#endif /* aes_alt.h */
