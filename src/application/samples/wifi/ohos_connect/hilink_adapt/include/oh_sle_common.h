/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE PUBLIC API module.
 */

/**
 * @defgroup oh_sle_common PUBLIC API
 * @ingroup  SLE
 * @{
 */

#ifndef OH_SLE_COMMON_H
#define OH_SLE_COMMON_H

#include <stdint.h>
#include "oh_sle_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  SLE version 1.0.
 * @else
 * @brief  SLE版本1.0。
 * @endif
 */
#define OH_SLE_VERSION         0x10

/**
 * @if Eng
 * @brief  SLE address length.
 * @else
 * @brief  SLE地址长度。
 * @endif
 */
#define OH_SLE_ADDR_LEN        6

/**
 * @if Eng
 * @brief  SLE device name max length.
 * @else
 * @brief  SLE设备名称最大长度。
 * @endif
 */
#define OH_SLE_NAME_MAX_LEN    31

/**
 * @if Eng
 * @brief  SLE link key length.
 * @else
 * @brief  SLE 链路密钥长度。
 * @endif
 */
#define OH_SLE_LINK_KEY_LEN    16

/**
 * @if Eng
 * @brief  SLE address type.
 * @else
 * @brief  SLE地址类型。
 * @endif
 */
typedef enum {
    OH_SLE_ADDRESS_TYPE_PUBLIC = 0,      /*!< @if Eng public address
                                           @else   公有地址 @endif */
    OH_SLE_ADDRESS_TYPE_RANDOM = 6,      /*!< @if Eng random address
                                           @else   随机地址 @endif */
} SleAddrType;

/**
 * @if Eng
 * @brief  Struct of device address.
 * @else
 * @brief  设备地址。
 * @endif
 */
typedef struct {
    uint8_t type;                         /*!< @if Eng SLE device address type { @ref @ref SleAddrType }
                                               @else   SLE设备地址类型 { @ref @ref SleAddrType } @endif */
    unsigned char addr[OH_SLE_ADDR_LEN];     /*!< @if Eng SLE device address
                                               @else   SLE设备地址 @endif */
} SleAddr;

#ifdef __cplusplus
}
#endif
#endif  /* OH_SLE_COMMON_H */
