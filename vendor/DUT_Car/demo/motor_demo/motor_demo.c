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
#include "motor_control.h"

#define MOTOR_TASK_STACK_SIZE    0x2000      // 任务栈大小(8KB)
#define MOTOR_TASK_PRIO          20          // 任务优先级

#define CONFIG_I2C_SCL_MASTER_PIN 15          // I2C SCL 引脚
#define CONFIG_I2C_SDA_MASTER_PIN 16          // I2C SDA 引脚
#define CONFIG_I2C_MASTER_PIN_MODE 2          // I2C 引脚复用模式
#define I2C_MASTER_ADDR           0x0         // I2C 总线主机地址
#define I2C_SET_BANDRATE          100000      // I2C 波特率(100kHz)

static void *motor_test(const char *arg){
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

    pwm_write(0x16);                      // PWM 0x16

    while (1) {
        left_wheel_set(500, 500, true);       // 左轮正转速度500
        right_wheel_set(500, 500, true);      // 右轮正转速度500
        osal_msleep(2000);                    // 2000延时2s
        left_wheel_set(500, 500, false);      // 左轮反转速度500
        right_wheel_set(500, 500, false);     // 右轮反转速度500
        osal_msleep(2000);                    // 2000延时2s
        left_wheel_set(0, 0, true);           // 停止左轮
        right_wheel_set(0, 0, true);          // 停止右轮
        osal_msleep(2000);                    // 2000ms延时
    }
    return NULL;
}

static void test_entry(void)
{
    uint32_t ret;
    osal_task *taskid_sensor;
    // 创建任务调度锁
    osal_kthread_lock();
    // 创建测试任务
    taskid_sensor = osal_kthread_create((osal_kthread_handler)motor_test, NULL, "motor_test", MOTOR_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid_sensor, MOTOR_TASK_PRIO); // 设置任务优先级
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock(); // 解锁任务调度
}

/* Run the blinky_entry. */
app_run(test_entry);
