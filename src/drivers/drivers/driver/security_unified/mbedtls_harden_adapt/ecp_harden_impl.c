/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls ecp harden implement.
 *
 * Create: 2024-3-13
*/

#include "ecp_harden_impl.h"
#include "crypto_drv_common.h"

#include "kapi_pke.h"
#include "kapi_pke_cal.h"

int mbedtls_alt_ecp_mul_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r)
{
    return kapi_pke_mul_dot(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_data *)k,
        (const drv_pke_ecc_point *)p,
        (const drv_pke_ecc_point *)r);
}

int mbedtls_alt_ecdsa_sign_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *priv_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data)
{
    drv_pke_ecc_sig sig = { 0 };
    crypto_chk_return(r_data == NULL, ERROR_PARAM_IS_NULL, "r_data is NULL\n");
    crypto_chk_return(s_data == NULL, ERROR_PARAM_IS_NULL, "s_data is NULL\n");

    sig.r = r_data->data;
    sig.s = s_data->data;
    sig.length = r_data->length;
    return kapi_pke_ecdsa_sign(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_data *)priv_key,
        (const drv_pke_data *)hash_data, &sig);
}

int mbedtls_alt_ecdsa_verify_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_point *pub_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data)
{
    drv_pke_ecc_sig sig = { 0 };
    crypto_chk_return(r_data == NULL, ERROR_PARAM_IS_NULL, "r_data is NULL\n");
    crypto_chk_return(s_data == NULL, ERROR_PARAM_IS_NULL, "s_data is NULL\n");

    sig.r = r_data->data;
    sig.s = s_data->data;
    sig.length = r_data->length;
    return kapi_pke_ecdsa_verify(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash_data, &sig);
}

int mbedtls_alt_ecdh_compute_shared_impl(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_point *input_pub_key,
    const mbedtls_alt_ecp_data *input_priv_key, const mbedtls_alt_ecp_data *output_shared_key)
{
    return kapi_pke_ecc_gen_ecdh_key(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)input_pub_key,
        (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_shared_key);
}

int mbedtls_alt_ecdsa_genkey_impl(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_data *output_priv_key, const mbedtls_alt_ecp_point *output_pub_key)
{
    return kapi_pke_ecc_gen_key(
        (drv_pke_ecc_curve_type)curve_type,
        NULL,
        (const drv_pke_data *)output_priv_key,
        (const drv_pke_ecc_point *)output_pub_key);
};

void __attribute__((__weak__)) mbedtls_alt_ecp_register(const mbedtls_alt_ecp_harden_func *ecp_func)
{
    crypto_unused(ecp_func);
    return;
}

int mbedtls_alt_ecp_init(void)
{
    mbedtls_alt_ecp_harden_func func_list = {
        .ecp_mul = mbedtls_alt_ecp_mul_impl,
        .ecdsa_sign = mbedtls_alt_ecdsa_sign_impl,
        .ecdsa_verify = mbedtls_alt_ecdsa_verify_impl,
        .ecdh_compute_shared = mbedtls_alt_ecdh_compute_shared_impl,
        .ecdsa_genkey = mbedtls_alt_ecdsa_genkey_impl
    };
    mbedtls_alt_ecp_register(&func_list);
    return 0;
}

int mbedtls_alt_ecp_deinit(void)
{
    mbedtls_alt_ecp_harden_func func_list = { 0 };
    mbedtls_alt_ecp_register(&func_list);
    return 0;
}