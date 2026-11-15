set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_bme680/bme680.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_bme680/dfrobot_bme680.c
)
set(HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/dfrobot_bme680)


if(DEFINED CONFIG_SAMPLE_SUPPORT_BME680_I2C_SAMPLE)
    add_subdirectory_if_exist(bme680_i2c_sample)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BME680_SPI_SAMPLE)
    add_subdirectory_if_exist(bme680_spi_sample)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${HEADER_LIST} PARENT_SCOPE)
