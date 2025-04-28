#===============================================================================
# @brief    cmake file.
# Copyright (c) Adragon
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/weather.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/wifista.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/http.c"
    PARENT_SCOPE)