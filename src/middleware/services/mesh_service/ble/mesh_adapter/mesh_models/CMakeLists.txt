add_subdirectory(src)
add_subdirectory(common)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST} PARENT_SCOPE)