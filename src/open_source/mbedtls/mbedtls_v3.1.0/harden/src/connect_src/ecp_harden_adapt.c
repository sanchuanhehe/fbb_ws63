/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ecp harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls_harden_adapt_api.h"
#include "dfx.h"
#include "securec.h"

static mbedtls_alt_ecp_harden_func g_ecp_func;

int mbedtls_alt_ecp_mul(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_ecp_func.ecp_mul == NULL) {
        mbedtls_printf("Error: ecp_mul unregister!\n");
        return -1;
    }
    ret = g_ecp_func.ecp_mul(curve_type, k, p, r);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_ecdsa_verify(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_point *pub_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_ecp_func.ecdsa_verify == NULL) {
        mbedtls_printf("Error: ecdsa_verify unregister!\n");
        return -1;
    }
    ret = g_ecp_func.ecdsa_verify(curve_type, pub_key, hash_data, r_data, s_data);
    mbedtls_harden_log_func_exit();
    return ret;
}

void mbedtls_alt_ecp_register(const mbedtls_alt_ecp_harden_func *ecp_func)
{
    if (ecp_func == NULL) {
        return;
    }
    (void)memcpy_s(&g_ecp_func, sizeof(mbedtls_alt_ecp_harden_func), ecp_func, sizeof(mbedtls_alt_ecp_harden_func));
}