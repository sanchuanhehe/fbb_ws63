/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt header file.
 * Create: 2025-12-03
*/

#ifndef CRYPTO_CIPHER_COMMON_STRUCT_H
#define CRYPTO_CIPHER_COMMON_STRUCT_H

#include "mbedtls_crypto_type.h"
#include "mbedtls_platform_hardware_config.h"
#include "crypto_common_struct.h"
#include "crypto_symc_struct.h"
#include "crypto_hash_struct.h"

#define CRYPTO_IV_LEN_IN_BYTES      16
#define CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD 16      // for SHA-512
#define CRYPTO_HASH_BLOCK_SIZE_MAX 128              // for SHA-512

#endif /* CRYPTO_CIPHER_COMMON_STRUCT_H */