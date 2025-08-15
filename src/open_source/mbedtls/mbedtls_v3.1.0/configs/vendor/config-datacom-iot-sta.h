/**
 * \file config-datacom-iot-sta.h
 *
 * \brief 数据通信产品线IOT Gateway产品PLC STA的mbedtls库的配置文件，基于3.1.0版本的suite-b配置文件。
 *
 *  产品在使用时，在编译前，将此文件强制覆盖默认的配置文件即可，使用方式如下：
 *  cp -f ./configs/vendor/config-datacom-iot-sta.h ./include/mbedtls/mbedtls_config.h
 */
#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_HAVE_TIME

/* mbed TLS feature support */
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_GCM_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA224_C // Required by MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_X509_CSR_PARSE_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS12_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS
#define MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_GENPRIME
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_ECP_RESTARTABLE

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_MPI_MAX_SIZE   256 // HLINK MODIFY

/* Save RAM at the expense of speed, see ecp.h */
#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
#define MBEDTLS_ECP_NIST_OPTIM

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "mbedtls_platform_entropy_poll" source, but you may want to add other ones.
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 2

/* Save ROM and a few bytes of RAM by specifying our own ciphersuite list */
#define MBEDTLS_SSL_CIPHERSUITES                        \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,    \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,    \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,      \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256

/*
 * Save RAM at the expense of interoperability: do this only if you control
 * both ends of the connection!  (See coments in "mbedtls/ssl.h".)
 * The minimum size here depends on the certificate chain used as well as the
 * typical size of records.
 */
// HLINK MODIFY
#define MBEDTLS_SSL_IN_CONTENT_LEN (1024*5)
#define MBEDTLS_SSL_OUT_CONTENT_LEN (1024*5)

/* Support DTLS */
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_TIMING_C // HLINK NOT NEEDED
#define MBEDTLS_DEBUG_C
#define MBEDTLS_ERROR_C

// HLINK ADD
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#define MBEDTLS_SSL_SERVER_NAME_INDICATION

/*****************************************************************************
 * \brief 数通IOT Gateway产品定制特性，适配LwIP网络层。
******************************************************************************/
#define VENDOR_IOT_LWIP_C

/*****************************************************************************
 * \brief 数通IOT Gateway产品定制特性，允许DTLS客户端提前设置ClientHello报文的cookie字段，以减少一次报文交互。
******************************************************************************/
#define VENDOR_IOT_DTLS_CLIENT_SET_COOKIE_C

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
