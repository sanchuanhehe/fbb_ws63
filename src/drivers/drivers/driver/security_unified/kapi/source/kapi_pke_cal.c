/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: big number calculate implement file.
 *
 * Create: 2023-03-25
*/

#include "kapi_pke_cal.h"
#include "crypto_osal_adapt.h"
#include "drv_pke_cal.h"

td_s32 kapi_pke_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_add_mod(a, b, p, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_add_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_sub_mod(a, b, p, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_sub_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_mul_mod(a, b, p, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_mul_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_inv_mod(a, p, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_inv_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_mod(a, p, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_mul(a, b, c);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_mul failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}

td_s32 kapi_pke_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out)
{
    td_s32 ret = TD_SUCCESS;
    ret = drv_cipher_pke_lock_secure();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_lock_secure failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_exp_mod(n, k, in, out);
    crypto_chk_goto(ret != TD_SUCCESS, exit, "drv_cipher_pke_exp_mod failed, ret is 0x%x", ret);
exit:
    drv_cipher_pke_unlock_secure();
    return ret;
}