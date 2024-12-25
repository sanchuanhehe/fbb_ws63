/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: ADC Sample Source. \n
 *
 * History: \n
 * 2023-07-06, Create file. \n
 */
#include "pinctrl.h"
#include "adc.h"
#include "adc_porting.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "tcxo.h"
 
#define DELAY_10000MS                   10000
#define CYCLES                          10
#define ADC_TASK_PRIO                   26
#define ADC_TASK_STACK_SIZE             0x1000
 
static void *adc_task(const char *arg)
{
    unused(arg);
    osal_printk("start adc sample\r\n");
    uapi_adc_init(ADC_CLOCK_NONE);
    uint8_t adc_channel = CONFIG_ADC_CHANNEL;
    uint16_t voltage = 0;
    uint32_t cnt = 0;
    while (cnt++ < CYCLES) {
        adc_port_read(adc_channel, &voltage);
        osal_printk("voltage: %d mv\r\n", voltage);
        osal_msleep(DELAY_10000MS);
    }
    /* 当前测量的电压值和实际值可能有较大差别，请确认是否有分压电阻，如果有分压电阻，则差别符合预期 */
    uapi_adc_deinit();

    return NULL;
}

static void adc_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)adc_task, 0, "AdcTask", ADC_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, ADC_TASK_PRIO);
    }
    osal_kthread_unlock();
}
 
/* Run the adc_entry. */
app_run(adc_entry);