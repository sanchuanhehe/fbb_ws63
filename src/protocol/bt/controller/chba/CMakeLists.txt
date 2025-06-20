#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if("CHBA_SUPPORT" IN_LIST DEFINES)
include(sle_netdev.cmake)
include(achba.cmake)
endif()