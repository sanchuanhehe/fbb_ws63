/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#include "bh1750.h"
#include "i2c.h"

#define I2C_MASTER_BUS_ID 1

uint16_t bh1750_buff_H = 0; // 高字节光照数据缓存
uint16_t bh1750_buff_L = 0; // 低字节光照数据缓存

void bh1750_SendCMD(uint8_t cmd){

    uint8_t buffer[] = {cmd}; // 单字节I2C命令缓冲
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer); // 缓冲区长度1字节
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, BH1750_ADDR >> 1, &data); // BH1750 7位地址需右移1位
    if (ret != 0) {
        printf("BH1750:I2cWriteCMD(%02X) failed, %0X!\n", cmd, ret); // 打印命令码和错误码
        return ;
    }

}

void bh1750_ReadData(void){

    uint8_t buffer[2] = {0}; // 接收2字节光照数据
    i2c_data_t data;
    data.receive_len = sizeof(buffer); // 长度2字节
    data.receive_buf = buffer;
    errcode_t ret = uapi_i2c_master_read(I2C_MASTER_BUS_ID, BH1750_ADDR >> 1, &data); // BH1750 7位地址需右移1位
    if (ret != 0) {
        printf("BH1750:I2cRead(len:%d) failed, %0X!\n", data.receive_len, ret); // 打印读取长度和错误码
        return ;
    }
    
    bh1750_buff_H = data.receive_buf[0]; // 存储高8位光照数据
    bh1750_buff_L = data.receive_buf[1]; // 存储低8位光照数据

}

uint16_t bh1750_GetLightIntensity(void){

    uint16_t data;
    bh1750_SendCMD(BH1750_POWER_ON);       // 电源开启命令
    bh1750_SendCMD(MEASURE_MODE);          // 触发一次测量命令
    osal_msleep(200);                       // 等待200ms以完成测量
    bh1750_ReadData();
    data = (bh1750_buff_H << 8) | bh1750_buff_L; // 左移8位合并高、低字节为16位原始值
    return (data * BH1750_RES) / 1.2;     // 按分辨率常数(BH1750_RES)和校准因子1.2计算光照强度
}

void bh1750_init(void){

    bh1750_SendCMD(BH1750_POWER_ON);       // 电源开启初始化
    bh1750_SendCMD(BH1750_MODULE_RESET);   // 模块复位命令
    osal_msleep(200);                       // 等待200ms以完成复位
    osal_printk("BH1750 Init SUCC!\r\n");   // 初始化成功提示

}
