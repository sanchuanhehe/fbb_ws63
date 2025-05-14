#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_AT_TEST)
    add_subdirectory_if_exist(at_test)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
set(LIBS "${LIBS}" PARENT_SCOPE)