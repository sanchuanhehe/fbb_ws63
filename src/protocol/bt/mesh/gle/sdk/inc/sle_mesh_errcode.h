/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle_mesh_errcode.h sle mesh 错误码定义
 */

#ifndef SLE_MESH_ERRCODE_H
#define SLE_MESH_ERRCODE_H

/**
 * @if Eng
 * @brief  SLE MESH error code base.
 * @else
 * @brief  SLE MESH 错误码起始。
 * @endif
 */
#define ERRCODE_SLE_MESH_BASE 0x8000F000

/**
 * @if Eng
 * @brief  SLE MESH error code base.
 * @else
 * @brief  SLE MESH 错误码结束。
 * @endif
 */
#define ERRCODE_SLE_MESH_END  0x8000F0FF

/**
 * @if Eng
 * @brief  SLE MESH error code.
 * @else
 * @brief  SLE MESH 错误码。
 * @endif
 */
typedef enum {
    ERRCODE_SLE_MESH_SUCCESS = 0,
    ERRCODE_SLE_MESH_PARAM_ERR = ERRCODE_SLE_MESH_BASE, /*!< @if Eng Invalid input parameters
                                                             @else 参数错误。@endif */
    ERRCODE_SLE_MESH_UNSUPPORTED,                       /*!< @if Eng Feature not supported
                                                             @else 功能不支持。@endif */
    ERRCODE_SLE_MESH_INITIALIZED,                       /*!< @if Eng Already initialized
                                                             @else 已经初始化。@endif */
    ERRCODE_SLE_MESH_UNINITIALIZED,                     /*!< @if Eng Not initialized
                                                             @else 未初始化。@endif */
    ERRCODE_SLE_MESH_EXCEEDS_MAX_LEN,                   /*!< @if Eng Exceeds maximum length
                                                             @else 超出最大长度。@endif */
    ERRCODE_SLE_MESH_STATUS_ERR,                        /*!< @if Eng Status error
                                                             @else 状态错误。@endif */
    ERRCODE_SLE_MESH_ITEM_NOT_FOUND,                    /*!< @if Eng MESH node not found
                                                             @else MESH结点未找到。@endif */
    ERRCODE_SLE_MESH_MALLOC_FAILED,                     /*!< @if Eng Failed to allocate heap memory
                                                             @else 堆内存申请失败。@endif */
    ERRCODE_SLE_MESH_MEMCPY_FAILED,                     /*!< @if Eng Failed to copy memory
                                                             @else 拷贝内存失败。@endif */
    ERRCODE_SLE_MESH_ACCESS_DENIED,                     /*!< @if Eng Access denied due to device role.
                                                             @else 拒绝访问，角色非法。 @endif */
    ERRCODE_SLE_MESH_TRANS_BUSY,                        /*!< @if Eng sle mesh trans channel busy.
                                                             @else 传输通道繁忙 @endif */
    ERRCODE_SLE_MESH_MAX = ERRCODE_SLE_MESH_END,
    ERRCODE_SLE_MESH_FAIL = 0xFFFFFFFF
} errcode_sle_mesh_t;

#endif /* end of SLE_MESH_ERRCODE_H */