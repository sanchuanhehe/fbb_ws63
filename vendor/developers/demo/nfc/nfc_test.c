/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <unistd.h>
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "i2c.h"
#include "pinctrl.h"
#include "gpio.h"

#include "nfc_config.h"

#define I2C_TASK_PRIO 24
#define I2C_TASK_STACK_SIZE 0x1000
#define DELAY_250_US 250
#define DELAY_10_MS 10000

static int i2c_init(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);

    return uapi_i2c_master_init(1, HI_I2C_IDX_BAUDRATE, 0);
}
void *nfc_task(const char *arg)
{
    unused(arg);

    i2c_init();

    // CSN
    uapi_pin_set_mode(CONFIG_NFC_CSN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_NFC_CSN, GPIO_DIRECTION_OUTPUT);
    uapi_pin_set_pull(CONFIG_NFC_CSN, 0);
    osal_udelay(DELAY_250_US);

    // IRQ_N
    uapi_pin_set_mode(CONFIG_NFC_IRQ, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_NFC_IRQ, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_NFC_IRQ, 1);

    printf("[NfcTask] NfcTask is running ...\n");
    nfc_init();
    while (1) {
        nfcread();
        osal_udelay(DELAY_10_MS);
    }
}

static void nfc_test_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)nfc_task, 0, "NfcTask", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    } else {
        printf("[NfcTestEntry] Falied to create %s!\n", "NfcTask");
    }
    osal_kthread_unlock();
}

app_run(nfc_test_entry);
