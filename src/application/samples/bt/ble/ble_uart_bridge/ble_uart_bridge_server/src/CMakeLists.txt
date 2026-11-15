#===============================================================================
# @brief    CMake file for the BLE Hello server sources.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_bridge_server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_bridge_server_adv.c"
    PARENT_SCOPE)
