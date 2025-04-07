#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/lcd_drv9853.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/img_test.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/img.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/lcd_driver.c")
    

set(HEADER_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/lcd_drv9853"
    "${CMAKE_CURRENT_SOURCE_DIR}/img_test"
    "${CMAKE_CURRENT_SOURCE_DIR}/img"
    "${CMAKE_CURRENT_SOURCE_DIR}/lcd_driver")

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
Set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)