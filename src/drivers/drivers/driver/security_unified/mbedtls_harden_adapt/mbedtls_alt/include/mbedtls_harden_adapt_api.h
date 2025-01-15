/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt adapt api header.
 *
 * Create: 2024-07-09
*/

#ifndef MBEDTLS_HARDEN_ADAPT_API_H
#define MBEDTLS_HARDEN_ADAPT_API_H

#include "mbedtls_harden_struct.h"
#if defined(MBEDTLS_RSA_ALT)
#include "rsa_alt.h"
#endif

/* Hash. */
typedef int (*mbedtls_alt_hash_start_func)(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type);
typedef int (*mbedtls_alt_hash_update_func)(mbedtls_alt_hash_clone_ctx *clone_ctx,
    const unsigned char *data, unsigned int data_len);
typedef int (*mbedtls_alt_hash_finish_func)(mbedtls_alt_hash_clone_ctx *clone_ctx,
    unsigned char *out, unsigned int out_len);

typedef int (*mbedtls_alt_hkdf_func)(mbedtls_alt_hash_type hmac_type, const unsigned char *salt, unsigned int salt_len,
    const unsigned char *ikm, unsigned int ikm_len, const unsigned char *info, unsigned int info_len,
    unsigned char *okm, unsigned int okm_len);

typedef int (*mbedtls_alt_hkdf_extract_func)(mbedtls_alt_hash_type hmac_type,
    const unsigned char *salt, unsigned int salt_len, const unsigned char *ikm,
    unsigned int ikm_len, unsigned char *prk);

typedef int (*mbedtls_alt_hkdf_expand_func)(mbedtls_alt_hash_type hmac_type,
    const unsigned char *prk, unsigned int prk_len,
    const unsigned char *info, unsigned int info_len, unsigned char *okm, unsigned int okm_len);

typedef int (*mbedtls_alt_pkcs5_pbkdf2_hmac_func)(mbedtls_alt_hash_type hmac_type,
    const unsigned char *password, unsigned int plen,
    const unsigned char *salt, unsigned int slen, unsigned int iteration_count,
    unsigned int key_length, unsigned char *output);

typedef struct {
    mbedtls_alt_hash_start_func         start;
    mbedtls_alt_hash_update_func        update;
    mbedtls_alt_hash_finish_func        finish;
    mbedtls_alt_hkdf_func               hkdf;
    mbedtls_alt_hkdf_extract_func       hkdf_extract;
    mbedtls_alt_hkdf_expand_func        hkdf_expand;
    mbedtls_alt_pkcs5_pbkdf2_hmac_func  pkcs5_pbkdf2_hmac;
} mbedtls_alt_hash_harden_func;

int mbedtls_alt_hash_start(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type);

int mbedtls_alt_hash_update(mbedtls_alt_hash_clone_ctx *clone_ctx, const unsigned char *data, unsigned int data_len);

int mbedtls_alt_hash_finish(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len);

int mbedtls_alt_hkdf(mbedtls_alt_hash_type hmac_type, const unsigned char *salt, unsigned int salt_len,
    const unsigned char *ikm, unsigned int ikm_len, const unsigned char *info, unsigned int info_len,
    unsigned char *okm, unsigned int okm_len);

int mbedtls_alt_hkdf_extract(mbedtls_alt_hash_type hmac_type, const unsigned char *salt, unsigned int salt_len,
    const unsigned char *ikm, unsigned int ikm_len, unsigned char *prk);

int mbedtls_alt_hkdf_expand(mbedtls_alt_hash_type hmac_type, const unsigned char *prk, unsigned int prk_len,
    const unsigned char *info, unsigned int info_len, unsigned char *okm, unsigned int okm_len);

int mbedtls_alt_pkcs5_pbkdf2_hmac(mbedtls_alt_hash_type hmac_type, const unsigned char *password, unsigned int plen,
    const unsigned char *salt, unsigned int slen, unsigned int iteration_count,
    unsigned int key_length, unsigned char *output);

void mbedtls_alt_hash_register(const mbedtls_alt_hash_harden_func *hash_func);

/* AES. */
typedef int (*mbedtls_alt_aes_crypt_func)(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);
typedef int (*mbedtls_alt_aes_cbc_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_cfb8_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ofb_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ctr_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

typedef struct {
    mbedtls_alt_aes_crypt_func        encrypt;
    mbedtls_alt_aes_crypt_func        decrypt;
    mbedtls_alt_aes_cbc_crypt_func    cbc_encrypt;
    mbedtls_alt_aes_cbc_crypt_func    cbc_decrypt;
    mbedtls_alt_aes_cfb8_crypt_func   cfb8_encrypt;
    mbedtls_alt_aes_cfb8_crypt_func   cfb8_decrypt;
    mbedtls_alt_aes_ofb_crypt_func    ofb_crypt;
    mbedtls_alt_aes_ctr_crypt_func    ctr_crypt;
} mbedtls_alt_aes_harden_func;

int mbedtls_alt_aes_encrypt(unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_decrypt(unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_cbc_encrypt(unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cbc_decrypt(unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_encrypt(unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_decrypt(unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ofb_crypt(unsigned char *key, unsigned key_len,
    unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ctr_crypt(unsigned char *key, unsigned key_len,
    unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

void mbedtls_alt_aes_register(const mbedtls_alt_aes_harden_func *aes_func);

#if defined(MBEDTLS_RSA_ALT)
int mbedtls_alt_rsa_public_encrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output, unsigned int olen);

int mbedtls_alt_rsa_private_decrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output,
    unsigned int olen_max, unsigned int *olen);

int mbedtls_alt_rsa_private_sign(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    unsigned char *sig, unsigned int olen);

int mbedtls_alt_rsa_public_verify(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    const unsigned char *sig, unsigned int olen);
#endif

/* ECP. */
typedef int (*mbedtls_alt_ecp_mul_func)(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

typedef int (*mbedtls_alt_ecdsa_sign_func)(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *priv_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

typedef int (*mbedtls_alt_ecdsa_verify_func)(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_point *pub_key, const mbedtls_alt_ecp_data *hash_data,
    const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

typedef int (*mbedtls_alt_ecdh_compute_shared_func)(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_point *input_pub_key,
    const mbedtls_alt_ecp_data *input_priv_key, const mbedtls_alt_ecp_data *output_shared_key);

typedef int (*mbedtls_alt_ecdsa_genkey_func)(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_data *output_priv_key, const mbedtls_alt_ecp_point *output_pub_key);

typedef struct {
    mbedtls_alt_ecp_mul_func ecp_mul;
    mbedtls_alt_ecdsa_sign_func ecdsa_sign;
    mbedtls_alt_ecdsa_verify_func ecdsa_verify;
    mbedtls_alt_ecdh_compute_shared_func ecdh_compute_shared;
    mbedtls_alt_ecdsa_genkey_func ecdsa_genkey;
} mbedtls_alt_ecp_harden_func;

int mbedtls_alt_ecp_mul(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

int mbedtls_alt_ecdh_compute_shared(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_point *input_pub_key,
    const mbedtls_alt_ecp_data *input_priv_key, const mbedtls_alt_ecp_data *output_shared_key);

int mbedtls_alt_ecdsa_sign(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *priv_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

int mbedtls_alt_ecdsa_verify(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_point *pub_key,
    const mbedtls_alt_ecp_data *hash_data, const mbedtls_alt_ecp_data *r_data, const mbedtls_alt_ecp_data *s_data);

int mbedtls_alt_ecdsa_genkey(mbedtls_alt_ecp_curve_type curve_type,
    const mbedtls_alt_ecp_data *output_priv_key, const mbedtls_alt_ecp_point *output_pub_key);

void mbedtls_alt_ecp_register(const mbedtls_alt_ecp_harden_func *ecp_func);

#endif