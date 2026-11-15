/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-04-26, Create file. \n
 */
#ifndef TIOT_XFER_UTILS_H
#define TIOT_XFER_UTILS_H

#include "tiot_xfer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_transfer_utils Transfer Utils
 * @ingroup  tiot_common_transfer
 * @{
 */

/**
 * @brief  Direct send by xmit->write().
 * @param  [in] xfer Transfer manager.
 * @param  [in] data Data to be sent.
 * @param  [in] len  Data len.
 * @return return actual send len if ok, else return negative.
 */
int32_t tiot_xfer_direct_send(tiot_xfer_manager *xfer, const uint8_t *data, uint32_t len);

/**
 * @brief  Save data to circ buf.
 * @param  [in] cb   Circ buf.
 * @param  [in] data Data to be saved.
 * @param  [in] len  Data len.
 * @return return ERRCODE_TIOT_SUCC (zero) if ok, else return negative.
 */
int32_t tiot_xfer_read_store(tiot_circ_buf *cb, uint8_t *data, uint32_t len);

/**
 * @brief  Read by xmit->read().
 * @param  [in]  xfer Transfer manager.
 * @param  [out] buff Read buff.
 * @param  [in]  len  Expected read len.
 * @param  [in]  timeout_ms Read timeout param.
 * @return return actual read len if ok, return zero if timeout, else return negative.
 */
int32_t tiot_xfer_read_out(tiot_xfer_manager *xfer, uint8_t *buff, uint32_t len, uint32_t timeout_ms);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif