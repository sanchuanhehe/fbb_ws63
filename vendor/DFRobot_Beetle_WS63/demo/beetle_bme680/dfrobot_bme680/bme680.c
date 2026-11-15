/**
 * @file bme680.c
 * @brief Sensor driver for BME680 sensor.
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#include "bme680.h"

/**<static variables */
/**<Look up table for the possible gas range values */
uint32_t g_lookup_table1[16] = {UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647),
                                UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2130303777),
                                UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2143188679), UINT32_C(2136746228),
                                UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2147483647)};
/**<Look up table for the possible gas range values */
uint32_t g_lookup_table2[16] = {UINT32_C(4096000000), UINT32_C(2048000000), UINT32_C(1024000000), UINT32_C(512000000),
                                UINT32_C(255744255),  UINT32_C(127110228),  UINT32_C(64000000),   UINT32_C(32258064),
                                UINT32_C(16016016),   UINT32_C(8000000),    UINT32_C(4000000),    UINT32_C(2000000),
                                UINT32_C(1000000),    UINT32_C(500000),     UINT32_C(250000),     UINT32_C(125000)};

/**
 * @fn get_calib_data
 * @brief This internal API is used to read the calibrated data from the sensor.
 * @n This function is used to retrieve the calibration data from the image registers of the sensor.
 * @note Registers 89h  to A1h for calibration data 1 to 24  from bit 0 to 7
 * @note Registers E1h to F0h for calibration data 25 to 40 from bit 0 to 7
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_calib_data(struct bme680_dev *dev);
static void parse_temp_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev);
static void parse_pressure_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev);
static void parse_humidity_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev);
static void parse_gas_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev);
static int8_t parse_other_coefficients(struct bme680_dev *dev);
static int8_t interleave_reg_data(const uint8_t *reg_addr,
                                  const uint8_t *reg_data,
                                  uint8_t len,
                                  uint8_t *tmp_buff,
                                  struct bme680_dev *dev);

/**
 * @fn set_gas_config
 * @brief This internal API is used to set the gas configuration of the sensor.
 *
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_gas_config(struct bme680_dev *dev);

/**
 * @fn get_gas_config
 * @brief This internal API is used to get the gas configuration of the sensor.
 * @param dev    :Structure instance of bme680_dev.
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_gas_config(struct bme680_dev *dev);

/**
 * @fn calc_heater_dur
 * @brief This internal API is used to calculate the Heat duration value.
 * @param dur    :Value of the duration to be shared.
 * @return uint8_t threshold duration after calculation.
 */
static uint8_t calc_heater_dur(uint16_t dur);

/**
 * @fn calc_temperature
 * @brief This internal API is used to calculate the temperature value.
 * @param temp_adc    :Contains the temperature ADC value .
 * @param dev    :Structure instance of bme680_dev.
 * @return uint32_t calculated temperature.
 */
static int16_t calc_temperature(uint32_t temp_adc, struct bme680_dev *dev);

/**
 * @fn calc_pressure
 * @brief This internal API is used to calculate the pressure value.
 * @param pres_adc    :Contains the pressure ADC value .
 * @param dev    :Structure instance of bme680_dev.
 * @return uint32_t calculated pressure.
 */
static uint32_t calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev);

/**
 * @fn calc_humidity
 * @brief This internal API is used to calculate the humidity value.
 * @param hum_adc    :Contains the humidity ADC value.
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return uint32_t calculated humidity.
 */
static uint32_t calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev);

/**
 * @fn calc_heater_res
 * @brief This internal API is used to calculate the Heat Resistance value.
 * @param temp    :Contains the temporary value.
 * @param dev    :Structure instance of bme680_dev.
 * @return uint8_t calculated heater resistance.
 */
static uint8_t calc_heater_res(uint16_t temp, const struct bme680_dev *dev);

/**
 * @fn read_field_data
 * @brief This internal API is used to calculate the field data of sensor.
 *
 * @param data :Structure instance to hold the data
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return int8_t result of the field data from sensor.
 */
static int8_t read_field_data(struct bme680_field_data *data, struct bme680_dev *dev);

/**
 * @fn set_mem_page
 * @brief This internal API is used to set the memory page based on register address.
 *
 * @n The value of memory page
 * @n  value  | Description
 * @n --------|--------------
 * @n   0     | BME680_PAGE0_SPI
 * @n   1     | BME680_PAGE1_SPI
 *
 * @param reg_addr    :Contains the register address array.
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_mem_page(uint8_t reg_addr, struct bme680_dev *dev);

/**
 * @fn get_mem_page
 * @brief This internal API is used to get the memory page based on register address.
 * @n The value of memory page
 * @n  value  | Description
 * @n --------|--------------
 * @n   0     | BME680_PAGE0_SPI
 * @n   1     | BME680_PAGE1_SPI
 *
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t get_mem_page(struct bme680_dev *dev);

/**
 * @fn null_ptr_check
 * @brief This internal API is used to validate the device pointer for null conditions.
 *
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t null_ptr_check(const struct bme680_dev *dev);

/**
 * @fn boundary_check
 * @brief This internal API is used to check the boundary conditions.
 *
 * @param value    :pointer to the value.
 * @param min    :minimum value.
 * @param max    :maximum value.
 * @param dev    :Structure instance of bme680_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t boundary_check(uint8_t *value, uint8_t min, uint8_t max, struct bme680_dev *dev);

/**
 * @fn configure_filter_settings
 * @brief Configure filter settings for the sensor.
 * @param desired_settings : Desired settings mask.
 * @param reg_array : Register address array.
 * @param data_array : Register data array.
 * @param count : Current count in arrays.
 * @param dev : Structure instance of bme680_dev.
 * @return Result of API execution status.
 */
static int8_t configure_filter_settings(uint16_t desired_settings,
                                        uint8_t *reg_array,
                                        uint8_t *data_array,
                                        uint8_t *count,
                                        struct bme680_dev *dev);

/**
 * @fn configure_heater_control
 * @brief Configure heater control settings for the sensor.
 * @param desired_settings : Desired settings mask.
 * @param reg_array : Register address array.
 * @param data_array : Register data array.
 * @param count : Current count in arrays.
 * @param dev : Structure instance of bme680_dev.
 * @return Result of API execution status.
 */
static int8_t configure_heater_control(uint16_t desired_settings,
                                       uint8_t *reg_array,
                                       uint8_t *data_array,
                                       uint8_t *count,
                                       struct bme680_dev *dev);

/**
 * @fn configure_tph_oversampling
 * @brief Configure temperature and pressure oversampling settings.
 * @param desired_settings : Desired settings mask.
 * @param reg_array : Register address array.
 * @param data_array : Register data array.
 * @param count : Current count in arrays.
 * @param dev : Structure instance of bme680_dev.
 * @return Result of API execution status.
 */
static int8_t configure_tph_oversampling(uint16_t desired_settings,
                                         uint8_t *reg_array,
                                         uint8_t *data_array,
                                         uint8_t *count,
                                         struct bme680_dev *dev);

/**
 * @fn configure_humidity_oversampling
 * @brief Configure humidity oversampling settings.
 * @param desired_settings : Desired settings mask.
 * @param reg_array : Register address array.
 * @param data_array : Register data array.
 * @param count : Current count in arrays.
 * @param dev : Structure instance of bme680_dev.
 * @return Result of API execution status.
 */
static int8_t configure_humidity_oversampling(uint16_t desired_settings,
                                              uint8_t *reg_array,
                                              uint8_t *data_array,
                                              uint8_t *count,
                                              struct bme680_dev *dev);

/**
 * @fn configure_gas_settings
 * @brief Configure gas measurement settings.
 * @param desired_settings : Desired settings mask.
 * @param reg_array : Register address array.
 * @param data_array : Register data array.
 * @param count : Current count in arrays.
 * @param dev : Structure instance of bme680_dev.
 * @return Result of API execution status.
 */
static int8_t configure_gas_settings(uint16_t desired_settings,
                                     uint8_t *reg_array,
                                     uint8_t *data_array,
                                     uint8_t *count,
                                     struct bme680_dev *dev);

/**
 * @fn calculate_tph_duration
 * @brief Calculate TPH (Temperature, Pressure, Humidity) measurement duration.
 * @param dev : Structure instance of bme680_dev.
 * @return Calculated TPH duration in milliseconds.
 */
static uint32_t calculate_tph_duration(const struct bme680_dev *dev);

/****************** Global Function Definitions *******************************/
int8_t bme680_init(struct bme680_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    /* Soft reset to restore it to default values */
    rslt = bme680_soft_reset(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    rslt = bme680_get_regs(BME680_CHIP_ID_ADDR, &dev->chip_id, 1, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if (dev->chip_id != BME680_CHIP_ID) {
        return BME680_E_DEV_NOT_FOUND;
    }

    /* Get the Calibration data */
    rslt = get_calib_data(dev);

    return rslt;
}

int8_t bme680_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len, struct bme680_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        if (dev->intf == BME680_SPI_INTF) {
            /* Set the memory page */
            rslt = set_mem_page(reg_addr, dev);
            if (rslt == BME680_OK) {
                reg_addr = reg_addr | BME680_SPI_RD_MSK;
            }
        }
        dev->com_rslt = dev->read(dev->dev_id, reg_addr, reg_data, len);
        if (dev->com_rslt != 0) {
            rslt = BME680_E_COM_FAIL;
        }
    }

    return rslt;
}

int8_t bme680_set_regs(const uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len, struct bme680_dev *dev)
{
    int8_t rslt;
    /* Length of the temporary buffer is 2*(length of register) */
    uint8_t tmp_buff[BME680_TMP_BUFFER_LENGTH] = {0};

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if ((len == 0) || (len >= BME680_TMP_BUFFER_HALF)) {
        return BME680_E_INVALID_LENGTH;
    }

    /* Interleave the 2 arrays */
    rslt = interleave_reg_data(reg_addr, reg_data, len, tmp_buff, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    /* Write the interleaved array */
    dev->com_rslt = dev->write(dev->dev_id, tmp_buff[BME680_TMP_BUFF_REG_ADDR_INDEX],
                               &tmp_buff[BME680_TMP_BUFF_REG_DATA_INDEX], (BME680_REG_PAIR_MULTIPLIER * len) - 1);
    if (dev->com_rslt != 0) {
        return BME680_E_COM_FAIL;
    }

    return BME680_OK;
}

int8_t bme680_soft_reset(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME680_SOFT_RESET_ADDR;
    /* 0xb6 is the soft reset command */
    uint8_t soft_rst_cmd = BME680_SOFT_RESET_CMD;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if (dev->intf == BME680_SPI_INTF) {
        rslt = get_mem_page(dev);
        if (rslt != BME680_OK) {
            return rslt;
        }
    }

    /* Reset the device */
    rslt = bme680_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);
    /* Wait for 5ms */
    dev->delay_ms(BME680_RESET_PERIOD);

    if (rslt != BME680_OK) {
        return rslt;
    }

    /* After reset get the memory page */
    if (dev->intf == BME680_SPI_INTF) {
        rslt = get_mem_page(dev);
    }

    return rslt;
}

int8_t bme680_set_sensor_settings(uint16_t desired_settings, struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t count = 0;
    uint8_t reg_array[BME680_REG_BUFFER_LENGTH] = {0};
    uint8_t data_array[BME680_REG_BUFFER_LENGTH] = {0};
    uint8_t intended_power_mode = dev->power_mode;

    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if (desired_settings & BME680_GAS_MEAS_SEL) {
        rslt = set_gas_config(dev);
        if (rslt != BME680_OK) {
            return rslt;
        }
    }

    dev->power_mode = BME680_SLEEP_MODE;
    rslt = bme680_set_sensor_mode(dev);
    if (rslt != BME680_OK) {
        dev->power_mode = intended_power_mode;
        return rslt;
    }

    rslt = configure_filter_settings(desired_settings, reg_array, data_array, &count, dev);
    if (rslt == BME680_OK) {
        rslt = configure_heater_control(desired_settings, reg_array, data_array, &count, dev);
    }
    if (rslt == BME680_OK) {
        rslt = configure_tph_oversampling(desired_settings, reg_array, data_array, &count, dev);
    }
    if (rslt == BME680_OK) {
        rslt = configure_humidity_oversampling(desired_settings, reg_array, data_array, &count, dev);
    }
    if (rslt == BME680_OK) {
        rslt = configure_gas_settings(desired_settings, reg_array, data_array, &count, dev);
    }

    if (rslt == BME680_OK && count > 0) {
        rslt = bme680_set_regs(reg_array, data_array, count, dev);
    }

    dev->power_mode = intended_power_mode;
    return rslt;
}

int8_t bme680_get_sensor_settings(uint16_t desired_settings, struct bme680_dev *dev)
{
    int8_t rslt;
    /* starting address of the register array for burst read */
    uint8_t reg_addr = BME680_CONF_HEAT_CTRL_ADDR;
    uint8_t data_array[BME680_REG_BUFFER_LENGTH] = {0};

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        rslt = bme680_get_regs(reg_addr, data_array, BME680_REG_BUFFER_LENGTH, dev);
        if (rslt == BME680_OK) {
            if (desired_settings & BME680_GAS_MEAS_SEL) {
                rslt = get_gas_config(dev);
            }

            /* get the T,P,H ,Filter,ODR settings here */
            if (desired_settings & BME680_FILTER_SEL) {
                dev->tph_sett.filter =
                    bme680_get_bits(data_array[BME680_REG_FILTER_INDEX], BME680_FILTER_MSK, BME680_FILTER_POS);
            }

            if (desired_settings & (BME680_OST_SEL | BME680_OSP_SEL)) {
                dev->tph_sett.os_temp =
                    bme680_get_bits(data_array[BME680_REG_TEMP_INDEX], BME680_OST_MSK, BME680_OST_POS);
                dev->tph_sett.os_pres =
                    bme680_get_bits(data_array[BME680_REG_PRES_INDEX], BME680_OSP_MSK, BME680_OSP_POS);
            }

            if (desired_settings & BME680_OSH_SEL) {
                dev->tph_sett.os_hum = bme680_get_bits_pos_0(data_array[BME680_REG_HUM_INDEX], BME680_OSH_MSK);
            }

            /* get the gas related settings */
            if (desired_settings & BME680_HCNTRL_SEL) {
                dev->gas_sett.heatr_ctrl = bme680_get_bits_pos_0(data_array[BME680_REG_HCTRL_INDEX], BME680_HCTRL_MSK);
            }

            if (desired_settings & (BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)) {
                dev->gas_sett.nb_conv = bme680_get_bits_pos_0(data_array[BME680_REG_NBCONV_INDEX], BME680_NBCONV_MSK);
                dev->gas_sett.run_gas =
                    bme680_get_bits(data_array[BME680_REG_RUN_GAS_INDEX], BME680_RUN_GAS_MSK, BME680_RUN_GAS_POS);
            }
        }
    } else {
        rslt = BME680_E_NULL_PTR;
    }

    return rslt;
}

int8_t bme680_set_sensor_mode(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t tmp_pow_mode;
    uint8_t pow_mode = 0;
    uint8_t reg_addr = BME680_CONF_T_P_MODE_ADDR;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    /* Call recursively until in sleep */
    do {
        rslt = bme680_get_regs(BME680_CONF_T_P_MODE_ADDR, &tmp_pow_mode, 1, dev);
        if (rslt != BME680_OK) {
            return rslt;
        }

        /* Put to sleep before changing mode */
        pow_mode = (tmp_pow_mode & BME680_MODE_MSK);
        if (pow_mode == BME680_SLEEP_MODE) {
            break;
        }

        tmp_pow_mode = tmp_pow_mode & (~BME680_MODE_MSK); /* Set to sleep */
        rslt = bme680_set_regs(&reg_addr, &tmp_pow_mode, 1, dev);
        if (rslt != BME680_OK) {
            return rslt;
        }
        dev->delay_ms(BME680_POLL_PERIOD_MS);
    } while (pow_mode != BME680_SLEEP_MODE);

    /* Already in sleep */
    if (dev->power_mode == BME680_SLEEP_MODE) {
        return rslt;
    }

    tmp_pow_mode = (tmp_pow_mode & ~BME680_MODE_MSK) | (dev->power_mode & BME680_MODE_MSK);
    if (rslt == BME680_OK) {
        rslt = bme680_set_regs(&reg_addr, &tmp_pow_mode, 1, dev);
    }

    return rslt;
}

int8_t bme680_get_sensor_mode(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t mode;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        rslt = bme680_get_regs(BME680_CONF_T_P_MODE_ADDR, &mode, 1, dev);
        /* Masking the other register bit info */
        dev->power_mode = mode & BME680_MODE_MSK;
    }

    return rslt;
}

void bme680_set_profile_dur(uint16_t duration, struct bme680_dev *dev)
{
    uint32_t tph_dur;

    tph_dur = calculate_tph_duration(dev);
    dev->gas_sett.heatr_dur = duration - (uint16_t)tph_dur;
}

void bme680_get_profile_dur(uint16_t *duration, const struct bme680_dev *dev)
{
    uint32_t tph_dur;

    tph_dur = calculate_tph_duration(dev);
    *duration = (uint16_t)tph_dur;

    if (dev->gas_sett.run_gas) {
        *duration += dev->gas_sett.heatr_dur;
    }
}

int8_t bme680_get_sensor_data(struct bme680_field_data *data, struct bme680_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        /* Reading the sensor data in forced mode only */
        rslt = read_field_data(data, dev);
        if (rslt == BME680_OK) {
            if (data->status & BME680_NEW_DATA_MSK) {
                dev->new_fields = 1;
            } else {
                dev->new_fields = 0;
            }
        }
    }

    return rslt;
}

static void parse_temp_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev)
{
    dev->calib.par_t1 = (uint16_t)(bme680_concat_bytes(coeff_array[BME680_T1_MSB_REG], coeff_array[BME680_T1_LSB_REG]));
    dev->calib.par_t2 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_T2_MSB_REG], coeff_array[BME680_T2_LSB_REG]));
    dev->calib.par_t3 = (int8_t)(coeff_array[BME680_T3_REG]);
}

static void parse_pressure_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev)
{
    dev->calib.par_p1 = (uint16_t)(bme680_concat_bytes(coeff_array[BME680_P1_MSB_REG], coeff_array[BME680_P1_LSB_REG]));
    dev->calib.par_p2 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_P2_MSB_REG], coeff_array[BME680_P2_LSB_REG]));
    dev->calib.par_p3 = (int8_t)coeff_array[BME680_P3_REG];
    dev->calib.par_p4 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_P4_MSB_REG], coeff_array[BME680_P4_LSB_REG]));
    dev->calib.par_p5 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_P5_MSB_REG], coeff_array[BME680_P5_LSB_REG]));
    dev->calib.par_p6 = (int8_t)(coeff_array[BME680_P6_REG]);
    dev->calib.par_p7 = (int8_t)(coeff_array[BME680_P7_REG]);
    dev->calib.par_p8 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_P8_MSB_REG], coeff_array[BME680_P8_LSB_REG]));
    dev->calib.par_p9 = (int16_t)(bme680_concat_bytes(coeff_array[BME680_P9_MSB_REG], coeff_array[BME680_P9_LSB_REG]));
    dev->calib.par_p10 = (uint8_t)(coeff_array[BME680_P10_REG]);
}

static void parse_humidity_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev)
{
    dev->calib.par_h1 = (uint16_t)(((uint16_t)coeff_array[BME680_H1_MSB_REG] << BME680_HUM_REG_SHIFT_VAL) |
                                   (coeff_array[BME680_H1_LSB_REG] & BME680_BIT_H1_DATA_MSK));
    dev->calib.par_h2 = (uint16_t)(((uint16_t)coeff_array[BME680_H2_MSB_REG] << BME680_HUM_REG_SHIFT_VAL) |
                                   ((coeff_array[BME680_H2_LSB_REG]) >> BME680_HUM_REG_SHIFT_VAL));
    dev->calib.par_h3 = (int8_t)coeff_array[BME680_H3_REG];
    dev->calib.par_h4 = (int8_t)coeff_array[BME680_H4_REG];
    dev->calib.par_h5 = (int8_t)coeff_array[BME680_H5_REG];
    dev->calib.par_h6 = (uint8_t)coeff_array[BME680_H6_REG];
    dev->calib.par_h7 = (int8_t)coeff_array[BME680_H7_REG];
}

static void parse_gas_coefficients(const uint8_t *coeff_array, struct bme680_dev *dev)
{
    dev->calib.par_gh1 = (int8_t)coeff_array[BME680_GH1_REG];
    dev->calib.par_gh2 =
        (int16_t)(bme680_concat_bytes(coeff_array[BME680_GH2_MSB_REG], coeff_array[BME680_GH2_LSB_REG]));
    dev->calib.par_gh3 = (int8_t)coeff_array[BME680_GH3_REG];
}

static int8_t parse_other_coefficients(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t temp_var = 0;

    rslt = bme680_get_regs(BME680_ADDR_RES_HEAT_RANGE_ADDR, &temp_var, 1, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    dev->calib.res_heat_range = ((temp_var & BME680_RHRANGE_MSK) / BME680_RHRANGE_DIVISOR);
    rslt = bme680_get_regs(BME680_ADDR_RES_HEAT_VAL_ADDR, &temp_var, 1, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    dev->calib.res_heat_val = (int8_t)temp_var;
    rslt = bme680_get_regs(BME680_ADDR_RANGE_SW_ERR_ADDR, &temp_var, 1, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    dev->calib.range_sw_err = ((int8_t)temp_var & (int8_t)BME680_RSERROR_MSK) / BME680_RSERROR_DIVISOR;

    return rslt;
}

static int8_t interleave_reg_data(const uint8_t *reg_addr,
                                  const uint8_t *reg_data,
                                  uint8_t len,
                                  uint8_t *tmp_buff,
                                  struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint16_t index;

    for (index = 0; index < len; index++) {
        if (dev->intf == BME680_SPI_INTF) {
            /* Set the memory page */
            rslt = set_mem_page(reg_addr[index], dev);
            if (rslt != BME680_OK) {
                return rslt;
            }
            tmp_buff[(BME680_REG_PAIR_MULTIPLIER * index)] = reg_addr[index] & BME680_SPI_WR_MSK;
        } else {
            tmp_buff[(BME680_REG_PAIR_MULTIPLIER * index)] = reg_addr[index];
        }
        tmp_buff[(BME680_REG_PAIR_MULTIPLIER * index) + 1] = reg_data[index];
    }

    return rslt;
}

static int8_t get_calib_data(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t coeff_array[BME680_COEFF_SIZE] = {0};

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    rslt = bme680_get_regs(BME680_COEFF_ADDR1, coeff_array, BME680_COEFF_ADDR1_LEN, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    /* Append the second half in the same array */
    rslt = bme680_get_regs(BME680_COEFF_ADDR2, &coeff_array[BME680_COEFF_ADDR1_LEN], BME680_COEFF_ADDR2_LEN, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    /* Parse all coefficients from the array */
    parse_temp_coefficients(coeff_array, dev);
    parse_pressure_coefficients(coeff_array, dev);
    parse_humidity_coefficients(coeff_array, dev);
    parse_gas_coefficients(coeff_array, dev);

    /* Parse other coefficients from additional registers */
    rslt = parse_other_coefficients(dev);

    return rslt;
}

static int8_t set_gas_config(struct bme680_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        uint8_t reg_addr[BME680_GAS_CONFIG_REG_COUNT] = {0};
        uint8_t reg_data[BME680_GAS_CONFIG_REG_COUNT] = {0};

        if (dev->power_mode == BME680_FORCED_MODE) {
            reg_addr[0] = BME680_RES_HEAT0_ADDR;
            reg_data[0] = calc_heater_res(dev->gas_sett.heatr_temp, dev);
            reg_addr[1] = BME680_GAS_WAIT0_ADDR;
            reg_data[1] = calc_heater_dur(dev->gas_sett.heatr_dur);
            dev->gas_sett.nb_conv = 0;
        } else {
            rslt = BME680_W_DEFINE_PWR_MODE;
        }
        if (rslt == BME680_OK) {
            rslt = bme680_set_regs(reg_addr, reg_data, BME680_GAS_CONFIG_REG_COUNT, dev);
        }
    }

    return rslt;
}

static int8_t get_gas_config(struct bme680_dev *dev)
{
    int8_t rslt;
    /* starting address of the register array for burst read */
    uint8_t reg_addr1 = BME680_ADDR_SENS_CONF_START;
    uint8_t reg_addr2 = BME680_ADDR_GAS_CONF_START;
    uint8_t data_array[BME680_GAS_HEATER_PROF_LEN_MAX] = {0};
    uint8_t index;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if (dev->intf == BME680_SPI_INTF) {
        /* Memory page switch the SPI address */
        rslt = set_mem_page(reg_addr1, dev);
        if (rslt != BME680_OK) {
            return rslt;
        }
    }

    rslt = bme680_get_regs(reg_addr1, data_array, BME680_GAS_HEATER_PROF_LEN_MAX, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    for (index = 0; index < BME680_GAS_HEATER_PROF_LEN_MAX; index++) {
        dev->gas_sett.heatr_temp = data_array[index];
    }

    rslt = bme680_get_regs(reg_addr2, data_array, BME680_GAS_HEATER_PROF_LEN_MAX, dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    for (index = 0; index < BME680_GAS_HEATER_PROF_LEN_MAX; index++) {
        dev->gas_sett.heatr_dur = data_array[index];
    }

    return rslt;
}

static int16_t calc_temperature(uint32_t temp_adc, struct bme680_dev *dev)
{
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int16_t calc_temp;

    var1 = ((int32_t)temp_adc >> BME680_TEMP_ADC_SHIFT_RIGHT) -
           ((int32_t)dev->calib.par_t1 << BME680_TEMP_PAR_T1_SHIFT_LEFT);
    var2 = (var1 * (int32_t)dev->calib.par_t2) >> BME680_TEMP_VAR2_SHIFT_RIGHT;
    var3 = ((var1 >> 1) * (var1 >> 1)) >> BME680_TEMP_VAR3_SHIFT_RIGHT;
    var3 = ((var3) * ((int32_t)dev->calib.par_t3 << BME680_TEMP_PAR_T3_SHIFT_LEFT)) >> BME680_TEMP_VAR3_SHIFT_RIGHT_2;
    dev->calib.t_fine = (int32_t)(var2 + var3);
    calc_temp = (int16_t)(((dev->calib.t_fine * BME680_TEMP_CALC_MULTIPLIER) + BME680_TEMP_CALC_OFFSET) >>
                          BME680_TEMP_SHIFT_RIGHT);

    return calc_temp;
}

static uint32_t calc_pressure(uint32_t pres_adc, const struct bme680_dev *dev)
{
    int32_t var1 = 0;
    int32_t var2 = 0;
    int32_t var3 = 0;
    int32_t var4 = 0;
    int32_t pressure_comp = 0;

    var1 = (((int32_t)dev->calib.t_fine) >> BME680_PRES_VAR1_SHIFT_RIGHT) - BME680_PRES_VAR1_SUBTRACT;
    var2 = ((((var1 >> BME680_PRES_VAR2_SHIFT_RIGHT) * (var1 >> BME680_PRES_VAR2_SHIFT_RIGHT)) >>
             BME680_PRES_VAR2_SHIFT_RIGHT_2) *
            (int32_t)dev->calib.par_p6) >>
           BME680_PRES_VAR2_SHIFT_RIGHT_3;
    var2 = var2 + ((var1 * (int32_t)dev->calib.par_p5) << BME680_PRES_VAR1_SHIFT_RIGHT);
    var2 = (var2 >> BME680_PRES_VAR2_SHIFT_RIGHT) + ((int32_t)dev->calib.par_p4 << BME680_PRES_PAR_P4_SHIFT_LEFT);
    var1 = (((((var1 >> BME680_PRES_VAR2_SHIFT_RIGHT) * (var1 >> BME680_PRES_VAR2_SHIFT_RIGHT)) >>
              BME680_PRES_VAR1_SHIFT_RIGHT_2) *
             ((int32_t)dev->calib.par_p3 << BME680_PRES_PAR_P3_SHIFT_LEFT)) >>
            BME680_PRES_VAR1_SHIFT_RIGHT_3) +
           (((int32_t)dev->calib.par_p2 * var1) >> BME680_PRES_VAR1_SHIFT_RIGHT);
    var1 = var1 >> BME680_PRES_VAR1_SHIFT_RIGHT_4;
    var1 = ((BME680_PRES_VAR1_ADD + var1) * (int32_t)dev->calib.par_p1) >> BME680_PRES_VAR1_SHIFT_RIGHT_5;
    pressure_comp = BME680_PRES_COMP_SUBTRACT - pres_adc;
    pressure_comp =
        (int32_t)((pressure_comp - (var2 >> BME680_PRES_VAR2_SHIFT_RIGHT_4)) * ((uint32_t)BME680_PRES_COMP_MULTIPLIER));
    var4 = (1 << BME680_PRES_VAR4_SHIFT_LEFT);
    if (pressure_comp >= var4) {
        pressure_comp = ((pressure_comp / (uint32_t)var1) << BME680_PRES_COMP_SHIFT_LEFT);
    } else {
        pressure_comp = ((pressure_comp << BME680_PRES_COMP_SHIFT_LEFT) / (uint32_t)var1);
    }
    var1 = ((int32_t)dev->calib.par_p9 * (int32_t)(((pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT) *
                                                    (pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT)) >>
                                                   BME680_PRES_COMP_SHIFT_RIGHT_2)) >>
           BME680_PRES_COMP_SHIFT_RIGHT_3;
    var2 = ((int32_t)(pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT_4) * (int32_t)dev->calib.par_p8) >>
           BME680_PRES_PAR_P8_SHIFT_RIGHT;
    var3 = ((int32_t)(pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT_5) *
            (int32_t)(pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT_5) *
            (int32_t)(pressure_comp >> BME680_PRES_COMP_SHIFT_RIGHT_5) * (int32_t)dev->calib.par_p10) >>
           BME680_PRES_COMP_SHIFT_RIGHT_6;

    pressure_comp = (int32_t)(pressure_comp) +
                    ((var1 + var2 + var3 + ((int32_t)dev->calib.par_p7 << BME680_PRES_PAR_P7_SHIFT_LEFT)) >>
                     BME680_PRES_COMP_SHIFT_RIGHT_7);

    return (uint32_t)pressure_comp;
}

static uint32_t calc_humidity(uint16_t hum_adc, const struct bme680_dev *dev)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t temp_scaled;
    int32_t calc_hum;

    temp_scaled = (((int32_t)dev->calib.t_fine * BME680_TEMP_CALC_MULTIPLIER) + BME680_TEMP_CALC_OFFSET) >>
                  BME680_TEMP_SHIFT_RIGHT;
    var1 = (int32_t)(hum_adc - ((int32_t)((int32_t)dev->calib.par_h1 * BME680_HUM_PAR_H1_MULTIPLIER))) -
           (((temp_scaled * (int32_t)dev->calib.par_h3) / ((int32_t)BME680_HUM_PERCENT_DIVISOR)) >>
            BME680_HUM_VAR1_SHIFT_RIGHT);
    var2 = ((int32_t)dev->calib.par_h2 *
            (((temp_scaled * (int32_t)dev->calib.par_h4) / ((int32_t)BME680_HUM_PERCENT_DIVISOR)) +
             (((temp_scaled * ((temp_scaled * (int32_t)dev->calib.par_h5) / ((int32_t)BME680_HUM_PERCENT_DIVISOR))) >>
               BME680_HUM_VAR2_SHIFT_RIGHT) /
              ((int32_t)BME680_HUM_PERCENT_DIVISOR)) +
             (int32_t)(1 << BME680_HUM_VAR2_SHIFT_LEFT))) >>
           BME680_HUM_VAR2_SHIFT_RIGHT_2;
    var3 = var1 * var2;
    var4 = (int32_t)dev->calib.par_h6 << BME680_HUM_VAR4_SHIFT_LEFT;
    var4 = ((var4) + ((temp_scaled * (int32_t)dev->calib.par_h7) / ((int32_t)BME680_HUM_PERCENT_DIVISOR))) >>
           BME680_HUM_VAR4_SHIFT_RIGHT;
    var5 = ((var3 >> BME680_HUM_VAR5_SHIFT_RIGHT) * (var3 >> BME680_HUM_VAR5_SHIFT_RIGHT)) >>
           BME680_HUM_VAR5_SHIFT_RIGHT_2;
    var6 = (var4 * var5) >> BME680_HUM_VAR6_SHIFT_RIGHT;
    calc_hum = (((var3 + var6) >> BME680_HUM_CALC_SHIFT_RIGHT) * ((int32_t)BME680_HUM_CALC_MULTIPLIER)) >>
               BME680_HUM_CALC_SHIFT_RIGHT_2;

    if (calc_hum > BME680_HUM_MAX_VALUE) { /* Cap at 100%rH */
        calc_hum = BME680_HUM_MAX_VALUE;
    } else if (calc_hum < BME680_HUM_MIN_VALUE) {
        calc_hum = BME680_HUM_MIN_VALUE;
    }

    return (uint32_t)calc_hum;
}

uint32_t calc_gas_resistance(uint16_t gas_res_adc, uint8_t gas_range, struct bme680_dev *dev)
{
    int64_t var1;
    uint64_t var2;
    int64_t var3;
    uint32_t calc_gas_res;

    var1 = (int64_t)((BME680_GAS_VAR1_BASE + (BME680_GAS_VAR1_MULTIPLIER * (int64_t)dev->calib.range_sw_err)) *
                     ((int64_t)g_lookup_table1[gas_range])) >>
           BME680_GAS_VAR1_SHIFT_RIGHT;
    var2 =
        (((int64_t)((int64_t)gas_res_adc << BME680_GAS_VAR2_SHIFT_LEFT) - (int64_t)(BME680_GAS_VAR2_SUBTRACT)) + var1);
    var3 = (((int64_t)g_lookup_table2[gas_range] * (int64_t)var1) >> BME680_GAS_VAR3_SHIFT_RIGHT);
    calc_gas_res = (uint32_t)((var3 + ((int64_t)var2 >> BME680_GAS_VAR2_SHIFT_RIGHT)) / (int64_t)var2);

    return calc_gas_res;
}

static uint8_t calc_heater_res(uint16_t temp, const struct bme680_dev *dev)
{
    uint8_t heatr_res;
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t heatr_res_x100;

    if (temp < BME680_HEATR_TEMP_MIN) { /* Cap temperature */
        temp = BME680_HEATR_TEMP_MIN;
    } else if (temp > BME680_HEATR_TEMP_MAX) {
        temp = BME680_HEATR_TEMP_MAX;
    }

    var1 = (((int32_t)dev->amb_temp * dev->calib.par_gh3) / BME680_HEATR_AMBIENT_DIVISOR) *
           BME680_HEATR_AMBIENT_MULTIPLIER;
    var2 = (dev->calib.par_gh1 + BME680_HEATR_PAR_GH1_ADD) *
           (((((dev->calib.par_gh2 + BME680_HEATR_PAR_GH2_ADD) * temp * BME680_HEATR_TEMP_MULTIPLIER) /
              BME680_HEATR_TEMP_DIVISOR) +
             BME680_HEATR_TEMP_ADD) /
            BME680_HEATR_TEMP_DIVISOR_2);
    var3 = var1 + (var2 / BME680_REG_PAIR_MULTIPLIER);
    var4 = (var3 / (dev->calib.res_heat_range + BME680_HEATR_RES_RANGE_ADD));
    var5 = (BME680_HEATR_VAR5_MULTIPLIER * dev->calib.res_heat_val) + BME680_HEATR_VAR5_ADD;
    heatr_res_x100 = (int32_t)(((var4 / var5) - BME680_HEATR_VAR5_SUBTRACT) * BME680_HEATR_VAR5_MULTIPLIER_2);
    heatr_res = (uint8_t)((heatr_res_x100 + BME680_HEATR_ROUNDING_ADD) / BME680_HEATR_ROUNDING_DIVISOR);

    return heatr_res;
}

static uint8_t calc_heater_dur(uint16_t dur)
{
    uint8_t factor = 0;
    uint8_t durval;

    if (dur >= BME680_HEATR_DUR_MAX_THRESHOLD) {
        durval = BME680_HEATR_DUR_MAX_VALUE; /* Max duration */
    } else {
        while (dur > BME680_HEATR_DUR_THRESHOLD) {
            dur = dur / BME680_HEATR_DUR_DIVISOR;
            factor += 1;
        }
        durval = (uint8_t)(dur + (factor * BME680_HEATR_DUR_FACTOR_MULTIPLIER));
    }

    return durval;
}

static int8_t read_field_data(struct bme680_field_data *data, struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t buff[BME680_FIELD_LENGTH] = {0};
    uint8_t gas_range;
    uint32_t adc_temp;
    uint32_t adc_pres;
    uint16_t adc_hum;
    uint16_t adc_gas_res;
    uint8_t tries = BME680_FIELD_READ_TRIES;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    do {
        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(((uint8_t)(BME680_FIELD0_ADDR)), buff, (uint16_t)BME680_FIELD_LENGTH, dev);

            data->status = buff[BME680_FIELD_STATUS_INDEX] & BME680_NEW_DATA_MSK;
            data->gas_index = buff[BME680_FIELD_STATUS_INDEX] & BME680_GAS_INDEX_MSK;
            data->meas_index = buff[BME680_FIELD_MEAS_INDEX];

            /* read the raw data from the sensor */
            adc_pres = (uint32_t)(((uint32_t)buff[BME680_FIELD_PRES_MSB_INDEX] * BME680_ADC_PRES_MULTIPLIER_MSB) |
                                  ((uint32_t)buff[BME680_FIELD_PRES_LSB_INDEX] * BME680_ADC_PRES_MULTIPLIER_MID) |
                                  ((uint32_t)buff[BME680_FIELD_PRES_XLSB_INDEX] / BME680_ADC_PRES_DIVISOR));
            adc_temp = (uint32_t)(((uint32_t)buff[BME680_FIELD_TEMP_MSB_INDEX] * BME680_ADC_TEMP_MULTIPLIER_MSB) |
                                  ((uint32_t)buff[BME680_FIELD_TEMP_LSB_INDEX] * BME680_ADC_TEMP_MULTIPLIER_MID) |
                                  ((uint32_t)buff[BME680_FIELD_TEMP_XLSB_INDEX] / BME680_ADC_TEMP_DIVISOR));
            adc_hum = (uint16_t)(((uint32_t)buff[BME680_FIELD_HUM_MSB_INDEX] * BME680_ADC_HUM_MULTIPLIER) |
                                 (uint32_t)buff[BME680_FIELD_HUM_LSB_INDEX]);
            adc_gas_res = (uint16_t)(((uint32_t)buff[BME680_FIELD_GAS_RES_MSB_INDEX] * BME680_ADC_GAS_MULTIPLIER) |
                                     (((uint32_t)buff[BME680_FIELD_GAS_RES_LSB_INDEX]) / BME680_ADC_GAS_DIVISOR));
            gas_range = buff[BME680_FIELD_GAS_RES_LSB_INDEX] & BME680_GAS_RANGE_MSK;

            data->status |= buff[BME680_FIELD_GAS_RES_LSB_INDEX] & BME680_GASM_VALID_MSK;
            data->status |= buff[BME680_FIELD_GAS_RES_LSB_INDEX] & BME680_HEAT_STAB_MSK;

            if (data->status & BME680_NEW_DATA_MSK) {
                data->temperature = calc_temperature(adc_temp, dev);
                data->pressure = calc_pressure(adc_pres, dev);
                data->humidity = calc_humidity(adc_hum, dev);
                data->gas_resistance = calc_gas_resistance(adc_gas_res, gas_range, dev);
                break;
            }
            /* Delay to poll the data */
            dev->delay_ms(BME680_POLL_PERIOD_MS);
        }
        tries--;
    } while (tries);

    if (!tries) {
        rslt = BME680_W_NO_NEW_DATA;
    }

    return rslt;
}

static int8_t set_mem_page(uint8_t reg_addr, struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t reg;
    uint8_t mem_page;

    /* Check for null pointers in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt != BME680_OK) {
        return rslt;
    }

    if (reg_addr > 0x7f) {
        mem_page = BME680_MEM_PAGE1;
    } else {
        mem_page = BME680_MEM_PAGE0;
    }

    if (mem_page == dev->mem_page) {
        return rslt;
    }

    dev->mem_page = mem_page;

    dev->com_rslt = dev->read(dev->dev_id, BME680_MEM_PAGE_ADDR | BME680_SPI_RD_MSK, &reg, 1);
    if (dev->com_rslt != 0) {
        return BME680_E_COM_FAIL;
    }

    reg = reg & (~BME680_MEM_PAGE_MSK);
    reg = reg | (dev->mem_page & BME680_MEM_PAGE_MSK);

    dev->com_rslt = dev->write(dev->dev_id, BME680_MEM_PAGE_ADDR & BME680_SPI_WR_MSK, &reg, 1);
    if (dev->com_rslt != 0) {
        return BME680_E_COM_FAIL;
    }

    return rslt;
}

static int8_t get_mem_page(struct bme680_dev *dev)
{
    int8_t rslt;
    uint8_t reg;

    /* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
    if (rslt == BME680_OK) {
        dev->com_rslt = dev->read(dev->dev_id, BME680_MEM_PAGE_ADDR | BME680_SPI_RD_MSK, &reg, 1);
        if (dev->com_rslt != 0) {
            rslt = BME680_E_COM_FAIL;
        } else {
            dev->mem_page = reg & BME680_MEM_PAGE_MSK;
        }
    }

    return rslt;
}

static int8_t boundary_check(uint8_t *value, uint8_t min, uint8_t max, struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;

    if (value != NULL) {
        /* Check if value is below minimum value */
        if (*value < min) {
            /* Auto correct the invalid value to minimum value */
            *value = min;
            dev->info_msg |= BME680_I_MIN_CORRECTION;
        }
        /* Check if value is above maximum value */
        if (*value > max) {
            /* Auto correct the invalid value to maximum value */
            *value = max;
            dev->info_msg |= BME680_I_MAX_CORRECTION;
        }
    } else {
        rslt = BME680_E_NULL_PTR;
    }

    return rslt;
}

static int8_t null_ptr_check(const struct bme680_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_ms == NULL)) {
        /* Device structure pointer is not valid */
        rslt = BME680_E_NULL_PTR;
    } else {
        /* Device structure is fine */
        rslt = BME680_OK;
    }

    return rslt;
}

static int8_t configure_filter_settings(uint16_t desired_settings,
                                        uint8_t *reg_array,
                                        uint8_t *data_array,
                                        uint8_t *count,
                                        struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint8_t reg_addr;
    uint8_t data = 0;

    if (desired_settings & BME680_FILTER_SEL) {
        rslt = boundary_check(&dev->tph_sett.filter, BME680_FILTER_SIZE_0, BME680_FILTER_SIZE_127, dev);
        reg_addr = BME680_CONF_ODR_FILT_ADDR;

        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(reg_addr, &data, 1, dev);
        }

        if (rslt == BME680_OK) {
            if (desired_settings & BME680_FILTER_SEL) {
                data = bme680_set_bits(data, BME680_FILTER_MSK, BME680_FILTER_POS, dev->tph_sett.filter);
            }

            reg_array[*count] = reg_addr;
            data_array[*count] = data;
            (*count)++;
        }
    }

    return rslt;
}

static int8_t configure_heater_control(uint16_t desired_settings,
                                       uint8_t *reg_array,
                                       uint8_t *data_array,
                                       uint8_t *count,
                                       struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint8_t reg_addr;
    uint8_t data = 0;

    if (desired_settings & BME680_HCNTRL_SEL) {
        rslt = boundary_check(&dev->gas_sett.heatr_ctrl, BME680_ENABLE_HEATER, BME680_DISABLE_HEATER, dev);
        reg_addr = BME680_CONF_HEAT_CTRL_ADDR;

        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(reg_addr, &data, 1, dev);
        }

        if (rslt == BME680_OK) {
            data = bme680_set_bits_pos_0(data, BME680_HCTRL_MSK, dev->gas_sett.heatr_ctrl);

            reg_array[*count] = reg_addr;
            data_array[*count] = data;
            (*count)++;
        }
    }

    return rslt;
}

static int8_t configure_tph_oversampling(uint16_t desired_settings,
                                         uint8_t *reg_array,
                                         uint8_t *data_array,
                                         uint8_t *count,
                                         struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint8_t reg_addr;
    uint8_t data = 0;

    if (desired_settings & (BME680_OST_SEL | BME680_OSP_SEL)) {
        rslt = boundary_check(&dev->tph_sett.os_temp, BME680_OS_NONE, BME680_OS_16X, dev);
        reg_addr = BME680_CONF_T_P_MODE_ADDR;

        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(reg_addr, &data, 1, dev);
        }

        if (rslt == BME680_OK) {
            if (desired_settings & BME680_OST_SEL) {
                data = bme680_set_bits(data, BME680_OST_MSK, BME680_OST_POS, dev->tph_sett.os_temp);
            }

            if (desired_settings & BME680_OSP_SEL) {
                data = bme680_set_bits(data, BME680_OSP_MSK, BME680_OSP_POS, dev->tph_sett.os_pres);
            }

            reg_array[*count] = reg_addr;
            data_array[*count] = data;
            (*count)++;
        }
    }

    return rslt;
}

static int8_t configure_humidity_oversampling(uint16_t desired_settings,
                                              uint8_t *reg_array,
                                              uint8_t *data_array,
                                              uint8_t *count,
                                              struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint8_t reg_addr;
    uint8_t data = 0;

    if (desired_settings & BME680_OSH_SEL) {
        rslt = boundary_check(&dev->tph_sett.os_hum, BME680_OS_NONE, BME680_OS_16X, dev);
        reg_addr = BME680_CONF_OS_H_ADDR;

        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(reg_addr, &data, 1, dev);
        }

        if (rslt == BME680_OK) {
            data = bme680_set_bits_pos_0(data, BME680_OSH_MSK, dev->tph_sett.os_hum);

            reg_array[*count] = reg_addr;
            data_array[*count] = data;
            (*count)++;
        }
    }

    return rslt;
}

static int8_t configure_gas_settings(uint16_t desired_settings,
                                     uint8_t *reg_array,
                                     uint8_t *data_array,
                                     uint8_t *count,
                                     struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    uint8_t reg_addr;
    uint8_t data = 0;

    if (desired_settings & (BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)) {
        rslt = boundary_check(&dev->gas_sett.run_gas, BME680_RUN_GAS_DISABLE, BME680_RUN_GAS_ENABLE, dev);
        if (rslt == BME680_OK) {
            rslt = boundary_check(&dev->gas_sett.nb_conv, BME680_NBCONV_MIN, BME680_NBCONV_MAX, dev);
        }

        reg_addr = BME680_CONF_ODR_RUN_GAS_NBC_ADDR;

        if (rslt == BME680_OK) {
            rslt = bme680_get_regs(reg_addr, &data, 1, dev);
        }

        if (rslt == BME680_OK) {
            if (desired_settings & BME680_RUN_GAS_SEL) {
                data = bme680_set_bits(data, BME680_RUN_GAS_MSK, BME680_RUN_GAS_POS, dev->gas_sett.run_gas);
            }

            if (desired_settings & BME680_NBCONV_SEL) {
                data = bme680_set_bits_pos_0(data, BME680_NBCONV_MSK, dev->gas_sett.nb_conv);
            }

            reg_array[*count] = reg_addr;
            data_array[*count] = data;
            (*count)++;
        }
    }

    return rslt;
}

static uint32_t calculate_tph_duration(const struct bme680_dev *dev)
{
    uint32_t tph_dur;

    tph_dur = ((uint32_t)(dev->tph_sett.os_temp + dev->tph_sett.os_pres + dev->tph_sett.os_hum) * BME680_TPH_DUR_COEFF);
    tph_dur += (BME680_TPH_SWITCH_DUR_BASE * BME680_TPH_SWITCH_MULTIPLIER);
    tph_dur += (BME680_TPH_SWITCH_DUR_BASE * BME680_GAS_MEAS_DUR_MULTIPLIER);
    tph_dur += BME680_DUR_ROUNDING_FACTOR;
    tph_dur /= BME680_MS_PER_SEC;
    tph_dur += BME680_WAKE_UP_DUR_MS;

    return tph_dur;
}
