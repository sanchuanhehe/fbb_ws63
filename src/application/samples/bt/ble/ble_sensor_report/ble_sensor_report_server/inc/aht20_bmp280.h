/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Declares the AHT20 and BMP280 sensor module interface.
 * @else
 * @brief 声明 AHT20 和 BMP280 传感器模块接口。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#ifndef AHT20_BMP280_H
#define AHT20_BMP280_H

#include <stdint.h>
#include "errcode.h"

/**
 * @if Eng
 * @brief Defines one compensated environmental sensor sample.
 * @else
 * @brief 定义一组完成补偿换算的环境传感器数据。
 * @endif
 */
typedef struct {
    int32_t temperature_tenths_celsius;
    uint32_t humidity_tenths_percent;
    uint32_t pressure_pa;
} aht20_bmp280_data_t;

/**
 * @if Eng
 * @brief Initializes I2C1 and the AHT20 and BMP280 sensors.
 * @else
 * @brief 初始化 I2C1、AHT20 和 BMP280 传感器。
 * @endif
 */
errcode_t aht20_bmp280_init(void);

/**
 * @if Eng
 * @brief Reads and converts one environmental sensor sample.
 * @else
 * @brief 读取并换算一组环境传感器数据。
 * @endif
 */
errcode_t aht20_bmp280_read(aht20_bmp280_data_t *sensor_data);

#endif /* AHT20_BMP280_H */
