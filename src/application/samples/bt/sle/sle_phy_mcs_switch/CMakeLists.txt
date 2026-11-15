#===============================================================================
# @brief    CMake file for the SLE PHY/MCS dynamic switch sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_phy_mcs_switch.c)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_phy_mcs_switch_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_phy_mcs_switch_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
