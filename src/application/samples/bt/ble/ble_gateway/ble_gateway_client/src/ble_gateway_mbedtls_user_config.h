/* MbedTLS additions required by Huawei Cloud IoTDA MQTTS. */
#ifndef BLE_GATEWAY_MBEDTLS_USER_CONFIG_H
#define BLE_GATEWAY_MBEDTLS_USER_CONFIG_H

#include "mbedtls_platform_hardware_config.h"

/* The WS63 PKE alternative rejects the ECDHE operation used by this endpoint. */
#undef MBEDTLS_ECP_MUL_ALT
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_SSL_CIPHERSUITES                         \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,      \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256

#endif
