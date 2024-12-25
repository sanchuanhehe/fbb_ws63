/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE Service Access Protocol data struct defination.
 */

/**
 * @defgroup sle_ssap_server Service Access Protocol data struct defination.
 * @ingroup  SLE
 * @{
 */

#ifndef OH_SLE_SSAP_STRU_H
#define OH_SLE_SSAP_STRU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Length of sle UUID.
 * @else
 * @brief  星闪UUID长度。
 * @endif
 */
#define SLE_UUID_LEN 16

/**
 * @if Eng
 * @brief  Enum of write types for ssap clients to write property and descriptor.
 * @else
 * @brief  定义SSAP client写property和descriptor时的写类型。
 * @endif
 */
typedef enum {
    OH_SSAP_WRITE_NO_RSP           = 0x01, /*!< @if Eng type of wirte with no server response
                                             @else 只写，且不需要server回复响应 @endif */
    OH_SSAP_WRITE_DEFAULT_WITH_RSP = 0x02, /*!< @if Eng type of wirte with server response
                                             @else 只写，且需要server回复响应 @endif */
} SsapWriteType;

/**
 * @if Eng
 * @brief  SSAP attribute permission.
 * @else
 * @brief  SSAP attribute操作权限。
 * @endif
 */
typedef enum {
    OH_SSAP_PERMISSION_READ                = 0x01, /*!< @if Eng readable
                                                     @else   可读 @endif */
    OH_SSAP_PERMISSION_WRITE               = 0x02, /*!< @if Eng writable
                                                     @else   可写 @endif */
    OH_SSAP_PERMISSION_ENCRYPTION_NEED     = 0x04, /*!< @if Eng need encryption
                                                     @else   需要加密 @endif */
    OH_SSAP_PERMISSION_AUTHENTICATION_NEED = 0x08, /*!< @if Eng need authentication
                                                     @else   需要认证 @endif */
    OH_SSAP_PERMISSION_AUTHORIZATION_NEED  = 0x10, /*!< @if Eng need authorization
                                                     @else   需要授权 @endif */
} SsapPermissionType;

/**
 * @if Eng
 * @brief  SSAP find type.
 * @else
 * @brief  SSAP 查找类型
 * @endif
 */
typedef enum {
    OH_SSAP_FIND_TYPE_SERVICE_STRUCTURE = 0x00, /*!< @if Eng service structure
                                                  @else   服务结构 @endif */
    OH_SSAP_FIND_TYPE_PRIMARY_SERVICE   = 0x01, /*!< @if Eng primary service
                                                  @else   首要服务 @endif */
    OH_SSAP_FIND_TYPE_REFERENCE_SERVICE = 0x02, /*!< @if Eng reference service
                                                  @else   引用服务 @endif */
    OH_SSAP_FIND_TYPE_PROPERTY          = 0x03, /*!< @if Eng property
                                                  @else   属性 @endif */
    OH_SSAP_FIND_TYPE_METHOD            = 0x04, /*!< @if Eng method
                                                  @else   方法 @endif */
    OH_SSAP_FIND_TYPE_EVENT             = 0x05, /*!< @if Eng event
                                                  @else   事件 @endif */
} SsapFindType;

/**
 * @if Eng
 * @brief  SSAP property type.
 * @else
 * @brief  SSAP 特征类型。
 * @endif
 */
typedef enum {
    OH_SSAP_PROPERTY_TYPE_VALUE             = 0x00, /*!< @if Eng property value
                                                      @else   特征值 @endif */
    OH_SSAP_DESCRIPTOR_USER_DESCRIPTION     = 0x01, /*!< @if Eng user description descriptor
                                                      @else   属性说明描述符 @endif */
    OH_SSAP_DESCRIPTOR_CLIENT_CONFIGURATION = 0x02, /*!< @if Eng client configuration descriptor
                                                      @else   客户端配置描述符 @endif */
    OH_SSAP_DESCRIPTOR_SERVER_CONFIGURATION = 0x03, /*!< @if Eng server configuration descriptor
                                                      @else   服务端配置描述符 @endif */
    OH_SSAP_DESCRIPTOR_PRESENTATION_FORMAT  = 0x04, /*!< @if Eng presentation format descriptor
                                                      @else   格式描述符 @endif */
    OH_SSAP_DESCRIPTOR_RFU                  = 0x05, /*!< @if Eng RFU descriptor, 0x05 – 0x1F
                                                      @else   服务管理保留描述符，0x05 – 0x1F @endif */
    OH_SSAP_DESCRIPTOR_CUSTOM               = 0xFF, /*!< @if Eng custom descriptor
                                                      @else   厂商自定义描述符 @endif */
} SsapPropertyType;

/**
 * @if Eng
 * @brief  SSAP operation indication.
 * @else
 * @brief  SSAP 操作指示。
 * @endif
 */
typedef enum {
    OH_SSAP_OPERATE_INDICATION_BIT_READ = 0x01,              /*!< @if Eng data values can be read
                                                               @else   数据值可被读取 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP = 0x02,      /*!< @if Eng data values can be written without feedback.
                                                               @else   数据值可被写入，写入后无反馈 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_WRITE = 0x04,        /*!< @if Eng data values can be written. After being written,
                                                                       a feedback is generated to the client.
                                                               @else   数据值可被写入，写入后产生反馈给客户端 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_NOTIFY = 0x08,            /*!< @if Eng data value send to client by notification.
                                                               @else   数据值通过通知方式传递给客户端 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_INDICATE = 0x10,          /*!< @if Eng data value send to client by indication.
                                                               @else   数据值通过指示方式传递给客户端 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_BROADCAST = 0x20,         /*!< @if Eng data value can be carried in broadcast
                                                               @else   数据值可携带在广播中 @endif */
    OH_SSAP_OPERATE_INDICATION_BIT_DESCRITOR_WRITE = 0x100,  /*!< @if Eng data value description descriptor can be
                                                                       written
                                                               @else   数据值说明描述符可被写入 @endif */
    OH_SSAP_OPERATE_INDICATION_MAX,                          /*!< @if Eng maximum of operation indication
                                                               @else   操作指示最大值 @endif */
} SsapOperateIndication;

/**
 * @if Eng
 * @brief  UUID.
 * @else
 * @brief  通用唯一识别码。
 * @endif
 */
typedef struct {
    uint8_t len;                /*!< @if Eng uuid len
                                     @else   UUID 长度 @endif */
    uint8_t uuid[SLE_UUID_LEN]; /*!< @if Eng uuid
                                     @else   UUID字段 @endif */
} SleUuid;

/**
 * @if Eng
 * @brief  Struct of ssap info exchange
 * @else
 * @brief  ssap 信息交换结构体。
 * @endif
 */
typedef struct {
    uint32_t mtuSize; /*!< @if Eng mtu size
        ·                   @else   mtu大小 @endif */
    uint16_t version;  /*!< @if Eng version
                            @else   版本 @endif */
} SsapcExchangeInfo;

#ifdef __cplusplus
}
#endif
#endif /* OH_SLE_SSAP_STRU_H */
