/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: crypto algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

td_s32 ecc_ecfp_demontgomery_data_jac_z(const drv_pke_data *z, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    /* Step 1: start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_demont_cz_3, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 2: get data from PKE DRAM */
    if (z != TD_NULL && z->data != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cz, z->data, z->length));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ecc_ecfp_point_valid_standard(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    const drv_pke_ecc_curve *ecc;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), curve_type, pub_key, is_on_curve);

    drv_crypto_pke_check_param(pub_key == TD_NULL);
    drv_crypto_pke_check_param(pub_key->x == TD_NULL);
    drv_crypto_pke_check_param(pub_key->y == TD_NULL);
    drv_crypto_pke_check_param(is_on_curve == TD_NULL);
    drv_crypto_pke_check_param(curve_type >= DRV_PKE_ECC_TYPE_MAX || curve_type < DRV_PKE_ECC_TYPE_RFC5639_P256 ||
        curve_type == DRV_PKE_ECC_TYPE_RFC8032);
    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");
    drv_crypto_pke_check_param(ecc->ksize != pub_key->length);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(pub_key->x, pub_key->length) == TD_TRUE);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(pub_key->y, pub_key->length) == TD_TRUE);

    /* 1. set data into DRAM */
    td_u8 z[DRV_PKE_LEN_576] = {0};
    drv_pke_data mod_p = {0};
    drv_pke_data kk = {0};
    drv_pke_data check_z = (drv_pke_data){.data = (td_u8 *)z, .length = ecc->ksize};
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 0: set montgomery param into register. */
    ret = update_modulus(sec_arg_add_cs(ecc->p, ecc->ksize));
    crypto_chk_func_return(update_modulus, ret);

    /* Step 1: montgomerize the pub_key point */
    mod_p = (drv_pke_data){.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    ret = ecc_ecfp_montgomery_data_aff(sec_arg_add_cs(pub_key, &mod_p, TD_NULL));
    crypto_chk_func_return(ecc_ecfp_montgomery_data_aff, ret);
    /* Step 2: trans Affine coordinate system to Jacobin coordinate system */
    ret = ecc_ecfp_aff_to_jac(TD_NULL, &mod_p, TD_NULL);
    crypto_chk_func_return(ecc_ecfp_aff_to_jac, ret);

    /* Step 3: calculate [n] * P */
    kk = (drv_pke_data){.data = (td_u8 *)ecc->n, .length = ecc->ksize};
    ret = ecc_ecfp_mul_naf_cal((td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE, &kk);
    crypto_chk_func_return(ecc_ecfp_mul_naf_cal, ret);

    /* Step 4: no need to trans result data from jacobin coordinate system to affine coordinate system. */

    /* Step 5: demontgomery data of point.z. */
    ret = ecc_ecfp_demontgomery_data_jac_z(&check_z, (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE);
    crypto_chk_func_return(ecc_ecfp_demontgomery_data_jac_z, ret);

    ret = crypto_drv_pke_common_is_zero(check_z.data, check_z.length);
    if (ret == TD_TRUE) {
        *is_on_curve = TD_TRUE;
    } else {
        *is_on_curve = TD_FALSE;
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}