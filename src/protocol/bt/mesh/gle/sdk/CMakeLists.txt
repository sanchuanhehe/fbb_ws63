set(
    sle_mesh_sdk_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_evt_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_transport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_route.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_scene.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_equip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_network.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_system.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mesh_sdk_dfx.c
)

set(MESH_SRC_LIST "${MESH_SRC_LIST}" "${sle_mesh_sdk_src_list}" CACHE INTERNAL "sle mesh sdk obj list" FORCE)

set(
    sle_mesh_sdk_header_list
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
set(MESH_HEADER_LIST ${MESH_HEADER_LIST} "${sle_mesh_sdk_header_list}" CACHE INTERNAL "sle mesh sdk header file list" FORCE)