/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief GPIO button, SK6805-EC20 indicator and NV-backed one-metre RSSI calibration. \n
 * @else
 * @brief GPIO 按键、SK6805-EC20 状态灯及基于 NV 的一米 RSSI 校准。 \n
 * @endif
 */
#include <stdbool.h>
#include <stdint.h>
#include "common_def.h"
#include "gpio.h"
#include "nv.h"
#include "osal_interrupt.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "sle_rssi_ranging_calibration.h"

#define SLE_RSSI_CAL_RSSI_MIN (-100)
#define SLE_RSSI_CAL_RSSI_MAX (-20)
#define SLE_RSSI_CAL_RAW_MIN (-127)
#define SLE_RSSI_CAL_RAW_MAX 20
#define SLE_RSSI_CAL_MAD_MAX 30U
#define SK6805_ONE_HIGH_DELAY 20
#define SK6805_ONE_LOW_DELAY 4
#define SK6805_ZERO_HIGH_DELAY 7
#define SK6805_ZERO_LOW_DELAY 18
/*
 * GPIO and task parameters are fixed for the HiHope WS63E board used by this sample.
 * GPIO 和任务参数对应本案例使用的 HiHope WS63E 开发板。
 */
#define SLE_RSSI_CAL_LOG "[sle rssi cal]"
#define SLE_RSSI_CAL_BUTTON_PIN 13
#define SLE_RSSI_CAL_LED_PIN 5
#define SLE_RSSI_CAL_LED_PIN_MODE 4
#define SLE_RSSI_CAL_BUTTON_POLL_MS 50
#define SLE_RSSI_CAL_BUTTON_DEBOUNCE 3
#define SLE_RSSI_CAL_LONG_PRESS_COUNT 40
#define SLE_RSSI_CAL_SAMPLE_COUNT 31
#define SLE_RSSI_CAL_LED_BLINK_COUNT 5
#define SLE_RSSI_CAL_LED_HOLD_COUNT 60
#define SLE_RSSI_CAL_LED_BOOT_CLEAR_COUNT 10
#define SLE_RSSI_CAL_TASK_PRIO 30
#define SLE_RSSI_CAL_TASK_STACK_SIZE 0x800
#define SLE_RSSI_CAL_NV_ID 0x5101
#define SLE_RSSI_CAL_NV_MAGIC 0x52535349
#define SLE_RSSI_CAL_NV_VERSION 1
#define SLE_RSSI_CAL_GPIO_SET_ADDR 0x44028030
#define SLE_RSSI_CAL_GPIO_CLEAR_ADDR 0x44028034

/*
 * The LED task runs every 50 ms. Therefore, the count constants above represent:
 * debounce=150 ms, long press=2 s, blink half-period=250 ms, result hold=3 s,
 * and delayed startup clear=500 ms after SLE is connected.
 * LED 任务每 50 ms 运行一次，因此以上计数依次表示：消抖 150 ms、长按 2 s、
 * 闪烁半周期 250 ms、结果保持 3 s，以及 SLE 连接后延迟 500 ms 清灯。
 */
typedef enum {
    SLE_RSSI_LED_IDLE = 0,
    SLE_RSSI_LED_RECORDING,
    SLE_RSSI_LED_SUCCESS,
    SLE_RSSI_LED_FAILURE,
} sle_rssi_led_state_t;

/*
 * Store calibration metadata together with A so corrupted or incompatible NV
 * data can be rejected instead of silently affecting every distance result.
 * 将校准元数据与 A 一并保存，用于拒绝损坏或版本不兼容的 NV 数据，
 * 避免异常数据在无提示的情况下影响后续所有测距结果。
 */
typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t rssi_at_1m;
    uint32_t mad;
    int32_t min_rssi;
    int32_t max_rssi;
    uint32_t sample_count;
    uint32_t checksum;
} sle_rssi_calibration_nv_t;

static volatile bool g_connected = false;
static volatile bool g_calibrating = false;
static volatile sle_rssi_led_state_t g_led_state = SLE_RSSI_LED_IDLE;
static bool g_led_boot_clear_pending = true;
static int8_t g_calibration_samples[SLE_RSSI_CAL_SAMPLE_COUNT] = {0};
static uint8_t g_calibration_count = 0;
static int8_t g_rssi_at_1m = CONFIG_SLE_RSSI_RANGING_RSSI_AT_1M;

/**
 * @if Eng
 * @brief Calculate the integrity checksum of one NV calibration record.
 * @param [in] record Calibration record excluding no fields.
 * @return XOR checksum of all metadata fields.
 * @else
 * @brief 计算一条 NV 校准记录的完整性校验值。
 * @param [in] record 校准记录。
 * @return 所有元数据字段的异或校验值。
 * @endif
 */
static uint32_t sle_rssi_calibration_checksum(const sle_rssi_calibration_nv_t *record)
{
    /*
     * This XOR checksum detects accidental NV corruption; it is not a security checksum.
     * 该异或校验用于发现意外的 NV 数据损坏，不属于安全校验算法。
     */
    return record->magic ^ record->version ^ (uint32_t)record->rssi_at_1m ^ record->mad ^ (uint32_t)record->min_rssi ^
           (uint32_t)record->max_rssi ^ record->sample_count ^ 0xA55A5AA5U;
}

/**
 * @if Eng
 * @brief Validate the format and values of an NV calibration record.
 * @param [in] record Calibration record read from NV.
 * @param [in] length Actual NV record length.
 * @retval true The record is valid.
 * @retval false The record must be ignored.
 * @else
 * @brief 校验从 NV 读取的校准记录格式和数值。
 * @param [in] record 从 NV 读取的校准记录。
 * @param [in] length NV 记录的实际长度。
 * @retval true 记录有效。
 * @retval false 必须忽略该记录。
 * @endif
 */
static bool sle_rssi_calibration_record_valid(const sle_rssi_calibration_nv_t *record, uint16_t length)
{
    /*
     * Validate the format, physical ranges, sample policy and checksum before using A.
     * 使用 A 之前，校验记录格式、物理取值范围、采样策略和校验和。
     */
    return (length == sizeof(*record)) && (record->magic == SLE_RSSI_CAL_NV_MAGIC) &&
           (record->version == SLE_RSSI_CAL_NV_VERSION) && (record->rssi_at_1m >= SLE_RSSI_CAL_RSSI_MIN) &&
           (record->rssi_at_1m <= SLE_RSSI_CAL_RSSI_MAX) && (record->mad <= SLE_RSSI_CAL_MAD_MAX) &&
           (record->min_rssi >= SLE_RSSI_CAL_RAW_MIN) && (record->max_rssi <= SLE_RSSI_CAL_RAW_MAX) &&
           (record->min_rssi <= record->rssi_at_1m) && (record->rssi_at_1m <= record->max_rssi) &&
           (record->sample_count == SLE_RSSI_CAL_SAMPLE_COUNT) &&
           (record->checksum == sle_rssi_calibration_checksum(record));
}

/**
 * @if Eng
 * @brief Load a validated one-metre RSSI value or retain the Kconfig default.
 * @else
 * @brief 加载通过校验的一米 RSSI，记录无效时保留 Kconfig 默认值。
 * @endif
 */
static void sle_rssi_calibration_load(void)
{
    sle_rssi_calibration_nv_t record = {0};
    uint16_t length = 0;
    errcode_t ret = uapi_nv_read(SLE_RSSI_CAL_NV_ID, sizeof(record), &length, (uint8_t *)&record);
    if ((ret == ERRCODE_SUCC) && sle_rssi_calibration_record_valid(&record, length)) {
        g_rssi_at_1m = (int8_t)record.rssi_at_1m;
        osal_printk("%s NV calibration loaded: A=%d dBm, MAD=%u dB, range=[%d,%d] dBm, samples=%u\r\n",
                    SLE_RSSI_CAL_LOG, record.rssi_at_1m, record.mad, record.min_rssi, record.max_rssi,
                    record.sample_count);
        return;
    }
    osal_printk("%s no valid NV calibration, use default A=%d dBm\r\n", SLE_RSSI_CAL_LOG, g_rssi_at_1m);
}

/**
 * @if Eng
 * @brief Build and persist one complete calibration record.
 * @param [in] median Calibrated one-metre RSSI in dBm.
 * @param [in] mad Median absolute deviation in dB.
 * @param [in] min_rssi Minimum sampled RSSI in dBm.
 * @param [in] max_rssi Maximum sampled RSSI in dBm.
 * @return NV write result.
 * @else
 * @brief 构造并持久化一条完整的校准记录。
 * @param [in] median 校准得到的一米 RSSI，单位为 dBm。
 * @param [in] mad 中位绝对偏差，单位为 dB。
 * @param [in] min_rssi 样本最小 RSSI，单位为 dBm。
 * @param [in] max_rssi 样本最大 RSSI，单位为 dBm。
 * @return NV 写入结果。
 * @endif
 */
static errcode_t sle_rssi_calibration_save(int8_t median, uint8_t mad, int8_t min_rssi, int8_t max_rssi)
{
    sle_rssi_calibration_nv_t record = {0};

    record.magic = SLE_RSSI_CAL_NV_MAGIC;
    record.version = SLE_RSSI_CAL_NV_VERSION;
    record.rssi_at_1m = median;
    record.mad = mad;
    record.min_rssi = min_rssi;
    record.max_rssi = max_rssi;
    record.sample_count = SLE_RSSI_CAL_SAMPLE_COUNT;
    record.checksum = sle_rssi_calibration_checksum(&record);
    return uapi_nv_write(SLE_RSSI_CAL_NV_ID, (const uint8_t *)&record, sizeof(record));
}

/**
 * @if Eng
 * @brief Sort signed RSSI samples in ascending order.
 * @param [inout] values Sample array.
 * @param [in] count Number of samples.
 * @else
 * @brief 将有符号 RSSI 样本按升序排列。
 * @param [inout] values 样本数组。
 * @param [in] count 样本数量。
 * @endif
 */
static void sle_rssi_calibration_sort(int8_t *values, uint8_t count)
{
    uint8_t i;
    uint8_t j;

    for (i = 1; i < count; i++) {
        int8_t value = values[i];
        j = i;
        while ((j > 0U) && (values[j - 1U] > value)) {
            values[j] = values[j - 1U];
            j--;
        }
        values[j] = value;
    }
}

/**
 * @if Eng
 * @brief Sort unsigned deviation samples in ascending order.
 * @param [inout] values Deviation array.
 * @param [in] count Number of values.
 * @else
 * @brief 将无符号偏差样本按升序排列。
 * @param [inout] values 偏差数组。
 * @param [in] count 数值数量。
 * @endif
 */
static void sle_rssi_calibration_sort_u8(uint8_t *values, uint8_t count)
{
    uint8_t i;
    uint8_t j;

    for (i = 1; i < count; i++) {
        uint8_t value = values[i];
        j = i;
        while ((j > 0U) && (values[j - 1U] > value)) {
            values[j] = values[j - 1U];
            j--;
        }
        values[j] = value;
    }
}

/**
 * @if Eng
 * @brief Generate a short non-optimizable delay for SK6805 bit timing.
 * @param [in] read_count Number of GPIO register reads.
 * @else
 * @brief 通过不可优化的寄存器读取生成 SK6805 位时序短延时。
 * @param [in] read_count GPIO 寄存器读取次数。
 * @endif
 */
static inline void sle_rssi_led_delay(uint8_t read_count)
{
    volatile uint32_t value;
    uint8_t i;

    /*
     * Register reads provide the short, non-optimizable pacing required by the
     * SK6805 bit stream. Keep these counts together with send_one/send_zero;
     * changing generated instructions or the GPIO clock requires waveform revalidation.
     * 寄存器读取用于提供 SK6805 位流所需且不会被优化掉的短延时。延时计数必须与
     * send_one/send_zero 配套维护；生成指令或 GPIO 时钟变化后需要重新验证波形。
     */
    for (i = 0; i < read_count; i++) {
        uapi_reg_read32(SLE_RSSI_CAL_GPIO_SET_ADDR, value);
    }
    unused(value);
}

/**
 * @if Eng
 * @brief Send one logical-one waveform to the SK6805.
 * @else
 * @brief 向 SK6805 发送一个逻辑 1 波形。
 * @endif
 */
static inline void sle_rssi_led_send_one(void)
{
    uapi_reg_setbit(SLE_RSSI_CAL_GPIO_SET_ADDR, SLE_RSSI_CAL_LED_PIN);
    sle_rssi_led_delay(SK6805_ONE_HIGH_DELAY);
    uapi_reg_setbit(SLE_RSSI_CAL_GPIO_CLEAR_ADDR, SLE_RSSI_CAL_LED_PIN);
    sle_rssi_led_delay(SK6805_ONE_LOW_DELAY);
}

/**
 * @if Eng
 * @brief Send one logical-zero waveform to the SK6805.
 * @else
 * @brief 向 SK6805 发送一个逻辑 0 波形。
 * @endif
 */
static inline void sle_rssi_led_send_zero(void)
{
    uapi_reg_setbit(SLE_RSSI_CAL_GPIO_SET_ADDR, SLE_RSSI_CAL_LED_PIN);
    sle_rssi_led_delay(SK6805_ZERO_HIGH_DELAY);
    uapi_reg_setbit(SLE_RSSI_CAL_GPIO_CLEAR_ADDR, SLE_RSSI_CAL_LED_PIN);
    sle_rssi_led_delay(SK6805_ZERO_LOW_DELAY);
}

/**
 * @if Eng
 * @brief Send one byte most-significant bit first.
 * @param [in] value Byte to send.
 * @else
 * @brief 按最高位优先顺序发送一个字节。
 * @param [in] value 待发送字节。
 * @endif
 */
static void sle_rssi_led_send_byte(uint8_t value)
{
    uint8_t mask;

    for (mask = 0x80U; mask != 0U; mask >>= 1U) {
        if ((value & mask) != 0U) {
            sle_rssi_led_send_one();
        } else {
            sle_rssi_led_send_zero();
        }
    }
}

/**
 * @if Eng
 * @brief Send and latch one 24-bit GRB color.
 * @param [in] red Red component.
 * @param [in] green Green component.
 * @param [in] blue Blue component.
 * @else
 * @brief 发送并锁存一个 24 位 GRB 颜色值。
 * @param [in] red 红色分量。
 * @param [in] green 绿色分量。
 * @param [in] blue 蓝色分量。
 * @endif
 */
static void sle_rssi_led_set(uint8_t red, uint8_t green, uint8_t blue)
{
    /*
     * Interrupts must not split the 24-bit frame and stretch a bit into reset timing.
     * 发送 24 位数据帧时禁止中断，避免某一位被拉长并被灯珠误判为复位时序。
     */
    uint32_t irq_status = osal_irq_lock();

    /*
     * SK6805-EC20 transmits MSB first in GRB order.
     * SK6805-EC20 按 GRB 顺序发送，每个字节均从最高位开始。
     */
    sle_rssi_led_send_byte(green);
    sle_rssi_led_send_byte(red);
    sle_rssi_led_send_byte(blue);
    osal_irq_restore(irq_status);
    /*
     * SK6805 requires a low-level reset/latch interval after the last data bit.
     * SK6805 在最后一个数据位之后需要低电平复位和锁存间隔。
     */
    (void)osal_msleep(1);
}

/**
 * @if Eng
 * @brief Reliably switch the SK6805 off with repeated black frames.
 * @else
 * @brief 通过重复全黑帧可靠熄灭 SK6805。
 * @endif
 */
static void sle_rssi_led_off(void)
{
    /*
     * A repeated black frame makes the state transition robust at a clock boundary.
     * 连续发送两次全黑帧，提高时钟切换边界处熄灭状态转换的可靠性。
     */
    sle_rssi_led_set(0, 0, 0);
    sle_rssi_led_set(0, 0, 0);
}

/**
 * @if Eng
 * @brief Advance startup-clear, blink and result-hold LED states.
 * @else
 * @brief 推进启动清灯、采集闪烁和结果保持等 LED 状态。
 * @endif
 */
static void sle_rssi_led_service(void)
{
    /*
     * This state machine is called once per 50 ms button-task iteration.
     * 按键任务每轮 50 ms 调用一次该状态机。
     */
    static sle_rssi_led_state_t last_state = SLE_RSSI_LED_IDLE;
    static uint8_t state_ticks = 0;
    static uint8_t boot_clear_ticks = 0;
    static bool blink_on = false;
    sle_rssi_led_state_t state = g_led_state;

    /*
     * SK6805 retains its last color across an MCU-only reset. Do not transmit
     * during SLE startup, when clock switching can distort software-generated
     * pulses. Clear the retained color once, 500 ms after the link is ready.
     * SK6805 在仅复位 MCU 时仍会锁存最后一次颜色。SLE 启动期间的时钟切换可能
     * 使软件生成的脉冲失真，因此此阶段不发送数据；建链完成 500 ms 后清除一次残留颜色。
     */
    if (g_led_boot_clear_pending && (state == SLE_RSSI_LED_IDLE) && g_connected) {
        if (++boot_clear_ticks >= SLE_RSSI_CAL_LED_BOOT_CLEAR_COUNT) {
            sle_rssi_led_off();
            g_led_boot_clear_pending = false;
            osal_printk("%s stale LED state cleared after SLE connection\r\n", SLE_RSSI_CAL_LOG);
        }
        return;
    }

    /*
     * Apply a color only on state entry; periodic work below handles blink/timeout.
     * 仅在进入新状态时设置颜色，后续周期逻辑负责闪烁和超时处理。
     */
    if (state != last_state) {
        last_state = state;
        state_ticks = 0;
        blink_on = false;
        if (state == SLE_RSSI_LED_RECORDING) {
            blink_on = true;
            sle_rssi_led_set(0, 0, 0xFF);
        } else if (state == SLE_RSSI_LED_SUCCESS) {
            sle_rssi_led_set(0, 0xFF, 0);
        } else if (state == SLE_RSSI_LED_FAILURE) {
            sle_rssi_led_set(0xFF, 0, 0);
        } else {
            sle_rssi_led_off();
        }
        return;
    }

    state_ticks++;
    if ((state == SLE_RSSI_LED_RECORDING) && (state_ticks >= SLE_RSSI_CAL_LED_BLINK_COUNT)) {
        state_ticks = 0;
        blink_on = !blink_on;
        sle_rssi_led_set(0, 0, blink_on ? 0xFF : 0);
    } else if (((state == SLE_RSSI_LED_SUCCESS) || (state == SLE_RSSI_LED_FAILURE)) &&
               (state_ticks >= SLE_RSSI_CAL_LED_HOLD_COUNT)) {
        g_led_state = SLE_RSSI_LED_IDLE;
    }
}

/**
 * @if Eng
 * @brief Start a new calibration only when the SLE link is ready.
 * @else
 * @brief 仅在 SLE 链路就绪时启动一轮新校准。
 * @endif
 */
static void sle_rssi_calibration_start(void)
{
    if (!g_connected) {
        osal_printk("%s long press ignored: SLE is not connected\r\n", SLE_RSSI_CAL_LOG);
        g_led_state = SLE_RSSI_LED_FAILURE;
        return;
    }
    if (g_calibrating) {
        return;
    }
    g_calibration_count = 0;
    g_calibrating = true;
    g_led_state = SLE_RSSI_LED_RECORDING;
    osal_printk("%s long press detected, calibration start: distance=100 cm, samples=%u\r\n", SLE_RSSI_CAL_LOG,
                SLE_RSSI_CAL_SAMPLE_COUNT);
}

/**
 * @if Eng
 * @brief Poll GPIO13, detect a debounced long press and service the indicator.
 * @param [in] arg Reserved task argument.
 * @return The task does not normally return.
 * @else
 * @brief 轮询 GPIO13、检测消抖后的长按并驱动状态灯。
 * @param [in] arg 预留的任务参数。
 * @return 任务正常情况下不会返回。
 * @endif
 */
static void *sle_rssi_calibration_button_task(const char *arg)
{
    bool last_raw = false;
    bool stable_pressed = false;
    bool long_press_handled = false;
    uint8_t stable_count = 0;
    uint8_t press_count = 0;

    unused(arg);
    while (1) {
        bool raw_pressed = (uapi_gpio_get_val(SLE_RSSI_CAL_BUTTON_PIN) == GPIO_LEVEL_LOW);
        /*
         * Accept a new level only after three identical samples (150 ms debounce).
         * 连续获得 3 次相同采样值后才接受新电平，实现 150 ms 按键消抖。
         */
        if (raw_pressed == last_raw) {
            if (stable_count < SLE_RSSI_CAL_BUTTON_DEBOUNCE) {
                stable_count++;
            }
        } else {
            last_raw = raw_pressed;
            stable_count = 1;
        }
        if (stable_count >= SLE_RSSI_CAL_BUTTON_DEBOUNCE) {
            stable_pressed = raw_pressed;
        }

        /*
         * Saturating counters avoid wraparound while the key remains held.
         * 使用饱和计数，避免按键持续按下时计数器回绕。
         */
        if (stable_pressed) {
            if (press_count < SLE_RSSI_CAL_LONG_PRESS_COUNT) {
                press_count++;
            }
            if ((press_count >= SLE_RSSI_CAL_LONG_PRESS_COUNT) && !long_press_handled) {
                long_press_handled = true;
                sle_rssi_calibration_start();
            }
        } else {
            press_count = 0;
            long_press_handled = false;
        }
        sle_rssi_led_service();
        (void)osal_msleep(SLE_RSSI_CAL_BUTTON_POLL_MS);
    }
    return NULL;
}

errcode_t sle_rssi_calibration_init(void)
{
    osal_task *task_handle;

    /*
     * GPIO13 is active-low; the internal pull-up also defines the released state.
     * GPIO13 为低电平有效，内部上拉同时确定按键释放时的稳定电平。
     */
    (void)uapi_pin_set_mode(SLE_RSSI_CAL_BUTTON_PIN, HAL_PIO_FUNC_GPIO);
    (void)uapi_pin_set_pull(SLE_RSSI_CAL_BUTTON_PIN, PIN_PULL_TYPE_UP);
    (void)uapi_gpio_set_dir(SLE_RSSI_CAL_BUTTON_PIN, GPIO_DIRECTION_INPUT);
    /*
     * GPIO5 drives the SK6805 DIN pin directly.
     * GPIO5 直接驱动 SK6805 的 DIN 引脚。
     */
    (void)uapi_pin_set_mode(SLE_RSSI_CAL_LED_PIN, SLE_RSSI_CAL_LED_PIN_MODE);
    (void)uapi_gpio_set_dir(SLE_RSSI_CAL_LED_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_reg_setbit(SLE_RSSI_CAL_GPIO_CLEAR_ADDR, SLE_RSSI_CAL_LED_PIN);
    (void)osal_msleep(1);
    g_led_state = SLE_RSSI_LED_IDLE;
    /*
     * Keep DIN low until SLE is connected and the clock transition has settled.
     * SLE 建链且时钟切换稳定之前，始终保持 DIN 为低电平。
     */
    sle_rssi_calibration_load();

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_rssi_calibration_button_task, 0, "SLERssiCal",
                                      SLE_RSSI_CAL_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_RSSI_CAL_TASK_PRIO);
    }
    osal_kthread_unlock();
    if (task_handle == NULL) {
        return ERRCODE_MALLOC;
    }
    osal_printk("%s ready: hold GPIO13 for 2000 ms at 100 cm; LED GPIO5 blue=recording, green=saved\r\n",
                SLE_RSSI_CAL_LOG);
    return ERRCODE_SUCC;
}

void sle_rssi_calibration_set_connected(bool connected)
{
    g_connected = connected;
    /*
     * Samples collected across a disconnection do not describe one radio condition.
     * 跨越断链采集的样本不属于同一个无线环境，因此断链时取消本轮校准。
     */
    if (!connected && g_calibrating) {
        g_calibrating = false;
        g_calibration_count = 0;
        g_led_state = SLE_RSSI_LED_FAILURE;
        osal_printk("%s calibration cancelled: SLE disconnected\r\n", SLE_RSSI_CAL_LOG);
    }
}

bool sle_rssi_calibration_is_active(void)
{
    return g_calibrating;
}

bool sle_rssi_calibration_add_sample(int8_t rssi)
{
    int8_t sorted[SLE_RSSI_CAL_SAMPLE_COUNT];
    uint8_t deviations[SLE_RSSI_CAL_SAMPLE_COUNT];
    int8_t median;
    uint8_t mad;
    uint8_t i;
    errcode_t ret;

    if (!g_calibrating || (g_calibration_count >= SLE_RSSI_CAL_SAMPLE_COUNT)) {
        return false;
    }
    g_calibration_samples[g_calibration_count++] = rssi;
    if (((g_calibration_count % 5U) == 0U) || (g_calibration_count == SLE_RSSI_CAL_SAMPLE_COUNT)) {
        osal_printk("%s recording: %u/%u, rssi=%d dBm\r\n", SLE_RSSI_CAL_LOG, g_calibration_count,
                    SLE_RSSI_CAL_SAMPLE_COUNT, rssi);
    }
    if (g_calibration_count < SLE_RSSI_CAL_SAMPLE_COUNT) {
        return false;
    }

    /*
     * Median A rejects isolated RSSI spikes better than an arithmetic mean.
     * 使用中位数计算 A，相比算术平均值可以更好地抑制孤立的 RSSI 毛刺。
     */
    for (i = 0; i < SLE_RSSI_CAL_SAMPLE_COUNT; i++) {
        sorted[i] = g_calibration_samples[i];
    }
    sle_rssi_calibration_sort(sorted, SLE_RSSI_CAL_SAMPLE_COUNT);
    median = sorted[SLE_RSSI_CAL_SAMPLE_COUNT / 2U];
    /*
     * MAD is stored as a simple indicator of calibration-environment stability.
     * 保存 MAD，作为校准环境稳定性的简易评价指标。
     */
    for (i = 0; i < SLE_RSSI_CAL_SAMPLE_COUNT; i++) {
        int16_t difference = (int16_t)g_calibration_samples[i] - median;
        deviations[i] = (uint8_t)((difference < 0) ? -difference : difference);
    }
    sle_rssi_calibration_sort_u8(deviations, SLE_RSSI_CAL_SAMPLE_COUNT);
    mad = deviations[SLE_RSSI_CAL_SAMPLE_COUNT / 2U];
    /*
     * Publish A immediately; persist the same value for subsequent normal boots.
     * 立即启用新的 A，并将同一数值持久化供后续正常启动加载。
     */
    g_rssi_at_1m = median;
    ret = sle_rssi_calibration_save(median, mad, sorted[0], sorted[SLE_RSSI_CAL_SAMPLE_COUNT - 1U]);
    g_calibrating = false;
    g_led_state = (ret == ERRCODE_SUCC) ? SLE_RSSI_LED_SUCCESS : SLE_RSSI_LED_FAILURE;
    osal_printk(
        "%s calibration complete: A=%d dBm, MAD=%u dB, range=[%d,%d] dBm, "
        "samples=%u, nv=%s\r\n",
        SLE_RSSI_CAL_LOG, median, mad, sorted[0], sorted[SLE_RSSI_CAL_SAMPLE_COUNT - 1U], SLE_RSSI_CAL_SAMPLE_COUNT,
        (ret == ERRCODE_SUCC) ? "ok" : "failed");
    return true;
}

int8_t sle_rssi_calibration_get_rssi_at_1m(void)
{
    return g_rssi_at_1m;
}
