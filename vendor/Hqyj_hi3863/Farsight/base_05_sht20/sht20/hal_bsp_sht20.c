/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
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

#include "hal_bsp_sht20.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "i2c.h"
#include "osal_task.h"
#include "securec.h"
#define SHT20_HoldMaster_Temp_REG_ADDR 0xE3 // 主机模式会阻塞其他IIC设备的通信
#define SHT20_HoldMaster_Humi_REG_ADDR 0xE5
#define SHT20_NoHoldMaster_Temp_REG_ADDR 0xF3
#define SHT20_NoHoldMaster_Humi_REG_ADDR 0xF5
#define SHT20_W_USER_REG_ADDR 0xE6
#define SHT20_R_USER_REG_ADDR 0xE7
#define SHT20_SW_REG_ADDR 0xFE

#define SHT20_READ_TEMP_DATA_TIME 85
#define SHT20_READ_HUMI_DATA_TIME 50
#define SHT20_RESET_TIME 1000

#define TEMP_VALUE_1 175.72
#define TEMP_VALUE_2 65536.0
#define TEMP_VALUE_3 46.85
#define TEMP_LEFT_SHIFT_8 8

#define HUMI_VALUE_1 125
#define HUMI_VALUE_2 65536.0
#define HUMI_VALUE_3 6
#define HUMI_LEFT_SHIFT_8 8

// 读从机设备的数据
static uint32_t SHT20_RecvData(uint8_t *data, size_t size)
{
    i2c_data_t i2cData = {0};
    i2cData.receive_buf = data;
    i2cData.receive_len = size;
  
    return uapi_i2c_master_read(SHT20_I2C_IDX, SHT20_I2C_ADDR, &i2cData);
}

// 向从机设备 发送数据
 uint32_t SHT20_WiteByteData(uint8_t byte)
{
    uint8_t buffer[] = {byte};
    i2c_data_t i2cData = {0};
    i2cData.send_buf = buffer;
    i2cData.send_len = sizeof(buffer);

    return  uapi_i2c_master_write(SHT20_I2C_IDX, SHT20_I2C_ADDR, &i2cData);
}
#define READ_TEMP_DATA_NUM 3
#define READ_HUMI_DATA_NUM 3

// 读温湿度的值
uint32_t SHT20_ReadData(float *temp, float *humi)
{
    uint32_t result;
    uint8_t buffer[4] = {0};

    /* 发送检测温度命令 */
    result = SHT20_WiteByteData(SHT20_NoHoldMaster_Temp_REG_ADDR);
    if (result != ERRCODE_SUCC) {
        printf("I2C SHT20 status = 0x%x!!!\r\n", result);
        return result;
    }

     osal_msleep(SHT20_READ_TEMP_DATA_TIME); /* datasheet: typ=66, max=85 */

    // 读数据
    result = SHT20_RecvData(buffer, READ_TEMP_DATA_NUM);
    if (result != ERRCODE_SUCC) {
        printf("I2C  SHT20 status = 0x%x!!!\r\n", result);
        return result;
    }

    // 看芯片手册，手册中有转换公式的说明
    *temp = TEMP_VALUE_1 * (((((int)buffer[0]) << TEMP_LEFT_SHIFT_8) + buffer[1]) / TEMP_VALUE_2) - TEMP_VALUE_3;

    memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));

    /* 发送检测湿度命令 */
    result = SHT20_WiteByteData(SHT20_NoHoldMaster_Humi_REG_ADDR);
    if (result != ERRCODE_SUCC) {
        printf("I2C SHT20 status = 0x%x!!!\r\n", result);
        return result;
    }

     osal_msleep(SHT20_READ_HUMI_DATA_TIME); /* datasheet: typ=22, max=29 */

    // 读数据
    result = SHT20_RecvData(buffer, READ_HUMI_DATA_NUM);
    if (result != ERRCODE_SUCC) {
        printf("I2C SHT20 status = 0x%x!!!\r\n", result);
        return result;
    }

    // 看芯片手册，手册中有转换公式的说明
    *humi = HUMI_VALUE_1 * (((((int)buffer[0]) << HUMI_LEFT_SHIFT_8) + buffer[1]) / HUMI_VALUE_2) - HUMI_VALUE_3;

    return ERRCODE_SUCC;
}


   

// 传感器 SHT20 的初始化
uint32_t SHT20_Init(void)
{
    uint32_t result;
    uint32_t baudrate = SHT20_I2C_SPEED;
    uint32_t hscode = I2C_MASTER_ADDR;
    uapi_pin_set_mode(I2C_SCL_MASTER_PIN, CONFIG_PIN_MODE);
    uapi_pin_set_mode(I2C_SDA_MASTER_PIN, CONFIG_PIN_MODE);       
    uapi_pin_set_pull(I2C_SCL_MASTER_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(I2C_SDA_MASTER_PIN, PIN_PULL_TYPE_UP);
   
    result = uapi_i2c_master_init(SHT20_I2C_IDX, baudrate, hscode);
    if (result != ERRCODE_SUCC) {
        printf("I2C Init status is 0x%x!!!\r\n", result);
        return result;
    }
     osal_msleep(100);
    // 软复位
    result = SHT20_WiteByteData(SHT20_SW_REG_ADDR);
    if (result != ERRCODE_SUCC) {
        printf("I2C SHT20 status = 0x%x!!!\r\n", result);
        return result;
    }

    osal_msleep(SHT20_RESET_TIME);
    printf("I2C SHT20 Init is succeeded!!!\r\n");
    return ERRCODE_SUCC;
}
