#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_AT_TEST)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_client/sle_uuid_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_server/sle_uuid_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_server/sle_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at_test.c
)

set(HEADER_LIST 
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_client/
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_server/
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)