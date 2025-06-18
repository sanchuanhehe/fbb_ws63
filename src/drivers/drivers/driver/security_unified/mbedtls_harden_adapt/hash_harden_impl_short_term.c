/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls hash harden implement short term.
 *
 * Create: 2023-12-23
*/

#include "hash_harden_impl.h"

#include "kapi_hash.h"
#include "drv_hash.h"

#include "crypto_drv_common.h"

#define INVALID_HANDLE      0xFFFFFFFF
static crypto_mutex g_mbedtls_hash_lock;

int mbedtls_alt_hash_start_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type)
{
    int ret = TD_FAILURE;
    const unsigned char *sha_val = NULL;
    unsigned int sha_val_len = 0;
    crypto_drv_func_enter();

    (void)memset_s(clone_ctx, sizeof(mbedtls_alt_hash_clone_ctx), 0, sizeof(mbedtls_alt_hash_clone_ctx));

    sha_val = drv_hash_get_state_iv((crypto_hash_type)hash_type, &sha_val_len);
    crypto_chk_return(sha_val == TD_NULL, TD_FAILURE, "drv_hash_get_state_iv failed\n");

    ret = memcpy_s(clone_ctx->state, sizeof(clone_ctx->state), sha_val, sha_val_len);
    crypto_chk_return(ret != EOK, ret, "memcpy_s failed\n");

    clone_ctx->hash_type = hash_type;

    crypto_drv_func_exit();

    return 0;
}

int mbedtls_alt_hash_update_impl(mbedtls_alt_hash_clone_ctx *clone_ctx,
    const unsigned char *data, unsigned int data_len)
{
    int ret;
    td_handle hash_handle = INVALID_HANDLE;
    crypto_hash_attr hash_attr;
    crypto_buf_attr src_buf;
    crypto_drv_func_enter();

    src_buf.virt_addr = (unsigned char *)data;

    ret = crypto_mutex_lock(&g_mbedtls_hash_lock);
    crypto_chk_return(ret != 0, ret, "crypto_mutex_lock failed\n");
    crypto_service_preprocess();

    hash_attr.hash_type = clone_ctx->hash_type;
    ret = drv_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_set(hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_update(hash_handle, &src_buf, data_len);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_update failed\n");

    ret = drv_cipher_hash_get(hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_get failed\n");

exit_hash_destroy:
    if (hash_handle != INVALID_HANDLE) {
        (void)drv_cipher_hash_destroy(hash_handle);
    }
exit_mutex_unlock:
    crypto_service_postprocess();
    crypto_mutex_unlock(&g_mbedtls_hash_lock);
    crypto_drv_func_exit();
    return ret;
}

int mbedtls_alt_hash_finish_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len)
{
    int ret;
    td_handle hash_handle = INVALID_HANDLE;
    crypto_hash_attr hash_attr;
    unsigned int hash_out_len = out_len;
    crypto_drv_func_enter();

    ret = crypto_mutex_lock(&g_mbedtls_hash_lock);
    crypto_chk_return(ret != 0, ret, "crypto_mutex_lock failed\n");
    crypto_service_preprocess();

    hash_attr.hash_type = clone_ctx->hash_type;
    ret = drv_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_set(hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_finish(hash_handle, out, &hash_out_len);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_finish failed\n");
    hash_handle = INVALID_HANDLE;

exit_hash_destroy:
    if (hash_handle != INVALID_HANDLE) {
        (void)drv_cipher_hash_destroy(hash_handle);
    }
exit_mutex_unlock:
    crypto_service_postprocess();
    crypto_mutex_unlock(&g_mbedtls_hash_lock);
    crypto_drv_func_exit();
    return 0;
}

int mbedtls_alt_pkcs5_pbkdf2_hmac_impl(mbedtls_alt_hash_type hmac_type, const unsigned char *password,
    unsigned int plen, const unsigned char *salt, unsigned int slen, unsigned int iteration_count,
    unsigned int key_length, unsigned char *output)
{
    crypto_kdf_pbkdf2_param param = {
        .hash_type = hmac_type,
        .password = (unsigned char *)password,
        .plen = plen,
        .salt = (unsigned char *)salt,
        .slen = slen,
        .count = iteration_count,
    };

    return kapi_cipher_pbkdf2(&param, output, key_length);
}

int mbedtls_alt_hash_init(void)
{
    mbedtls_alt_hash_harden_func func = {
        .start = mbedtls_alt_hash_start_impl,
        .update = mbedtls_alt_hash_update_impl,
        .finish = mbedtls_alt_hash_finish_impl,
        .pkcs5_pbkdf2_hmac = mbedtls_alt_pkcs5_pbkdf2_hmac_impl
    };

    mbedtls_alt_hash_register(&func);
    (void)crypto_mutex_init(&g_mbedtls_hash_lock);
    return 0;
}

int mbedtls_alt_hash_deinit(void)
{
    mbedtls_alt_hash_harden_func func = { 0 };

    mbedtls_alt_hash_register(&func);
    (void)crypto_mutex_destroy(&g_mbedtls_hash_lock);
    return 0;
}