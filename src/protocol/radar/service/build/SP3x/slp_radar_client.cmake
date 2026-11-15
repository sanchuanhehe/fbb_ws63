#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "slp_radar_client")

set(MODULE_NAME "slp")
set(AUTO_DEF_FILE_ID TRUE)

set(SOURCES)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/assemble_radar_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/parse_radar_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/slp_log_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/slp_radar_client_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/slp_radar_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/slp_radar_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/slp_radar_at.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../middleware/utils/common_headers
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SP3x/
    ${CMAKE_CURRENT_SOURCE_DIR}/client/
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63
)

if("SUPPORT_SLP_RADAR_PERF" IN_LIST DEFINES)
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SP3x/2T2R_PERF/
)
else()
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SP3x/2T2R/
)
endif()

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

if(CONFIG_RADAR_SLP_MFG IN_LIST PRIVATE_DEFINES)
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/mfg/slp_radar_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/mfg/parse_radar_mfg_msg.c
)
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/client/ws63/mfg/
)
endif()

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

set(BUILD_AS_OBJ
    false
)

set(LOG_DEF
    ${ROOT_DIR}/middleware/chips/ws63/dfx/include/log_def_slp_radar.h
)

if(EXISTS "${ROOT_DIR}/open_source/libboundscheck/include")
    set(LIBC_SEC_DIR "${ROOT_DIR}/open_source/libboundscheck/include")
endif()

#指定静态库生成位置
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/radar/${TARGET_COMMAND})
return_if_not_build()

build_component()
