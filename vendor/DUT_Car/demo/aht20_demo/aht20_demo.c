/**
*    Copyright (c) 2025/4/28  KangBohao@OurEDA， Dalian Univ of Tech
*/

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#include "i2c.h"
#include "aht20.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"

#define SENSOR_TASK_STACK_SIZE    0x2000      // 传感器任务栈大小(8KB)
#define SENSOR_TASK_PRIO          20          // 传感器任务优先级

#define CONFIG_I2C_SCL_MASTER_PIN 15          // I2C SCL 引脚
#define CONFIG_I2C_SDA_MASTER_PIN 16          // I2C SDA 引脚
#define CONFIG_I2C_MASTER_PIN_MODE 2          // I2C 引脚复用模式
#define I2C_MASTER_ADDR           0x0         // I2C 总线主机地址
#define I2C_SET_BANDRATE          100000      // I2C 波特率(100kHz)


static void *aht20_test(const char *arg){
    unused(arg);
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE); // 设置SCL引脚复用模式
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE); // 设置SDA引脚复用模式

    uint32_t baudrate = I2C_SET_BANDRATE;  // I2C波特率
    uint32_t hscode = I2C_MASTER_ADDR;     // I2C主机地址
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode); // 初始化I2C总线1
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }

    ssd1306_Init();                       // 初始化OLED
    ssd1306_Fill(Black);                  // 全屏填充黑色
    ssd1306_SetCursor(0, 0);              // 设置光标到(0,0)

    aht20_init();                         // 初始化温湿度传感器

    while (1) {
        // Serial print
        printf("========SENSOR========\r\n");
        float humis = 0, temps = 0;
        aht20_GetData(&temps, &humis);    // 读取温湿度
        printf("AHT20 :temp : %d humi : %d%%\r\n", (uint16_t)temps, (uint16_t)humis);
        printf("=========END=========\r\n");

        // OLED print
        ssd1306_ClearOLED();             // 清屏
        ssd1306_printf("%02dC %02d%%", (uint16_t)temps, (uint16_t)humis); // 显示温度/湿度
        ssd1306_UpdateScreen();          // 刷新屏幕

        osal_msleep(1000);               // 1000延时1s
    }
    return NULL;
}

static void test_entry(void)
{
    uint32_t ret;
    osal_task *taskid_sensor;
    // 创建任务调度锁
    osal_kthread_lock();
    // 创建传感器测试任务
    taskid_sensor = osal_kthread_create((osal_kthread_handler)aht20_test, NULL, "aht20_test", SENSOR_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid_sensor, SENSOR_TASK_PRIO); // 设置传感器任务优先级
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock(); // 解锁任务调度
}

/* Run the blinky_entry. */
app_run(test_entry);
