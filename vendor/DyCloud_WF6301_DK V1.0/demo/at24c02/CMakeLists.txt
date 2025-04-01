#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_AT24C02)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/at24c02_demo.c" PARENT_SCOPE)
endif()