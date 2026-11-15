/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt header file.
 * Create: 2025-12-03
*/

#ifndef SHA1_ALT_H
#define SHA1_ALT_H

#include "mbedtls_harden_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha1_context {
    mbedtls_alt_hash_clone_ctx clone_ctx;
    unsigned int result_size;
} mbedtls_sha1_context;


#ifdef __cplusplus
}
#endif

#endif /* sha1_alt.h */
