/**
 * @file dfrobot_bme680.h
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#ifndef DFROBOT_BME680_H
#define DFROBOT_BME680_H

#include "bme680.h"

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "systick.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "app_init.h"

#define BME680_SEALEVEL 1015

/** Default sensor configuration constants */
#define BME680_DEFAULT_OS_HUM UINT8_C(5)
#define BME680_DEFAULT_OS_PRES UINT8_C(5)
#define BME680_DEFAULT_OS_TEMP UINT8_C(5)
#define BME680_DEFAULT_FILTER UINT8_C(4)
#define BME680_DEFAULT_HEATR_TEMP UINT16_C(320)
#define BME680_DEFAULT_HEATR_DUR UINT16_C(150)

/** Convert command constants */
#define BME680_CONVERT_CMD_OS_TEMP_SHIFT 5
#define BME680_CONVERT_CMD_OS_PRES_SHIFT 2
#define BME680_CONVERT_CMD_OS_TEMP_VAL 0x05
#define BME680_CONVERT_CMD_OS_PRES_VAL 0x05
#define BME680_CONVERT_CMD_MODE_VAL 0x01

/** Register address constants */
#define BME680_REG_CTRL_TEMP_PRES UINT8_C(0x74)
#define BME680_REG_CTRL_HUM UINT8_C(0x72)
#define BME680_REG_CTRL_FILTER UINT8_C(0x75)
#define BME680_REG_CTRL_SPI_PAGE UINT8_C(0x73)

/** Register value constants */
#define BME680_REG_DATA_SIZE UINT8_C(1)
#define BME680_REG_SPI_PAGE_RESET UINT8_C(0x00)

/** Parameter validation constants */
#define BME680_PARAM_MAX_VALUE UINT8_C(0x05)
#define BME680_PARAM_MASK UINT8_C(0x07)

/** Bit shift constants */
#define BME680_PARAM_TEMP_SHIFT 5
#define BME680_PARAM_PRES_SHIFT 2
#define BME680_PARAM_HUM_SHIFT 0
#define BME680_PARAM_FILTER_SHIFT 2

/** Bit mask constants */
#define BME680_BIT_MASK_LSB UINT8_C(0x01)

/** Return value constants */
#define BME680_INIT_SUCCESS 0
#define BME680_INIT_FAILURE (-1)

/** Altitude calculation constants */
#define BME680_PRESSURE_TO_HPA_DIVISOR 100.0f
#define BME680_ALTITUDE_EXPONENT 0.190284f
#define BME680_ALTITUDE_COEFFICIENT_1 287.15f
#define BME680_ALTITUDE_COEFFICIENT_2 0.0065f
#define BME680_SEA_LEVEL_ALTITUDE_BASE 44330.0f
#define BME680_SEA_LEVEL_EXPONENT 5.255f

/**\name C standard macros */
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

typedef enum { BME680_INTERFACE_SPI, BME680_INTERFACE_I2C } e_bme680_interface;

typedef void (*pf_start_convert_t)(void);
typedef void (*pf_update_t)(void);

void bme680_delay_ms(uint32_t period);

typedef enum {
    BME680_PARAM_TEMPSAMP,
    BME680_PARAM_HUMISAMP,
    BME680_PARAM_PREESAMP,
    BME680_PARAM_IIRSIZE
} e_bme680_param_t;

void dfrobot_bme680(bme680_com_fptr_t read_reg,
                    bme680_com_fptr_t write_reg,
                    bme680_delay_fptr_t delay_ms,
                    e_bme680_interface interface);

extern uint8_t g_bme680_i2c_addr;
/**
 * @fn begin
 * @brief begin BME680 device
 * @return result
 * @retval  non-zero : failed
 * @retval  0        : succussful
 */
int16_t bme680_bme680_begin(void);
/**
 * @fn update
 * @brief update all data to MCU ram
 */
void bme680_bme680_update(void);

/**
 * @fn start_convert
 * @brief start convert to get a accurate values
 */
void start_convert(void);
/**
 * @fn read_temperature
 * @brief read the temperature value (unit C)
 *
 * @return temperature valu, this value has two decimal points
 */
float read_temperature(void);
/**
 * @fn read_pressure
 * @brief read the pressure value (unit pa)
 *
 * @return pressure value, this value has two decimal points
 */
float read_pressure(void);
/**
 * @fn read_humidity
 * @brief read the humidity value (unit %rh)
 * @return humidity value, this value has two decimal points
 */
float read_humidity(void);
/**
 * @fn read_altitude
 * @brief read the altitude (unit meter)
 * @return altitude value, this value has two decimal points
 */
float read_altitude(void);
/**
 * @fn read_calibrated_altitude
 * @brief read the Calibrated altitude (unit meter)
 *
 * @param sea_level  normalised atmospheric pressure
 *
 * @return calibrated altitude value , this value has two decimal points
 */
float read_calibrated_altitude(float sea_level);
/**
 * @fn read_gas_resistance
 * @brief read the gas resistance(unit ohm)
 * @return temperature value, this value has two decimal points
 */
float read_gas_resistance(void);
/**
 * @fn read_sea_level
 * @brief read normalised atmospheric pressure (unit pa)
 * @param altitude   accurate altitude for normalising
 * @return normalised atmospheric pressure
 */
float read_sea_level(float altitude);
/**
 * @fn set_param
 * @brief set bme680 parament
 *
 * @param e_param        :which param you want to change
 *        dat           :object data, can't more than 5
 */
void set_param(e_bme680_param_t e_param, uint8_t dat);
/**
 * @fn set_gas_heater
 * @brief set bme680 gas heater
 * @param temp        :your object temp
 * @param t           :time spend in milliseconds
 */
void set_gas_heater(uint16_t temp, uint16_t t);

void write_param_helper(uint8_t reg, uint8_t dat, uint8_t addr);

#endif
