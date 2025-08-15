/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of cJSON in sdk side. \n
 *
 * History: \n
 * 2024-11-26, Create file. \n
 */

#include "app_call.h"
#include "mbedtls/md.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/bignum.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ecp.h"

const mbedtls_md_info_t *mbedtls_md_info_from_type(mbedtls_md_type_t md_type)
{
    app_call1(APP_CALL_MBEDTLS_MD_INFO_FROM_TYPE, mbedtls_md_info_from_type, const mbedtls_md_info_t *,
        mbedtls_md_type_t, md_type);
    return NULL;
}

void mbedtls_ctr_drbg_init(mbedtls_ctr_drbg_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_CTR_DRBG_INIT, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_context *, ctx);
}

void mbedtls_mpi_init(mbedtls_mpi *X)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_MPI_INIT, mbedtls_mpi_init, mbedtls_mpi *, X);
}

void mbedtls_ecdh_init(mbedtls_ecdh_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_ECDH_INIT, mbedtls_ecdh_init, mbedtls_ecdh_context *, ctx);
}

int mbedtls_ctr_drbg_random(void *p_rng, unsigned char *output, size_t output_len)
{
    app_call3(APP_CALL_MBEDTLS_CTR_DRBG_RANDOM, mbedtls_ctr_drbg_random, int,
        void *, p_rng, unsigned char *, output, size_t, output_len);
    return 0;
}

int mbedtls_mpi_read_binary(mbedtls_mpi *X, const unsigned char *buf, size_t buflen)
{
    app_call3(APP_CALL_MBEDTLS_MPI_READ_BINARY, mbedtls_mpi_read_binary, int,
        mbedtls_mpi *, X, const unsigned char *, buf, size_t, buflen);
    return 0;
}

int mbedtls_mpi_sub_mpi(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B)
{
    app_call3(APP_CALL_MBEDTLS_MPI_SUB_MPI, mbedtls_mpi_sub_mpi, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, A, const mbedtls_mpi *, B);
    return 0;
}

int mbedtls_hkdf(const mbedtls_md_info_t *md, const unsigned char *salt, size_t salt_len, const unsigned char *ikm,
    size_t ikm_len, const unsigned char *info, size_t info_len, unsigned char *okm, size_t okm_len)
{
    app_call9(APP_CALL_MBEDTLS_HKDF, mbedtls_hkdf, int,
        const mbedtls_md_info_t *, md, const unsigned char *, salt,
        size_t, salt_len, const unsigned char *, ikm, size_t, ikm_len,
        const unsigned char *, info, size_t, info_len,
        unsigned char *, okm, size_t, okm_len);
    return 0;
}

unsigned char mbedtls_md_get_size(const mbedtls_md_info_t *md_info)
{
    app_call1(APP_CALL_MBEDTLS_MD_GET_SIZE, mbedtls_md_get_size, unsigned char,
        const mbedtls_md_info_t *, md_info);
    return 0;
}

void mbedtls_entropy_init(mbedtls_entropy_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_ENTROPY_INIT, mbedtls_entropy_init, mbedtls_entropy_context *, ctx);
}

int mbedtls_mpi_cmp_mpi(const mbedtls_mpi *X, const mbedtls_mpi *Y)
{
    app_call2(APP_CALL_MBEDTLS_MPI_CMP_MPI, mbedtls_mpi_cmp_mpi, int,
        const mbedtls_mpi *, X, const mbedtls_mpi *, Y);
    return 0;
}

typedef int (*func_rng_t)(void *, unsigned char *, size_t);
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z, const mbedtls_ecp_point *Q,
    const mbedtls_mpi *d, func_rng_t f_rng, void *p_rng)
{
    app_call6(APP_CALL_MBEDTLS_ECDH_COMPUTE_SHARED, mbedtls_ecdh_compute_shared, int,
        mbedtls_ecp_group *, grp, mbedtls_mpi *, z,
        const mbedtls_ecp_point *, Q, const mbedtls_mpi *, d,
        func_rng_t, f_rng,
        void *, p_rng);
    return 0;
}

int mbedtls_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E,
    const mbedtls_mpi *N, mbedtls_mpi *prec_RR)
{
    app_call5(APP_CALL_MBEDTLS_MPI_EXP_MOD, mbedtls_mpi_exp_mod, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, A,
        const mbedtls_mpi *, E, const mbedtls_mpi *, N,
        mbedtls_mpi *, prec_RR);
    return 0;
}

int mbedtls_mpi_mod_mpi(mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B)
{
    app_call3(APP_CALL_MBEDTLS_MPI_MOD_MPI, mbedtls_mpi_mod_mpi, int,
        mbedtls_mpi *, R, const mbedtls_mpi *, A, const mbedtls_mpi *, B);
    return 0;
}

int mbedtls_sha256(const unsigned char *input, size_t ilen, unsigned char *output, int is224)
{
    app_call4(APP_CALL_MBEDTLS_SHA256, mbedtls_sha256, int,
        const unsigned char *, input, size_t, ilen, unsigned char *, output, int, is224);
    return 0;
}

void mbedtls_mpi_free(mbedtls_mpi *X)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_MPI_FREE, mbedtls_mpi_free, mbedtls_mpi *, X);
}

int mbedtls_mpi_write_binary(const mbedtls_mpi *X, unsigned char *buf, size_t buflen)
{
    app_call3(APP_CALL_MBEDTLS_MPI_WRITE_BINARY, mbedtls_sha256, int,
        const mbedtls_mpi *, X, unsigned char *, buf, size_t, buflen);
    return 0;
}

int mbedtls_mpi_mul_mpi(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B)
{
    app_call3(APP_CALL_MBEDTLS_MPI_MUL_MPI, mbedtls_mpi_mul_mpi, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, A, const mbedtls_mpi *, B);
    return 0;
}

int mbedtls_mpi_add_mpi(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B)
{
    app_call3(APP_CALL_MBEDTLS_MPI_ADD_MPI, mbedtls_mpi_add_mpi, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, A, const mbedtls_mpi *, B);
    return 0;
}

int mbedtls_entropy_func(void *data, unsigned char *output, size_t len)
{
    app_call3(APP_CALL_MBEDTLS_ENTROPY_FUNC, mbedtls_entropy_func, int,
        void *, data, unsigned char *, output, size_t, len);
    return 0;
}

void mbedtls_ecdh_free(mbedtls_ecdh_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_ECDH_FREE, mbedtls_ecdh_free, mbedtls_ecdh_context *, ctx);
}

int mbedtls_mpi_inv_mod(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *N)
{
    app_call3(APP_CALL_MBEDTLS_MPI_INV_MOD, mbedtls_mpi_inv_mod, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, A, const mbedtls_mpi *, N);
    return 0;
}

typedef int (*func_entropy_t)(void *, unsigned char *, size_t);
int mbedtls_ctr_drbg_seed(mbedtls_ctr_drbg_context *ctx, func_entropy_t f_entropy,
    void *p_entropy, const unsigned char *custom, size_t len)
{
    app_call5(APP_CALL_MBEDTLS_CTR_DRBG_SEED, mbedtls_ctr_drbg_seed, int,
        mbedtls_ctr_drbg_context *, ctx,
        func_entropy_t, f_entropy,
        void *, p_entropy, const unsigned char *, custom, size_t, len);
    return 0;
}

void mbedtls_ctr_drbg_free(mbedtls_ctr_drbg_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_CTR_DRBG_FREE, mbedtls_ctr_drbg_free, mbedtls_ctr_drbg_context *, ctx);
}

int mbedtls_mpi_copy(mbedtls_mpi *X, const mbedtls_mpi *Y)
{
    app_call2(APP_CALL_MBEDTLS_MPI_COPY, mbedtls_mpi_copy, int,
        mbedtls_mpi *, X, const mbedtls_mpi *, Y);
    return 0;
}

void mbedtls_entropy_free(mbedtls_entropy_context *ctx)
{
    app_call1_ret_void(APP_CALL_MBEDTLS_ENTROPY_FREE, mbedtls_entropy_free, mbedtls_entropy_context *, ctx);
}

int mbedtls_ecp_group_load(mbedtls_ecp_group *grp, mbedtls_ecp_group_id id)
{
    app_call2(APP_CALL_MBEDTLS_ECP_GROUP_LOAD, mbedtls_ecp_group_load, int,
        mbedtls_ecp_group *, grp, mbedtls_ecp_group_id, id);
    return 0;
}

int mbedtls_mpi_safe_cond_swap(mbedtls_mpi *X, mbedtls_mpi *Y, unsigned char assign)
{
    app_call3(APP_CALL_MBEDTLS_MPI_SAFE_COND_SWAP, mbedtls_mpi_safe_cond_swap, int,
        mbedtls_mpi *, X, mbedtls_mpi *, Y, unsigned char, assign);
    return 0;
}

int mbedtls_mpi_lset(mbedtls_mpi *X, mbedtls_mpi_sint z)
{
    app_call2(APP_CALL_MBEDTLS_MPI_LSET, mbedtls_mpi_lset, int,
        mbedtls_mpi *, X, mbedtls_mpi_sint, z);
    return 0;
}
