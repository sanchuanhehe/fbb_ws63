#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "loaderboot_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/startup/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup/riscv_init.S
    ${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/src/boot_uart_auth.c
)

if(DEFINED CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE)
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/src/boot_erase.c")
endif()

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

build_component()