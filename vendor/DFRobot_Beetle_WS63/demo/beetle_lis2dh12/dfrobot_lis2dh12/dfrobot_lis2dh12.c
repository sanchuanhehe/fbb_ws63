/*!
 * @file dfrobot_lis2dh12.c
 * @brief Define the basic structure of DFRobot_LIS2DH12
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/DFRobot_LIS
 */

#include "dfrobot_lis2dh12.h"

#define I2C_MASTER_ADDR LIS2DH12_I2C_MASTER_ADDR
#define I2C_SET_BAUDRATE LIS2DH12_I2C_SET_BAUDRATE
#define I2C_MASTER_PIN_MODE LIS2DH12_I2C_MASTER_PIN_MODE

uint8_t g_iic_bus_id;
uint8_t g_device_addr;
uint8_t g_mg_scale_vel = LIS2DH12_DEFAULT_MG_SCALE_VEL;
uint8_t g_reset = LIS2DH12_RESET_DEFAULT;

bool dfrobot_lis2dh12_init(uint8_t addr, uint8_t iic_scl_master_pin, uint8_t iic_sda_master_pin, uint8_t iic_bus_id)
{
    g_device_addr = addr;
    g_iic_bus_id = iic_bus_id;
    uapi_pin_set_mode(iic_scl_master_pin, I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(iic_sda_master_pin, I2C_MASTER_PIN_MODE);
    uapi_i2c_master_init(g_iic_bus_id, I2C_SET_BAUDRATE, I2C_MASTER_ADDR);

    uint8_t identifier = LIS2DH12_ARRAY_INDEX_0;
    bool ret = false;
    g_reset = LIS2DH12_RESET_SET;
    read_reg(REG_CARD_ID, &identifier, LIS2DH12_REG_ADDR_SIZE);
    DBG(identifier);
    if (identifier == LIS2DH12_CHIP_ID_VALID) {
        ret = true;
    } else if (identifier == LIS2DH12_CHIP_ID_INVALID_1 || identifier == LIS2DH12_CHIP_ID_INVALID_2) {
        DBG("Communication failure");
        ret = false;
    } else {
        DBG("the ic is not LIS2DH12");
        ret = false;
    }
    return ret;
}

void write_reg(uint8_t reg, const uint8_t *p_buf, size_t size)
{
    if (p_buf == NULL) {
        osal_printk("p_buf ERROR!! : null pointer\n");
        return;
    }

    // 拼接寄存器地址 + 数据
    uint8_t buf[LIS2DH12_REG_ADDR_SIZE + size];
    buf[LIS2DH12_ARRAY_INDEX_0] = reg;
    memcpy(&buf[LIS2DH12_ARRAY_INDEX_1], p_buf, size);

    i2c_data_t data = {0};
    data.send_buf = buf;
    data.send_len = LIS2DH12_REG_ADDR_SIZE + size;
    data.receive_buf = NULL;
    data.receive_len = LIS2DH12_ARRAY_INDEX_0;

    if (uapi_i2c_master_write(g_iic_bus_id, g_device_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C write_reg error!\n");
    }
}

uint8_t read_reg(uint8_t reg, uint8_t *p_buf, size_t size)
{
    if (p_buf == NULL) {
        osal_printk("p_buf ERROR!! : null pointer\n");
        return 0;
    }

    i2c_data_t data = {0};
    data.send_buf = &reg;
    data.send_len = LIS2DH12_REG_ADDR_SIZE; // 先发寄存器地址
    data.receive_buf = p_buf;
    data.receive_len = size; // 再收数据

    if (uapi_i2c_master_write(g_iic_bus_id, g_device_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C read_reg send error!\n");
        return LIS2DH12_ARRAY_INDEX_0;
    }

    if (uapi_i2c_master_read(g_iic_bus_id, g_device_addr, &data) != ERRCODE_SUCC) {
        osal_printk("I2C read_reg recv error!\n");
        return LIS2DH12_ARRAY_INDEX_0;
    }

    return size;
}

/**
 * @fn limit_acceleration_data
 * @brief Limit acceleration data based on measurement range
 * @param data Acceleration data to be limited
 * @return Limited acceleration data
 */
static int32_t limit_acceleration_data(int32_t data)
{
    // Set appropriate limit values based on measurement range
    if (g_mg_scale_vel == LIS2DH12_MG_SCALE_2G) { // 2g range
        if (data > LIS2DH12_ACC_LIMIT_2G_POS) {
            data = LIS2DH12_ACC_LIMIT_2G_POS;
        } else if (data < LIS2DH12_ACC_LIMIT_2G_NEG) {
            data = LIS2DH12_ACC_LIMIT_2G_NEG;
        }
    } else if (g_mg_scale_vel == LIS2DH12_MG_SCALE_4G) { // 4g range
        if (data > LIS2DH12_ACC_LIMIT_4G_POS) {
            data = LIS2DH12_ACC_LIMIT_4G_POS;
        } else if (data < LIS2DH12_ACC_LIMIT_4G_NEG) {
            data = LIS2DH12_ACC_LIMIT_4G_NEG;
        }
    } else if (g_mg_scale_vel == LIS2DH12_MG_SCALE_8G) { // 8g range
        if (data > LIS2DH12_ACC_LIMIT_8G_POS) {
            data = LIS2DH12_ACC_LIMIT_8G_POS;
        } else if (data < LIS2DH12_ACC_LIMIT_8G_NEG) {
            data = LIS2DH12_ACC_LIMIT_8G_NEG;
        }
    } else if (g_mg_scale_vel == LIS2DH12_MG_SCALE_16G) { // 16g range
        if (data > LIS2DH12_ACC_LIMIT_16G_POS) {
            data = LIS2DH12_ACC_LIMIT_16G_POS;
        } else if (data < LIS2DH12_ACC_LIMIT_16G_NEG) {
            data = LIS2DH12_ACC_LIMIT_16G_NEG;
        }
    }
    return data;
}

int32_t read_acc_x(void)
{
    uint8_t sensor_data[LIS2DH12_SENSOR_DATA_SIZE];
    int32_t data;
    read_reg(REG_OUT_X_L | LIS2DH12_REG_READ_MASK, sensor_data, LIS2DH12_SENSOR_DATA_SIZE);
    data = ((int8_t)sensor_data[LIS2DH12_ARRAY_INDEX_1] * (uint8_t)g_mg_scale_vel);

    return limit_acceleration_data(data);
}

int32_t read_acc_y(void)
{
    uint8_t sensor_data[LIS2DH12_SENSOR_DATA_SIZE];
    int32_t a;
    read_reg(REG_OUT_Y_L | LIS2DH12_REG_READ_MASK, sensor_data, LIS2DH12_SENSOR_DATA_SIZE);
    a = ((int8_t)sensor_data[LIS2DH12_ARRAY_INDEX_1] * (uint8_t)g_mg_scale_vel);

    return limit_acceleration_data(a);
}

int32_t read_acc_z(void)
{
    uint8_t sensor_data[LIS2DH12_SENSOR_DATA_SIZE];
    int32_t a;
    read_reg(REG_OUT_Z_L | LIS2DH12_REG_READ_MASK, sensor_data, LIS2DH12_SENSOR_DATA_SIZE);
    DBG(sensor_data[LIS2DH12_ARRAY_INDEX_0]);
    DBG(sensor_data[LIS2DH12_ARRAY_INDEX_1]);
    a = ((int8_t)sensor_data[LIS2DH12_ARRAY_INDEX_1] * (uint8_t)g_mg_scale_vel);
    return limit_acceleration_data(a);
}

void set_range(e_range_t range)
{
    switch (range) {
        case E_LIS2DH12_2G:
            g_mg_scale_vel = LIS2DH12_MG_SCALE_2G;
            break;
        case E_LIS2DH12_4G:
            g_mg_scale_vel = LIS2DH12_MG_SCALE_4G;
            break;
        case E_LIS2DH12_8G:
            g_mg_scale_vel = LIS2DH12_MG_SCALE_8G;
            break;
        default:
            g_mg_scale_vel = LIS2DH12_MG_SCALE_16G;
            break;
    }
    DBG(range);
    write_reg(REG_CTRL_REG4, &range, LIS2DH12_REG_ADDR_SIZE);
}

void set_acquire_rate(e_power_mode_t rate)
{
    uint8_t reg = LIS2DH12_CTRL_REG1_DEFAULT;
    reg = reg | rate;
    DBG(reg);
    write_reg(REG_CTRL_REG1, &reg, LIS2DH12_REG_ADDR_SIZE);
}

uint8_t get_id(void)
{
    uint8_t identifier;
    read_reg(REG_CARD_ID, &identifier, LIS2DH12_REG_ADDR_SIZE);
    return identifier;
}

void set_int1_th(uint8_t threshold)
{
    uint8_t reg = (threshold * LIS2DH12_THRESHOLD_MULTIPLIER) / g_mg_scale_vel;
    uint8_t reg1 = LIS2DH12_CTRL_REG5_INT1_DEFAULT;
    uint8_t reg2 = LIS2DH12_CTRL_REG2_DEFAULT;
    uint8_t data = LIS2DH12_CTRL_REG3_INT1_DEFAULT;

    write_reg(REG_CTRL_REG2, &reg2, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG3, &data, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG5, &reg1, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG6, &reg2, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_INT1_THS, &reg, LIS2DH12_REG_ADDR_SIZE);
    read_reg(REG_CTRL_REG5, &reg2, LIS2DH12_REG_ADDR_SIZE);
    DBG(reg2);
    read_reg(REG_CTRL_REG3, &reg2, LIS2DH12_REG_ADDR_SIZE);
    DBG(reg2);
}

void set_int2_th(uint8_t threshold)
{
    uint8_t reg = (threshold * LIS2DH12_THRESHOLD_MULTIPLIER) / g_mg_scale_vel;
    uint8_t reg1 = LIS2DH12_CTRL_REG5_INT2_DEFAULT;
    uint8_t reg2 = LIS2DH12_CTRL_REG2_DEFAULT;
    uint8_t data = LIS2DH12_CTRL_REG6_INT2_DEFAULT;

    write_reg(REG_CTRL_REG2, &reg2, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG3, &reg2, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG5, &reg1, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_CTRL_REG6, &data, LIS2DH12_REG_ADDR_SIZE);
    write_reg(REG_INT2_THS, &reg, LIS2DH12_REG_ADDR_SIZE);
    read_reg(REG_CTRL_REG5, &reg2, LIS2DH12_REG_ADDR_SIZE);
    DBG(reg2);
    read_reg(REG_CTRL_REG6, &reg2, LIS2DH12_REG_ADDR_SIZE);
    DBG(reg2);
}

void enable_interrupt_event(e_interrupt_source_t source, e_interrupt_event_t event)
{
    uint8_t data = LIS2DH12_ARRAY_INDEX_0;
    data = LIS2DH12_INT_CFG_BASE | event;
    DBG(data);
    if (source == E_INT1) {
        write_reg(REG_INT1_CFG, &data, LIS2DH12_REG_ADDR_SIZE);
    } else {
        write_reg(REG_INT2_CFG, &data, LIS2DH12_REG_ADDR_SIZE);
    }
    read_reg(REG_INT1_CFG, &data, LIS2DH12_REG_ADDR_SIZE);
    DBG(data);
}

bool get_int1_event(e_interrupt_event_t event)
{
    uint8_t data = LIS2DH12_ARRAY_INDEX_0;
    bool ret = false;
    read_reg(REG_INT1_SRC, &data, LIS2DH12_REG_ADDR_SIZE);
    DBG(data, HEX);
    if (data & LIS2DH12_INT_STATUS_IA_MASK) {
        switch (event) {
            case E_X_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_XL_MASK))
                    ret = true;
                break;
            case E_X_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_XH_MASK) == LIS2DH12_INT_EVENT_XH_MASK)
                    ret = true;
                break;
            case E_Y_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_YL_MASK))
                    ret = true;
                break;
            case E_Y_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_YH_MASK) == LIS2DH12_INT_EVENT_YH_MASK)
                    ret = true;
                break;
            case E_Z_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_ZL_MASK))
                    ret = true;
                break;
            case E_Z_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_ZH_MASK) == LIS2DH12_INT_EVENT_ZH_MASK)
                    ret = true;
                break;
            default:
                ret = false;
        }
    } else {
        ret = false;
    }
    return ret;
}

bool get_int2_event(e_interrupt_event_t event)
{
    uint8_t data = LIS2DH12_ARRAY_INDEX_0;
    bool ret = false;
    read_reg(REG_INT2_SRC, &data, LIS2DH12_REG_ADDR_SIZE);
    DBG(data, HEX);
    if (data & LIS2DH12_INT_STATUS_IA_MASK) {
        switch (event) {
            case E_X_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_XL_MASK))
                    ret = true;
                break;
            case E_X_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_XH_MASK) == LIS2DH12_INT_EVENT_XH_MASK)
                    ret = true;
                break;
            case E_Y_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_YL_MASK))
                    ret = true;
                break;
            case E_Y_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_YH_MASK) == LIS2DH12_INT_EVENT_YH_MASK)
                    ret = true;
                break;
            case E_Z_LOWER_THAN_TH:
                if (!(data & LIS2DH12_INT_EVENT_ZL_MASK))
                    ret = true;
                break;
            case E_Z_HIGHER_THAN_TH:
                if ((data & LIS2DH12_INT_EVENT_ZH_MASK) == LIS2DH12_INT_EVENT_ZH_MASK)
                    ret = true;
                break;
            default:
                ret = false;
        }
    } else {
        ret = false;
    }
    return ret;
}