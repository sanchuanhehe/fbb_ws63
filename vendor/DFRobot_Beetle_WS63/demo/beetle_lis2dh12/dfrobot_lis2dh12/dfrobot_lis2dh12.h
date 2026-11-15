#ifndef DFROBOT_LIS2DH12_H
#define DFROBOT_LIS2DH12_H

/*!
 * @file dfrobot_lis2dh12.h
 * @brief Define the basic structure of DFRobot_LIS2DH12
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/DFRobot_LIS
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "systick.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "app_init.h"
#include "i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef ENABLE_DBG
#define DBG(...)                     \
    do {                             \
        Serial.print("[");           \
        Serial.print(__FUNCTION__);  \
        Serial.print("(): ");        \
        Serial.print(__LINE__);      \
        Serial.print(" ] ");         \
        Serial.println(__VA_ARGS__); \
    } while (0)
#else
#define DBG(...)
#endif

#define REG_CARD_ID 0x0F    ///< Chip id
#define REG_CTRL_REG1 0x20  ///< Control register 1
#define REG_CTRL_REG4 0x23  ///< Control register 4
#define REG_CTRL_REG2 0x21  ///< Control register 2
#define REG_CTRL_REG3 0x22  ///< Control register 3
#define REG_CTRL_REG5 0x24  ///< Control register 5
#define REG_CTRL_REG6 0x25  ///< Control register 6
#define REG_STATUS_REG 0x27 ///< Status register
#define REG_OUT_X_L 0x28    ///< The low order of the X-axis acceleration register
#define REG_OUT_X_H 0x29    ///< The high point of the X-axis acceleration register
#define REG_OUT_Y_L 0x2A    ///< The low order of the Y-axis acceleration register
#define REG_OUT_Y_H 0x2B    ///< The high point of the Y-axis acceleration register
#define REG_OUT_Z_L 0x2C    ///< The low order of the Z-axis acceleration register
#define REG_OUT_Z_H 0x2D    ///< The high point of the Z-axis acceleration register
#define REG_INT1_THS 0x32   ///< Interrupt source 1 threshold
#define REG_INT2_THS 0x36   ///< Interrupt source 2 threshold
#define REG_INT1_CFG 0x30   ///< Interrupt source 1 configuration register
#define REG_INT2_CFG 0x34   ///< Interrupt source 2 configuration register
#define REG_INT1_SRC 0x31   ///< Interrupt source 1 status register
#define REG_INT2_SRC 0x35   ///< Interrupt source 2 status register

/** I2C configuration constants */
#define LIS2DH12_I2C_MASTER_ADDR UINT8_C(0x0)
#define LIS2DH12_I2C_SET_BAUDRATE UINT32_C(400000)
#define LIS2DH12_I2C_MASTER_PIN_MODE UINT8_C(2)

/** Default scale velocity constants */
#define LIS2DH12_DEFAULT_MG_SCALE_VEL UINT8_C(16)
#define LIS2DH12_RESET_DEFAULT UINT8_C(0)
#define LIS2DH12_RESET_SET UINT8_C(1)

/** Chip identification constants */
#define LIS2DH12_CHIP_ID_VALID UINT8_C(0x33)
#define LIS2DH12_CHIP_ID_INVALID_1 UINT8_C(0x00)
#define LIS2DH12_CHIP_ID_INVALID_2 UINT8_C(0xFF)

/** Buffer and array size constants */
#define LIS2DH12_REG_ADDR_SIZE UINT8_C(1)
#define LIS2DH12_SENSOR_DATA_SIZE UINT8_C(2)
#define LIS2DH12_ARRAY_INDEX_0 UINT8_C(0)
#define LIS2DH12_ARRAY_INDEX_1 UINT8_C(1)

/** Acceleration scale velocity constants */
#define LIS2DH12_MG_SCALE_2G UINT8_C(16)
#define LIS2DH12_MG_SCALE_4G UINT8_C(32)
#define LIS2DH12_MG_SCALE_8G UINT8_C(64)
#define LIS2DH12_MG_SCALE_16G UINT8_C(192)

/** Acceleration limit constants (in mg) */
#define LIS2DH12_ACC_LIMIT_2G_POS UINT16_C(2000)
#define LIS2DH12_ACC_LIMIT_2G_NEG INT16_C(-2000)
#define LIS2DH12_ACC_LIMIT_4G_POS UINT16_C(4000)
#define LIS2DH12_ACC_LIMIT_4G_NEG INT16_C(-4000)
#define LIS2DH12_ACC_LIMIT_8G_POS UINT16_C(8000)
#define LIS2DH12_ACC_LIMIT_8G_NEG INT16_C(-8000)
#define LIS2DH12_ACC_LIMIT_16G_POS UINT16_C(16000)
#define LIS2DH12_ACC_LIMIT_16G_NEG INT16_C(-16000)

/** Register operation constants */
#define LIS2DH12_REG_READ_MASK UINT8_C(0x80)
#define LIS2DH12_CTRL_REG1_DEFAULT UINT8_C(0x0F)
#define LIS2DH12_CTRL_REG2_DEFAULT UINT8_C(0x00)
#define LIS2DH12_CTRL_REG3_INT1_DEFAULT UINT8_C(0x40)
#define LIS2DH12_CTRL_REG3_INT2_DEFAULT UINT8_C(0x00)
#define LIS2DH12_CTRL_REG5_INT1_DEFAULT UINT8_C(0x08)
#define LIS2DH12_CTRL_REG5_INT2_DEFAULT UINT8_C(0x02)
#define LIS2DH12_CTRL_REG6_INT2_DEFAULT UINT8_C(0x40)
#define LIS2DH12_INT_CFG_BASE UINT8_C(0x80)

/** Threshold calculation constants */
#define LIS2DH12_THRESHOLD_MULTIPLIER UINT16_C(1024)

/** Interrupt status bit masks */
#define LIS2DH12_INT_STATUS_IA_MASK UINT8_C(0x40)
#define LIS2DH12_INT_EVENT_XL_MASK UINT8_C(0x01)
#define LIS2DH12_INT_EVENT_XH_MASK UINT8_C(0x02)
#define LIS2DH12_INT_EVENT_YL_MASK UINT8_C(0x04)
#define LIS2DH12_INT_EVENT_YH_MASK UINT8_C(0x08)
#define LIS2DH12_INT_EVENT_ZL_MASK UINT8_C(0x10)
#define LIS2DH12_INT_EVENT_ZH_MASK UINT8_C(0x20)

/**
 * @fn  e_power_mode_t
 * @brief  Power mode selection, determine the frequency of data collection Represents the number of data collected per
 * second
 */
typedef enum {
    E_POWER_DOWN_0HZ = 0,
    E_LOW_POWER_1HZ = 0x10,
    E_LOW_POWER_10HZ = 0x20,
    E_LOW_POWER_25HZ = 0x30,
    E_LOW_POWER_50HZ = 0x40,
    E_LOW_POWER_100HZ = 0x50,
    E_LOW_POWER_200HZ = 0x60,
    E_LOW_POWER_400HZ = 0x70
} e_power_mode_t;

/**
 * @fn  e_range_t
 * @brief  Sensor range selection
 */
typedef enum {
    E_LIS2DH12_2G = 0x00, /**<±2g>*/
    E_LIS2DH12_4G = 0x10, /**<±4g>*/
    E_LIS2DH12_8G = 0x20, /**<±8g>*/
    E_LIS2DH12_16G = 0x30 /**<±16g>*/
} e_range_t;

/**
 * @fn  e_interrupt_event_t
 * @brief  Interrupt event
 */
typedef enum {
    E_X_LOWER_THAN_TH = 0x01,  /**<The acceleration in the x direction is less than the threshold>*/
    E_X_HIGHER_THAN_TH = 0x02, /**<The acceleration in the x direction is greater than the threshold>*/
    E_Y_LOWER_THAN_TH = 0x04,  /**<The acceleration in the y direction is less than the threshold>*/
    E_Y_HIGHER_THAN_TH = 0x08, /**<The acceleration in the y direction is greater than the threshold>*/
    E_Z_LOWER_THAN_TH = 0x10,  /**<The acceleration in the z direction is less than the threshold>*/
    E_Z_HIGHER_THAN_TH = 0x20, /**<The acceleration in the z direction is greater than the threshold>*/
    E_EVENT_ERROR = 0,         /**< No event>*/
} e_interrupt_event_t;

/**
 * @fn  e_interrupt_source_t
 * @brief  Interrupt pin selection
 */
typedef enum {
    E_INT1 = 0, /**<int1 >*/
    E_INT2,     /**<int2>*/
} e_interrupt_source_t;

/**
 * @fn dfrobot_lis2dh12_init
 * @brief initialization function
 * @param addr I2C slave addr
 * @param iic_scl_master_pin SCL pin
 * @param iic_sda_master_pin SDA pin
 * @param iic_bus_id I2C bus id
 * @return true/false
 */
bool dfrobot_lis2dh12_init(uint8_t addr, uint8_t iic_scl_master_pin, uint8_t iic_sda_master_pin, uint8_t iic_bus_id);

/**
 * @fn set_range
 * @brief Set the measurement range
 * @param range Range(g)
 * @n           E_LIS2DH12_2G, //2g
 * @n           E_LIS2DH12_4G, //4g
 * @n           E_LIS2DH12_8G, //8g
 * @n           E_LIS2DH12_16G, //16g
 */
void set_range(e_range_t range);

/**
 * @fn set_acquire_rate
 * @brief Set data measurement rate
 * @param rate rate(HZ)
 * @n          E_POWER_DOWN_0HZ
 * @n          E_LOW_POWER_1HZ
 * @n          E_LOW_POWER_10HZ
 * @n          E_LOW_POWER_25HZ
 * @n          E_LOW_POWER_50HZ
 * @n          E_LOW_POWER_100HZ
 * @n          E_LOW_POWER_200HZ
 * @n          E_LOW_POWER_400HZ
 */
void set_acquire_rate(e_power_mode_t rate);

/**
 * @fn get_id
 * @brief Get chip id
 * @return 8 bit serial number
 */
uint8_t get_id(void);

/**
 * @fn read_acc_x
 * @brief Get the acceleration in the x direction
 * @return acceleration from x (unit:g), the mearsurement range is ±100g or ±200g, set by set_range() function.
 */
int32_t read_acc_x(void);

/**
 * @fn read_acc_y
 * @brief Get the acceleration in the y direction
 * @return acceleration from y(unit:g), the mearsurement range is ±100g or ±200g, set by set_range() function.
 */
int32_t read_acc_y(void);

/**
 * @fn read_acc_z
 * @brief Get the acceleration in the z direction
 * @return acceleration from z(unit:g), the mearsurement range is ±100g or ±200g, set by set_range() function.
 */
int32_t read_acc_z(void);

/**
 * @fn set_int1_th
 * @brief Set the threshold of interrupt source 1 interrupt
 * @param threshold The threshold is within the measurement range(unit:g)
 */
void set_int1_th(uint8_t threshold);

/**
 * @fn set_int2_th
 * @brief Set interrupt source 2 interrupt generation threshold
 * @param threshold The threshold is within the measurement range(unit:g）
 */
void set_int2_th(uint8_t threshold);

/**
 * @fn enable_interrupt_event
 * @brief Enable interrupt
 * @param source Interrupt pin selection
 * @n           E_INT1 = 0,/<int1 >/
 * @n           E_INT2,/<int2>/
 * @param event Interrupt event selection
 * @n           E_X_LOWER_THAN_TH ,/<The acceleration in the x direction is less than the threshold>/
 * @n           E_X_HIGHER_THAN_TH ,/<The acceleration in the x direction is greater than the threshold>/
 * @n           E_Y_LOWER_THAN_TH,/<The acceleration in the y direction is less than the threshold>/
 * @n           E_Y_HIGHER_THAN_TH,/<The acceleration in the y direction is greater than the threshold>/
 * @n           E_Z_LOWER_THAN_TH,/<The acceleration in the z direction is less than the threshold>/
 * @n           E_Z_HIGHER_THAN_TH,/<The acceleration in the z direction is greater than the threshold>/
 */
void enable_interrupt_event(e_interrupt_source_t source, e_interrupt_event_t event);

/**
 * @fn get_int1_event
 * @brief Check whether the interrupt event'event' is generated in interrupt 1
 * @param event Interrupt event
 * @n           E_X_LOWER_THAN_TH ,/<The acceleration in the x direction is less than the threshold>/
 * @n           E_X_HIGHER_THAN_TH ,/<The acceleration in the x direction is greater than the threshold>/
 * @n           E_Y_LOWER_THAN_TH,/<The acceleration in the y direction is less than the threshold>/
 * @n           E_Y_HIGHER_THAN_TH,/<The acceleration in the y direction is greater than the threshold>/
 * @n           E_Z_LOWER_THAN_TH,/<The acceleration in the z direction is less than the threshold>/
 * @n           E_Z_HIGHER_THAN_TH,/<The acceleration in the z direction is greater than the threshold>/
 * @return true Generated/false Not generated
 */
bool get_int1_event(e_interrupt_event_t event);

/**
 * @fn get_int2_event
 * @brief Check whether the interrupt event'event' is generated in interrupt 1
 * @param event Interrupt event
 * @n           E_X_LOWER_THAN_TH ,/<The acceleration in the x direction is less than the threshold>/
 * @n           E_X_HIGHER_THAN_TH ,/<The acceleration in the x direction is greater than the threshold>/
 * @n           E_Y_LOWER_THAN_TH,/<The acceleration in the y direction is less than the threshold>/
 * @n           E_Y_HIGHER_THAN_TH,/<The acceleration in the y direction is greater than the threshold>/
 * @n           E_Z_LOWER_THAN_TH,/<The acceleration in the z direction is less than the threshold>/
 * @n           E_Z_HIGHER_THAN_TH,/<The acceleration in the z direction is greater than the threshold>/
 * @return true Generated/false Not generated
 */
bool get_int2_event(e_interrupt_event_t event);

/**
 * @fn read_reg
 * @brief read data from sensor chip register
 * @param reg chip register
 * @param p_buf  buf for store data to read
 * @param size  number of data to read
 * @return The number of successfully read data
 */
uint8_t read_reg(uint8_t reg, uint8_t *p_buf, size_t size);

/**
 * @fn write_reg
 * @brief Write data to sensor register
 * @param reg register
 * @param p_buf  buf for store data to write
 * @param size  The number of the data in p_buf
 */
void write_reg(uint8_t reg, const uint8_t *p_buf, size_t size);

#endif
