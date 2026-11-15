set(
    SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npl_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_atomic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_eventq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_callout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_sem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_list.c
)

set(
    HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nimble
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST} PARENT_SCOPE)

