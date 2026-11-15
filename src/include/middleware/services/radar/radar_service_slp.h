/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file of slp radar
 */

/**
 * @defgroup middleware_service_radar_slp Radar SLP API
 * @ingroup middleware_service_radar
 * @{
 */

#ifndef RADAR_SERVICE_SLP_H
#define RADAR_SERVICE_SLP_H

#include <stdint.h>
#include "radar_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RADAR_MAX_ANT_CH_NUM 8 /* 最大天线对个数 */

#ifdef CONFIG_RADAR_HW_SPEC_ENABLE
#define RADAR_MAX_RPT_TARGET_NUM 1 /* 最大目标个数 */
#else
#define RADAR_MAX_RPT_TARGET_NUM 3 /* 最大目标个数 */
#endif

#define RADAR_MAX_WAVE_LEN 64     /* 波形最大长度 */
#define RADAR_HW_PARA_RSV_BYTE 12 /* 预留长度 */
#define RADAR_ALG_PARA_RSV_BYTE 8 /* 预留长度 */
#define RADAR_MAX_SENSITIVITY_ARR_LEN 60 /* 灵敏度数组最大长度 */

#define RADAR_AI_OFFSET_PARA_NUM 8 /* AI补偿参数个数 */

#define RADAR_VENDOR_INFO_BYTE 2 /* 预留长度 */

#pragma pack(1)

/**
 * @if Eng

 * @else
 * @brief 芯片射频开关控制码字共用体
 * @endif
 */
typedef union {
    struct {
        uint8_t ctrl0 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 0, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 0 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t ctrl1 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 1, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 1 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t ctrl2 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 2, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 2 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t ctrl3 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 3, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 3 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t ctrl4 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 4, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 4 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t ctrl5 : 1;   /*!< @if Eng Enable or disable the function of antenna control pin 5, 0:dis, 1:en
                                @else 开启或关闭天线控制管脚 5 的对应功能，0:关闭, 1:开启 @endif */
        uint8_t reserve : 2; /*!< @if Eng reserve
                                @else 保留比特位 @endif */
    } bits;                  /*!< @if Eng Bit field structure
                                @else 位域结构体 @endif */
    uint8_t u8;              /*!< @if Eng 8-bit unsigned integer
                                @else 8位无符号整数 @endif */
} radar_rf_sw_bit;

/**
 * @if Eng
 * @brief  radar AGC parameter.
 * @else
 * @brief  雷达AGC参数。
 * @endif
 */
typedef struct {
    uint8_t lna_code[RADAR_MAX_ANT_CH_NUM]; /*!< @if Eng radar AGC parameter: lna code
                                                @else   雷达AGC参数：LNA码字 @endif */
    uint8_t vga_code[RADAR_MAX_ANT_CH_NUM]; /*!< @if Eng radar AGC parameter: vga code
                                                @else   雷达AGC参数：VGA码字 @endif */
} radar_agc_para_t;

/**
 * @if Eng
 * @brief  radar wave parameter.
 * @else
 * @brief  雷达波形参数。
 * @endif
 */
typedef struct {
    uint8_t spread_factor;                   /*!< @if Eng radar wave parameter: Spreading Factor
                                                    @else   雷达波形参数：波形序列扩频因子 @endif */
    uint8_t acc_rshift_bit;                  /*!< @if Eng radar wave parameter: Accumulation right shift
                                                    @else   雷达波形参数：累加位宽保护右移位数 @endif */
    uint16_t acc_num;                        /*!< @if Eng radar wave parameter: Accumulation Number
                                                    @else   雷达波形参数：累加次数 @endif */
    uint8_t corr_div;                        /*!< @if Eng radar wave parameter: Correlation Divisor
                                                    @else   雷达波形参数：相关后除数因子 @endif */
    uint8_t tx_wave_len;                     /*!< @if Eng radar wave parameter: tx wave len
                                                    @else   雷达波形参数：发射波形长度 @endif */
    int8_t tx_wave_bits[RADAR_MAX_WAVE_LEN]; /*!< @if Eng radar wave parameter: tx wave bits
                                                    @else   雷达波形参数：波形扩频前的三元码序列 @endif */
} radar_wave_para_t;

/**
 * @if Eng
 * @brief  radar frame parameter.
 * @else
 * @brief  雷达帧参数。
 * @endif
 */
typedef struct {
    uint16_t subframe_period;     /*!< @if Eng radar frame parameter: sub frame period
                                                    @else   雷达帧参数：子帧间隔，单位0.01ms @endif */
    uint16_t ant_switch_interval; /*!< @if Eng radar frame parameter: antenna switch interval
                                                    @else   雷达帧参数：天线切换间隔，单位0.01ms @endif */
} radar_frame_para_t;

/**
 * @if Eng
 * @brief  radar raw data parameter.
 * @else
 * @brief  原始数据参数。
 * @endif
 */
typedef struct {
    uint8_t start_bin; /*!< @if Eng radar raw data parameter: start bin
                                                    @else   雷达原始数据参数：起始bin索引 @endif */
    uint8_t end_bin;   /*!< @if Eng radar raw data parameter: end bin
                                                    @else   雷达原始数据参数：终止bin索引 @endif */
} radar_raw_data_para_t;

/**
 * @if Eng
 * @brief  complex short data structure.
 * @else
 * @brief  复数数据结构体。
 * @endif
 */
typedef struct {
    int16_t real; /*!< @if Eng complex short data: real part
                                                    @else   复数数据：实部 @endif */
    int16_t imag; /*!< @if Eng complex short data: imaginary part
                                                    @else   复数数据：虚部 @endif */
} complex_short_t;

/**
 * @if Eng
 * @brief  power on/off status set.
 * @else
 * @brief  芯片上下电状态配置。
 * @endif
 */
typedef enum {
    CHIP_STATUS_POWEROFF = 0, /*!< @if Eng chip power off
                                                    @else   芯片下电 @endif */
    CHIP_STATUS_POWERON,      /*!< @if Eng chip power on
                                                    @else   芯片上电 @endif */
} radar_set_power_status_t;

/**
 * @if Eng
 * @brief  radar status set.
 * @else
 * @brief  雷达状态配置。
 * @endif
 */
typedef enum {
    RADAR_STATUS_STOP = 0, /*!< @if Eng radar status set: stop
                                                    @else   雷达状态配置停止 @endif */
    RADAR_STATUS_START,    /*!< @if Eng radar status set: start
                                                    @else   雷达状态配置启动 @endif */
} radar_set_status_t;

/**
 * @if Eng
 * @brief  radar status.
 * @else
 * @brief  雷达状态。
 * @endif
 */
typedef enum {
    RADAR_STATUS_RADAR_ON,     /*!< @if Eng radar status: radar on
                                                    @else   雷达使能状态 @endif */
    RADAR_STATUS_RADAR_OFF     /*!< @if Eng radar status: radar off
                                                    @else   雷达关闭状态 @endif */
} radar_get_status_t;

/**
 * @if Eng
 * @brief  radar frame mode.
 * @else
 * @brief  雷达帧模式。
 * @endif
 */
typedef enum {
    RADAR_FRAME_MODE_RANGING,               /*!< @if Eng radar frame mode: SLP ranging coexistence frame mode
                                                @else   SLP测距共存帧模式 @endif */
    RADAR_FRAME_MODE_DEFAULT,               /*!< @if Eng radar frame mode: single radar frame mode
                                                @else   单雷达帧模式 @endif */
    RADAR_FRAME_MODE_VENDOR,                /*!< @if Eng radar frame mode: vendor frame mode
                                                @else   单雷达模式：厂商帧模式 @endif */
} radar_frame_mode_t;

/**
 * @if Eng
 * @brief  radar anti multipath mode.
 * @else
 * @brief  雷达抗多径虚警模式。
 * @endif
 */
typedef enum {
    RADAR_ANTI_MULTIPATH_DEFAULT_MODE,      /*!< @if Eng radar anti multipath mode: default mode
                                                @else   抗多径虚警默认模式 @endif */
    RADAR_ANTI_MULTIPATH_STRENGTHEN_MODE,   /*!< @if Eng radar anti multipath mode: strengthen mode
                                                @else   抗多径虚警能力强化模式 @endif */
    RADAR_ANTI_MULTIPATH_WEAKEN_MODE,       /*!< @if radar anti multipath mode: weaken mode
                                                @else   抗多径虚警能力弱化模式 @endif */
} radar_anti_multipath_mode_t;

/**
 * @if Eng
 * @brief  radar cali res save mode.
 * @else
 * @brief  雷达校准参数保存模式。
 * @endif
 */
typedef enum {
    RADAR_CALI_RES_SAVE_IN_NV,              /*!< @if Eng radar cali res save mode: save in host nv
                                                @else   雷达校准参数存储在host NV中 @endif */
    RADAR_CALI_RES_SAVE_IN_EEPROM,          /*!< @if Eng radar cali res save mode: save in device eeprom
                                                @else   雷达校准参数存储在device EEPROM中 @endif */
} radar_cali_res_save_mode_t;

/**
 * @if Eng
 * @brief  radar report mode.
 * @else
 * @brief  雷达数据上报模式。
 * @endif
 */
typedef enum {
    RADAR_REPORT_RES = 0,                   /*!< @if Eng radar report mode: only report detection result
                                                @else   只上报检测结果 @endif */
    RADAR_REPORT_SLEEP_RAW_DATA = 1,        /*!< @if Eng radar report mode: report both detection result and sleep raw data
                                                @else   同时上报检测结果和睡眠原始数据 @endif */
} radar_report_mode_t;

/**
 * @if Eng
 * @brief  radar target detection result.
 * @else
 * @brief  雷达目标检测结果信息。
 * @endif
 */
typedef struct {
    uint16_t id; /*!< @if Eng radar detection res: target id
                                                    @else   雷达检测结果：目标序号 @endif */
    int16_t rng; /*!< @if Eng radar detection res: target range(cm)
                                                    @else   雷达检测结果：目标距离(厘米） @endif */
    int16_t agl; /*!< @if Eng radar detection res: target angle(0.01°)
                                                    @else   雷达检测结果：目标角度(0.01度) @endif */
    int16_t vel; /*!< @if Eng radar detection res: target velocity(cm/s)
                                                    @else   雷达检测结果：目标速度(厘米/秒) @endif */
    uint16_t activity_intensity;    /*!< @if Eng radar detection res: activity intensity
                                        @else   雷达检测结果：目标活动强度 @endif */
    uint16_t reserve0;              /*!< @if Eng radar raw data msg: reserve0
                                        @else    雷达数据：保留字段0 @endif */
    uint16_t reserve1;              /*!< @if Eng radar raw data msg: reserve1
                                        @else    雷达数据：保留字段1 @endif */
} radar_target_info_t;

/**
 * @if Eng
 * @brief  radar results.
 * @else
 * @brief  雷达检测结果。
 * @endif
 */
typedef struct {
    uint8_t target_num;                                        /*!< @if Eng radar detection res: valid target num
                                                                    @else   雷达检测结果：有效目标个数 @endif */
    radar_target_info_t target_info[RADAR_MAX_RPT_TARGET_NUM]; /*!< @if Eng radar res: target detection results
                                                                    @else   雷达检测结果：目标检测结果数组 @endif */
} radar_result_t;

/**
 * @if Eng
 * @brief  report struct of radar raw data.
 * @else
 * @brief  原始数据上报的结构体, 注意data与结构体内存不连续，不能直接memcpy整个结构体
 * @attention data 数据格式, 按照距离bin数目，I,Q顺序存放
 * @endif
 */
typedef struct {
    uint16_t data_len;                                   /*!< @if Eng radar raw data msg: data len
                                                              @else    雷达数据：数据长度 @endif */
    uint16_t crc;                                        /*!< @if Eng radar raw data msg: crc value
                                                              @else    雷达数据：数据crc校验值 @endif */
    uint16_t counter;                                    /*!< @if Eng radar raw frame count: crc value
                                                              @else    雷达数据：数据上报帧号 @endif */
    uint64_t tick_cnt;                                   /*!< @if Eng radar raw data msg: tick cnt
                                                              @else   雷达数据：数据帧32k时钟计数 @endif */
    radar_raw_data_para_t data_para;                     /*!< @if Eng radar raw data msg: data para
                                                              @else    雷达数据: 数据参数 @endif */
    radar_frame_mode_t mode;                               /*!< @if Eng radar raw data msg: radar frame mode
                                                              @else    雷达数据：雷达帧模式 @endif */
    uint8_t vendor_info[RADAR_VENDOR_INFO_BYTE];         /*!< @if Eng radar raw data msg: vendor infomation
                                                              @else    雷达数据：vendor参数 @endif */
    uint64_t reserve;                                    /*!< @if Eng radar raw data msg: reserve
                                                              @else    雷达数据：保留字段 @endif */
    complex_short_t *data;                               /*!< @if Eng radar raw data msg: data pointer
                                                              @else    雷达数据：数据指针 @endif */
} radar_raw_data_msg_t;

/**
 * @if Eng
 * @brief  radar RF parameter.
 * @else
 * @brief  雷达射频参数。
 * @endif
 */
typedef struct {
    bool pwr_ctrl;                              /*!< @if Eng radar RF parameter: power control
                                                    @else   雷达射频参数：射频开关电源是否独立控制 @endif */
    uint8_t ch_idx;                             /*!< @if Eng radar RF parameter: channel number
                                                    @else   雷达射频参数：信道号索引 @endif */
    uint8_t ant_ch_num;                         /*!< @if Eng radar RF parameter: antenna channel num
                                                    @else   雷达射频参数：有效天线通道个数 @endif */
    uint8_t tx_power_idx[RADAR_MAX_ANT_CH_NUM]; /*!< @if Eng radar RF parameter: tx power index
                                                    @else   雷达射频参数：Tx发射功率索引 @endif */
    uint8_t ant_code[RADAR_MAX_ANT_CH_NUM];     /*!< @if Eng radar RF parameter: antenna code
                                                    @else   雷达射频参数：天线码字 @endif */
    uint32_t board_id;                          /*!< @if Eng radar hardware parameter: board id
                                                    @else   雷达硬件参数：模组型号 @endif */
    radar_agc_para_t agc_para;                  /*!< @if Eng radar RF parameter: agc parameter
                                                    @else   雷达射频参数：AGC参数 @endif */
    radar_rf_sw_bit ant_sw_ctrl_en;             /*!< @if Eng radar RF parameter: antenna switch control enable
                                                    @else   雷达射频参数：射频开关控制管脚使能  @endif */
} radar_rf_para_t;

/**
 * @if Eng
 * @brief  radar hardware parameter.
 * @else
 * @brief  雷达硬件参数。
 * @endif
 */
typedef struct {
    radar_rf_para_t rf_para;                            /*!< @if Eng radar hardware parameter: raw data parameter
                                                             @else  雷达硬件参数：射频参数 @see radar_rf_para_t @endif */
    radar_frame_para_t frame_para;                      /*!< @if Eng radar hardware parameter: raw data parameter
                                                             @else  雷达硬件参数：帧参数 @see radar_frame_para_t @endif */
    radar_raw_data_para_t raw_data_para;                /*!< @if Eng radar hardware parameter: raw data parameter
                                                             @else  雷达硬件参数：原始数据上报配置参数 @see radar_raw_data_para_t @endif */
    radar_wave_para_t wave_para;                        /*!< @if Eng radar hardware parameter: wave parameter
                                                             @else  雷达硬件参数：波形参数 @see radar_wave_para_t @endif */
    uint8_t vendor_info[RADAR_VENDOR_INFO_BYTE];        /*!< @if Eng radar hardware parameter: vendor infomation
                                                             @else  雷达硬件参数：vendor参数 @see vendor_info @endif */
    uint8_t reserve[RADAR_HW_PARA_RSV_BYTE];            /*!< @if Eng radar hardware parameter: reserve parameter
                                                             @else  雷达硬件参数：预留位 @see reserve @endif */
} radar_hardware_para_t;

/**
 * @if Eng
 * @brief  radar algorithm basic parameter.
 * @else
 * @brief  雷达算法基础参数。
 * @endif
 */
typedef struct {
    uint32_t rpt_mode;      /*!< @if Eng radar algorithm basic parameter: report mode. default is 0.
                                @else   雷达算法基础参数: 数据上报类型，默认值为0 see @ref radar_report_mode_t @endif */
    uint32_t sensitivity;   /*!< @if Eng radar algorithm basic parameter: detection sensitivity. default is 0.
                                @else   雷达算法基础参数: 检测灵敏度, 默认值为0 @endif */
    uint32_t rng_boundary;  /*!< @if Eng radar algorithm basic parameter: detection range boundary. uint cm, default is 700.
                                @else   雷达算法基础参数: 距离检测边界，单位cm，默认值700 @endif */
    uint32_t delay_time;    /*!< @if Eng radar algorithm basic parameter: delay time. uint s, default is 20.
                                @else   雷达算法基础参数: 退出时延，单位S，默认值20 @endif */
    uint16_t ant_space_amp_coef;    /*!< @if Eng radar algorithm basic parameter: antena spce amplify coefficient, default is 10000.
                                        @else   雷达算法基础参数: 天线间距缩放系数，默认值10000 @endif */
    uint16_t static_create_range;   /*!< @if Eng radar algorithm basic parameter: static target creation range threshold. uint cm, default is 0.
                                        @else   雷达算法基础参数: 静目标允许创建的距离门限。单位cm，默认为0。@endif */
    uint8_t is_wire_down;   /*!< @if Eng radar algorithm basic parameter: is terminal wire heading down or up. 1 for down, 0 for up.
                                @else   雷达算法基础参数: 端子线朝向，影响角度上报值的正负。1表示朝下，0表示朝上 @endif */
    uint8_t angle_boundary; /*!< @if Eng radar algorithm parameter: detection angle boundary. uint degree, default is 60.
                                @else   雷达算法基础参数: 角度检测边界，单位°，默认值60 @endif */
    uint8_t anti_multipath_mode; /*!< @if Eng radar algorithm parameter: anti-multipath mode. default is 0. see @ref radar_anti_multipath_mode_t.
                                    @else   雷达算法基础参数: 抗多径虚警模式。默认值0 @endif */
    uint8_t cali_res_save_mode; /*!< @if Eng radar algorithm parameter: cali result save mode. default is 0. see @ref radar_cali_res_save_mode_t.
                                    @else   雷达算法基础参数: 校准参数存储模式，默认值为0，表示存入Host芯片的NV中。@endif */
} radar_alg_basic_para_t;

/**
 * @if Eng
 * @brief  radar AI parameter.
 * @else
 * @brief  雷达AI参数。
 * @endif
 */
typedef struct {
    int32_t dynamic_offset_direction[RADAR_AI_OFFSET_PARA_NUM]; /*!< @if Eng radar ai parameter: AI offset parameters for dynamic detection.
                                                                     ranging from -100000 to 100000, default is 0.
                                                            @else   雷达AI参数: AI动检测补偿参数。
                                                                    取值范围[-100000, 100000]，默认值0。 @endif */
    int32_t static_offset_direction[RADAR_AI_OFFSET_PARA_NUM]; /*!< @if Eng radar ai parameter: AI offset parameters for static detection.
                                                                     ranging from -100000 to 100000, default is 0.
                                                            @else   雷达AI参数: AI静检测补偿参数。
                                                                    取值范围[-100000, 100000]，默认值0。 @endif */
    uint16_t ai_status;       /*!< @if Eng radar ai parameter: ai anti-interference function switch.
                                    0 for off, 1 for on. default is 1.
                                 @else   雷达AI参数: AI抗干扰功能开关. 0表示关闭，1表示打开。默认是1  @endif */
} radar_ai_para_t;

/**
 * @if Eng
 * @brief  radar algorithm detection sensitivity parameter.
 * @else
 * @brief  雷达算法检测灵敏度参数。
 * @endif
 */
typedef struct {
    uint8_t para_len;                                           /*!< @if Eng radar sensitivity parameter: sensitivity parameter array length.
                                                                     @else   雷达检测灵敏度参数: 灵敏度参数长度 @endif */
    uint8_t dyn_sensitivity_0[RADAR_MAX_SENSITIVITY_ARR_LEN];   /*!< @if Eng radar sensitivity parameter: dynamic detection sensitivity_0 parameters.
                                                                     @else   雷达检测灵敏度参数: 动目标检测灵敏度参数0 @endif */
    uint8_t dyn_sensitivity_1[RADAR_MAX_SENSITIVITY_ARR_LEN];   /*!< @if Eng radar sensitivity parameter: dynamic detection sensitivity_1 parameters.
                                                                     @else   雷达检测灵敏度参数: 动目标检测灵敏度参数1 @endif */
    uint8_t sta_sensitivity_0[RADAR_MAX_SENSITIVITY_ARR_LEN];   /*!< @if Eng radar sensitivity parameter: static detection sensitivity_0 parameters.
                                                                     @else   雷达检测灵敏度参数: 静目标检测灵敏度参数0 @endif */
} radar_sensitivity_para_t;

/**
 * @if Eng
 * @brief  radar algorithm parameter.
 * @else
 * @brief  雷达算法参数。
 * @endif
 */
typedef struct {
    radar_alg_basic_para_t basic_para;          /*!< @if Eng radar algorithm parameter: basic parameter
                                                        @else   雷达算法参数: 基础参数 @endif */
    radar_ai_para_t ai_para;                    /*!< @if Eng radar algorithm parameter: ai parameter
                                                        @else   雷达算法参数: AI补偿参数 @endif */
    radar_sensitivity_para_t sensitivity_para;  /*!< @if Eng radar algorithm parameter: sensitivity parameter
                                                        @else   雷达算法参数: 灵敏度参数 @endif */
    uint8_t reserve[RADAR_ALG_PARA_RSV_BYTE];   /*!< @if Eng radar alg parameter: reserve parameter
                                                        @else   雷达算法参数：预留位 @endif */
} radar_alg_para_t;

/**
 * @if Eng
 * @brief Callback invoked when the radar detection is complete.
 * @par Description: Callback invoked when the radar detection is complete.

 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] result radar_result_t.
 * @else
 * @brief  雷达检测完成的结果回调函数。
 * @par Description: 雷达检测完成的结果回调函数。

 * @param [in] result 雷达检测结果。
 * @endif
 */
typedef void (*radar_result_cb_t)(const radar_result_t *result);

/**
 * @if Eng
 * @brief Callback invoked when power on/off process is complete.
 * @par Description: Callback invoked when power on/off process is complete.
 * @attention This callback function runs on the radar client thread.
 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] status status of chip power, see @ref radar_set_power_status_t.
 * @param [in] err_code errcode_radar_client_t
 * @else
 * @brief  设置芯片上下电状态的回调函数。
 * @par Description: 设置芯片上下电状态的回调函数。
 * @attention  该回调函数运行于radar client线程, 不能阻塞或长时间等待, 不能使用较大栈空间。
 * @param [in] status 设置的芯片上下电状态，0表示下电，1表示上电。
 * @param [in] err_code 错误码
 * @endif
 */
typedef void (*radar_set_power_status_cb_t)(uint8_t status, errcode_radar_client_t err_code);

/**
 * @if Eng
 * @brief Callback invoked when the radar detection is complete.
 * @par Description: Callback invoked when the radar detection is complete.

 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] err_code errcode_radar_client_t
 * @else
 * @brief  雷达错误码上报的回调函数。
 * @par Description: 雷达错误码上报的回调函数。

 * @param [in] err_code 错误码
 * @endif
 */
typedef void (*radar_report_errcode_cb_t)(errcode_radar_client_t err_code);

/**
 * @if Eng
 * @brief Callback invoked to obtain the raw data collected by the radar.
 * @par Description: Callback invoked to obtain the raw data collected by the radar.

 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] dataMsg The starting address of the radar raw data array.
 * @else
 * @brief  获取雷达原始数据的回调函数。
 * @par Description: 获取雷达原始数据的回调函数。

 * @param [in] dataMsg 雷达原始数据数组的首地址。
 * @endif
 */
typedef void (*radar_raw_data_cb_t)(radar_raw_data_msg_t *dataMsg);

/**
 * @if Eng
 * @brief  Radar set hardware parameter.
 * @par Description: Radar set hardware parameter.
 * @param [in] para see @ref radar_hardware_para_t.
 * @retval error code.
 * @else
 * @brief  设置雷达硬件参数。
 * @par Description: 设置雷达硬件参数。
 * @param [in] para 雷达硬件参数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_set_hardware_para(radar_hardware_para_t *para);

/**
 * @if Eng
 * @brief  Radar set algorithm parameter.
 * @par Description: Radar set algorithm parameter.
 * @param [in] para see @ref radar_alg_para_t.
 * @param [in] is_write_to_nv: whether write input parameters into nv or not.
 * @retval error code.
 * @else
 * @brief  设置雷达算法参数。
 * @par Description: 设置雷达算法参数。
 * @param [in] para 雷达算法参数。
 * @param [in] is_write_to_nv 是否将参数写入NV。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_set_alg_para(radar_alg_para_t *para, uint8_t is_write_to_nv);

/**
 * @if Eng
 * @brief  Radar result callback registration function.
 * @par Description: Radar result callback registration function.
 * @param [in] cb radar_result_cb_t.
 * @retval error code.
 * @else
 * @brief  雷达检测结果回调注册函数。
 * @par Description: 雷达检测结果回调注册函数。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_register_result_cb(radar_result_cb_t cb);

/**
 * @if Eng
 * @brief  Radar raw data callback registration function.
 * @par Description: Radar raw data callback registration function.
 * @param [in] cb radar_raw_data_cb_t.
 * @retval error code.
 * @else
 * @brief  获取雷达原始数据。
 * @par Description: 获取雷达原始数据。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_register_raw_data_cb(radar_raw_data_cb_t cb);

/**
 * @if Eng
 * @brief  Get radar detection result.
 * @par Description: Get radar etection result.
 * @param [in] result: radar_result_t.
 * @retval error code.
 * @else
 * @brief  获取雷达检测结果。
 * @par Description: 获取雷达检测结果。
 * @param [in] result 雷达检测结果。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_get_result(radar_result_t *result);

/**
 * @if Eng
 * @brief  set chip power status callback registration function.
 * @par Description: set chip power status callback registration function.
 * @param [in] cb radar_set_power_status_cb_t.
 * @retval error code.
 * @else
 * @brief  设置芯片上下电状态回调注册函数。
 * @par Description: 设置芯片上下电状态回调注册函数。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_register_set_power_status_cb(radar_set_power_status_cb_t cb);

/**
 * @if Eng
 * @brief  Radar report errcode callback registration function.
 * @par Description: Radar report errcode callback registration function.
 * @param [in] cb radar_report_errcode_cb_t.
 * @retval error code.
 * @else
 * @brief  雷达错误码回调注册函数。
 * @par Description: 雷达错误码回调注册函数。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_register_report_errcode_cb(radar_report_errcode_cb_t cb);

/**
 * @if Eng
 * @brief  Set status of radar.
 * @par Description: Set status of radar.
 * @param [in] status status of radar, see @ref radar_set_status_t.
 * @retval error code.
 * @else
 * @brief  设置雷达状态。
 * @par Description: 设置雷达状态。
 * @param [in] status 雷达状态。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_set_status(uint8_t status);

/**
 * @if Eng
 * @brief  chip power on or power off.
 * @par Description: chip power on or power off.
 * @param [in] status of chip power. see @ref radar_set_power_status_t.
 * @retval error code.
 * @else
 * @brief  控制芯片上下电。
 * @par Description: 控制芯片上下电。
 * @param [in] status 芯片上下电状态。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_set_power_status(uint8_t status);

/**
 * @if Eng
 * @brief  get radar status.
 * @par Description: get radar status.
 * @param [in] status of radar. see @ref radar_get_status_t.
 * @retval error code.
 * @else
 * @brief  获取雷达状态。
 * @par Description: 获取雷达状态。
 * @param [in] status 雷达状态
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_get_status(uint8_t *status);

#pragma pack()

#ifdef __cplusplus
}
#endif
#endif

/**
 * @}
 */