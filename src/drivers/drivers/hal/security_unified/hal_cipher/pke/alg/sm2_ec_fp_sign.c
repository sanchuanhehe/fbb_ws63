/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 sign algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

/* for inner calculate, in fact, all the input parameters has been set into the DRAM when calculate r,
except private key. */
td_s32 sm2_ecfn_sign_s(const drv_pke_data *k, const drv_pke_data *d, const drv_pke_data *n,
    const drv_pke_data *r, const drv_pke_data *s)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = n->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    td_u8 k_inv[DRV_PKE_LEN_576] = {0};
    drv_pke_data aa = {0};
    crypto_drv_func_enter();

    /* Step 1: set data into DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_k, k->data, k->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_d, d->data, d->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_r, r->data, r->length, n->length));
    /* set module, the addr is ecc_addr_m */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, n->data, n->length, n->length));

    /* Step 2: start calculate. montgomery the data, and calculate (1 + dA) & (k - r*dA). */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_sm2_sign_s_pre_6, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    /* Step 2.1 get data from the DRAM. 1+da from ecc_addr_k, k-r*da from ecc_addr_e */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_k, k_inv, n->length));

    /* Step 3: calculate the (1+da)^(-1). */
    aa = (drv_pke_data) {.data = k_inv, .length = n->length};
    ret = ecc_ecfn_inv(sec_arg_add_cs(&aa, n, s));
    crypto_chk_func_return(ecc_ecfn_inv, ret);

    /* Step 4: calculate k_inv * (k - r*da) mod n. and demontgomery the r & s. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_sm2_sign_s_post_6, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 5: get data out from the DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_s, s->data, s->length));
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_r, r->data, r->length));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}