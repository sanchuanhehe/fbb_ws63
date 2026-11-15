/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt source file.
 * Create: 2025-12-03
*/

#include "library/common.h"

#if defined(MBEDTLS_AES_C)

#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include "securec.h"
#include "mbedtls_harden_adapt_api.h"

#define BITS_IN_BYTE        8

#if defined(MBEDTLS_AES_ALT)

/* check parameter validation macro */
#define AES_VALIDATE_RET(cond)    \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA)
#define AES_VALIDATE(cond)        \
    MBEDTLS_INTERNAL_VALIDATE(cond)

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    AES_VALIDATE(ctx != NULL);

    (void)memset_s(ctx, sizeof(mbedtls_aes_context), 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(mbedtls_aes_context), 0, sizeof(mbedtls_aes_context));
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init(mbedtls_aes_xts_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_aes_init(&ctx->crypt);
    mbedtls_aes_init(&ctx->tweak);
}

void mbedtls_aes_xts_free(mbedtls_aes_xts_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_aes_free(&ctx->crypt);
    mbedtls_aes_free(&ctx->tweak);
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    int ret;

    if (ctx == NULL || key == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    ret = memcpy_s(ctx->key, sizeof(ctx->key), key, keybits / BITS_IN_BYTE);
    if (ret != EOK) {
        return -1;
    }
    ctx->key_len = keybits / BITS_IN_BYTE;
    return 0;
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    int ret;
    AES_VALIDATE_RET(ctx != NULL);
    AES_VALIDATE_RET(key != NULL);

    ret = memcpy_s(ctx->key, sizeof(ctx->key), key, keybits / BITS_IN_BYTE);
    if (ret != EOK) {
        return -1;
    }
    ctx->key_len = keybits / BITS_IN_BYTE;
    return 0;
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
#define BLOCK_SIZE_IN_BYTE 16

static void inner_byte_xor(unsigned char *r, const unsigned char *a,
                           const unsigned char *b, size_t n)
{
    size_t i = 0;
    for (; i < n; i++) {
        r[i] = a[i] ^ b[i];
    }
}

static inline void inner_gf128mul_x_ble(unsigned char r[16], const unsigned char x[16])
{
    uint64_t a, b, ra, rb;

    a = MBEDTLS_GET_UINT64_LE(x, 0);
    b = MBEDTLS_GET_UINT64_LE(x, 8);    // 8: byte len

    ra = (a << 1)  ^ 0x0087 >> (8 - ((b >> 63) << 3));  // 0x0087, 8, 63, 3: algo require
    rb = (a >> 63) | (b << 1);  // 63: algo require

    MBEDTLS_PUT_UINT64_LE(ra, r, 0);
    MBEDTLS_PUT_UINT64_LE(rb, r, 8);    // 8: byte len
}

static int inner_xts_process_tail(mbedtls_aes_xts_context *ctx, int mode,
    unsigned char *t, const unsigned char *input, unsigned char *output, size_t tail_len)
{
    int ret;
    unsigned int i;
    unsigned char tmp[BLOCK_SIZE_IN_BYTE];
    unsigned char *prev_output = output - BLOCK_SIZE_IN_BYTE;

    if (tail_len == 0) {
        return 0;
    }

    for (i = 0; i < tail_len; i++) {
        output[i] = prev_output[i];
    }
    inner_byte_xor(tmp, input, t, tail_len);
    inner_byte_xor(tmp + i, prev_output + i, t + i, BLOCK_SIZE_IN_BYTE - i);
    ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
    if (ret != 0) {
        return ret;
    }
    inner_byte_xor(prev_output, tmp, t, BLOCK_SIZE_IN_BYTE);
    return 0;
}

int mbedtls_aes_xts_setkey_enc(mbedtls_aes_xts_context *ctx,
    const unsigned char *key, unsigned int keybits)
{
    int ret;
    unsigned int key_len;
    if (keybits == 256) {   // 256: 256bits
        key_len = 16;   // 16: aes-128
    } else if (keybits == 512) {    // 512: 512bits
        key_len = 32;   // 32: aes-256
    } else {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ret = mbedtls_aes_setkey_enc(&ctx->crypt, key, keybits / 2);    // 2: half
    if (ret != 0) {
        return ret;
    }

    return mbedtls_aes_setkey_enc(&ctx->tweak, key + key_len, keybits / 2); // 2: half
}


int mbedtls_aes_xts_setkey_dec(mbedtls_aes_xts_context *ctx,
    const unsigned char *key, unsigned int keybits)
{
    return mbedtls_aes_xts_setkey_enc(ctx, key, keybits);
}

int mbedtls_aes_crypt_xts(mbedtls_aes_xts_context *ctx, int mode,
    size_t length, const unsigned char data_unit[16],
    const unsigned char *input, unsigned char *output)
{
    int ret;
    size_t block_num = length / BLOCK_SIZE_IN_BYTE;
    size_t tail_len = length % BLOCK_SIZE_IN_BYTE;
    unsigned char curr_tweak[BLOCK_SIZE_IN_BYTE];
    unsigned char last_tweak[BLOCK_SIZE_IN_BYTE];
    unsigned char tmp[BLOCK_SIZE_IN_BYTE];
    unsigned int i;
    unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? last_tweak : curr_tweak;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if (block_num == 0) {
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    if (length > (1 << 20) * BLOCK_SIZE_IN_BYTE) {  // 20: max block size
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Compute the tweak. */
    ret = mbedtls_aes_crypt_ecb(&ctx->tweak, MBEDTLS_AES_ENCRYPT, data_unit, curr_tweak);
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < block_num; i++) {
        if ((i == block_num - 1) && (mode == MBEDTLS_AES_DECRYPT) && (tail_len != 0)) {
            (void)memcpy_s(last_tweak, sizeof(last_tweak), curr_tweak, sizeof(curr_tweak));
            inner_gf128mul_x_ble(curr_tweak, curr_tweak);
        }
        inner_byte_xor(tmp, input + i * BLOCK_SIZE_IN_BYTE, curr_tweak, BLOCK_SIZE_IN_BYTE);
        ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
        if (ret != 0) {
            return ret;
        }
        inner_byte_xor(output + i * BLOCK_SIZE_IN_BYTE, tmp, curr_tweak, BLOCK_SIZE_IN_BYTE);
        /* Update the tweak for the next block. */
        inner_gf128mul_x_ble(curr_tweak, curr_tweak);
    }

    return inner_xts_process_tail(ctx, mode, t, input + block_num * BLOCK_SIZE_IN_BYTE,
        output + block_num * BLOCK_SIZE_IN_BYTE, tail_len);
}

#endif /* MBEDTLS_CIPHER_MODE_XTS */

/*
 * AES-ECB block encryption
 */
int mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16])
{
    if (ctx == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    return mbedtls_alt_aes_encrypt(ctx->key, ctx->key_len, input, output);
}

/*
 * AES-ECB block decryption
 */
int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16])
{
    if (ctx == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    return mbedtls_alt_aes_decrypt(ctx->key, ctx->key_len, input, output);
}

/*
 * AES-ECB encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
    if (ctx == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        return mbedtls_internal_aes_encrypt(ctx, input, output);
    } else {
        return mbedtls_internal_aes_decrypt(ctx, input, output);
    }
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16],
    const unsigned char *input, unsigned char *output)
{
    if (ctx == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (iv == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        return mbedtls_alt_aes_cbc_encrypt(ctx->key, ctx->key_len, iv, input, output, length);
    } else {
        return mbedtls_alt_aes_cbc_decrypt(ctx->key, ctx->key_len, iv, input, output, length);
    }
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 encryption/decryption
 */
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx, int mode, size_t length, size_t *iv_off, unsigned char iv[16],
    const unsigned char *input, unsigned char *output)
{
    unsigned char c;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t n;
    size_t local_length = length;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (ctx == NULL || iv_off == NULL || iv == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    if (n > 15) { // 15: aes block size - 1
        return (MBEDTLS_ERR_AES_BAD_INPUT_DATA);
    }

    if (mode == MBEDTLS_AES_DECRYPT) {
        while (local_length--) {
            if (n == 0) {
                ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
                if (ret != 0) {
                    goto exit;
                }
            }

            c = *input++;
            *output++ = (unsigned char)(c ^ iv[n]);
            iv[n] = c;

            n = (n + 1) & 0x0F;
        }
    } else {
        while (local_length--) {
            if (n == 0) {
                ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
                if (ret != 0) {
                    goto exit;
                }
            }

            iv[n] = *output++ = (unsigned char)(iv[n] ^ *input++);

            n = (n + 1) & 0x0F;
        }
    }

    *iv_off = n;
    ret = 0;

exit:
    return (ret);
}

/*
 * AES-CFB8 encryption/decryption
 */
int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16],
    const unsigned char *input, unsigned char *output)
{
    if (ctx == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (iv == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        return mbedtls_alt_aes_cfb8_encrypt(ctx->key, ctx->key_len, iv, input, output, length);
    } else {
        return mbedtls_alt_aes_cfb8_decrypt(ctx->key, ctx->key_len, iv, input, output, length);
    }
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB encryption/decryption
 */
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx, size_t length, size_t *iv_off, unsigned char iv[16],
    const unsigned char *input, unsigned char *output)
{
    if (ctx == NULL || iv_off == NULL || iv == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (*iv_off > 15) { // 15: aes block size - 1
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    return mbedtls_alt_aes_ofb_crypt(ctx->key, ctx->key_len, (uint8_t *)iv_off, iv, input, output, length);
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx, size_t length, size_t *nc_off, unsigned char nonce_counter[16],
    unsigned char stream_block[16], const unsigned char *input, unsigned char *output)
{
    if (ctx == NULL || nc_off == NULL || nonce_counter == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (stream_block == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    return mbedtls_alt_aes_ctr_crypt(ctx->key, ctx->key_len, (uint8_t *)nc_off, nonce_counter, stream_block, input,
        output, length);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* !MBEDTLS_AES_ALT */
#endif /* MBEDTLS_AES_C */
