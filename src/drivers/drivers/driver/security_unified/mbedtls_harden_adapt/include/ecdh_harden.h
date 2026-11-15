/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt header file.
 * Create: 2025-12-03
*/

#ifndef MBEDTLS_ECDH_HARDEN_H
#define MBEDTLS_ECDH_HARDEN_H

#include "mbedtls/ecdh.h"

int ecdh_harden(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                int (*f_rng)(void *, unsigned char *, size_t),
                void *p_rng);

#endif /* MBEDTLS_ECDH_HARDEN_H */