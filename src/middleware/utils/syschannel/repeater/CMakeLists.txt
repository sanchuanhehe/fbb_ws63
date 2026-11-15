#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "syschannel_filter")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/syschannel_filter_ipv4.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER

)

set(PRIVATE_DEFINES
    SYSCHANNEL_LITTLE_ENDIAN=1
    SYSCHANNEL_BIG_ENDIAN=2
    SYSCHANNEL_ENDIAN=SYSCHANNEL_LITTLE_ENDIAN
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

build_component()