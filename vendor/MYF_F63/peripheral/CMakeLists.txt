#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_BLINKY)
    add_subdirectory_if_exist(blinky)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BUTTON)
    add_subdirectory_if_exist(button)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_DMA)
    add_subdirectory_if_exist(dma)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_PWM)
    add_subdirectory_if_exist(pwm)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SYSTICK)
    add_subdirectory_if_exist(systick)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_TIMER)
    add_subdirectory_if_exist(timer)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_UART)
    add_subdirectory_if_exist(uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WDT)
    add_subdirectory_if_exist(watchdog)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)