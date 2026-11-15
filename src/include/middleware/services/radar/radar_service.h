/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file of radar common API
 */

/**
 * @defgroup middleware_service_radar_common Radar Common API
 * @ingroup middleware_service_radar
 * @{
 */

#ifndef RADAR_SERVICE_H
#define RADAR_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "radar_errcode.h"

#ifdef CONFIG_RADAR_PLT_SUB_6G
#include "radar_service_sub_6g.h"
#endif

#ifdef CONFIG_RADAR_PLT_SLP
#include "radar_service_slp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)
/**
 * @if Eng
 * @brief Struct of version report interface
 * @else
 * @brief 版本号上报接口出参结构体
 * @endif
 */
typedef struct {
    uint16_t major; /*!< @if Eng major version
                                             @else 主版本号 @endif */
    uint16_t minor; /*!< @if Eng minor version
                                             @else 次版本号 @endif */
    uint16_t patch; /*!< @if Eng patch version
                                             @else 修订版本号 @endif */
} radar_version_t;

/**
 * @if Eng
 * @brief  report struct of version info.
 * @else
 * @brief  版本信息的上报结构体。
 * @endif
 */
typedef struct {
    radar_version_t narrow_band; /*!< @if Eng version number of narrow band system.
                                             @else 窄带软件版本号 @endif */
    radar_version_t wide_band;   /*!< @if Eng version number of wide band system.
                                             @else 宽带软件版本号 @endif */
} radar_version_info_t;

/**
 * @if Eng
 * @brief  Get radar version version.
 * @par Description: Get radar version.
 * @param [in] radar_version see @ref radar_version_info_t.
 * @retval error code.
 * @else
 * @brief  获取雷达版本号。
 * @par Description: 获取雷达版本号。
 * @param [in] radar_version 版本号
 * @retval 执行结果错误码。
 * @endif
 */
errcode_radar_client_t uapi_radar_get_version(radar_version_info_t *radar_version);

#pragma pack()

#ifdef __cplusplus
}
#endif
#endif

/**
 * @}
 */