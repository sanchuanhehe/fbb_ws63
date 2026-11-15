set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_lis2dh12/dfrobot_lis2dh12.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_lis2dh12)


if(DEFINED CONFIG_SAMPLE_SUPPORT_ACCELERATION_SAMPLE)
    add_subdirectory_if_exist(get_acceleration_sample)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_INTERRUPT_SAMPLE)
    add_subdirectory_if_exist(interrupt_sample)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
