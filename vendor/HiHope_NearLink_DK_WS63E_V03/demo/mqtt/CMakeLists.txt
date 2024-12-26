set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi/wifi_connect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/beep/pwm_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cjson/cjson_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/aht20_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/aht20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/ssd1306_fonts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/ssd1306.c
)
set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi 
    ${CMAKE_CURRENT_SOURCE_DIR}/cjson
    ${CMAKE_CURRENT_SOURCE_DIR}/environment
    ${CMAKE_CURRENT_SOURCE_DIR}/beep
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
