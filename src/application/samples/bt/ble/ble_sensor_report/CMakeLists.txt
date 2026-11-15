#===============================================================================
# @brief    CMake file for the BLE Hello sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_CLIENT_SAMPLE)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/ble_sensor_report.c)

    if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_SERVER_SAMPLE)
        add_subdirectory_if_exist(ble_sensor_report_server)
    endif()

    if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_CLIENT_SAMPLE)
        add_subdirectory_if_exist(ble_sensor_report_client)
    endif()
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
