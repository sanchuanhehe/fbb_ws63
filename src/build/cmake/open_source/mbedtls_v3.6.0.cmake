#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls_v3.6.0")

set(SOURCES
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/aes.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/aesce.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/aesni.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/aria.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/asn1parse.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/asn1write.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/base64.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/bignum.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/bignum_core.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/bignum_mod.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/bignum_mod_raw.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/block_cipher.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/blowfish.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/camellia.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ccm.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/chacha20.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/chachapoly.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/cipher.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/cipher_wrap.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/cmac.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/constant_time.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ctr_drbg.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/debug.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/des.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/dhm.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecdh.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecdsa.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecjpake.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecp.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecp_curves.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ecp_curves_new.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/entropy.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/error.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/gcm.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/hkdf.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/hmac_drbg.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/hw_aescrypt2.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/hw_mbedtls_config.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/hw_polarssl_api.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/lmots.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/lms.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/md2.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/md4.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/md5.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/md.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/memory_buffer_alloc.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/mps_reader.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/mps_trace.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/net_sockets.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/nist_kw.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/oid.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/padlock.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pem.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pk.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pkcs12.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pkcs5.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pkcs7.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pk_ecc.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pkparse.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pk_wrap.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/pkwrite.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/platform.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/platform_util.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/poly1305.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ripemd160.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/rsa_alt_helpers.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/rsa.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/sha1.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/sha256.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/sha3.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/sha512.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_cache.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_ciphersuites.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_client.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_cookie.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_debug_helpers_generated.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_msg.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_ticket.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls12_client.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls12_server.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls13_client.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls13_generic.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls13_keys.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls13_server.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/ssl_tls.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/threading.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/version.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/version_features.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509_create.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509_crl.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509_crt.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509_csr.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509write.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509write_crt.c
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/x509write_csr.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0
)
set(PUBLIC_DEFINES
    MBEDTLS_CONFIG_FILE="${ROOT_DIR}/drivers/chips/ws63/porting/mbedtls/config-ws-iot_v3.6.0.h"
)

set(PRIVATE_HEADER
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include/psa
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include/mbedtls
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/3rdparty/everest/include/everest
)

set(PRIVATE_DEFINES
    SYSCALLS_H
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)
set(COMPONENT_CCFLAGS
	-Wno-error=switch-default
	-Wno-error=unused-parameter
    -Wno-error=unused-variable
    -Wno-error=strict-prototypes
    -Wno-error=sign-compare
    -Wno-error=return-type
    -Wno-error=unused-value
    -Wno-error=comment
    -Wno-error=pointer-sign
    -Wno-error=attributes
    -Wno-error=uninitialized
    -Wno-error=unused-function
	-Wno-error=format
	-Wno-error=incompatible-pointer-types
    -Wno-error
    -include lwip_adapt.h
)
if ((${CHIP} STREQUAL "ws63") OR (${CHIP} STREQUAL "ws53") OR (${CHIP} STREQUAL "sw21"))
    list(APPEND PRIVATE_DEFINES
        WS_IOT_LWIP_C
    )
endif()

if("${ARCH}" STREQUAL "riscv31")
list (APPEND COMPONENT_CCFLAGS
	-Wno-error=jump-misses-init
)
endif()

if("${ARCH}" STREQUAL "cortex_m7")
	list (APPEND COMPONENT_CCFLAGS
			-Wno-error=jump-misses-init
	)
endif()


set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
