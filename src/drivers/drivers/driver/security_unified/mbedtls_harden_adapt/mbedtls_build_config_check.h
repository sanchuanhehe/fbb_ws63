/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: check if mbedtls macro match security drv macro.
 *
 */
#ifndef MBEDTLS_BUILD_CONFIG_CHECK_H
#define MBEDTLS_BUILD_CONFIG_CHECK_H

#if defined(__has_include)
#if __has_include("security_unified_config.h")
#include "security_unified_config.h"
#endif
#endif

#if defined(MBEDTLS_ECP_MUL_ALT) || \
    defined(MBEDTLS_ECC_GEN_KEY_ALT) || \
    defined(MBEDTLS_ECDSA_SIGN_ALT) || \
    defined(MBEDTLS_ECDSA_VERIFY_ALT) || \
    defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)

#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_FIPS_P192R)
#error "MBEDTLS_ECP_DP_SECP192R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_FIPS_P192R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_FIPS_P192R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_FIPS_P224R)
#error "MBEDTLS_ECP_DP_SECP224R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_FIPS_P224R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_FIPS_P224R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_FIPS_P256R)
#error "MBEDTLS_ECP_DP_SECP256R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_FIPS_P256R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_FIPS_P256R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_FIPS_P384R)
#error "MBEDTLS_ECP_DP_SECP384R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_FIPS_P384R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_FIPS_P384R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_FIPS_P521R)
#error "MBEDTLS_ECP_DP_SECP521R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_FIPS_P521R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_FIPS_P521R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_BP256R)
#error "MBEDTLS_ECP_DP_BP256R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_BP256R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_BP256R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_BP384R)
#error "MBEDTLS_ECP_DP_BP384R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_BP384R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_BP384R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_BP512R)
#error "MBEDTLS_ECP_DP_BP512R1_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_BP512R is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_BP512R in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_CURVE_25519)
#error "MBEDTLS_ECP_DP_CURVE25519_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_CURVE_25519 is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_CURVE_25519 in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED) && !defined(CONFIG_PKE_SUPPORT_ECC_CURVE_448)
#error "MBEDTLS_ECP_DP_CURVE448_ENABLED is defined but CONFIG_PKE_SUPPORT_ECC_CURVE_448 is not." \
       "Enable CONFIG_PKE_SUPPORT_ECC_CURVE_448 in menuconfig."
#endif

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
#error "SECP192K1 UNSUPPORT!"
#endif

#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
#error "SECP224K1 UNSUPPORT!"
#endif

#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
#error "SECP256K1 UNSUPPORT!"
#endif
#endif /* MBEDTLS_ECP_MUL_ALT / MBEDTLS_ECC_GEN_KEY_ALT / MBEDTLS_ECDSA_SIGN_ALT / MBEDTLS_ECDSA_VERIFY_ALT / MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

#endif /* MBEDTLS_BUILD_CONFIG_CHECK_H */