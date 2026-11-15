#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(syschannel_host)
add_subdirectory_if_exist(syschannel_dev)
add_subdirectory_if_exist(repeater)
if(syschannel_host_ko IN_LIST TARGET_COMPONENT)
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/syschannel_host "*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/syschannel_common "*")
endif()