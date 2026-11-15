/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: mesh common config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __MESH_COMM_H__
#define __MESH_COMM_H__

#include "mesh/slist.h"
#include "bts_le_gap.h"
#include "bts_gatt_server.h"
#include "atomic_adapter.h"
#include "mesh_task.h"
#define MESH_UNUSED(x) (void)(x)
#define BLE_MESH_HCI_VERSION_5_0 9

/* advertising channel map */
#define BLE_MESH_ADV_CHNL_37 BIT(0)
#define BLE_MESH_ADV_CHNL_38 BIT(1)
#define BLE_MESH_ADV_CHNL_39 BIT(2)

#define BLE_NPL_TIME_FOREVER    INT32_MAX
#define TASK_DEFAULT_PRIORITY       1
#define TASK_DEFAULT_STACK          NULL
#define TASK_DEFAULT_STACK_SIZE     400
#define MS_PER_SEC 1000

#define LEN_UUID_16 2
#define LEN_UUID_32 4
#define LEN_UUID_128 16

#define BLE_MESH_GATTS_APP_UUID_BYTE 0x96
#define BLE_UUID_MAX_LEN 16
#define BT_UUID_MESH_PROXY_VAL 0x1828
#define BT_UUID_MESH_PROXY_DATA_IN_VAL 0x2add

#ifndef BLE_MESH_SUCCESS
#define BLE_MESH_SUCCESS 0
#endif
#ifndef BLE_MESH_FAIL
#define BLE_MESH_FAIL (-1)
#endif

#define BLE_GATT_SVC_COUNT 2
#define BLE_GATT_SVC_HANDLE_NUM 6

/* bt_mesh_dev flags: the flags defined here represent BT controller state */
enum {
    BLE_MESH_DEV_ADVERTISING,
    BLE_MESH_DEV_KEEP_ADVERTISING,
    BLE_MESH_DEV_SCANNING,
    BLE_MESH_DEV_FLAGS_END,
};

struct bt_mesh_queue {
    uint32_t handle;
};

/**
 * @ingroup bt_def
 *
 * Struct of UUID. CNcomment:定义UUID结构。CNend
 */
typedef struct {
    unsigned char uuid_len; /**< length of uuid CNcomment:蓝牙地址 CNend */
    char *uuid;             /**< uuid CNcomment:UUID字段 CNend */
} bt_uuid;

typedef struct bt_mesh_msg {
    void *arg;          /* Pointer to the struct net_buf */
} bt_mesh_msg_t;

struct ble_gap_scan_params {
    /** Scan interval in 0.625ms units */
    uint16_t itvl;

    /** Scan window in 0.625ms units */
    uint16_t window;

    /** Scan filter policy */
    uint8_t filter_policy;

    /** If limited discovery procedure should be used */
    uint8_t limited;

    /** If passive scan should be used */
    uint8_t passive;

    /** If enable duplicates filtering */
    uint8_t filter_duplicates;
};

/* State tracking for the local Bluetooth controller */
struct bt_mesh_dev {
    /* Flags indicate which functionality is enabled */
    ATOMIC_DEFINE(flags, BLE_MESH_DEV_FLAGS_END);

    /* Controller version & manufacturer information */
    uint8_t hci_version;
    uint8_t res[3];
};

struct bt_mesh_conn_cb {
    /* A new connection has been established */
    void (*connected)(uint16_t conn_id);

    /* A connection has been disconnected */
    void (*disconnected)(uint16_t conn_id, uint8_t reason);

    void (*gatt_mtu_change)(uint16_t conn_id, uint16_t mtu);
};

#define OCTET_BIT_LEN 8
#define UUID_LEN_2     2
#define BT_INDEX_1     1
#define BT_INDEX_2     2
#define BT_INDEX_3     3
#define BT_INDEX_4     4
#define BT_INDEX_5     5
#define BT_INDEX_0     0

#endif