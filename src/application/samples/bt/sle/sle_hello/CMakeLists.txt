#===============================================================================
# @brief    cmake file for SLE Hello World sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_hello.c)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_HELLO_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_hello_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_hello_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
