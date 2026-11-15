#===============================================================================
# @brief    cmake file for SLE Fragmentation sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_fragmentation.c)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_fragmentation_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_fragmentation_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
