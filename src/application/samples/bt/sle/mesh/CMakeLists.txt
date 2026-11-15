set(SOURCES
)
 
set(PUBLIC_HEADER
)
 
set(COMPONENT_PUBLIC_CCFLAGS
)
 
set(COMPONENT_CCFLAGS
)

add_subdirectory_if_exist(one_touch_start)
add_subdirectory_if_exist(profile)
add_subdirectory_if_exist(led_ctrl)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
set(COMPONENT_CCFLAGS "${COMPONENT_CCFLAGS}" ${COMPONENT_CCFLAGS} PARENT_SCOPE)