#===============================================================================
# @brief    CMake file for the BLE MQTT gateway role.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
add_subdirectory_if_exist(src)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../open_source/mqtt/paho.mqtt.c/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../open_source/mbedtls/mbedtls_v3.6.0/include"
    PARENT_SCOPE)
