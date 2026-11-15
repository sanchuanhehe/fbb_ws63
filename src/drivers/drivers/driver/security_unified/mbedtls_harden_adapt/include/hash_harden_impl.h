/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt header file.
 * Create: 2025-12-03
*/

#ifndef HASH_HARDEN_IMPL_H
#define HASH_HARDEN_IMPL_H

#include "mbedtls_harden_adapt_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int mbedtls_alt_hash_start_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type);

int mbedtls_alt_hash_update_impl(mbedtls_alt_hash_clone_ctx *clone_ctx,
    const unsigned char *data, unsigned int data_len);

int mbedtls_alt_hash_finish_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len);

int mbedtls_alt_hash_init(void);

int mbedtls_alt_hash_deinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif