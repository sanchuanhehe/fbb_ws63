set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/hal_bsp_nfc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/NT3H.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/nfc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/nfcForum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/rtdText.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/rtdUri.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/ndef.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc/rtdText.c
    ${CMAKE_CURRENT_SOURCE_DIR}/base_nfc_demo.c
)
set(HEADER_LIST 
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_bsp_nfc
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
