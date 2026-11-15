#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SPEED_SERVER_SAMPLE)
    add_subdirectory_if_exist(ble_speed_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SPEED_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_speed_client)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_CFG_SAMPLE)
    add_subdirectory_if_exist(ble_wifi_cfg_client)
    add_subdirectory_if_exist(ble_wifi_cfg_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART_BRIDGE_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_uart_bridge)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SENSOR_REPORT_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_sensor_report)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_gateway)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_HID_BTN_SAMPLE)
    add_subdirectory_if_exist(ble_hid_btn)
endif()

add_subdirectory_if_exist(ble_hello)

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
