/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Hello Server Config. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_DEVICE_CONFIG_SERVER_H
#define SLE_DEVICE_CONFIG_SERVER_H

#include <stdint.h>
#include "sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID */
#define SLE_DEVICE_CONFIG_SERVICE_UUID 0x3333

/* Property UUID */
#define SLE_DEVICE_CONFIG_NTF_REPORT_UUID 0x3434

/* Property Property */
#define SLE_DEVICE_CONFIG_TEST_PROPERTIES \
    (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE | SSAP_PERMISSION_AUTHORIZATION_NEED)

/* Operation indication */
#define SLE_DEVICE_CONFIG_TEST_OPERATION_INDICATION \
    (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SLE_DEVICE_CONFIG_TEST_DESCRIPTOR (SSAP_PERMISSION_READ)

errcode_t sle_device_config_server_init(void);

errcode_t sle_device_config_server_send_data(const uint8_t *data, uint16_t len);

uint16_t sle_device_config_server_is_connected(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
