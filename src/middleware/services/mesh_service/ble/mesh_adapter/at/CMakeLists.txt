set(AT_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "at header path"
)
if(${MESH_OS} MATCHES "linux")
    add_subdirectory(linux)
endif()