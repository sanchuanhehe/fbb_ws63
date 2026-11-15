#===============================================================================
# @brief    CMake file for the SLE connection parameter tuning server.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_conn_param_tuning_server.c)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/sle_conn_param_tuning_server_adv.c)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
