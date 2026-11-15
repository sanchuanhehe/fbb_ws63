/**
 * @file dfrobot_bme680.c
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */
#include <math.h>
#include "dfrobot_bme680.h"

static struct bme680_dev g_bme680_sensor;
static struct bme680_field_data g_bme680_data;
static uint8_t g_convert_cmd = (BME680_CONVERT_CMD_OS_TEMP_VAL << BME680_CONVERT_CMD_OS_TEMP_SHIFT) |
                               (BME680_CONVERT_CMD_OS_PRES_VAL << BME680_CONVERT_CMD_OS_PRES_SHIFT) |
                               BME680_CONVERT_CMD_MODE_VAL;

uint8_t g_bme680_i2c_addr = BME680_INIT_SUCCESS;

void bme680_delay_ms(uint32_t period)
{
    uapi_systick_delay_ms(period);
}

void dfrobot_bme680(bme680_com_fptr_t read_reg,
                    bme680_com_fptr_t write_reg,
                    bme680_delay_fptr_t delay_ms,
                    e_bme680_interface interface)
{
    g_bme680_sensor.read = read_reg;
    g_bme680_sensor.write = write_reg;
    g_bme680_sensor.delay_ms = delay_ms;
    switch (interface) {
        case BME680_INTERFACE_I2C:
            g_bme680_sensor.intf = BME680_I2C_INTF;
            break;
        case BME680_INTERFACE_SPI:
            g_bme680_sensor.intf = BME680_SPI_INTF;
            break;
    }
}

int16_t bme680_bme680_begin(void)
{
    g_bme680_sensor.dev_id = g_bme680_i2c_addr;
    if (bme680_init(&g_bme680_sensor) != BME680_OK) {
        return BME680_INIT_FAILURE;
    }

    printf("bme begin step1\r\n");

    uint8_t set_required_settings;

    /* Set the temperature, pressure and humidity settings */
    g_bme680_sensor.tph_sett.os_hum = BME680_DEFAULT_OS_HUM;
    g_bme680_sensor.tph_sett.os_pres = BME680_DEFAULT_OS_PRES;
    g_bme680_sensor.tph_sett.os_temp = BME680_DEFAULT_OS_TEMP;
    g_bme680_sensor.tph_sett.filter = BME680_DEFAULT_FILTER;

    /* Set the remaining gas sensor settings and link the heating profile */
    g_bme680_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    g_bme680_sensor.gas_sett.heatr_temp = BME680_DEFAULT_HEATR_TEMP; /* degree Celsius */
    g_bme680_sensor.gas_sett.heatr_dur = BME680_DEFAULT_HEATR_DUR;   /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    g_bme680_sensor.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings =
        BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    bme680_set_sensor_settings(set_required_settings, &g_bme680_sensor);

    /* Set the power mode */
    bme680_set_sensor_mode(&g_bme680_sensor);

    /* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
    uint16_t meas_period;
    bme680_get_profile_dur(&meas_period, &g_bme680_sensor);
    g_bme680_sensor.delay_ms(meas_period); /* Delay till the measurement is ready */
    return BME680_INIT_SUCCESS;
}

void start_convert(void)
{
    g_bme680_sensor.write(g_bme680_sensor.dev_id, BME680_REG_CTRL_TEMP_PRES, &g_convert_cmd, BME680_REG_DATA_SIZE);
}

void bme680_bme680_update(void)
{
    bme680_get_sensor_data(&g_bme680_data, &g_bme680_sensor);
}

float read_temperature(void)
{
    return g_bme680_data.temperature;
}

float read_pressure(void)
{
    return g_bme680_data.pressure;
}

float read_humidity(void)
{
    return g_bme680_data.humidity;
}

float read_altitude(void)
{
    return (1.0f - pow((float)g_bme680_data.pressure / BME680_PRESSURE_TO_HPA_DIVISOR / BME680_SEALEVEL,
                       BME680_ALTITUDE_EXPONENT)) *
           BME680_ALTITUDE_COEFFICIENT_1 / BME680_ALTITUDE_COEFFICIENT_2;
}

float read_calibrated_altitude(float sea_level)
{
    return (1.0f - pow((float)g_bme680_data.pressure / sea_level, BME680_ALTITUDE_EXPONENT)) *
           BME680_ALTITUDE_COEFFICIENT_1 / BME680_ALTITUDE_COEFFICIENT_2;
}

float read_gas_resistance(void)
{
    return g_bme680_data.gas_resistance;
}

float read_sea_level(float altitude)
{
    return (g_bme680_data.pressure /
            pow(1.0f - (altitude / BME680_SEA_LEVEL_ALTITUDE_BASE), BME680_SEA_LEVEL_EXPONENT));
}

void set_param(e_bme680_param_t e_param, uint8_t dat)
{
    if (dat > BME680_PARAM_MAX_VALUE) {
        return;
    }

    switch (e_param) {
        case BME680_PARAM_TEMPSAMP:
            write_param_helper(BME680_REG_CTRL_TEMP_PRES, dat, BME680_PARAM_MASK << BME680_PARAM_TEMP_SHIFT);
            break;
        case BME680_PARAM_PREESAMP:
            write_param_helper(BME680_REG_CTRL_TEMP_PRES, dat, BME680_PARAM_MASK << BME680_PARAM_PRES_SHIFT);
            break;
        case BME680_PARAM_HUMISAMP:
            write_param_helper(BME680_REG_CTRL_HUM, dat, BME680_PARAM_MASK << BME680_PARAM_HUM_SHIFT);
            break;
        case BME680_PARAM_IIRSIZE:
            write_param_helper(BME680_REG_CTRL_FILTER, dat, BME680_PARAM_MASK << BME680_PARAM_FILTER_SHIFT);
            break;
    }
}

void set_gas_heater(uint16_t temp, uint16_t t)
{
    UNUSED(temp);
    UNUSED(t);
    g_bme680_sensor.gas_sett.heatr_temp = BME680_DEFAULT_HEATR_TEMP; /* degree Celsius */
    g_bme680_sensor.gas_sett.heatr_dur = BME680_DEFAULT_HEATR_DUR;   /* milliseconds */
    uint8_t set_required_settings = BME680_GAS_SENSOR_SEL;
    bme680_set_sensor_settings(set_required_settings, &g_bme680_sensor);
}

void write_param_helper(uint8_t reg, uint8_t dat, uint8_t addr)
{
    uint8_t var1 = BME680_INIT_SUCCESS;
    uint8_t addr_count = BME680_INIT_SUCCESS;
    if (g_bme680_sensor.intf == BME680_SPI_INTF) {
        uint8_t spi_page_reset = BME680_REG_SPI_PAGE_RESET;
        g_bme680_sensor.write(g_bme680_sensor.dev_id, BME680_REG_CTRL_SPI_PAGE, &spi_page_reset, BME680_REG_DATA_SIZE);
    }
    g_bme680_sensor.read(g_bme680_sensor.dev_id, reg, &var1, BME680_REG_DATA_SIZE);
    var1 &= ~addr;
    while (!(addr & BME680_BIT_MASK_LSB)) {
        addr_count++;
        addr >>= 1;
    }
    var1 |= dat << addr_count;
    g_bme680_sensor.write(g_bme680_sensor.dev_id, reg, &var1, BME680_REG_DATA_SIZE);
}
