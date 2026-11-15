#include "app_init.h"
#include "systick.h"
#include "los_memory.h"
#include "securec.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "gpio.h"
#include "pinctrl.h"
#include "i2c.h"
#include "I2C_SHT30.h"
#include "tcxo.h"
#include "osal_addr.h"


#define CONFIG_I2C_SCL_MASTER_PIN 16
#define CONFIG_I2C_SDA_MASTER_PIN 15
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x01
#define I2C_SET_BANDRATE 400000
#define TEST_MAX_TIMES             5
#define DALAY_MS                   100
#define TEST_TCXO_DELAY_1000MS     1000

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

void SHT30_task(void)
{
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    SHT30_Data read_data;
    app_i2c_init_pin();// 初始化I2C引脚
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);// 初始化I2C总线1
    if (ret != 0)
    {
        osal_printk("i2c init failed, ret = %0x\r\n", ret);
    }
    Init_SHT30();//初始化SHT30传感器
    while (1)
    {
        SHT30_Read_Data(&read_data);
        static char env_data[100] = {0};
        sprintf(env_data, "temp: %.2f, humi: %.2f%%RH\r\n", read_data.Temperature, read_data.Humidity);
        osal_printk("%s", env_data);
        osal_msleep(200);
    }
}

void task_entry(void)
{
    // 定义任务句柄
    osal_task *SHT30_task_handle = NULL;
    osal_kthread_lock();
    // 创建线程
    SHT30_task_handle = osal_kthread_create((osal_kthread_handler)SHT30_task, 0, "SHT30_task", 0x2000);
    if (SHT30_task_handle != NULL)
    {
        osal_kthread_set_priority(SHT30_task_handle, 24);
        osal_kfree(SHT30_task_handle);
    }
    osal_kthread_unlock();
}

app_run(task_entry);
