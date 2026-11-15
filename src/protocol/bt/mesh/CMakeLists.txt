#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
if("sle_mesh" IN_LIST RAM_COMPONENT OR "bth_mesh" IN_LIST RAM_COMPONENT)
add_subdirectory_if_exist(gle)
endif()
