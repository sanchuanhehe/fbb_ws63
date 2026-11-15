/**！
 * @file interrupt_sample.c
 * @brief Interrupt detection
 * @n In this example, the enable E_Z_HIGHER_THAN_TH interrupt event means when the acceleration in the Z direction
 * exceeds the
 * @n threshold set by the program, the interrupt level can be detected on the interrupt pin int1/int2 we set, and the
 * level change on the
 * @n interrupt pin can be used to determine whether the interrupt occurs. The following are the 6 settable interrupt
 * events：E_X_HIGHER_THAN_TH,
 * @n E_X_LOWER_THAN_TH, E_Y_HIGHER_THAN_TH, E_Y_LOWER_THAN_TH, E_Z_HIGHER_THAN_TH, E_Z_LOWER_THAN_TH. For a detailed
 * explanation of each of them,
 * @n please look up the comments of the enable_interrupt_event() function.
 * @n This example needs to connect the int2/int1 pin of the module to the interrupt pin of the motherboard.
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
#define THRESHOLD 6
#define INTERRUPT_DELAY_MS (300 * DELAY_MS)

// Interrupt generation flag
volatile bool int_flag = false;

void inter_event(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    int_flag = true;
}

// from Arduino
#define RISING 0x00000001
#define FALLING 0x00000002
#define CHANGE 0x00000003
#define ONLOW 0x00000004
#define ONHIGH 0x00000005

void attach_interrupt(uint8_t pin, gpio_callback_t callback, uint32_t mode)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(pin, mode, callback);
    if (ret != 0) {
        uapi_gpio_unregister_isr_func(pin);
    }
}

static void interrupt_task(void)
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

    attach_interrupt(CONFIG_INTERRUPT_PIN, inter_event, CHANGE);

    /**
    Set the threshold of interrupt source 1 interrupt
    threshold:Threshold(g)
    */
    set_int1_th(THRESHOLD); // Unit: g

    /*!
    Enable interrupt
    Interrupt pin selection:
      E_INT1 = 0,/<int1 >/
      E_INT2,/<int2>/
    Interrupt event selection:
      E_X_LOWER_THAN_TH ,/<The acceleration in the x direction is less than the threshold>/
      E_X_HIGHER_THAN_TH ,/<The acceleration in the x direction is greater than the threshold>/
      E_Y_LOWER_THAN_TH,/<The acceleration in the y direction is less than the threshold>/
      E_Y_HIGHER_THAN_TH,/<The acceleration in the y direction is greater than the threshold>/
      E_Z_LOWER_THAN_TH,/<The acceleration in the z direction is less than the threshold>/
      E_Z_HIGHER_THAN_TH,/<The acceleration in the z direction is greater than the threshold>/
    */
    enable_interrupt_event(E_INT1 /* int pin */, E_Z_HIGHER_THAN_TH /* interrupt event */);

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
        uapi_systick_delay_ms(INTERRUPT_DELAY_MS);
        // The interrupt flag is set
        if (int_flag == true) {
            // Check whether the interrupt event is generated in interrupt 1
            if (get_int1_event(E_Z_HIGHER_THAN_TH)) {
                osal_printk("The acceleration in the z direction is greater than the threshold\r\n");
            }
            int_flag = false;
        }
    }
}

static void interrupt_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)interrupt_task, 0, "interruptTask", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(interrupt_entry);