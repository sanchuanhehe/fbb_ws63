/**！
 * @file beetle_df9gms_sample.c
 * @brief The servo will gradually rotate from 0 degrees to 180 degrees, and then gradually return to 0 degrees.
 * @copyright  Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "systick.h"
#include "watchdog.h"
#include "app_init.h"
#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include <stddef.h>

#define DF9GMS_TASK_STACK_SIZE 0x1000
#define DF9GMS_TASK_PRIO 17
#define FREQ_TIME 20000
#define DF9GMS_PULSE_COUNT 10
#define DELAY_S 1000
#define ANGLE_0 0
#define ANGLE_90 90
#define ANGLE_180 180

// 定时器相关定义
#define SERVO_TIMER_INDEX 2
#define SERVO_TIMER_PRIO 2
#define SERVO_TIMER_IRQ TIMER_2_IRQN
#define SERVO_PWM_PERIOD_US 20000 // 20ms周期
#define SERVO_MIN_PULSE_US 500    // 0.5ms脉宽
#define SERVO_MAX_PULSE_US 2500   // 2.5ms脉宽

// 全局变量
static timer_handle_t g_servo_timer = NULL;
static uint32_t g_current_angle = 0;
static uint32_t g_current_pulse_width = 0;
static uint32_t g_pulse_start_time = 0;

// 定时器回调函数 - 产生持续PWM信号
static void servo_timer_callback(uintptr_t data)
{
    unused(data);
    static uint32_t pulse_phase = 0; // 0: 高电平阶段, 1: 低电平阶段

    if (pulse_phase == 0) {
        // 高电平阶段开始
        uapi_gpio_set_val(CONFIG_DF9GMS_PIN, GPIO_LEVEL_HIGH);
        g_pulse_start_time = uapi_tcxo_get_us();
        pulse_phase = 1;
        // 设置脉宽定时
        uapi_timer_start(g_servo_timer, g_current_pulse_width, servo_timer_callback, 0);
    } else {
        // 低电平阶段开始
        uapi_gpio_set_val(CONFIG_DF9GMS_PIN, GPIO_LEVEL_LOW);
        pulse_phase = 0;
        // 设置剩余周期定时，持续发波
        uapi_timer_start(g_servo_timer, SERVO_PWM_PERIOD_US - g_current_pulse_width, servo_timer_callback, 0);
    }
}

void df9gms_init(void)
{
    uapi_pin_set_mode(CONFIG_DF9GMS_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_DF9GMS_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_DF9GMS_PIN, GPIO_LEVEL_LOW);
}

// 初始化定时器PWM
void servo_pwm_init(void)
{
    // 初始化定时器
    uapi_timer_init();
    uapi_timer_adapter(SERVO_TIMER_INDEX, SERVO_TIMER_IRQ, SERVO_TIMER_PRIO);
    uapi_timer_create(SERVO_TIMER_INDEX, &g_servo_timer);

    // 设置初始角度为0度
    g_current_angle = 0;
    g_current_pulse_width = SERVO_MIN_PULSE_US;
}

// 停止PWM输出
void servo_pwm_stop(void)
{
    if (g_servo_timer != NULL) {
        uapi_timer_stop(g_servo_timer);
        uapi_gpio_set_val(CONFIG_DF9GMS_PIN, GPIO_LEVEL_LOW);
    }
}

// 销毁定时器
void servo_pwm_deinit(void)
{
    if (g_servo_timer != NULL) {
        uapi_timer_stop(g_servo_timer);
        uapi_timer_delete(g_servo_timer);
        g_servo_timer = NULL;
    }
}

// 将角度(0~180)映射为脉宽并启动持续PWM
void servo_set_pos(unsigned int pos)
{
    if (pos > ANGLE_180) {
        pos = ANGLE_180; // 限制最大角度
    }

    // 计算对应的脉宽 (us)
    unsigned int duty = SERVO_MIN_PULSE_US + (pos * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US)) / 180;

    // 更新全局变量
    g_current_angle = pos;
    g_current_pulse_width = duty;

    // 启动定时器开始持续发波
    if (g_servo_timer != NULL) {
        uapi_timer_start(g_servo_timer, g_current_pulse_width, servo_timer_callback, 0);
    }
}

void df9gms_task(void)
{
    // 初始化GPIO
    df9gms_init();

    // 初始化定时器PWM
    servo_pwm_init();

    // 舵机运动序列 - 每次调用ServoSetPos都会启动持续PWM
    servo_set_pos(ANGLE_0); // 转到0度并保持
    uapi_systick_delay_ms(DELAY_S);

    servo_set_pos(ANGLE_90); // 转到90度并保持
    uapi_systick_delay_ms(DELAY_S);

    servo_set_pos(ANGLE_180); // 转到180度并保持
    uapi_systick_delay_ms(DELAY_S);

    servo_set_pos(ANGLE_90); // 转到90度并保持
    uapi_systick_delay_ms(DELAY_S);

    servo_set_pos(ANGLE_0); // 转到0度并保持
    uapi_systick_delay_ms(DELAY_S);

    // 停止PWM
    servo_pwm_stop();
    uapi_systick_delay_ms(DELAY_S);

    // 清理定时器资源
    servo_pwm_deinit();
}

void df9gms_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)df9gms_task, NULL, "df9gms_task", DF9GMS_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, DF9GMS_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}
app_run(df9gms_entry);