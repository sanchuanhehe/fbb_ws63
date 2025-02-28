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

#include "hal_bsp_ap3216.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "i2c.h"
#include "securec.h"

#define AP3216C_SYSTEM_ADDR 0x00
#define AP3216C_IR_L_ADDR 0x0A
#define AP3216C_IR_H_ADDR 0x0B
#define AP3216C_ALS_L_ADDR 0x0C
#define AP3216C_ALS_H_ADDR 0x0D
#define AP3216C_PS_L_ADDR 0x0E
#define AP3216C_PS_H_ADDR 0x0F

#define AP3216C_RESET_TIME 50
#define LEFT_SHIFT_2 2
#define LEFT_SHIFT_4 4
#define LEFT_SHIFT_8 8

// 向从机设备 发送数据
static uint32_t AP3216C_WiteByteData(uint8_t byte)
{
    uint8_t buffer[] = {byte};
    i2c_data_t i2cData = {0};
    i2cData.send_buf = &byte;
    i2cData.send_len = sizeof(buffer);
    return uapi_i2c_master_write(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}
// 读从机设备数据
static uint32_t AP3216C_RecvData(uint8_t *data, size_t size)
{
    i2c_data_t i2cData = {0};
    i2cData.receive_buf = data;
    i2cData.receive_len = size;

    return uapi_i2c_master_read(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}

// 向寄存器中写数据
static uint32_t AP3216C_WiteCmdByteData(uint8_t regAddr, uint8_t byte)
{
    uint8_t buffer[] = {regAddr, byte};
    i2c_data_t i2cData = {0};
    i2cData.send_buf = buffer;
    i2cData.send_len = sizeof(buffer);
    return uapi_i2c_master_write(AP3216C_I2C_IDX, AP3216C_I2C_ADDR, &i2cData);
}

// 读寄存器中的数据   参数: regAddr 目标寄存器地址, byte: 取到的数据
static uint32_t AP3216C_ReadRegByteData(uint8_t regAddr, uint8_t *byte)
{
    uint32_t result = 0;
    uint8_t buffer[2] = {0};

    // 写命令
    result = AP3216C_WiteByteData(regAddr);
    if (result != ERRCODE_SUCC) {
        printf("I2C AP3216C status = 0x%x!!!\r\n", result);
        return result;
    }

    // 读数据
    result = AP3216C_RecvData(buffer, 1);
    if (result != ERRCODE_SUCC) {
        printf("I2C AP3216C status = 0x%x!!!\r\n", result);
        return result;
    }
    *byte = buffer[0];

    return ERRCODE_SUCC;
}

uint32_t AP3216C_ReadData(uint16_t *irData, uint16_t *alsData, uint16_t *psData)
{
    uint32_t result = 0;
    uint8_t data_H = 0;
    uint8_t data_L = 0;

    // 读取IR传感器数据    10-bit
    result = AP3216C_ReadRegByteData(AP3216C_IR_L_ADDR, &data_L);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    result = AP3216C_ReadRegByteData(AP3216C_IR_H_ADDR, &data_H);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    if (data_L & 0x80) {
        // IR_OF为1, 则数据无效
        *irData = 0;
    } else {
        // 芯片数据手册中有数据转换的说明
        *irData = ((uint16_t)data_H << LEFT_SHIFT_2) | (data_L & 0x03);
    }

    // 读取ALS传感器数据    16-bit
    result = AP3216C_ReadRegByteData(AP3216C_ALS_L_ADDR, &data_L);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    result = AP3216C_ReadRegByteData(AP3216C_ALS_H_ADDR, &data_H);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    // 芯片数据手册中有数据转换的说明
    *alsData = ((uint16_t)data_H << LEFT_SHIFT_8) | (data_L);

    // 读取PS传感器数据    10-bit
    result = AP3216C_ReadRegByteData(AP3216C_PS_L_ADDR, &data_L);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    result = AP3216C_ReadRegByteData(AP3216C_PS_H_ADDR, &data_H);
    if (result != ERRCODE_SUCC) {
        return result;
    }

    if (data_L & 0x40) {
        // IR_OF为1, 则数据无效
        *psData = 0;
    } else {
        // 芯片数据手册中有数据转换的说明
        *psData = ((uint16_t)(data_H & 0x3F) << LEFT_SHIFT_4) | (data_L & 0x0F);
    }

    return ERRCODE_SUCC;
}

uint32_t AP3216C_Init(void)
{
    uint32_t result;
    uint32_t baudrate = AP3216C_I2C_SPEED;
    uint32_t hscode = I2C_MASTER_ADDR;
    uapi_pin_set_mode(I2C_SCL_MASTER_PIN, CONFIG_PIN_MODE);
    uapi_pin_set_mode(I2C_SDA_MASTER_PIN, CONFIG_PIN_MODE);
    uapi_pin_set_pull(I2C_SCL_MASTER_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(I2C_SDA_MASTER_PIN, PIN_PULL_TYPE_UP);

    result = uapi_i2c_master_init(AP3216C_I2C_IDX, baudrate, hscode);
    if (result != ERRCODE_SUCC) {
        printf("I2C Init status is 0x%x!!!\r\n", result);
        return result;
    }
    osDelay(AP3216C_RESET_TIME);

    // 复位芯片
    result = AP3216C_WiteCmdByteData(AP3216C_SYSTEM_ADDR, 0x04);
    if (result != ERRCODE_SUCC) {
        printf("I2C AP3216C AP3216C_SYSTEM_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }

    osDelay(AP3216C_RESET_TIME);

    // 开启ALS\PS\IR
    result = AP3216C_WiteCmdByteData(AP3216C_SYSTEM_ADDR, 0x03);
    if (result != ERRCODE_SUCC) {
        printf("I2C AP3216C AP3216C_SYSTEM_ADDR status = 0x%x!!!\r\n", result);
        return result;
    }
    printf("I2C AP3216C Init is succeeded!!!\r\n");

    return ERRCODE_SUCC;
}
