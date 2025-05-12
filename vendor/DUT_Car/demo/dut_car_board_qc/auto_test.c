/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    Copyright (c) 2024/12/18  KangBohao@OurEDA， Dalian Univ of Tech 
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#include "i2c.h"
#include "bh1750.h"
#include "aht20.h"
#include "cw2015.h"
#include "ins5699s.h"
#include "bmx055.h"
#include "ws2812b.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "io_expander.h"
#include "moter_control.h"
#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"

#define WIFI_SOFTAP_TASK_PRIO     24          // 软AP任务优先级
#define WIFI_TASK_STACK_SIZE      0x2000      // 软AP任务栈大小(8KB)
#define WIFI_IFNAME_MAX_SIZE      16          // 网络接口名最大长度

#define SENSOR_TASK_STACK_SIZE    0x2000      // 传感器任务栈大小(8KB)
#define SENSOR_TASK_PRIO          20          // 传感器任务优先级

#define TRICOLORED_TASK_PRIO      16          // 三色灯任务优先级
#define TRICOLORED_TASK_STACK_SIZE 0x1000     // 三色灯任务栈大小(4KB)

#define WS2812B_LED_0             5           // WS2812B 数据引脚GPIO编号
#define WS2812B_LED_MODE          4           // WS2812B 引脚复用模式
#define WS2812B_BRIGHTNESS        100         // WS2812B 亮度(0-255)

#define CONFIG_I2C_SCL_MASTER_PIN 15          // I2C SCL 引脚
#define CONFIG_I2C_SDA_MASTER_PIN 16          // I2C SDA 引脚
#define CONFIG_I2C_MASTER_PIN_MODE 2          // I2C 引脚复用模式
#define I2C_MASTER_ADDR           0x0         // I2C 总线主机地址
#define I2C_SET_BANDRATE          100000      // I2C 波特率(100kHz)

#define RGB_MAX 255
#define RGB_NUM 6


static void *auto_test(const char *arg){
    unused(arg);
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE); // 设置SCL引脚复用模式
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE); // 设置SDA引脚复用模式

    uint32_t baudrate = I2C_SET_BANDRATE;  // I2C波特率
    uint32_t hscode = I2C_MASTER_ADDR;     // I2C主机地址
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode); // 初始化I2C总线1
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }

    // motor test
    pwm_write(0x16);                      // PWM通道0x16
    left_wheel_set(500, 500, true);       // 左轮正转速度500
    right_wheel_set(500, 500, true);      // 右轮正转速度500
    osal_msleep(2000);                    // 2000延时2s
    left_wheel_set(500, 500, false);      // 左轮反转速度500
    right_wheel_set(500, 500, false);     // 右轮反转速度500
    osal_msleep(2000);                    // 2000延时2s
    left_wheel_set(0, 0, true);           // 停止左轮
    right_wheel_set(0, 0, true);          // 停止右轮

    // sensor test
    bh1750_init();                        // 初始化光照传感器
    aht20_init();                         // 初始化温湿度传感器
    cw2015_init();                        // 初始化电池电压传感器
    ins5699s_init();                      // 初始化RTC
    bmx055_init();                        // 初始化九轴传感器

    ssd1306_Init();                       // 初始化OLED
    ssd1306_Fill(Black);                  // 全屏填充黑色
    ssd1306_SetCursor(0, 0);              // 设置光标到(0,0)

    ins5699s_time new_time = {
        .year  = 24,                      // 年份低两位BCD
        .month = 12,                      // 月份BCD
        .day   = 18,                      // 日BCD
        .week  = 4,                       // 星期4(周四)
        .hour  = 15,                      // 时BCD
        .min   = 30,                      // 分BCD
        .sec   = 45                       // 秒BCD
    };
    ins5699s_SetTime(new_time);           // 设置RTC时间
    
    io_expander_init();                   // 初始化IO扩展器

    while (1) {
        // Serial print
        printf("========SENSOR========\r\n");
        uint32_t lightness = bh1750_GetLightIntensity(); // 读取光照值
        printf("BH1750 : %d\r\n", lightness);
        float humis = 0, temps = 0;
        aht20_GetData(&temps, &humis);    // 读取温湿度
        printf("AHT20 :temp : %d humi : %d%%\r\n", (uint16_t)temps, (uint16_t)humis);
        uint32_t vol = cw2015_GetBatteryVoltage(); // 读取电池电压
        printf("CW2015 : %d mV\r\n", vol);
        ins5699s_time time = ins5699s_GetTime(); // 读取RTC时间
        printf("INS5699S: sec:%d min:%d hour:%d week:%d day:%d month:%d year:%d \r\n",
               time.sec, time.min, time.hour, time.week, time.day, time.month, time.year);
        float ax, ay, az, gx, gy, gz, mx, my, mz;
        BMX055_GetAllData(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz); // 读取加速度、陀螺、磁力数据
        osal_printk("BMX055: ax:%d ay:%d az:%d gx:%d gy:%d gz:%d mx:%d my:%d mz:%d \r\n",
                    (int)ax, (int)ay, (int)az, (int)gx, (int)gy, (int)gz, (int)mx, (int)my, (int)mz);
        printf("=========END=========\r\n");

        // OLED print
        ssd1306_ClearOLED();             // 清屏
        ssd1306_printf("%dLx %02dC %02d%%", lightness, (uint16_t)temps, (uint16_t)humis); // 显示光照/温度/湿度
        ssd1306_printf("%02d-%02d-%02d %02d:%02d:%02d", time.year, time.month, time.day, time.hour, time.min, time.sec); // 显示时间
        ssd1306_printf("%02d|%02d|%02d %02d|%02d|%02d", (int32_t)ax, (int32_t)ay, (int32_t)az, (int32_t)gx, (int32_t)gy, (int32_t)gz); // 显示加速度/陀螺
        ssd1306_printf("%02d|%02d|%02d %04dmV", (int32_t)mx, (int32_t)my, (int32_t)mz, vol); // 显示磁力/电压
        ssd1306_UpdateScreen();          // 刷新屏幕

        osal_msleep(1000);               // 1000延时1s
    }
    return NULL;
}

static void *tricolored_task(const char *arg){
    UNUSED(arg);
    uapi_pin_set_mode(WS2812B_LED_0, WS2812B_LED_MODE); // 设置WS2812B数据引脚模式
    uapi_gpio_set_dir(WS2812B_LED_0, GPIO_DIRECTION_OUTPUT); // 配置为输出
    uapi_reg_setbit(0x44028034, 5);                  // 设置引脚寄存器位置5
    uapi_tcxo_delay_us(500);                         // 延时500µs以初始化LED
    while(1){
        for(uint8_t j = 0; j < RGB_NUM; j++){               // 循环6个LED
            rgb_display(RGB_MAX, 0, 0, WS2812B_BRIGHTNESS); // 全部显示红色
        }
        osal_msleep(1);                              // 1延时1ms
        for(uint8_t j = 0; j < RGB_NUM; j++){
            rgb_display(RGB_MAX, 0, 0, WS2812B_BRIGHTNESS);
        }
        osal_msleep(1000);                           // 1000延时1s
        for(uint8_t j = 0; j < RGB_NUM; j++){
            rgb_display(0, RGB_MAX, 0, WS2812B_BRIGHTNESS); // 全部显示绿色
        }
        osal_msleep(1);
        for(uint8_t j = 0; j < RGB_NUM; j++){
            rgb_display(0, RGB_MAX, 0, WS2812B_BRIGHTNESS);
        }
        osal_msleep(1000);                           // 1000延时1s
        for(uint8_t j = 0; j < RGB_NUM; j++){
            rgb_display(0, 0, RGB_MAX, WS2812B_BRIGHTNESS); // 全部显示蓝色
        }
        osal_msleep(1);
        for(uint8_t j = 0; j < RGB_NUM; j++){
            rgb_display(0, 0, RGB_MAX, WS2812B_BRIGHTNESS);
        }
        osal_msleep(1000);                           // 1000延时1s
    }
    return NULL;
}

static errcode_t example_softap_function(void)
{
    /* SoftAp SSID */
    char ssid[WIFI_MAX_SSID_LEN] = "WS63_AP_TEST"; // SoftAP默认SSID

    char pre_shared_key[WIFI_MAX_KEY_LEN] = "386338633863"; // 预共享密钥
    softap_config_stru hapd_conf = {0};

    char ifname[WIFI_IFNAME_MAX_SIZE] = "ap0";           // WiFi设备名
    struct netif *netif_p = NULL;
    ip4_addr_t st_gw = {0};
    ip4_addr_t st_ipaddr = {0};
    ip4_addr_t st_netmask = {0};
    IP4_ADDR(&st_ipaddr, 192, 168, 63, 1);   // 分配IP 192.168.63.1
    IP4_ADDR(&st_netmask, 255, 255, 255, 0); // 子网掩码255.255.255.0
    IP4_ADDR(&st_gw, 192, 168, 63, 2);       // 网关192.168.63.2

    PRINT("SoftAP try enable.\r\n");

    (void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));             // 复制SSID
    (void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN); // 复制密码

    hapd_conf.security_type = WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX; // WPA/WPA2混合安全模式
    hapd_conf.channel_num = 6;                              // 信道6

    /* 启动SoftAp接口 */
    if (wifi_softap_enable(&hapd_conf) != ERRCODE_SUCC) {
        PRINT("softap enable fail.\r\n");
        return ERRCODE_FAIL;
    }

    /* 配置DHCP服务器 */
    netif_p = netif_find(ifname);
    if (netif_p == NULL) {
        PRINT("find netif fail.\r\n", ifname);
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != ERR_OK) {
        PRINT("set addr() fail.\r\n");
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != ERR_OK) {
        PRINT("dhcps start() fail.\r\n");
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    PRINT("SoftAp start success.\r\n");
    return ERRCODE_SUCC;
}

static int demo_init(const char *arg)
{
    unused(arg);

    (void)osal_msleep(5000); /* 5000延时5s等待WiFi初始化 */
    /* 启动SoftAP功能和DHCP服务 */
    if (example_softap_function() != ERRCODE_SUCC) {
        return -1;
    }
    return 0;
}

static void test_entry(void)
{
    uint32_t ret;
    osal_task *taskid_sensor, *taskid_tricolored, *task_handle;
    // 创建任务调度锁
    osal_kthread_lock();
    // 创建传感器测试任务
    taskid_sensor = osal_kthread_create((osal_kthread_handler)auto_test, NULL, "auto_test", SENSOR_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid_sensor, SENSOR_TASK_PRIO); // 设置传感器任务优先级
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    // 创建三色灯任务
    taskid_tricolored = osal_kthread_create((osal_kthread_handler)tricolored_task, NULL, "TricoloredTask", TRICOLORED_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid_tricolored, TRICOLORED_TASK_PRIO); // 设置三色灯任务优先级
    if (ret != OSAL_SUCCESS) {
        printf("create task2 failed .\n");
    }
    // 创建WiFi SoftAP任务
    task_handle = osal_kthread_create((osal_kthread_handler)demo_init, 0, "WiFiSoftAPTask", WIFI_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WIFI_SOFTAP_TASK_PRIO); // 设置SoftAP任务优先级
        osal_kfree(task_handle);
    }
    osal_kthread_unlock(); // 解锁任务调度
}

/* Run the blinky_entry. */
app_run(test_entry);
