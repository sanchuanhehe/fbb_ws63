/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BREATHING_LIGHT Sample Source. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */
#if defined(CONFIG_PWM_SUPPORT_LPM)
#include "pm_veto.h"
#endif
#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"
#include "math.h"

#define DELAY_1000MS 1000
#define BREATHING_LIGHT_TASK_PRIO 24
#define WS2812B_TASK_STACK_SIZE 0x1000
#define WS2812B_CNT 8
#define PI 3.1415926535f
#define T1H 67                                    // 1码高电平占67%的周期数
#define T1L 33                                    // 1码低电平占33%的周期数
#define T0H 33                                    // 0码高电平占33%的周期数
#define T0L 67                                    // 0码低电平占67%的周期数
#define OFFSET_INCREMENT 1                        // 每次滚动的增量
#define MAX_LED_OFFSET (65535 - OFFSET_INCREMENT) // 防止溢出的阈值

// PWM周期 = g_PwmCycle*0.0125   eg. 100*0.0125=1.25us 即频率800K

uint32_t led_colors[8] = {0};
#ifdef CONFIG_PWM_USING_V151
uint8_t channelId = CONFIG_WS2812B_CHANNEL;
#endif

void generate_rainbow_colors(uint32_t *colors, uint16_t time_counter)
{
    const uint8_t max_brightness = 100; // 适当减小亮度，提高柔和度
    const float hue_step = 360.0f / WS2812B_CNT;

    //  防止计数器溢出
    uint16_t safe_counter = time_counter % 60000;
    // 计算波状偏移（让 LED 像波浪流动）  多个波叠加;
    float wave_offset = sin(safe_counter * 0.08f) * 50.0f + cos(safe_counter * 0.08f) * 30.0f;

    for (int i = 0; i < WS2812B_CNT; i++) {
        uint16_t hue = (uint16_t)(i * hue_step + safe_counter + wave_offset) % 360;
        float h = fmod(hue / 60.0f, 6.0f); // 标准化h范围

        // HSV→RGB转换
        uint8_t r;
        uint8_t g;
        uint8_t b;
        int sector = (int)h; // 用于表示 色相（Hue）所处的六分之一区间
        float f = h - sector;
        float p = max_brightness * (1 - 1.0f); // 固定饱和度
        float q = max_brightness * (1 - 1.0f * f);
        float t = max_brightness * (1 - 1.0f * (1 - f));

        switch (sector) {
            case 0: // 0:色相第一个扇区 红色 → 黄色（红满，绿递增）
                r = max_brightness;
                g = t;
                b = p;
                break;
            case 1: // 1:色相第二个扇区 黄色 → 绿色（绿满，红递减）
                r = q;
                g = max_brightness;
                b = p;
                break;
            case 2: // 2:色相第三个扇区 绿色 → 青色（绿满，蓝递增）
                r = p;
                g = max_brightness;
                b = t;
                break;
            case 3: // 3:色相第四个扇区 青色 → 蓝色（蓝满，绿递减）
                r = p;
                g = q;
                b = max_brightness;
                break;
            case 4: // 4:色相第五个扇区 蓝色 → 品红（蓝满，红递增）
                r = t;
                g = p;
                b = max_brightness;
                break;
            default: // 5:色相第六个扇区 品红 → 红色（红满，蓝递减）
                r = max_brightness;
                g = p;
                b = q;
                break;
        }

        // 限制动态亮度范围  动态亮度调整（让灯光更像呼吸/霓虹效果）
        uint8_t brightness = fmaxf(0, fminf(255, max_brightness + sin(safe_counter * 0.15f + i * 0.5f) * 30));

        // 颜色计算（Gamma校正） 255:满量程值;16:左移16位;8:左移8位;
        colors[i] = (((r * brightness) / 255) << 16) | (((g * brightness) / 255) << 8) | ((b * brightness) / 255);
    }
}
void ws2812b_send_color_8(uint32_t *colors)
{
    // 定义PWM配置
    pwm_config_t pwm_cfg;
    pwm_cfg.offset_time = 0;
    pwm_cfg.cycles = 1; // 只输出一个波形
    pwm_cfg.repeat = false;

    // 将8个LED的RGB颜色转换为GRB格式并合并为一个192位的数据流
    uint32_t grb_data[8]; // 存储转换后的GRB数据

    // 首先转换所有颜色到GRB格式
    for (int i = 0; i < WS2812B_CNT; i++) {
        uint8_t r = (colors[i] >> 16) & 0xFF; // 红色
        uint8_t g = (colors[i] >> 8) & 0xFF;  // 绿色
        uint8_t b = colors[i] & 0xFF;         // 蓝色
        // 16:左移16位;8:左移8位;
        grb_data[i] = (g << 16) | (r << 8) | b; // GRB格式
    }
    // 发送所有LED的数据 (数据顺序是第一个LED到最后一个LED)
    for (int led = 0; led < WS2812B_CNT; led++) {
        for (int bit = 23; bit >= 0; bit--) { // 每个LED 24位数据
            if (grb_data[led] & (1 << bit)) {
                // 发送1码
                pwm_cfg.high_time = T1H;
                pwm_cfg.low_time = T1L;
            } else {
                // 发送0码
                pwm_cfg.high_time = T0H;
                pwm_cfg.low_time = T0L;
            }
            uapi_pwm_open(CONFIG_WS2812B_CHANNEL, &pwm_cfg);
            uapi_pwm_start_group(CONFIG_WS2812B_GROUP_ID);
        }
    }
    // WS2812B需要RESET信号 (至少50μs的低电平)
    pwm_cfg.high_time = 1;
    pwm_cfg.low_time = 40; // 50μs / (1.25μs) = 40，但为了保险取更大值
    uapi_pwm_open(CONFIG_WS2812B_CHANNEL, &pwm_cfg);
    uapi_pwm_start_group(CONFIG_WS2812B_GROUP_ID);
}
// WS2812B颜色控制函数
void ws2812b_send_color(uint32_t color)
{
    // WS2812B要求数据格式是GRB，不是RGB
    uint8_t r = (color >> 16) & 0xFF; // 绿色
    uint8_t g = (color >> 8) & 0xFF;  // 红色
    uint8_t b = color & 0xFF;         // 蓝色

    uint32_t grb = (g << 16) | (r << 8) | b; // 组合成GRB格式
    pwm_config_t pwm_cfg;
    pwm_cfg.offset_time = 0;
    pwm_cfg.cycles = 1; // 只输出一个波形
    pwm_cfg.repeat = false;

    // 发送24位数据 (GRB各8位)
    for (int i = 23; i >= 0; i--) {
        if (grb & (1 << i)) {
            // 发送1码
            pwm_cfg.high_time = T1H;
            pwm_cfg.low_time = T1L;
        } else {
            // 发送0码
            pwm_cfg.high_time = T0H;
            pwm_cfg.low_time = T0L;
        }

        uapi_pwm_open(CONFIG_WS2812B_CHANNEL, &pwm_cfg);
        uapi_pwm_start_group(CONFIG_WS2812B_GROUP_ID);
    }
    // WS2812B需要RESET信号 (至少50μs的低电平)
    // 可以通过设置PWM为低电平来实现
    pwm_cfg.high_time = 1;
    pwm_cfg.low_time = 40; // 50μs / (1.25μs) = 40, 但为了保险取更大值
    uapi_pwm_open(CONFIG_WS2812B_CHANNEL, &pwm_cfg);
    uapi_pwm_start_group(CONFIG_WS2812B_GROUP_ID);
}
static void *ws2812b_task(const char *arg)
{
    UNUSED(arg);
    uint16_t offset = 0;

    // 指定管脚 若修改则需查询（GPIO复用管脚）修改复用模式和PWM通道

    uapi_pin_set_mode(CONFIG_WS2812B_PIN, CONFIG_WS2812B_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();

    uapi_pwm_set_group(CONFIG_WS2812B_GROUP_ID, &channelId, 1);

    // 单色

    ws2812b_send_color(0xffffff); // 白色
    osal_msleep(DELAY_1000MS);
    ws2812b_send_color(0xff0000); // 红色
    osal_msleep(DELAY_1000MS);
    ws2812b_send_color(0x00ff00); // 绿色
    osal_msleep(DELAY_1000MS);
    ws2812b_send_color(0x0000ff); // 蓝色
    osal_msleep(DELAY_1000MS);
    ws2812b_send_color_8(led_colors); // 全部关闭
    osal_printk("Color RGB start.\r\n");
    while (1) {
        generate_rainbow_colors(led_colors, offset); // 生成带有当前偏移的彩虹色
        ws2812b_send_color_8(led_colors);            // 发送颜色到LED

        if (offset >= MAX_LED_OFFSET) { // 增加偏移，实现滚动效果
            offset = 0;
        }
        offset += OFFSET_INCREMENT;
        osal_msleep(10); // 10:延时10ms;
    }

#ifdef CONFIG_PWM_USING_V151
    uapi_pwm_close(CONFIG_WS2812B_GROUP_ID);
#else
    uapi_pwm_close(CONFIG_WS2812B_CHANNEL);
#endif

    uapi_tcxo_delay_ms(DELAY_1000MS);
    uapi_pwm_deinit();
    return NULL;
}

static void ws2812b_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)ws2812b_task, 0, "Ws2812btTask", WS2812B_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BREATHING_LIGHT_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(ws2812b_entry);