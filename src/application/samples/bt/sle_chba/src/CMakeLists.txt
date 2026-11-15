#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_chba_link_mng.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_chba_server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_chba_netif_mng.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sle_chba_bridge.c"
    PARENT_SCOPE)