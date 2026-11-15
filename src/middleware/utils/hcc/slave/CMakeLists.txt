#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "hcc_slave")

set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID FALSE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hcc_test_slave_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_bus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_flow_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_test_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_rom_callback.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../cfg
    ${CMAKE_CURRENT_SOURCE_DIR}/../host
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/driver
)

set(PRIVATE_DEFINES
    CONFIG_HCC_SUPPORT_FLOW_CONTRL
    CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
    CONFIG_HCC_SUPPORT_DFX
    CONFIG_HCC_SUPPORT_TEST
    CONFIG_HCC_SUPPORT_TEST_SYSCH
    CONFIG_HCC_SUPPORT_UNC_POOL
)

if (${CHIP} MATCHES "ws63")
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_SPI)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hcc_spi_device.c)
else()
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hcc_ipc_device.c)
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_IPC)
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_NON_OS)
endif()

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