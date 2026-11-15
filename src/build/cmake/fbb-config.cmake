#===============================================================================
# @brief    fbb package config — entry for find_package(fbb).
#
# Usage:
#   find_package(fbb REQUIRED HINTS "$ENV{FBB_SDK_DIR}/build/cmake")
#
# This file is a thin shim that loads project.cmake (which defines the
# project() macro override and pulls in build_core.cmake).
#
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================

include("${CMAKE_CURRENT_LIST_DIR}/project.cmake")

set(fbb_FOUND TRUE)
