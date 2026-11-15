#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART)
    add_subdirectory_if_exist(beetle_ble_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART)
    add_subdirectory_if_exist(beetle_sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_MQTT)
    add_subdirectory_if_exist(beetle_mqtt)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_URM37)
    add_subdirectory_if_exist(beetle_urm37)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BUTTON)
    add_subdirectory_if_exist(beetle_button)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_DS18B20)
    add_subdirectory_if_exist(beetle_ds18b20)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_DF9GMS)
    add_subdirectory_if_exist(beetle_df9gms)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_C4001)
    add_subdirectory_if_exist(beetle_c4001)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_LIS2DH12)
    add_subdirectory_if_exist(beetle_lis2dh12)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SSD1306)
    add_subdirectory_if_exist(beetle_ssd1306)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BME680)
    add_subdirectory_if_exist(beetle_bme680)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)

