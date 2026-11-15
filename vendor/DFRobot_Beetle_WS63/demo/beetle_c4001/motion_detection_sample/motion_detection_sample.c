/*!
 * @file  motion_detection_sample.c
 * @brief  Example of radar detecting whether an object is moving
 * @copyright Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version V1.0
 * @date 2025-09-29
 * @url https://github.com/dfrobot/DFRobot_C4001
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "systick.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "app_init.h"
#include "uart.h"
#include "dfrobot_c4001.h"

#define MINRANGE 30
#define MAXRANGE 1000
#define TRIGRANGE 1000
#define TRIG_SENSITIVITY 1
#define KEEP_SENSITIVITY 2
#define TRIG_DELAY 100
#define KEEP_TIME 4
#define PWM_NO_TARGET_DUTY 50
#define PWM_TARGET_DUTY 0
#define DETECTION_TIMER_MS 10
#define IOPOLAITY 1
#define DELAY_MS 1

/** Delay constants */
#define MOTION_DETECTION_DELAY_MS (100 * DELAY_MS)

/** Task configuration constants */
#define MOTION_DETECTION_TASK_STACK_SIZE 0x2000
#define MOTION_DETECTION_TASK_PRIO 25

static void radar_example(void)
{
    osal_printk("Radar example start!\r\n");

    dfrobot_c4001_init(CONFIG_RADAR_UART_BAUD, CONFIG_RADAR_UART_TX_PIN, CONFIG_RADAR_UART_RX_PIN, CONFIG_UART_BUS_ID);

    osal_printk("Radar connected!\r\n");

    // exist Mode
    set_sensor_mode(EXITMODE);

    s_sensor_status_t status = get_status();
    osal_printk("work status  = %d\r\n", status.work_status); // 0 stop 1 start
    osal_printk("work mode    = %d\r\n", status.work_mode);   // 0 exist 1 speed
    osal_printk("init status  = %d\r\n", status.init_status); // 0 no init 1 success

    // 设置检测范围
    if (set_detection_range(MINRANGE, MAXRANGE, TRIGRANGE)) {
        osal_printk("set detection range successfully!\r\n");
    }

    // 设置触发灵敏度
    if (set_trig_sensitivity(TRIG_SENSITIVITY)) {
        osal_printk("set trig sensitivity successfully!\r\n");
    }

    // 设置保持灵敏度
    if (set_keep_sensitivity(KEEP_SENSITIVITY)) {
        osal_printk("set keep sensitivity successfully!\r\n");
    }

    // 设置触发延时和保持时间
    if (set_delay(TRIG_DELAY, KEEP_TIME)) {
        osal_printk("set delay successfully!\r\n");
    }

    uapi_watchdog_kick();

    // 设置 PWM 输出
    if (set_pwm(PWM_NO_TARGET_DUTY, PWM_TARGET_DUTY, DETECTION_TIMER_MS)) {
        osal_printk("set pwm period successfully!\r\n");
    }

    // 设置 IO 极性
    if (set_io_polarity(IOPOLAITY)) {
        osal_printk("set Io Polaity successfully!\r\n");
    }

    // 打印当前参数
    osal_printk("trig sensitivity = %d\r\n", get_trig_sensitivity());
    osal_printk("keep sensitivity = %d\r\n", get_keep_sensitivity());
    osal_printk("min range = %d\r\n", get_min_range());
    osal_printk("max range = %d\r\n", get_max_range());
    uapi_watchdog_kick();
    osal_printk("trig range = %d\r\n", get_trig_range());
    osal_printk("keep time = %d\r\n", get_keep_timeout());
    osal_printk("trig delay = %d\r\n", get_trig_delay());
    osal_printk("polaity = %d\r\n", get_io_polarity());
    uapi_watchdog_kick();
    s_pwm_data_t pwmData = get_pwm();
    osal_printk("pwm1 = %d\r\n", pwmData.pwm1);
    osal_printk("pwm2 = %d\r\n", pwmData.pwm2);
    osal_printk("pwm timer = %d\r\n", pwmData.timer);

    // 主循环
    while (1) {
        uapi_watchdog_kick();
        if (motion_detection()) {
            osal_printk("exist motion\r\n");
        }
        uapi_systick_delay_ms(MOTION_DETECTION_DELAY_MS);
    }
}

// 示例入口
static void radar_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)radar_example, NULL, "RadarTask", MOTION_DETECTION_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MOTION_DETECTION_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(radar_entry);
