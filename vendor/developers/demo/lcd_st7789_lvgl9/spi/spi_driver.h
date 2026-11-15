/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */

#ifndef _SPI_DRIVER_H
#define _SPI_DRIVER_H

#include "pinctrl.h"
#include "spi.h"
#include "soc_osal.h"
#include "app_init.h"
#include "gpio.h"

#define SPI_FREQUENCY 32
#define SPI_CLK_POLARITY 0
#define SPI_CLK_PHASE 0
#define SPI_SLAVE_NUM 1

#define GPIO_PINMODE 0
#define SPI_PINMODE 3

void spi_init(void);
void send_cmd(uint8_t cmd);
void send_data(uint8_t data);
void send_data_array(uint8_t *data, uint32_t len);

#endif
