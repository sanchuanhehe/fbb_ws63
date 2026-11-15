/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */
#include "spi_driver.h"

void spi_init(void)
{
    uapi_pin_set_mode(CONFIG_SPI_DI_MASTER_PIN, SPI_PINMODE);
    uapi_pin_set_mode(CONFIG_SPI_DO_MASTER_PIN, SPI_PINMODE);
    uapi_pin_set_mode(CONFIG_SPI_CLK_MASTER_PIN, SPI_PINMODE);
    uapi_pin_set_mode(CONFIG_SPI_CS_MASTER_PIN, SPI_PINMODE);

    uapi_pin_set_mode(CONFIG_SPI_RESET_MASTER_PIN, GPIO_PINMODE);
    uapi_gpio_set_dir(CONFIG_SPI_RESET_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_pin_set_mode(CONFIG_SPI_DC_MASTER_PIN, GPIO_PINMODE);
    uapi_gpio_set_dir(CONFIG_SPI_DC_MASTER_PIN, GPIO_DIRECTION_OUTPUT);

    spi_attr_t config = {0};
    spi_extra_attr_t ext_config = {0};

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_CLK_FREQ;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = HAL_SPI_TRANS_MODE_TX;
    config.sste = 0;

    int ret = uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &config, &ext_config);
    if (ret != 0) {
        osal_printk("spi init fail %0x\r\n", ret);
    }
    uapi_dma_init();
    uapi_dma_open();
}

void send_cmd(uint8_t cmd)
{
    uint8_t d = cmd;
    spi_xfer_data_t spi_data = {
        .tx_buff = &d,
        .tx_bytes = 1,
    };
    uapi_gpio_set_val(CONFIG_SPI_DC_MASTER_PIN, GPIO_LEVEL_LOW);
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &spi_data, 0xFFFFFFFF);
}

void send_data(uint8_t data)
{

    uint8_t d = data;
    spi_xfer_data_t spi_data = {
        .tx_buff = &d,
        .tx_bytes = 1,
    };
    uapi_gpio_set_val(CONFIG_SPI_DC_MASTER_PIN, GPIO_LEVEL_HIGH);
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &spi_data, 0xFFFFFFFF);
}

void send_data_array(uint8_t *data, uint32_t len)
{
    spi_xfer_data_t spi_data = {
        .tx_buff = data,
        .tx_bytes = len,
    };
    uapi_gpio_set_val(CONFIG_SPI_DC_MASTER_PIN, GPIO_LEVEL_HIGH);
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &spi_data, 0xFFFFFFFF);
}