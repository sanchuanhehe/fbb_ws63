/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: pke parameter get header file.
 *
 * Create: 2023-06-01
*/

#ifndef CRYPTO_CURVE_PARAM_H
#define CRYPTO_CURVE_PARAM_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void crypto_curve_param_init(void);

const drv_pke_ecc_curve *secp192r1_curve(void);

const drv_pke_ecc_curve *secp224r1_curve(void);

const drv_pke_ecc_curve *secp256r1_curve(void);

const drv_pke_ecc_curve *secp384r1_curve(void);

const drv_pke_ecc_curve *secp521r1_curve(void);

const drv_pke_ecc_curve *brainpoolP256r1_curve(void);

const drv_pke_ecc_curve *brainpoolP384r1_curve(void);

const drv_pke_ecc_curve *brainpoolP512r1_curve(void);

const drv_pke_ecc_curve *curve25519_curve(void);

const drv_pke_ecc_curve *curve448_curve(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif