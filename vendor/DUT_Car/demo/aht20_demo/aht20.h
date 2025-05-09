/**
*    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech
*/

#ifndef AHT20_H
#define AHT20_H

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"

#define AHT20_ADDR 0x70

void aht20_init(void);
void aht20_GetData(float *temp, float *humi);
void i2c_test(void);

#endif

