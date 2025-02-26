/**
 * Copyright (c) dt-sir
 *
 * Description: Blinky with PWM Sample Source. \n
 *              This file implements a LED blinking example using function of PWM. \n
 *              Using PWM to control the average current size by periodically and quickly adjusting the duty cycle of the LED power-on time. \n
 *              So as to form a brightness gradient effect under the temporary effect of human vision. \n
 *
 * History: \n
 * 2025-02-19, Create file. \n
 */

#include "cmsis_os2.h"           // 引入CMSIS RTOS2库，用于多线程处理
#include "app_init.h"            // 引入应用初始化的头文件
#include "pinctrl.h"             // 引入引脚控制头文件
#include "tcxo.h"                // 引入TCXO相关头文件
#include "pwm.h"                 // 引入PWM控制头文件

#define LIGHT_TASK_STACK_SIZE 0x1000        // 定义光效任务的栈大小
#define LIGHT_TASK_PRIO (osPriority_t)(17)  // 定义光效任务的优先级
#define BREATH_MAX_BRIGHTNESS 300           // 定义呼吸灯的最大亮度
#define BREATH_MIN_BRIGHTNESS 50            // 定义呼吸灯的最小亮度
#define BREATH_STEP 10                      // 定义每次亮度变化的步长
#define BREATH_DELAY_MS 50                  // 定义亮度变化的延迟时间
#define PWM_GPIO 7                          // 定义PWM信号的GPIO引脚
#define PWM_PIN_MODE 1                      // 定义引脚模式为
#define PWM_CHANNEL 7                       // 定义PWM使用的通道
#define PWM_GROUP_ID 0                      // 定义PWM组的ID

// 定义光效任务函数
static void *light_task(const char *arg)
{
    UNUSED(arg);  // 防止编译器警告，未使用的参数

    // 初始化亮度、步长等参数
    int brightness = BREATH_MIN_BRIGHTNESS;                 // 亮度从最小值开始
    int step = BREATH_STEP;                                 // 步长设为10
    
    pwm_config_t cfg_repeat = {50, 300, 0, 0, true};        // 配置PWM参数
    
    uapi_pin_set_mode(PWM_GPIO, PWM_PIN_MODE);              // 配置指定GPIO引脚的模式

    // 循环执行PWM呼吸灯效果
    while (1)
    {
        uapi_pwm_deinit();  // 重置PWM
        uapi_pwm_init();    // 初始化PWM模块

        // 配置PWM占空比
        cfg_repeat.low_time = brightness;                           // 设置低电平持续时间
        cfg_repeat.high_time = BREATH_MAX_BRIGHTNESS - brightness;  // 设置高电平持续时间
        uapi_pwm_open(PWM_CHANNEL, &cfg_repeat);                    // 打开PWM通道并配置参数

        uint8_t channel_id = PWM_CHANNEL;
        uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);   // 设置 PWM 通道组
        uapi_pwm_start_group(PWM_GROUP_ID);                 // 启动 PWM 通道组

        // 更新亮度值，逐步增加或减少
        brightness += step;

        // 判断是否达到亮度极限，反向改变步长
        if (brightness >= BREATH_MAX_BRIGHTNESS || brightness <= BREATH_MIN_BRIGHTNESS)
        {
            step = -step;  // 反向变化亮度
        }

        uapi_tcxo_delay_ms((uint32_t)BREATH_DELAY_MS);      // 延迟指定的时间，模拟呼吸灯的渐变效果
        uapi_pwm_close(PWM_GROUP_ID);                       // 关闭PWM通道组
        uapi_pwm_deinit();                                  // 重置PWM
    }

    uapi_pwm_close(PWM_GROUP_ID);                           // 确保退出时关闭PWM通道组
    uapi_pwm_deinit();                                      // 重置PWM
    
    return NULL;
}

// 启动 PWM 呼吸灯任务的入口函数
static void pwm_breathing_light_entry(void)
{
    osThreadAttr_t attr;                        // 定义线程属性结构体
    attr.name = "PWM_Breathing_light_Task";     // 设置线程名称
    attr.attr_bits = 0U;                        // 设置线程属性位，0表示无特殊属性
    attr.cb_mem = NULL;                         // 设置线程控制块内存为NULL
    attr.cb_size = 0U;                          // 设置线程控制块大小为0
    attr.stack_mem = NULL;                      // 设置线程栈内存为NULL
    attr.stack_size = LIGHT_TASK_STACK_SIZE;    // 设置线程栈大小
    attr.priority = LIGHT_TASK_PRIO;            // 设置线程优先级

    // 创建PWM呼吸灯任务
    if (osThreadNew((osThreadFunc_t)light_task, NULL, &attr) == NULL)
    {
        printf("Error: Failed to create PWM Breathing Light Task.\n");  // 如果创建任务失败输出错误信息
    }
}

// 应用启动函数
app_run(pwm_breathing_light_entry);
