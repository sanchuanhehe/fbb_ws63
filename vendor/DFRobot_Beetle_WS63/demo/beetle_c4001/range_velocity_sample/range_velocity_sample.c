/*!
 * @file  range_velocity_sample.c
 * @brief  radar measurement demo
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

#define DELAY_MS 1

/** Task configuration constants */
#define RANGE_VELOCITY_TASK_STACK_SIZE 0x2000
#define RANGE_VELOCITY_TASK_PRIO 25

/** Detection threshold constants */
#define RANGE_VELOCITY_DETECT_MIN_CM 11
#define RANGE_VELOCITY_DETECT_MAX_CM 1200
#define RANGE_VELOCITY_DETECT_THRES 10

/** Delay constants */
#define RANGE_VELOCITY_DELAY_MS (100 * DELAY_MS)

/** Buffer size constants */
#define RANGE_VELOCITY_TEMPLINE_BUFFER_SIZE 32

static void m_range_velocity_task(void)
{
    osal_printk("Radar example start!\r\n");

    dfrobot_c4001_init(CONFIG_RADAR_UART_BAUD, CONFIG_RADAR_UART_TX_PIN, CONFIG_RADAR_UART_RX_PIN, CONFIG_UART_BUS_ID);

    osal_printk("Radar connected!\r\n");

    // speed Mode
    set_sensor_mode(SPEEDMODE);

    s_sensor_status_t status = get_status();
    osal_printk("work status  = %d\r\n", status.work_status); // 0 stop 1 start
    osal_printk("work mode    = %d\r\n", status.work_mode);   // 0 exist 1 speed
    osal_printk("init status  = %d\r\n", status.init_status); // 0 no init 1 success
    uapi_watchdog_kick();

    /*
     * min Detection range Minimum distance, unit cm, range 0.3~20m (30~2000), not exceeding max, otherwise the function
     * is abnormal. max Detection range Maximum distance, unit cm, range 2.4~20m (240~2000) thres Target detection
     * threshold, dimensionless unit 0.1, range 0~6553.5 (0~65535)
     */
    if (set_detect_thres(RANGE_VELOCITY_DETECT_MIN_CM /* min */, RANGE_VELOCITY_DETECT_MAX_CM /* max */,
                         RANGE_VELOCITY_DETECT_THRES /* thres */)) {
        osal_printk("set detect threshold successfully\r\n");
    }

    // set Fretting Detection
    set_fretting_detection(SWITCH_ON);

    uapi_watchdog_kick();

    osal_printk("min range = %d\r\n", get_t_min_range());
    osal_printk("max range = %d\r\n", get_t_max_range());
    osal_printk("threshold range = %d\r\n", get_thres_range());
    osal_printk("fretting detection = %d\r\n", get_fretting_detection());

    static char templine[RANGE_VELOCITY_TEMPLINE_BUFFER_SIZE] = {0};
    while (1) {
        uapi_watchdog_kick();
        osal_printk("target number = %d\r\n", get_target_number());
        sprintf(templine, "target Speed = %.2f m/s\r\n", get_target_speed());
        osal_printk("%s", templine);
        sprintf(templine, "target distance = %.2f m\r\n", get_target_range());
        osal_printk("%s", templine);
        uapi_systick_delay_ms(RANGE_VELOCITY_DELAY_MS);
    }
}

// 示例入口
static void m_range_velocity_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)m_range_velocity_task, NULL, "RadarTask",
                                      RANGE_VELOCITY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, RANGE_VELOCITY_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(m_range_velocity_entry);
