/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: PB Remote Client Model Function Implementation
 *
 * History:
 * 2025-6-5, Create file.
 */
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "ble_mesh_prov.h"
#include "provisioner.h"
#include "access.h"
#include "remote_prov_client.h"

static int32_t tx_timeout = MSG_WAIT_TIMEOUT;  // 默认发送后接收等待时间

struct rmt_prov_bearer_t rmt_prov_cli_bearer;
struct bt_mesh_model_rmt_prov_cli *bt_mesh_rmt_prov_cli;
struct bt_mesh_rmt_prov g_rmt_prov = {.rmt_unprovisioned_beacon_recv = NULL};

static void link_reset(struct bt_mesh_model_rmt_prov_cli *cli);
static void link_closed(struct bt_mesh_model_rmt_prov_cli *cli, enum bt_mesh_rmt_prov_mes_reqs_status status);
static void link_report(struct bt_mesh_model_rmt_prov_cli *cli, struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link);
static int bt_mesh_rmt_prov_send_wait(struct bt_mesh_model_rmt_prov_cli *cli,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, struct os_mbuf *msg, uint16_t rx_opcode, void *rx_ctx_data);

struct bt_mesh_model *bt_mesh_get_rmt_prov_cli_model(void)
{
    if (bt_mesh_rmt_prov_cli) {
        return bt_mesh_rmt_prov_cli->model;
    }

    return NULL;
}

static int bt_mesh_rmt_prov_send_wait(struct bt_mesh_model_rmt_prov_cli *cli,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, struct os_mbuf *msg, uint16_t rx_opcode, void *rx_ctx_data)
{
    struct bt_mesh_msg_ack_ctx *ack = NULL;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_msg_ctx ctx = RMT_PROV_CTX(srv_node, false);

    if (rx_opcode == BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA) {
        ack = &cli->link.ack_ctx;
    } else {
        ack = &cli->scan.ack_ctx;
    }

    int err = bt_mesh_msg_ack_ctx_prepare(ack, rx_opcode, srv_node->addr, rx_ctx_data);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Ack_ctx prepare fail! opcode: %d", rx_opcode);
        goto err;
    }

    err = bt_mesh_msg_send(&ctx, msg, bt_mesh_model_elem(cli->model)->addr, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Send msg failed (wait opcode: %d) (err %d)", rx_opcode, err);
        goto err;
    }

    err = bt_mesh_msg_ack_ctx_wait(ack, K_MSEC(tx_timeout));
    if (err != BLE_MESH_SUCC) {
        BT_ERR("Timeout (wait opcode: %d) (err %d)", rx_opcode, err);
        goto err;
    }
    return BLE_MESH_SUCC;

err:
    bt_mesh_msg_ack_ctx_clear(ack);
    link_closed(cli, RMT_PROV_LINK_AS_CANNOT_SEND_PDU);
    return err;
}

int bt_mesh_rmt_prov_scan_abilities_get(const struct bt_mesh_model *model,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, struct bt_mesh_rmt_prov_abilities_sts_t *scan_abilities_data)
{
    BT_DBG("bt_mesh_rmt_prov_scan_abilities_get send!");
    if (model == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_SCAN_ABILITE_GET_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_GET);

    int err = bt_mesh_rmt_prov_send_wait(
        cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_STA, scan_abilities_data);

    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_rmt_prov_scan_abilities_sts(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_rmt_prov_abilities_sts_t *scan_abilities_sts;

    if (!bt_mesh_msg_ack_ctx_match(&cli->scan.ack_ctx,
                                   BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_STA,
                                   ctx->addr,
                                   (void **)&scan_abilities_sts)) {
        BT_WARN("Scan_abilities opcode or node address mismatch: opcode (%u) 0x%04x", cli->scan.ack_ctx.op, ctx->addr);
        return 0;
    }

    scan_abilities_sts->max_scanned_items = net_buf_simple_pull_u8(msg);
    scan_abilities_sts->active_scan = net_buf_simple_pull_u8(msg);

    BT_DBG("The maximum of that can be reported during scanning: %u scan is supported: %u",
        scan_abilities_sts->max_scanned_items,
        scan_abilities_sts->active_scan);

    bt_mesh_msg_ack_ctx_rx(&cli->scan.ack_ctx);

    return 0;
}

int bt_mesh_rmt_prov_scan_get(const struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_scan_sts_t *status)
{
    if (model == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_SCAN_GET_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_GET);

    int err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA, status);

    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_rmt_prov_scan_sts(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_rmt_prov_scan_sts_t *status;
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    if (!bt_mesh_msg_ack_ctx_match(&cli->scan.ack_ctx,
                                   BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA,
                                   srv_node.addr,
                                   (void **)&status)) {
        BT_ERR("Unexpected scan status from op:0x%x dst:0x%x dst_in0x%04x",
            cli->scan.ack_ctx.op,
            cli->scan.ack_ctx.dst,
            srv_node.addr);
        return 0;
    }

    status->req_sts = net_buf_simple_pull_u8(msg);
    status->scan_sts = net_buf_simple_pull_u8(msg);
    status->max_report_dev = net_buf_simple_pull_u8(msg);
    status->timeout = net_buf_simple_pull_u8(msg);

    BT_DBG("req_sts: %u remote prov scan_sts: %u max_devs: %u timeout: %u seconds",
        status->req_sts,
        status->scan_sts,
        status->max_report_dev,
        status->timeout);
    bt_mesh_msg_ack_ctx_rx(&cli->scan.ack_ctx);

    return 0;
}

static int bt_mesh_rmt_prov_scan_report(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_rmt_prov_unprov_dev_t device = {0};

    BT_DBG("rmt cli get report data_len is %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    device.rssi = net_buf_simple_pull_u8(msg);

    errno_t ret =
        memcpy_s(device.uuid, BLE_MESH_UUID_LEN, net_buf_simple_pull_mem(msg, BLE_MESH_UUID_LEN), BLE_MESH_UUID_LEN);
    if (ret != EOK) {
        BT_ERR("bt_mesh_rmt_prov_scan_report,  memcpy uuid error!");
        return -EINVAL;
    }
    device.oob = net_buf_simple_pull_le16(msg);
    if (msg->om_len == BLE_MESH_RMT_PROV_URI_HASH_LEN) {
        ret = memcpy_s(&device.uri_hash,
            BLE_MESH_RMT_PROV_URI_HASH_LEN,
            net_buf_simple_pull_mem(msg, BLE_MESH_RMT_PROV_URI_HASH_LEN),
            BLE_MESH_RMT_PROV_URI_HASH_LEN);
        if (ret != EOK) {
            BT_ERR("bt_mesh_rmt_prov_scan_report,  memcpy uri_hash error!");
            return -EINVAL;
        }
        device.flags = BT_MESH_RMT_PROV_UNPROV_DEV_HASH;
    } else {
        return -EINVAL;
    }

    if (g_rmt_prov.rmt_unprovisioned_beacon_recv != NULL) {
        g_rmt_prov.rmt_unprovisioned_beacon_recv(device.uuid, device.oob, &device.uri_hash, device.rssi);
    }

    BT_DBG(
        "Server addr: 0x%04x:\nuuid:%s\nOOB:0x%04x", srv_node.addr, bt_hex(device.uuid, BLE_MESH_UUID_LEN), device.oob);

    return 0;
}

static int bt_mesh_rmt_prov_ex_scan_report(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    struct bt_mesh_rmt_prov_unprov_dev_t device = {0};
    uint8_t status = 0;

    status = net_buf_simple_pull_u8(msg);
    if (status != RMT_PROV_SUCCESS) {
        BT_WARN("scan report fail (%u)", status);
        return 0;
    }

    memcpy_s(device.uuid, BLE_MESH_UUID_LEN, net_buf_simple_pull_mem(msg, BLE_MESH_UUID_LEN), BLE_MESH_UUID_LEN);
    if (msg->om_len >= 2) { // 2:oob len
        device.oob = net_buf_simple_pull_le16(msg);
        BT_DBG("0x%04x: %s oob: 0x%04x adv data: %s",
            srv_node.addr,
            bt_hex(device.uuid, BLE_MESH_UUID_LEN),
            device.oob,
            bt_hex(msg->om_data, msg->om_len));
    } else {
        BT_WARN("0x%04x: %s not found.", srv_node.addr, bt_hex(device.uuid, BLE_MESH_UUID_LEN));
    }

    if (g_rmt_prov.rmt_unprovisioned_beacon_recv != NULL) {
        g_rmt_prov.rmt_unprovisioned_beacon_recv(device.uuid, device.oob, &device.uri_hash, device.rssi);
    }

    return 0;
}

int bt_mesh_cli_rmt_prov_scan_start(const struct bt_mesh_rmt_prov_node_t *ctx, uint8_t timeout,
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t max_scan_devs, struct bt_mesh_rmt_prov_scan_sts_t *status)
{
    struct bt_mesh_model_rmt_prov_cli *cli = ctx->cli;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_SCAN_START_LEN);  // 长度未定
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_START);

    net_buf_simple_add_u8(msg, max_scan_devs);
    net_buf_simple_add_u8(msg, timeout);
    if (uuid) {
        net_buf_simple_add_mem(msg, uuid, BLE_MESH_UUID_LEN);
    }

    int err = bt_mesh_rmt_prov_send_wait(cli, ctx->srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA, status);

    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_cli_rmt_prov_ex_scan_start(const struct bt_mesh_rmt_prov_node_t *node, uint8_t timeout,
    const uint8_t *uuid, ble_mesh_rmt_prov_ex_scan_t *ad_filter)
{
    int err = 0;
    struct bt_mesh_model_rmt_prov_cli *cli = node->cli;
    if (cli == NULL || ad_filter == NULL) {
        BT_ERR("Remote prov client model or ad_filter not exist!");
        return -EINVAL;
    }
    struct bt_mesh_msg_ctx ctx = RMT_PROV_CTX(node->srv_node, false);

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_SCAN_START_LEN);  // 长度未定
    if (!msg) {
        BT_ERR("net buf simple failed");
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_EXTEND_SCAN_START);

    net_buf_simple_add_u8(msg, ad_filter->ad_count);
    net_buf_simple_add_mem(msg, ad_filter->ad_type, ad_filter->ad_count);
    if (uuid) {
        net_buf_simple_add_mem(msg, uuid, BLE_MESH_UUID_LEN);
        net_buf_simple_add_u8(msg, timeout);
    }

    err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
    if (!err) {
        BT_ERR("rmt prov ex scan start failed:%d", err);
    }
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_rmt_prov_scan_stop(const struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_scan_sts_t *scan_sts)
{
    if (model == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_SCAN_GET_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STOP);

    int err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA, scan_sts);

    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_rmt_prov_link_get(struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link_rsp)
{
    if (model == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_LINK_GET_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_GET);

    int err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA, link_rsp);

    os_mbuf_free_chain(msg);
    return err;
}

/* 链接状态 */
static int bt_mesh_rmt_prov_link_sts(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    struct bt_mesh_rmt_prov_link_t *link_rsp;
    struct bt_mesh_rmt_prov_link_t link;

    link.sts = net_buf_simple_pull_u8(msg);
    link.state = net_buf_simple_pull_u8(msg);
    BT_DBG("srv_node_addr 0x%04x: status: %u state: %u", srv_node.addr, link.sts, link.state);

    if (bt_mesh_msg_ack_ctx_match(&cli->link.ack_ctx,
                                  BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA,
                                  srv_node.addr,
                                  (void **)&link_rsp)) {
        *link_rsp = link;
        bt_mesh_msg_ack_ctx_rx(&cli->link.ack_ctx);
    }
    if (cli->link.srv_node.addr == srv_node.addr) {
        k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));

        cli->link.state = link.state;
        if (link.state == RMT_PROV_LINK_STATUS_IDLE) {
            cli->link.srv_node.addr = BT_MESH_ADDR_UNASSIGNED;
        }
        if (link.sts != RMT_PROV_SUCCESS) {
            link_reset(cli);
            BT_ERR("Remote prov client model not exist!");
        }
    }
    return 0;
}

/* 链接报告 */
static int bt_mesh_rmt_prov_link_report(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    struct bt_mesh_rmt_prov_link_t link;
    uint8_t reason = LINK_STS_SUCCESS;  // 默认成功

    BT_DBG("rmt cli get link report data_len is %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    link.sts = net_buf_simple_pull_u8(msg);
    link.state = net_buf_simple_pull_u8(msg);
    if (msg->om_len) {
        reason = net_buf_simple_pull_u8(msg);
    }

    BT_DBG("Link report: srv_node_addr 0x%04x: status: %u state: %u reason: %u",
        srv_node.addr,
        link.sts,
        link.state,
        reason);

    if (cli->link.srv_node.addr != srv_node.addr) {
        BT_ERR("Link report from unknown server 0x%04x", srv_node.addr);
        return 0;
    }

    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));

    cli->link.state = link.state;
    link_report(cli, &cli->link.srv_node, &link);

    return 0;
}

int bt_mesh_rmt_prov_link_close(struct bt_mesh_model *model, const struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link_rsp)
{
    if (model == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_LINK_CLOSE_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_CLOSE);
    net_buf_simple_add_u8(msg, LINK_STS_SUCCESS);

    int err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA, link_rsp);

    os_mbuf_free_chain(msg);
    return err;
}

/* 发送的PDU成功出站报告 */
static int bt_mesh_rmt_prov_pdu_ob_report(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    BT_DBG("pdu rcv data len %d, hex %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));
    uint8_t ob_pdu_num;  // 成功出站PDU的数量
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    void *cb_data = NULL;

    if (cli->link.srv_node.addr != srv_node.addr) {
        BT_WARN("Recv outbound PDU data srv_node addr error! 0x%04x", srv_node.addr);
        return 0;
    }

    ob_pdu_num = net_buf_simple_pull_u8(msg);
    BT_DBG("node: 0x%04x outbound PDU numbers is %u", srv_node.addr, ob_pdu_num);

    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));

    if (ob_pdu_num != cli->link.tx_pdu) {
        BT_WARN("Mismatch between outbound and sent PDU counts! tx_ob_pdu: %u, report_ob_pdu: %u",
            cli->link.tx_pdu,
            ob_pdu_num);  // 报告的出站PDU与发送的PDU不匹配
        return 0;
    }

    cli->link.tx_pdu++;
    if (rmt_prov_cli_bearer.tx.cb) {
        BT_DBG("pdu rcv data len %d, hex %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));
        /* 发送下一个PDU */
        rmt_prov_cli_bearer.tx.cb(0, cb_data);
    }

    return 0;
}
static int rmt_prov_send(struct os_mbuf *msg, prov_bearer_send_complete_t cb, void *cb_data);
static int rmt_prov_cli_link_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], int32_t timeout, const struct prov_bearer_cb *cb, void *cb_data);
static void rmt_prov_link_close(enum prov_bearer_link_status status);
static void rmt_clear_tx(void);

const struct prov_bearer bt_mesh_rmt_prov_bearer_cli = {
    .type = BT_MESH_RMT_PROV_REMOTE,
    .send = rmt_prov_send,
    .clear_tx = rmt_clear_tx,
    .link_open = rmt_prov_cli_link_open,
    .link_close = rmt_prov_link_close,
};

/* 发送的PDU接收报告 */
static int bt_mesh_rmt_prov_pdu_report(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    BT_DBG("pdu rcv data len %d, hex %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));
    uint8_t prov_pdu;
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = RMT_PROV_NODE(ctx);
    struct bt_mesh_rmt_prov_node_t cb_ctx = {
        cli,
        &cli->link.srv_node,
        BT_MESH_RMT_PROV_NODE_REFRESH_DEVKEY
    };

    if (cli->link.srv_node.addr != srv_node.addr) {
        BT_ERR("Recv PDU srv_node addr error! 0x%04x", srv_node.addr);
        return -EINVAL;
    }

    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));

    prov_pdu = net_buf_simple_pull_u8(msg);
    if (prov_pdu <= cli->link.rx_pdu) {
        BT_ERR("PDU duplicate! %u", prov_pdu);
        return -EINVAL;
    }

    BT_DBG("srv_node: 0x%04x: pdu(%u) data_len(%u bytes)", srv_node.addr, prov_pdu, msg->om_len);

    if (rmt_prov_cli_bearer.cb != NULL) {
        rmt_prov_cli_bearer.cb->recv(&bt_mesh_rmt_prov_bearer_cli, &cb_ctx, msg);
    } else {
        BT_ERR("rmt_prov_cli_bearer.cb is NULL!");
        return -EINVAL;
    }

    return 0;
}

const struct bt_mesh_model_op bt_mesh_rmt_prov_cli_op[] = {
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_STA, BT_MESH_LEN_EXACT(2), bt_mesh_rmt_prov_scan_abilities_sts},
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA, BT_MESH_LEN_EXACT(4), bt_mesh_rmt_prov_scan_sts},
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_RSP, BT_MESH_LEN_MIN(19), bt_mesh_rmt_prov_scan_report},
    {BLE_MESH_MODEL_OP_RMT_PROV_EXTEND_SCAN_RSP, BT_MESH_LEN_MIN(17), bt_mesh_rmt_prov_ex_scan_report},
    {BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA, BT_MESH_LEN_EXACT(2), bt_mesh_rmt_prov_link_sts},
    {BLE_MESH_MODEL_OP_RMT_PROV_LINK_RSP, BT_MESH_LEN_MIN(2), bt_mesh_rmt_prov_link_report},
    {BLE_MESH_MODEL_OP_RMT_PROV_PDU_OUNBOUND_RSP, BT_MESH_LEN_EXACT(1), bt_mesh_rmt_prov_pdu_ob_report},
    {BLE_MESH_MODEL_OP_RMT_PROV_PDU_RSP, BT_MESH_LEN_MIN(2), bt_mesh_rmt_prov_pdu_report},
    BT_MESH_MODEL_OP_END,
};

static void link_report(struct bt_mesh_model_rmt_prov_cli *cli, struct bt_mesh_model_rmt_prov_node_t *srv_node,
    struct bt_mesh_rmt_prov_link_t *link)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }
    struct bt_mesh_rmt_prov_node_t ctx = {cli, srv_node, 0};

    if (link->state == RMT_PROV_LINK_STATUS_ACTIVE && rmt_prov_cli_bearer.link == RMT_PROV_BEARER_LINK_OPENING) {
        rmt_prov_cli_bearer.link = RMT_PROV_BEARER_LINK_OPENED;
        BT_ERR("Link opened !");
        rmt_prov_cli_bearer.cb->link_opened(&bt_mesh_rmt_prov_bearer_cli, &ctx);

        /* 默认超时时间 */
        cli->link.time = PROTOCOL_TIMEOUT_SEC;
        return;
    }

    if (link->state == RMT_PROV_LINK_STATUS_IDLE && rmt_prov_cli_bearer.link != RMT_PROV_BEARER_LINK_IDLE) {
        rmt_prov_cli_bearer.link = RMT_PROV_BEARER_LINK_IDLE;

        BT_ERR("Link closed !(%u)", link->sts);
        rmt_prov_cli_bearer.cb->link_closed(&bt_mesh_rmt_prov_bearer_cli,
            &ctx,
            ((link->sts == RMT_PROV_SUCCESS) ? PROV_BEARER_LINK_STATUS_SUCCESS : PROV_BEARER_LINK_STATUS_FAIL));
    }

    if (link->state == RMT_PROV_LINK_STATUS_IDLE) {
        link_reset(cli);
    }
}

static void link_timeout_work_handler(struct ble_npl_event *work)
{
    struct bt_mesh_model_rmt_prov_cli *cli = ble_npl_event_get_arg(work);

    if (cli == NULL) {
        BT_ERR("timeout! bt_mesh_model_rmt_prov_cli is NULL.");
        return;
    }
    BT_DBG("");

    if (rmt_prov_cli_bearer.link != RMT_PROV_BEARER_LINK_IDLE) {
        BT_DBG("Timeout");
        link_closed(cli, RMT_PROV_LINK_CLOSED_BY_CLI);
    }
}
static int rmt_prov_model_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int bt_mesh_rmt_prov_model_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("No remote prov client!");
        return -EINVAL;
    }

    k_work_cancel_delayable_delete((&cli->link.timeout));

    return 0;
}

static int bt_mesh_rmt_prov_model_cli_init(struct bt_mesh_model *model)
{
    struct bt_mesh_model_rmt_prov_cli *cli = model->user_data;
    if (!cli) {
        BT_ERR("No remote prov client!");
        return -EINVAL;
    }

    model->keys[0] = BT_MESH_KEY_DEV_ANY;
    model->flags |= BT_MESH_MOD_DEVKEY_ONLY;
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = rmt_prov_model_pub_update;
    }
    cli->link.time = LINK_TIMEOUT_SECONDS_DEFAULT;

    if (bt_mesh_rmt_prov_cli == NULL) {
        bt_mesh_rmt_prov_cli = cli;
    }

    bt_mesh_msg_ack_ctx_init(&cli->scan.ack_ctx);
    bt_mesh_msg_ack_ctx_init(&cli->link.ack_ctx);
    k_work_init_delayable((&cli->link.timeout), link_timeout_work_handler);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_rmt_prov_cli_cb = {
    .init = bt_mesh_rmt_prov_model_cli_init,
    .deinit = bt_mesh_rmt_prov_model_cli_deinit,
};

static void bt_mesh_rmt_prov_pdu_send_start(uint16_t keep_time, int err, void *cb_data)
{
    struct bt_mesh_model_rmt_prov_cli *cli = cb_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }

    if (err) {
        BT_ERR("PDU Send failed: %d", err);
        link_closed(cli, RMT_PROV_LINK_AS_CANNOT_SEND_PDU);
    }
}

static void bt_mesh_rmt_prov_pdu_send_end(int err, void *cb_data)
{
    struct bt_mesh_model_rmt_prov_cli *cli = cb_data;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }

    if (err) {
        BT_ERR("PDU Send failed: %d", err);
        link_closed(cli, RMT_PROV_LINK_AS_CANNOT_SEND_PDU);
        return;
    }

    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));
}

static const struct bt_mesh_send_cb bt_mesh_rmt_prov_pdu_send_cb = {
    .start = bt_mesh_rmt_prov_pdu_send_start,
    .end = bt_mesh_rmt_prov_pdu_send_end,
};

static void bt_mesh_rmt_prov_link_init(
    struct bt_mesh_model_rmt_prov_cli *cli, const struct bt_mesh_model_rmt_prov_node_t *srv_node)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }
    cli->link.srv_node = *srv_node;
    cli->link.state = RMT_PROV_LINK_STATUS_IDLE;
    cli->link.rx_pdu = 0;
    cli->link.tx_pdu = 1;
    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));
}

static int bt_mesh_cli_rmt_prov_link_open(struct bt_mesh_model_rmt_prov_cli *cli,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, const uint8_t uuid[BLE_MESH_UUID_LEN])
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    int err;
    struct bt_mesh_rmt_prov_link_t link_rsp;

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_LINK_OPEN_PROV_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_OPEN);
    net_buf_simple_add_mem(msg, uuid, BLE_MESH_UUID_LEN);
    net_buf_simple_add_u8(msg, cli->link.time);
    err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA, &link_rsp);

    os_mbuf_free_chain(msg);
    return err;
}

static int bt_mesh_cli_rmt_prov_link_open_nouuid(struct bt_mesh_model_rmt_prov_cli *cli,
    const struct bt_mesh_model_rmt_prov_node_t *srv_node, enum bt_mesh_rmt_prov_node_nppi type)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    int err;
    struct bt_mesh_rmt_prov_link_t link_rsp;

    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_OPCODE_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_OPEN);
    net_buf_simple_add_u8(msg, type);
    err = bt_mesh_rmt_prov_send_wait(cli, srv_node, msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA, &link_rsp);

    os_mbuf_free_chain(msg);
    return err;
}

static void link_reset(struct bt_mesh_model_rmt_prov_cli *cli)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }
    k_work_cancel_delayable(&cli->link.timeout);
    cli->link.srv_node.addr = BT_MESH_ADDR_UNASSIGNED;
    cli->link.state = RMT_PROV_LINK_STATUS_IDLE;
}

static void link_closed(struct bt_mesh_model_rmt_prov_cli *cli, enum bt_mesh_rmt_prov_mes_reqs_status status)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }
    struct bt_mesh_model_rmt_prov_node_t srv_node = cli->link.srv_node;
    struct bt_mesh_rmt_prov_link_t link = {
        .sts = status,
        .state = RMT_PROV_LINK_STATUS_IDLE,
    };

    BT_ERR("Link closed: 0x%04x: status: %u state: %u rx: %u tx: %u",
        srv_node.addr,
        link.sts,
        cli->link.state,
        cli->link.rx_pdu,
        cli->link.tx_pdu);

    rmt_prov_link_close(PROV_BEARER_LINK_STATUS_FAIL);

    link_reset(cli);

    link_report(cli, &srv_node, &link);
}

static int send(struct bt_mesh_model_rmt_prov_cli *cli, struct os_mbuf *msg, void *cb_data)
{
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    struct bt_mesh_msg_ctx ctx = RMT_PROV_CTX(&cli->link.srv_node, TRUE);
    int err;

    if (cli->link.srv_node.addr == BT_MESH_ADDR_UNASSIGNED) {
        BT_ERR("No server");
        return -ESHUTDOWN;
    }

    if (net_buf_simple_headroom(msg) < BLE_MESH_RMT_PROV_MIN_PDU_LEN) {
        BT_ERR("Invalid buffer");
        return -EINVAL;
    }

    BT_DBG("0x%02x", msg->om_data[0]);

    net_buf_simple_push_u8(msg, cli->link.tx_pdu);
    net_buf_simple_push_be16(msg, BLE_MESH_MODEL_OP_RMT_PROV_PDU_SEND);

    err = bt_mesh_model_send(cli->model, &ctx, msg, &bt_mesh_rmt_prov_pdu_send_cb, cli);
    if (err) {
        link_closed(cli, RMT_PROV_LINK_AS_CANNOT_SEND_PDU);
    }

    return err;
}

static int rmt_prov_cli_link_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], int32_t timeout, const struct prov_bearer_cb *cb, void *cb_data)
{
    struct bt_mesh_rmt_prov_node_t *ctx = cb_data;
    struct bt_mesh_model_rmt_prov_cli *cli = ctx->cli;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return -EINVAL;
    }
    const struct bt_mesh_model_rmt_prov_node_t *srv_node = ctx->srv_node;
    int err;
    if (cli->link.srv_node.addr != BT_MESH_ADDR_UNASSIGNED) {
        return -EBUSY;
    }
    rmt_prov_cli_bearer.cli = ctx->cli;
    rmt_prov_cli_bearer.cb = cb;

    cli->link.time = timeout ? timeout : LINK_TIMEOUT_SECONDS_DEFAULT;

    bt_mesh_rmt_prov_link_init(cli, srv_node);

    if (uuid) {
        err = bt_mesh_cli_rmt_prov_link_open(cli, srv_node, uuid);
    } else {
        err = bt_mesh_cli_rmt_prov_link_open_nouuid(cli, srv_node, ctx->nppi);
    }

    rmt_prov_cli_bearer.link = RMT_PROV_BEARER_LINK_OPENING;

    if (err) {
        link_reset(cli);
        return err;
    }

    return 0;
}

static void rmt_prov_link_close(enum prov_bearer_link_status status)
{
    struct bt_mesh_model_rmt_prov_cli *cli = rmt_prov_cli_bearer.cli;
    if (cli == NULL) {
        BT_ERR("Remote prov client model not exist!");
        return;
    }
    struct bt_mesh_model_rmt_prov_node_t *srv_node = &cli->link.srv_node;
    struct bt_mesh_msg_ctx ctx = RMT_PROV_CTX(srv_node, false);
    if (cli->link.srv_node.addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(BLE_MESH_RMT_PROV_OPCODE_LEN);
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_RMT_PROV_LINK_CLOSE);
    net_buf_simple_add_u8(msg, status);

    int err = bt_mesh_model_send(cli->model, &ctx, msg, NULL, NULL);
    if (err) {
        link_reset(cli);
        BT_ERR("Link close failed (%d)", err);
    }
    os_mbuf_free_chain(msg);
    k_work_reschedule(&cli->link.timeout, K_SECONDS(cli->link.time));
}

static int rmt_prov_send(struct os_mbuf *msg, prov_bearer_send_complete_t cb, void *cb_data)
{
    rmt_prov_cli_bearer.tx.cb = cb;
    return send(rmt_prov_cli_bearer.cli, msg, cb_data);
}

static void rmt_clear_tx(void)
{}

void bt_mesh_rmt_prov_register(bt_mesh_rmt_prov_t *rmt_prov)
{
    g_rmt_prov.rmt_unprovisioned_beacon_recv = rmt_prov->rmt_unprovisioned_beacon_recv;
}

int bt_mesh_rmt_prov_adv_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t rmt_addr, uint16_t prov_addr, uint8_t nppi)
{
    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = rmt_addr,
        .net_idx = net_idx,
        .ttl = BT_MESH_TTL_DEFAULT,

    };
    struct bt_mesh_rmt_prov_node_t ctx = {bt_mesh_rmt_prov_cli, &srv_node, nppi};

    return bt_mesh_pb_rmt_open(uuid, prov_addr, 0, &bt_mesh_rmt_prov_bearer_cli, &ctx);
}

int bt_mesh_rmt_prov_start_scan(
    uint16_t net_idx, uint16_t rmt_addr, const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t items_limit, uint8_t timeout)
{
    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = rmt_addr,
        .net_idx = net_idx,
        .ttl = BT_MESH_TTL_DEFAULT,

    };
    const struct bt_mesh_rmt_prov_node_t ctx = {bt_mesh_rmt_prov_cli, &srv_node, 0};
    struct bt_mesh_rmt_prov_scan_sts_t status;

    return bt_mesh_cli_rmt_prov_scan_start(&ctx, timeout, uuid, items_limit, &status);
}

int bt_mesh_rmt_prov_start_ex_scan(
    uint16_t net_idx, uint16_t rmt_addr, const uint8_t *uuid, uint8_t timeout, ble_mesh_rmt_prov_ex_scan_t *ad_filter)
{
    struct bt_mesh_model_rmt_prov_node_t srv_node = {
        .addr = rmt_addr,
        .net_idx = net_idx,
        .ttl = BT_MESH_TTL_DEFAULT,

    };
    struct bt_mesh_rmt_prov_node_t ctx = {bt_mesh_rmt_prov_cli, &srv_node, 0};

    return bt_mesh_cli_rmt_prov_ex_scan_start(&ctx, timeout, uuid, ad_filter);
}