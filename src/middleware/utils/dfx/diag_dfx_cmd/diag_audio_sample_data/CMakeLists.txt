#===============================================================================
# @brief    cmake make file
# Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd.
#===============================================================================
set(COMPONENT_NAME "dfx_audio_sample_data")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_audio_sample_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zdiag_audio_hook.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include/" "*.h")

build_component()
