set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SLEController.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306_oled.c
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES ${SOURCES} ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER ${PUBLIC_HEADER} ${HEADER_LIST} PARENT_SCOPE)