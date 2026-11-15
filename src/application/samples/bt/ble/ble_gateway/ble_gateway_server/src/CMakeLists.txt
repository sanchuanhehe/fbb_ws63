#===============================================================================
# @brief    CMake file for the BLE sensor-node sources.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_server_adv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_gateway_sensor.c"
    PARENT_SCOPE)
