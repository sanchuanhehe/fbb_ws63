/**！
 * @file get_acceleration_sample.c
 * @brief Get the acceleration in the three directions of xyz, the range can be ±2g、±4g、±8g、±16g
 * @copyright  Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/DFRobot_LIS
 */

#include "dfrobot_lis2dh12.h"

#define I2C_TASK_PRIO 24
#define I2C_TASK_STACK_SIZE 0x1000
#define DELAY_S 1000
#define DELAY_MS 1
#define ACCELERATION_DELAY_MS (300 * DELAY_MS)

static void get_acceleration_task(void)
{
    // Chip initialization
    while (!dfrobot_lis2dh12_init(CONFIG_I2C_SLAVE_ADDR, CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_SDA_MASTER_PIN,
                                  CONFIG_I2C_MASTER_BUS_ID)) {
        uapi_watchdog_kick();
        osal_printk("Initialization failed, please check the connection and I2C address settings\r\n");
        uapi_systick_delay_ms(DELAY_S);
    }

    // Get chip id
    osal_printk("chip id : %X\r\n", get_id());

    /**
      set range:Range(g)
                E_LIS2DH12_2G,/< ±2g>/
                E_LIS2DH12_4G,/< ±4g>/
                E_LIS2DH12_8G,/< ±8g>/
                E_LIS2DH12_16G,/< ±16g>/
    */
    set_range(E_LIS2DH12_16G);

    /**
      Set data measurement rate：
        E_POWER_DOWN_0HZ
        E_LOW_POWER_1HZ
        E_LOW_POWER_10HZ
        E_LOW_POWER_25HZ
        E_LOW_POWER_50HZ
        E_LOW_POWER_100HZ
        E_LOW_POWER_200HZ
        E_LOW_POWER_400HZ
    */
    set_acquire_rate(E_LOW_POWER_10HZ);
    osal_printk("Acceleration:\r\n");
    uapi_systick_delay_ms(DELAY_S);

    while (1) {
        uapi_watchdog_kick();
        // Get the acceleration in the three directions of xyz
        long ax;
        long ay;
        long az;
        // The measurement range can be ±100g or ±200g set by the set_range() function
        ax = read_acc_x(); // Get the acceleration in the x direction
        ay = read_acc_y(); // Get the acceleration in the y direction
        az = read_acc_z(); // Get the acceleration in the z direction
        // Print acceleration
        osal_printk("x: %d mg\t y: %d mg\t z: %d mg\r\n", ax, ay, az);

        uapi_systick_delay_ms(ACCELERATION_DELAY_MS);
    }
}

static void get_acceleration_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)get_acceleration_task, 0, "getAccelerationTask", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(get_acceleration_entry);