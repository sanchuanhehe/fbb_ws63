#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "syschannel_dev")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/channel_dev/syschannel_dev_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool/syschannel_mem_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wal/lwip_adapt/syschannel_wal_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/syschannel_netbuf/lwip_adapt/syschannel_netbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/repeater/syschannel_dev_filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common/syschannel_common.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_api
    ${CMAKE_CURRENT_SOURCE_DIR}/wal
    ${CMAKE_CURRENT_SOURCE_DIR}/syschannel_netbuf
    ${CMAKE_CURRENT_SOURCE_DIR}/channel_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool
    ${CMAKE_CURRENT_SOURCE_DIR}/repeater
    ${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common
    ${ROOT_DIR}/kernel/osal_adapt/inc
)

set(PRIVATE_DEFINES
    PBUF_ZERO_COPY_RESERVE=80
    SYSCHANNEL_LITTLE_ENDIAN=1
    SYSCHANNEL_BIG_ENDIAN=2
    SYSCHANNEL_ENDIAN=SYSCHANNEL_LITTLE_ENDIAN
    _PRE_OS_VERSION_LITEOS=6
    _PRE_OS_VERSION=_PRE_OS_VERSION_LITEOS
    _PRE_FEATURE_SYSCHANNEL_LITEOS
)

set(PUBLIC_DEFINES
    #_PRE_LWIP_SYSCHANNEL_MEM_ALLOC_BUF
    _PRE_SYSCHANNEL_FEATURE
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -fno-inline-small-functions
    -fno-inline-functions-called-once
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(syschannel_dev IN_LIST TARGET_COMPONENT)
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/channel_dev" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/mempool" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/repeater" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_common" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/../syschannel_api" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wal" "*.h")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/syschannel_netbuf" "*.h")
endif()

build_component()