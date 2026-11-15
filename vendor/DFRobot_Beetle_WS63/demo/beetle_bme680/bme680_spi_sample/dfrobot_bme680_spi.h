/**
 * @file dfrobot_bme680_spi.h
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#ifndef DFROBOT_BME680_SPI_H
#define DFROBOT_BME680_SPI_H

#include "dfrobot_bme680.h"
#include "spi.h"

void dfrobot_bme680_spi_init(uint8_t pin_cs, uint8_t pin_miso, uint8_t pin_mosi, uint8_t pin_clk, uint8_t spi_bus_id);

#endif