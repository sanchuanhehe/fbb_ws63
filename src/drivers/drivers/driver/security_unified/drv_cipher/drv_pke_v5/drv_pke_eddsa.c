/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: the implementation of eddsa.
 *
 * Create: 2023-01-12
*/

#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"
#include "ed_ec_fp.h"
#include "drv_pke.h"

/************************************************** inner function API start************************************/

/************************************************** inner function API define start************************************/

td_s32 pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig CIPHER_CHECK_WORD);

td_s32 pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig CIPHER_CHECK_WORD);

/************************************************** inner function API define end************************************/

td_s32 pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u8 a[DRV_PKE_LEN_256];
    td_u8 r[DRV_PKE_LEN_256];
    td_u8 h[DRV_PKE_LEN_256];
    union {
        td_u8 prefix[DRV_PKE_LEN_256];
        td_u8 Rs[DRV_PKE_LEN_256];
    } data1;
    union {
        td_u8 A[DRV_PKE_LEN_256];
        td_u8 s[DRV_PKE_LEN_256];
    } data2;
    drv_pke_data kk = {0};
    drv_pke_data pp = {0};
    drv_pke_data bb = {0};
    drv_pke_data cc = {0};
    drv_pke_data dd = {0};
    drv_pke_ecc_point gg = {0};
    drv_pke_ecc_point rr = {0};
    td_u32 i = 0;
    drv_pke_data msg_data = {0};
    volatile td_u32 local_cnt = local_step(0);
    td_u8 R_x_tmp[DRV_PKE_LEN_256] = {0};
    const drv_pke_ecc_curve *ed_curve = TD_NULL;

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), curve_type, priv_key, sig);
    drv_crypto_pke_check_param(curve_type != DRV_PKE_ECC_TYPE_RFC8032);
    drv_crypto_pke_check_param(priv_key == TD_NULL);
    drv_crypto_pke_check_param(priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(msg == TD_NULL);
    drv_crypto_pke_check_param(msg->data == TD_NULL);
    drv_crypto_pke_check_param(sig == TD_NULL);
    drv_crypto_pke_check_param(sig->r == TD_NULL);
    drv_crypto_pke_check_param(sig->s == TD_NULL);
    ed_curve = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ed_curve == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");
    drv_crypto_pke_check_param(ed_curve->ksize != priv_key->length);
    drv_crypto_pke_check_param(ed_curve->ksize != sig->length);

    /* Step 0: set initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(ed_curve);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 1: expand private key. a, prefix = secret_expand(secret) */
    ret = memset_enhance(a, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = memset_enhance(data1.prefix, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = TD_FAILURE;
    ret = secret_expand(priv_key->data, a, data1.prefix, DRV_PKE_LEN_256);
    crypto_chk_func_return(secret_expand, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 2: get public key. A = point_compress(point_mul(a, G)) */
    ret = memset_enhance(data2.A, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = TD_FAILURE;
    kk = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = a};
    gg = (drv_pke_ecc_point) {.x = (td_u8 *)ed_curve->gx,
                              .y = (td_u8 *)ed_curve->gy,
                              .length = ed_curve->ksize};
    pp = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)ed_curve->p};
    rr = (drv_pke_ecc_point) {.x = R_x_tmp, .y = data2.A, .length = ed_curve->ksize};
    ret = point_mul_compress(&kk, &gg, &pp, &rr);
    crypto_chk_func_return(point_mul_compress, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 3: r = sha512_modq(prefix + msg) */
    ret = memset_enhance(r, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    dd = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = data1.prefix};
    cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = r};
    pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = (td_u8 *)ed_curve->n};
    msg_data.data = msg->data;
    msg_data.length = msg->length;
    ret = TD_FAILURE;
    ret = sha512_modq(&pp, TD_NULL, &dd, &msg_data, &cc);
    crypto_chk_func_return(sha512_modq, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 4: point multiplication with compress to get Rs. Rs = point_compress(point_mul(r, G)) */
    /* from here, the curve initial parameters should have been set into PKE DRAM. */
    ret = memset_enhance(data1.Rs, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = TD_FAILURE;
    kk = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = r};
    gg = (drv_pke_ecc_point) {.x = (td_u8 *)ed_curve->gx,
                              .y = (td_u8 *)ed_curve->gy,
                              .length = ed_curve->ksize};
    pp = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)ed_curve->p};
    rr = (drv_pke_ecc_point) {.x = R_x_tmp, .y = data1.Rs, .length = ed_curve->ksize};
    ret = point_mul_compress(&kk, &gg, &pp, &rr);
    crypto_chk_func_return(point_mul_compress, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 5: h = sha512_modq(Rs + A + msg) */
    ret = memset_enhance(h, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    kk = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = data1.Rs};
    dd = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = data2.A};
    cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = h};
    pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = (td_u8 *)ed_curve->n};
    ret = TD_FAILURE;
    ret = sha512_modq(&pp, &kk, &dd, &msg_data, &cc);
    crypto_chk_func_return(sha512_modq, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 6: s = (r + h * a) % q */
    ret = TD_FAILURE;
    kk = (drv_pke_data) {.data = a, .length = DRV_PKE_LEN_256};
    pp = (drv_pke_data) {.data = (td_u8 *)ed_curve->n, .length = ed_curve->ksize};
    bb = (drv_pke_data) {.data = r, .length = DRV_PKE_LEN_256};
    cc = (drv_pke_data) {.data = h, .length = DRV_PKE_LEN_256};
    dd = (drv_pke_data) {.data = data2.s, .length = DRV_PKE_LEN_256};
    ret = ed_ecfn_sign_s(&kk, &bb, &cc, &pp, &dd);
    crypto_chk_func_return(ed_ecfn_sign_s, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 7: return sig = (Rs, int.to_bytes(s, 32, "little")) */
    ret = TD_FAILURE;
    ret = memcpy_enhance(sig->r, DRV_PKE_LEN_256, data1.Rs, DRV_PKE_LEN_256);
    crypto_chk_func_return(memcpy_enhance, ret);
    for (i = 0; i < DRV_PKE_LEN_256; i++) {
        sig->s[i] = data2.s[DRV_PKE_LEN_256 - 1 - i];
    }

    val_enhance_chk(local_cnt, local_step(6));  /* whole need 6 steps. */

    return TD_SUCCESS;
}

td_s32 pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 work_len = 0;
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, pub_key, msg, sig);
    td_u8 Ax[DRV_PKE_LEN_256];  /* store the decoded public key. */
    td_u8 Ay[DRV_PKE_LEN_256];  /* store the decoded public key. */
    td_u8 Rx[DRV_PKE_LEN_256];  /* store the decoded R point */
    td_u8 Ry[DRV_PKE_LEN_256];  /* store the decoded R point */
    td_u8 Cx[DRV_PKE_LEN_256];
    td_u8 Cy[DRV_PKE_LEN_256];
    td_u8 s[DRV_PKE_LEN_256];   /* store the real signature value. */
    td_u8 h[DRV_PKE_LEN_256];   /* store the sha512 result. */
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data pp = {0};
    drv_pke_data cc = {0};
    drv_pke_ecc_point A = {TD_NULL};    /* store the decoded public key. */
    drv_pke_ecc_point R = {TD_NULL};    /* store the decoded R point */
    td_u32 i = 0;
    drv_pke_data msg_data = {0};
    td_u32 local_cnt = local_step(0);
    const drv_pke_ecc_curve *ed_curve = TD_NULL;

    drv_crypto_pke_check_param(curve_type != DRV_PKE_ECC_TYPE_RFC8032);
    drv_crypto_pke_check_param(pub_key == TD_NULL);
    drv_crypto_pke_check_param(pub_key->y == TD_NULL);
    drv_crypto_pke_check_param(msg == TD_NULL);
    drv_crypto_pke_check_param(msg->data == TD_NULL);
    drv_crypto_pke_check_param(sig == TD_NULL);
    drv_crypto_pke_check_param(sig->r == TD_NULL);
    drv_crypto_pke_check_param(sig->s == TD_NULL);
    ed_curve = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ed_curve == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");
    drv_crypto_pke_check_param(ed_curve->ksize != pub_key->length);
    drv_crypto_pke_check_param(ed_curve->ksize != sig->length);

    /* Step 1: check whether s is [0, n - 1] */
    /* decode s, s = int.from_bytes(signature[32:], "little") */
    for (i = 0; i < DRV_PKE_LEN_256; i++) {
        s[i] = sig->s[DRV_PKE_LEN_256 - 1 - i];
    }
    ret = crypto_drv_pke_common_limit_value_check(s, ed_curve->n, ed_curve->ksize);
    crypto_chk_func_return(crypto_drv_pke_common_limit_value_check, ret);

    /* Step 2: set initial parameters into DRAM */
    ret = crypto_drv_pke_common_init_param(ed_curve);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 3: decode public key, A = point_decompress(public) */
    ret = memset_enhance(Ax, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = memset_enhance(Ay, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    A = (drv_pke_ecc_point) {.length = ed_curve->ksize, .x = Ax, .y = Ay};
    aa = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)pub_key->y};
    pp = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)ed_curve->p};
    ret = TD_FAILURE;
    ret = point_decompress(&aa, &pp, &A);
    crypto_chk_func_return(point_decompress, ret);
    work_len = (td_u32)ed_curve->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE;
    /* copy data from ed_addr_tx0, ed_addr_ty to ed_addr_gx, ed_addr_gy for Step 5. */
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 4: calculate h, h = sha512_modq(Rs + public + msg). */
    /* Step 4.1: decode Rs as R, Rs = signature[:32], R = point_decompress(Rs) */
    ret = memset_enhance(Rx, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    ret = memset_enhance(Ry, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    R = (drv_pke_ecc_point) {.length = sig->length, .x = Rx, .y = Ry};
    aa = (drv_pke_data) {.length = sig->length, .data = sig->r};
    pp = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)ed_curve->p};
    ret = TD_FAILURE;
    ret = point_decompress(&aa, &pp, &R);
    crypto_chk_func_return(point_decompress, ret);
    /* copy data from ed_addr_tx0, ed_addr_ty to ed_addr_rx, ed_addr_ry for Step 5. */
    local_cnt += LOCAL_STEP_AUTH;
    /* Step 4.2: h = sha512_modq(Rs + public + msg) */
    ret = memset_enhance(h, DRV_PKE_LEN_256, 0x00, DRV_PKE_LEN_256);
    crypto_chk_func_return(memset_enhance, ret);
    aa = (drv_pke_data) {.length = sig->length, .data = sig->r};
    bb = (drv_pke_data) {.length = pub_key->length, .data = pub_key->y};
    cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = h};
    pp = (drv_pke_data) {.length = ed_curve->ksize, .data = (td_u8 *)ed_curve->n};
    msg_data.data = msg->data;
    msg_data.length = msg->length;
    ret = TD_FAILURE;
    ret = sha512_modq(&pp, &aa, &bb, &msg_data, &cc);
    crypto_chk_func_return(sha512_modq, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 5: check whether sB = R + hA. */
    /* Step 5.1: calculate C = sB - hA, sB = point_mul(s, G). */
    aa = (drv_pke_data) {.length = sig->length, .data = s};
    bb = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = h};
    ret = TD_FAILURE;
    ret = ed_mul_sub(ed_curve, &aa, &bb, &A, TD_NULL);
    crypto_chk_func_return(ed_mul_sub, ret);
    local_cnt += LOCAL_STEP_AUTH;
    /* Step 5.2: check whether C is equal to R. */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_rx, Rx, sig->length, ed_curve->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ry, Ry, sig->length, ed_curve->ksize));
    ret = TD_FAILURE;
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_c_eq_r, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cx, Cx, sig->length));
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cy, Cy, sig->length));
    if (crypto_drv_pke_common_is_zero(Cx, sig->length) == TD_TRUE &&
        crypto_drv_pke_common_is_zero(Cy, sig->length) == TD_TRUE) {
        local_cnt += LOCAL_STEP_AUTH;
    } else {
        return  TD_FAILURE;
    }

    val_enhance_chk(local_cnt, local_step(5));  /* total is 5 step. */

    return TD_SUCCESS;
}

/************************************************** inner function API end************************************/

td_s32 drv_cipher_pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = pke_eddsa_sign(sec_arg_add_cs(curve_type, priv_key, msg, sig));
    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = pke_eddsa_verify(sec_arg_add_cs(curve_type, pub_key, msg, sig));
    crypto_drv_pke_common_suspend();
    return ret;
}
