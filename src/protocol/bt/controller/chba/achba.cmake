#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(MODULE_NAME "bt")
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "achba")
set(CHBA_NETDEV_LIST  "" CACHE INTERNAL "" FORCE)
set(CHBA_NETDEV_HEADER_LIST  "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(achba)

set(PUBLIC_DEFINES
    ACHBA_SUPPORT
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

set(COMPONENT_NAME "achba")

if("${ACHBA_LIST}" STREQUAL "")
    set(ACHBA_LIST "__null__")
endif()

set(SOURCES
    ${ACHBA_LIST}
)

set(PUBLIC_HEADER
    ${ACHBA_HEADER_LIST}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include
    ${ROOT_DIR}/drivers/chips/ws53/arch/include
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
    ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include/
    ${ROOT_DIR}/open_source/lwip/lwip_adapt/src/include/
    ${ROOT_DIR}/middleware/utils/common_headers/osal/
    ${ACHBA_HEADER_LIST}
)
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND})
build_component()
