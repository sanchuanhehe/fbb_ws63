#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
include(${ROOT_DIR}/build/cmake/open_source/mbedtls_v3.1.0.cmake)
include(${ROOT_DIR}/build/cmake/open_source/mbedtls_v3.6.0.cmake)

if(mbedtls_v3.1.0 IN_LIST TARGET_COMPONENT)
    install_sdk(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0 "*")
endif()
if(mbedtls_v3.6.0 IN_LIST TARGET_COMPONENT)
    install_sdk(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0 "*")
endif()
