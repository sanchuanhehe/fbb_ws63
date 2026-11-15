#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_PWM_PRELOAD)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/pwm_preload_demo.c" PARENT_SCOPE)
else()
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/pwm_demo.c" PARENT_SCOPE)
endif()