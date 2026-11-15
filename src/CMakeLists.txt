#===============================================================================
# @brief    cmake entry (in-tree). Out-of-tree projects use
#           build/cmake/project.cmake instead; both share build_core.cmake.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2026. All rights reserved.
#===============================================================================
cmake_minimum_required(VERSION 3.14.1)

# In-tree: ROOT_DIR is this directory. Must be set before include(build_core).
get_filename_component(ROOT_DIR "${CMAKE_SOURCE_DIR}" ABSOLUTE)

include("${ROOT_DIR}/build/cmake/build_core.cmake")
cfbb_build_prologue()

project(${CHIP}_CFBB C ASM CXX)

cfbb_build_epilogue()
