/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#include "aht20.h"
#include "i2c.h"

#define I2C_MASTER_BUS_ID 1

uint8_t aht20_buff[6] = {0}; // 缓冲区长度为6字节

errcode_t i2c_SendData(uint8_t slave_addr, uint8_t *buffer, uint32_t size){
    uint16_t dev_addr = slave_addr;
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = size;
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, dev_addr, &data);
    if (ret != 0) {
        printf("AHT20:I2cWrite(%02X) failed, %0X!\n", data.send_buf[1], ret); // 使用缓冲区第2字节作为命令码进行错误打印
        return ret;
    }
    return ERRCODE_SUCC;
}

void aht20_SendResetCMD(void){

    uint8_t buffer[] = {0xBE, 0x08, 0x00}; // 0xBE: 复位/初始化命令，0x08: 复位触发位，0x00: 占位
    i2c_SendData(AHT20_ADDR >> 1, buffer, sizeof(buffer)); // AHT20 7位地址需右移1位

}

void aht20_SendReadCMD(void){

    uint8_t buffer[] = {0xAC, 0x33, 0x00}; // 0xAC: 触发测量命令，0x33: 校准参数，0x00: 占位
    i2c_SendData(AHT20_ADDR >> 1, buffer, sizeof(buffer)); // AHT20 7位地址需右移1位

}

void aht20_ReadData(void){

    i2c_data_t data;
    data.receive_len = sizeof(aht20_buff);
    data.receive_buf = aht20_buff;
    errcode_t ret = uapi_i2c_master_read(I2C_MASTER_BUS_ID, AHT20_ADDR >> 1, &data); // AHT20 7位地址需右移1位
    if (ret != 0) {
        printf("AHT20:I2cRead(len:%d) failed, %0X!\n", data.receive_len, ret); // 打印读取长度及错误码
        return ;
    }

}

void aht20_GetData(float *temp, float *humi){

    uint32_t tempdata;
    uint32_t humidata;
    aht20_SendReadCMD();
    osal_msleep(80); // 等待80ms以完成测量
    aht20_ReadData();
    if((aht20_buff[0] & 0x80) == 0){ // 检查状态寄存器忙标志(最高位为0表示空闲)
        humidata = ((uint32_t)aht20_buff[1] << 12) + ((uint32_t)aht20_buff[2] << 4) + ((uint32_t)aht20_buff[3] >> 4); // aht20_buff[1] << 12，aht20_buff[2] << 4)+aht20_buff[3] >> 4代表拼接20位湿度原始值
        *humi = humidata * 100.0f / (1<<20); // 将原始值转换为湿度百分比(0-100%)
        tempdata = (((uint32_t)aht20_buff[3] & 0x0F) << 16) + ((uint32_t)aht20_buff[4] << 8) + (uint32_t)aht20_buff[5]; // aht20_buff[3] << 16，aht20_buff[4] << 8)+aht20_buff[5]代表拼接20位湿度原始值拼接20位温度原始值
        *temp = (tempdata * 200.0f / (1<<20)) - 50; // 将原始值转换为温度(−50~150°C)
    }
    
}

void aht20_init(void){

    osal_msleep(50); // 上电后等待50ms以确保传感器稳定
    aht20_SendResetCMD();
    osal_msleep(10); // 复位命令后等待10ms
    aht20_ReadData();
    if((aht20_buff[0] & 0x08) == 0){ // 检查初始化完成标志(状态寄存器第3位)
        aht20_SendResetCMD();
    }
    osal_printk("AHT20 Init SUCC!\r\n");

}
