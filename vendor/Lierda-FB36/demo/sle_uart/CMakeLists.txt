#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER)
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server_adv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server.c"
    PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server" PARENT_SCOPE)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_client/sle_uart_client.c"
    PARENT_SCOPE)
endif()