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

#ifndef HAL_BSP_SHT20_H
#define HAL_BSP_SHT20_H

#include "cmsis_os2.h"

#define SHT20_I2C_ADDR 0x40    // 器件的I2C从机地址
#define SHT20_I2C_IDX 1        // 模块的I2C总线号
#define SHT20_I2C_SPEED 100000 // 100KHz
#define I2C_MASTER_ADDR 0x0
/* io */
#define I2C_SCL_MASTER_PIN 16
#define I2C_SDA_MASTER_PIN 15
#define CONFIG_PIN_MODE 2

/**
 * @brief SHT20 读取器件的温湿度
 * @param temp 温度值
 * @param humi 湿度值
 * @return Returns {@link IOT_SUCCESS} 成功;
 *         Returns {@link IOT_FAILURE} 失败.
 */
uint32_t SHT20_ReadData(float *temp, float *humi);
/**
 * @brief SHT20 初始化
 * @return Returns {@link IOT_SUCCESS} 成功;
 *         Returns {@link IOT_FAILURE} 失败.
 */
uint32_t SHT20_Init(void);
uint32_t SHT20_WiteByteData(uint8_t byte);
#endif // !__HAL_BSP_SHT20_H__
