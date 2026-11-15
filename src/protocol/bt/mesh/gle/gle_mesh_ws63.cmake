######################################### mesh start ###########################################
set(MESH_SRC_LIST  "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(core)
add_subdirectory_if_exist(adapter)
add_subdirectory_if_exist(util)
add_subdirectory_if_exist(app_adp)
add_subdirectory_if_exist(schedule)
add_subdirectory_if_exist(main)
add_subdirectory_if_exist(example)
add_subdirectory_if_exist(sdk)
add_subdirectory_if_exist(porting)

set(COMPONENT_NAME "sle_mesh")

if("${MESH_SRC_LIST}" STREQUAL "")
    set(MESH_SRC_LIST "__null__")
endif()

set(SOURCES
    ${MESH_SRC_LIST}
)

set(PUBLIC_HEADER
    ${PUBLIC_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adp/cmd/inc
    ${MESH_HEADER_LIST}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(WHOLE_LINK
    true
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/sdk/inc "*.h")
######################################### mesh end ###########################################

