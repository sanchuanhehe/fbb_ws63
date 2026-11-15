#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_sample_init.c
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
