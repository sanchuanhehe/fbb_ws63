#===============================================================================
# @brief    CMake file for the BLE Hello client.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
add_subdirectory_if_exist(src)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}/inc" PARENT_SCOPE)
