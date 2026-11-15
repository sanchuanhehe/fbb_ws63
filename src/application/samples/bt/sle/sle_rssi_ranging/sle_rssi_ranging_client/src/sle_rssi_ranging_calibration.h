/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging one-metre calibration interface. \n
 * @else
 * @brief SLE RSSI 测距一米校准接口。 \n
 * @endif
 */
#ifndef SLE_RSSI_RANGING_CALIBRATION_H
#define SLE_RSSI_RANGING_CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Initialize GPIO13, the SK6805 indicator, the NV record and the button task.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @else
 * @brief 初始化 GPIO13、SK6805 状态灯、NV 校准记录和按键任务。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t sle_rssi_calibration_init(void);

/**
 * @if Eng
 * @brief Notify the calibration module of the SLE link state.
 * @param [in] connected Whether the SLE link is connected.
 * @else
 * @brief 通知校准模块当前 SLE 链路状态。
 * @param [in] connected SLE 链路是否已连接。
 * @endif
 */
void sle_rssi_calibration_set_connected(bool connected);

/**
 * @if Eng
 * @brief Check whether raw RSSI samples must be routed to calibration.
 * @retval true Calibration is active.
 * @retval false Calibration is inactive.
 * @else
 * @brief 检查原始 RSSI 样本是否需要转交校准模块。
 * @retval true 正在校准。
 * @retval false 未在校准。
 * @endif
 */
bool sle_rssi_calibration_is_active(void);

/**
 * @if Eng
 * @brief Add one raw RSSI sample to the current calibration batch.
 * @param [in] rssi Raw connection RSSI in dBm.
 * @retval true The complete sample batch has been processed.
 * @retval false More samples are required or calibration is inactive.
 * @else
 * @brief 向当前校准批次加入一个原始 RSSI 样本。
 * @param [in] rssi 原始连接态 RSSI，单位为 dBm。
 * @retval true 完整采样批次已处理完成。
 * @retval false 仍需继续采样，或当前未在校准。
 * @endif
 */
bool sle_rssi_calibration_add_sample(int8_t rssi);

/**
 * @if Eng
 * @brief Get the active one-metre RSSI value from validated NV or the Kconfig default.
 * @return Active one-metre RSSI value in dBm.
 * @else
 * @brief 获取当前生效的一米 RSSI；该值来自通过校验的 NV 记录或 Kconfig 默认值。
 * @return 当前生效的一米 RSSI，单位为 dBm。
 * @endif
 */
int8_t sle_rssi_calibration_get_rssi_at_1m(void);

#ifdef __cplusplus
}
#endif
#endif
