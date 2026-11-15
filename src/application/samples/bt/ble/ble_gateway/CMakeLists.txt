#===============================================================================
# @brief    CMake file for the BLE MQTT gateway sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
if(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_SERVER_SAMPLE OR
   CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway.c)

    if(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
        # IoTDA accepts ECDHE-based TLS suites. The WS63 default MbedTLS profile
        # removes those suites. Add the gateway config directory to the two
        # MbedTLS targets, then select the user config by its header name.
        function(ble_gateway_configure_mbedtls target_name)
            target_include_directories(${target_name} PRIVATE
                "${ROOT_DIR}/application/samples/bt/ble/ble_gateway/ble_gateway_client/src")
            target_compile_options(${target_name} PRIVATE
                -UMBEDTLS_USER_CONFIG_FILE
                "-DMBEDTLS_USER_CONFIG_FILE=\"ble_gateway_mbedtls_user_config.h\"")
        endfunction()

        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL
            ble_gateway_configure_mbedtls mbedtls_v3.6.0)
        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL
            ble_gateway_configure_mbedtls mbedtls_harden)
    endif()

    if(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_SERVER_SAMPLE)
        add_subdirectory_if_exist(ble_gateway_server)
    endif()

    if(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
        add_subdirectory_if_exist(ble_gateway_client)
    endif()
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}/inc" PARENT_SCOPE)
