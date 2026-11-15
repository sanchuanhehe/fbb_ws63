/**
 * @file bme680_defs.h
 * @brief Sensor driver for BME680 sensor.
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#ifndef BME680_DEFS_H_
#define BME680_DEFS_H_

/********************************************************/
/* header includes */
#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

/******************************************************************************/
/*! @name        Common macros                          */
/******************************************************************************/

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x) (x)
#define UINT8_C(x) (x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x) (x)
#define UINT16_C(x) (x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x) (x)
#define UINT32_C(x) (x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x) (x)
#define UINT64_C(x) (x)
#endif

/**@}*/

/**\name C standard macros */
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

/** BME680 General config */
#define BME680_POLL_PERIOD_MS UINT8_C(10)

/** BME680 I2C addresses */
#define BME680_I2C_ADDR_PRIMARY UINT8_C(0x76)
#define BME680_I2C_ADDR_SECONDARY UINT8_C(0x77)

/** BME680 unique chip identifier */
#define BME680_CHIP_ID UINT8_C(0x61)

/** BME680 coefficients related defines */
#define BME680_COEFF_SIZE UINT8_C(0x41)
#define BME680_COEFF_ADDR1_LEN UINT8_C(25)
#define BME680_COEFF_ADDR2_LEN UINT8_C(16)

/** Common constants */
#define BME680_BITS_PER_BYTE UINT8_C(8)

/** BME680 field_x related defines */
#define BME680_FIELD_LENGTH UINT8_C(15)
#define BME680_FIELD_ADDR_OFFSET UINT8_C(17)

/** Soft reset command */
#define BME680_SOFT_RESET_CMD UINT8_C(0xb6)

/** Error code definitions */
#define BME680_OK INT8_C(0)
/* Errors */
#define BME680_E_NULL_PTR INT8_C(-1)
#define BME680_E_COM_FAIL INT8_C(-2)
#define BME680_E_DEV_NOT_FOUND INT8_C(-3)
#define BME680_E_INVALID_LENGTH INT8_C(-4)

/* Warnings */
#define BME680_W_DEFINE_PWR_MODE INT8_C(1)
#define BME680_W_NO_NEW_DATA INT8_C(2)

/* Info's */
#define BME680_I_MIN_CORRECTION UINT8_C(1)
#define BME680_I_MAX_CORRECTION UINT8_C(2)

/** Register map */
/** Other coefficient's address */
#define BME680_ADDR_RES_HEAT_VAL_ADDR UINT8_C(0x00)
#define BME680_ADDR_RES_HEAT_RANGE_ADDR UINT8_C(0x02)
#define BME680_ADDR_RANGE_SW_ERR_ADDR UINT8_C(0x04)
#define BME680_ADDR_SENS_CONF_START UINT8_C(0x5A)
#define BME680_ADDR_GAS_CONF_START UINT8_C(0x64)

/** Field settings */
#define BME680_FIELD0_ADDR UINT8_C(0x1d)

/** Heater settings */
#define BME680_RES_HEAT0_ADDR UINT8_C(0x5a)
#define BME680_GAS_WAIT0_ADDR UINT8_C(0x64)

/** Sensor configuration registers */
#define BME680_CONF_HEAT_CTRL_ADDR UINT8_C(0x70)
#define BME680_CONF_ODR_RUN_GAS_NBC_ADDR UINT8_C(0x71)
#define BME680_CONF_OS_H_ADDR UINT8_C(0x72)
#define BME680_MEM_PAGE_ADDR UINT8_C(0xf3)
#define BME680_CONF_T_P_MODE_ADDR UINT8_C(0x74)
#define BME680_CONF_ODR_FILT_ADDR UINT8_C(0x75)

/** Coefficient's address */
#define BME680_COEFF_ADDR1 UINT8_C(0x89)
#define BME680_COEFF_ADDR2 UINT8_C(0xe1)

/** Chip identifier */
#define BME680_CHIP_ID_ADDR UINT8_C(0xd0)

/** Soft reset register */
#define BME680_SOFT_RESET_ADDR UINT8_C(0xe0)

/** Heater control settings */
#define BME680_ENABLE_HEATER UINT8_C(0x00)
#define BME680_DISABLE_HEATER UINT8_C(0x08)

/** Gas measurement settings */
#define BME680_DISABLE_GAS_MEAS UINT8_C(0x00)
#define BME680_ENABLE_GAS_MEAS UINT8_C(0x01)

/** Over-sampling settings */
#define BME680_OS_NONE UINT8_C(0)
#define BME680_OS_1X UINT8_C(1)
#define BME680_OS_2X UINT8_C(2)
#define BME680_OS_4X UINT8_C(3)
#define BME680_OS_8X UINT8_C(4)
#define BME680_OS_16X UINT8_C(5)

/** IIR filter settings */
#define BME680_FILTER_SIZE_0 UINT8_C(0)
#define BME680_FILTER_SIZE_1 UINT8_C(1)
#define BME680_FILTER_SIZE_3 UINT8_C(2)
#define BME680_FILTER_SIZE_7 UINT8_C(3)
#define BME680_FILTER_SIZE_15 UINT8_C(4)
#define BME680_FILTER_SIZE_31 UINT8_C(5)
#define BME680_FILTER_SIZE_63 UINT8_C(6)
#define BME680_FILTER_SIZE_127 UINT8_C(7)

/** Power mode settings */
#define BME680_SLEEP_MODE UINT8_C(0)
#define BME680_FORCED_MODE UINT8_C(1)

/** Delay related macro declaration */
#define BME680_RESET_PERIOD UINT32_C(10)

/** SPI memory page settings */
#define BME680_MEM_PAGE0 UINT8_C(0x10)
#define BME680_MEM_PAGE1 UINT8_C(0x00)

/** Ambient humidity shift value for compensation */
#define BME680_HUM_REG_SHIFT_VAL UINT8_C(4)

/** Run gas enable and disable settings */
#define BME680_RUN_GAS_DISABLE UINT8_C(0)
#define BME680_RUN_GAS_ENABLE UINT8_C(1)

/** Buffer length macro declaration */
#define BME680_TMP_BUFFER_LENGTH UINT8_C(40)
#define BME680_REG_BUFFER_LENGTH UINT8_C(6)
#define BME680_FIELD_DATA_LENGTH UINT8_C(3)
#define BME680_GAS_REG_BUF_LENGTH UINT8_C(20)
#define BME680_GAS_HEATER_PROF_LEN_MAX UINT8_C(10)

/** Register pair and buffer calculation macros */
#define BME680_REG_PAIR_MULTIPLIER UINT8_C(2)
#define BME680_TMP_BUFFER_HALF (BME680_TMP_BUFFER_LENGTH / BME680_REG_PAIR_MULTIPLIER)
#define BME680_GAS_CONFIG_REG_COUNT UINT8_C(2)

/** Duration calculation constants */
#define BME680_TPH_DUR_COEFF UINT32_C(1963)
#define BME680_TPH_SWITCH_DUR_BASE UINT32_C(477)
#define BME680_TPH_SWITCH_MULTIPLIER UINT8_C(4)
#define BME680_GAS_MEAS_DUR_MULTIPLIER UINT8_C(5)
#define BME680_DUR_ROUNDING_FACTOR UINT32_C(500)
#define BME680_MS_PER_SEC UINT32_C(1000)
#define BME680_WAKE_UP_DUR_MS UINT32_C(1)

/** Bit shift and mask constants */
#define BME680_RHRANGE_SHIFT UINT8_C(4)
#define BME680_RHRANGE_DIVISOR UINT8_C(16)
#define BME680_RSERROR_DIVISOR UINT8_C(16)

/** ADC data parsing constants */
#define BME680_ADC_PRES_MULTIPLIER_MSB UINT32_C(4096)
#define BME680_ADC_PRES_MULTIPLIER_MID UINT8_C(16)
#define BME680_ADC_PRES_DIVISOR UINT8_C(16)
#define BME680_ADC_TEMP_MULTIPLIER_MSB UINT32_C(4096)
#define BME680_ADC_TEMP_MULTIPLIER_MID UINT8_C(16)
#define BME680_ADC_TEMP_DIVISOR UINT8_C(16)
#define BME680_ADC_HUM_MULTIPLIER UINT16_C(256)
#define BME680_ADC_GAS_MULTIPLIER UINT8_C(4)
#define BME680_ADC_GAS_DIVISOR UINT8_C(64)

/** Temperature calculation constants */
#define BME680_TEMP_CALC_MULTIPLIER UINT8_C(5)
#define BME680_TEMP_CALC_OFFSET UINT8_C(128)
#define BME680_TEMP_SHIFT_RIGHT UINT8_C(8)
#define BME680_TEMP_ADC_SHIFT_RIGHT UINT8_C(3)
#define BME680_TEMP_PAR_T1_SHIFT_LEFT UINT8_C(1)
#define BME680_TEMP_VAR2_SHIFT_RIGHT UINT8_C(11)
#define BME680_TEMP_VAR3_SHIFT_RIGHT UINT8_C(12)
#define BME680_TEMP_VAR3_SHIFT_RIGHT_2 UINT8_C(14)
#define BME680_TEMP_PAR_T3_SHIFT_LEFT UINT8_C(4)

/** Pressure calculation constants */
#define BME680_PRES_VAR1_SHIFT_RIGHT UINT8_C(1)
#define BME680_PRES_VAR1_SUBTRACT UINT32_C(64000)
#define BME680_PRES_VAR2_SHIFT_RIGHT UINT8_C(2)
#define BME680_PRES_VAR2_SHIFT_RIGHT_2 UINT8_C(11)
#define BME680_PRES_VAR2_SHIFT_RIGHT_3 UINT8_C(2)
#define BME680_PRES_VAR2_SHIFT_RIGHT_4 UINT8_C(12)
#define BME680_PRES_VAR1_SHIFT_RIGHT_2 UINT8_C(13)
#define BME680_PRES_VAR1_SHIFT_RIGHT_3 UINT8_C(3)
#define BME680_PRES_VAR1_SHIFT_RIGHT_4 UINT8_C(18)
#define BME680_PRES_VAR1_SHIFT_RIGHT_5 UINT8_C(15)
#define BME680_PRES_VAR1_ADD UINT32_C(32768)
#define BME680_PRES_COMP_SUBTRACT UINT32_C(1048576)
#define BME680_PRES_COMP_MULTIPLIER UINT32_C(3125)
#define BME680_PRES_VAR4_SHIFT_LEFT UINT8_C(31)
#define BME680_PRES_COMP_SHIFT_LEFT UINT8_C(1)
#define BME680_PRES_COMP_SHIFT_RIGHT UINT8_C(3)
#define BME680_PRES_COMP_SHIFT_RIGHT_2 UINT8_C(13)
#define BME680_PRES_COMP_SHIFT_RIGHT_3 UINT8_C(12)
#define BME680_PRES_COMP_SHIFT_RIGHT_4 UINT8_C(2)
#define BME680_PRES_COMP_SHIFT_RIGHT_5 UINT8_C(8)
#define BME680_PRES_COMP_SHIFT_RIGHT_6 UINT8_C(17)
#define BME680_PRES_COMP_SHIFT_RIGHT_7 UINT8_C(4)
#define BME680_PRES_PAR_P3_SHIFT_LEFT UINT8_C(5)
#define BME680_PRES_PAR_P4_SHIFT_LEFT UINT8_C(16)
#define BME680_PRES_PAR_P7_SHIFT_LEFT UINT8_C(7)
#define BME680_PRES_PAR_P8_SHIFT_RIGHT UINT8_C(13)

/** Humidity calculation constants */
#define BME680_HUM_PAR_H1_MULTIPLIER UINT8_C(16)
#define BME680_HUM_PERCENT_DIVISOR UINT8_C(100)
#define BME680_HUM_VAR1_SHIFT_RIGHT UINT8_C(1)
#define BME680_HUM_VAR2_SHIFT_RIGHT UINT8_C(6)
#define BME680_HUM_VAR2_SHIFT_LEFT UINT8_C(14)
#define BME680_HUM_VAR2_SHIFT_RIGHT_2 UINT8_C(10)
#define BME680_HUM_VAR4_SHIFT_LEFT UINT8_C(7)
#define BME680_HUM_VAR4_SHIFT_RIGHT UINT8_C(4)
#define BME680_HUM_VAR5_SHIFT_RIGHT UINT8_C(14)
#define BME680_HUM_VAR5_SHIFT_RIGHT_2 UINT8_C(10)
#define BME680_HUM_VAR6_SHIFT_RIGHT UINT8_C(1)
#define BME680_HUM_CALC_SHIFT_RIGHT UINT8_C(10)
#define BME680_HUM_CALC_MULTIPLIER UINT32_C(1000)
#define BME680_HUM_CALC_SHIFT_RIGHT_2 UINT8_C(12)
#define BME680_HUM_MAX_VALUE INT32_C(100000)
#define BME680_HUM_MIN_VALUE INT32_C(0)

/** Gas resistance calculation constants */
#define BME680_GAS_VAR1_BASE UINT32_C(1340)
#define BME680_GAS_VAR1_MULTIPLIER UINT8_C(5)
#define BME680_GAS_VAR1_SHIFT_RIGHT UINT8_C(16)
#define BME680_GAS_VAR2_SHIFT_LEFT UINT8_C(15)
#define BME680_GAS_VAR2_SUBTRACT UINT32_C(16777216)
#define BME680_GAS_VAR2_SHIFT_RIGHT UINT8_C(1)
#define BME680_GAS_VAR3_SHIFT_RIGHT UINT8_C(9)

/** Heater resistance calculation constants */
#define BME680_HEATR_TEMP_MIN UINT16_C(200)
#define BME680_HEATR_TEMP_MAX UINT16_C(400)
#define BME680_HEATR_AMBIENT_DIVISOR UINT16_C(1000)
#define BME680_HEATR_AMBIENT_MULTIPLIER UINT16_C(256)
#define BME680_HEATR_PAR_GH1_ADD UINT16_C(784)
#define BME680_HEATR_PAR_GH2_ADD UINT32_C(154009)
#define BME680_HEATR_TEMP_MULTIPLIER UINT8_C(5)
#define BME680_HEATR_TEMP_DIVISOR UINT8_C(100)
#define BME680_HEATR_TEMP_ADD UINT32_C(3276800)
#define BME680_HEATR_TEMP_DIVISOR_2 UINT8_C(10)
#define BME680_HEATR_VAR3_SHIFT_RIGHT UINT8_C(2)
#define BME680_HEATR_RES_RANGE_ADD UINT8_C(4)
#define BME680_HEATR_VAR5_MULTIPLIER UINT8_C(131)
#define BME680_HEATR_VAR5_ADD UINT32_C(65536)
#define BME680_HEATR_VAR5_SUBTRACT UINT16_C(250)
#define BME680_HEATR_VAR5_MULTIPLIER_2 UINT8_C(34)
#define BME680_HEATR_ROUNDING_ADD UINT8_C(50)
#define BME680_HEATR_ROUNDING_DIVISOR UINT8_C(100)

/** Heater duration calculation constants */
#define BME680_HEATR_DUR_MAX_THRESHOLD UINT16_C(0xfc0)
#define BME680_HEATR_DUR_MAX_VALUE UINT8_C(0xff)
#define BME680_HEATR_DUR_DIVISOR UINT8_C(4)
#define BME680_HEATR_DUR_FACTOR_MULTIPLIER UINT8_C(64)
#define BME680_HEATR_DUR_THRESHOLD UINT8_C(0x3F)

/** Field data reading constants */
#define BME680_FIELD_READ_TRIES UINT8_C(10)

/** Field data buffer index definitions */
#define BME680_FIELD_STATUS_INDEX UINT8_C(0)
#define BME680_FIELD_MEAS_INDEX UINT8_C(1)
#define BME680_FIELD_PRES_MSB_INDEX UINT8_C(2)
#define BME680_FIELD_PRES_LSB_INDEX UINT8_C(3)
#define BME680_FIELD_PRES_XLSB_INDEX UINT8_C(4)
#define BME680_FIELD_TEMP_MSB_INDEX UINT8_C(5)
#define BME680_FIELD_TEMP_LSB_INDEX UINT8_C(6)
#define BME680_FIELD_TEMP_XLSB_INDEX UINT8_C(7)
#define BME680_FIELD_HUM_MSB_INDEX UINT8_C(8)
#define BME680_FIELD_HUM_LSB_INDEX UINT8_C(9)
#define BME680_FIELD_GAS_RES_MSB_INDEX UINT8_C(13)
#define BME680_FIELD_GAS_RES_LSB_INDEX UINT8_C(14)

/** Temporary buffer index definitions */
#define BME680_TMP_BUFF_REG_ADDR_INDEX UINT8_C(0)
#define BME680_TMP_BUFF_REG_DATA_INDEX UINT8_C(1)

/** Settings selector */
#define BME680_OST_SEL UINT16_C(1)
#define BME680_OSP_SEL UINT16_C(2)
#define BME680_OSH_SEL UINT16_C(4)
#define BME680_GAS_MEAS_SEL UINT16_C(8)
#define BME680_FILTER_SEL UINT16_C(16)
#define BME680_HCNTRL_SEL UINT16_C(32)
#define BME680_RUN_GAS_SEL UINT16_C(64)
#define BME680_NBCONV_SEL UINT16_C(128)
#define BME680_GAS_SENSOR_SEL (BME680_GAS_MEAS_SEL | BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)

/** Number of conversion settings*/
#define BME680_NBCONV_MIN UINT8_C(0)
#define BME680_NBCONV_MAX UINT8_C(10)

/** Mask definitions */
#define BME680_GAS_MEAS_MSK UINT8_C(0x30)
#define BME680_NBCONV_MSK UINT8_C(0X0F)
#define BME680_FILTER_MSK UINT8_C(0X1C)
#define BME680_OST_MSK UINT8_C(0XE0)
#define BME680_OSP_MSK UINT8_C(0X1C)
#define BME680_OSH_MSK UINT8_C(0X07)
#define BME680_HCTRL_MSK UINT8_C(0x08)
#define BME680_RUN_GAS_MSK UINT8_C(0x10)
#define BME680_MODE_MSK UINT8_C(0x03)
#define BME680_RHRANGE_MSK UINT8_C(0x30)
#define BME680_RSERROR_MSK UINT8_C(0xf0)
#define BME680_NEW_DATA_MSK UINT8_C(0x80)
#define BME680_GAS_INDEX_MSK UINT8_C(0x0f)
#define BME680_GAS_RANGE_MSK UINT8_C(0x0f)
#define BME680_GASM_VALID_MSK UINT8_C(0x20)
#define BME680_HEAT_STAB_MSK UINT8_C(0x10)
#define BME680_MEM_PAGE_MSK UINT8_C(0x10)
#define BME680_SPI_RD_MSK UINT8_C(0x80)
#define BME680_SPI_WR_MSK UINT8_C(0x7f)
#define BME680_BIT_H1_DATA_MSK UINT8_C(0x0F)

/** Bit position definitions for sensor settings */
#define BME680_GAS_MEAS_POS UINT8_C(4)
#define BME680_FILTER_POS UINT8_C(2)
#define BME680_OST_POS UINT8_C(5)
#define BME680_OSP_POS UINT8_C(2)
#define BME680_RUN_GAS_POS UINT8_C(4)

/** Array Index to Field data mapping for Calibration Data*/
#define BME680_T2_LSB_REG (1)
#define BME680_T2_MSB_REG (2)
#define BME680_T3_REG (3)
#define BME680_P1_LSB_REG (5)
#define BME680_P1_MSB_REG (6)
#define BME680_P2_LSB_REG (7)
#define BME680_P2_MSB_REG (8)
#define BME680_P3_REG (9)
#define BME680_P4_LSB_REG (11)
#define BME680_P4_MSB_REG (12)
#define BME680_P5_LSB_REG (13)
#define BME680_P5_MSB_REG (14)
#define BME680_P7_REG (15)
#define BME680_P6_REG (16)
#define BME680_P8_LSB_REG (19)
#define BME680_P8_MSB_REG (20)
#define BME680_P9_LSB_REG (21)
#define BME680_P9_MSB_REG (22)
#define BME680_P10_REG (23)
#define BME680_H2_MSB_REG (25)
#define BME680_H2_LSB_REG (26)
#define BME680_H1_LSB_REG (26)
#define BME680_H1_MSB_REG (27)
#define BME680_H3_REG (28)
#define BME680_H4_REG (29)
#define BME680_H5_REG (30)
#define BME680_H6_REG (31)
#define BME680_H7_REG (32)
#define BME680_T1_LSB_REG (33)
#define BME680_T1_MSB_REG (34)
#define BME680_GH2_LSB_REG (35)
#define BME680_GH2_MSB_REG (36)
#define BME680_GH1_REG (37)
#define BME680_GH3_REG (38)

/** BME680 register buffer index settings*/
#define BME680_REG_FILTER_INDEX UINT8_C(5)
#define BME680_REG_TEMP_INDEX UINT8_C(4)
#define BME680_REG_PRES_INDEX UINT8_C(4)
#define BME680_REG_HUM_INDEX UINT8_C(2)
#define BME680_REG_NBCONV_INDEX UINT8_C(1)
#define BME680_REG_RUN_GAS_INDEX UINT8_C(1)
#define BME680_REG_HCTRL_INDEX UINT8_C(0)

/** Inline function to combine two 8 bit data's to form a 16 bit data */
static inline uint16_t bme680_concat_bytes(uint8_t msb, uint8_t lsb)
{
    return ((uint16_t)msb << BME680_BITS_PER_BYTE) | (uint16_t)lsb;
}

/** Inline function to SET BITS of a register */
static inline uint8_t bme680_set_bits(uint8_t reg_data, uint8_t mask, uint8_t pos, uint8_t data)
{
    return (reg_data & ~mask) | ((data << pos) & mask);
}

/** Inline function to GET BITS of a register */
static inline uint8_t bme680_get_bits(uint8_t reg_data, uint8_t mask, uint8_t pos)
{
    return (reg_data & mask) >> pos;
}

/** Inline function variant to handle the bitname position if it is zero */
static inline uint8_t bme680_set_bits_pos_0(uint8_t reg_data, uint8_t mask, uint8_t data)
{
    return (reg_data & ~mask) | (data & mask);
}

/** Inline function variant to GET BITS when position is zero */
static inline uint8_t bme680_get_bits_pos_0(uint8_t reg_data, uint8_t mask)
{
    return reg_data & mask;
}

/**
 * @fn bme680_com_fptr_t
 * @brief Generic communication function pointer
 * @param dev_id: Place holder to store the id of the device structure
 * @n                 Can be used to store the index of the Chip select or
 * @n                 I2C address of the device.
 * @param reg_addr:    Used to select the register the where data needs to
 *                      be read from or written to.
 * @param data: Data array to read/write
 * @param len: Length of the data array
 * @return int8_t type
 */
typedef int8_t (*bme680_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @fn bme680_delay_fptr_t
 * @brief Delay function pointer
 * @param[in] period: Time period in milliseconds
 */
typedef void (*bme680_delay_fptr_t)(uint32_t period);

/**
 * @enum bme680_intf
 * @brief Interface selection Enumerations
 */
enum bme680_intf {
    BME680_SPI_INTF,
    BME680_I2C_INTF /**< I2C interface */
};

/**
 * @struct bme680_field_data
 * @brief Sensor field data structure
 */
struct bme680_field_data {
    uint8_t status;          /**< Contains new_data, gasm_valid & heat_stab */
    uint8_t gas_index;       /**< The index of the heater profile used */
    uint8_t meas_index;      /**< Measurement index to track order */
    int16_t temperature;     /**< Temperature in degree celsius x100 */
    uint32_t pressure;       /**< Pressure in Pascal */
    uint32_t humidity;       /**< Humidity in % relative humidity x1000 */
    uint32_t gas_resistance; /**< Gas resistance in Ohms */
};

/**
 * @struct bme680_calib_data
 * @brief Structure to hold the Calibration data
 */
struct bme680_calib_data {
    uint16_t par_h1;        /**< Variable to store calibrated humidity data */
    uint16_t par_h2;        /**< Variable to store calibrated humidity data */
    int8_t par_h3;          /**< Variable to store calibrated humidity data */
    int8_t par_h4;          /**< Variable to store calibrated humidity data */
    int8_t par_h5;          /**< Variable to store calibrated humidity data */
    uint8_t par_h6;         /**< Variable to store calibrated humidity data */
    int8_t par_h7;          /**< Variable to store calibrated humidity data */
    int8_t par_gh1;         /**< Variable to store calibrated gas data */
    int16_t par_gh2;        /**< Variable to store calibrated gas data */
    int8_t par_gh3;         /**< Variable to store calibrated gas data */
    uint16_t par_t1;        /**< Variable to store calibrated temperature data */
    int16_t par_t2;         /**< Variable to store calibrated temperature data */
    int8_t par_t3;          /**< Variable to store calibrated temperature data */
    uint16_t par_p1;        /**< Variable to store calibrated pressure data */
    int16_t par_p2;         /**< Variable to store calibrated pressure data */
    int8_t par_p3;          /**< Variable to store calibrated pressure data */
    int16_t par_p4;         /**< Variable to store calibrated pressure data */
    int16_t par_p5;         /**< Variable to store calibrated pressure data */
    int8_t par_p6;          /**< Variable to store calibrated pressure data */
    int8_t par_p7;          /**< Variable to store calibrated pressure data */
    int16_t par_p8;         /**< Variable to store calibrated pressure data */
    int16_t par_p9;         /**< Variable to store calibrated pressure data */
    uint8_t par_p10;        /**< Variable to store calibrated pressure data */
    int32_t t_fine;         /**< Variable to store t_fine size */
    uint8_t res_heat_range; /**< Variable to store heater resistance range */
    int8_t res_heat_val;    /**< Variable to store heater resistance value */
    int8_t range_sw_err;    /**< Variable to store error range */
};

/**
 * @struct bme680_tph_sett
 * @brief BME680 sensor settings structure which comprises of ODR,
 * over-sampling and filter settings.
 */
struct bme680_tph_sett {
    uint8_t os_hum;  /**< Humidity oversampling */
    uint8_t os_temp; /**< Temperature oversampling */
    uint8_t os_pres; /**< Pressure oversampling */
    uint8_t filter;  /**< Filter coefficient */
};

/**
 * @struct bme680_gas_sett
 * @brief BME680 gas sensor which comprises of gas settings
 *  and status parameters
 */
struct bme680_gas_sett {
    uint8_t nb_conv;     /**< Variable to store nb conversion */
    uint8_t heatr_ctrl;  /**< Variable to store heater control */
    uint8_t run_gas;     /**< Run gas enable value */
    uint16_t heatr_temp; /**< Pointer to store heater temperature */
    uint16_t heatr_dur;  /**< Pointer to store duration profile */
};

/**
 * @struct bme680_dev
 * @brief BME680 device structure
 */
struct bme680_dev {
    uint8_t chip_id;                 /**< Chip Id */
    uint8_t dev_id;                  /**< Device Id */
    enum bme680_intf intf;           /**< SPI/I2C interface */
    uint8_t mem_page;                /**< Memory page used */
    int8_t amb_temp;                 /**< Ambient temperature in Degree C*/
    struct bme680_calib_data calib;  /**< Sensor calibration data */
    struct bme680_tph_sett tph_sett; /**< Sensor settings */
    struct bme680_gas_sett gas_sett; /**< Gas Sensor settings */
    uint8_t power_mode;              /**< Sensor power modes */
    uint8_t new_fields;              /**< New sensor fields */
    uint8_t info_msg;                /**< Store the info messages */
    bme680_com_fptr_t read;          /**< Burst read structure */
    bme680_com_fptr_t write;         /**< Burst write structure */
    bme680_delay_fptr_t delay_ms;    /**< Delay in ms */
    int8_t com_rslt;                 /**< Communication function result */
};

#endif
