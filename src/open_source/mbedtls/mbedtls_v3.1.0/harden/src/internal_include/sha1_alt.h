/* sha1_alt.h with dummy types for MBEDTLS_SHA1_ALT */
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

#ifndef SHA1_ALT_H
#define SHA1_ALT_H

#include "mbedtls_harden_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha1_context
{
    mbedtls_alt_hash_clone_ctx clone_ctx;
    unsigned int result_size;
}
mbedtls_sha1_context;


#ifdef __cplusplus
}
#endif

#endif /* sha1_alt.h */
