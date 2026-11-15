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

set(SOURCES
    ${CHBA_NETDEV_LIST}
)

set(PUBLIC_HEADER
    ${CHBA_NETDEV_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include
    ${ROOT_DIR}/drivers/chips/ws53/arch/include
    ${CHBA_NETDEV_HEADER_LIST}
)

if("${CHBA_NETDEV_LIST}" STREQUAL "")
    set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
endif()

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND})
build_component()

set(COMPONENT_NAME "chba_at")
set(CHBA_AT_LIST  "" CACHE INTERNAL "" FORCE)
set(CHBA_AT_HEADER_LIST  "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(at)

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

set(SOURCES
    ${CHBA_AT_LIST}
)

set(PUBLIC_HEADER
    ${CHBA_AT_HEADER_LIST}
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
)

if("${CHBA_AT_LIST}" STREQUAL "")
    set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
endif()
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND})
build_component()
