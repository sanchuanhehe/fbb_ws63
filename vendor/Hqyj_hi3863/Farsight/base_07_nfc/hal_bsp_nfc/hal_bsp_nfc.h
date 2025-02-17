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

#ifndef HAL_BSP_NFC_H
#define HAL_BSP_NFC_H

#include "cmsis_os2.h"
#include "NT3H.h"
#define NFC_I2C_ADDR 0x55  // 器件的I2C从机地址
#define NFC_I2C_IDX 1        // 模块的I2C总线号
#define NFC_I2C_SPEED 100000 // 100KHz
#define I2C_MASTER_ADDR          0x0    
/* io*/
#define I2C_SCL_MASTER_PIN 16
#define I2C_SDA_MASTER_PIN 15
#define CONFIG_PIN_MODE 2

#define NDEF_HEADER_SIZE 0x2 // NDEF协议的头部大小

#define NDEF_PROTOCOL_HEADER_OFFSET 0           // NDEF协议头(固定)
#define NDEF_PROTOCOL_LENGTH_OFFSET 1           // NDEF协议数据的总长度位
#define NDEF_PROTOCOL_MEG_CONFIG_OFFSET 2       // 标签的控制字节位
#define NDEF_PROTOCOL_DATA_TYPE_LENGTH_OFFSET 3 // 标签数据类型的长度位
#define NDEF_PROTOCOL_DATA_LENGTH_OFFSET 4      // 标签的数据长度位
#define NDEF_PROTOCOL_DATA_TYPE_OFFSET 6        // 标签的数据类型位
#define NDEF_PROTOCOL_VALID_DATA_OFFSET 20      // 有效数据位
/**
 * @brief  从Page页中组成NDEF协议的包裹
 * @note
 * @param  *dataBuff: 最终的内容
 * @param  dataBuff_MaxSize: 存储缓冲区的长度
 * @retval
 */
uint32_t get_NDEFDataPackage(uint8_t *dataBuff, const uint16_t dataBuff_MaxSize);

/**
 * @brief  NFC传感器的引脚初始化
 * @note
 * @retval
 */
uint32_t nfc_Init(void);

#endif /* __HAL_BSP_NFC_H__ */
