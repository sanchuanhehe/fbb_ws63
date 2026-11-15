#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
add_subdirectory(manager)
add_subdirectory(model)
add_subdirectory(prov)
add_subdirectory(message)
add_subdirectory(ota)
add_subdirectory(sm)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST} PARENT_SCOPE)