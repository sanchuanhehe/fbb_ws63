/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Tiot miscellaneous header file. \n
 *
 * History: \n
 * 2024-06-20, Create file. \n
 */
#ifndef TIOT_MISC_H
#define TIOT_MISC_H

#include "tiot_types.h"
#include "tiot_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_utils_tiot_misc TIOT miscellaneous function
 * @ingroup  tiot_common_utils
 * @{
 */

/**
 * @brief  TIoT converts a non-negative integer to a hexadecimal string.
 * @param  [in]  num   Nonnegative integer.
 * @param  [in]  str_buff   String after conversion num.
 * @param  [in]  str_buff_len Length of str_buff.
 * @return ERRCODE_TIOT_SUCC (zero) is returned if Conversion succeeded.
 *         Otherwise, negative is returned.
 */
int32_t tiot_num_to_hex(uint32_t num, char* str_buff, uint32_t str_buff_len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif