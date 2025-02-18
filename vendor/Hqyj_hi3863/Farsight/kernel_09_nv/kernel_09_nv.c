/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.h"
#include "nv.h"
#include "soc_osal.h"
#include "app_init.h"
#include "nv_common_cfg.h"
#include "common_def.h"
#include "cmsis_os2.h"
#include <stdio.h>
#define ADDR_LEN6 6
#define KEYID 0xa
#define DELAY_TIME_MS 300
osThreadId_t Task1_ID; //  任务1 ID
uint8_t buf[ADDR_LEN6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
static int nv_write_func(void)
{
    printf("[write data]: ");
    for (size_t i = 0; i < sizeof(buf); i++) {
        /* code */
        printf("%x ", buf[i]);
    }
    printf("\r\n");
    uint16_t key = KEYID;
    uint16_t key_len = sizeof(mytest_config_t);
    uint8_t *product_config = osal_vmalloc(key_len);
    memcpy(product_config, buf, sizeof(buf));
    int ret = uapi_nv_write(key, product_config, key_len);
    osal_vfree(product_config);
    product_config = NULL;
    if (ret == 0) {
        return 1;
    } else {
        return 0;
    }
}

static int nv_read_func(void)
{
    uint16_t key = KEYID;
    uint16_t key_len = sizeof(mytest_config_t);
    uint16_t real_len = 0;
    uint8_t *read_value = osal_vmalloc(key_len);
    int ret = uapi_nv_read(key, key_len, &real_len, read_value);
    if (ret == 0) {
        printf("[get data]: ");
        for (size_t i = 0; i < sizeof(buf); i++) {
            /* code */
            printf("%x ", read_value[i]);
        }
        printf("\r\n");
        for (size_t i = 0; i < sizeof(buf); i++) {
            buf[i] += read_value[i];
        }
        osal_vfree(read_value);
        read_value = NULL;
        return 1;
    } else {
        osal_vfree(read_value);
        read_value = NULL;
        return 0;
    }
}

/**
 * @description: 任务1
 * @param {*}
 * @return {*}
 */
void task1(const char *argument)
{
    unused(argument);
    osDelay(DELAY_TIME_MS); // 等待你打开串口
    printf("----------------------------------!\n");
    uint8_t ret = nv_read_func();
    if (ret) {
        printf("nv read ok!\n");
    } else {
        printf("nv read fail!\n");
    }
    osDelay(DELAY_TIME_MS);
    ret = nv_write_func();
    if (ret) {
        printf("nv write ok!\n");
    } else {
        printf("nv write fail!\n");
    }
}

static void kernel_task_example(void)
{
    osThreadAttr_t attr;
    attr.name = "Task1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x1000;
    attr.priority = osPriorityNormal;

    Task1_ID = osThreadNew((osThreadFunc_t)task1, NULL, &attr); // 创建任务1
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\n", Task1_ID);
    }
}

/* Run the sample. */
app_run(kernel_task_example);