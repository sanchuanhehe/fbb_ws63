/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt source file.
 * Create: 2025-12-03
*/

#include "common.h"
#include "mbedtls/error.h"
#include "cipher_common.h"
#include "dfx.h"
#include "cmac_harden.h"

#if defined(MBEDTLS_CMAC_ALT)

static cmac_harden_context g_cmac_harden_context[MAX_CMAC_HARD_CTX_NUM] = {0};

static cmac_harden_context *get_cmac_harden_ctx(mbedtls_cipher_context_t *ctx)
{
    unsigned int i = 0;
    for (i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i) {
        if (g_cmac_harden_context[i].ctx == ctx) {
            return &g_cmac_harden_context[i];
        }
    }
    return NULL;
}

static int internal_cmac_starts_harden(cmac_harden_context *harden_ctx)
{
    int ret = 0;
    crypto_symc_mac_attr mac_attr = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_CMAC,
        .is_long_term = 1
    };

    ret = CIPHER_SYMC_INIT();
    if (ret != 0)
        return ret;
    ret = KM_INIT();
    if (ret != 0)
        goto cipher_deinit;
    ret = CIPHER_CREATE_KEYSLOT(&harden_ctx->keyslot_handle, KM_KEYSLOT_ENGINE_AES);
    if (ret != 0)
        goto km_deinit;
    
    ret = CIPHER_SET_CLEAR_KEY(harden_ctx->keyslot_handle, harden_ctx->key, harden_ctx->key_length,
        KM_KEYSLOT_ENGINE_AES);
    if (ret != 0)
        goto keyslot_destroy;
    mac_attr.keyslot_chn = harden_ctx->keyslot_handle;
    switch (harden_ctx->key_length) {
        case 16:    // 16: 128-bit key
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_128BIT;
            break;
        case 24:    // 24: 192-bit key
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_192BIT;
            break;
        case 32:    // 32: 256-bit key
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_256BIT;
            break;
    }
    ret = CIPHER_SYMC_MAC_START(&harden_ctx->symc_handle, &mac_attr);
    if (ret != 0)
        goto keyslot_destroy;
    return ret;

keyslot_destroy:
    CIPHER_DESTROY_KEYSLOT(harden_ctx->keyslot_handle);
km_deinit:
    (void)KM_DEINIT();
cipher_deinit:
    (void)CIPHER_SYMC_DEINIT();
    return ret;
}

static void internal_check_and_free_harden_ctx(mbedtls_cipher_context_t *ctx)
{
    int i = 0;
    for (i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i) {
        if (g_cmac_harden_context[i].ctx == ctx) {
            if (g_cmac_harden_context[i].symc_handle != 0) {
                (void)CIPHER_SYMC_DESTROY(g_cmac_harden_context[i].symc_handle);
                (void)CIPHER_SYMC_DEINIT();
            }
            if (g_cmac_harden_context[i].keyslot_handle != 0) {
                (void)CIPHER_DESTROY_KEYSLOT(g_cmac_harden_context[i].keyslot_handle);
                (void)KM_DEINIT();
            }
            (void)memset_s(&g_cmac_harden_context[i], sizeof(cmac_harden_context), 0, sizeof(cmac_harden_context));
        }
    }
}

int mbedtls_cipher_cmac_starts_harden(mbedtls_cipher_context_t *ctx, const unsigned char *key, size_t keybits,
    mbedtls_cipher_type_t type)
{
    int i = 0;
    int ret = 0;
    internal_check_and_free_harden_ctx(ctx);
    if (IS_SYMC_FUNC_REGISTERED() != TD_TRUE)
        return (-1);
    if (type == MBEDTLS_CIPHER_DES_EDE3_ECB)
        return (-1);
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if (type == MBEDTLS_CIPHER_AES_192_ECB)
        return (-1);
#endif
    for (i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i) {
        if (g_cmac_harden_context[i].is_used == 0) {
            g_cmac_harden_context[i].ctx = ctx;
            g_cmac_harden_context[i].is_used = 1;
            g_cmac_harden_context[i].key_length = keybits / 8;  // 8: 8 bits in byte
            g_cmac_harden_context[i].is_update = 0;
            g_cmac_harden_context[i].is_finish = 0;
            (void)memcpy_s(g_cmac_harden_context[i].key, sizeof(g_cmac_harden_context[i].key), key,
                g_cmac_harden_context[i].key_length);
            ret = internal_cmac_starts_harden(&g_cmac_harden_context[i]);
            if (ret != 0)
                g_cmac_harden_context[i].use_software = 1;
            break;
        }
    }
    return ret;
}

int mbedtls_cipher_cmac_update_harden(mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen)
{
    int ret = 0;
    cmac_harden_context *harden_ctx;
    crypto_buf_attr src_buf = {0};

    harden_ctx = get_cmac_harden_ctx(ctx);
    if (harden_ctx == NULL || harden_ctx->use_software == 1)
        return (-1);
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if (harden_ctx->key_length == 24)   // 24: 192-bit key
        return (-1);
#endif
    if (harden_ctx->is_finish == 1) {
        ret = internal_cmac_starts_harden(harden_ctx);
        if (ret != 0) {
            harden_ctx->is_finish = 0;
            harden_ctx->use_software = 1;
            return (-1);
        }
    }
    if (ilen == 0)
        return (0);
    harden_ctx->is_update = 1;
    src_buf.virt_addr = (void *)input;
    ret = CIPHER_SYMC_MAC_UPDATE(harden_ctx->symc_handle, &src_buf, ilen);
    return ret;
}

static void reset_harden_ctx(cmac_harden_context *harden_ctx)
{
    harden_ctx->is_finish = 1;
    harden_ctx->is_update = 0;
    harden_ctx->use_software = 0;
    harden_ctx->symc_handle = 0;
    harden_ctx->keyslot_handle = 0;
}

static void release_hard_resource(cmac_harden_context *harden_ctx)
{
    (void)CIPHER_DESTROY_KEYSLOT(harden_ctx->keyslot_handle);
    (void)KM_DEINIT();
    (void)CIPHER_SYMC_DEINIT();
}

int mbedtls_cipher_cmac_finish_harden(mbedtls_cipher_context_t *ctx, unsigned char *output)
{
    int ret = 0;
    cmac_harden_context *harden_ctx;
    unsigned int mac_len = MBEDTLS_CIPHER_BLKSIZE_MAX;
    harden_ctx = get_cmac_harden_ctx(ctx);
    if (harden_ctx == NULL)
        return (-1);
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if (harden_ctx->key_length == 24)   // 24: 192-bit key
        return (-1);
#endif
    if (harden_ctx->is_update == 0) {
        (void)CIPHER_SYMC_DESTROY(harden_ctx->symc_handle);
        release_hard_resource(harden_ctx);
        reset_harden_ctx(harden_ctx);
        return (-1);
    }
    ret = CIPHER_SYMC_MAC_FINISH(harden_ctx->symc_handle, output, &mac_len);
    release_hard_resource(harden_ctx);
    reset_harden_ctx(harden_ctx);
    return ret;
}

int mbedtls_cipher_cmac_reset_harden(mbedtls_cipher_context_t *ctx)
{
    cmac_harden_context *harden_ctx;
    harden_ctx = get_cmac_harden_ctx(ctx);
    if (harden_ctx == NULL)
        return (-1);

    if (harden_ctx->symc_handle != 0) {
        (void)CIPHER_SYMC_DESTROY(harden_ctx->symc_handle);
        (void)CIPHER_SYMC_DEINIT();
    }
    if (harden_ctx->keyslot_handle != 0) {
        (void)CIPHER_DESTROY_KEYSLOT(harden_ctx->keyslot_handle);
        (void)KM_DEINIT();
    }
    reset_harden_ctx(harden_ctx);
    return 0;
}

int mbedtls_cipher_cmac_starts(mbedtls_cipher_context_t *ctx, const unsigned char *key, size_t keybits)
{
    mbedtls_cipher_type_t type;

    if (ctx == NULL || ctx->cipher_info == NULL || key == NULL)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    type = ctx->cipher_info->type;

    switch (type) {
        case MBEDTLS_CIPHER_AES_128_ECB:
        case MBEDTLS_CIPHER_AES_192_ECB:
        case MBEDTLS_CIPHER_AES_256_ECB:
            break;
        default:
            return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    return mbedtls_cipher_cmac_starts_harden(ctx, key, keybits, type);
}

int mbedtls_cipher_cmac_update(mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen)
{
    if (ctx == NULL || ctx->cipher_info == NULL || input == NULL)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    return mbedtls_cipher_cmac_update_harden(ctx, input, ilen);
}

int mbedtls_cipher_cmac_finish(mbedtls_cipher_context_t *ctx, unsigned char *output)
{
    if (ctx == NULL || ctx->cipher_info == NULL || output == NULL)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    return mbedtls_cipher_cmac_finish_harden(ctx, output);
}

int mbedtls_cipher_cmac_reset(mbedtls_cipher_context_t *ctx)
{
    if (ctx == NULL || ctx->cipher_info == NULL)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    return mbedtls_cipher_cmac_reset_harden(ctx);
}

int mbedtls_cipher_cmac(const mbedtls_cipher_info_t *cipher_info, const unsigned char *key, size_t keylen,
    const unsigned char *input, size_t ilen, unsigned char *output)
{
    mbedtls_cipher_context_t ctx;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    int final_ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_cipher_init(&ctx);
 
    ret = mbedtls_cipher_setup(&ctx, cipher_info);
    if (ret != 0) {
        final_ret = ret;
        goto exit;
    }

    ret = mbedtls_cipher_cmac_starts(&ctx, key, keylen);
    if (ret != 0) {
        final_ret = ret;
        goto exit;
    }

    ret = mbedtls_cipher_cmac_update(&ctx, input, ilen);
    if (ret != 0) {
        final_ret = ret;
        goto exit;
    }
    final_ret = 0;
exit:
    ret = mbedtls_cipher_cmac_finish(&ctx, output);
    if (ret != 0) {
        final_ret = ret;
    }
    mbedtls_cipher_free(&ctx);

    return final_ret;
}

static int inner_cmac_prf_int_key(const mbedtls_cipher_info_t *cipher_info,
                                  const unsigned char *key,
                                  size_t key_length,
                                  unsigned char int_key[MBEDTLS_AES_BLOCK_SIZE])
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char zero_key[MBEDTLS_AES_BLOCK_SIZE];

    if (key_length == MBEDTLS_AES_BLOCK_SIZE) {
        memcpy_s(int_key, sizeof(int_key), key, MBEDTLS_AES_BLOCK_SIZE);
        return 0;
    }

    memset_s(zero_key, sizeof(zero_key), 0, sizeof(zero_key));
    ret = mbedtls_cipher_cmac(cipher_info,
                              zero_key, 128,   /* 128 : AES-128 key bits */
                              key, key_length,
                              int_key);

    mbedtls_platform_zeroize(zero_key, sizeof(zero_key));
    return ret;
}

int mbedtls_aes_cmac_prf_128(const unsigned char *key, size_t key_length,
                             const unsigned char *input, size_t in_len,
                             unsigned char output[MBEDTLS_AES_BLOCK_SIZE])
{
    int ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    const mbedtls_cipher_info_t *cipher_info = NULL;
    unsigned char int_key[MBEDTLS_AES_BLOCK_SIZE];

    if (key == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    /* 1. set cipher type */
    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        return MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
    }

    /* 2. set init key */
    ret = inner_cmac_prf_int_key(cipher_info, key, key_length, int_key);
    if (ret != 0) {
        mbedtls_platform_zeroize(int_key, sizeof(int_key));
        return ret;
    }

    ret = mbedtls_cipher_cmac(cipher_info,
                              int_key, 128,   /* 128 : AES-128 key bits */
                              input, in_len,
                              output);
    mbedtls_platform_zeroize(int_key, sizeof(int_key));
    return ret;
}

#endif /* !MBEDTLS_CMAC_ALT */