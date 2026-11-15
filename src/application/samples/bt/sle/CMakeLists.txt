#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SPEED_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_speed_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SPEED_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_speed_client)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_device_config)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_SLE_FRAGMENTATION_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_fragmentation)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_HELLO_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_hello)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_hello)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE OR DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_sensor_report)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_conn_param_tuning)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_phy_mcs_switch)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE OR
   DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_rssi_ranging)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
