/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 crypto implement.
 *
 * Create: 2023-05-19
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

#define SM2_PC_UNCOMPRESS                   0x04
#define SM2_CRYPTO_ADD_LENGTH_IN_BYTE       97

/*
 * inner function API start
*/
CRYPTO_STATIC td_s32 inner_sm2_kdf(const drv_pke_ecc_point *param, td_u8 *out, const td_u32 klen);
CRYPTO_STATIC td_s32 inner_sm2_encrypt_decrypt_hash(const drv_pke_ecc_point *param,
    const drv_pke_data *msg, drv_pke_data *hash);
CRYPTO_STATIC td_s32 inner_sm2_public_encrypt_param_check(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *plain_text, const drv_pke_data *cipher_text);
CRYPTO_STATIC td_s32 inner_sm2_private_decrypt_param_check(const drv_pke_data *priv_key,
    const drv_pke_data *cipher_text, const drv_pke_data *plain_text);

CRYPTO_STATIC td_s32 inner_sm2_kdf(const drv_pke_ecc_point *param, td_u8 *out, const td_u32 klen)
{
    td_s32 ret = TD_FAILURE;
    td_u32 block = 0;
    td_u32 i = 0;
    td_u32 ct = 0;
    drv_pke_data arr[3];
    td_u8 h[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    drv_pke_data hash = {DRV_PKE_SM2_LEN_IN_BYTES, h};

    crypto_param_check(param == TD_NULL);
    crypto_param_check(param->x == TD_NULL);
    crypto_param_check(param->y == TD_NULL);
    crypto_param_check(out == TD_NULL);

    arr[0].data = param->x;
    arr[0].length = DRV_PKE_SM2_LEN_IN_BYTES;
    arr[1].data = param->y;
    arr[1].length = DRV_PKE_SM2_LEN_IN_BYTES;
    arr[2].data = (td_u8 *)&ct; // 2 is index of arr
    arr[2].length = (td_u32)sizeof(ct); // 2 is index of arr
    if (klen == 0) {
        return TD_SUCCESS;
    }

    block = (klen + DRV_PKE_SM2_LEN_IN_BYTES - 1) / DRV_PKE_SM2_LEN_IN_BYTES;
    for (i = 0; i < block; i++) {
        ct = crypto_cpu_to_be32(i + 1);
        /* *** H = SM3(X || Y || CT) *** */
        ret = crypto_drv_pke_common_calc_hash(arr, sizeof(arr) / sizeof(arr[0]), DRV_PKE_HASH_TYPE_SM3, &hash);
        crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_calc_hash failed, ret is 0x%x\n", ret);

        if (i == (block - 1)) {
            ret = memcpy_s(out + i * DRV_PKE_SM2_LEN_IN_BYTES, DRV_PKE_SM2_LEN_IN_BYTES, h,
                klen - i * DRV_PKE_SM2_LEN_IN_BYTES);
            crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
        } else {
            (void)memcpy_s(out + i * DRV_PKE_SM2_LEN_IN_BYTES, DRV_PKE_SM2_LEN_IN_BYTES, h, DRV_PKE_SM2_LEN_IN_BYTES);
        }
    }

    return TD_SUCCESS;
}

CRYPTO_STATIC td_s32 inner_sm2_encrypt_decrypt_hash(const drv_pke_ecc_point *param,
    const drv_pke_data *msg, drv_pke_data *hash)
{
    td_s32 ret = TD_FAILURE;
    drv_pke_data arr[3];

    crypto_param_check(param == TD_NULL);
    crypto_param_check(param->x == TD_NULL);
    crypto_param_check(param->y == TD_NULL);
    crypto_param_check(msg == TD_NULL);
    crypto_param_check(msg->data == TD_NULL);
    crypto_param_check(hash == TD_NULL);

    arr[0].data = param->x;
    arr[0].length = DRV_PKE_SM2_LEN_IN_BYTES;
    arr[1].data = msg->data;
    arr[1].length = msg->length;
    arr[2].data = param->y;  // 2 is data of the 7th element
    arr[2].length = DRV_PKE_SM2_LEN_IN_BYTES;   // 2 is length of the 7th element

    /* *** C3 =Hash(x2||M||y2) *** */
    ret = crypto_drv_pke_common_calc_hash(arr, sizeof(arr) / sizeof(arr[0]), DRV_PKE_HASH_TYPE_SM3, hash);
    return ret;
}

CRYPTO_STATIC td_s32 inner_sm2_public_encrypt_param_check(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *plain_text, const drv_pke_data *cipher_text)
{
    drv_crypto_pke_check_param(pub_key == TD_NULL || pub_key->x == TD_NULL || pub_key->y == TD_NULL);
    drv_crypto_pke_check_param(plain_text == TD_NULL || plain_text->data == TD_NULL);
    drv_crypto_pke_check_param(cipher_text == TD_NULL || cipher_text->data == TD_NULL);
    drv_crypto_pke_check_param(pub_key->length != DRV_PKE_LEN_256);
    drv_crypto_pke_check_param(cipher_text->length < (plain_text->length + SM2_CRYPTO_ADD_LENGTH_IN_BYTE));
    return TD_SUCCESS;
}

CRYPTO_STATIC td_s32 inner_sm2_private_decrypt_param_check(const drv_pke_data *priv_key,
    const drv_pke_data *cipher_text, const drv_pke_data *plain_text)
{
    drv_crypto_pke_check_param(priv_key == TD_NULL || priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(plain_text == TD_NULL || plain_text->data == TD_NULL);
    drv_crypto_pke_check_param(cipher_text == TD_NULL || cipher_text->data == TD_NULL);
    drv_crypto_pke_check_param(priv_key->length != DRV_PKE_LEN_256);
    drv_crypto_pke_check_param(cipher_text->length > (plain_text->length + SM2_CRYPTO_ADD_LENGTH_IN_BYTE));
    drv_crypto_pke_check_param(cipher_text->data[0] != SM2_PC_UNCOMPRESS);
    return TD_SUCCESS;
}
/*
 * inner function API end
*/

td_s32 crypto_drv_pke_common_sm2_public_encrypt(const drv_pke_ecc_point *pub_key, const drv_pke_data *plain_text,
    const drv_pke_data *cipher_text)
{
    // declaration and assignment of variables and parameter verification
    td_s32 ret = TD_FAILURE;
    td_u32 klen = DRV_PKE_SM2_LEN_IN_BYTES;
    td_u32 c1_index = 1;
    td_u32 c3_index = c1_index + klen * 2;
    td_u32 c2_index = c1_index + klen * 3;
    td_u32 i = 0;
    td_u8 k[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    td_u8 c1x[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    td_u8 c1y[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    td_u8 k_pb_x[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    td_u8 k_pb_y[DRV_PKE_SM2_LEN_IN_BYTES] = {0};
    drv_pke_data k_data = {klen, k};
    drv_pke_data c1x_data = {klen, c1x};
    drv_pke_data c1y_data = {klen, c1y};
    drv_pke_data c3_data = {0};
    drv_pke_ecc_point c1_point = {c1x, c1y, klen};
    drv_pke_ecc_point k_pb_point = {k_pb_x, k_pb_y, klen};
    drv_pke_ecc_point sm2_gpoint;

    crypto_drv_func_enter();
    ret = inner_sm2_public_encrypt_param_check(pub_key, plain_text, cipher_text);
    crypto_chk_func_return(inner_sm2_public_encrypt_param_check, ret);
    const drv_pke_ecc_curve *sm2 = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(sm2 == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    c3_data = (drv_pke_data){klen, cipher_text->data + c3_index};
    sm2_gpoint = (drv_pke_ecc_point){(td_u8 *)sm2->gx, (td_u8 *)sm2->gy, klen};
    (void)memset_s(cipher_text->data, cipher_text->length, 0, cipher_text->length);
    // the beginning of the algorithm
    cipher_text->data[0] = SM2_PC_UNCOMPRESS;

    for (i = 0; i < PKE_MAX_TIMES; i++) {
        /* Step 1: random k */
        ret = get_random_key(sec_arg_add_cs(sm2->n, sm2->ksize, k_data.data));
        crypto_chk_func_return(get_random_key, ret);

        /* Step 2: C1 = k * G */
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(sm2, &k_data, &sm2_gpoint, &c1_point));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);

        ret = memcpy_enhance(cipher_text->data + c1_index, klen, c1x_data.data, klen);
        crypto_chk_func_return(memcpy_enhance, ret);
        ret = memcpy_enhance(cipher_text->data + c1_index + klen, klen, c1y_data.data, klen);
        crypto_chk_func_return(memcpy_enhance, ret);

        /* Step 3: kPB = k * PB */
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(sm2, &k_data, pub_key, &k_pb_point));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);

        /* Step 4: t = KDF(kPBx || kPBy, plain_text->length) */
        ret = inner_sm2_kdf(&k_pb_point, cipher_text->data + c2_index, plain_text->length);
        crypto_chk_func_return(inner_sm2_kdf, ret);

        /* Step 5: t == 0? */
        if (crypto_drv_pke_common_is_zero(cipher_text->data + c2_index, plain_text->length) != TD_TRUE) {
            break;
        }
    }
    if (i == PKE_MAX_TIMES) {
        crypto_log_err("Error! Can't gen t in %d times!\n", ECC_TRY_CNT);
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    /* Step 6: C2 = plain_text->data ^ t */
    for (i = 0; i < plain_text->length; i++) {
        *(cipher_text->data + c2_index + i) ^= *(plain_text->data + i);
    }

    /* Step 7: C3 = Hash(kPBx || plain_text || kPBy) */
    ret = inner_sm2_encrypt_decrypt_hash(&k_pb_point, plain_text, &c3_data);
    crypto_chk_func_return(inner_sm2_encrypt_decrypt_hash, ret);

    /* Step 8: cipher_text = C1 || C2 || C3. which has realized in the before steps. */
    crypto_drv_func_exit();
    return ret;
}

td_s32 crypto_drv_pke_common_sm2_private_decrypt(const drv_pke_data *priv_key, const drv_pke_data *cipher_text,
    const drv_pke_data *plain_text)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = DRV_PKE_SM2_LEN_IN_BYTES;
    td_u32 c1_index = 1;
    td_u32 c2_index = 1 + klen * 3;
    td_u32 c3_index = 1 + klen * 2;
    td_u32 i = 0;
    td_bool is_on_curve = TD_FALSE;
    td_u8 u[DRV_PKE_SM2_LEN_IN_BYTES];
    td_u8 c1x[DRV_PKE_SM2_LEN_IN_BYTES];
    td_u8 c1y[DRV_PKE_SM2_LEN_IN_BYTES];
    td_u8 x2[DRV_PKE_SM2_LEN_IN_BYTES];
    td_u8 y2[DRV_PKE_SM2_LEN_IN_BYTES];
    drv_pke_data u_data = {klen, u};
    drv_pke_data plain_data;
    const drv_pke_ecc_point c1_point = {c1x, c1y, klen};
    const drv_pke_ecc_point db_c1_point = {x2, y2, klen};

    crypto_drv_func_enter();
    ret = inner_sm2_private_decrypt_param_check(priv_key, cipher_text, plain_text);
    crypto_chk_func_return(inner_sm2_private_decrypt_param_check, ret);
    const drv_pke_ecc_curve *sm2 = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(sm2 == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    plain_data = (drv_pke_data){cipher_text->length - SM2_CRYPTO_ADD_LENGTH_IN_BYTE, plain_text->data};

    (void)memcpy_s(c1x, klen, cipher_text->data + c1_index, klen);
    (void)memcpy_s(c1y, klen, cipher_text->data + c1_index + klen, klen);

    /* Step 1: check c1x, c1y on curve */
    ret = ecc_ecfp_point_valid_standard(sec_arg_add_cs(DRV_PKE_ECC_TYPE_SM2, &c1_point, &is_on_curve));
    crypto_chk_func_return(ecc_ecfp_point_valid_standard, ret);
    crypto_chk_return(is_on_curve != TD_TRUE, ret, "pke_check_dot_on_curve failed\n");

    /* Step 2: (x2, y2) = dB * C1 */
    ret = ecc_ecfp_mul_naf(sec_arg_add_cs(sm2, priv_key, &c1_point, &db_c1_point));
    crypto_chk_func_return(ecc_ecfp_mul_naf, ret);

    /* Step 3: check x2, y2 on curve */
    ret = ecc_ecfp_point_valid_standard(sec_arg_add_cs(DRV_PKE_ECC_TYPE_SM2, &db_c1_point, &is_on_curve));
    crypto_chk_func_return(ecc_ecfp_point_valid_standard, ret);
    crypto_chk_return(is_on_curve != TD_TRUE, ret, "pke_check_dot_on_curve failed\n");

    /* Step 4: t = KDF(db_c1_point_x || db_c1_point_y, plain_data.length) */
    ret = inner_sm2_kdf(&db_c1_point, plain_data.data, plain_data.length);
    crypto_chk_func_return(inner_sm2_kdf, ret);

    /* Step 5: check whether t is 0. */
    if (crypto_drv_pke_common_is_zero(plain_data.data, plain_data.length) == TD_TRUE) {
        return TD_FAILURE;
    }

    /* Step 6: plain_data.data = C2 ^ t. */
    for (i = 0; i < plain_data.length; i++) {
        *(plain_data.data + i) ^= *(cipher_text->data + c2_index + i);
    }

    /* Step 7: u = Hash(db_c1_point_x || plain_data.data || db_c1_point_y) */
    ret = inner_sm2_encrypt_decrypt_hash(&db_c1_point, &plain_data, &u_data);
    crypto_chk_func_return(inner_sm2_encrypt_decrypt_hash, ret);

    /* Step 8: check whether u == C3 */
    ret = memcmp_enhance(cipher_text->data + c3_index, u, DRV_PKE_SM2_LEN_IN_BYTES);
    crypto_chk_func_return(memcmp_enhance, ret);

    crypto_drv_func_exit();
    return ret;
}