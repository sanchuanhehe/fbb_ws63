#===============================================================================
# fbb sample project — out-of-tree project entry.
#
# The SDK is located via the FBB_SDK_DIR environment variable (set by
# `fbb build`). project.cmake defines a project() macro override that
# brackets the real project() with the SDK build prologue / epilogue.
#===============================================================================
cmake_minimum_required(VERSION 3.14.1)

# Locate SDK. `fbb build` exports FBB_SDK_DIR; manual invocations can pass
# it on the cmake command line via -DFBB_SDK_DIR=... or in the env.
if(NOT DEFINED FBB_SDK_DIR AND DEFINED ENV{FBB_SDK_DIR})
    set(FBB_SDK_DIR "$ENV{FBB_SDK_DIR}")
endif()
if(NOT DEFINED FBB_SDK_DIR)
    message(FATAL_ERROR
        "FBB_SDK_DIR is not set. Run via `fbb build`, or set the env var, "
        "or pass -DFBB_SDK_DIR=<path-to-sdk> on the cmake command line.")
endif()

include("${FBB_SDK_DIR}/build/cmake/project.cmake")

# Literal project() — the macro from project.cmake wraps prologue + epilogue.
project(sample_project C ASM CXX)
