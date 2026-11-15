set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_sensor_report.c)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_sensor_report_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_sensor_report_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
