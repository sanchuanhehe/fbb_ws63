#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lvgl")

set(CMAKE_LVGL_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED CONFIG_OPEN_SOURCE_LVGL)
    file(GLOB_RECURSE SOURCES ${CMAKE_LVGL_SOURCE_DIR}/src/*.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_LVGL_SOURCE_DIR}
    ${CMAKE_LVGL_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_LVGL_SOURCE_DIR}
    ${CMAKE_LVGL_SOURCE_DIR}/src
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -DLV_CONF_INCLUDE_SIMPLE
    -U__linux__
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
