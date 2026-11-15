#===============================================================================
# @brief    fbb project.cmake — public entry for out-of-tree projects.
#
# Usage in user's project CMakeLists.txt:
#
#   cmake_minimum_required(VERSION 3.14.1)
#   include("$ENV{FBB_SDK_DIR}/build/cmake/project.cmake")
#   project(my_app)
#
# Or via find_package (fbb-config.cmake lives next to this file):
#
#   cmake_minimum_required(VERSION 3.14.1)
#   find_package(fbb REQUIRED HINTS "$ENV{FBB_SDK_DIR}/build/cmake")
#   project(my_app)
#
# project() is a macro that runs cfbb_build_prologue() before the real
# project() call and cfbb_build_epilogue() after. The user's project
# CMakeLists.txt looks like a stock CMake file.
#
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================

# 1. Locate SDK root. If ROOT_DIR was passed via -D (CLI route), use it;
#    else derive from this file's own location.
#    project.cmake lives at <SDK>/build/cmake/project.cmake.
if(NOT DEFINED ROOT_DIR)
    get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif()
file(TO_CMAKE_PATH "${ROOT_DIR}" ROOT_DIR)
set(FBB_SDK_DIR "${ROOT_DIR}")

# 2. Mark mode so build_core can branch on it. Out-of-tree means the cmake
#    source dir is the user's project, not the SDK.
set(FBB_OUT_OF_TREE TRUE)

# 3. Default project component name. Override before project() if your main
#    component is called something other than "main".
if(NOT DEFINED FBB_PROJECT_COMPONENT_NAME)
    set(FBB_PROJECT_COMPONENT_NAME "main")
endif()

# 4. Load shared build core (defines cfbb_build_prologue / cfbb_build_epilogue)
include("${ROOT_DIR}/build/cmake/build_core.cmake")

# 5. Override project() with a macro that brackets it with prologue + epilogue.
#    CMake accepts macro-overridden project() — the original is callable as
#    _project. The user's textual project() call satisfies CMake's
#    "top-level must call project()" requirement.
macro(project _fbb_proj_name)
    cfbb_build_prologue()
    _project(${_fbb_proj_name} ${ARGN})
    cfbb_build_epilogue()
endmacro()
