
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_heartbeat.c
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
