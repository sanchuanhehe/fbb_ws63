/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TIoT firmware load module private cfg handle. \n
 *
 * History: \n
 * 2023-12-05, Create file. \n
 */
#ifndef TIOT_CFG_HANDLE_PRIV_H
#define TIOT_CFG_HANDLE_PRIV_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_firmware_cfg_handle_priv CFG HANDLE PRIVATE
 * @ingroup  tiot_common_firmware
 * @{
 */

/**
 * @brief  TIoT firmware private extend command handle.
 * @param  [in]  fw TIoT firmware object.
 * @param  [in]  ext_cmd Extend command.
 * @param  [in]  cmd Firmware Command.
 * @param  [in]  len Firmware Command length.
 * @return if OK, return ERRCODE_TIOT_SUCC (zero), else return negative.
 */
int32_t tiot_fw_ext_cmd_handle_priv(tiot_fw *fw, uint16_t ext_cmd, const uint8_t *cmd, uint16_t len);

/**
 * @brief  TIoT firmware before private extend command handle.
 * @param  [in]  fw TIoT firmware object.
 * @param  [in]  ext_cmd Extend command.
 * @param  [in]  cmd Firmware Command.
 * @param  [in]  len Firmware Command length.
 * @return if OK, return ERRCODE_TIOT_SUCC (zero), else return negative.
 */
int32_t tiot_fw_pre_ext_cmd_handle_priv(tiot_fw *fw, uint16_t ext_cmd, uint8_t *cmd, uint16_t *len);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif