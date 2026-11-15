#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ws63_liteos_app")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reset_vector.S
    ${CMAKE_CURRENT_SOURCE_DIR}/clock_init.c
)

# This wrapper depends on an audited vendor-private factory ABI. Do not
# silently apply it to a different Controller library or another image.
if("${TARGET_COMMAND}" STREQUAL "ws63-liteos-app")
    file(SHA256 "${ROOT_DIR}/protocol/bt/controller/bgtp/ws63-liteos-app/libbgtp.a"
         FACTORY_CONTROLLER_SHA256)
    if(NOT FACTORY_CONTROLLER_SHA256 STREQUAL
       "bfedcd51bf15e4ccda299ad09ccc09d212ef1e150d98fc80dde099c409c892e2")
        message(FATAL_ERROR "Re-audit factory TX ABI before applying align4 workaround")
    endif()
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/factory_tx_align4.c)
    target_link_options(${TARGET_NAME} PRIVATE "-Wl,--wrap=evt_task_single_task_start")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    true
)

build_component()
