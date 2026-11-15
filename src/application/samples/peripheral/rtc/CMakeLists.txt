#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
#===============================================================================
list(APPEND RAM_COMPONENT
    rtc_unified
    hal_rtc_unified
    rtc_unified_port
)
list(REMOVE_DUPLICATES RAM_COMPONENT)
set(RAM_COMPONENT "${RAM_COMPONENT}" CACHE INTERNAL "" FORCE)

set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/rtc_sample.c" PARENT_SCOPE)
