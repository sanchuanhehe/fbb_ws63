#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "alilink")

if (DEFINED CONFIG_SAMPLE_SUPPORT_ALILINK_SAMPLE)
set(CMAKE_ALILINK_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ${CMAKE_ALILINK_SOURCE_DIR}/alilink_sample.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/demos/mqtt_basic_demo.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/aiot_mqtt_api.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/sysdep/core_adapter.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/sysdep/core_sysdep.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/aiot_state_api.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_sha256.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_auth.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_global.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_log.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_string.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/core_diag.c
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/portfiles/aiot_port/posix_port.c
)

set(PUBLIC_HEADER
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/sysdep/
)

set(PRIVATE_HEADER
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/utils/
    ${CMAKE_ALILINK_SOURCE_DIR}/LinkSDK/core/sysdep/
)
endif()
# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=logical-op
    -Wno-error=sign-compare
    -Wno-error=jump-misses-init
    -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-error=unused-variable
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)
set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()