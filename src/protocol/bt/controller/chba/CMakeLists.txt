#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if("CHBA_SUPPORT" IN_LIST DEFINES)
include(achba.cmake)
include(sle_netdev.cmake)
endif()