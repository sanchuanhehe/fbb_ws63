/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: TIoT errcode. \n
 *
 * History: \n
 * 2024-09-04, Create file. \n
 */
#ifndef TIOT_ERRCODE_H
#define TIOT_ERRCODE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_interface_errcode TIoT errcode.
 * @ingroup  tiot_common_interface
 * @{
 */

/*
 * (0x8000_9C00 - 0x8000_A000)
 * common:          (0x8000_9C00 - 0x8000_9C20) ~ 0x20
 * controller:      (0x8000_9C20 - 0x8000_9C40) ~ 0x20
 * fw_load:         (0x8000_9C40 - 0x8000_9C70) ~ 0x30
 * pm:              (0x8000_9C70 - 0x8000_9C90) ~ 0x20
 * transfer:        (0x8000_9C90 - 0x8000_9CC0) ~ 0x30
 * utils:           (0x8000_9CC0 - 0x8000_9CE0) ~ 0x20
 * device:          (0x8000_9CE0 - 0x8000_9D00) ~ 0x20
 */
#define ERRCODE_TIOT_SUCC                               0           /* Succ */
#define ERRCODE_TIOT_INVALID_PARAM                      0x80009C00  /* invalid param */
#define ERRCODE_TIOT_NOT_SUPPORT                        0x80009C01  /* operation not support */
#define ERRCODE_TIOT_WAIT_INIT_FAIL                     0x80009C02  /* osal wait init fail */
#define ERRCODE_TIOT_MUTEX_INIT_FAIL                    0x80009C03  /* osal mutex init fail */
#define ERRCODE_TIOT_MEMCPY_FAIL                        0x80009C04  /* memcpy fail */
#define ERRCODE_TIOT_MALLOC_FAIL                        0x80009C05  /* malloc fail */
#define ERRCODE_TIOT_WAIT_TIMEOUT_FAIL                  0x80009C06  /* osal_wait_timeout() err */
#define ERRCODE_TIOT_TIMED_OUT                          0x80009C07  /* operation timed out */
#define ERRCODE_TIOT_CHECK_KCONFIG                      0x80009C08  /* kconfig config err */
#define ERRCODE_TIOT_RETRY                              0x80009C09  /* need retry */
#define ERRCODE_TIOT_RETRY_FAIL                         0x80009C0A  /* reach max retries */
#define ERRCODE_TIOT_SPIN_LOCK_INIT_FAIL                0x80009C0B  /* osal spin lock init fail */
#define ERRCODE_TIOT_CTRL_INITED                        0x80009C20  /* Controller is inited */
#define ERRCODE_TIOT_CTRL_UNINITED                      0x80009C21  /* Controller is uninited */
#define ERRCODE_TIOT_CTRL_NOT_OPEN                      0x80009C22  /* Controller not open */
#define ERRCODE_TIOT_CTRL_TOO_MANY_OPEN                 0x80009C23  /* Controller reach max open count */
#define ERRCODE_TIOT_FW_LOAD_INVALID_ACK_LEN            0x80009C40  /* Firmware invalid ack len */
#define ERRCODE_TIOT_FW_LOAD_INVALID_ACK                0x80009C41  /* Firmware invalid ack */
#define ERRCODE_TIOT_FW_LOAD_INVALID_EXT_CMD            0x80009C42  /* Firmware invalid ext cmd */
#define ERRCODE_TIOT_FW_LOAD_INVALID_PRE_EXT_CMD        0x80009C43  /* Firmware invalid pre ext cmd */
#define ERRCODE_TIOT_FW_LOAD_EXT_CMD_FAIL               0x80009C44  /* Firmware ext cmd exec fail */
#define ERRCODE_TIOT_FW_LOAD_PRE_EXT_CMD_FAIL           0x80009C45  /* Firmware pre ext cmd exec fail */
#define ERRCODE_TIOT_FW_LOAD_INVALID_BOOT_STR           0x80009C46  /* Firmware hiburn recv boot str invalid */
#define ERRCODE_TIOT_FW_LOAD_QUIT                       0x80009C47  /* Firmware load quit */
#define ERRCODE_TIOT_FW_LOAD_CUST_ERR                   0x80009C48  /* Firmware customize err */
#define ERRCODE_TIOT_PM_FAIL                            0x80009C70  /* Pm event handler fail */
#define ERRCODE_TIOT_PM_IN_PROCESS                      0x80009C71  /* Pm in process, just put event in queue */
#define ERRCODE_TIOT_PM_INVALID_STATE                   0x80009C72  /* Pm state invalid */
#define ERRCODE_TIOT_PM_WAKE_LOCK_INIT_FAIL             0x80009C73  /* Pm board wake lock init fail */
#define ERRCODE_TIOT_XFER_OPEN_FAIL                     0x80009C90  /* Xfer xmit(uart/i2c/spi) open fail */
#define ERRCODE_TIOT_XFER_READ_FAIL                     0x80009C91  /* Xfer xmit read fail */
#define ERRCODE_TIOT_XFER_WRITE_FAIL                    0x80009C92  /* Xfer xmit write fail */
#define ERRCODE_TIOT_XFER_SET_CONFIG_FAIL               0x80009C93  /* Xfer set xmit config fail */
#define ERRCODE_TIOT_XFER_INVALID_XMIT_TYPE             0x80009C94  /* Xfer xmit type invalid */
#define ERRCODE_TIOT_XFER_NOT_OPEN                      0x80009C95  /* Xfer state not open */
#define ERRCODE_TIOT_XFER_OPENED                        0x80009C96  /* Xfer state is opened */
#define ERRCODE_TIOT_XFER_INVALID_SUBSYS                0x80009C97  /* Xfer pkt subsys id invalid */
#define ERRCODE_TIOT_XFER_INVALID_PKT_CONTEXT           0x80009C98  /* Xfer pkt context is null */
#define ERRCODE_TIOT_XFER_MULTI_READ                    0x80009C99  /* Xfer multi-thread read the same subsys msg */
#define ERRCODE_TIOT_XFER_INVALID_PAYLOAD_LEN           0x80009C9A  /* Xfer payload len invalid */
#define ERRCODE_TIOT_FILE_OPEN_FAIL                     0x80009CC0  /* File open fail */
#define ERRCODE_TIOT_FILE_SEEK_FAIL                     0x80009CC1  /* File seek fail */
#define ERRCODE_TIOT_FILE_READ_FAIL                     0x80009CC2  /* File read fail */
#define ERRCODE_TIOT_CIRC_BUF_READ_FAIL                 0x80009CC3  /* Circ buf read fail */
#define ERRCODE_TIOT_CIRC_BUF_WRITE_FAIL                0x80009CC4  /* Circ buf write fail */
#define ERRCODE_TIOT_CIRC_BUF_SPACE_NOT_ENOUGH          0x80009CC5  /* Circ buf free space not enough */
#define ERRCODE_TIOT_CIRC_QUEUE_FULL                    0x80009CC6  /* Circ queue is full */
#define ERRCODE_TIOT_DEV_INVALID                        0x80009CE0  /* Dev invalid device */
#define ERRCODE_TIOT_DEV_BOARD_INIT_FAIL                0x80009CE1  /* Dev board init fail */
#define ERRCODE_TIOT_DEV_NO_BOARD_INFO                  0x80009CE2  /* Dev no board info */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif