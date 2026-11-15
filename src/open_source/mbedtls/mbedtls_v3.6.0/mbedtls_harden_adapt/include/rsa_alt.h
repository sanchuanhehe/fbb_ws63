/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 * Description: mbedtls harden adapt header file.
 * Create: 2025-12-03
*/

#ifndef RSA_ALT_H
#define RSA_ALT_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"

#include "mbedtls/bignum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   The RSA context structure.the mutex will be implicated in the harden driver, which means kapi for PKE.
 */
typedef struct mbedtls_rsa_context {
    size_t len;                 /*!<  The size of \p N in Bytes. */

    mbedtls_mpi N;              /*!<  The public modulus. */
    mbedtls_mpi E;              /*!<  The public exponent. */

    mbedtls_mpi D;              /*!<  The private exponent. */
    mbedtls_mpi P;              /*!<  The first prime factor. */
    mbedtls_mpi Q;              /*!<  The second prime factor. */

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi DP;             /*!<  <code>D % (P - 1)</code>. */
    mbedtls_mpi DQ;             /*!<  <code>D % (Q - 1)</code>. */
    mbedtls_mpi QP;             /*!<  <code>1 / (Q % P)</code>. */
#endif

    int padding;                /*!< Selects padding mode:
                                     #MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                     #MBEDTLS_RSA_PKCS_V21 for OAEP or PSS. */
    int hash_id;                /*!< Hash identifier of mbedtls_md_type_t type,
                                     as specified in md.h for use in the MGF
                                     mask generating function used in the
                                     EME-OAEP and EMSA-PSS encodings. */
} mbedtls_rsa_context;

/**
 * \brief          The RSA checkup routine.
 *
 * \return         \c 0 on success.
 * \return         \c 1 on failure.
 */
int mbedtls_rsa_v21_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* rsa_alt.h */
