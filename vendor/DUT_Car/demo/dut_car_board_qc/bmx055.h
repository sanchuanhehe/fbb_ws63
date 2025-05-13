/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#ifndef BMX055_H
#define BMX055_H

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"

#define BMX055_ACC_ADDRESS  0x18
#define BMX055_GYR_ADDRESS  0x68
#define BMX055_MAG_ADDRESS  0x10

#define BMX055_ACC_REGISTER_ADDRESS  0x02
#define BMX055_GYR_REGISTER_ADDRESS  0x02
#define BMX055_MAG_REGISTER_ADDRESS  0x42


void bmx055_init(void);
void BMX055_ReadAccel(float *ax, float *ay, float *az);
void BMX055_ReadGyro(float *gx, float *gy, float *gz);
void BMX055_ReadMag(float *mx, float *my, float *mz);
void BMX055_GetAllData(float *ax, float *ay, float *az,
                       float *gx, float *gy, float *gz,
                       float *mx, float *my, float *mz);

#endif
