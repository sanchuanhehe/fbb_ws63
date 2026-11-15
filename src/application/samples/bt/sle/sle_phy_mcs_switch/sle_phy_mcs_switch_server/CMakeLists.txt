#===============================================================================
# @brief    CMake file for the SLE PHY/MCS dynamic switch server.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_phy_mcs_switch_server.c)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_phy_mcs_switch_server_adv.c)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
