/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 *
 * @if Eng
 * @brief Implements I2C acquisition and compensation for the AHT20 and BMP280 sensors.
 * @else
 * @brief 实现 AHT20 和 BMP280 传感器的 I2C 采集与补偿换算。
 * @endif
 *
 * History: \n
 * 2026-07-25, Create file. \n
 */

#include <stdbool.h>
#include <stdint.h>
#include "soc_osal.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "i2c.h"
#include "aht20_bmp280.h"

#define AHT20_BMP280_LOG "[aht20+bmp280]"

/* Shared I2C bus configuration. / 共用 I2C 总线配置。 */
#define SENSOR_I2C_BUS I2C_BUS_1
#define SENSOR_I2C_BAUDRATE 100000
#define SENSOR_I2C_HS_CODE 0
#define SENSOR_I2C_PIN_MODE PIN_MODE_2
#define SENSOR_I2C_SDA_PIN GPIO_15
#define SENSOR_I2C_SCL_PIN GPIO_16

/* AHT20 commands and conversion constants. / AHT20 命令与换算常量。 */
#define AHT20_I2C_ADDR 0x38
#define AHT20_STATUS_COMMAND 0x71
#define AHT20_TRIGGER_COMMAND 0xAC
#define AHT20_TRIGGER_PARAM_1 0x33
#define AHT20_TRIGGER_PARAM_2 0x00
#define AHT20_CALIBRATED_MASK 0x08
#define AHT20_BUSY_MASK 0x80
#define AHT20_POWER_ON_DELAY_MS 100
#define AHT20_TRIGGER_DELAY_MS 10
#define AHT20_MEASUREMENT_DELAY_MS 80
#define AHT20_RESPONSE_LEN 7
#define AHT20_DATA_LEN 6
#define AHT20_CRC_INIT 0xFF
#define AHT20_CRC_POLYNOMIAL 0x31
#define AHT20_CRC_HIGH_BIT 0x80
#define AHT20_CRC_BIT_COUNT 8
#define AHT20_RAW_FULL_SCALE (1UL << 20)
#define AHT20_RAW_ROUNDING (1UL << 19)
#define AHT20_HUMIDITY_TENTHS_SCALE 1000
#define AHT20_TEMPERATURE_TENTHS_SCALE 2000
#define AHT20_TEMPERATURE_TENTHS_OFFSET 500

/* BMP280 registers and compensation constants. / BMP280 寄存器与补偿常量。 */
#define BMP280_I2C_ADDR_LOW 0x76
#define BMP280_I2C_ADDR_HIGH 0x77
#define BMP280_CHIP_ID_REG 0xD0
#define BMP280_CHIP_ID 0x58
#define BMP280_CALIB_REG 0x88
#define BMP280_CALIB_LEN 24
#define BMP280_CONFIG_REG 0xF5
#define BMP280_CONFIG_VALUE 0xA0
#define BMP280_CTRL_MEAS_REG 0xF4
#define BMP280_CTRL_MEAS_VALUE 0x27
#define BMP280_DATA_REG 0xF7
#define BMP280_DATA_LEN 6
#define BMP280_STARTUP_DELAY_MS 10
#define BMP280_RAW_INVALID 0x80000
#define BMP280_PRESSURE_Q24_8_SCALE 256
#define BMP280_PRESSURE_Q24_8_ROUNDING 128
#define BMP280_PRESSURE_CALC_OFFSET 128000
#define BMP280_PRESSURE_ADC_FULL_SCALE 1048576
#define BMP280_PRESSURE_CALC_MULTIPLIER 3125

#define SENSOR_BYTE_SHIFT 8
#define BMP280_CALIB_T1_OFFSET 0
#define BMP280_CALIB_T2_OFFSET 2
#define BMP280_CALIB_T3_OFFSET 4
#define BMP280_CALIB_P1_OFFSET 6
#define BMP280_CALIB_P2_OFFSET 8
#define BMP280_CALIB_P3_OFFSET 10
#define BMP280_CALIB_P4_OFFSET 12
#define BMP280_CALIB_P5_OFFSET 14
#define BMP280_CALIB_P6_OFFSET 16
#define BMP280_CALIB_P7_OFFSET 18
#define BMP280_CALIB_P8_OFFSET 20
#define BMP280_CALIB_P9_OFFSET 22
#define AHT20_HUMIDITY_HIGH_OFFSET 1
#define AHT20_HUMIDITY_MIDDLE_OFFSET 2
#define AHT20_SHARED_DATA_OFFSET 3
#define AHT20_TEMPERATURE_MIDDLE_OFFSET 4
#define AHT20_TEMPERATURE_LOW_OFFSET 5
#define AHT20_RAW_HIGH_SHIFT 12
#define AHT20_RAW_MIDDLE_SHIFT 4
#define AHT20_TEMPERATURE_HIGH_SHIFT 16
#define AHT20_TEMPERATURE_HIGH_MASK 0x0F
#define BMP280_PRESSURE_HIGH_OFFSET 0
#define BMP280_PRESSURE_MIDDLE_OFFSET 1
#define BMP280_PRESSURE_LOW_OFFSET 2
#define BMP280_TEMPERATURE_HIGH_OFFSET 3
#define BMP280_TEMPERATURE_MIDDLE_OFFSET 4
#define BMP280_TEMPERATURE_LOW_OFFSET 5
#define BMP280_RAW_HIGH_SHIFT 12
#define BMP280_RAW_MIDDLE_SHIFT 4
#define BMP280_TEMP_ADC_VAR1_SHIFT 3
#define BMP280_TEMP_CALIB_T1_SHIFT 1
#define BMP280_TEMP_VAR1_RESULT_SHIFT 11
#define BMP280_TEMP_ADC_VAR2_SHIFT 4
#define BMP280_TEMP_VAR2_PRODUCT_SHIFT 12
#define BMP280_TEMP_VAR2_RESULT_SHIFT 14
#define BMP280_TEMP_RESULT_MULTIPLIER 5
#define BMP280_TEMP_RESULT_ROUNDING 128
#define BMP280_TEMP_RESULT_SHIFT 8
#define BMP280_PRESSURE_P5_SHIFT 17
#define BMP280_PRESSURE_P4_SHIFT 35
#define BMP280_PRESSURE_P3_SHIFT 8
#define BMP280_PRESSURE_P2_SHIFT 12
#define BMP280_PRESSURE_BASE_SHIFT 47
#define BMP280_PRESSURE_VAR1_SHIFT 33
#define BMP280_PRESSURE_NUMERATOR_SHIFT 31
#define BMP280_PRESSURE_P9_INPUT_SHIFT 13
#define BMP280_PRESSURE_P9_RESULT_SHIFT 25
#define BMP280_PRESSURE_P8_SHIFT 19
#define BMP280_PRESSURE_RESULT_SHIFT 8
#define BMP280_PRESSURE_P7_SHIFT 4

/**
 * @if Eng
 * @brief Stores factory calibration parameters read from the BMP280.
 * @else
 * @brief 保存从 BMP280 读取的出厂校准参数。
 * @endif
 */
typedef struct {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
} bmp280_calibration_t;

/* BMP280 runtime state. / BMP280 运行状态。 */
static bmp280_calibration_t g_bmp280_calibration;
static uint16_t g_bmp280_addr;
static int32_t g_bmp280_t_fine;

/**
 * @if Eng
 * @brief Writes a byte sequence to one sensor on the shared I2C bus.
 * @else
 * @brief 向共用 I2C 总线上的指定传感器写入字节序列。
 * @endif
 */
static errcode_t sensor_i2c_write(uint16_t addr, uint8_t *buffer, uint32_t length)
{
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = length;
    return uapi_i2c_master_write(SENSOR_I2C_BUS, addr, &data);
}

/**
 * @if Eng
 * @brief Reads a byte sequence from one sensor on the shared I2C bus.
 * @else
 * @brief 从共用 I2C 总线上的指定传感器读取字节序列。
 * @endif
 */
static errcode_t sensor_i2c_read(uint16_t addr, uint8_t *buffer, uint32_t length)
{
    i2c_data_t data = {0};
    data.receive_buf = buffer;
    data.receive_len = length;
    return uapi_i2c_master_read(SENSOR_I2C_BUS, addr, &data);
}

/**
 * @if Eng
 * @brief Writes a command or register address and reads the response.
 * @else
 * @brief 写入命令或寄存器地址后读取响应数据。
 * @endif
 */
static errcode_t sensor_i2c_read_command(uint16_t addr,
                                         uint8_t command,
                                         uint8_t *buffer,
                                         uint32_t length)
{
    i2c_data_t data = {0};
    data.send_buf = &command;
    data.send_len = sizeof(command);
    data.receive_buf = buffer;
    data.receive_len = length;
    return uapi_i2c_master_writeread(SENSOR_I2C_BUS, addr, &data);
}

/**
 * @if Eng
 * @brief Writes one value to a BMP280 register.
 * @else
 * @brief 向 BMP280 的指定寄存器写入一个数值。
 * @endif
 */
static errcode_t bmp280_write_register(uint8_t reg, uint8_t value)
{
    uint8_t command[] = {reg, value};
    return sensor_i2c_write(g_bmp280_addr, command, sizeof(command));
}

/**
 * @if Eng
 * @brief Decodes an unsigned 16-bit little-endian value.
 * @else
 * @brief 解析无符号 16 位小端数值。
 * @endif
 */
static uint16_t bmp280_read_u16_le(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << SENSOR_BYTE_SHIFT);
}

/**
 * @if Eng
 * @brief Decodes a signed 16-bit little-endian value.
 * @else
 * @brief 解析有符号 16 位小端数值。
 * @endif
 */
static int16_t bmp280_read_s16_le(const uint8_t *data)
{
    return (int16_t)bmp280_read_u16_le(data);
}

/**
 * @if Eng
 * @brief Loads BMP280 temperature and pressure calibration parameters.
 * @else
 * @brief 加载 BMP280 温度和气压校准参数。
 * @endif
 */
static void bmp280_load_calibration(const uint8_t *data)
{
    g_bmp280_calibration.dig_t1 = bmp280_read_u16_le(&data[BMP280_CALIB_T1_OFFSET]);
    g_bmp280_calibration.dig_t2 = bmp280_read_s16_le(&data[BMP280_CALIB_T2_OFFSET]);
    g_bmp280_calibration.dig_t3 = bmp280_read_s16_le(&data[BMP280_CALIB_T3_OFFSET]);
    g_bmp280_calibration.dig_p1 = bmp280_read_u16_le(&data[BMP280_CALIB_P1_OFFSET]);
    g_bmp280_calibration.dig_p2 = bmp280_read_s16_le(&data[BMP280_CALIB_P2_OFFSET]);
    g_bmp280_calibration.dig_p3 = bmp280_read_s16_le(&data[BMP280_CALIB_P3_OFFSET]);
    g_bmp280_calibration.dig_p4 = bmp280_read_s16_le(&data[BMP280_CALIB_P4_OFFSET]);
    g_bmp280_calibration.dig_p5 = bmp280_read_s16_le(&data[BMP280_CALIB_P5_OFFSET]);
    g_bmp280_calibration.dig_p6 = bmp280_read_s16_le(&data[BMP280_CALIB_P6_OFFSET]);
    g_bmp280_calibration.dig_p7 = bmp280_read_s16_le(&data[BMP280_CALIB_P7_OFFSET]);
    g_bmp280_calibration.dig_p8 = bmp280_read_s16_le(&data[BMP280_CALIB_P8_OFFSET]);
    g_bmp280_calibration.dig_p9 = bmp280_read_s16_le(&data[BMP280_CALIB_P9_OFFSET]);
}

/**
 * @if Eng
 * @brief Verifies that the AHT20 is calibrated and ready for measurement.
 * @else
 * @brief 检查 AHT20 已标定并可以执行测量。
 * @endif
 */
static errcode_t aht20_init(void)
{
    uint8_t status = 0;
    osal_msleep(AHT20_POWER_ON_DELAY_MS);
    errcode_t ret = sensor_i2c_read_command(AHT20_I2C_ADDR, AHT20_STATUS_COMMAND, &status, sizeof(status));
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s status read failed: 0x%x\r\n", AHT20_BMP280_LOG, ret);
        return ret;
    }
    if ((status & AHT20_CALIBRATED_MASK) == 0) {
        osal_printk("%s calibration status invalid: 0x%02x\r\n", AHT20_BMP280_LOG, status);
        return ERRCODE_FAIL;
    }
    osal_printk("%s AHT20 ready, status=0x%02x\r\n", AHT20_BMP280_LOG, status);
    return ERRCODE_SUCC;
}

/**
 * @if Eng
 * @brief Probes, validates, and configures the BMP280.
 * @else
 * @brief 探测、校验并配置 BMP280。
 * @endif
 */
static errcode_t bmp280_init(void)
{
    uint8_t chip_id = 0;
    uint8_t calibration_data[BMP280_CALIB_LEN] = {0};
    const uint16_t addresses[] = {BMP280_I2C_ADDR_LOW, BMP280_I2C_ADDR_HIGH};
    errcode_t ret = ERRCODE_FAIL;

    for (uint32_t index = 0; index < sizeof(addresses) / sizeof(addresses[0]); index++) {
        ret = sensor_i2c_read_command(addresses[index], BMP280_CHIP_ID_REG, &chip_id, sizeof(chip_id));
        if (ret == ERRCODE_SUCC && chip_id == BMP280_CHIP_ID) {
            g_bmp280_addr = addresses[index];
            break;
        }
    }
    if (g_bmp280_addr == 0) {
        osal_printk("%s BMP280 not found, last id=0x%02x ret=0x%x\r\n", AHT20_BMP280_LOG, chip_id, ret);
        return ERRCODE_FAIL;
    }

    ret = sensor_i2c_read_command(g_bmp280_addr, BMP280_CALIB_REG, calibration_data, sizeof(calibration_data));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    bmp280_load_calibration(calibration_data);
    if (g_bmp280_calibration.dig_p1 == 0) {
        return ERRCODE_FAIL;
    }
    ret = bmp280_write_register(BMP280_CONFIG_REG, BMP280_CONFIG_VALUE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = bmp280_write_register(BMP280_CTRL_MEAS_REG, BMP280_CTRL_MEAS_VALUE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    osal_msleep(BMP280_STARTUP_DELAY_MS);
    osal_printk("%s BMP280 ready, addr=0x%02x id=0x%02x\r\n", AHT20_BMP280_LOG, g_bmp280_addr, chip_id);
    return ERRCODE_SUCC;
}

/**
 * @if Eng
 * @brief Calculates the CRC-8 value defined by the AHT20 protocol.
 * @else
 * @brief 计算 AHT20 协议规定的 CRC-8 校验值。
 * @endif
 */
static uint8_t aht20_crc8(const uint8_t *data, uint32_t length)
{
    uint8_t crc = AHT20_CRC_INIT;
    for (uint32_t index = 0; index < length; index++) {
        crc ^= data[index];
        for (uint32_t bit = 0; bit < AHT20_CRC_BIT_COUNT; bit++) {
            crc = (crc & AHT20_CRC_HIGH_BIT) != 0 ?
                (uint8_t)((crc << 1) ^ AHT20_CRC_POLYNOMIAL) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

/**
 * @if Eng
 * @brief Triggers an AHT20 measurement and converts temperature and humidity.
 * @else
 * @brief 触发 AHT20 测量并换算温度和湿度。
 * @endif
 */
static errcode_t aht20_read(int32_t *temperature_tenths, uint32_t *humidity_tenths)
{
    uint8_t command[] = {AHT20_TRIGGER_COMMAND, AHT20_TRIGGER_PARAM_1, AHT20_TRIGGER_PARAM_2};
    uint8_t response[AHT20_RESPONSE_LEN] = {0};
    uint32_t raw_humidity;
    uint32_t raw_temperature;
    errcode_t ret;

    osal_msleep(AHT20_TRIGGER_DELAY_MS);
    ret = sensor_i2c_write(AHT20_I2C_ADDR, command, sizeof(command));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    osal_msleep(AHT20_MEASUREMENT_DELAY_MS);
    ret = sensor_i2c_read(AHT20_I2C_ADDR, response, sizeof(response));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if ((response[0] & AHT20_BUSY_MASK) != 0 || aht20_crc8(response, AHT20_DATA_LEN) != response[AHT20_DATA_LEN]) {
        return ERRCODE_FAIL;
    }

    raw_humidity = ((uint32_t)response[AHT20_HUMIDITY_HIGH_OFFSET] << AHT20_RAW_HIGH_SHIFT) |
                   ((uint32_t)response[AHT20_HUMIDITY_MIDDLE_OFFSET] << AHT20_RAW_MIDDLE_SHIFT) |
                   (response[AHT20_SHARED_DATA_OFFSET] >> AHT20_RAW_MIDDLE_SHIFT);
    raw_temperature = (((uint32_t)response[AHT20_SHARED_DATA_OFFSET] & AHT20_TEMPERATURE_HIGH_MASK) <<
                       AHT20_TEMPERATURE_HIGH_SHIFT) |
                      ((uint32_t)response[AHT20_TEMPERATURE_MIDDLE_OFFSET] << SENSOR_BYTE_SHIFT) |
                      response[AHT20_TEMPERATURE_LOW_OFFSET];
    *humidity_tenths = (uint32_t)(((uint64_t)raw_humidity * AHT20_HUMIDITY_TENTHS_SCALE +
                                   AHT20_RAW_ROUNDING) /
                                  AHT20_RAW_FULL_SCALE);
    *temperature_tenths = (int32_t)(((uint64_t)raw_temperature * AHT20_TEMPERATURE_TENTHS_SCALE +
                                     AHT20_RAW_ROUNDING) /
                                    AHT20_RAW_FULL_SCALE) - AHT20_TEMPERATURE_TENTHS_OFFSET;
    return ERRCODE_SUCC;
}

/**
 * @if Eng
 * @brief Compensates the BMP280 raw temperature and updates t_fine.
 * @else
 * @brief 补偿 BMP280 原始温度并更新 t_fine。
 * @endif
 */
static int32_t bmp280_compensate_temperature(int32_t adc_temperature)
{
    int32_t var1 = ((((adc_temperature >> BMP280_TEMP_ADC_VAR1_SHIFT) -
                     ((int32_t)g_bmp280_calibration.dig_t1 << BMP280_TEMP_CALIB_T1_SHIFT))) *
                    (int32_t)g_bmp280_calibration.dig_t2) >> BMP280_TEMP_VAR1_RESULT_SHIFT;
    int32_t var2 = (((((adc_temperature >> BMP280_TEMP_ADC_VAR2_SHIFT) -
                       (int32_t)g_bmp280_calibration.dig_t1) *
                      ((adc_temperature >> BMP280_TEMP_ADC_VAR2_SHIFT) -
                       (int32_t)g_bmp280_calibration.dig_t1)) >> BMP280_TEMP_VAR2_PRODUCT_SHIFT) *
                    (int32_t)g_bmp280_calibration.dig_t3) >> BMP280_TEMP_VAR2_RESULT_SHIFT;
    g_bmp280_t_fine = var1 + var2;
    return (g_bmp280_t_fine * BMP280_TEMP_RESULT_MULTIPLIER + BMP280_TEMP_RESULT_ROUNDING) >>
           BMP280_TEMP_RESULT_SHIFT;
}

/**
 * @if Eng
 * @brief Compensates BMP280 raw pressure using the 64-bit fixed-point formula.
 * @else
 * @brief 使用 64 位定点公式补偿 BMP280 原始气压。
 * @endif
 */
static uint32_t bmp280_compensate_pressure(int32_t adc_pressure)
{
    int64_t var1 = (int64_t)g_bmp280_t_fine - BMP280_PRESSURE_CALC_OFFSET;
    int64_t var2 = var1 * var1 * (int64_t)g_bmp280_calibration.dig_p6;
    int64_t pressure;

    var2 += (var1 * (int64_t)g_bmp280_calibration.dig_p5) << BMP280_PRESSURE_P5_SHIFT;
    var2 += (int64_t)g_bmp280_calibration.dig_p4 << BMP280_PRESSURE_P4_SHIFT;
    var1 = ((var1 * var1 * (int64_t)g_bmp280_calibration.dig_p3) >> BMP280_PRESSURE_P3_SHIFT) +
           ((var1 * (int64_t)g_bmp280_calibration.dig_p2) << BMP280_PRESSURE_P2_SHIFT);
    var1 = (((((int64_t)1 << BMP280_PRESSURE_BASE_SHIFT) + var1) *
             (int64_t)g_bmp280_calibration.dig_p1) >> BMP280_PRESSURE_VAR1_SHIFT);
    if (var1 == 0) {
        return 0;
    }
    pressure = BMP280_PRESSURE_ADC_FULL_SCALE - adc_pressure;
    pressure = (((pressure << BMP280_PRESSURE_NUMERATOR_SHIFT) - var2) *
                BMP280_PRESSURE_CALC_MULTIPLIER) / var1;
    var1 = ((int64_t)g_bmp280_calibration.dig_p9 * (pressure >> BMP280_PRESSURE_P9_INPUT_SHIFT) *
            (pressure >> BMP280_PRESSURE_P9_INPUT_SHIFT)) >> BMP280_PRESSURE_P9_RESULT_SHIFT;
    var2 = ((int64_t)g_bmp280_calibration.dig_p8 * pressure) >> BMP280_PRESSURE_P8_SHIFT;
    pressure = ((pressure + var1 + var2) >> BMP280_PRESSURE_RESULT_SHIFT) +
               ((int64_t)g_bmp280_calibration.dig_p7 << BMP280_PRESSURE_P7_SHIFT);
    return (uint32_t)pressure;
}

/**
 * @if Eng
 * @brief Reads and compensates one BMP280 pressure sample.
 * @else
 * @brief 读取并补偿一组 BMP280 气压数据。
 * @endif
 */
static errcode_t bmp280_read(uint32_t *pressure_pa)
{
    uint8_t response[BMP280_DATA_LEN] = {0};
    int32_t raw_pressure;
    int32_t raw_temperature;
    uint32_t pressure_q24_8;
    errcode_t ret = sensor_i2c_read_command(g_bmp280_addr, BMP280_DATA_REG, response, sizeof(response));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    raw_pressure = ((int32_t)response[BMP280_PRESSURE_HIGH_OFFSET] << BMP280_RAW_HIGH_SHIFT) |
                   ((int32_t)response[BMP280_PRESSURE_MIDDLE_OFFSET] << BMP280_RAW_MIDDLE_SHIFT) |
                   (response[BMP280_PRESSURE_LOW_OFFSET] >> BMP280_RAW_MIDDLE_SHIFT);
    raw_temperature = ((int32_t)response[BMP280_TEMPERATURE_HIGH_OFFSET] << BMP280_RAW_HIGH_SHIFT) |
                      ((int32_t)response[BMP280_TEMPERATURE_MIDDLE_OFFSET] << BMP280_RAW_MIDDLE_SHIFT) |
                      (response[BMP280_TEMPERATURE_LOW_OFFSET] >> BMP280_RAW_MIDDLE_SHIFT);
    if (raw_pressure == BMP280_RAW_INVALID || raw_temperature == BMP280_RAW_INVALID) {
        return ERRCODE_FAIL;
    }
    (void)bmp280_compensate_temperature(raw_temperature);
    pressure_q24_8 = bmp280_compensate_pressure(raw_pressure);
    if (pressure_q24_8 == 0) {
        return ERRCODE_FAIL;
    }
    *pressure_pa = (pressure_q24_8 + BMP280_PRESSURE_Q24_8_ROUNDING) / BMP280_PRESSURE_Q24_8_SCALE;
    return ERRCODE_SUCC;
}

/**
 * @if Eng
 * @brief Initializes I2C1 and the AHT20 and BMP280 sensors.
 * @else
 * @brief 初始化 I2C1、AHT20 和 BMP280 传感器。
 * @endif
 */
errcode_t aht20_bmp280_init(void)
{
    errcode_t ret = uapi_pin_set_mode(SENSOR_I2C_SCL_PIN, SENSOR_I2C_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_pin_set_mode(SENSOR_I2C_SDA_PIN, SENSOR_I2C_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_i2c_master_init(SENSOR_I2C_BUS, SENSOR_I2C_BAUDRATE, SENSOR_I2C_HS_CODE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s I2C1 init failed: 0x%x\r\n", AHT20_BMP280_LOG, ret);
        return ret;
    }
    ret = aht20_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return bmp280_init();
}

/**
 * @if Eng
 * @brief Reads and converts one environmental sensor sample.
 * @else
 * @brief 读取并换算一组环境传感器数据。
 * @endif
 */
errcode_t aht20_bmp280_read(aht20_bmp280_data_t *sensor_data)
{
    errcode_t ret;
    if (sensor_data == NULL) {
        return ERRCODE_FAIL;
    }
    ret = aht20_read(&sensor_data->temperature_tenths_celsius, &sensor_data->humidity_tenths_percent);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return bmp280_read(&sensor_data->pressure_pa);
}
