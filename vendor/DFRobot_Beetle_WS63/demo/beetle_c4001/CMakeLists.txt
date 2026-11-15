set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_c4001/dfrobot_c4001.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_c4001)


if(DEFINED CONFIG_SAMPLE_SUPPORT_MOTION_SAMPLE)
    add_subdirectory_if_exist(motion_detection_sample)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_MRANGE_SAMPLE)
    add_subdirectory_if_exist(range_velocity_sample)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
