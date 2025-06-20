#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hilinkdevicesdk")

set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()

set(COMPONENT_NAME "hilinkota")

set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()

set(COMPONENT_NAME "hilinkbtsdk")

set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()

if (DEFINES MATCHES "SUPPORT_QUICK_NETCFG")
set(COMPONENT_NAME "hilinkquickcfg")
set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()
endif()

set(COMPONENT_NAME "hilink_addr_map")

set(CMAKE_HILINK_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_call_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_function_mapping.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hichain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_kv_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_mem_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_network_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_open_ota_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_open_ota_mcu_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_drbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_kdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_md.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_mpi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sal_rsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_socket_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_softap_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_stdio_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_str_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_sys_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_thread_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_time_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_tls_client.c

    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_ble_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_hilink_ble_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_cmsis_liteos2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_cJSON.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_mbed_tls.c

    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_oh_sle_connection_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_oh_sle_device_discovery.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/uapi_oh_sle_ssap_server.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/product/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/adapter/include/

    ${ROOT_DIR}/open_source/deviceauth/interfaces/innerkits/deviceauth_lite/

    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/app_uapi/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/

    ${ROOT_DIR}/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/open_source/CMSIS/CMSIS/RTOS2/Include/
    ${ROOT_DIR}/include/
    ${ROOT_DIR}/open_source/cjson/cjson
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include
)

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