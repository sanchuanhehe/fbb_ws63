/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: I2C Sample Source. \n
 *
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "i2c.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BAUDRATE 100000
#define I2C_TASK_DURATION_MS 500
#define I2C_TRANSFER_LEN 100
#define AT24C02_MAX_ADDR 0x100          // AT24C64 最多存储256字节
#define AT24C02_PAGE_SIZE 8             // AT24C64 每页大小为8字节
#define AT24C02_WRITE_CYCLE_DELAY_MS 10 /* 写入周期完成等待时间 (最大5ms，取10ms保证可靠性) */
#define AT24C02_DATE_LEN 40             /* 测试写入读取的数据长度 */

#define I2C_TASK_PRIO 24
#define I2C_TASK_STACK_SIZE 0x1000

i2c_data_t data = {0};
/**
 * @brief 从 AT24C64 EEPROM 读取数据
 * @param addr 要读取的地址 (16位)
 * @param buffer 用于存储读取数据的缓冲区
 * @param len 要读取的数据长度
 * @return 成功返回0，失败返回非0
 */
errcode_t AT24C02_Read(uint8_t addr, uint8_t *buffer, uint16_t len)
{
    uint8_t addr_buffer[2];

    if (addr > AT24C02_MAX_ADDR) {
        return ERRCODE_I2C_ADDRESS_INVLID;
    }

    if (len > AT24C02_MAX_ADDR - addr) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }

    addr_buffer[0] = (uint8_t)(addr & 0xFF); // 地址字节

    data.send_buf = addr_buffer;
    data.send_len = 1;
    data.receive_buf = buffer;
    data.receive_len = len;
    // 首先发送要读取的地址
    int ret = uapi_i2c_master_write(CONFIG_I2C_AT24C02_BUS_ID, CONFIG_I2C_AT24C02_ADDR, &data);
    if (ret != 0) {
        return ret; // 如果写入地址失败，直接返回错误
    }
    osal_msleep(AT24C02_WRITE_CYCLE_DELAY_MS);
    // 然后读取数据
    return uapi_i2c_master_read(CONFIG_I2C_AT24C02_BUS_ID, CONFIG_I2C_AT24C02_ADDR, &data);
}

/**
 * @brief 写入数据到 AT24C64 EEPROM
 * @param addr 要写入的地址 (16位)
 * @param buffer 要写入的数据缓冲区
 * @param len 要写入的数据长度
 * @return 成功返回0，失败返回非0
 */
errcode_t AT24C02_Write(uint8_t addr, uint8_t *buffer, uint16_t len)
{
    uint16_t remaining_len = len;                // 剩余需要写入的字节数
    uint16_t current_addr = addr;                // 当前写入地址
    uint16_t page_offset;                        // 当前页的偏移量
    uint16_t write_len;                          // 当前写入的字节数
    uint8_t write_buffer[AT24C02_PAGE_SIZE + 1]; // 缓冲区大小为页大小 + 1字节的地址

    if (addr > AT24C02_MAX_ADDR) {
        return ERRCODE_I2C_ADDRESS_INVLID;
    }

    if (len > AT24C02_MAX_ADDR - addr) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    while (remaining_len > 0) {
        // 计算当前页的偏移量
        page_offset = current_addr % AT24C02_PAGE_SIZE;
        // 计算当前可以写入的字节数
        write_len = AT24C02_PAGE_SIZE - page_offset;
        if (write_len > remaining_len) {
            write_len = remaining_len;
        }
        write_buffer[0] = (uint8_t)(current_addr & 0xFF); // 地址字节

        // 将数据复制到缓冲区中
        for (uint16_t i = 0; i < write_len; i++) {
            write_buffer[i + 1] = buffer[len - remaining_len + i];
        }
        data.send_buf = write_buffer;
        data.send_len = write_len + 1;
        // 调用 I2C 写入函数
        if (uapi_i2c_master_write(CONFIG_I2C_AT24C02_BUS_ID, CONFIG_I2C_AT24C02_ADDR, &data) != ERRCODE_SUCC) {
            return ERRCODE_FAIL; // 写入失败
        }

        // 更新剩余字节数和当前地址
        remaining_len -= write_len;
        current_addr += write_len;

        // 延时等待写入完成（根据AT24C64的写入周期要求）
        // 例如：延时 10ms
        osal_msleep(AT24C02_WRITE_CYCLE_DELAY_MS);
    }
    return ERRCODE_SUCC; // 写入成功
}
static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_AT24C02_PIN, CONFIG_I2C_AT24C02_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_AT24C02_PIN, CONFIG_I2C_AT24C02_PIN_MODE);
}

static void *i2c_at24c02_task(const char *arg)
{
    unused(arg);
    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
    uint8_t tx_buff[AT24C02_DATE_LEN] = {0};
    uint8_t rx_buff[AT24C02_DATE_LEN] = {0};

    /* I2C master init config. */
    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_AT24C02_BUS_ID, baudrate, hscode);

    /* I2C data config. */
    for (uint32_t loop = 0; loop < AT24C02_DATE_LEN; loop++) {
        tx_buff[loop] = (loop & 0xFF);
    }
    osal_msleep(I2C_TASK_DURATION_MS);

    while (1) {
        // 判断是否正常工作

        if (AT24C02_Write(0x00, tx_buff, AT24C02_DATE_LEN) == ERRCODE_SUCC) {
            osal_printk("i2c%d at24c02 send succ!\r\n", CONFIG_I2C_AT24C02_BUS_ID);
        } else {
            osal_printk("No at24c02 detected\r\n");
            osal_msleep(I2C_TASK_DURATION_MS);
            continue;
        }
        osal_msleep(I2C_TASK_DURATION_MS);
        osal_printk("i2c%d at24c02 receive start!\r\n at24c02 receive data:", CONFIG_I2C_AT24C02_BUS_ID);
        if (AT24C02_Read(0x00, rx_buff, AT24C02_DATE_LEN) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < AT24C02_DATE_LEN; i++) {
                osal_printk(" %x", rx_buff[i]);
            }
            osal_printk("\r\ni2c%d at24c02 receive succ!\r\n", CONFIG_I2C_AT24C02_BUS_ID);
        }
        osal_msleep(I2C_TASK_DURATION_MS * 10);
    }
    return NULL;
}

static void i2c_at24c02_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2c_at24c02_task, 0, "I2cAt24c02Task", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the i2c_master_entry. */
app_run(i2c_at24c02_entry);