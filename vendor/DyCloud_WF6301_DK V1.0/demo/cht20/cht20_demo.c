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



#define I2C_MASTER_ADDR                   0x0
#define I2C_SET_BAUDRATE                  100000
#define I2C_TASK_DURATION_MS              500
#define I2C_TRANSFER_LEN                  100

#define I2C_TASK_PRIO                     24
#define I2C_TASK_STACK_SIZE               0x1000


bool Parse_temperature_humidity(uint8_t *data, uint32_t receive_len, float *temperature, float *humidity) {
    // 检查数据长度是否足够
    if (receive_len < 6) {
        return false; // 数据长度不足，无法解析
    }

    // 检查状态字节的Bit[7]是否为0
    if ((data[0] & 0x80) != 0) {
        return false; // 转换未完成，无法解析
    }

    // 提取湿度的20个bit
    uint32_t humidity_data = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
    // 解析湿度
    *humidity = (float)(humidity_data*100 / 1048576.0f);

    // 提取温度的20个bit
    uint32_t temperature_data = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];
    // 解析温度
    *temperature = (float)(temperature_data * 200.0f / 1048576.0f - 50);

    return true; // 解析成功
}

static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_CHT20_PIN, CONFIG_I2C_CHT20_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_CHT20_PIN, CONFIG_I2C_CHT20_PIN_MODE);
}

static void *i2c_cht20_task(const char *arg)
{
    unused(arg);
    i2c_data_t data = { 0 };

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
    uint16_t dev_addr = CONFIG_I2C_CHT20_SLAVE_ADDR;
    float temperature = 0;
    float humidity = 0;

    /* I2C CHT20 init config. */
    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_CHT20_BUS_ID, baudrate, hscode);

    /* I2C data config. */
    uint8_t tx_buff[I2C_TRANSFER_LEN] = { 0x71,0 };
    uint8_t rx_buff[I2C_TRANSFER_LEN] = { 0 };
    data.send_buf = tx_buff;
    data.send_len = 1;
    data.receive_buf = rx_buff;
    data.receive_len = 2;

    while (1) {
        osal_msleep(I2C_TASK_DURATION_MS);
        if (uapi_i2c_master_write(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
        } 
        else{
            osal_printk("No CHT20 detected\r\n");
            osal_msleep(I2C_TASK_DURATION_MS);
            continue;                       //i2c发送失败
        }
        osal_msleep(20);   
        if (uapi_i2c_master_read(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
        }
        if ((data.receive_buf[0] &0x18) == 0x18)
        {
            tx_buff[0] = 0xAC;//发送开始测量指令
            tx_buff[1] = 0x33;
            tx_buff[2] = 0x00;
            data.send_buf = tx_buff;
            data.send_len = 3;
            data.receive_buf = rx_buff;
            data.receive_len = 6;

            if (uapi_i2c_master_write(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {

            } else {
                osal_printk("The CHT20 measurement fails to be written\r\n");
                continue;
            }
            osal_msleep(200);                           
            
            if (uapi_i2c_master_read(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &data) == ERRCODE_SUCC) {
                osal_printk("original data is ");
            for (uint32_t i = 0; i < data.receive_len; i++) {
                osal_printk("%02X ", data.receive_buf[i]);
            }
            osal_printk("\n");

            if(!Parse_temperature_humidity(data.receive_buf, data.receive_len,&temperature,&humidity)){
                osal_printk("Parsing failure\r\n");
                continue;    
            }
            osal_printk("temperature is %d.%02dC    "
                        "humidity is %d.%02d%%\n"
                                                ,(uint32_t)temperature,((uint32_t)(temperature*100))%100
                                                ,(uint32_t)humidity,((uint32_t)(humidity*100))%100);
            osal_msleep(1000);
            }
        }  
        else{
            for (uint32_t i = 0; i < 2; i++) {
                osal_printk("%02x ", data.receive_buf[i]);
            }
            tx_buff[0] = 0xAC;//发送开始测量指令
            tx_buff[1] = 0x33;
            tx_buff[2] = 0x00;
            data.send_buf = tx_buff;
            data.send_len = 3;
            data.receive_buf = rx_buff;
            data.receive_len = 6;
            uapi_i2c_master_write(CONFIG_I2C_CHT20_BUS_ID, dev_addr, &data);
            osal_msleep(1000);
        }  

    }

    return NULL;
}

static void i2c_cht20_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2c_cht20_task, 0, "I2cCht20Task", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the i2c_master_entry. */
app_run(i2c_cht20_entry);