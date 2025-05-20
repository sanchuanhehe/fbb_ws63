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

#define I2C_MASTER_BUS_ID 1

void ins5699s_SendREG(uint8_t reg, uint8_t reg_data)
{
    uint8_t buffer[] = {reg, reg_data};
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer); // 发送两字节：寄存器地址 + 数据
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, INS5699S_ADDR >> 1, &data); // INS5699S 7位地址需右移1位
    if (ret != 0) {
        printf("INS5699S:I2cWriteREG(%02X) failed, %0X!\n", reg, ret); // 打印失败的寄存器地址和错误码
        return;
    }
}

uint8_t ins5699s_ReadREG(uint8_t reg)
{
    uint8_t send_buffer[] = {reg};
    uint8_t read_buffer[1] = {0}; // 接收1字节寄存器值
    i2c_data_t data = {0};
    data.send_buf = send_buffer;
    data.send_len = sizeof(send_buffer); // 发送长度1字节
    data.receive_buf = read_buffer;
    data.receive_len = 1; // 接收长度1字节
    errcode_t ret =
        uapi_i2c_master_writeread(I2C_MASTER_BUS_ID, INS5699S_ADDR >> 1, &data); // INS5699S 7位地址需右移1位
    if (ret != 0) {
        printf("INS5699S:I2cReadREG(%02X) failed, %0X!\n", reg, ret); // 打印失败的寄存器地址和错误码
        return 0;
    }

    return data.receive_buf[0]; // 返回寄存器值
}

ins5699s_time ins5699s_GetTime(void)
{
    ins5699s_time time;
    uint8_t t_reg;
    t_reg = ins5699s_ReadREG(INS5699S_REG_SEC);
    time.sec = (((t_reg & 0x70) >> 4) * 10) + (t_reg & 0x0F); // ((t_reg & 0x70) >> 4) * 10) + (t_reg & 0x0F)BCD格式解析秒（高4位*10 + 低4位）
    t_reg = ins5699s_ReadREG(INS5699S_REG_MIN);
    time.min = (((t_reg & 0x70) >> 4) * 10) + (t_reg & 0x0F); // (t_reg & 0x70) >> 4) * 10) + (t_reg & 0x0F)BCD格式解析分
    t_reg = ins5699s_ReadREG(INS5699S_REG_HOUR);
    time.hour = (((t_reg & 0x30) >> 4) * 10) + (t_reg & 0x0F); // (((t_reg & 0x30) >> 4) * 10) + (t_reg & 0x0F)BCD格式解析时
    t_reg = ins5699s_ReadREG(INS5699S_REG_WEEK);
    time.week = ((t_reg >> 1) & 1) * 1    // 位1->周一
                + ((t_reg >> 2) & 1) * 2  // 位2->周二
                + ((t_reg >> 3) & 1) * 3  // 位3->周三
                + ((t_reg >> 4) & 1) * 4  // 位4->周四
                + ((t_reg >> 5) & 1) * 5  // 位5->周五
                + ((t_reg >> 6) & 1) * 6; // 位6->周六
    t_reg = ins5699s_ReadREG(INS5699S_REG_DAY);
    time.day = (((t_reg & 0x30) >> 4) * 10) + (t_reg & 0x0F); // (((t_reg & 0x30) >> 4) * 10) + (t_reg & 0x0F)代表BCD格式解析日
    t_reg = ins5699s_ReadREG(INS5699S_REG_MONTH);
    time.month = (((t_reg & 0x10) >> 4) * 10) + (t_reg & 0x0F); // (((t_reg & 0x10) >> 4) * 10) + (t_reg & 0x0F)代表BCD格式解析月
    t_reg = ins5699s_ReadREG(INS5699S_REG_YEAR);
    time.year = ((t_reg >> 4) * 10) + (t_reg & 0x0F); // ((t_reg >> 4) * 10) + (t_reg & 0x0F)BCD格式解析年

    return time;
}

void ins5699s_SetTime(ins5699s_time time)
{
    uint8_t ctrl_reg = ins5699s_ReadREG(INS5699S_REG_CONTROL);
    ctrl_reg |= 0x01; // 置位写使能位(bit0)以允许写入寄存器
    ins5699s_SendREG(INS5699S_REG_CONTROL, ctrl_reg);

    uint8_t week_reg;
    if (time.week > 6) { // 6代表无效周数时清零
        week_reg = 0x00;
    } else {
        week_reg = 1 << time.week; // 根据周几生成位掩码
    }

    ins5699s_SendREG(INS5699S_REG_SEC, ((time.sec / 10) << 4) | (time.sec % 10));    // ((time.sec / 10) << 4) | (time.sec % 10)BCD编码秒
    ins5699s_SendREG(INS5699S_REG_MIN, ((time.min / 10) << 4) | (time.min % 10));    // ((time.min / 10) << 4) | (time.min % 10))BCD编码分
    ins5699s_SendREG(INS5699S_REG_HOUR, ((time.hour / 10) << 4) | (time.hour % 10)); // ((time.hour / 10) << 4) | (time.hour % 10))BCD编码时
    ins5699s_SendREG(INS5699S_REG_WEEK, week_reg);
    ins5699s_SendREG(INS5699S_REG_DAY, ((time.day / 10) << 4) | (time.day % 10));       // ((time.day / 10) << 4) | (time.day % 10))BCD编码日
    ins5699s_SendREG(INS5699S_REG_MONTH, ((time.month / 10) << 4) | (time.month % 10)); // ((time.month / 10) << 4) | (time.month % 10))BCD编码月
    ins5699s_SendREG(INS5699S_REG_YEAR, ((time.year / 10) << 4) | (time.year % 10));    // ((time.year / 10) << 4) | (time.year % 10))BCD编码年

    ctrl_reg &= ~0x01; // 清除写使能位(bit0)结束写入
    ins5699s_SendREG(INS5699S_REG_CONTROL, ctrl_reg);
}

void ins5699s_init(void)
{

    osal_msleep(100); // 延时100ms以等待设备稳定
    osal_printk("INS5699S Init SUCC!\r\n");
}
