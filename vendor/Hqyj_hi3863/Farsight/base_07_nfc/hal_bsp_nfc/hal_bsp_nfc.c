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

#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "i2c.h"
#include "osal_task.h"
#include "securec.h"
#include "hal_bsp_nfc.h"

/**
 * @brief  从Page页中组成NDEF协议的包裹
 * @note
 * @param  *dataBuff: 最终的内容
 * @param  dataBuff_MaxSize: 存储缓冲区的长度
 * @retval
 */
uint32_t get_NDEFDataPackage(uint8_t *dataBuff, const uint16_t dataBuff_MaxSize)
{
    if (dataBuff == NULL || dataBuff_MaxSize <= 0) {
        printf("dataBuff==NULL or dataBuff_MaxSize<=0\r\n");
        return ERRCODE_FAIL;
    }

    uint8_t userMemoryPageNum = 0; // 用户的数据操作页数

    // 算出要取多少页
    if (dataBuff_MaxSize <= NFC_PAGE_SIZE) {
        userMemoryPageNum = 1; // 1页
    } else {
        // 需要访问多少页
        userMemoryPageNum = (dataBuff_MaxSize / NFC_PAGE_SIZE) + \
                            ((dataBuff_MaxSize % NFC_PAGE_SIZE) >= 0 ? 1 : 0);
    }

    // 内存拷贝
    uint8_t *p_buff = (uint8_t *)malloc(userMemoryPageNum * NFC_PAGE_SIZE);
    if (p_buff == NULL) {
        printf("p_buff == NULL.\r\n");
        return ERRCODE_FAIL;
    }

    // 读取数据
    for (int i = 0; i < userMemoryPageNum; i++) {
        if (NT3HReadUserData(i) == true) {
            memcpy_s(p_buff + i * NFC_PAGE_SIZE, userMemoryPageNum * NFC_PAGE_SIZE,
                     nfcPageBuffer, NFC_PAGE_SIZE);
        }
    }

    memcpy_s(dataBuff, dataBuff_MaxSize, p_buff, dataBuff_MaxSize);

    free(p_buff);
    p_buff = NULL;

    return ERRCODE_SUCC;
}

/**
 * @brief  NFC引脚初始化
 * @note
 * @retval
 */
uint32_t nfc_Init(void)
{
    uint32_t result;
    uint32_t baudrate = NFC_I2C_SPEED;
    uint32_t hscode = I2C_MASTER_ADDR;
    uapi_pin_set_mode(I2C_SCL_MASTER_PIN, CONFIG_PIN_MODE);
    uapi_pin_set_mode(I2C_SDA_MASTER_PIN, CONFIG_PIN_MODE);       
    uapi_pin_set_pull(I2C_SCL_MASTER_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(I2C_SDA_MASTER_PIN, PIN_PULL_TYPE_UP);
   
    result = uapi_i2c_master_init(NFC_I2C_IDX, baudrate, hscode);
    if (result != ERRCODE_SUCC) {
        printf("I2C Init status is 0x%x!!!\r\n", result);
        return result;
    }
    printf("I2C nfc Init is succeeded!!!\r\n");

    return true;
}