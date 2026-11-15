/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: ble_mesh_errcode.h ble mesh 错误码定义
 */

/**
 * @defgroup bluetooth_bts_mesh_errocode BLE MESH ERROR CODE
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_ERRCODE_H
#define BLE_MESH_ERRCODE_H

#include <stdint.h>

#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* *
 * @if Eng
 * @brief  BLE MESH error code base.
 * @else
 * @brief  BLE MESH 错误码起始。
 * @endif
 */
#define ERRCODE_BLE_MESH_BASE 0x80006B00

/* *
 * @if Eng
 * @brief  BLE MESH error code base.
 * @else
 * @brief  BLE MESH 错误码结束。
 * @endif
 */
#define ERRCODE_BLE_MESH_END 0x80006BFF

/* *
 * @if Eng
 * @brief  BLE MESH error code.
 * @else
 * @brief  BLE MESH 错误码。
 * @endif
 */
typedef enum {
    ERRCODE_BLE_MESH_SUCCESS = 0,
    ERRCODE_BLE_MESH_FAIL = ERRCODE_BLE_MESH_BASE,            /* !< @if Eng error code of failure
                                                                    @else   执行失败错误码 @endif */
    ERRCODE_BLE_MESH_NOT_READY             = 0x80006B01,      /* !< @if Eng error code of not ready
                                                                    @else   执行状态未就绪错误码 @endif */
    ERRCODE_BLE_MESH_PARAM_ERR             = 0x80006B02,      /* !< @if Eng Invalid input parameters
                                                                    @else 参数错误。@endif */
    ERRCODE_BLE_MESH_UNSUPPORTED           = 0x80006B03,      /* !< @if Eng Feature not supported
                                                                    @else 功能不支持。@endif */
    ERRCODE_BLE_MESH_INITIALIZED           = 0x80006B04,      /* !< @if Eng Already initialized
                                                                    @else 已经初始化。@endif */
    ERRCODE_BLE_MESH_EXCEEDS_MAX_LEN       = 0x80006B05,      /* !< @if Eng Exceeds maximum length
                                                                    @else 超出最大长度。@endif */
    ERRCODE_BLE_MESH_STATUS_ERR            = 0x80006B06,      /* !< @if Eng Status error
                                                                    @else 状态错误。@endif */
    ERRCODE_BLE_MESH_ITEM_NOT_FOUND        = 0x80006B07,      /* !< @if Eng MESH node not found
                                                                    @else MESH节点未找到。@endif */
    ERRCODE_BLE_MESH_NOT_PROVISIONED       = 0x80006B08,      /* !< @if Eng MESH node not found
                                                                    @else MESH节点未入网。@endif */
    ERRCODE_BLE_MESH_MALLOC_FAILED         = 0x80006B09,      /* !< @if Eng Failed to allocate heap memory
                                                                    @else 堆内存申请失败。@endif */
    ERRCODE_BLE_MESH_MEMCPY_FAILED         = 0x80006B0A,      /* !< @if Eng Failed to copy memory
                                                                    @else 拷贝内存失败。@endif */
    ERRCODE_BLE_MESH_ALLOC_BUFF_FAILED     = 0x80006B0B,      /* !< @if Eng Failed to alloc msg buf
                                                                    @else 消息缓存申请失败。@endif */
    ERRCODE_BLE_MESH_RESOURCE_INSUFFICIENT = 0x80006B0C,      /* !< @if Eng Resource is insufficient
                                                                    @else 资源不足。@endif */
    ERRCODE_BLE_MESH_MODEL_NOT_FOUND       = 0x80006B0D,      /* !< @if Eng model not found
                                                                    @else 模型不存在。@endif */
    ERRCODE_BLE_MESH_TTL_NOT_INVALID       = 0x80006B0E,      /* !< @if Eng ttl not invalid
                                                                    @else TTL不合法。@endif */
    ERRCODE_BLE_MESH_NET_KEY_INVALID       = 0x80006B0F,      /* !< @if Eng net key not invalid
                                                                    @else 无效的net key。@endif */
    ERRCODE_BLE_MESH_APP_KEY_ALREADY_STORED = 0x80006B10,      /* !< @if Eng app key already store
                                                                    @else app key已经存在。@endif */
    ERRCODE_BLE_MESH_APP_KEY_CANNOT_SET    = 0x80006B11,      /* !< @if Eng app key cannot set
                                                                    @else app key无法设置。@endif */
    ERRCODE_BLE_MESH_BUSY                  = 0x80006B12,      /* !< @if Eng System is busy
                                                                    @else 系统繁忙。@endif */
    ERRCODE_BLE_MESH_MAX = ERRCODE_BLE_MESH_END,
} errcode_ble_mesh_t;

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */