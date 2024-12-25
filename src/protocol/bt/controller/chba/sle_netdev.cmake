#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(MODULE_NAME "bt")
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "sle_netdev")
set(CHBA_NETDEV_LIST  "" CACHE INTERNAL "" FORCE)
set(CHBA_NETDEV_HEADER_LIST  "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(sle_netdev)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wmissing-declarations -Wundef  -Wmissing-prototypes -Wswitch-default
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(COMPONENT_NAME "sle_netdev")

if("${CHBA_NETDEV_LIST}" STREQUAL "")
    set(CHBA_NETDEV_LIST "__null__")
endif()

set(SOURCES
    ${CHBA_NETDEV_LIST}
)

set(PUBLIC_HEADER
    ${CHBA_NETDEV_HEADER_LIST}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include
    ${ROOT_DIR}/drivers/chips/ws53/arch/include
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
    ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include/
    ${ROOT_DIR}/open_source/lwip/lwip_adapt/src/include/
    ${CHBA_NETDEV_HEADER_LIST}
)
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND})
build_component()
