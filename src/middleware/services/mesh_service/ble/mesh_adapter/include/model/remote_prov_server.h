/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Remote provision Server Model Statement
 *
 * History:
 * 2025-6-4, Create file.
 */
#ifndef __REMOTE_PROV_SERVER_H__
#define __REMOTE_PROV_SERVER_H__

#include "model_def.h"
#include "mesh/main.h"
#include "prov.h"
#include "osal_addr.h"
#include "osal_list.h"
#include "ble_mesh_common.h"
#include "model/model_opcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MESH_RMT_PROV_ACTIVE_SCAN_EN 1
#define BT_MESH_RMT_PROV_AD_TYPE_LEN 16
#define BT_MESH_RMT_PROV_URI_LEN 14
#define BT_MESH_RMT_PROV_LINK_TIME_OUT 0x3c
#define BT_MESH_RMT_PROV_DATA_LEN_NPPI 1
#define BT_MESH_RMT_PROV_DATA_LEN_TIMEOUT 17
#define BT_MESH_RMT_PROV_DATA_LEN_UUID 16
#define BT_MESH_RMT_EX_SCAN_TIMEOUT_MAX 21
#define BT_MESH_RMT_DEV_AD_LEN 3

struct bt_mesh_rmt_prov_srv_t {
    struct bt_mesh_model *model;
    struct transition scan_time_transition;
    struct transition ex_scan_time_transition;
};

extern const struct bt_mesh_model_op bt_mesh_rmt_prov_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_rmt_prov_srv_cb;

#define BT_MESH_MODEL_RMT_PROV_SRV(srv) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_RMT_PROV_SRV, bt_mesh_rmt_prov_srv_op, NULL, srv, &bt_mesh_rmt_prov_srv_cb)

// scan start
typedef struct {
    uint8_t scan_items_limit;
    uint8_t timeout;
    uint8_t uuid_len;
    uint8_t uuid[BLE_MESH_UUID_LEN];  // 不存在时为多设备扫描
} bt_mesh_rmt_prov_scan_start_t;

enum {
    SUCCESS = 0,
    INVALID_PARAM,
    REPEAT_PARAM,
    OP_FAIL,
};

// RPScanningState
enum {
    RMT_PROV_SRV_SCAN_IDLE = 0,
    RMT_PROV_SRV_SCAN_MULTI_NODES,
    RMT_PROV_SRV_SCAN_SINGLE_NODE,
    RMT_PROV_SRV_SCAN_RFU,
};

// remote prov nppi
enum {
    RMT_PROV_SRV_DEV_KEY_REF = 0,
    RMT_PROV_SRV_NODE_ADDR_REF,
    RMT_PROV_SRV_NODE_COM_REF,
    RMT_PROV_SRV_NPPI_RFU,
};

// remote prov scan && link report status
enum {
    RMT_PROV_SRV_RSP_SUC = 0,
    RMT_PROV_SRV_SCAN_CANNOT_START,
    RMT_PROV_SRV_INVALID_STATE,
    RMT_PROV_SRV_LIMITED_RESOURCES,
    RMT_PROV_SRV_LINK_CANNOT_OPEN,
    RMT_PROV_SRV_LINK_OPEN_FAILED,
    RMT_PROV_SRV_LINK_CLOSE_BY_DEV,
    RMT_PROV_SRV_LINK_CLOSE_BY_SRV,
    RMT_PROV_SRV_LINK_CLOSE_BY_CLI,
    RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_RCV,
    RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND,
    RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_DELIV,
    RMT_PROV_SRV_LINK_RFU,
};

// remote prov link sta
enum {
    RMT_PROV_SRV_LINK_IDLE = 0,
    RMT_PROV_SRV_LINK_OPEN,
    RMT_PROV_SRV_LINK_ACTIVE,
    RMT_PROV_SRV_LINK_TRANS,
    RMT_PROV_SRV_LINK_CLOSE,
    RMT_PROV_SRV_LINK_PROHIBIT,
};

// remote prov link close resaon
enum {
    RMT_PROV_SRV_LINK_CLOSE_SUC = 0,
    RMT_PROV_SRV_LINK_CLOSE_TIMEOUT,
    RMT_PROV_SRV_LINK_CLOSE_FAIL,
    RMT_PROV_SRV_LINK_CLOSE_RFU,
};

/*
 * remote prov server sta record
 */

typedef struct {
    uint8_t scan_count;
    uint8_t status;                   // 扫描状态
    uint8_t rp_scanning_state;        // 设备扫描模式
    uint8_t scanned_items_limit;      // 扫描条目
    uint8_t timeout;                  // timeout
    uint8_t uuid[BLE_MESH_UUID_LEN];  // uuid
} bt_mesh_rmt_prov_scan_t;

typedef struct {
    uint8_t type;
    uint8_t data_len;
    const uint8_t *data;
} bt_mesh_rmt_prov_ex_scan_ad_t;

typedef struct {
    bool en;
    uint8_t ad_count;
    uint8_t ad_type_filter[BT_MESH_RMT_PROV_AD_TYPE_LEN];
    uint8_t uuid[BLE_MESH_UUID_LEN];  // uuid
    bt_addr_le_t addr;
    bool oob_en;
    bt_mesh_prov_oob_info_t oob_info;  // oob info
    bool uri_en;
    uint32_t uri_hash;
    uint8_t ad_has_count;
    struct os_mbuf *ad_data;
} bt_mesh_rmt_prov_ex_scan_dev_t;

typedef struct {
    uint8_t state;
    uint8_t timeout;  // timeout
    bt_mesh_rmt_prov_ex_scan_dev_t devs[MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM];
} bt_mesh_rmt_prov_ex_scan_sts_t;

typedef struct {
    uint8_t status;  // 消息状态
    uint8_t state;   // link状态
    uint8_t uuid[BLE_MESH_UUID_LEN];
    uint8_t timeout;
    int8_t rssi;
    uint16_t oob_info;
    uint32_t uri_hash;
    struct {
        uint8_t nppi_procedure;
        bool refreshing;
    } nppi;
    prov_bearer_send_complete_t send_complate_cb;
    void *send_complate_cb_data;
    struct prov_bearer_cb *rmt_prov_bearer_cb;
    void *rmt_prov_bearer_cb_data;
    uint8_t close_reason;
} bt_mesh_rmt_prov_link_sta_t;

typedef struct {
    uint8_t outbound;
    uint8_t inbound;
} bt_mesh_rmt_prov_pdu_sta_t;

// rsp dev list
typedef struct {
    struct osal_list_head entry;
    int8_t rssi;
    uint8_t uuid[BLE_MESH_UUID_LEN];
    uint16_t oob_info;
    uint32_t uri_hash;
} bt_mesh_rmt_prov_dev_list_t;

typedef struct {
    struct bt_mesh_model *model;
    struct bt_mesh_msg_ctx ctx;
    bt_mesh_rmt_prov_scan_t scan;            // 扫描
    struct osal_list_head dev_list;          // 已上报设备列表
    bt_mesh_rmt_prov_ex_scan_sts_t ex_scan;  // 扩展扫描
    bt_mesh_rmt_prov_link_sta_t link;        // 链接
    bt_mesh_rmt_prov_pdu_sta_t pdu_sta;      // out && in bound pdu sta
} bt_mesh_rmt_prov_srv_sta_t;

void bt_mesh_rmt_prov_unprovisioned_beacon(
    const bt_addr_le_t *addr, int8_t rssi, uint8_t *uuid, bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash);
void bt_mesh_rmt_prov_extend_unprovisioned_adv(
    uint8_t event_type, const bt_addr_le_t *addr, const uint8_t *data, uint8_t len);

void bt_mesh_rmt_prov_scan_state_get(void);
void bt_mesh_rmt_prov_link_state_get(void);
uint8_t bt_mesh_node_refresh_get(void);
uint8_t bt_mesh_link_close_reason_get(void);
int bt_mesh_comp_get_page_128(struct os_mbuf *buf);
uint8_t bt_mesh_rmt_prov_ex_scan_state_get(void);

typedef bool (* bt_mesh_rmt_prov_adv_recv_callback)(uint16_t src_addr, int8_t rssi,
                                                    uint32_t link_id, struct os_mbuf *buf);

int bt_mesh_rmt_prov_scan_start(struct bt_mesh_msg_ctx *ctx, bt_mesh_rmt_prov_scan_start_t *scan_start);
int bt_mesh_rmt_prov_pdu_report_send(bool send_rel, uint8_t inbound, const uint8_t *data, uint16_t len);
int bt_mesh_stop_rmt_scan(void);
void bt_mesh_rmt_prov_adv_recv_cb_set(bt_mesh_rmt_prov_adv_recv_callback recv_func);

#ifdef __cplusplus
}
#endif

#endif /* __REMOTE_PROV_SERVER_H__ */
