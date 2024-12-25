/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt bignum header.
 *
 * Create: 2024-07-10
*/

#ifndef MBEDTLS_BIGNUM_HARDEN_H
#define MBEDTLS_BIGNUM_HARDEN_H

#include "mbedtls/bignum.h"

int check_exp_mod_harden_can_do(const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N);

int exp_mod_harden(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N);

int check_mod_harden_can_do(const mbedtls_mpi *A, const mbedtls_mpi *B);

int mod_harden(mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B);

#endif /* MBEDTLS_BIGNUM_HARDEN_H */