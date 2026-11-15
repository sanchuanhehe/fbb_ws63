#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(MODULE_NAME "bt")

# bgtp与bgtp_rom组件公共部分
set(BTC_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTC_ROM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTC_ROM_DATA_LIST  "" CACHE INTERNAL "" FORCE)
set(BTC_HEADER_LIST  "" CACHE INTERNAL "" FORCE)
set(AUTO_DEF_FILE_ID TRUE)

if(DEFINED ROM_COMPONENT)
    set(BT_ROM_VERSION true)
else()
    set(BT_ROM_VERSION false)
endif()

if("DEVICE_ONLY" IN_LIST DEFINES)
    set(BGTP_DEVICE_ONLY true)
else()
    set(BGTP_DEVICE_ONLY false)
endif()

if(BGTP_DEVICE_ONLY)
    set(BT_ONETRACK false)
else()
    set(BT_ONETRACK true)
endif()

MESSAGE("BGTP_PROJECT=" ${BGTP_PROJECT})
MESSAGE("BGTP_ROM_VERSION=" ${BT_ROM_VERSION})
MESSAGE("BGTP_DEVICE_ONLY=" ${BGTP_DEVICE_ONLY})

add_subdirectory_if_exist(chip)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    "BTC_SYS_PART=100"
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
	-Wundef
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/chip/ws63/ram/dts/ws63/log_def_btc.h)
set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/chip/ws63/ram/dts/ws63/log_def_btc.h
)
endif()

# ram组件，编译BTC_RAM_LIST
set(COMPONENT_NAME "bgtp")

if("${BTC_RAM_LIST}" STREQUAL "")
    set(BTC_RAM_LIST "__null__")
endif()

set(SOURCES
    ${BTC_RAM_LIST}
)

set(PUBLIC_HEADER
    ${BTC_HEADER_LIST}
)

set(PRIVATE_HEADER
)

unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
if("UPDATE_BTC_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/btc/${TARGET_COMMAND})
endif()
build_component()

# rom组件，编译BTC_ROM_LIST
set(COMPONENT_NAME "bgtp_rom")
set(SOURCES
    ${BTC_ROM_LIST}
)

set(PUBLIC_HEADER
    ${BTC_HEADER_LIST}
)

set(PRIVATE_HEADER
)

unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
if("UPDATE_BTC_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/btc/${TARGET_COMMAND})
endif()
build_component()

set(COMPONENT_NAME "bgtp_rom_data")
if(DEFINED ROM_SYM_PATH)
if("${BTC_ROM_DATA_LIST}" STREQUAL "")
    set(BTC_ROM_DATA_LIST "__null__")
endif()

set(SOURCES
    ${BTC_ROM_DATA_LIST}
)
endif()
unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
if("UPDATE_BTC_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/btc/${TARGET_COMMAND})
endif()
build_component()
