/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"

#include "app_init.h"
#include "ins5699s.h"
#include "i2c.h"
#include "bmx055.h"

#define I2C_MASTER_BUS_ID 1

void BMX055_SendREG(uint8_t devAddr, uint8_t reg, uint8_t reg_data){

    uint8_t buffer[] = {reg, reg_data}; // 两字节：寄存器地址和要写入的数据
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer); // 发送长度2字节
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, devAddr, &data);
    if (ret != 0) {
        printf("BMX055:I2cWriteREG(%02X) failed, %0X!\n", reg, ret); // 打印失败的寄存器地址和错误码
        return ;
    }
}

uint8_t BMX055_ReadREG(uint8_t devAddr, uint8_t reg){

    uint8_t send_buffer[] = {reg}; // 写入要读取的寄存器地址
    uint8_t read_buffer[1];        // 接收1字节寄存器数据
    i2c_data_t data = {0};
    data.send_buf = send_buffer;
    data.send_len = sizeof(send_buffer); // 发送长度1字节
    data.receive_buf = read_buffer;
    data.receive_len = 1;                 // 接收长度1字节
    errcode_t ret = uapi_i2c_master_writeread(I2C_MASTER_BUS_ID, devAddr, &data);
    if (ret != 0) {
        printf("BMX055:I2cReadREG(%02X) failed, %0X!\n", reg, ret); // 打印失败的寄存器地址和错误码
        return 0; // 读取失败返回0
    }

    return data.receive_buf[0]; // 返回寄存器值
}

void BMX055_ReadBytes(uint8_t devAddr, uint8_t startAddr, uint8_t *dst, uint8_t len){
    uint8_t sendBuf[1] = { startAddr }; // 起始寄存器地址

    i2c_data_t i2cData;
    i2cData.send_buf    = sendBuf;
    i2cData.send_len    = 1;    // 发送长度1字节
    i2cData.receive_buf = dst;
    i2cData.receive_len = len;  // 接收长度len字节

    errcode_t ret = uapi_i2c_master_writeread(I2C_MASTER_BUS_ID, devAddr, &i2cData);
    if (ret != 0) {
        printf("BMX055_ReadBytes(0x%02X, len=%d) failed, %X!\n", 
                startAddr, len, ret); // 打印起始地址、长度和错误码
    }
}

void bmx055_init(void){
    osal_msleep(50); // 上电后延时50ms确保传感器稳定

    BMX055_SendREG(BMX055_ACC_ADDRESS, 0x0F, 0x03);   // 加速度计±2g范围设置
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_ACC_ADDRESS, 0x10, 0x0F);   // 输出数据速率和带宽配置
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_ACC_ADDRESS, 0x11, 0x00);   // 加速度计正常模式
    osal_msleep(10); // 延时10ms

    BMX055_SendREG(BMX055_GYR_ADDRESS, 0x0F, 0x00);   // 陀螺仪±2000°/s范围设置
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_GYR_ADDRESS, 0x10, 0x07);   // 输出数据速率和带宽配置
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_GYR_ADDRESS, 0x11, 0x00);   // 陀螺仪正常模式
    osal_msleep(10); // 延时10ms

    uint8_t data = BMX055_ReadREG(BMX055_MAG_ADDRESS, 0x4B); // 读取磁力计芯片ID寄存器
    osal_msleep(10); // 延时10ms
    if (data == 0) { // ID寄存器返回0表示未就绪
        BMX055_SendREG(BMX055_MAG_ADDRESS, 0x4B, 0x83); // 磁力计软复位命令
        osal_msleep(20); // 延时20ms
    }
    BMX055_SendREG(BMX055_MAG_ADDRESS, 0x4B, 0x01);   // 磁力计电源控制开启
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_MAG_ADDRESS, 0x4C, 0x38);   // 磁力计预设模式4配置
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_MAG_ADDRESS, 0x4E, 0x84);   // 设置XY轴数据速率和温度补偿
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_MAG_ADDRESS, 0x51, 0x04);   // 配置XY轴采样次数
    osal_msleep(10); // 延时10ms
    BMX055_SendREG(BMX055_MAG_ADDRESS, 0x52, 0x0F);   // 配置Z轴采样次数
    osal_msleep(10); // 延时10ms
}

void BMX055_ReadAccel(float *ax, float *ay, float *az){
    uint8_t accData[6] = {0}; 
    BMX055_ReadBytes(BMX055_ACC_ADDRESS, BMX055_ACC_REGISTER_ADDRESS, accData, 6); // 读取6字节加速度原始数据

    for (int i = 0; i < 3; i++) { // 连续读3次
        int raw = 0;
        uint8_t lsb = accData[2*i];
        uint8_t msb = accData[2*i + 1];
        raw = ((msb << 4) | ((lsb & 0xF0) >> 4)); // 左移4位拼接12位原始值
        if (raw > 2047) { // 2047判断12位两补负值
            raw -= 4096;  // 4096转换为有符号值
        }
        float val = raw * 0.0098f; // 1LSB≈0.0098g，转换为m/s²
        switch (i) {
            case 0: *ax = val; break;
            case 1: *ay = val; break;
            case 2: *az = val; break; // 2代表读取az
            default: break;
        }
    }
}

void BMX055_ReadGyro(float *gx, float *gy, float *gz){
    uint8_t gyrData[6] = {0};
    BMX055_ReadBytes(BMX055_GYR_ADDRESS, BMX055_GYR_REGISTER_ADDRESS, gyrData, 6); // 读取6字节陀螺仪原始数据

    for (int i = 0; i < 3; i++) { // 连续读3次
        int raw = 0;
        uint8_t lsb = gyrData[2*i];
        uint8_t msb = gyrData[2*i + 1];
        raw = (msb << 8) | lsb; // 8拼接16位原始值
        if (raw > 32767) { // 32767判断16位两补负值
            raw -= 65536;  // 65536转换为有符号值
        }
        float val = raw * (0.0038f * 16) * 3.1415926f / 180; // 1LSB≈0.0038°/s*16，转换为rad/s
        switch (i) {
            case 0: *gx = val; break;
            case 1: *gy = val; break;
            case 2: *gz = val; break; // 2代表读取gz值
            default: break;    
        }
    }
}

void BMX055_ReadMag(float *mx, float *my, float *mz){
    uint8_t magData[8] = {0};
    BMX055_ReadBytes(BMX055_MAG_ADDRESS, BMX055_MAG_REGISTER_ADDRESS, magData, 8); // 读取8字节磁力计原始数据
    // X轴数据
    int rawX = ((magData[1] << 5) | ((magData[0] & 0xF8) >> 3)); // 左移5位拼接13位原始值
    if (rawX > 4095) rawX -= 8192; // 4095两补转换8192
    *mx = (float)rawX * 0.3f;      // 1LSB≈0.3µT

    // Y轴数据
    int rawY = ((magData[3] << 5) | ((magData[2] & 0xF8) >> 3)); // 拼接13位原始值
    if (rawY > 4095) rawY -= 8192; // 4095两补转换8192
    *my = (float)rawY * 0.3f;      // 1LSB≈0.3µT

    // Z轴数据
    int rawZ = ((magData[5] << 7) | ((magData[4] & 0xFE) >> 1)); // 拼接15位原始值
    if (rawZ > 16383) rawZ -= 32768; // 16383两补转换32768
    *mz = (float)rawZ * 0.3f;       // 1LSB≈0.3µT
}

void BMX055_GetAllData(float *ax, float *ay, float *az,
                       float *gx, float *gy, float *gz,
                       float *mx, float *my, float *mz)
{
    BMX055_ReadAccel(ax, ay, az);
    BMX055_ReadGyro(gx, gy, gz);
    BMX055_ReadMag(mx, my, mz);
}
