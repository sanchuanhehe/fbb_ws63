if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_01_TASK)
    add_subdirectory_if_exist(kernel_01_task)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_02_TIMER)
    add_subdirectory_if_exist(kernel_02_timer)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_03_EVENT)
    add_subdirectory_if_exist(kernel_03_event)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_04_MUTEX)
    add_subdirectory_if_exist(kernel_04_mutex)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_05_MUTEX_SEMAPHORE)
    add_subdirectory_if_exist(kernel_05_mutex_Semaphore)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_06_SYNC_SEMAPHORE)
    add_subdirectory_if_exist(kernel_06_sync_Semaphore)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_07_COUNT_SEMAPHORE)
    add_subdirectory_if_exist(kernel_07_count_Semaphore)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_KERNEL_08_MESSGE_QUEQUE)
    add_subdirectory_if_exist(kernel_08_message_queque)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_LEDBLINK)
    add_subdirectory_if_exist(base_01_ledblink)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_UART)
    add_subdirectory_if_exist(base_02_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OLED_SSD1306)
    add_subdirectory_if_exist(base_03_ssd1306)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_RGB)
    add_subdirectory_if_exist(base_04_rgb)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SHT20)
    add_subdirectory_if_exist(base_05_sht20)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_AP3216)
    add_subdirectory_if_exist(base_06_ap3216)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_LCD)
    add_subdirectory_if_exist(base_07_spi_lcd)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_01_STA)
    add_subdirectory_if_exist(wifi_01_sta)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_02_AP)
    add_subdirectory_if_exist(wifi_02_ap)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_03_UDP)
    add_subdirectory_if_exist(wifi_03_udp)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_04_TCP)
    add_subdirectory_if_exist(wifi_04_tcp)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_05_MQTT)
    add_subdirectory_if_exist(wifi_05_mqtt)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WIFI_06_HUAWEI_IOT)
    add_subdirectory_if_exist(wifi_06_huawei_iot)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_TRANS_CLIENT)
    add_subdirectory_if_exist(ble_01_trans_client)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_TRANS_SERVER)
    add_subdirectory_if_exist(ble_02_trans_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_TRANS_SERVER)
    add_subdirectory_if_exist(sle_01_trans_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_TRANS_CLIENT)
    add_subdirectory_if_exist(sle_02_trans_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)