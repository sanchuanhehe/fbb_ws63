/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
  ******************************************************************************
  * @file   bsp_ft6336.h
  * @brief  2.8寸屏ft6336驱动文件,i2c接口
  * 
  ******************************************************************************
  */
#ifndef __BSP_FT6336_H__
#define __BSP_FT6336_H__

#include <stdio.h>
#include <stdlib.h>
#include "i2c.h"
#include "gpio.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "osal_task.h"


#define I2C_MASTER_ADDR              0x0              
#define I2C_ADDR_FT6336 0x38 // 器件的I2C从机地址
#define FT6336_ADDR_READ  0x71
#define FT6336_ADDR_WRITE 0x70
#define FT6336_I2C_IDX 1        // 模块的I2C总线号
#define FT6336_I2C_SPEED 100000 // 100KHz

#define TOUCH_INT 1
#define TOUCH_INT_MASTER_PIN 14
#define I2C_SCL_MASTER_PIN 16
#define I2C_SDA_MASTER_PIN 15
#define CONFIG_PIN_MODE 2

// Touch Parameter
#define FT6336_PRES_DOWN 0x2
#define FT6336_COORD_UD 0x1

// Registers
#define FT6336_ADDR_DEVICE_MODE 0x00

typedef enum{
    working_mode = 0x00,
    factory_mode = 0x04,
}DEVICE_MODE_Enum;

#define FT6336_ADDR_GESTURE_ID 0x01
#define FT6336_ADDR_TD_STATUS 0x02

#define FT6336_ADDR_TOUCH1_EVENT 0x03
#define FT6336_ADDR_TOUCH1_ID 0x05
#define FT6336_ADDR_TOUCH1_X 0x03
#define FT6336_ADDR_TOUCH1_Y 0x05
#define FT6336_ADDR_TOUCH1_WEIGHT 0x07
#define FT6336_ADDR_TOUCH1_MISC 0x08

#define FT6336_ADDR_TOUCH2_EVENT 0x09
#define FT6336_ADDR_TOUCH2_ID 0x0B
#define FT6336_ADDR_TOUCH2_X 0x09
#define FT6336_ADDR_TOUCH2_Y 0x0B
#define FT6336_ADDR_TOUCH2_WEIGHT 0x0D
#define FT6336_ADDR_TOUCH2_MISC 0x0E

#define FT6336_ADDR_THRESHOLD 0x80
#define FT6336_ADDR_FILTER_COE 0x85
#define FT6336_ADDR_CTRL 0x86

typedef enum
{
    keep_active_mode = 0,
    switch_to_monitor_mode = 1,
} CTRL_MODE_Enum;

#define FT6336_ADDR_TIME_ENTER_MONITOR 0x87
#define FT6336_ADDR_ACTIVE_MODE_RATE 0x88
#define FT6336_ADDR_MONITOR_MODE_RATE 0x89

#define FT6336_ADDR_RADIAN_VALUE 0x91
#define FT6336_ADDR_OFFSET_LEFT_RIGHT 0x92
#define FT6336_ADDR_OFFSET_UP_DOWN 0x93
#define FT6336_ADDR_DISTANCE_LEFT_RIGHT 0x94
#define FT6336_ADDR_DISTANCE_UP_DOWN 0x95
#define FT6336_ADDR_DISTANCE_ZOOM 0x96

#define FT6336_ADDR_LIBRARY_VERSION_H 0xA1
#define FT6336_ADDR_LIBRARY_VERSION_L 0xA2
#define FT6336_ADDR_CHIP_ID 0xA3
#define FT6336_ADDR_G_MODE 0xA4

typedef enum
{
    pollingMode = 0,
    triggerMode = 1,
} G_MODE_Enum;
#define FT6336_ADDR_POWER_MODE 0xA5
#define FT6336_ADDR_FIRMARE_ID 0xA6
#define FT6336_ADDR_FOCALTECH_ID 0xA8
#define FT6336_ADDR_RELEASE_CODE_ID 0xAF
#define FT6336_ADDR_STATE 0xBC

// Function Specific Type
typedef enum
{
    touch = 0,
    stream,
    release,
} TouchStatusEnum;

typedef struct
{
    TouchStatusEnum status;
    uint16_t x;
    uint16_t y;
} TouchPointType;

typedef struct
{
    uint8_t touch_count;
    TouchPointType tp[2];
} FT6336_TouchPointType;

/**************************************************************************/
/*!
    @brief  FT6336 I2C CTP controller driver
*/
/**************************************************************************/

void FT6336_init(void);

uint8_t FT6336_read_device_mode(void);
void FT6336_write_device_mode(DEVICE_MODE_Enum);
uint8_t FT6336_read_gesture_id(void);
uint8_t FT6336_read_td_status(void);
uint8_t FT6336_read_touch_number(void);
uint16_t FT6336_read_touch1_x(void);
uint16_t FT6336_read_touch1_y(void);
uint8_t FT6336_read_touch1_event(void);
uint8_t FT6336_read_touch1_id(void);
uint8_t FT6336_read_touch1_weight(void);
uint8_t FT6336_read_touch1_misc(void);
uint16_t FT6336_read_touch2_x(void);
uint16_t FT6336_read_touch2_y(void);
uint8_t FT6336_read_touch2_event(void);
uint8_t FT6336_read_touch2_id(void);
uint8_t FT6336_read_touch2_weight(void);
uint8_t FT6336_read_touch2_misc(void);

// Mode Parameter Register
uint8_t FT6336_read_touch_threshold(void);
uint8_t FT6336_read_filter_coefficient(void);
uint8_t FT6336_read_ctrl_mode(void);
void FT6336_write_ctrl_mode(CTRL_MODE_Enum mode);
uint8_t FT6336_read_time_period_enter_monitor(void);
uint8_t FT6336_read_active_rate(void);
uint8_t FT6336_read_monitor_rate(void);

// Gestrue Parameter Register
uint8_t FT6336_read_radian_value(void);
void FT6336_write_radian_value(uint8_t val);
uint8_t FT6336_read_offset_left_right(void);
void FT6336_write_offset_left_right(uint8_t val);
uint8_t FT6336_read_offset_up_down(void);
void FT6336_write_offset_up_down(uint8_t val);
uint8_t FT6336_read_distance_left_right(void);
void FT6336_write_distance_left_right(uint8_t val);
uint8_t FT6336_read_distance_up_down(void);
void FT6336_write_distance_up_down(uint8_t val);
uint8_t FT6336_read_distance_zoom(void);
void FT6336_write_distance_zoom(uint8_t val);

// System Information
uint16_t FT6336_read_library_version(void);
uint8_t FT6336_read_chip_id(void);
uint8_t FT6336_read_g_mode(void);
void FT6336_write_g_mode(G_MODE_Enum mode);
uint8_t FT6336_read_pwrmode(void);
uint8_t FT6336_read_firmware_id(void);
uint8_t FT6336_read_focaltech_id(void);
uint8_t FT6336_read_release_code_id(void);
uint8_t FT6336_read_state(void);

// Scan Function
FT6336_TouchPointType FT6336_scan(void);
//
void FT6336_irq_fuc(void);
void FT6336_scan_task(void);	
#endif

