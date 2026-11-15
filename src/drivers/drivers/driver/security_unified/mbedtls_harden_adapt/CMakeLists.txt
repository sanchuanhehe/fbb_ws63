#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls_harden")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/aes_harden_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/aes_harden_impl_long_term.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/bignum_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp_harden_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp_harden_impl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/hash_harden_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/hash_harden_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/hash_harden_impl_long_term.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/mbedtls_harden_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/pbkdf2_hmac_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/rsa_harden_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha512.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecc_harden_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/cipher_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/entropy_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ccm_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/cipher_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/cmac_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecc_harden.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/gcm_harden.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    MBEDTLS_USER_CONFIG_FILE="mbedtls_platform_hardware_config.h"
    MBEDTLS_HARDEN_OPEN
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

if("mbedtls_v3.6.0" IN_LIST RAM_COMPONENT)
    list(APPEND SOURCES 
        ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/mbedtls_harden_adapt/library/rsa.c
    )
endif()

if("mbedtls_v3.1.0" IN_LIST RAM_COMPONENT)
    list(APPEND SOURCES 
        ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/connect_src/rsa.c
    )
endif()

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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
