/**！
 * @file beetle_ds18b20_sample.c
 * @brief Implementing the OneWire protocol via GPIO to drive the DS18B20.
 * @copyright  Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Martin](Martin@dfrobot.com)
 * @version  V1.0
 * @date  2025-9-29
 */

#include "pinctrl.h"
#include "gpio.h"
#include "systick.h"
#include "soc_osal.h"
#include "app_init.h"

#include "tcxo.h"

/** Task configuration constants */
#define DS_TASK_PRIO 24
#define DS_TASK_STACK_SIZE 0x1000
#define READITV 10
#define TASKITV 1000

/** OneWire timing constants (in microseconds) */
#define DS18B20_RESET_LOW_TIME_US 480
#define DS18B20_RESET_RELEASE_TIME_US 70
#define DS18B20_RESET_WAIT_TIME_US 410
#define DS18B20_WRITE_BIT_1_LOW_TIME_US 6
#define DS18B20_WRITE_BIT_1_RELEASE_TIME_US 64
#define DS18B20_WRITE_BIT_0_LOW_TIME_US 60
#define DS18B20_WRITE_BIT_0_RELEASE_TIME_US 10
#define DS18B20_READ_BIT_LOW_TIME_US 6
#define DS18B20_READ_BIT_SAMPLE_TIME_US 9
#define DS18B20_READ_BIT_WAIT_TIME_US 55

/** OneWire protocol constants */
#define DS18B20_BITS_PER_BYTE 8
#define DS18B20_BIT_MASK_LSB 0x01
#define DS18B20_BIT_MASK_MSB 0x80

/** DS18B20 command constants */
#define DS18B20_CMD_SKIP_ROM 0xCC
#define DS18B20_CMD_CONVERT_T 0x44
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE

/** DS18B20 conversion timeout (in microseconds) */
#define DS18B20_CONVERSION_TIMEOUT_US 800000

/** Temperature conversion constants */
#define DS18B20_TEMP_DIVISOR 16.0f

/** Buffer size constants */
#define DS18B20_TEMPLINE_BUFFER_SIZE 32

static inline uint32_t get_time_us(void)
{
    return (uint32_t)uapi_tcxo_get_us();
}

static void delay_us(uint32_t us)
{
    uapi_tcxo_delay_us(us);
}

static inline void bus_drive_low(void)
{
    uapi_gpio_set_dir(CONFIG_ONEWIRE_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_ONEWIRE_PIN, GPIO_LEVEL_LOW);
}

static inline void bus_release(void)
{
    uapi_gpio_set_dir(CONFIG_ONEWIRE_PIN, GPIO_DIRECTION_INPUT);
}

static inline uint8_t bus_read_level(void)
{
    return (uint8_t)uapi_gpio_get_val(CONFIG_ONEWIRE_PIN);
}

static bool ow_reset(void)
{
    bus_drive_low();
    delay_us(DS18B20_RESET_LOW_TIME_US);
    bus_release();
    delay_us(DS18B20_RESET_RELEASE_TIME_US);
    bool present = (bus_read_level() == 0);
    delay_us(DS18B20_RESET_WAIT_TIME_US);
    return present;
}

static void ow_write_bit(uint8_t bit)
{
    if (bit) {
        bus_drive_low();
        delay_us(DS18B20_WRITE_BIT_1_LOW_TIME_US);
        bus_release();
        delay_us(DS18B20_WRITE_BIT_1_RELEASE_TIME_US);
    } else {
        bus_drive_low();
        delay_us(DS18B20_WRITE_BIT_0_LOW_TIME_US);
        bus_release();
        delay_us(DS18B20_WRITE_BIT_0_RELEASE_TIME_US);
    }
}

static uint8_t ow_read_bit(void)
{
    uint8_t bit;
    bus_drive_low();
    delay_us(DS18B20_READ_BIT_LOW_TIME_US);
    bus_release();
    delay_us(DS18B20_READ_BIT_SAMPLE_TIME_US);
    bit = bus_read_level();
    delay_us(DS18B20_READ_BIT_WAIT_TIME_US);
    return bit;
}

static void ow_write_byte(uint8_t val)
{
    for (int i = 0; i < DS18B20_BITS_PER_BYTE; i++) {
        ow_write_bit(val & DS18B20_BIT_MASK_LSB);
        val >>= 1;
    }
}

static uint8_t ow_read_byte(void)
{
    uint8_t v = 0;
    for (int i = 0; i < DS18B20_BITS_PER_BYTE; i++) {
        v >>= 1;
        if (ow_read_bit()) {
            v |= DS18B20_BIT_MASK_MSB;
        }
    }
    return v;
}

static bool ds18b20_read_temp_raw(int16_t *temp_raw)
{
    if (!ow_reset()) {
        return false;
    }
    ow_write_byte(DS18B20_CMD_SKIP_ROM); // SKIP ROM
    ow_write_byte(DS18B20_CMD_CONVERT_T); // CONVERT T

    uint32_t start = get_time_us();
    while ((uint32_t)(get_time_us() - start) < DS18B20_CONVERSION_TIMEOUT_US) {
        if (ow_read_bit()) {
            break;
        }
        osal_msleep(READITV);
    }

    if (!ow_reset()) {
        return false;
    }
    ow_write_byte(DS18B20_CMD_SKIP_ROM);
    ow_write_byte(DS18B20_CMD_READ_SCRATCHPAD); // READ SCRATCHPAD

    uint8_t temp_l = ow_read_byte();
    uint8_t temp_h = ow_read_byte();
    *temp_raw = (int16_t)((temp_h << DS18B20_BITS_PER_BYTE) | temp_l);
    return true;
}

static float ds18b20_raw_to_celsius(int16_t raw)
{
    return (float)raw / DS18B20_TEMP_DIVISOR;
}

static int ds18b20_task(const char *arg)
{
    unused(arg);

    uapi_pin_set_mode(CONFIG_ONEWIRE_PIN, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(CONFIG_ONEWIRE_PIN, PIN_PULL_TYPE_UP);
    uapi_gpio_init();
    uapi_systick_init();

    static char templine[DS18B20_TEMPLINE_BUFFER_SIZE] = {0};
    while (1) {
        int16_t raw = 0;
        bool ok = ds18b20_read_temp_raw(&raw);
        if (ok) {
            float t = ds18b20_raw_to_celsius(raw);
            sprintf(templine, "DS18B20: %.2f C\r\n", t);
            printf(templine);
        } else {
            osal_printk("DS18B20: no presence\r\n");
        }
        osal_msleep(TASKITV);
    }

    return 0;
}

static void ds18b20_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ds18b20_task, 0, "DS18B20Task", DS_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, DS_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(ds18b20_entry);