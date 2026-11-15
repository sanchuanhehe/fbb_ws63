#===============================================================================
# @brief    CMake file for the BLE MQTT gateway client sources.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_client.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_iotda.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_iotda_ca.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_wifi.c"
    PARENT_SCOPE)
