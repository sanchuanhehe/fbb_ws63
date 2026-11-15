/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 sign function implement.
 *
 * Create: 2023-05-19
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

CRYPTO_STATIC td_s32 inner_sm2_sign_param_check(const drv_pke_data *priv_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig);

CRYPTO_STATIC td_s32 inner_sm2_sign_param_check(const drv_pke_data *priv_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig)
{
    drv_crypto_pke_check_param(priv_key == TD_NULL || priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(hash == TD_NULL || hash->data == TD_NULL || hash->length == 0);
    drv_crypto_pke_check_param(sig == TD_NULL || sig->r == TD_NULL || sig->s == TD_NULL);
    drv_crypto_pke_check_param(priv_key->length != DRV_PKE_LEN_256);
    drv_crypto_pke_check_param(sig->length != DRV_PKE_LEN_256);
    /* increase security strength check. */
    drv_crypto_pke_check_param(hash->length < DRV_PKE_LEN_256);
    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 local_cnt = local_step(0);
    td_u32 i = 0;
    /* construct these vars in unions for storage occupation optimization */
    td_u8 k[DRV_PKE_LEN_256];       /* step 2.1, 2.2, 2.5, 3 */
    td_u8 ry[DRV_PKE_LEN_256];      /* step 2.2 */
    union {
        td_u8 rx[DRV_PKE_LEN_256];      /* step 2.2, 2.4  */
        td_u8 rk[DRV_PKE_LEN_256];      /* step 2.5, 2.6 */
    } pke_data_1;
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data cc = {0};
    drv_pke_data pp = {0};
    drv_pke_ecc_point rr = {0};
    drv_pke_ecc_point gg = {0};

    crypto_drv_func_enter();

    ret = inner_sm2_sign_param_check(priv_key, hash, sig);
    crypto_chk_func_return(inner_sm2_sign_param_check, ret);
    const drv_pke_ecc_curve *sm2 = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(sm2 == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    /* Step 1: set curve initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(sm2);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 2 & 3: calculate r and s. */
    for (i = 0; i < PKE_MAX_TIMES; i++) {
        /* Step 2: calculate r. */
        /* Step 2.1: get random k in [1, n-1] */
        ret = TD_FAILURE;
        ret = get_random_key(sec_arg_add_cs(sm2->n, DRV_PKE_LEN_256, k));
        crypto_chk_func_return(get_random_key, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 2.2 : R(x1, y1) = k * G, outPut point is in the affine coordinate system. */
        ret = TD_FAILURE;
        aa = (drv_pke_data) {.length = sm2->ksize, .data = k};
        gg = (drv_pke_ecc_point) {.x = (td_u8 *)sm2->gx, .y = (td_u8 *)sm2->gy,
            .length = sm2->ksize};
        rr = (drv_pke_ecc_point) {.length = sm2->ksize, .x = pke_data_1.rx, .y = ry};
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(sm2, &aa, &gg, &rr));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 2.3: update module and set montgomery parameters. */
        ret = update_modulus(sec_arg_add_cs(sm2->n, sm2->ksize));
        crypto_chk_func_return(update_modulus, ret);

        /* Step 2.4: ecfp_add_mod to calculate the r value of signature. r = (e + Rx) mod n */
        /* Need to truncate digest if it is too long. */
        aa = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = hash->data};
        bb = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = pke_data_1.rx};
        pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = (td_u8 *)sm2->n};
        cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = sig->r};
        ret = TD_FAILURE;
        ret = ecc_ecfn_add_mod(sec_arg_add_cs(&aa, &bb, &pp, &cc));
        crypto_chk_func_return(ecc_ecfn_add_mod, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 2.4: r = 0 ? */
        if (crypto_drv_pke_common_is_zero(sig->r, DRV_PKE_LEN_256) == TD_TRUE) {
            local_cnt += LOCAL_STEP_AUTH;
            continue;
        } else {
            local_cnt += LOCAL_STEP_AUTH;
        }

        /* Step 2.5: rk = r + k mod n */
        ret = TD_FAILURE;
        aa = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = sig->r};
        bb = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = k};
        pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = (td_u8 *)sm2->n};
        cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = pke_data_1.rk};
        ret = ecc_ecfn_add_mod(sec_arg_add_cs(&aa, &bb, &pp, &cc));
        crypto_chk_func_return(ecc_ecfn_add_mod, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 2.6: r + k = n ? that's to say rk = r + k mod n = 0 ? */
        if (crypto_drv_pke_common_is_zero(pke_data_1.rk, DRV_PKE_LEN_256) == TD_TRUE) {
            local_cnt += LOCAL_STEP_AUTH;
            continue;
        } else {
            local_cnt += LOCAL_STEP_AUTH;
        }

        /* Step 3: calculate s = ((1 + dA)^(-1) * (k - r*dA)) mod n. */
        cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = sig->s};
        ret = sm2_ecfn_sign_s(&bb, priv_key, &pp, &aa, &cc);
        crypto_chk_func_return(sm2_ecfn_sign_s, ret);
        /* Step 3.1: s = 0 ? */
        if (crypto_drv_pke_common_is_zero(sig->s, DRV_PKE_LEN_256) == TD_TRUE) {
            local_cnt += LOCAL_STEP_AUTH;
            continue;
        } else {
            local_cnt += LOCAL_STEP_AUTH;
            break;
        }
    }

    if (i != 0) {
        crypto_log_trace("Generate r & s times: %d", i);
    }

    if (i >= PKE_MAX_TIMES) {
        crypto_log_err("Generate r & s timeout!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    val_enhance_chk(local_cnt, local_step(i * 7)); // i * 7 steps total
    crypto_drv_func_exit();
    return TD_SUCCESS;
}