/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging client implementation. \n
 * @else
 * @brief SLE RSSI 测距 Client 实现。 \n
 * @endif
 */
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "common_def.h"
#include "securec.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_rssi_ranging_calibration.h"
#include "sle_rssi_ranging_client.h"

#define SLE_RSSI_Q8_SCALE 256
#define SLE_RSSI_DECIMAL_SCALE 10U
#define SLE_RSSI_CLIENT_LOG "[sle rssi client]"
#define SLE_RSSI_SERVER_NAME "sle_rssi_server"
#define SLE_RSSI_SEEK_INTERVAL 100
#define SLE_RSSI_SEEK_WINDOW 100
#define SLE_RSSI_CORE_READY_DELAY_MS 5000
#define SLE_RSSI_SAMPLE_INTERVAL_MS 1000
#define SLE_RSSI_CAL_SAMPLE_INTERVAL_MS 200
#define SLE_RSSI_MEDIAN_WINDOW_SIZE 7
#define SLE_RSSI_EMA_OLD_WEIGHT 3
#define SLE_RSSI_EMA_TOTAL_WEIGHT 4
#define SLE_RSSI_INVALID_VALUE 0x7F
#define SLE_RSSI_NEAR_LIMIT_CM 150
#define SLE_RSSI_MIDDLE_LIMIT_CM 500
#define SLE_RSSI_MAX_DISTANCE_CM 10000
#define SLE_RSSI_RANGING_TASK_PRIO 27
#define SLE_RSSI_RANGING_STACK_SIZE 0x1000

static sle_addr_t g_remote_addr = {0};
static volatile bool g_target_found = false;
static volatile bool g_connected = false;
static uint16_t g_conn_id = 0;
static int8_t g_rssi_window[SLE_RSSI_MEDIAN_WINDOW_SIZE] = {0};
static uint8_t g_rssi_count = 0;
static uint8_t g_rssi_index = 0;
static bool g_ema_valid = false;
static int32_t g_ema_q8 = 0;

/**
 * @if Eng
 * @brief Reset the median window and EMA state when the link or calibration changes.
 * @else
 * @brief 链路或校准参数变化时，复位中值窗口和 EMA 状态。
 * @endif
 */
static void sle_rssi_reset_filter(void)
{
    (void)memset_s(g_rssi_window, sizeof(g_rssi_window), 0, sizeof(g_rssi_window));
    g_rssi_count = 0;
    g_rssi_index = 0;
    g_ema_valid = false;
    g_ema_q8 = 0;
}

/**
 * @if Eng
 * @brief Sort the active RSSI window and return its median.
 * @return Median RSSI in dBm.
 * @else
 * @brief 对当前有效 RSSI 窗口排序并返回中位数。
 * @return RSSI 中位数，单位为 dBm。
 * @endif
 */
static int8_t sle_rssi_get_median(void)
{
    int8_t sorted[SLE_RSSI_MEDIAN_WINDOW_SIZE];
    uint8_t i;
    uint8_t j;

    for (i = 0; i < g_rssi_count; i++) {
        sorted[i] = g_rssi_window[i];
    }
    for (i = 1; i < g_rssi_count; i++) {
        int8_t value = sorted[i];
        j = i;
        while ((j > 0U) && (sorted[j - 1U] > value)) {
            sorted[j] = sorted[j - 1U];
            j--;
        }
        sorted[j] = value;
    }
    return sorted[g_rssi_count / 2U];
}

/**
 * @if Eng
 * @brief Estimate distance with the log-distance path loss model.
 * @param [in] filtered_rssi_q8 Filtered RSSI in signed Q8 fixed-point format.
 * @return Estimated distance in centimetres, clamped to the sample output range.
 * @else
 * @brief 使用对数距离路径损耗模型估算距离。
 * @param [in] filtered_rssi_q8 采用有符号 Q8 定点格式表示的滤波 RSSI。
 * @return 估算距离，单位为厘米，并限制在案例输出范围内。
 * @endif
 */
static uint32_t sle_rssi_estimate_distance_cm(int32_t filtered_rssi_q8)
{
    float filtered_rssi = (float)filtered_rssi_q8 / 256.0f;
    float path_loss = (float)CONFIG_SLE_RSSI_RANGING_PATH_LOSS_TENTHS / 10.0f;
    float exponent = ((float)sle_rssi_calibration_get_rssi_at_1m() - filtered_rssi) / (10.0f * path_loss);
    float distance_cm = 100.0f * powf(10.0f, exponent);
    if (distance_cm < 1.0f) {
        distance_cm = 1.0f;
    } else if (distance_cm > (float)SLE_RSSI_MAX_DISTANCE_CM) {
        distance_cm = (float)SLE_RSSI_MAX_DISTANCE_CM;
    }
    return (uint32_t)(distance_cm + 0.5f);
}

/**
 * @if Eng
 * @brief Map an estimated distance to the near, middle or far zone.
 * @param [in] distance_cm Estimated distance in centimetres.
 * @return Static zone name string.
 * @else
 * @brief 将估算距离映射为 near、middle 或 far 分区。
 * @param [in] distance_cm 估算距离，单位为厘米。
 * @return 静态分区名称字符串。
 * @endif
 */
static const char *sle_rssi_distance_zone(uint32_t distance_cm)
{
    if (distance_cm <= SLE_RSSI_NEAR_LIMIT_CM) {
        return "near";
    }
    if (distance_cm <= SLE_RSSI_MIDDLE_LIMIT_CM) {
        return "middle";
    }
    return "far";
}

/**
 * @if Eng
 * @brief Apply seven-point median filtering and EMA, then print the ranging result.
 * @param [in] rssi Latest valid connection RSSI in dBm.
 * @else
 * @brief 对有效 RSSI 执行七点中值滤波和 EMA，并输出测距结果。
 * @param [in] rssi 最新连接态 RSSI，单位为 dBm。
 * @endif
 */
static void sle_rssi_process_sample(int8_t rssi)
{
    int8_t median;
    int32_t filtered_whole;
    uint32_t filtered_tenths;
    uint32_t distance_cm;

    g_rssi_window[g_rssi_index] = rssi;
    g_rssi_index = (uint8_t)((g_rssi_index + 1U) % SLE_RSSI_MEDIAN_WINDOW_SIZE);
    if (g_rssi_count < SLE_RSSI_MEDIAN_WINDOW_SIZE) {
        g_rssi_count++;
    }
    median = sle_rssi_get_median();
    if (!g_ema_valid) {
        g_ema_q8 = (int32_t)median * SLE_RSSI_Q8_SCALE;
        g_ema_valid = true;
    } else {
        g_ema_q8 =
            ((g_ema_q8 * SLE_RSSI_EMA_OLD_WEIGHT) + ((int32_t)median * SLE_RSSI_Q8_SCALE)) / SLE_RSSI_EMA_TOTAL_WEIGHT;
    }
    distance_cm = sle_rssi_estimate_distance_cm(g_ema_q8);
    filtered_whole = g_ema_q8 / SLE_RSSI_Q8_SCALE;
    filtered_tenths = (uint32_t)((g_ema_q8 >= 0) ? (g_ema_q8 % SLE_RSSI_Q8_SCALE) : -(g_ema_q8 % SLE_RSSI_Q8_SCALE));
    filtered_tenths = (filtered_tenths * SLE_RSSI_DECIMAL_SCALE) / SLE_RSSI_Q8_SCALE;
    osal_printk(
        "%s range: raw=%d dBm, median=%d dBm, filtered=%d.%u dBm, samples=%u, "
        "distance=%u cm, zone=%s\r\n",
        SLE_RSSI_CLIENT_LOG, rssi, median, filtered_whole, filtered_tenths, g_rssi_count, distance_cm,
        sle_rssi_distance_zone(distance_cm));
}

/**
 * @if Eng
 * @brief Parse announcement fields and match the exact Server local name.
 * @param [in] data Announcement or seek-response payload.
 * @param [in] data_len Payload length.
 * @retval true The expected complete local name is present.
 * @retval false The payload is malformed or the name does not match.
 * @else
 * @brief 解析广播字段并精确匹配 Server 本地名称。
 * @param [in] data 广播或扫描响应数据。
 * @param [in] data_len 数据长度。
 * @retval true 找到期望的完整本地名称。
 * @retval false 数据格式异常或名称不匹配。
 * @endif
 */
static bool sle_rssi_has_server_name(const uint8_t *data, uint8_t data_len)
{
    const uint8_t server_name[] = SLE_RSSI_SERVER_NAME;
    uint16_t index = 0;

    while (index < data_len) {
        uint8_t field_len = data[index];
        uint16_t next = index + field_len + 1U;
        if ((field_len < 1U) || (next > data_len)) {
            return false;
        }
        if ((data[index + 1U] == 0x0B) && ((field_len - 1U) == (sizeof(server_name) - 1U)) &&
            (memcmp(&data[index + 2U], server_name, sizeof(server_name) - 1U) == 0)) {
            return true;
        }
        index = next;
    }
    return false;
}

/**
 * @if Eng
 * @brief Configure active seeking and start discovery.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 配置主动扫描参数并启动设备发现。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_start_seek(void)
{
    sle_seek_param_t param = {0};
    errcode_t ret;

    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_RSSI_SEEK_INTERVAL;
    param.seek_window[0] = SLE_RSSI_SEEK_WINDOW;
    ret = sle_set_seek_param(&param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }
    ret = sle_start_seek();
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start seek\r\n", SLE_RSSI_CLIENT_LOG);
    }
    return ret;
}

/**
 * @if Eng
 * @brief Start seeking after the SLE stack is enabled.
 * @param [in] status SLE enable result.
 * @else
 * @brief SLE 协议栈使能成功后启动扫描。
 * @param [in] status SLE 使能结果。
 * @endif
 */
static void sle_rssi_enable_cb(errcode_t status)
{
    osal_printk("%s SLE enabled, status=0x%x\r\n", SLE_RSSI_CLIENT_LOG, status);
    if (status == ERRCODE_SLE_SUCCESS) {
        (void)sle_rssi_start_seek();
    }
}

/**
 * @if Eng
 * @brief Report the asynchronous seek-enable result.
 * @param [in] status Seek-enable result.
 * @else
 * @brief 输出异步扫描使能结果。
 * @param [in] status 扫描使能结果。
 * @endif
 */
static void sle_rssi_seek_enable_cb(errcode_t status)
{
    osal_printk("%s seek enabled, status=0x%x\r\n", SLE_RSSI_CLIENT_LOG, status);
}

/**
 * @if Eng
 * @brief Select the named Server from seek results and stop seeking.
 * @param [in] result One SLE seek result.
 * @else
 * @brief 从扫描结果中选择指定名称的 Server 并停止扫描。
 * @param [in] result 一条 SLE 扫描结果。
 * @endif
 */
static void sle_rssi_seek_result_cb(sle_seek_result_info_t *result)
{
    if ((result == NULL) || (result->data == NULL) || g_target_found) {
        return;
    }
    if (!sle_rssi_has_server_name(result->data, result->data_length)) {
        return;
    }
    if (memcpy_s(&g_remote_addr, sizeof(g_remote_addr), &result->addr, sizeof(result->addr)) != EOK) {
        return;
    }
    g_target_found = true;
    osal_printk("%s found %s, scan_rssi=%d dBm, stop seek\r\n", SLE_RSSI_CLIENT_LOG, SLE_RSSI_SERVER_NAME,
                (int8_t)result->rssi);
    (void)sle_stop_seek();
}

/**
 * @if Eng
 * @brief Connect to the selected Server after seeking has stopped.
 * @param [in] status Seek-disable result.
 * @else
 * @brief 扫描停止后连接已选中的 Server。
 * @param [in] status 扫描停止结果。
 * @endif
 */
static void sle_rssi_seek_disable_cb(errcode_t status)
{
    errcode_t ret;
    osal_printk("%s seek disabled, status=0x%x\r\n", SLE_RSSI_CLIENT_LOG, status);
    if ((status != ERRCODE_SLE_SUCCESS) || !g_target_found) {
        return;
    }
    (void)sle_remove_paired_remote_device(&g_remote_addr);
    ret = sle_connect_remote_device(&g_remote_addr);
    osal_printk("%s connect request sent, status=0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
}

/**
 * @if Eng
 * @brief Maintain ranging and calibration state on SLE connection changes.
 * @param [in] conn_id SLE connection ID.
 * @param [in] addr Peer address.
 * @param [in] conn_state Current connection state.
 * @param [in] pair_state Current pairing state.
 * @param [in] disc_reason Disconnection reason.
 * @else
 * @brief 在 SLE 连接状态变化时维护测距和校准状态。
 * @param [in] conn_id SLE 连接 ID。
 * @param [in] addr 对端地址。
 * @param [in] conn_state 当前连接状态。
 * @param [in] pair_state 当前配对状态。
 * @param [in] disc_reason 断链原因。
 * @endif
 */
static void sle_rssi_state_changed_cb(uint16_t conn_id,
                                      const sle_addr_t *addr,
                                      sle_acb_state_t conn_state,
                                      sle_pair_state_t pair_state,
                                      sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_conn_id = conn_id;
        g_connected = true;
        sle_rssi_calibration_set_connected(true);
        sle_rssi_reset_filter();
        osal_printk("%s connected, conn_id=0x%02x, calibration=%d dBm@1m, path_loss=%d.%u\r\n", SLE_RSSI_CLIENT_LOG,
                    conn_id, sle_rssi_calibration_get_rssi_at_1m(),
                    CONFIG_SLE_RSSI_RANGING_PATH_LOSS_TENTHS / SLE_RSSI_DECIMAL_SCALE,
                    CONFIG_SLE_RSSI_RANGING_PATH_LOSS_TENTHS % SLE_RSSI_DECIMAL_SCALE);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_connected = false;
        sle_rssi_calibration_set_connected(false);
        sle_rssi_reset_filter();
        g_target_found = false;
        osal_printk("%s disconnected, reason=0x%x, restart seek\r\n", SLE_RSSI_CLIENT_LOG, disc_reason);
        (void)sle_remove_paired_remote_device(&g_remote_addr);
        (void)sle_rssi_start_seek();
    }
}

/**
 * @if Eng
 * @brief Route an asynchronous RSSI result to calibration or normal ranging.
 * @param [in] conn_id SLE connection ID.
 * @param [in] rssi Raw connection RSSI in dBm.
 * @param [in] status RSSI read result.
 * @else
 * @brief 将异步 RSSI 结果分流到校准或普通测距流程。
 * @param [in] conn_id SLE 连接 ID。
 * @param [in] rssi 原始连接态 RSSI，单位为 dBm。
 * @param [in] status RSSI 读取结果。
 * @endif
 */
static void sle_rssi_read_cb(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    if ((conn_id != g_conn_id) || !g_connected || (status != ERRCODE_SLE_SUCCESS)) {
        osal_printk("%s RSSI read failed: conn_id=0x%02x, status=0x%x\r\n", SLE_RSSI_CLIENT_LOG, conn_id, status);
        return;
    }
    if (rssi == SLE_RSSI_INVALID_VALUE) {
        osal_printk("%s ignore invalid RSSI=0x7f\r\n", SLE_RSSI_CLIENT_LOG);
        return;
    }
    if (sle_rssi_calibration_is_active()) {
        /*
         * Calibration consumes raw RSSI; do not mix these samples into the ranging filters.
         * 校准过程使用原始 RSSI，不能将这些样本混入普通测距滤波器。
         */
        if (sle_rssi_calibration_add_sample(rssi)) {
            sle_rssi_reset_filter();
        }
        return;
    }
    sle_rssi_process_sample(rssi);
}

/**
 * @if Eng
 * @brief Register SLE enable and discovery callbacks.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 注册 SLE 使能和设备发现回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_register_seek_callbacks(void)
{
    sle_announce_seek_callbacks_t callbacks = {0};
    callbacks.sle_enable_cb = sle_rssi_enable_cb;
    callbacks.seek_enable_cb = sle_rssi_seek_enable_cb;
    callbacks.seek_result_cb = sle_rssi_seek_result_cb;
    callbacks.seek_disable_cb = sle_rssi_seek_disable_cb;
    return sle_announce_seek_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Register connection-state and RSSI-read callbacks.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 注册连接状态和 RSSI 读取回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
static errcode_t sle_rssi_register_connection_callbacks(void)
{
    sle_connection_callbacks_t callbacks = {0};
    callbacks.connect_state_changed_cb = sle_rssi_state_changed_cb;
    callbacks.read_rssi_cb = sle_rssi_read_cb;
    return sle_connection_register_callbacks(&callbacks);
}

/**
 * @if Eng
 * @brief Periodically request connection RSSI at the rate required by the current mode.
 * @param [in] arg Reserved task argument.
 * @return The task does not normally return.
 * @else
 * @brief 按当前模式所需频率周期性请求连接态 RSSI。
 * @param [in] arg 预留的任务参数。
 * @return 任务正常情况下不会返回。
 * @endif
 */
static void *sle_rssi_poll_task(const char *arg)
{
    unused(arg);
    while (1) {
        if (g_connected) {
            errcode_t ret = sle_read_remote_device_rssi(g_conn_id);
            if (ret != ERRCODE_SLE_SUCCESS) {
                osal_printk("%s RSSI request failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
            }
        }
        /*
         * Calibration needs 31 closely spaced samples; normal ranging remains once per second.
         * 校准需要连续采集 31 个密集样本，普通测距仍保持每秒一次。
         */
        (void)osal_msleep(sle_rssi_calibration_is_active() ? SLE_RSSI_CAL_SAMPLE_INTERVAL_MS
                                                           : SLE_RSSI_SAMPLE_INTERVAL_MS);
    }
    return NULL;
}

/**
 * @if Eng
 * @brief Create the asynchronous RSSI polling task.
 * @retval ERRCODE_SUCC Success.
 * @retval ERRCODE_MALLOC Task creation failed.
 * @else
 * @brief 创建异步 RSSI 轮询任务。
 * @retval ERRCODE_SUCC 成功。
 * @retval ERRCODE_MALLOC 任务创建失败。
 * @endif
 */
static errcode_t sle_rssi_start_poll_task(void)
{
    osal_task *task_handle;

    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)sle_rssi_poll_task, 0, "SLERssiPoll", SLE_RSSI_RANGING_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_RSSI_RANGING_TASK_PRIO);
    }
    osal_kthread_unlock();
    return (task_handle == NULL) ? ERRCODE_MALLOC : ERRCODE_SUCC;
}

/**
 * @if Eng
 * @brief Initialize Client callbacks, calibration hardware, polling task and SLE.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 初始化 Client 回调、校准硬件、轮询任务和 SLE。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_ranging_client_init(void)
{
    errcode_t ret;

    (void)osal_msleep(SLE_RSSI_CORE_READY_DELAY_MS);
    ret = sle_rssi_register_seek_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register seek callbacks failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
        return ret;
    }
    ret = sle_rssi_register_connection_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
        return ret;
    }
    ret = sle_rssi_calibration_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s calibration hardware init failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
        return ret;
    }
    ret = sle_rssi_start_poll_task();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s create polling task failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
        return ret;
    }
    ret = enable_sle();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s enable SLE failed: 0x%x\r\n", SLE_RSSI_CLIENT_LOG, ret);
    }
    return ret;
}
