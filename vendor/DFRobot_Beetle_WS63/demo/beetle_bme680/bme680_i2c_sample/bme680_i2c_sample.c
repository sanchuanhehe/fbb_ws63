/*!
 * @file bme680_i2c_sample.c
 * @brief connect bme680 I2C interface with your board (please reference board compatibility)
 * @n Temprature, Humidity, pressure, altitude, calibrate altitude and gas resistance data will print on serial window.
 *
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author Martin(Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 * @url https://github.com/DFRobot/dfrobot_bme680
 */

#include "dfrobot_bme680_i2c.h"

#define I2C_TASK_PRIO 24
#define I2C_TASK_STACK_SIZE 0x1000
#define DELAY_S 1000
#define TEMP_RAW_TO_CELSIUS 100
#define HUMIDITY_RAW_TO_PERCENT 1000
#define BME680_BEGIN_DELAY_MS (DELAY_S * 2)

float g_sea_level;

void bme680_task(void)
{
    dfrobot_bme680_i2c_init(CONFIG_I2C_SLAVE_ADDR, CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_SDA_MASTER_PIN,
                            CONFIG_I2C_MASTER_BUS_ID);

    while (bme680_bme680_begin() != 0) {
        uapi_watchdog_kick();
        printf("bme begin failure\r\n");
        uapi_systick_delay_ms(BME680_BEGIN_DELAY_MS);
    }
    printf("bme begin successful\r\n");
    static char templine[64] = {0};
#ifdef CONFIG_CALIBRATE_PRESSURE
    start_convert();
    uapi_systick_delay_ms(DELAY_S);
    bme680_bme680_update();
    /* You can use an accurate altitude to calibrate sea level air pressure.
     * And then use this calibrated sea level pressure as a reference to obtain the calibrated altitude.
     * In this case,525.0m is chendu accurate altitude.
     */
    g_sea_level = read_sea_level(525.0);
    sprintf(templine, "seaLevel: %.2f\r\n", g_sea_level);
    printf(templine);
#endif

    while (1) {
        uapi_watchdog_kick();
        start_convert();
        uapi_systick_delay_ms(DELAY_S);
        bme680_bme680_update();
        sprintf(templine, "temperature(C) : %.2f\r\n", read_temperature() / TEMP_RAW_TO_CELSIUS);
        printf(templine);
        sprintf(templine, "pressure(Pa) : %.2f\r\n", read_pressure());
        printf(templine);
        sprintf(templine, "humidity(rh) : %.2f\r\n", read_humidity() / HUMIDITY_RAW_TO_PERCENT);
        printf(templine);
        sprintf(templine, "gas resistance(ohm) : %.2f\r\n", read_gas_resistance());
        printf(templine);
        sprintf(templine, "altitude(m) : %.2f\r\n", read_altitude());
        printf(templine);
#ifdef CONFIG_CALIBRATE_PRESSURE
        sprintf(templine, "calibrated altitude(m) : %.2f\r\n", read_calibrated_altitude(g_sea_level));
        printf(templine);
#endif
    }
}

void bme680_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)bme680_task, NULL, "bme680_task", I2C_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, I2C_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}
app_run(bme680_entry);