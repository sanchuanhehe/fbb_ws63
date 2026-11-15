/**
 * @file dfrobot_bme680_spi.c
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */
#include "dfrobot_bme680_spi.h"

#define SPI_SLAVE_NUM 1
#define SPI_FREQUENCY 1
#define SPI_CLK_POLARITY 0
#define SPI_CLK_PHASE 0
#define SPI_FRAME_FORMAT 0
#define SPI_FRAME_FORMAT_STANDARD 0
#define SPI_FRAME_SIZE_8 HAL_SPI_FRAME_SIZE_8
#define SPI_TMOD 0
#define SPI_WAIT_CYCLES 0x10

#define BME680_SPI_TIMEOUT 0xFFFFFFFF
#define BME680_MAX_TRANSFER_LEN 1
#define SPI_MASTER_PIN_MODE 3

uint8_t g_spi_bus_id;
static uint8_t g_bme680_cs_pin;

static void bme680_cs_low(void)
{
    uapi_gpio_set_val(g_bme680_cs_pin, GPIO_LEVEL_LOW); // CS低电平有效
}

static void bme680_cs_high(void)
{
    uapi_gpio_set_val(g_bme680_cs_pin, GPIO_LEVEL_HIGH);
}

static int8_t bme680_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    UNUSED(dev_id);
    errcode_t ret;
    uint8_t tx;
    uint8_t rx;

    /* 相当于 SPI.beginTransaction */
    bme680_cs_low();

    /* 先发寄存器地址（读操作最高位=1） */
    tx = reg_addr | 0x80;
    rx = 0;
    spi_xfer_data_t xfer_addr = {
        .tx_buff = &tx, .tx_bytes = BME680_MAX_TRANSFER_LEN, .rx_buff = &rx, .rx_bytes = BME680_MAX_TRANSFER_LEN};
    ret = uapi_spi_master_writeread(g_spi_bus_id, &xfer_addr, BME680_SPI_TIMEOUT);
    if (ret != ERRCODE_SUCC) {
        bme680_cs_high();
        return -1;
    }

    /* 循环逐字节读取数据 */
    while (len--) {
        tx = 0x00; /* dummy data */
        rx = 0;
        spi_xfer_data_t xfer_data = {
            .tx_buff = &tx, .tx_bytes = BME680_MAX_TRANSFER_LEN, .rx_buff = &rx, .rx_bytes = BME680_MAX_TRANSFER_LEN};
        ret = uapi_spi_master_writeread(g_spi_bus_id, &xfer_data, BME680_SPI_TIMEOUT);
        if (ret != ERRCODE_SUCC) {
            bme680_cs_high();
            return -1;
        }
        *data++ = rx;
    }

    bme680_cs_high();
    /* 相当于 SPI.endTransaction */
    return BME680_OK;
}

static int8_t bme680_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    UNUSED(dev_id);
    errcode_t ret;
    uint8_t tx;
    uint8_t rx;

    bme680_cs_low();

    /* 先发寄存器地址（写操作最高位=0） */
    tx = reg_addr & 0x7F;
    rx = 0;
    spi_xfer_data_t xfer_addr = {
        .tx_buff = &tx, .tx_bytes = BME680_MAX_TRANSFER_LEN, .rx_buff = &rx, .rx_bytes = BME680_MAX_TRANSFER_LEN};
    ret = uapi_spi_master_writeread(g_spi_bus_id, &xfer_addr, BME680_SPI_TIMEOUT);
    if (ret != ERRCODE_SUCC) {
        bme680_cs_high();
        return -1;
    }

    /* 循环逐字节写数据 */
    while (len--) {
        tx = *data++;
        rx = 0;
        spi_xfer_data_t xfer_data = {
            .tx_buff = &tx, .tx_bytes = BME680_MAX_TRANSFER_LEN, .rx_buff = &rx, .rx_bytes = BME680_MAX_TRANSFER_LEN};
        ret = uapi_spi_master_writeread(g_spi_bus_id, &xfer_data, BME680_SPI_TIMEOUT);
        if (ret != ERRCODE_SUCC) {
            bme680_cs_high();
            return -1;
        }
    }

    bme680_cs_high();
    return BME680_OK;
}

/* ---------------- BME680 SPI 初始化 ---------------- */
void dfrobot_bme680_spi_init(uint8_t pin_cs, uint8_t pin_miso, uint8_t pin_mosi, uint8_t pin_clk, uint8_t spi_bus_id)
{
    g_bme680_cs_pin = pin_cs;

    g_spi_bus_id = spi_bus_id;

    uapi_pin_set_mode(pin_miso, SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(pin_mosi, SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(pin_clk, SPI_MASTER_PIN_MODE);

    uapi_pin_set_mode(pin_cs, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin_cs, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(pin_cs, GPIO_LEVEL_HIGH);

    spi_attr_t config = {0};
    spi_extra_attr_t ext_config = {0};

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_CLK_FREQ;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 1;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;

    int ret = uapi_spi_init(g_spi_bus_id, &config, &ext_config);
    if (ret != 0) {
        printf("spi init fail %0x\r\n", ret);
    }

    dfrobot_bme680(bme680_spi_read, bme680_spi_write, bme680_delay_ms, BME680_INTERFACE_SPI);
}
