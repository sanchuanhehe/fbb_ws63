#===============================================================================
# @brief    CMake file for the SLE RSSI ranging client.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_rssi_ranging_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_rssi_ranging_calibration.c
)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
