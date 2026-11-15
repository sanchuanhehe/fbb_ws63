#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_UART_POLL)
    add_subdirectory_if_exist(uart_poll)
endif()

if(DEFINED CONFIG_UART_INT)
    add_subdirectory_if_exist(uart_interrupt)
endif()
 
set(SOURCES "${SOURCES}" PARENT_SCOPE)