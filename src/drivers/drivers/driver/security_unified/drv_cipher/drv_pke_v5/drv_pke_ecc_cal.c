/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc calculate algorithm function implement.
 *
 * Create: 2023-05-24
*/

#include "crypto_osal_adapt.h"
#include "drv_common.h"
#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "ed_ec_fp.h"
#include "curve_ec_fp.h"
#include "drv_pke.h"

td_s32 drv_cipher_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = ecc_ecfp_point_valid_standard(sec_arg_add_cs(curve_type, pub_key, is_on_curve));
    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_mul_dot(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    if (ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC5639_P256 || ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC5639_P384 ||
        ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC5639_P512 || ecc->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P192R ||
        ecc->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P224R || ecc->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P256R ||
        ecc->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P384R || ecc->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P521R) {
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(ecc, k, p, r));
    } else if (ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC7748 || ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
        ret = curve_ecfp_mul_dot(sec_arg_add_cs(ecc, k, p, r));
    } else if (ecc->ecc_type == DRV_PKE_ECC_TYPE_RFC8032) {
        ret = ed_ecfp_mul_naf(sec_arg_add_cs(ecc, k, p, r));
    } else {
        ret = PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    crypto_drv_pke_common_suspend();
    return ret;
}
