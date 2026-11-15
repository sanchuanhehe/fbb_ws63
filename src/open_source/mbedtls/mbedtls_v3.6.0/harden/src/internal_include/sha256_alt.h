/* sha256_alt.h with dummy types for MBEDTLS_SHA256_ALT */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#if defined(MBEDTLS_SHA256_ALT)
#include "mbedtls_harden_struct.h"
#endif /* MBEDTLS_SHA256_ALT */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha256_context
{
#if defined(MBEDTLS_SHA256_ALT)
    mbedtls_alt_hash_clone_ctx clone_ctx;
#elif defined(MBEDTLS_DIAL_SHA256_ALT)
    uint32_t hash_handle;
#endif /* MBEDTLS_DIAL_SHA256_ALT */
    unsigned int result_size;
}
mbedtls_sha256_context;

#ifdef __cplusplus
}
#endif

#endif /* sha256_alt.h */
