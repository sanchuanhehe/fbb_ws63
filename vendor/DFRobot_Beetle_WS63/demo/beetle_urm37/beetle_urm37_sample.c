/**！
 * @file beetle_urm37_sample.c
 * @brief Measuring distance by driving the URM37 via GPIO.
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
#include "systick.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "app_init.h"

#define DELAY_US 1100
#define DELAY_MS 200
#define URM37_INVALID_TIME 50000

#define URM37_TASK_STACK_SIZE 0x1000
#define URM37_TASK_PRIO 24

void urm37_init(void)
{
    uapi_pin_set_mode(CONFIG_URTRIG_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_URTRIG_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_URTRIG_PIN, GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(CONFIG_URECHO_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_URECHO_PIN, GPIO_DIRECTION_INPUT);
}

unsigned int get_distance(void)
{
    unsigned int flag = 0;
    static uint64_t start_time = 0;
    static uint64_t time = 0;
    unsigned int distance_measured = 0;
    gpio_level_t value = 0;

    uapi_gpio_set_val(CONFIG_URTRIG_PIN, GPIO_LEVEL_LOW);
    uapi_systick_delay_us(DELAY_US);
    uapi_gpio_set_val(CONFIG_URTRIG_PIN, GPIO_LEVEL_HIGH);

    while (1) {
        uapi_watchdog_kick();

        value = uapi_gpio_get_output_val(CONFIG_URECHO_PIN);
        if (value == GPIO_LEVEL_LOW && flag == 0) {
            /*
             * 获取系统时间
             * get SysTime
             */
            start_time = uapi_systick_get_us();
            flag = 1;
        }

        if (value == GPIO_LEVEL_HIGH && flag == 1) {
            /*
             * 获取高电平持续时间
             * Get high level duration
             */
            time = uapi_systick_get_us() - start_time;
            break;
        }
    }

    if (time >= URM37_INVALID_TIME) {
        printf("Invalid");
    } else {
        distance_measured = time / 50; // every 50us low level stands for 1cm
        printf("distance = %ucm\n", distance_measured);
    }

    return distance_measured;
}

void urm37_task(void)
{
    urm37_init();
    printf("urm37_task init\r\n");

    while (1) {
        get_distance();
        osal_mdelay(DELAY_MS);
    }
}

void urm37_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)urm37_task, NULL, "urm37_task", URM37_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, URM37_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}
app_run(urm37_entry);