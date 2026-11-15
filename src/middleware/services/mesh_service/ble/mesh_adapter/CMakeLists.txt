#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
add_subdirectory(at)
add_subdirectory(ble_adapter)
add_subdirectory(mesh_porting)
add_subdirectory(mesh_models)
add_subdirectory(sdk)
add_subdirectory(shell)
add_subdirectory(utils)
add_subdirectory(include)
set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST} PARENT_SCOPE)