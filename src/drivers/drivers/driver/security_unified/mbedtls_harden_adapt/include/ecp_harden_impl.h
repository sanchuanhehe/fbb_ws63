/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls ecp harden api.
 *
 * Create: 2024-3-13
*/

#ifndef ECP_HARDEN_IMPL_H
#define ECP_HARDEN_IMPL_H

#include "mbedtls_harden_adapt_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int mbedtls_alt_ecp_mul_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

int mbedtls_alt_ecdh_compute_shared_impl(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_point *input_pub_key,
    const mbedtls_alt_ecp_data *input_priv_key, const mbedtls_alt_ecp_data *output_shared_key);

int mbedtls_alt_ecdsa_sign_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *priv_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

int mbedtls_alt_ecdsa_verify_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_point *pub_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

int mbedtls_alt_ecdsa_genkey_impl(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_data *output_priv_key, const mbedtls_alt_ecp_point *output_pub_key);

int mbedtls_alt_ecp_init(void);

int mbedtls_alt_ecp_deinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif