/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt platform hardware configuration.
 *
 * Create: 2024-07-10
*/
#ifndef MBEDTLS_PLATFORM_HARDWARE_CONFIG_H
#define MBEDTLS_PLATFORM_HARDWARE_CONFIG_H

#if defined(MBEDTLS_HARDEN_OPEN)
/*******************************************unsupport macro begin*********************************************/
#undef MBEDTLS_CIPHER_MODE_CFB
#undef MBEDTLS_CIPHER_MODE_XTS
/*******************************************unsupport macro end*********************************************/

/*******************************************alternative macro begin*********************************************/
/*
 * Accelerate the AES algorithm by hardware.
 *
 * Requires: MBEDTLS_AES_C
 *
 * Module:  library/aes.c
*/
#ifndef MBEDTLS_AES_ALT
#define MBEDTLS_AES_ALT
#endif

/*
 * use hardware entropy instead.
 *
 * Requires: MBEDTLS_ENTROPY_C
 *
 * Module:  library/entropy.c
*/
#ifndef MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

#if !defined(MBEDTLS_ECP_RESTARTABLE)
/*
 * Accelerate the ECDH algorithm by hardware.
 *
 * This configuration replaces the following interfaces with the hardware acceleration implementation:
 *      mbedtls_ecdh_compute_shared
 *
 * Requires: MBEDTLS_ECDH_C
 *
 * Module:  library/ecdh.c
*/
#ifndef MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#endif

/*
 * Accelerate the ECDSA Sign algorithm by hardware.
 *
 * This configuration replaces the following interfaces with the hardware acceleration implementation:
 *      mbedtls_ecdsa_sign
 *
 * Requires: MBEDTLS_ECDSA_C
 *
 * Module:  library/ecdsa.c
*/
#ifndef MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_SIGN_ALT
#endif

/*
 * Accelerate the ECDSA Verify algorithm by hardware.
 *
 * This configuration replaces the following interfaces with the hardware acceleration implementation:
 *      mbedtls_ecdsa_verify
 *
 * Requires: MBEDTLS_ECDSA_C
 *
 * Module:  library/ecdsa.c
*/
#ifndef MBEDTLS_ECDSA_VERIFY_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT
#endif

/*
 * Accelerate the ECDSA Gen Key algorithm by hardware.
 *
 * This configuration replaces the following interfaces with the hardware acceleration implementation:
 *      mbedtls_ecdsa_genkey
 *
 * Requires: MBEDTLS_ECDSA_C
 *
 * Module:  library/ecdsa.c
*/
#ifndef MBEDTLS_ECDSA_GENKEY_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#endif
#endif

/*
 * Accelerate the SHA1 algorithm by hardware.
 *
 * Requires: MBEDTLS_SHA1_C
 *
 * Module:  library/sha1.c
*/
#ifndef MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA1_ALT
#endif
/*
 * Accelerate the SHA224/256 algorithm by hardware.
 *
 * Requires: MBEDTLS_SHA256_C
 *
 * Module:  library/sha256.c
*/
#ifndef MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_ALT
#endif

/*
 * Accelerate the SHA384/512 algorithm by hardware.
 *
 * Requires: MBEDTLS_SHA512_C
 *
 * Module:  library/sha512.c
*/
#ifndef MBEDTLS_SHA512_ALT
#define MBEDTLS_SHA512_ALT
#endif
/*
 * Accelerate the PBKDF2 algorithm by hardware.
 *
 * This configuration replaces the following interfaces with the hardware acceleration implementation:
 *      mbedtls_pkcs5_pbkdf2_hmac
 *
 * Requires: MBEDTLS_PKCS5_C
 *
 * Module:  library/pkcs5.c
*/
#ifndef MBEDTLS_PBKDF2_HMAC_ALT
#define MBEDTLS_PBKDF2_HMAC_ALT
#endif
/*******************************************alternative macro end*********************************************/

/*******************************************hardware macro begin*********************************************/
/*
 * Accelerate the EXP MOD by hardware.
 * If the data format is not supported, the software calculation is still used.
 *
 * This configuration affects the following interfaces with the hardware acceleration implementation:
 *      mbedtls_mpi_exp_mod
 *
 * Requires: MBEDTLS_BIGNUM_C
 *
 * Module:  library/bignum.c
*/
#define MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE

/*******************************************hardware macro end*******************************************/

#endif

#endif /* MBEDTLS_PLATFORM_USER_CONFIG_H */