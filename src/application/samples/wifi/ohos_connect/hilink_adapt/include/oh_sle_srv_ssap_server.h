/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE ssap server manager module.
 */
#ifndef SLE_SERVICE_SERVICE_ACCESS_PROTOCOL_SERVER_H
#define SLE_SERVICE_SERVICE_ACCESS_PROTOCOL_SERVER_H

#include "btsrv_task.h"
#include "sdk_list.h"
#include "sle_ssap_stru.h"
#include "bt_srv_msg.h"
#include "sle_ssap_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

#define btsrv_send_sle_ssaps_msg(id, p0, p1, p2) \
    btsrv_send_simple_msg(BTSRV_MSG_ID_SLE_SSAPS, \
    (uintptr_t)(id), (uintptr_t)(p0), (uintptr_t)(p1), (uintptr_t)(p2))
#define btsrv_errcode_base(ret) (((ret) == ERRCODE_GLE_SUCCESS) ? (ret) : ((ret) + ERRCODE_SLE_COMMON_BASE))

/* 本地设备管理模块消息定义 */
typedef enum {
    SLE_SSAPS_MSG_ID_SERVICE_ADD,
    SLE_SSAPS_MSG_ID_PROPERITY_ADD,
    SLE_SSAPS_MSG_ID_DESCRIPTOR_ADD,
    SLE_SSAPS_MSG_ID_START_SERVICE,
    SLE_SSAPS_MSG_ID_DELETE_ALL_SERVICE,
    SLE_SSAPS_MSG_ID_MTU_CHANGE,
    SLE_SSAPS_MSG_ID_REQUEST_READ,
    SLE_SSAPS_MSG_ID_REQUEST_WRITE,
} sle_ssaps_msg_id_t;

/**
 * @brief  bts 维护service状态枚举
 */
typedef enum {
    SLE_SSAP_SERVICE_UNKNOW            = 0x00,
    SLE_SSAP_SERVICE_NOT_ACTIVE        = 0x01,
    SLE_SSAP_SERVICE_STARTING          = 0x02,
    SLE_SSAP_SERVICE_ACTIVE            = 0x03,
} ssaps_service_t;

/**
 * @brief  星闪服务类型
 */
typedef enum {
    SLE_SSAP_STD_PRIMARY_SERVICE            = 0x00,    /*!< 星闪标准定义的首要服务 */
    SLE_SSAP_STD_SECOND_SERVICE             = 0x01,    /*!< 星闪标准定义的次要服务 */
    SLE_SSAP_STD_PROPERTY                   = 0x02,    /*!< 星闪标准定义的服务属性 */
    SLE_SSAP_STD_METHOD                     = 0x03,    /*!< 星闪标准定义的服务方法 */
    SLE_SSAP_STD_EVENT                      = 0x04,    /*!< 星闪标准定义的服务事件 */
    SLE_SSAP_STD_SERVICE_REFERENCE          = 0x05,    /*!< 引用星闪标准定义服务 */
    SLE_SSAP_STD_DESCRIPTOR                 = 0x06,    /*!< 星闪标准定义服务属性描述 */
    SLE_SSAP_STD_RFU                        = 0x07,    /*!< 保留后续扩展 */
    SLE_SSAP_CUST_PRIMARY_SERVICE           = 0x08,    /*!< 厂商自定义首要服务 */
    SLE_SSAP_CUST_SECOND_SERVICE            = 0x09,    /*!< 厂商自定义次要服务 */
    SLE_SSAP_CUST_PROPERTY                  = 0x0A,    /*!< 厂商自定义服务属性 */
    SLE_SSAP_CUST_METHOD                    = 0x0B,    /*!< 厂商自定义服务方法 */
    SLE_SSAP_CUST_EVENT                     = 0x0C,    /*!< 厂商自定义服务事件 */
    SLE_SSAP_CUST_SERVICE_REFERENCE         = 0x0D,    /*!< 厂商自定义服务引用 */
    SLE_SSAP_CUST_DESCRIPTOR                = 0x0E,    /*!< 厂商自定义服务属性描述 */
} ssap_table_type_t;

/**
 * @brief  bts 维护descriptor 节点，ssaps内部使用，禁止外部调用
 */
typedef struct {
    uint16_t handle;              /* 特征句柄 */
    uint16_t permission;          /* 特征描述符权限 */
    uint8_t type;                 /* 描述符类型, 参见ssap_property_descriptor_type_t定义 */
    SleUuid uuid;              /* 特征描述符uuid */
    uint32_t operateIndication;          /* 操作指示 */
    uint16_t len;                 /* 特征描述符值长度 */
    uint8_t *val;                 /* 特征描述符值 */
} ssaps_descriptor_node_t;

/**
 * @brief  bts 维护descriptor 链表，ssaps内部使用，禁止外部调用
 */
typedef struct {
    struct list_stru list; /* ssaps_descriptor_node_t */
} ssaps_descriptor_list_t;

/**
 * @brief  bts 维护property 节点，ssaps内部使用，禁止外部调用
 */
typedef struct {
    uint16_t handle;                    /* 特征句柄 */
    uint16_t permission;                /* 特征权限 */
    uint8_t type;                       /* 特征类型 */
    SleUuid uuid;                    /* 特征uuid */
    uint32_t operateIndication;                /* 操作指示 ssap_property_operate_indication_t */
    ssaps_descriptor_list_t descriptor; /* 包含描述符 */
    uint8_t protocol;                   /* see cs_service_type defination */

    uint16_t len;                       /* 特征值长度 */
    uint8_t *val;                       /* 特征值 */
} ssaps_property_node_t;

/**
 * @brief  bts 维护property 链表，ssaps内部使用，禁止外部调用
 */
typedef struct {
    struct list_stru list; /* ssaps_property_node_t */
} ssaps_property_list_t;

/**
 * @brief  bts 维护include 链表，ssaps内部使用，禁止外部调用
 */
typedef struct {
    struct list_stru list;
} ssaps_include_list_t;

/**
 * @brief  bts 维护service 节点，ssaps内部使用，禁止外部调用
 */
typedef struct serv_node {
    uint16_t handle;
    uint16_t endHandle;
    SleUuid uuid;
    uint8_t primary;            /* 是否是首要服务 */
    uint8_t active;             /* 是否开启 */

    ssaps_property_list_t property; /* 所含特征 */
    ssaps_include_list_t include;     /* 所含包含服务 */
    uint8_t service_type;
    uint8_t type_indication;    /* 服务成员类型指示 */
} ssaps_service_node_t;

/**
 * @brief  bts 维护service 链表，ssaps内部使用，禁止外部调用
 */
typedef struct {
    struct list_stru list; /* ssaps_service_node_t */
} ssaps_service_list_t;

/**
 * @brief  bts 维护server 节点，ssaps内部使用，禁止外部调用
 */
typedef struct {
    uint8_t serverId;
    SleUuid appUuid;
    ssaps_service_list_t service;
    uint16_t current_adding_service_hdl;
    uint16_t current_item_idle_hdl;
    uint16_t total_hdl_num;
    uint16_t current_add_item_type; /* ssap_item_type_t */
} ssaps_server_node_t;

/**
 * @brief  bts 维护server 链表，ssaps内部使用，禁止外部调用
 */
typedef struct {
    struct list_stru list;
} ssaps_server_list_t;

void btsrv_sle_ssaps_init(void);
void btsrv_sle_ssaps_deinit(void);
errcode_t btsrv_sle_ssap_server_callback_register(void);

/**
 * @brief  bts 设置当前添加节点，ssaps内部使用，禁止外部调用
 */
void btsrv_sle_set_current_starting_node(ssaps_server_node_t *srv_node, ssaps_service_node_t *node);
/**
 * @brief  bts 设置当前添加队列，ssaps内部使用，禁止外部调用
 */
struct list_stru *get_ssaps_start_service_queue(void);
#ifdef __cplusplus
}
#endif
#endif /* SLE_SERVICE_SERVICE_ACCESS_PROTOCOL_SERVER_H */
/**
 * @}
 */
