/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt sha256 header.
 *
 * Create: 2024-07-09
*/

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#include "mbedtls_harden_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha256_context {
    mbedtls_alt_hash_clone_ctx clone_ctx;
    unsigned int result_size;
} mbedtls_sha256_context;

#ifdef __cplusplus
}
#endif

#endif /* sha256_alt.h */
