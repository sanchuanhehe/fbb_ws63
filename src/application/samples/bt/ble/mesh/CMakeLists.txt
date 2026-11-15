set(SOURCES
)
 
set(PUBLIC_HEADER
)
 
set(COMPONENT_PUBLIC_CCFLAGS
)
 
set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-label
    -Wno-unused-function
    -Wno-unused-but-set-variable
    -Wno-sign-compare
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-builtin-declaration-mismatch
)

if(DEFINED CONFIG_BLE_MESH_PROVISIONER_ADV)
    add_subdirectory_if_exist(provisioner_adv)
endif()

if(DEFINED CONFIG_BLE_MESH_DEVICE)
    add_subdirectory_if_exist(mesh_device)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
set(COMPONENT_CCFLAGS "${COMPONENT_CCFLAGS}" ${COMPONENT_CCFLAGS} PARENT_SCOPE)