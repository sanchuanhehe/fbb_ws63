/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TIoT firmware load module utils. \n
 *
 * History: \n
 * 2023-12-05, Create file. \n
 */
#ifndef TIOT_FIRMWARE_UTILS_H
#define TIOT_FIRMWARE_UTILS_H

#include "tiot_firmware.h"
#include "tiot_controller.h"
#include "tiot_xfer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_firmware_utils Firmware Utils
 * @ingroup  tiot_common_firmware
 * @{
 */

static inline int32_t tiot_firmware_write(tiot_fw *fw, const uint8_t *data, uint32_t len)
{
    tiot_controller *controller = tiot_container_of(fw, tiot_controller, firmware);
    return tiot_xfer_send(&controller->transfer, data, len, NULL);
}

static inline int32_t tiot_firmware_read(tiot_fw *fw, uint8_t *data, uint32_t len, uint32_t timeout_ms)
{
    int32_t ret;
    tiot_xfer_rx_param rx_param = { 0 };
    tiot_controller *controller = tiot_container_of(fw, tiot_controller, firmware);
    rx_param.timeout = timeout_ms;
    rx_param.subsys  = 0;
    ret = tiot_xfer_recv(&controller->transfer, data, len, &rx_param);
    return ret == 0 ? (int32_t)ERRCODE_TIOT_TIMED_OUT : ret;
}

static inline int32_t tiot_fw_read_until(tiot_fw *fw, uint8_t *buff, uint32_t len, uint32_t timeout_ms)
{
    int32_t ret = 0;
    uint32_t read_len = len;
    uint32_t read_cnt = 0;
    while (read_len > 0) {
        ret = tiot_firmware_read(fw, &buff[read_cnt], read_len, timeout_ms);
        if (ret <= 0) {
            return ret;
        }
        read_len -= (uint32_t)ret;
        read_cnt += (uint32_t)ret;
    }
    return len;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif