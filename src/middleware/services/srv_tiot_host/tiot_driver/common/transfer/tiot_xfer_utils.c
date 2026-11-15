/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-04-26, Create file. \n
 */

#include "tiot_xfer_utils.h"

static inline int32_t xfer_direct_send(tiot_xfer_manager *xfer, const uint8_t *data, uint32_t len)
{
    int32_t ret;
    (void)osal_mutex_lock(&xfer->dev_mutex);
    if (xfer->xmit_state != TIOT_XMIT_STATE_OPEN) {
        (void)osal_mutex_unlock(&xfer->dev_mutex);
        return ERRCODE_TIOT_XFER_NOT_OPEN;
    }
    ret = xfer->xmit_ops->write(&xfer->xmit, data, len);
    (void)osal_mutex_unlock(&xfer->dev_mutex);
    return ret >= 0 ? ret : (int32_t)ERRCODE_TIOT_XFER_WRITE_FAIL;
}

static inline int32_t xfer_rx_buff_read(tiot_xfer_manager *xfer, uint8_t *buff, uint32_t len)
{
    uint32_t buff_data_len = circ_buf_query_busy(&xfer->rx_buff);
    int32_t read_len = buff_data_len < len ? buff_data_len : len;
    read_len = circ_buf_read(&xfer->rx_buff, buff, read_len);
    return read_len == 0 ? (int32_t)ERRCODE_TIOT_CIRC_BUF_READ_FAIL : read_len;
}

static inline int32_t xfer_direct_read(tiot_xfer_manager *xfer, uint8_t *buff, uint32_t len)
{
    int32_t read_len = xfer->xmit_ops->read(&xfer->xmit, buff, len);
    return (read_len >= 0 && read_len <= (int32_t)len) ? read_len : (int32_t)ERRCODE_TIOT_XFER_READ_FAIL;
}

int32_t tiot_xfer_direct_send(tiot_xfer_manager *xfer, const uint8_t *data, uint32_t len)
{
    int32_t ret;
    const uint8_t *buff = data;
    uint32_t remain_len = len;
    while (remain_len) {
        /* 板级接口可能有单次发送限制. */
        ret = xfer_direct_send(xfer, buff, remain_len);
        /* 阻塞式接口，发送结果为0情况时继续发送 */
        if (ret < 0) {
            return ret;
        }

        remain_len -= ret;
        buff += ret;
    }
    return len;
}

int32_t tiot_xfer_read_store(tiot_circ_buf *cb, uint8_t *data, uint32_t len)
{
    int32_t ret;
    if (cb == NULL) {
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    if (circ_buf_query_free(cb) < len) {
        return ERRCODE_TIOT_CIRC_BUF_SPACE_NOT_ENOUGH;
    }
    ret = circ_buf_write(cb, data, len);
    if (ret == 0) {
        return ERRCODE_TIOT_CIRC_BUF_WRITE_FAIL;
    }
    return ERRCODE_TIOT_SUCC;
}

static int tiot_xfer_read_out_check(const void *param)
{
    tiot_xfer_manager *xfer = (tiot_xfer_manager *)param;
    if ((xfer->rx_wait_func(xfer) != 0) ||
        ((xfer->xmit_ops->rx_mode == TIOT_XMIT_RX_MODE_BUFFED) && (circ_buf_query_busy(&xfer->rx_buff) != 0))) {
        return 1;
    }
    return 0;
}

int32_t tiot_xfer_read_out(tiot_xfer_manager *xfer, uint8_t *buff, uint32_t len, uint32_t timeout_ms)
{
    int32_t read_len;
    int32_t timeout = osal_wait_timeout_interruptible(&xfer->rx_wait, tiot_xfer_read_out_check,
                                                      (const void *)xfer, timeout_ms);
    if (timeout == 0) { /* read timed out. */
        return 0;
    } else if (timeout < 0) {
        return ERRCODE_TIOT_WAIT_TIMEOUT_FAIL;
    }
    osal_mutex_lock(&xfer->dev_mutex);

    if (xfer->xmit_ops->rx_mode == TIOT_XMIT_RX_MODE_BUFFED) {
        read_len = xfer_rx_buff_read(xfer, buff, len);
    } else {
        read_len = xfer_direct_read(xfer, buff, len);
    }

    osal_mutex_unlock(&xfer->dev_mutex);
    return read_len;
}
