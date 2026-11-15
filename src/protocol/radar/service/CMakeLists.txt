#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
#===============================================================================
MESSAGE(STATUS "project_name:" ${project_name})
if(CHIP STREQUAL "ws63")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/SP3x/slp_radar_client.cmake)
    return()
elseif("${board}" MATCHES "HI115")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/115x/115x_radar_service.cmake)
    return()
endif()