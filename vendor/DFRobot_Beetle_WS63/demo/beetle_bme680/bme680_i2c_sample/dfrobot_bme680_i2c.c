/**
 * @file dfrobot_bme680_i2c.c
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#include "dfrobot_bme680_i2c.h"

#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BAUDRATE 400000
#define I2C_MASTER_PIN_MODE 2

uint8_t g_iic_bus_id;

static int8_t bme680_i2c_read(uint8_t dev_id, uint8_t reg, uint8_t *p_buf, uint16_t size)
{
    UNUSED(dev_id);

    if (p_buf == NULL) {
        osal_printk("p_buf ERROR!! : null pointer\n");
        return 0;
    }

    i2c_data_t data = {0};
    data.send_buf = &reg;
    data.send_len = 1; // 先发寄存器地址
    data.receive_buf = p_buf;
    data.receive_len = size; // 再收数据

    if (uapi_i2c_master_write(g_iic_bus_id, g_bme680_i2c_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C readReg send error!\n");
        return 0;
    }

    if (uapi_i2c_master_read(g_iic_bus_id, g_bme680_i2c_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C readReg recv error!\n");
        return 0;
    }

    return 0;
}

static int8_t bme680_i2c_write(uint8_t dev_id, uint8_t reg, uint8_t *p_buf, uint16_t size)
{
    UNUSED(dev_id);

    if (p_buf == NULL) {
        osal_printk("p_buf ERROR!! : null pointer\n");
        return 0;
    }

    // 拼接寄存器地址 + 数据
    uint8_t buf[1 + size];
    buf[0] = reg;
    memcpy(&buf[1], p_buf, size);

    i2c_data_t data = {0};
    data.send_buf = buf;
    data.send_len = 1 + size;
    data.receive_buf = NULL;
    data.receive_len = 0;

    if (uapi_i2c_master_write(g_iic_bus_id, g_bme680_i2c_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C writeReg error!\n");
    }

    return 0;
}

void dfrobot_bme680_i2c_init(uint8_t I2CAddr_,
                             uint8_t iic_scl_master_pin,
                             uint8_t iic_sda_master_pin,
                             uint8_t iic_bus_id)
{
    dfrobot_bme680(bme680_i2c_read, bme680_i2c_write, bme680_delay_ms, BME680_INTERFACE_I2C);
    g_bme680_i2c_addr = I2CAddr_;
    g_iic_bus_id = iic_bus_id;
    uapi_pin_set_mode(iic_scl_master_pin, I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(iic_sda_master_pin, I2C_MASTER_PIN_MODE);
    uapi_i2c_master_init(g_iic_bus_id, I2C_SET_BAUDRATE, I2C_MASTER_ADDR);
}
