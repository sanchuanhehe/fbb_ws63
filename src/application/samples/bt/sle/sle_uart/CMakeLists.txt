#===============================================================================
# @brief    cmake file for SLE UART Bridge sample.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart.c)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_uart_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_uart_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
