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

#include "lwip/sntp.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"
#include <stdio.h>
#include "soc_osal.h"
#include "app_init.h"
#include "wifi_connect.h"
#include "lwip/opt.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "i2c.h"
#include "pinctrl.h"

#define CONFIG_WIFI_SSID "H"        // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD "123456789" // 要连接的WiFi 热点密码
#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SLAVE1_ADDR 0x38
#define I2C_SET_BANDRATE 400000

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }
    ssd1306_Init();
    ssd1306_Fill(Black);
}

int start_sntp(void)
{
    int ret;
    char year_str[11];
    char time_str[54];
    int server_num = 1;                    /*Number of SNTP servers available*/
    char *sntp_server = "203.107.6.88"; /*sntp_server : List of the available servers*/
    struct timeval time_local;             /*Output Local time of server, which will be received in NTP*/

    memset(&time_local, 0, sizeof(time_local));

    // Start SNTP
    ret = lwip_sntp_start(server_num, &sntp_server, &time_local);
    if (ret != 0) {
        printf("SNTP start failed with code: %d\n", ret);
        return ret;
    }
    // 确保时间已同步
    if (time_local.tv_sec == 0) {
        printf("SNTP time not yet synchronized\n");
        return -1;
    }
    // 转换为UTC时间结构（不进行时区转换）
    time_t rawtime = time_local.tv_sec;
    struct tm *utc_time = gmtime(&rawtime); // 使用gmtime而不是localtime

    if (utc_time == NULL) {
        printf("gmtime failed\r\n");
        return -1;
    }
    // 处理可能的溢出（超过 24 小时）
    mktime(utc_time); // 自动规范化时间（如 25:00 → 01:00）
    ret = sprintf(year_str, "%04d-%02d-%02d", utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday);
    if (ret < 0) {
        printf("year_str failed\r\n");
        return ret;
    }
    ret = sprintf(time_str, "%02d:%02d:%02d", utc_time->tm_hour + 8, utc_time->tm_min, utc_time->tm_sec);
    if (ret < 0) {
        printf("time_str failed\r\n");
        return ret;
    }
    ssd1306_SetCursor(32, 8);
    ssd1306_DrawString(year_str, Font_7x10, White);
    ssd1306_SetCursor(35, 20);
    ssd1306_DrawString(time_str, Font_7x10, White);
    ssd1306_UpdateScreen();
    return ret;
}

int sntp_task(void)
{
    /* after doing lwIP init, driver init and netifapi_netif_add */
    app_i2c_init_pin();
    // 初始化LWIP和网络接口
    wifi_connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);
    while (1) {
        osal_msleep(100);
        start_sntp();
    }

    return 0;
}

static void sntp_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sntp_task, 0, "sntpTask", 0x2000);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, 24);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(sntp_entry);
