set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_generics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_lighting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_time_scenes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_cfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_prov.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_health.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_vnd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_beacon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_priv_beacon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_ota.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_rmt_prov.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shell_message.c
)
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST} PARENT_SCOPE)