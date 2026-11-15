#===============================================================================
# @brief    CMake file for the SLE RSSI ranging sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_rssi_ranging.c)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_rssi_ranging_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_rssi_ranging_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
