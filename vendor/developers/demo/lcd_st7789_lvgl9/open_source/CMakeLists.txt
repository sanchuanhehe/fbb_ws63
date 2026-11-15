#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lvgl")

set(CMAKE_LVGL_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES ${CMAKE_LVGL_SOURCE_DIR}/src/*.c)

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
    # -Wno-error=logical-op
    # -Wno-error=sign-compare
    # -Wno-error=jump-misses-init
    # -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    # -Wno-unused-parameter
    # -Wno-unused-but-set-variable
    # -Wno-error=unused-variable
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()