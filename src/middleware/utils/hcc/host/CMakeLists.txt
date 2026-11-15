#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "hcc_host")

set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID FALSE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hcc_test_host.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../cfg
)

if(NOT "USE_WS53_ROM_REPO" IN_LIST DEFINES)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
endif()


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
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hcc_spi_host.c)
else()
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_MULTI_CHANNEL)
if("_PRE_FEATURE_WS53_DEVICE_MODE" IN_LIST DEFINES OR "_PRE_FEATURE_SYSCHANNEL_LITEOS" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hcc_sdio_host.c)
else()
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_IPC)
    list(APPEND PRIVATE_DEFINES CONFIG_HCC_SUPPORT_IPC_HOST)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hcc_ipc_host.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../slave/hcc_sdio_device.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../slave/hcc_sdio_device_rom.c)
endif()
endif()

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-implicit-function-declaration
    -Wno-sign-compare
    -Wno-unused-function
)
if("_PRE_FEATURE_WS53_DEVICE_MODE" IN_LIST DEFINES OR "_PRE_FEATURE_SYSCHANNEL_LITEOS" IN_LIST DEFINES)
    list(APPEND COMPONENT_CCFLAGS -Wno-pointer-sign)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "hcc_ut_core")

set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID FALSE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../slave/hcc_ipc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../slave/hcc_sdio_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../slave/hcc_sdio_device_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_bus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_flow_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm/hcc_rom_callback.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../comm
    ${CMAKE_CURRENT_SOURCE_DIR}/../cfg
)

if(NOT "USE_WS53_ROM_REPO" IN_LIST DEFINES)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
endif()


set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    CONFIG_HCC_SUPPORT_MULTI_CHANNEL
    CONFIG_HCC_SUPPORT_FLOW_CONTRL
    CONFIG_HCC_SUPPORT_FLOW_CONTRL_ACTIVE
    CONFIG_HCC_SUPPORT_DFX
    CONFIG_HCC_SUPPORT_TEST
    CONFIG_HCC_SUPPORT_SDIO
    CONFIG_HCC_SUPPORT_IPC
    CONFIG_HCC_SUPPORT_UNC_POOL
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
    -Wno-unused-parameter
    -Wno-sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()