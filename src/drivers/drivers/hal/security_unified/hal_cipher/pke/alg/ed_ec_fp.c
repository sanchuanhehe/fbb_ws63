/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ed25519 related functions complement.
 *
 * Create: 2022-10-31
*/

#include "crypto_osal_adapt.h"
#include "crypto_drv_common.h"
#include "drv_hash.h"
#include "rom_lib.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"
#include "ed_ec_fp.h"

/************************************************** inner API start************************************/
/* point compress, from real point get little endian data. */
td_s32 point_compress(const drv_pke_ecc_point *p, const drv_pke_data *enc_p);

/**
 * @brief Get the binary data object. And if the flag is 1, the output bit length should be even.
 * If not, the high address will be padding with zero.
 * @param mod_p the input data.
 * @param mod_p_bin the output binary data, which is from low bit to high bit [0->bin_len].
 * @param mod_p_bin_len the output binary length.
 * @param flag whether the mod_p_bin_len need to be even data, 1: even data, 0: no restriction.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_get_binary_data(const td_u8 *mod_p, const td_u32 mod_p_len, td_u8 *mod_p_bin,
    td_u32 *mod_p_bin_len, td_u32 flag);

/* recovery x coordinate from the y coordinate by curve equation. Before call this API, you should have set curve
initial parameters into the DRAM. */
td_s32 recovery_x(const td_u8 *y, const td_u8 *p, td_u32 byte_len, td_u8 sign, td_u8 *x);

/* transfer coordinate from extended coordinate to affine coordinate. */
td_s32 ed_ecfp_xyz_to_xy(const drv_pke_data *mod_p);

/* modulo operation for ed25519/ed448. */
td_s32 ed_mod(const drv_pke_data *in, const drv_pke_data *p, const drv_pke_data *out);

td_s32 secret_expand(const td_u8 *secret, td_u8 *a, td_u8 *prefix, const td_u32 len)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u8 h[DRV_PKE_LEN_512];
    drv_pke_data arr[1];
    drv_pke_data hash_data = {DRV_PKE_LEN_512, h};

    drv_crypto_pke_check_param(secret == TD_NULL);
    drv_crypto_pke_check_param(a == TD_NULL);
    drv_crypto_pke_check_param(prefix == TD_NULL);
    drv_crypto_pke_check_param(len != DRV_PKE_LEN_256);
    arr[0].data = (td_u8 *)secret;
    arr[0].length = len;
    ret = crypto_drv_pke_common_calc_hash(arr, 1, DRV_PKE_HASH_TYPE_SHA512, &hash_data);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_calc_hash failed, ret is 0x%x\n", ret);

    for (i = 0; i < DRV_PKE_LEN_256; i++) {
        a[i] = h[DRV_PKE_LEN_256 - 1 - i];
    }
    val_enhance_chk(i, DRV_PKE_LEN_256);
    /* [255], [254], [2], [1], [0] must be 0,1,0,0,0 */
    a[DRV_PKE_LEN_256 - 1] &= 0xF8;
    a[0] &= 0x7F;
    a[0] |= 0x40;

    ret = memcpy_enhance(prefix, DRV_PKE_LEN_256, h + DRV_PKE_LEN_256, DRV_PKE_LEN_256);
    crypto_chk_func_return(memcpy_enhance, ret);

    return TD_SUCCESS;
}

td_s32 point_compress(const drv_pke_ecc_point *p, const drv_pke_data *enc_p)
{
    td_s32 ret = TD_FAILURE;
    drv_crypto_pke_check_param(p->length != enc_p->length);
    drv_crypto_pke_check_param(enc_p->length != DRV_PKE_LEN_256);

    /* Step 1: trans to little-endian string. */
    ret = crypto_drv_pke_common_little_big_endian_trans(enc_p->data, p->y, p->length);
    crypto_chk_func_return(crypto_drv_pke_common_little_big_endian_trans, ret);

    /* Step 2: copy the least significant bit of the x-coordinate to the most significant bit of the final octet. */
    enc_p->data[enc_p->length - 1] |= (p->x[p->length - 1] & 0x01) << 7; /* 7: get least significant bit. */

    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_get_binary_data(const td_u8 *mod_p, const td_u32 mod_p_len, td_u8 *mod_p_bin,
    td_u32 *mod_p_bin_len, td_u32 flag)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 cnt = 0;
    td_u8 *tmp = crypto_malloc(mod_p_len);
    crypto_drv_func_enter();
    crypto_chk_return((tmp == TD_NULL), ret, "malloc failed!\n");
    ret = memcpy_enhance(tmp, mod_p_len, mod_p, mod_p_len);
    crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "copy data failed!\n");

    for (; (i < *mod_p_bin_len) && (crypto_drv_pke_common_is_zero(tmp, mod_p_len) != TD_TRUE); i++) {
        mod_p_bin[i] = tmp[mod_p_len - 1] % 2;  /* 2: get the last bit. */
        crypto_drv_pke_common_arry_right_shift_value(tmp, mod_p_len, 1);  /* 1: right shift one. */
        cnt++;
    }

    if (flag == 1) {
        if (cnt % 2 != 0) { /* 2: odd or even. */
            mod_p_bin[cnt] = 0x00; /* set 0 to high address. */
            cnt++;
        }
    }
    *mod_p_bin_len = cnt;
    crypto_dump_buffer("mod_p_bin", mod_p_bin, *mod_p_bin_len);

__EXIT:
    if (tmp != TD_NULL) {
        crypto_free(tmp);
        tmp = TD_NULL;
    }

    crypto_drv_func_exit();
    return ret;
}

td_s32 recovery_x(const td_u8 *y, const td_u8 *p, td_u32 byte_len, td_u8 sign, td_u8 *x)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    td_u8 t0[DRV_PKE_LEN_256];
    td_u8 t1[DRV_PKE_LEN_256];
    td_u8 mod_p_5[DRV_PKE_LEN_256];
    td_u8 mod_p_bin[DRV_PKE_LEN_256 * CRYPTO_BITS_IN_BYTE + 1];
    /* get work_len */
    td_u32 work_len = byte_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    drv_pke_data mod_p = {0};
    crypto_drv_func_enter();

    /* 1. set necessary data into DRAM, which should have been set by former process. */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ty, y, byte_len, byte_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, p, byte_len, byte_len));
    /* Step 1.2 set montgomery parameters into register. */
    mod_p = (drv_pke_data) {.data = (td_u8 *)p, .length = byte_len};
    ret = crypto_drv_pke_common_set_mont_param(&mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);

    /* 2. start recovery x from y by curve equation. x^2 = (y^2 - 1) / (d*y^2 + 1) (mod p) */
    /* 2.1 preprocess, initialize the data, use batch process step. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_recover_x_pre, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 2.2 expand (p-5)//8's bit length to 2n, and calculate from high bit */
    /* get p - 5 */
    ret = memcpy_enhance(mod_p_5, DRV_PKE_LEN_256, p, byte_len);
    crypto_chk_return((ret != TD_SUCCESS), ret, "memcpy_enhance failed!\n");
    crypto_drv_pke_common_array_sub_value(mod_p_5, byte_len, 5);    /* 5: sub value. */
    td_u32 mod_p_bin_len = byte_len * CRYPTO_BITS_IN_BYTE + 1;
    /* get (p-5)//8 */
    crypto_drv_pke_common_arry_right_shift_value(mod_p_5, byte_len, 3);   /* 8 = 2^3. */
    /* get (p-5)//8's bit form. */
    ret = crypto_drv_pke_common_get_binary_data(mod_p_5, byte_len, mod_p_bin, &mod_p_bin_len, NEED_PAD_ZERO);
    crypto_chk_return((ret != TD_SUCCESS), ret, "get binary data failed!\n");

    /* calculate by binary data, from high address to low address. */
    for (i = (td_s32)mod_p_bin_len - 1; i >= 0; i -= 2) {   /* 2: offset value */
        if (mod_p_bin[i] == 0x00 && mod_p_bin[i - 1] == 0x00) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_recover_x_exp_00, work_len));
        } else if (mod_p_bin[i] == 0x00 && mod_p_bin[i - 1] == 0x01) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_recover_x_exp_01, work_len));
        } else if (mod_p_bin[i] == 0x01 && mod_p_bin[i - 1] == 0x00) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_recover_x_exp_10, work_len));
        } else {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_recover_x_exp_11, work_len));
        }
        crypto_chk_func_return(hal_pke_set_mode, ret);
        ret = hal_pke_start(sec_arg_add_cs(PKE_BATCH_INSTR));
        crypto_chk_func_return(hal_pke_start, ret);
        ret = hal_pke_wait_done();
        crypto_chk_func_return(hal_pke_wait_done, ret);
    }

    /* 2.3 postprocess, demontgomery and complete reduction */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_recover_x_post, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 3. get result from DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_t0, t0, byte_len));
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_t1, t1, byte_len));
    /* u = (y^2 - 1), y = (d*y^2 + 1), there are three cases. */
    if (crypto_drv_pke_common_is_zero(t0, byte_len) == TD_TRUE) {
        /* if v * x^2 = u (mod p), x is a square root. */
        hal_pke_get_ram(sec_arg_add_cs(ed_addr_tx0, x, byte_len));
        ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_cpy_tx0_to_tx1, work_len);
    } else if (crypto_drv_pke_common_is_zero(t1, byte_len) == TD_TRUE) {
        /* If v x^2 = -u (mod p), set x <-- x * 2^((p-1)/4), which is a square root. */
        hal_pke_get_ram(sec_arg_add_cs(ed_addr_tx1, x, byte_len));
        ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_cpy_tx1_to_tx0, work_len);
    } else {
        /* Otherwise, no square root exists for modulo p, and decoding fails. */
        return TD_FAILURE;
    }
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    /* Finally, use the x_0 bit to select the right square root. */
    if (crypto_drv_pke_common_is_zero(x, byte_len) == TD_TRUE && sign == 0x01) {
        /* If x = 0, and x_0 = 1, decoding fails. */
        return TD_FAILURE;
    } else if ((x[byte_len - 1] & 0x01) != sign) {
        /* if x_0 != x mod 2, set x <-- p - x. */
        ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_cpy_tx1_to_tx0, work_len);
        crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
        hal_pke_get_ram(sec_arg_add_cs(ed_addr_tx0, x, byte_len));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 point_decompress(const drv_pke_data *enc_pkey, const drv_pke_data *p, const drv_pke_ecc_point *pkey)
{
    td_s32 ret = TD_FAILURE;
    td_u8 sign = 0; /* store the symbol to select the right x coordinate result. */

    /* Step 1: get y coordinate. */
    ret = crypto_drv_pke_common_little_big_endian_trans(pkey->y, enc_pkey->data, enc_pkey->length);
    crypto_chk_func_return(crypto_drv_pke_common_little_big_endian_trans, ret);
    /* Bit 255 of this number is the least significant bit of the x-coordinate and denote this value x_0 */
    sign = pkey->y[0] >> 7; /* 7: get least significant bit. */
    /* The y-coordinate is recovered simply by clearing the bit 255. */
    pkey->y[0] &= 0x7F;
    /* If the resulting value pkey->y is >= p, decoding fails. */
    ret = crypto_drv_pke_common_limit_value_check(pkey->y, p->data, p->length);
    crypto_chk_func_return(crypto_drv_pke_common_limit_value_check, ret);

    /* Step 2: recovery x coordinate from the y coordinate. */
    ret = recovery_x(pkey->y, p->data, p->length, sign, pkey->x);
    crypto_chk_func_return(recovery_x, ret);

    return ret;
}

td_s32 ed_ecfp_xyz_to_xy(const drv_pke_data *mod_p)
{
    td_s32 ret = TD_FAILURE;

    /* prepare batch instructions. */
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {instr_ed_ecfp_xyz2xy_pre, instr_ed_ecfp_xyz2xy_exp_00,
        instr_ed_ecfp_xyz2xy_exp_01, instr_ed_ecfp_xyz2xy_exp_10, instr_ed_ecfp_xyz2xy_exp_11,
        instr_ed_ecfp_xyz2xy_post};

    ret = crypto_drv_pke_common_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_jac_to_aff_cal, ret);

    return ret;
}

td_s32 ed_mod(const drv_pke_data *in, const drv_pke_data *p, const drv_pke_data *out)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step 1: set data into DRAM */
    crypto_dump_buffer("ed_mod in", in->data, in->length);
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_t1, in->data, DRV_PKE_LEN_256, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_t0, in->data + DRV_PKE_LEN_256, DRV_PKE_LEN_256, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, p->data, p->length, p->length));
    /* Step 2.2 set montgomery parameters into register. */
    ret = crypto_drv_pke_common_set_mont_param(p);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);

    /* Step 2: batch process to start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_calc_h, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: get data out from the DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_h, out->data, out->length));

    return TD_SUCCESS;
}

/************************************************** inner API end************************************/

/************************************************** outter API start************************************/
td_s32 ed_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 work_len = 0;
    td_u8 out_x[DRV_PKE_LEN_576] = {0};
    td_u8 out_y[DRV_PKE_LEN_576] = {0};
    drv_pke_data mod_p = {0};
    drv_pke_ecc_point out = {0};
    drv_pke_data enc_pkey = {0};

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    drv_crypto_pke_check_param(ecc == TD_NULL);
    drv_crypto_pke_check_param(ecc->gx == TD_NULL);
    drv_crypto_pke_check_param(ecc->gy == TD_NULL);
    drv_crypto_pke_check_param(ecc->n == TD_NULL);
    drv_crypto_pke_check_param(ecc->a == TD_NULL);
    drv_crypto_pke_check_param(ecc->p == TD_NULL);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(ecc->p, ecc->ksize) == TD_TRUE);
    drv_crypto_pke_check_param(k == TD_NULL);
    drv_crypto_pke_check_param(k->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL);
    /* the calculation use decompressed point, for x couldn't be NULL. */
    drv_crypto_pke_check_param(p->x == TD_NULL);
    drv_crypto_pke_check_param(p->y == TD_NULL);
    drv_crypto_pke_check_param(r == TD_NULL);
    drv_crypto_pke_check_param(r->y == TD_NULL);
    drv_crypto_pke_check_param(ecc->ksize != k->length || ecc->ksize != p->length || ecc->ksize != r->length);
    drv_crypto_pke_check_param(ecc->ksize != DRV_PKE_LEN_256);

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    /* Step 1: set curve initial parameters into PKE DRAM. */
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);
    /* Step 2.1: set point data into PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ax, p->x, p->length, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ay, p->y, p->length, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, ecc->p, ecc->ksize, ecc->ksize));
    /* Step 2.2 set montgomery parameters into register. */
    mod_p = (drv_pke_data) {.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    ret = crypto_drv_pke_common_set_mont_param(&mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);
    /* Step 2.3: start multiplication calculate. */
    work_len = (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE;
    ret = ed_ecfp_mul_naf_cal(work_len, k);
    crypto_chk_func_return(ed_ecfp_mul_naf_cal, ret);

    /* Step 3: coordinate transform, from extend coordinate to affine coordinate */
    ret = ed_ecfp_xyz_to_xy(&mod_p);
    crypto_chk_func_return(ed_ecfp_xyz_to_xy, ret);

    /* Step 4: get data out from the PKE DRAM to out. */
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cx, out_x, DRV_PKE_LEN_256));
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cy, out_y, DRV_PKE_LEN_256));

    /* Step 5: point compress to output the point to user. */
    enc_pkey = (drv_pke_data) {.data = r->y, .length = r->length};
    out = (drv_pke_ecc_point) {.x = out_x, .y = out_y, .length = r->length};
    ret = point_compress(&out, &enc_pkey);
    crypto_chk_func_return(point_compress, ret);
    if (r->x != TD_NULL) {
        memcpy_enhance_chk_return(ret, r->x, r->length, out_x, out.length);
    }

    return ret;
}

/* etip: 此函数必须保留，以保证curve/ecc/ed统一点乘入口之后可以直接在外部仅根据曲线类型选择调用对应函数？ */
td_s32 ed_ecfp_mul_naf_cal(td_u32 work_len, const drv_pke_data *k)
{
    volatile td_s32 ret = TD_FAILURE;

    /* Step 1: data montgomerize and data copy. the point and the initial parameters should have been set. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_mul_a_pre, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 2: NAF point multiplication. */
    rom_lib batch_instr_block[POINT_NAF_INSTR_NUM] = {instr_ed25519_mul_a_loop_1, instr_ed25519_mul_p_loop_1,
        instr_ed25519_mul_loop_0};
    ret = crypto_drv_pke_common_point_mul_naf(batch_instr_block, POINT_NAF_INSTR_NUM, k, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_point_mul_naf, ret);

    return ret;
}

td_s32 point_mul_compress(const drv_pke_data *k, const drv_pke_ecc_point *u, const drv_pke_data *p,
    const drv_pke_ecc_point *r)
{
    td_s32 ret = TD_FAILURE;
    td_u8 out_x[DRV_PKE_LEN_576] = {0};
    td_u8 out_y[DRV_PKE_LEN_576] = {0};
    drv_pke_ecc_point out = {0};
    drv_pke_data enc_pkey = {0};
    td_u32 work_len = p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    const drv_pke_ecc_curve *ed_curve = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_RFC8032);
    crypto_chk_return(ed_curve == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    /* Step 1: set curve initial parameters into PKE DRAM. */
    ret = crypto_drv_pke_common_init_param(ed_curve);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);
    /* Step 2.1: set point data into PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ax, u->x, u->length, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_ay, u->y, u->length, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, p->data, p->length, p->length));
    /* Step 2.2 set montgomery parameters into register. */
    ret = crypto_drv_pke_common_set_mont_param(p);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);
    /* Step 2.3: start multiplication calculate. */
    ret = ed_ecfp_mul_naf_cal(work_len, k);
    crypto_chk_func_return(ed_ecfp_mul_naf_cal, ret);

    /* Step 3: coordinate transform, from extend coordinate to affine coordinate */
    ret = ed_ecfp_xyz_to_xy(p);
    crypto_chk_func_return(ed_ecfp_xyz_to_xy, ret);

    /* Step 4: get data out from the PKE DRAM to out. */
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cx, out_x, DRV_PKE_LEN_256));
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_cy, out_y, DRV_PKE_LEN_256));

    /* Step 5: point compress to output the point to user. */
    enc_pkey = (drv_pke_data) {.data = r->y, .length = r->length};
    out = (drv_pke_ecc_point) {.x = out_x, .y = out_y, .length =  r->length};
    ret = point_compress(&out, &enc_pkey);
    crypto_chk_func_return(point_compress, ret);
    if (r->x != TD_NULL) {
        memcpy_enhance_chk_return(ret, r->x, r->length, out_x, out.length);
    }

    return ret;
}

td_s32 ed_secret_to_public(const drv_pke_ecc_curve *ecc, const drv_pke_data *priv_key,
    const drv_pke_ecc_point *pub_key)
{
    td_s32 ret = TD_FAILURE;
    td_u8 a[DRV_PKE_LEN_256];
    td_u8 dummy[DRV_PKE_LEN_256];
    drv_pke_data mod_p = (drv_pke_data) {.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    drv_pke_data skey = (drv_pke_data) {.data = a, .length = ecc->ksize};
    drv_pke_ecc_point g_point = \
        (drv_pke_ecc_point) {.x = (td_u8 *)ecc->gx, .y = (td_u8 *)ecc->gy, .length = ecc->ksize};
    drv_pke_ecc_point pkey = (drv_pke_ecc_point) {.x = dummy, .y = (td_u8 *)pub_key->y, .length = pub_key->length};

    /* inner function, no need to check parameters. */
    /* Step 1: expand private key, a, prefix = secret_expand(secret) */
    ret = secret_expand(priv_key->data, a, dummy, priv_key->length);
    crypto_chk_func_return(secret_expand, ret);

    /* Step 2: point multiplication to calculate public key. */
    ret = point_mul_compress(&skey, &g_point, &mod_p, &pkey);
    crypto_chk_func_return(point_mul_compress, ret);

    return TD_SUCCESS;
}

td_s32 sha512_modq(const drv_pke_data *q, const drv_pke_data *in1, const drv_pke_data *in2,
    const drv_pke_data *in3, const drv_pke_data *out)
{
    td_s32 ret = TD_FAILURE;
    drv_pke_data aa = {0};
    drv_pke_data pp = {0};
    td_u8 h[DRV_PKE_LEN_512];
    td_u8 tmp[DRV_PKE_LEN_512];
    td_u32 h_len = DRV_PKE_LEN_512;
    drv_pke_data hash = {0};
    hash.length = h_len;
    hash.data = tmp;
    crypto_drv_func_enter();

    drv_crypto_pke_check_param(q == TD_NULL);
    drv_crypto_pke_check_param(q->data == TD_NULL);
    drv_crypto_pke_check_param(q->length != DRV_PKE_LEN_256);
    drv_crypto_pke_check_param(out == TD_NULL);
    drv_crypto_pke_check_param(out->data == TD_NULL);
    drv_crypto_pke_check_param(out->length != DRV_PKE_LEN_256);

    /* *** H = Hash(in1 || in2 || in3) *** */
    drv_pke_data arr[ED25519_MAX_HASH_ELEMENTS];
    drv_pke_data *parr = arr;
    td_u32 index = 0;
    if ((in1 != TD_NULL) && (in1->data != TD_NULL)) {
        arr[index].data = in1->data;
        arr[index].length = in1->length;
        index++;
    }
    if ((in2 != TD_NULL) && (in2->data != TD_NULL)) {
        arr[index].data = in2->data;
        arr[index].length = in2->length;
        index++;
    }
    if ((in3 != TD_NULL) && (in3->data != TD_NULL)) {
        arr[index].data = in3->data;
        arr[index].length = in3->length;
        index++;
    }
    ret = crypto_drv_pke_common_calc_hash(parr, index, DRV_PKE_HASH_TYPE_SHA512, &hash);
    crypto_chk_func_return(crypto_drv_pke_common_calc_hash, ret);

    crypto_dump_buffer("etip hash output", tmp, h_len);
    /* Interpret the 64-octet digest as a little-endian integer r */
    ret = crypto_drv_pke_common_little_big_endian_trans(h, tmp, DRV_PKE_LEN_512);
    crypto_chk_func_return(crypto_drv_pke_common_little_big_endian_trans, ret);

    /* For efficiency, do this by first reducing r modulo L, the group order of B. */
    aa = (drv_pke_data) {.length = DRV_PKE_LEN_512, .data = h};
    pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = q->data};
    ret = ed_mod(&aa, &pp, out);
    crypto_chk_func_return(ed_mod, ret);
    crypto_dump_buffer("etip hash output mod data", out->data, out->length);

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ed_ecfn_sign_s(const drv_pke_data *sk, const drv_pke_data *r, const drv_pke_data *h,
    const drv_pke_data *n, const drv_pke_data *s)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = n->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    crypto_drv_func_enter();

    /* Step 1.1: set data into DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_sk, sk->data, sk->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_h, h->data, h->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_r, r->data, r->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, n->data, n->length, n->length));

    /* Step 1.2 set montgomery parameters into register. */
    ret = crypto_drv_pke_common_set_mont_param(n);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);
    /* Step 2: batch process to start the calculation. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_calc_s, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: get data out from the DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(ed_addr_s, s->data, s->length));

    crypto_drv_func_exit();
    return ret;
}

td_s32 ed_mul_sub(const drv_pke_ecc_curve *ecc, const drv_pke_data *s, const drv_pke_data *h,
    const drv_pke_ecc_point *a, const drv_pke_ecc_point *r)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    td_u32 work_len = (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE;
    td_u8 s_bin [DRV_PKE_LEN_256 * CRYPTO_BITS_IN_BYTE + 1];
    td_u8 h_bin [DRV_PKE_LEN_256 * CRYPTO_BITS_IN_BYTE + 1];
    td_u32 bin_max_len = DRV_PKE_LEN_256 * CRYPTO_BITS_IN_BYTE + 1;
    td_u32 s_bin_len = s->length * CRYPTO_BITS_IN_BYTE;
    td_u32 h_bin_len = h->length * CRYPTO_BITS_IN_BYTE;
    td_u32 bin_len = 0;
    drv_pke_data mod_p = {0};
    crypto_drv_func_enter();

    /* Step 1.1: set data into DRAM */
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_px, ecc->gx, ecc->ksize, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_py, ecc->gy, ecc->ksize, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_gx, a->x, a->length, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_gy, a->y, a->length, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_m, ecc->p, ecc->ksize, ecc->ksize));
    /* Step 1.2 set montgomery parameters into register. */
    mod_p = (drv_pke_data) {.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    ret = crypto_drv_pke_common_set_mont_param(&mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);

    /* Step 2: data preparation, include data montgomery and data copy. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ed25519_mul_sub_pre, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: transfer data into binary format. */
    ret = crypto_drv_pke_common_get_binary_data(s->data, s->length, s_bin, &s_bin_len, NO_NEED_PAD_ZERO);
    crypto_chk_func_return(crypto_drv_pke_common_get_binary_data, ret);
    ret = crypto_drv_pke_common_get_binary_data(h->data, h->length, h_bin, &h_bin_len, NO_NEED_PAD_ZERO);
    crypto_chk_func_return(crypto_drv_pke_common_get_binary_data, ret);
    bin_len = s_bin_len > h_bin_len ? s_bin_len : h_bin_len;
    if (bin_len > s_bin_len) {
        ret = memset_enhance(s_bin + s_bin_len, bin_max_len - s_bin_len, 0x00, bin_len - s_bin_len);
        crypto_chk_func_return(memset_enhance, ret);
    }
    if (bin_len > h_bin_len) {
        ret = memset_enhance(h_bin + h_bin_len, bin_max_len - h_bin_len, 0x00, bin_len - h_bin_len);
        crypto_chk_func_return(memset_enhance, ret);
    }

    /* Step 4: start calculate. */
    for (i = (td_s32)bin_len - 1; i >= 0; i--) {
        if (s_bin[i] == 0x01 && h_bin[i] == 0x01) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_mul_a_loop_1, work_len));
        } else if (s_bin[i] == 0x00 && h_bin[i] == 0x01) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_mul_g_loop_1, work_len));
        } else if (s_bin[i] == 0x01 && h_bin[i] == 0x00) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_mul_p_loop_1, work_len));
        } else {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &instr_ed25519_mul_loop_0, work_len));
        }
        crypto_chk_func_return(hal_pke_set_mode, ret);
        ret = hal_pke_start(sec_arg_add_cs(PKE_BATCH_INSTR));
        crypto_chk_func_return(hal_pke_start, ret);
        ret = hal_pke_wait_done();
        crypto_chk_func_return(hal_pke_wait_done, ret);
    }

    /* Step 5: get result out for user if this is the outter API, need Jac_to_aff and demontgomery. */
    /* here the output data is montgomeried in cx,cy,cz. */
    if (r != TD_NULL && r->x != TD_NULL && r->y != TD_NULL && r->length == a->length) {
        /* coordinate transform, from extend coordinate to affine coordinate */
        ret = ed_ecfp_xyz_to_xy(&mod_p);
        crypto_chk_func_return(ed_ecfp_xyz_to_xy, ret);
        /* the out data is in cx,cy. */
        hal_pke_get_ram(sec_arg_add_cs(ed_addr_cx, r->x, r->length));
        hal_pke_get_ram(sec_arg_add_cs(ed_addr_cy, r->y, r->length));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}
/************************************************** outter API end************************************/
