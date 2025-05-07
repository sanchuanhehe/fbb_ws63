/**
 *    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#include "cw2015.h"
#include "i2c.h"

#define I2C_MASTER_BUS_ID 1

uint32_t cw2015_vecll_buff_H = 0;
uint32_t cw2015_vecll_buff_L = 0;

void cw2015_SendREG(uint8_t reg, uint8_t reg_data)
{
    uint8_t buffer[] = {reg, reg_data};
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, CW2015_ADDR >> 1, &data);
    if (ret != 0) {
        printf("CW2015:I2cWriteREG(%02X) failed, %0X!\n", reg, ret);
        return;
    }
}

uint8_t cw2015_ReadREG(uint8_t reg)
{
    uint8_t send_buffer[] = {reg};
    uint8_t read_buffer[1] = {0}; // 接收缓冲区，1字节长度
    i2c_data_t data = {0};
    data.send_buf = send_buffer;
    data.send_len = sizeof(send_buffer);
    data.receive_buf = read_buffer;
    data.receive_len = 1; // 仅接收1字节寄存器值
    errcode_t ret = uapi_i2c_master_writeread(I2C_MASTER_BUS_ID, CW2015_ADDR >> 1, &data);
    if (ret != 0) {
        printf("CW2015:I2cReadREG(%02X) failed, %0X!\n", reg, ret);
        return 0;
    }

    return data.receive_buf[0];
}

uint32_t cw2015_GetBatteryVoltage(void)
{

    uint32_t vol;
    cw2015_vecll_buff_H = cw2015_ReadREG(CW2015_REG_VCELL_HIGH);
    cw2015_vecll_buff_L = cw2015_ReadREG(CW2015_REG_VCELL_LOW);
    vol = ((cw2015_vecll_buff_H << 8) | cw2015_vecll_buff_L) * 305 / 1000; // 左移8位原始值*305µV/LSB，除1000换算为mV

    return vol;
}

void cw2015_init(void)
{
    cw2015_SendREG(CW2015_REG_MODE, CW2015_MODE_AWAKE);
    osal_printk("CW2015 Init SUCC!\r\n");
}
