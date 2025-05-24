#===============================================================================
# @brief librws cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "librws")

set(CMAKE_LIBRWS_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/librws-1.2.4/)

set(SOURCES
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/librws.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_common.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_error.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_frame.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_list.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_memory.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_socketpriv.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_socketpub.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_string.c
        ${CMAKE_LIBRWS_SOURCE_DIR}/src/rws_thread.c
)

set(PUBLIC_HEADER
    ${CMAKE_LIBRWS_SOURCE_DIR}
    ${CMAKE_LIBRWS_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_LIBRWS_SOURCE_DIR}/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-variable
    -fstack-protector-strong
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()