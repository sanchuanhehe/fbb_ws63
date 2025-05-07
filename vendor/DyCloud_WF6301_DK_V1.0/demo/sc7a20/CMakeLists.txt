#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SC7A20)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/sc7a20_demo.c" PARENT_SCOPE)
endif()