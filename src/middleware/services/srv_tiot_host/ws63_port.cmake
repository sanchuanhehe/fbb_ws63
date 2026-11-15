#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2024-2024. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2024-05-13
]]

if(DEFINED CONFIG_TIOT_PORTING_WSXX_SLP OR "SUPPORT_SLP_RADAR_CLIENT" IN_LIST DEFINES)
set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/ws63_w33_evb)
set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/ws63_w33_evb)
endif()
