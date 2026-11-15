/*!
 * @file dfrobot_c4001.h
 * @brief Define the basic structure of the DFRobot_C4001 class, the implementation of the basic methods
 * @copyright    Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version V1.0
 * @date 2025-09-29
 * @url https://github.com/DFRobot/DFRobot_C4001
 */
#ifndef __DFROBOT_C4001_H__
#define __DFROBOT_C4001_H__

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "systick.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "app_init.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @struct s_sensor_status_t
 * @brief sensor status
 * @note sensor status
 */
typedef struct {
    uint8_t work_status;
    uint8_t work_mode;
    uint8_t init_status;
} s_sensor_status_t;

/**
 * @struct s_private_data_t
 * @brief speed mode data
 */
typedef struct {
    uint8_t number;
    float speed;
    float range;
    uint32_t energy;
} s_private_data_t;

/**
 * @struct s_response_data_t
 * @brief response data
 */
typedef struct {
    bool status;
    float response1;
    float response2;
    float response3;
} s_response_data_t;

/**
 * @struct s_pwm_data_t
 * @brief config pwm data param
 */
typedef struct {
    uint8_t pwm1;
    uint8_t pwm2;
    uint8_t timer;
} s_pwm_data_t;

/**
 * @struct s_all_data_t
 * @brief sensor return data
 */
typedef struct {
    uint8_t exist;
    s_sensor_status_t sta;
    s_private_data_t target;
} s_all_data_t;

/**
 * @enum e_mode_t
 * @brief sensor work mode
 */
typedef enum {
    EXITMODE = 0x00,
    SPEEDMODE = 0x01,
} e_mode_t;

/**
 * @enum e_switch_t
 * @brief Micromotion detection switch
 */
typedef enum {
    SWITCH_ON = 0x01,
    SWITCH_OFF = 0x00,
} e_switch_t;

/**
 * @enum e_set_mode_t
 * @brief Set parameters for the sensor working status
 */
typedef enum {
    STARTSEN = 0x55,
    STOPSEN = 0x33,
    RESETSEN = 0xCC,
    RECOVERSEN = 0xAA,
    SAVEPARAMS = 0x5C,
    CHANGEMODE = 0x3B,
} e_set_mode_t;

#define START_SENSOR "sensorStart"
#define STOP_SENSOR "sensorStop"
#define SAVE_CONFIG "saveConfig"
#define RECOVER_SENSOR "resetCfg"  ///< factory data reset
#define RESET_SENSOR "resetSystem" ///< RESET_SENSOR
#define SPEED_MODE "setRunApp 1"
#define EXIST_MODE "setRunApp 0"

/** Timeout and delay constants */
#define C4001_TIME_OUT_MS UINT8_C(100)
#define C4001_DELAY_MS_BASE UINT8_C(1)
#define C4001_DELAY_50_MS (50 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_100_MS (100 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_200_MS (200 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_400_MS (400 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_500_MS (500 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_800_MS (800 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_1000_MS (1000 * C4001_DELAY_MS_BASE)
#define C4001_DELAY_1500_MS (1500 * C4001_DELAY_MS_BASE)

/** Buffer size constants */
#define C4001_UART_TRANSFER_SIZE UINT16_C(200)
#define C4001_TEMP_BUFFER_SIZE UINT8_C(200)
#define C4001_STATUS_BUFFER_SIZE UINT8_C(100)
#define C4001_CMD_BUFFER_SIZE UINT8_C(64)
#define C4001_IO_BUFFER_SIZE UINT8_C(32)
#define C4001_SPACE_ARRAY_SIZE UINT8_C(5)
#define C4001_PARTS_ARRAY_SIZE UINT8_C(10)

/** Array index constants */
#define C4001_ARRAY_INDEX_0 UINT8_C(0)
#define C4001_ARRAY_INDEX_1 UINT8_C(1)
#define C4001_ARRAY_INDEX_2 UINT8_C(2)
#define C4001_ARRAY_INDEX_3 UINT8_C(3)
#define C4001_ARRAY_INDEX_4 UINT8_C(4)
#define C4001_ARRAY_INDEX_5 UINT8_C(5)
#define C4001_ARRAY_INDEX_7 UINT8_C(7)
#define C4001_ARRAY_INDEX_8 UINT8_C(8)
#define C4001_ARRAY_INDEX_9 UINT8_C(9)
#define C4001_ARRAY_INDEX_10 UINT8_C(10)
#define C4001_ARRAY_INDEX_15 UINT8_C(15)
#define C4001_ARRAY_INDEX_19 UINT8_C(19)

/** String parsing constants */
#define C4001_STR_CHAR_R 'R'
#define C4001_STR_CHAR_E 'e'
#define C4001_STR_CHAR_S 's'
#define C4001_STR_CHAR_SPACE ' '
#define C4001_STR_CHAR_DOLLAR '$'
#define C4001_STR_DFHPD_LEN (strlen("$DFHPD"))
#define C4001_STR_DFDMD_LEN (strlen("$DFDMD"))

/** Sensitivity and range limits */
#define C4001_SENSITIVITY_MAX UINT8_C(9)
#define C4001_TRIG_DELAY_MAX UINT16_C(200)
#define C4001_KEEP_DELAY_MIN UINT16_C(4)
#define C4001_KEEP_DELAY_MAX UINT16_C(3000)
#define C4001_RANGE_MAX_LIMIT UINT16_C(2500)
#define C4001_RANGE_MAX_MIN UINT16_C(240)
#define C4001_RANGE_MAX_MAX UINT16_C(2000)
#define C4001_RANGE_MIN_MIN UINT16_C(30)
#define C4001_RANGE_MIN_MAX UINT16_C(2000)
#define C4001_PWM_MAX UINT8_C(100)
#define C4001_TIMER_MAX UINT8_C(255)
#define C4001_IO_POLARITY_MAX UINT8_C(1)

/** Float conversion constants */
#define C4001_RANGE_TO_METER_DIVISOR 100.0f
#define C4001_TRIG_DELAY_TO_SEC_MULTIPLIER 0.01f
#define C4001_KEEP_DELAY_TO_SEC_MULTIPLIER 0.5f
#define C4001_TRIG_DELAY_FROM_SEC_MULTIPLIER 100.0f
#define C4001_KEEP_DELAY_FROM_SEC_MULTIPLIER 2.0f
#define C4001_RANGE_FROM_METER_MULTIPLIER 100.0f

/** String position constants */
#define C4001_STR_POS_DFHPD_EXIST_OFFSET UINT8_C(7)
#define C4001_STR_POS_SENSITIVITY_TRIG_OFFSET UINT8_C(19)
#define C4001_STR_POS_SENSITIVITY_KEEP_OFFSET UINT8_C(15)
#define C4001_STR_POS_PARTS_INDEX_MAX UINT8_C(8)

/** Flash counter threshold */
#define C4001_FLASH_COUNTER_THRESHOLD UINT8_C(10)

/**
 * @fn dfrobot_c4001_init
 * @brief initialization function
 * @param baud UART baud
 * @param txpin UART TX pin
 * @param rxpin UART RX pin
 * @param uart_id UART bus id
 */
void dfrobot_c4001_init(uint32_t baud, uint8_t txpin, uint8_t rxpin, uint8_t uart_id);

/**
 * @fn motion_detection
 * @brief motion Detection
 * @return true or false
 */
bool motion_detection(void);

/**
 * @fn set_sensor
 * @brief Set the Sensor object
 * @param mode
 * @n  STARTSEN        start collect
 * @n  STOPSEN         stop collect
 * @n  RESETSEN        reset sensor
 * @n  RECOVERSEN      recover params
 * @n  SAVEPARAMS      save config
 * @n  CHANGEMODE      chagne mode
 */
void set_sensor(e_set_mode_t mode);

/**
 * @fn set_delay
 * @brief Set the Delay object
 * @param trig Trigger delay, unit 0.01s, range 0~2s (0~200)
 * @param keep Maintain the detection timeout, unit 0.5s, range 2~1500 seconds (4~3000)
 * @return true or false
 */
bool set_delay(uint8_t trig, uint16_t keep);

/**
 * @fn get_trig_delay
 * @brief Get the Trig Delay object
 * @return uint8_t
 */
uint8_t get_trig_delay(void);

/**
 * @fn get_keep_timeout
 * @brief get keep timer out
 * @return  uint16_t
 */
uint16_t get_keep_timeout(void);

/**
 * @fn get_trig_range
 * @brief Get the Trig Range object
 * @n     The triggering distance, in cm, ranges from 2.4 to 20m (240 to 2000).
 * @n     The actual configuration range does not exceed the maximum and minimum detection distance.
 * @return uint16_t
 */
uint16_t get_trig_range(void);

/**
 * @fn get_max_range
 * @brief Get the Max Range object
 * @return  uint16_t
 */
uint16_t get_max_range(void);

/**
 * @fn get_min_range
 * @brief Get the Min Range object
 * @return uint16_t
 */
uint16_t get_min_range(void);

/**
 * @fn set_detection_range
 * @brief Set the Detection Range object
 * @param min Detection range Minimum distance, unit cm, range 0.3~20m (30~2000), not exceeding max, otherwise the
 * function is abnormal.
 * @param max Detection range Maximum distance, unit cm, range 2.4~20m (240~2000)
 * @param trig The trigger distance (unit: cm) ranges from 2.4 to 20m (240 to 2000). The actual configuration range does
 * not exceed the maximum and minimum detection distance.
 * @return true or false
 */
bool set_detection_range(uint16_t min, uint16_t max, uint16_t trig);

/**
 * @fn set_trig_sensitivity
 * @brief Set trigger sensitivity, 0~9
 * @param sensitivity
 * @return true or false
 */
bool set_trig_sensitivity(uint8_t sensitivity);

/**
 * @fn set_keep_sensitivity
 * @brief Set the Keep Sensitivity object，0~9
 * @param sensitivity
 * @return true or false
 */
bool set_keep_sensitivity(uint8_t sensitivity);

/**
 * @fn get_trig_sensitivity
 * @brief Get the Trig Sensitivity object
 * @return uint8_t
 */
uint8_t get_trig_sensitivity(void);

/**
 * @fn get_keep_sensitivity
 * @brief Get the Keep Sensitivity object
 * @return uint8_t
 */
uint8_t get_keep_sensitivity(void);

/**
 * @fn get_status
 * @brief Get the Status object
 * @return s_sensor_status_t
 * @n     work_status
 * @n       0 stop
 * @n       1 start
 * @n     work_mode
 * @n       0 indicates presence detection
 * @n       1 is speed measurement and ranging
 * @n     init_status
 * @n       0 not init
 * @n       1 init success
 */
s_sensor_status_t get_status(void);

/**
 * @fn set_io_polarity
 * @brief Set the Io Polaity object
 * @param value
 * @n     0：Output low level when there is a target, output high level when there is no target
 * @n     1: Output high level when there is a target, output low level when there is no target (default)
 * @return true or false
 */
bool set_io_polarity(uint8_t value);

/**
 * @fn get_io_polarity
 * @brief Get the Io Polaity object
 * @return uint8_t The level of the signal output by the pin after the configured I/O port detects the target
 */
uint8_t get_io_polarity(void);

/**
 * @fn set_pwm
 * @brief Set the Pwm object
 * @param pwm1 When no target is detected, the duty cycle of the output signal of the OUT pin ranges from 0 to 100
 * @param pwm2 After the target is detected, the duty cycle of the output signal of the OUT pin ranges from 0 to 100
 * @param timer timer The value ranges from 0 to 255, corresponding to timer x 64ms
 * @n     For example, timer=20, it takes 20*64ms=1.28s for the duty cycle to change from pwm1 to pwm2.
 * @return true or false
 */
bool set_pwm(uint8_t pwm1, uint8_t pwm2, uint8_t timer);

/**
 * @fn get_pwm
 * @brief Get the Pwm object
 * @return s_pwm_data_t
 * @retval pwm1  When no target is detected, the duty cycle of the output signal of the OUT pin ranges from 0 to 100
 * @retval pwm2  After the target is detected, the duty cycle of the output signal of the OUT pin ranges from 0 to 100
 * @retval timer The value ranges from 0 to 255, corresponding to timer x 64ms
 * @n      For example, timer=20, it takes 20*64ms=1.28s for the duty cycle to change from pwm1 to pwm2.
 */
s_pwm_data_t get_pwm(void);

/**
 * @fn set_sensor_mode
 * @brief Set the Sensor Mode object
 * @param mode
 * @return true or false
 */
bool set_sensor_mode(e_mode_t mode);

/**
 * @fn get_target_number
 * @brief Get the Target Number object
 * @return uint8_t
 */
uint8_t get_target_number(void);

/**
 * @fn get_target_speed
 * @brief Get the Target Speed object
 * @return float
 */
float get_target_speed(void);

/**
 * @fn get_target_range
 * @brief Get the Target Range object
 * @return float
 */
float get_target_range(void);

/**
 * @fn get_target_energy
 * @brief Get the Target Energy object
 * @return uint32_t
 */
uint32_t get_target_energy(void);

/**
 * @fn set_detect_thres
 * @brief Set the Detect Thres object
 * @param min Detection range Minimum distance, unit cm, range 0.3~20m (30~2000), not exceeding max, otherwise the
 * function is abnormal.
 * @param max Detection range Maximum distance, unit cm, range 2.4~20m (240~2000)
 * @param thres Target detection threshold, dimensionless unit 0.1, range 0~6553.5 (0~65535)
 * @return true or false
 */
bool set_detect_thres(uint16_t min, uint16_t max, uint16_t thres);

/**
 * @fn get_t_min_range
 * @brief get speed Min Range
 * @return uint16_t
 */
uint16_t get_t_min_range(void);

/**
 * @fn get_t_max_range
 * @brief get speed Max Range
 * @return uint16_t
 */
uint16_t get_t_max_range(void);

/**
 * @fn get_thres_range
 * @brief Get the Thres Range object
 * @return uint16_t
 */
uint16_t get_thres_range(void);

/**
 * @fn set_fretting_detection
 * @brief Set the Fretting Detection object
 * @param sta
 */
void set_fretting_detection(e_switch_t sta);

/**
 * @fn get_fretting_detection
 * @brief Get the Fretting Detection object
 * @return e_switch_t
 */
e_switch_t get_fretting_detection(void);

#endif
