set(
    SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_model_op.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_vnd_attr_cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mesh_vnd_attr_srv.c
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
