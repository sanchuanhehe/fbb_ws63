#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(MODULE_NAME "bt")
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "sle_chba_user")
set(CHBA_USER_LIST  "" CACHE INTERNAL "" FORCE)
set(CHBA_USER_HEADER_LIST  "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(sle_chba_user)
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

set(COMPONENT_NAME "sle_chba_user")

if("${CHBA_USER_LIST}" STREQUAL "")
    set(CHBA_USER_LIST "__null__")
endif()

set(SOURCES
    ${CHBA_USER_LIST}
)

set(PUBLIC_HEADER
    ${CHBA_USER_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include
    ${CHBA_USER_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/
)
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND})
build_component()