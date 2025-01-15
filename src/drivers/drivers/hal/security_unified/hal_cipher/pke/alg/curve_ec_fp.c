/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the curve drv common algorithm emplement. The data will transfor by the func param and will be
 * set into DRAM in the algorithms.
 *
 * Create: 2022-10-27
*/

#include "crypto_osal_adapt.h"
#include "rom_lib.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common.h"
#include "drv_pke_inner.h"
#include "drv_common_pke.h"
#include "curve_ec_fp.h"

/************************************************** inner API start************************************/

/************************************************** inner API define start************************************/
/**
 * @brief curve25519 montgomery point multiplication. Rx = k * ux.
 * Before call this API, you should have set montgomery parameter into register, and set rrp by call set_curve_param.
 *
 * @param k the input decoded normal k value.
 * @param ux the input decoded point.
 * @param mod_p the input module.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 curve_ecfp_mont_ladder(const drv_pke_data *k, const drv_pke_data *ux, const drv_pke_data *mod_p);

/**
 * @brief transform X & Z from jacobin coordinate to affine coordinate x.
 * Before call this API, you should have set montgomery parameter into register, and set rrp by call set_curve_param.
 *
 * @param mod_p the modulur.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 curve_ecfp_xz_to_x(const drv_pke_data *mod_p);

/**
 * @brief curve25519 point multiplication parameter check.
 *
 * @param ecc ECC curve parameter.
 * @param k The input data.
 * @param p The input point.
 * @param r The output point.
 * @return td_s32 TD_SUCCESS or others.
 */
CRYPTO_STATIC td_s32 curve_ecfp_mul_dot_param_chk(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r);
/************************************************** inner API define end************************************/
td_s32 curve_ecfp_mont_ladder(const drv_pke_data *k, const drv_pke_data *ux, const drv_pke_data *mod_p)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = mod_p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    td_s32 i = 0;
    td_s32 j = 0;
    td_u8 k_bit = 0;
    const rom_lib *rom_lib_list[2] = {    // 2: select by k_bit
        &instr_curve25519_ladder_0, &instr_curve25519_ladder_1
    };
    td_bool start_flag = TD_FALSE;

    crypto_drv_func_enter();
    /* Step 1: set calculation data into PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_x1, ux->data, ux->length, mod_p->length));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_m, mod_p->data, mod_p->length, mod_p->length));
    /* Step 2: pre process for curve montgomery ladder calculation. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_curve25519_pre_ladder, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: point add + times point calculation to calculate the point multiplication. */
    /*
     * 1) k->data is one byte array, k->length is the byte number.
     * 2) For each byte in k->data, get k_bit = byte[7], byte[6]...byte[0] in order.
     * 3) If there is one k_bit is not zero, then all the next k_bit(contain this time) will call
        crypto_drv_pke_common_batch_instr_process(), the first param depends on k_bit's value.
     */
    for (i = 0; i < (td_s32)k->length; i++) {
        for (j = CRYPTO_BITS_IN_BYTE - 1; j >= 0; j--) {
            k_bit = (k->data[i] >> (td_u32)j) & 1;
            if (start_flag == TD_FALSE && k_bit == 0) {
                continue;
            }
            if ((start_flag == TD_FALSE) && (k_bit != 0)) {
                start_flag = TD_TRUE;
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[k_bit], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
            } else {
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[k_bit], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
            }
        }
    }

    crypto_drv_func_exit();
    return ret;
}

td_s32 curve_ecfp_xz_to_x(const drv_pke_data *mod_p)
{
    td_s32 ret = TD_FAILURE;

    /* Step 1: prepare instructions to use */
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {instr_curve25519_xz2x_pre, instr_curve25519_xz2x_exp_00,
        instr_curve25519_xz2x_exp_01, instr_curve25519_xz2x_exp_10, instr_curve25519_xz2x_exp_11,
        instr_curve25519_xz2x_post};

    /* Step 2: start transfer calculation. */
    ret = crypto_drv_pke_common_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_jac_to_aff_cal, ret);

    return ret;
}

CRYPTO_STATIC td_s32 curve_ecfp_mul_dot_param_chk(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r)
{
    drv_crypto_pke_check_param(ecc == TD_NULL);
    drv_crypto_pke_check_param(ecc->ksize != DRV_PKE_LEN_256 && ecc->ksize != DRV_PKE_LEN_448);
    drv_crypto_pke_check_param(ecc->ecc_type != DRV_PKE_ECC_TYPE_RFC7748 &&
        ecc->ecc_type != DRV_PKE_ECC_TYPE_RFC7748_448);
    drv_crypto_pke_check_param(ecc->gx == TD_NULL);
    drv_crypto_pke_check_param(ecc->n == TD_NULL);
    drv_crypto_pke_check_param(ecc->a == TD_NULL);
    drv_crypto_pke_check_param(ecc->p == TD_NULL);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(ecc->p, ecc->ksize) == TD_TRUE);
    drv_crypto_pke_check_param(k == TD_NULL);
    drv_crypto_pke_check_param(k->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL);
    drv_crypto_pke_check_param(p->x == TD_NULL);
    drv_crypto_pke_check_param(r == TD_NULL);
    drv_crypto_pke_check_param(r->x == TD_NULL);
    drv_crypto_pke_check_param(ecc->ksize != k->length || ecc->ksize != p->length || ecc->ksize != r->length);
    return TD_SUCCESS;
}
/************************************************** inner API end************************************/

/************************************************** outter API start************************************/

/* Decode the private key for X25519 curve */
td_s32 decode_priv_key(td_u8 *output_key, const td_u8 *input_key, td_u32 klen)
{
    td_s32 ret = TD_FAILURE;

    /* Step 1: Do byte reverse on input_key */
    ret = crypto_drv_pke_common_little_big_endian_trans(output_key, input_key, klen);
    ret_enhance_chk(ret, TD_SUCCESS);

    /* Step 2: the priv_key[255], priv_key[254], priv_key[2], priv_key[1], priv_key[0] must be 0,1,0,0,0 */
    if (klen == DRV_PKE_LEN_256) {
        /* curve25519 decode private key. */
        output_key[klen - 1] = output_key[klen - 1] & 0xF8; /* & 248 */
        output_key[0] = output_key[0] & 0x7F;   /* & 127 */
        output_key[0] = output_key[0] | 0x40;   /* | 64 */
    } else {
        /* curve448 decode private key. */
        output_key[klen - 1] = output_key[klen - 1] & 0xFC; /* & 252 */
        output_key[klen - 56] = output_key[0] | 0x80;   /* 56: skip 7 bytes, 0x80: | 128 */
    }
    crypto_dump_buffer("input_key", input_key, klen);
    crypto_dump_buffer("output_key", output_key, klen);

    return ret;
}

td_s32 decode_point_x(td_u8 *output_u_x, const td_u8 *input_u_x, td_u32 ulen)
{
    td_s32 ret = TD_FAILURE;

    /* Step 1: Do byte reverse on input */
    ret = crypto_drv_pke_common_little_big_endian_trans(output_u_x, input_u_x, ulen);
    ret_enhance_chk(ret, TD_SUCCESS);

    /* Step 2: process the last byte value to set it to 0, & 0x7F */
    if (ulen == DRV_PKE_LEN_256) {
        /* curve25519 need further processing. */
        output_u_x[0] &= (1 << (CRYPTO_BITS_IN_BYTE - 1)) - 1;
    }
    crypto_dump_buffer("input_u_x", input_u_x, ulen);
    crypto_dump_buffer("output_u_x", output_u_x, ulen);

    return ret;
}

td_s32 encode_point_x(td_u8 *output_u_x, const td_u8 *input_u_x, td_u32 ulen)
{
    td_s32 ret = TD_FAILURE;

    /* Do byte reverse on input */
    ret = crypto_drv_pke_common_little_big_endian_trans(output_u_x, input_u_x, ulen);
    ret_enhance_chk(ret, TD_SUCCESS);

    return ret;
}

td_s32 curve_ecfp_mul(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_data *ux,
    const drv_pke_data *rx)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    drv_crypto_pke_check_param(ecc == TD_NULL);
    drv_crypto_pke_check_param(ecc->p == TD_NULL);
    /* only support curve25519 & curve448. */
    /* etip: 对于curve448的支持，其LEN不能直接在枚举中添加，直接添加会导致[384, 512]之间的数据对齐到448，未参与正确的ecc计算 */
    drv_crypto_pke_check_param(ecc->ksize != DRV_PKE_LEN_256 && ecc->ksize != DRV_PKE_LEN_448);
    drv_crypto_pke_check_param(ecc->ecc_type != DRV_PKE_ECC_TYPE_RFC7748 &&
        ecc->ecc_type != DRV_PKE_ECC_TYPE_RFC7748_448);
    drv_crypto_pke_check_param(rx->length != ecc->ksize);
    crypto_dump_buffer("decoded k", k->data, k->length);
    crypto_dump_buffer("decoded ux", ux->data, ux->length);

    drv_pke_data mod_p = (drv_pke_data){.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    /* Step 1: set montgomery parameters into register. */
    ret = crypto_drv_pke_common_set_ecc_mont_param(&mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_set_ecc_mont_param, ret);
    /* Step 2: Montgomery Ladder to calculate the point multiplication for curve25519/curve448. */
    ret = curve_ecfp_mont_ladder(k, ux, &mod_p);
    crypto_chk_func_return(curve_ecfp_mont_ladder, ret);
    /* Step 3: Jacobin to Affine coordinate system tranform. */
    ret = curve_ecfp_xz_to_x(&mod_p);
    crypto_chk_func_return(curve_ecfp_xz_to_x, ret);
    /* Step 4: get data result from PKE DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(curve_addr_x2, rx->data, rx->length));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 curve_ecfp_mul_dot(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u8 dec_k[DRV_PKE_LEN_448];
    td_u8 dec_p[DRV_PKE_LEN_448];
    td_u8 temp[DRV_PKE_LEN_448];
    drv_pke_data temp_data = {0};
    drv_pke_data px_data = {0};
    drv_pke_data k_data = {0};
    crypto_drv_func_enter();

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    ret = curve_ecfp_mul_dot_param_chk(ecc, k, p, r);
    crypto_chk_func_return(curve_ecfp_mul_dot_param_chk, ret);

    /* Step 1: set curve initial parameters(a24, etc.) into PKE DRAM for later calculation. */
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 2: The input data need to be decoded */
    memset_enhance_chk_return(ret, dec_k, DRV_PKE_LEN_448, 0x00, DRV_PKE_LEN_448);
    ret = decode_priv_key(dec_k, k->data, k->length);
    crypto_chk_func_return(decode_priv_key, ret);

    /* Step 3: The input point need to be decoded. */
    memset_enhance_chk_return(ret, dec_p, DRV_PKE_LEN_448, 0x00, DRV_PKE_LEN_448);
    ret = decode_point_x(dec_p, p->x, p->length);
    crypto_chk_func_return(decode_point_x, ret);

    /* Step 4: point multiplication */
    temp_data = gen_pke_data(ecc->ksize, temp);
    px_data = gen_pke_data(ecc->ksize, dec_p);
    k_data = gen_pke_data(ecc->ksize, dec_k);
    ret = curve_ecfp_mul(ecc, &k_data, &px_data, &temp_data);
    crypto_chk_func_return(curve_ecfp_mul, ret);

    /* Step 5: check whether the output point x coordinate is all zero. */
    ret = crypto_drv_pke_common_is_zero(temp, ecc->ksize);
    crypto_chk_return((ret == TD_TRUE), TD_FAILURE, "wrong shared dec_k, which is whole zero data.\n");

    /* Step 6: encode the generated point. The output should be byte reversed. */
    ret = encode_point_x(r->x, temp, ecc->ksize);
    crypto_chk_func_return(encode_point_x, ret);

    crypto_drv_func_exit();
    return ret;
}

/************************************************** outter API end************************************/

