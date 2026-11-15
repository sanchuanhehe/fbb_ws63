/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: pb remobe client config.
 *
 * History:
 * 2025-6-5, Create file.
 */

#ifndef __REMOTE_PROV_CLIENT_H_
#define __REMOTE_PROV_CLIENT_H_

#include "model_def.h"
#include "mesh/main.h"
#include "prov_bearer.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_common.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_pb_remote_cli Bluetooth Mesh PB-Remote Client Model
 * @ingroup bt_mesh
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif

#define MSG_WAIT_TIMEOUT 2000U
/* 配网超时时间 (seconds) */
#define PROTOCOL_TIMEOUT_SEC 60
/* 默认链接超时时间 (seconds) */
#define LINK_TIMEOUT_SECONDS_DEFAULT 60

/* 未配网设备 URI hash value */
#define BT_MESH_RMT_PROV_UNPROV_DEV_HASH BIT(0)
#define BT_MESH_RMT_PROV_UNPROV_DEV_ACTIVE BIT(1)
#define BT_MESH_RMT_PROV_UNPROV_DEV_FOUND BIT(2)
#define BT_MESH_RMT_PROV_UNPROV_DEV_REPORTED BIT(3)
#define BT_MESH_RMT_PROV_UNPROV_DEV_EXT BIT(4)
#define BT_MESH_RMT_PROV_UNPROV_DEV_HAS_LINK BIT(5)
#define BT_MESH_RMT_PROV_UNPROV_DEV_EXT_ADV_RXD BIT(6)

#define BLE_MESH_RMT_PROV_SCAN_ABILITE_GET_LEN (2 + 4)
#define BLE_MESH_RMT_PROV_SCAN_GET_LEN (2 + 4)
#define BLE_MESH_RMT_PROV_SCAN_START_LEN (2 + 4 + 18)
#define BLE_MESH_RMT_PROV_LINK_GET_LEN (2 + 4)
#define BLE_MESH_RMT_PROV_LINK_OPEN_NODE_LEN (2 + 4 + 1)
#define BLE_MESH_RMT_PROV_LINK_OPEN_PROV_LEN (2 + 4 + 17)
#define BLE_MESH_RMT_PROV_LINK_CLOSE_LEN (2 + 4 + 1)
#define BLE_MESH_RMT_PROV_OPCODE_LEN (2 + 4)
#define BLE_MESH_RMT_PROV_URI_HASH_LEN (4)
#define BLE_MESH_RMT_PROV_MIN_PDU_LEN (3)

#define RMT_PROV_CTX(_srv, _send_rel)                                                                                 \
    {                                                                                                                 \
        .net_idx = (_srv)->net_idx, .app_idx = BT_MESH_KEY_DEV_REMOTE, .addr = (_srv)->addr, .send_ttl = (_srv)->ttl, \
        .send_rel = (_send_rel),                                                                                      \
    }

#define RMT_PROV_NODE(ctx)                                                          \
    {                                                                               \
        .addr = (ctx)->addr, .net_idx = (ctx)->net_idx, .ttl = BT_MESH_TTL_DEFAULT, \
    }

extern const struct bt_mesh_model_cb bt_mesh_rmt_prov_cli_cb;

extern const struct bt_mesh_model_op bt_mesh_rmt_prov_cli_op[];

struct rmt_prov_bearer_t {
    int link;
    const struct prov_bearer_cb *cb;
    struct bt_mesh_model_rmt_prov_cli *cli;
    struct {
        prov_bearer_send_complete_t cb;
    } tx;
};

struct bt_mesh_model_rmt_prov_node_t {
    uint16_t addr;
    uint16_t net_idx;
    uint8_t ttl;
};

#define BT_MESH_MODEL_RMT_PROV_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_RMT_PROV_CLI, bt_mesh_rmt_prov_cli_op, pub, cli_data, &bt_mesh_rmt_prov_cli_cb)

enum bt_mesh_rmt_prov_link_state {
    RMT_PROV_LINK_STATUS_IDLE = 0,
    RMT_PROV_LINK_STATUS_OPENING = 1,
    RMT_PROV_LINK_STATUS_ACTIVE = 2,
    RMT_PROV_LINK_STATUS_SENDING = 3,
    RMT_PROV_LINK_STATUS_CLOSING = 4,
    RMT_PROV_LINK_STATUS_RFU = 5,
};

enum bt_mesh_rmt_prov_node_nppi {
    BT_MESH_RMT_PROV_NODE_REFRESH_DEVKEY = 0,
    BT_MESH_RMT_PROV_NODE_REFRESH_ADDR,
    BT_MESH_RMT_PROV_NODE_REFRESH_COMPOSITION,
};

enum bt_mesh_rmt_prov_mes_reqs_status {
    RMT_PROV_SUCCESS = 0,
    RMT_PROV_SCAN_CANNOT_START,
    RMT_PROV_INVALID_STATE,
    RMT_PROV_LIM_RESOURC,
    RMT_PROV_LINK_CANNOT_OPEN,
    RMT_PROV_LINK_OPEN_FAILED,
    RMT_PROV_LINK_CLOSED_BY_DEV,
    RMT_PROV_LINK_CLOSED_BY_SRV,
    RMT_PROV_LINK_CLOSED_BY_CLI,
    RMT_PROV_LINK_AS_CANNOT_RECV_PDU,
    RMT_PROV_LINK_AS_CANNOT_SEND_PDU,
    RMT_PROV_LINK_AS_CANNOT_DELIVER_PDU_REPORT,
    RMT_PROV_SERVER_STATUS_RFU,
};

enum bt_mesh_rmt_prov_scan_status {
    RMT_PROV_SCAN_IDLE = 0,
    RMT_PROV_SCAN_MULTI_NODES,
    RMT_PROV_SCAN_SINGLE_NODE,
    RMT_PROV_SCAN_RFU,
};

enum bt_mesh_rmt_prov_link_bearer_status {
    RMT_PROV_BEARER_LINK_IDLE = 0,
    RMT_PROV_BEARER_LINK_OPENING,
    RMT_PROV_BEARER_LINK_OPENED,
};

enum bt_mesh_rmt_prov_bearer {
    BT_MESH_RMT_PROV_ADV = BIT(0),
    BT_MESH_RMT_PROV_GATT = BIT(1),
    BT_MESH_RMT_PROV_REMOTE = BIT(2),
};

enum link_sts {
    LINK_STS_SUCCESS = 0,
    LINK_STS_TIMEOUT,
    LINK_STS_FAIL,
};

/* 远程配网客户端模型 */
struct bt_mesh_model_rmt_prov_cli {
    struct bt_mesh_model *model;

    struct bt_mesh_msg_ack_ctx prov_msg_ack_ctx;  // prov msg ack

    struct {
        struct k_work_delayable timeout;
        struct bt_mesh_msg_ack_ctx ack_ctx;  // link msg ack
        struct bt_mesh_model_rmt_prov_node_t srv_node;
        uint8_t time;
        uint8_t tx_pdu;
        uint8_t rx_pdu;
        enum bt_mesh_rmt_prov_link_state state;
    } link;

    struct {
        struct bt_mesh_msg_ack_ctx ack_ctx;  // scan msg ack
        uint16_t timeout;
    } scan;
};

/* 节点刷新结构体定义 */
struct bt_mesh_rmt_prov_node_t {
    struct bt_mesh_model_rmt_prov_cli *cli;
    const struct bt_mesh_model_rmt_prov_node_t *srv_node;
    enum bt_mesh_rmt_prov_node_nppi nppi;
};

struct bt_mesh_rmt_prov_scan_sts_t {
    enum bt_mesh_rmt_prov_mes_reqs_status req_sts;
    uint8_t scan_sts;
    uint8_t max_report_dev;
    /* 扫描超时时间 */
    uint8_t timeout;
};

/* srv端远程扫描能力 */
struct bt_mesh_rmt_prov_abilities_sts_t {
    /* 最大扫描数 */
    uint8_t max_scanned_items;
    /* 是否支持主动扫描 */
    bool active_scan;
};

/* 未配网设备信息 */
struct bt_mesh_rmt_prov_unprov_dev_t {
    /* RSSI 信号强度 */
    int8_t rssi;
    /* UUID */
    uint8_t uuid[BLE_MESH_UUID_LEN];
    /* OOB 信息 */
    bt_mesh_prov_oob_info_t oob;
    /* URI Hash */
    uint32_t uri_hash;
    uint8_t flags;
};

struct bt_mesh_rmt_prov_link_t {
    /* link status */
    enum bt_mesh_rmt_prov_mes_reqs_status sts;
    /* state of the Link State Machine */
    enum bt_mesh_rmt_prov_link_state state;
};

typedef void (*bt_mesh_rmt_unprovisioned_beacon_recv)(
    uint8_t uuid[BLE_MESH_UUID_LEN], bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash, int8_t rssi);

/**
 * @if Eng
 * @brief Struct of BLE prov callback function.
 * @else
 * @brief BLE配网回调函数接口定义。
 * @endif
 */
typedef struct bt_mesh_rmt_prov {
    bt_mesh_rmt_unprovisioned_beacon_recv
        rmt_unprovisioned_beacon_recv; /*!< @if Eng unprovisioned beacon receive callback
                                             @else   收到未入网beacon报文回调函数 @endif */
} bt_mesh_rmt_prov_t;

int bt_mesh_rmt_prov_scan_abilities_get(const struct bt_mesh_model *model,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, struct bt_mesh_rmt_prov_abilities_sts_t *scan_abilities_data);

int bt_mesh_rmt_prov_scan_get(const struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_scan_sts_t *status);

int bt_mesh_cli_rmt_prov_scan_start(const struct bt_mesh_rmt_prov_node_t *ctx, uint8_t timeout,
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t max_scan_devs, struct bt_mesh_rmt_prov_scan_sts_t *status);

int bt_mesh_rmt_prov_scan_stop(const struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_scan_sts_t *scan_sts);

int bt_mesh_rmt_prov_link_get(struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link_rsp);

int bt_mesh_rmt_prov_link_close(struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link_rsp);

int32_t bt_mesh_rmt_prov_cli_timeout_get(void);

void bt_mesh_rmt_prov_cli_timeout_set(int32_t timeout);

void bt_mesh_rmt_proc_register(bt_mesh_rmt_prov_t *rmt_prov);

int bt_mesh_rmt_prov_start_scan(
    uint16_t net_idx, uint16_t rmt_addr, const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t items_limit, uint8_t timeout);

int bt_mesh_rmt_prov_adv_open(
    const uint8_t uuid[16], uint16_t net_idx, uint16_t rmt_addr, uint16_t prov_addr, uint8_t nppi);

int bt_mesh_rmt_prov_start_ex_scan(
    uint16_t net_idx, uint16_t rmt_addr, const uint8_t *uuid, uint8_t timeout, ble_mesh_rmt_prov_ex_scan_t *ad_filter);

void bt_mesh_rmt_prov_register(bt_mesh_rmt_prov_t *rmt_prov);

struct bt_mesh_model *bt_mesh_get_rmt_prov_cli_model(void);

#ifdef __cplusplus
}
#endif

#endif /* __PB_REMOTE_CLI_H_ */
