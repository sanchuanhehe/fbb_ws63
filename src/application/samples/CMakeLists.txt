#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samples")

set(SOURCES
)

set(PUBLIC_HEADER
)

if(DEFINED CONFIG_ENABLE_BT_SAMPLE)
    add_subdirectory_if_exist(bt)
endif()
if(DEFINED CONFIG_ENABLE_PERIPHERAL_SAMPLE)
    add_subdirectory_if_exist(peripheral)
endif()

if(DEFINED CONFIG_ENABLE_OS_SAMPLE)
    add_subdirectory_if_exist(os)
endif()

if(DEFINED CONFIG_ENABLE_WIFI_SAMPLE)
    add_subdirectory_if_exist(wifi_test)
endif()
if(DEFINED CONFIG_ENABLE_PRODUCTS_SAMPLE)
    add_subdirectory_if_exist(products)
endif()
if(DEFINED CONFIG_ENABLE_RADAR_SAMPLE)
    add_subdirectory_if_exist(radar_test)
endif()
if(DEFINED CONFIG_ENABLE_NFC_SAMPLE)
    add_subdirectory_if_exist(nfc)
endif()

add_subdirectory_if_exist(custom)

set(PRIVATE_HEADER
)

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_WIFI_CFG_SAMPLE)
    list(APPEND PRIVATE_HEADER
        "${CMAKE_CURRENT_SOURCE_DIR}/bt/ble/ble_wifi_cfg_server/inc"
    )
endif()


set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

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

install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/os" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/products" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/sta_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/alilink_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/softap_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/ohos_connect" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/hilink_indie_upgrade" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/ble_wifi_cfg_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/radar_test" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/nfc" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/ble" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/sle" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/sle_chba" "*")

build_component()

set(COMPONENT_NAME "mesh_samples")

set(SOURCES
)

set(PUBLIC_HEADER
)

set(COMPONENT_CCFLAGS
)

if((DEFINED CONFIG_ENABLE_BT_SAMPLE) AND (DEFINED CONFIG_SAMPLE_SUPPORT_BLE_MESH) AND (${CHIP} MATCHES "ws63"))
    add_subdirectory_if_exist(bt/ble/mesh)
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)
if((DEFINED CONFIG_ENABLE_BT_SAMPLE) AND (DEFINED CONFIG_SAMPLE_SUPPORT_BLE_MESH) AND (${CHIP} MATCHES "ws63"))
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/ble/mesh" "*")
else ()
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/sle/mesh" "*")
endif()
 
build_component()