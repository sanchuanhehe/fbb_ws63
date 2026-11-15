#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "syschannel_host")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/channel_host/syschannel_host_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wal/liteos/wal_net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common/syschannel_common.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/middleware/utils/syschannel/syschannel_common/
    ${ROOT_DIR}/middleware/utils/syschannel/syschannel_api/
    ${ROOT_DIR}/middleware/utils/syschannel/
    ${ROOT_DIR}/middleware/utils/syschannel/syschannel_host/
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/channel_host
    ${CMAKE_CURRENT_SOURCE_DIR}/wal/liteos
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_api
    ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include
    ${ROOT_DIR}/middleware/utils/hcc/cfg/
    ${ROOT_DIR}/kernel/osal_adapt/inc/
)

set(PRIVATE_DEFINES
    CONFIG_HCC_SUPPORT_DFX
    CONFIG_HCC_SUPPORT_UNC_POOL
    CONFIG_HCC_SUPPORT_SDIO
    CONFIG_HCC_SUPPORT_TEST
    CONFIG_HCC_SUPPORT_TEST_SYSCH
    SYSCHANNEL_LITTLE_ENDIAN=1
    SYSCHANNEL_ENDIAN=SYSCHANNEL_LITTLE_ENDIAN
    ETH_PAD_SIZE=2
    PBUF_ZERO_COPY_RESERVE=80
)

if("CONFIG_HCC_SUPPORT_SPI" IN_LIST DEFINES)
    list(REMOVE_ITEM PRIVATE_DEFINES
        CONFIG_HCC_SUPPORT_SDIO
    )
endif()

set(PUBLIC_DEFINES
    _PRE_OS_VERSION=_PRE_OS_VERSION_LITEOS
    _PRE_FEATURE_SYSCHANNEL_LITEOS
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-pointer-sign
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()


set(COMPONENT_NAME "syschannel_host_ko")
return_if_not_build()

set(KO_NAME syschannel.ko)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(OUTPUT_DIR ${ROOT_DIR}/output/${CHIP}/${CORE}/${BUILD_TARGET_NAME})
    add_custom_command(
        OUTPUT install_${COMPONENT_NAME}
        COMMENT "Install ${COMPONENT_NAME} ..."
        COMMAND make all
        COMMAND cp -f ${KO_NAME} ${OUTPUT_DIR}/
        COMMAND make clean
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
 
    add_custom_target(${COMPONENT_NAME} ALL DEPENDS install_${COMPONENT_NAME})
    add_dependencies(${TARGET_NAME} ${COMPONENT_NAME})
endif()

build_component()