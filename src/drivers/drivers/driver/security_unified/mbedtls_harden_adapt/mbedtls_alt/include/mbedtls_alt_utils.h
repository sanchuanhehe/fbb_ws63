/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt utils header.
 *
 * Create: 2024-07-09
*/
#ifndef MBEDTLS_ALT_UTILS_H
#define MBEDTLS_ALT_UTILS_H

#include "mbedtls/ecp.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"

#include "mbedtls_harden_struct.h"

#define mbedtls_harden_log_err(fmt, ...)   \
    mbedtls_printf("[%s:%d]" "HARDEN MBED ERR! : " fmt, __func__, __LINE__, ##__VA_ARGS__)

#if defined(MBED_HARDEN_DEBUG)
#define mbedtls_harden_log_dbg             mbedtls_printf
#else
#define mbedtls_harden_log_dbg(...)
#endif

#define mbedtls_harden_log_func_enter()    mbedtls_harden_log_dbg("%s ===>Enter\n", __func__)
#define mbedtls_harden_log_func_exit()     mbedtls_harden_log_dbg("%s <===Exit\n", __func__)

void mbedtls_alt_utils_get_curve_type(mbedtls_ecp_group_id grp_id,
    mbedtls_alt_ecp_curve_type *curve_type, unsigned int *klen);

int mbedtls_alt_utils_ecp_data_alloc(unsigned int len, mbedtls_alt_ecp_data *data);

int mbedtls_alt_utils_ecp_data_alloc_with_value(unsigned int len, const mbedtls_mpi *d, mbedtls_alt_ecp_data *data);

void mbedtls_alt_utils_ecp_data_free(mbedtls_alt_ecp_data *data);

int mbedtls_alt_utils_ecp_priv_key_alloc_with_value(mbedtls_alt_ecp_curve_type curve_type, unsigned int klen,
    const mbedtls_mpi *d, mbedtls_alt_ecp_data *priv_key);

void mbedtls_alt_utils_ecp_priv_key_free(mbedtls_alt_ecp_data *priv_key);

int mbedtls_alt_utils_ecp_pub_key_alloc(unsigned int klen, mbedtls_alt_ecp_point *pub_key);

int mbedtls_alt_utils_ecp_pub_key_alloc_with_value(mbedtls_alt_ecp_curve_type curve_type, unsigned int klen,
    const mbedtls_ecp_point *Q, mbedtls_alt_ecp_point *pub_key);

void mbedtls_alt_utils_ecp_pub_key_free(mbedtls_alt_ecp_point *pub_key);

void mbedtls_alt_utils_hash_get_info(mbedtls_md_type_t md_type, mbedtls_alt_hash_type *hash_type);

#endif