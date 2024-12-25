/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt sha256 header.
 *
 * Create: 2024-07-09
*/

#ifndef SHA512_ALT_H
#define SHA512_ALT_H

#include "mbedtls_harden_struct.h"

typedef struct mbedtls_sha512_context {
    mbedtls_alt_hash_clone_ctx clone_ctx;
    unsigned int result_size;
} mbedtls_sha512_context;

#endif /* sha512_alt.h */
