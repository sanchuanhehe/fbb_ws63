/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mbedtls harden adapt header file.
*/

#ifndef RSA_ALT_H
#define RSA_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_rsa_context {
    size_t len;
    mbedtls_mpi N;
    mbedtls_mpi E;
    mbedtls_mpi D;
    mbedtls_mpi P;
    mbedtls_mpi Q;

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi DP;
    mbedtls_mpi DQ;
    mbedtls_mpi QP;
#endif
    int padding;
    int hash_id;
} mbedtls_rsa_context;

int mbedtls_rsa_v21_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* rsa_alt.h */
