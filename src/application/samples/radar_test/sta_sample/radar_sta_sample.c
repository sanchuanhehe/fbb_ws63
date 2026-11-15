/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Radar samples function \n
 *
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "wifi_device.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "radar_service.h"
#include "gpio.h"
#include "pinctrl.h"
#include "app_init.h"

#define RADAR_STACK_SIZE          0x800
#define RADAR_TASK_PRIO           24

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_INIT_WAIT_TIME              500 // 5s
#define WIFI_START_STA_DELAY             100 // 1s

#define RADAR_STATUS_START               1
#define RADAR_STATUS_QUERY_DELAY         1000 // 10s
#define RADAR_QUIT_DELAY_TIME            8 // 8s

#define RADAR_DEFAULT_TIMES 0
#define RADAR_DEFAULT_LOOP 8
#define RADAR_DEFAULT_ANT 0
#define RADAR_DEFAULT_PERIOD 5000
#define RADAR_DEFAULT_DBG_TYPE 3
#define RADAR_DEFAULT_WAVE 2

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

#define RADAR_DBG_INFO_RPT_COEF 1
#define RADAR_DBG_INFO_LEN 16

// led档位控制参数
typedef enum {
    RADAR_INSIDE_1M,
    RADAR_INSIDE_2M,
    RADAR_INSIDE_6M,
} radar_led_gear_t;

radar_led_gear_t g_radar_led_gear = RADAR_INSIDE_1M;

/*****************************************************************************
  STA 扫描-关联 sample用例
*****************************************************************************/
void radar_set_led_gear(radar_led_gear_t gear)
{
    PRINT("[RADAR_SAMPLE] SET LED GEAR:%u!\r\n", gear);
    g_radar_led_gear = gear;
}

static void radar_led_init(void)
{
    // 1. 初始化所有GPIO并设置GPIO的类型
    uapi_gpio_init();
    // 2. 设置GPIO为输出
    errcode_t ret = uapi_gpio_set_dir(GPIO_13, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led uapi_gpio_set_dir failed %u!\r\n", ret);
    }
    // 3. 设置GPIO PIN模式为0，普通模式
    ret = uapi_pin_set_mode(GPIO_13, PIN_MODE_0);
    if (ret != ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led uapi_pin_set_mode failed %u!\r\n", ret);
    }
}

static void radar_set_led_on(void)
{
    errcode_t ret = uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_HIGH);
    if (ret!= ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led ctrl failed %u!\r\n", ret);
    }
}

static void radar_set_led_off(void)
{
    errcode_t ret = uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_LOW);
    if (ret!= ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led ctrl failed %u!\r\n", ret);
    }
}

static void radar_ctrl_led(radar_result_t *res)
{
    switch (g_radar_led_gear) {
        case RADAR_INSIDE_1M:
            if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
            break;
        case RADAR_INSIDE_2M:
            if ((res->lower_boundary == RADAR_API_RANGE_NEAR &&
                 res->upper_boundary == RADAR_API_RANGE_MEDIUM) ||
                (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR)) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
            break;
        default:    // 默认6M档位
            if (res->is_human_presence == 1) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
    }
}

td_s32 radar_start_sta(td_void)
{
    (void)osDelay(WIFI_INIT_WAIT_TIME); /* 500: 延时0.5s, 等待wifi初始化完毕 */
    PRINT("STA try enable.\r\n");
    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        PRINT("sta enbale fail !\r\n");
        return -1;
    }

    /* 连接成功 */
    PRINT("STA enable success.\r\n");
    return 0;
}

static void radar_print_res(radar_result_t *res)
{
    PRINT("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);

    radar_ctrl_led(res);
}

static void radar_print_cur_frame_res(radar_current_frame_result_t *res)
{
    PRINT("[RADAR_SAMPLE] gear:%u, rng:%04d, vel:%04d, snr:%02u\r\n",
        res->gear, res->rng, res->vel, (uint32_t)res->snr);
}

// 维测信息依次为:
// 1.告知上层是否需要写入flash
// 2.LNA * 10 + VGA
// 3.原始回波峰值
// 4.过去period帧的平均MO1底噪
// 5.过去period帧的平均MO2底噪
// 6.过去period帧的平均DP底噪
// 7.过去period帧的平均帧间隔
// 8.过去period帧中帧间隔超过Xms的帧数
// 9.过去period帧中bitmap数量超过X门限的帧数
// 10.过去period帧中bitmap比例超过X门限的帧数
// 11.过去period帧中是在参与统计的帧数
// 12.过去period帧中帧间隔最大值
// 13.过去period帧中帧间隔最大值下标
// 14.当前所使用的算法参数MO1门限
// 15.当前所使用的算法参数MO2门限
// 16.当前所使用的算法参数DP门限
static void radar_print_dbg_info(int16_t *arr, uint8_t len)
{
    if (len > RADAR_DBG_INFO_LEN || len == 0) {
        return;
    }

    PRINT("dbg_info: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
        arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9], arr[10],
        arr[11], arr[12], arr[13], arr[14], arr[15]);
}

static void radar_init_para(void)
{
    // 维测模式设置
    radar_dbg_para_t dbg_para;
    dbg_para.times = RADAR_DEFAULT_TIMES;
    dbg_para.loop = RADAR_DEFAULT_LOOP;
    dbg_para.ant = RADAR_DEFAULT_ANT;
    dbg_para.wave = RADAR_DEFAULT_WAVE;
    dbg_para.dbg_type = RADAR_DEFAULT_DBG_TYPE;
    dbg_para.period = RADAR_DEFAULT_PERIOD;
    uapi_radar_set_debug_para(&dbg_para);
    // 无人档位退出时延设置
    int16_t dly_time = RADAR_QUIT_DELAY_TIME;
    uapi_radar_set_delay_time(dly_time);
    // 算法门限设置
    radar_alg_para_t alg_para;
    alg_para.d_th_1m = 32;
    alg_para.d_th_2m = 25;
    alg_para.p_th = 30;
    alg_para.t_th_1m = 20;
    alg_para.t_th_2m = 35;
    alg_para.b_th_ratio = 10;
    alg_para.b_th_cnt = 4;
    alg_para.a_th = 70;
    alg_para.pt_cld_para_1 = 7;
    alg_para.pt_cld_para_2 = 7;
    alg_para.pt_cld_para_3 = 7;
    alg_para.pt_cld_para_4 = 10;
    alg_para.rd_pwr_para_1 = 6;
    alg_para.rd_pwr_para_2 = 6;
    alg_para.rd_pwr_para_3 = 10;
    uapi_radar_set_alg_para(&alg_para, 0);
}

int radar_demo_init(void *param)
{
    PRINT("[RADAR_SAMPLE] radar_demo_init sta!\r\n");
    param = param;
    radar_led_init();
    radar_start_sta();
    uapi_radar_register_result_cb(radar_print_res); // 注册雷达档位结果回调函数
    uapi_radar_register_current_frame_result_cb(radar_print_cur_frame_res); // 注册雷达单帧结果回调函数
    uapi_radar_register_debug_info_cb(radar_print_dbg_info, RADAR_DBG_INFO_RPT_COEF);  // 注册雷达维测数据回调函数

    // 启动雷达
    (void)osDelay(WIFI_START_STA_DELAY);
    uapi_radar_set_status(RADAR_STATUS_START);
    radar_init_para();

    for (;;) {
        (void)osDelay(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts); // 打印查询到的雷达工作状态
        uint16_t time;
        uapi_radar_get_delay_time(&time); // 打印查询到的无人档位退出时延
        uint16_t iso;
        uapi_radar_get_isolation(&iso); // 打印查询到的隔离度值
        radar_result_t res = {0};
        uapi_radar_get_result(&res); // 打印查询到的结果档位上报结果
        int16_t arr[RADAR_DBG_INFO_LEN] = {0};
        uapi_radar_get_debug_info(arr, RADAR_DBG_INFO_LEN);
        radar_print_dbg_info(arr, RADAR_DBG_INFO_LEN); // 获取维测数据并打印
    }

    return 0;
}

static void radar_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name       = "radar_sample_task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = RADAR_STACK_SIZE;
    attr.priority   = RADAR_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)radar_demo_init, NULL, &attr) == NULL) {
        PRINT("[RADAR_SAMPLE] Create sta_sample_task fail.\r\n");
    }
    PRINT("[RADAR_SAMPLE] Create sta_sample_task succ.\r\n");
}

/* Run the sta_sample_task. */
app_run(radar_sample_entry);
