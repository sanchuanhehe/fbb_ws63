/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Remote Prov Server Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "mesh_priv.h"
#include "crypto.h"
#include "net.h"
#include "ble_mesh_nv_op.h"
#include "access.h"
#include "remote_prov_server.h"

#define BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM (sizeof(g_rmt_prov_sta.ex_scan.devs) / sizeof(g_rmt_prov_sta.ex_scan.devs[0]))

static bt_mesh_rmt_prov_srv_sta_t g_rmt_prov_sta;
static bool bt_mesh_rmt_prov_ctx_check_invalid(struct bt_mesh_msg_ctx *cli_ctx);
static void bt_mesh_rmt_prov_link_closed(const struct prov_bearer *bearer, void *cb_data, uint8_t reason);
static uint8_t bt_mesh_rmt_prov_ad_type_get(uint8_t *ad_filter, uint8_t ad_count, uint8_t ad);
static uint8_t bt_mesh_rmt_prov_ad_type_del(uint8_t *ad_filter, uint8_t ad_count, uint8_t ad);
static int bt_mesh_rmt_prov_extend_scan_unprov_rsp(bt_mesh_rmt_prov_ex_scan_dev_t *dev);
static bt_mesh_rmt_prov_ex_scan_dev_t *bt_mesh_rmt_prov_ex_scan_get_by_uuid(const uint8_t uuid[BLE_MESH_UUID_LEN]);
static bt_mesh_rmt_prov_adv_recv_callback g_rmt_pb_adv_recv = NULL;

uint8_t bt_mesh_rmt_prov_ex_scan_state_get(void)
{
    return g_rmt_prov_sta.ex_scan.state;
}

uint8_t bt_mesh_node_refresh_get(void)
{
    return g_rmt_prov_sta.link.nppi.nppi_procedure;
}

uint8_t bt_mesh_link_close_reason_get(void)
{
    return g_rmt_prov_sta.link.close_reason;
}


int bt_mesh_comp_get_page_128(struct os_mbuf *buf)
{
    int err;
    uint16_t len = 0;
    uint8_t data[68] = {0}; // 68:page 128 data len max

    err = ble_mesh_data_read(BTH_BLE_MESH_COMP_DATA_128, sizeof(data), &len, data);
    if (err) {
        BT_ERR("Failed reading composition data: %d", err);
        return -OP_FAIL;
    }
    net_buf_simple_add_mem(buf, data, len);

    return SUCCESS;
}

// 获取扫描当前状态
void bt_mesh_rmt_prov_scan_state_get(void)
{
    bt_mesh_rmt_prov_dev_list_t *dev = NULL;
    struct osal_list_head *list_entry = NULL;

    BT_INFO("bt mehs remote prov scan state:[ status:%u rp_scanning_state: %u scanned_items_limit:%u timeout:%u "
            "scan_uuid:%s ]",
        g_rmt_prov_sta.scan.status,
        g_rmt_prov_sta.scan.rp_scanning_state,
        g_rmt_prov_sta.scan.scanned_items_limit,
        g_rmt_prov_sta.scan.timeout,
        bt_hex(g_rmt_prov_sta.scan.uuid, BLE_MESH_UUID_LEN));

    BT_INFO("scaned_dev_count:%u scaned_dev:", g_rmt_prov_sta.scan.scan_count);
    osal_list_for_each(list_entry, &g_rmt_prov_sta.dev_list) {
        dev = osal_list_entry(list_entry, bt_mesh_rmt_prov_dev_list_t, entry);
        BT_INFO("%s", bt_hex(dev->uuid, BLE_MESH_UUID_LEN));
    }

    return;
}

// 获取链接当前状态
void bt_mesh_rmt_prov_link_state_get(void)
{
    BT_INFO("bt mehs remote prov link state:[ status:%u state: %u\r\n nppi:%u %s timeout:%u rssi:%d "
            "uuid:%s ]",
        g_rmt_prov_sta.link.status,
        g_rmt_prov_sta.link.state,
        g_rmt_prov_sta.link.nppi.nppi_procedure,
        g_rmt_prov_sta.link.nppi.refreshing == true ? "runing" : "stop",
        g_rmt_prov_sta.link.timeout,
        g_rmt_prov_sta.link.rssi,
        bt_hex(g_rmt_prov_sta.link.uuid, BLE_MESH_UUID_LEN));

    return;
}

static int bt_mesh_rmt_prov_dev_list_add(
    int8_t rssi, uint8_t *uuid, bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash)
{
    struct osal_list_head *list_entry = NULL;
    bt_mesh_rmt_prov_dev_list_t *dev = NULL;
    bt_mesh_rmt_prov_dev_list_t *dev_new = NULL;

    // check
    if (g_rmt_prov_sta.scan.rp_scanning_state == RMT_PROV_SRV_SCAN_SINGLE_NODE &&
        memcmp(g_rmt_prov_sta.scan.uuid, uuid, BLE_MESH_UUID_LEN) != 0) {
        return -INVALID_PARAM;
    }

    // 遍历
    osal_list_for_each(list_entry, &g_rmt_prov_sta.dev_list) {
        dev = osal_list_entry(list_entry, bt_mesh_rmt_prov_dev_list_t, entry);
        if (memcmp(dev->uuid, uuid, BLE_MESH_UUID_LEN) == 0) {
            return -REPEAT_PARAM;
        }
    }

    dev_new = (bt_mesh_rmt_prov_dev_list_t *)osal_kmalloc(sizeof(bt_mesh_rmt_prov_dev_list_t), OSAL_GFP_ATOMIC);
    if (dev_new == NULL) {
        BT_ERR("dev list mem alloc failed");
        return -OP_FAIL;
    }

    memcpy_s(dev_new->uuid, BLE_MESH_UUID_LEN, uuid, BLE_MESH_UUID_LEN);
    dev_new->oob_info = oob_info;
    dev_new->rssi = rssi;
    if (uri_hash) {
        dev_new->uri_hash = *uri_hash;
    }

    osal_list_add_tail(&(dev_new->entry), &g_rmt_prov_sta.dev_list);

    return 0;
}

static int bt_mesh_rmt_prov_dev_list_del(uint8_t *uuid)
{
    struct osal_list_head *list_entry = NULL;
    bt_mesh_rmt_prov_dev_list_t *dev = NULL;

    // 遍历
    osal_list_for_each(list_entry, &g_rmt_prov_sta.dev_list) {
        dev = osal_list_entry(list_entry, bt_mesh_rmt_prov_dev_list_t, entry);
        if (memcmp(dev->uuid, uuid, BLE_MESH_UUID_LEN) == 0) {
            osal_list_del(&(dev->entry));
            osal_kfree(dev);
        }
    }

    return 0;
}

static int bt_mesh_rmt_prov_dev_list_reset(void)
{
    struct osal_list_head *list_entry = NULL;
    struct osal_list_head *list_entry_tmp = NULL;
    bt_mesh_rmt_prov_dev_list_t *dev = NULL;

    // 遍历删除所有节点
    osal_list_for_each_safe(list_entry, list_entry_tmp, &g_rmt_prov_sta.dev_list) {
        dev = (bt_mesh_rmt_prov_dev_list_t *)osal_list_entry(list_entry, bt_mesh_rmt_prov_dev_list_t, entry);
        osal_list_del(&(dev->entry));
        osal_kfree(dev);
    }
    g_rmt_prov_sta.scan.scan_count = 0;

    return 0;
}

static int bt_mesh_rmt_prov_ctx_set(struct bt_mesh_msg_ctx *des, struct bt_mesh_msg_ctx *src, bool send_rel)
{
    if (des == NULL || src == NULL) {
        BT_ERR("remote prov ctx is NULL");
        return -1;
    }
    memcpy_s(des, sizeof(struct bt_mesh_msg_ctx), src, sizeof(struct bt_mesh_msg_ctx));
    des->send_rel = send_rel;
    return 0;
}

static int bt_mesh_rmt_unprov_scan_rsp(int8_t rssi, uint8_t *uuid, bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash)
{
    struct os_mbuf *rsp = NULL;
    struct bt_mesh_msg_ctx ctx = {0};

    // 上报
    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_RSP);
    net_buf_simple_add_u8(rsp, rssi);
    net_buf_simple_add_mem(rsp, uuid, BLE_MESH_UUID_LEN);
    net_buf_simple_add_le16(rsp, oob_info);
    if (uri_hash) {
        net_buf_simple_add_le32(rsp, *uri_hash);
    }

    bt_mesh_rmt_prov_ctx_set(&ctx, &g_rmt_prov_sta.ctx, false);
    bt_mesh_msg_send(&ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

void bt_mesh_rmt_prov_unprovisioned_beacon(
    const bt_addr_le_t *addr, int8_t rssi, uint8_t *uuid, bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash)
{
    int ret = 0;
    bt_mesh_rmt_prov_ex_scan_dev_t *dev = NULL;

    if (g_rmt_prov_sta.ex_scan.state && addr) {
        dev = bt_mesh_rmt_prov_ex_scan_get_by_uuid(uuid);
        if (dev) {
            dev->addr = *addr;
            dev->oob_en = true;
            dev->oob_info = oob_info;
            if (uri_hash) {
                dev->uri_en = true;
                dev->uri_hash = *uri_hash;
            }
        }
    }

    if (g_rmt_prov_sta.scan.rp_scanning_state == RMT_PROV_SRV_SCAN_IDLE) {
        return;
    }
    BT_DBG("remote prov recv uuid %s", bt_hex(uuid, BLE_MESH_UUID_LEN));

    // 记录
    ret = bt_mesh_rmt_prov_dev_list_add(rssi, uuid, oob_info, uri_hash);
    if (ret != 0) {
        BT_DBG("remote prov scan list add uuid:%s failed:%d", bt_hex(uuid, BLE_MESH_UUID_LEN), ret);
        return;
    }

    ret = bt_mesh_rmt_unprov_scan_rsp(rssi, uuid, oob_info, uri_hash);
    if (ret != 0) {
        BT_DBG("remote prov scan rsp failed");
        bt_mesh_rmt_prov_dev_list_del(uuid);
        return;
    }

    if (++g_rmt_prov_sta.scan.scan_count == g_rmt_prov_sta.scan.scanned_items_limit ||
        g_rmt_prov_sta.scan.rp_scanning_state == RMT_PROV_SRV_SCAN_SINGLE_NODE) {
        g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_IDLE;
        g_rmt_prov_sta.scan.timeout = 0;
        bt_mesh_rmt_prov_dev_list_reset();
        BT_WARN("remote prov scan scan end");
    }

    return;
}

static bt_mesh_rmt_prov_ex_scan_dev_t *bt_mesh_rmt_prov_ex_scan_dev_get(
    const bt_addr_le_t *addr, struct os_mbuf *buf, bool *uri_match)
{
    int i = 0;
    uint8_t hash[16] = {0};  // 16:hash 长度
    uint32_t uri_hash = 0;
    bt_mesh_rmt_prov_ex_scan_ad_t ad = {0};

    while (buf->om_len >= BT_MESH_RMT_DEV_AD_LEN) {  // 3:ltv格式最小值
        ad.data_len = net_buf_simple_pull_u8(buf);
        if (!ad.data_len || ad.data_len > buf->om_len) {
            BT_ERR("rmt prov extend scan data len invalid");
            return NULL;
        }
        ad.type = net_buf_simple_pull_u8(buf);
        ad.data = net_buf_simple_pull_mem(buf, ad.data_len - sizeof(ad.type));
        const uint8_t zero[16] = {0}; // 16:hash计算使用
        if (ad.type == BT_DATA_URI && !bt_mesh_aes_cmac_one(zero, ad.data, ad.data_len - sizeof(ad.type), hash)) {
            BT_DBG("rmt prov mesh s1 complete:%02x%02x%02x%02x", bt_hex(hash, 4));  // 4:hash len
            break;
        }
    }

    uri_hash = ntohl(g_rmt_prov_sta.ex_scan.devs[i].uri_hash);
    for (i = 0; i < BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM; ++i) {
        if (g_rmt_prov_sta.ex_scan.devs[i].en) {
            BT_DBG("rmt prov bt mesh uri hash:%08x", g_rmt_prov_sta.ex_scan.devs[i].uri_hash);
            if (memcmp(hash, &uri_hash, 4) == 0) {  // 4:uri_hash len
                *uri_match = true;
                return &g_rmt_prov_sta.ex_scan.devs[i];
            }
            if (memcmp(&g_rmt_prov_sta.ex_scan.devs[i].addr, addr, sizeof(bt_addr_le_t)) == 0) {
                return &g_rmt_prov_sta.ex_scan.devs[i];
            }
        }
    }

    return NULL;
}

static uint8_t bt_mesh_rmt_prov_ex_scan_state_foreach(void)
{
    uint8_t i, j = 0;

    for (i = 0; i < BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM; ++i) {
        if (g_rmt_prov_sta.ex_scan.devs[i].en) {
            j++;
        }
    }

    return j ? RMT_PROV_SRV_SCAN_MULTI_NODES : RMT_PROV_SRV_SCAN_IDLE;
}

static int bt_mesh_rmt_prov_ex_scan_adv_resolve(
    bt_mesh_rmt_prov_ex_scan_dev_t *dev, struct os_mbuf *buf, bool uri_match)
{
    uint8_t ad_type = 0;
    bt_mesh_rmt_prov_ex_scan_ad_t ad = {0};

    while (buf->om_len >= BT_MESH_RMT_DEV_AD_LEN) {  // 3:ltv格式最小值
        ad.data_len = net_buf_simple_pull_u8(buf);
        if (!ad.data_len || ad.data_len > buf->om_len) {
            BT_ERR("rmt prov extend scan data len invalid");
            return -OP_FAIL;
        }
        ad_type = net_buf_simple_pull_u8(buf);
        ad.type = bt_mesh_rmt_prov_ad_type_get(dev->ad_type_filter, dev->ad_count, ad_type);
        ad.data = net_buf_simple_pull_mem(buf, ad.data_len - sizeof(ad.type));
        if (dev->ad_data && ad.type) {
            net_buf_simple_add_u8(dev->ad_data, ad.data_len);
            net_buf_simple_add_u8(dev->ad_data, ad.type);
            net_buf_simple_add_mem(dev->ad_data, ad.data, ad.data_len - sizeof(ad.type));
            bt_mesh_rmt_prov_ad_type_del(dev->ad_type_filter, dev->ad_count, ad.type);
            dev->ad_has_count++;
        }
    }

    return SUCCESS;
}

static bool bt_mesh_rmt_prov_ex_scan_complete_check(
    bt_mesh_rmt_prov_ex_scan_dev_t *dev, uint8_t event_type, bool start_uri, bool uri_match)
{
    if (dev->ad_has_count == dev->ad_count) {
        BT_DBG("rmt prov ex scan ltv data:%s",
            dev->ad_data ? bt_hex(dev->ad_data->om_data, dev->ad_data->om_len) : "null");
        return true;
    }

    if (!start_uri && (event_type & BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP)) {
        return true;
    }

    if (dev->ad_count == 1 && start_uri && uri_match) {
        return true;
    }

    if (dev->ad_count == 1 && start_uri && dev->uri_en) {
        return true;
    }

    if (dev->ad_count > 1 && uri_match && (event_type & BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP)) {
        return true;
    }

    if (dev->ad_count > 1 && dev->uri_en && event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP) {
        return true;
    }

    return false;
}

// 普通广播远程配网处理函数，数据来源：ble_adv_gap_mesh_cb
void bt_mesh_rmt_prov_extend_unprovisioned_adv(
    uint8_t event_type, const bt_addr_le_t *addr, const uint8_t *data, uint8_t len)
{
    int ret = 0;
    bool start_uri = false;  // 扩展扫描启动报文中uri存在标记
    bool uri_match = false;
    bt_mesh_rmt_prov_ex_scan_dev_t *dev = NULL;
    struct net_buf_simple_state initial;

    if (!g_rmt_prov_sta.ex_scan.state) {
        return;
    }
    struct os_mbuf *buf = NET_BUF_SIMPLE(len);
    if (!buf) {
        BT_ERR("net buf simple failed");
        return;
    }
    net_buf_simple_add_mem(buf, data, len);
    net_buf_simple_save(buf, &initial);
    dev = bt_mesh_rmt_prov_ex_scan_dev_get(addr, buf, &uri_match);
    if (!dev || !uri_match) {
        goto err;
    }

    start_uri = bt_mesh_rmt_prov_ad_type_get(dev->ad_type_filter, dev->ad_count, BT_DATA_URI) ? true : false;
    net_buf_simple_restore(buf, &initial);
    // 解析扩展广播
    ret = bt_mesh_rmt_prov_ex_scan_adv_resolve(dev, buf, uri_match);
    if (ret) {
        goto err;
    }

    if (!bt_mesh_rmt_prov_ex_scan_complete_check(dev, event_type, start_uri, uri_match)) {
        goto err;
    }

    bt_mesh_rmt_prov_extend_scan_unprov_rsp(dev);
    g_rmt_prov_sta.ex_scan.state = bt_mesh_rmt_prov_ex_scan_state_foreach();
    if (!g_rmt_prov_sta.ex_scan.state) {
        struct bt_mesh_rmt_prov_srv_t *cfg = g_rmt_prov_sta.model->user_data;
        k_work_cancel_delayable(&cfg->ex_scan_time_transition.timer);
    }
err:
    os_mbuf_free_chain(buf);
    return;
}

static int bt_mesh_rmt_prov_op_scan_capabilities_get(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *rsp = NULL;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_STA);
    net_buf_simple_add_u8(rsp, MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM);
    net_buf_simple_add_u8(rsp, BT_MESH_RMT_PROV_ACTIVE_SCAN_EN);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_op_scan_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *rsp = NULL;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA);
    net_buf_simple_add_u8(rsp, RMT_PROV_SRV_RSP_SUC);

    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.rp_scanning_state);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.scanned_items_limit);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.timeout);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static bool bt_mesh_rmt_prov_ctx_check_invalid(struct bt_mesh_msg_ctx *cli_ctx)
{
    if (g_rmt_prov_sta.ctx.addr != cli_ctx->addr || g_rmt_prov_sta.ctx.net_idx != cli_ctx->net_idx) {
        BT_ERR("ctx check diff rcv %d %d check %d %d",
            cli_ctx->addr,
            cli_ctx->net_idx,
            g_rmt_prov_sta.ctx.addr,
            g_rmt_prov_sta.ctx.net_idx);
        return true;
    }

    return false;
}

static int bt_mesh_rmt_prov_scan_rsp(struct bt_mesh_msg_ctx *ctx, struct bt_mesh_model *model, uint8_t status)
{
    struct os_mbuf *rsp = NULL;

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA);
    net_buf_simple_add_u8(rsp, status);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.rp_scanning_state);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.scanned_items_limit);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.timeout);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_op_scan_start(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    bt_mesh_rmt_prov_scan_start_t scan_start = {0};
    uint8_t ret = RMT_PROV_SRV_RSP_SUC;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));
    scan_start.scan_items_limit = net_buf_simple_pull_u8(buf);
    if (scan_start.scan_items_limit > MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM) {
        BT_ERR("remote prov scan limit param too big");
        ret = RMT_PROV_SRV_SCAN_CANNOT_START;
        goto done;
    }

    scan_start.timeout = net_buf_simple_pull_u8(buf);
    if (g_rmt_prov_sta.scan.rp_scanning_state != RMT_PROV_SRV_SCAN_IDLE && bt_mesh_rmt_prov_ctx_check_invalid(ctx)) {
        BT_ERR("remote prov recv unknow client message");
        ret = RMT_PROV_SRV_INVALID_STATE;
        goto done;
    }

    // start scan
    if (buf->om_len == BLE_MESH_UUID_LEN) {
        scan_start.uuid_len = BLE_MESH_UUID_LEN;
        memcpy_s(scan_start.uuid,
            BLE_MESH_UUID_LEN,
            net_buf_simple_pull_mem(buf, BLE_MESH_UUID_LEN),
            BLE_MESH_UUID_LEN);
    } else {
        scan_start.uuid_len = 0;
    }

    bt_mesh_rmt_prov_scan_start(ctx, &scan_start);
done:
    bt_mesh_rmt_prov_scan_rsp(ctx, model, ret);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_op_scan_stop(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct os_mbuf *rsp = NULL;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    // 参数更新
    bt_mesh_stop_rmt_scan();

    // 上报
    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STA);
    net_buf_simple_add_u8(rsp, RMT_PROV_SRV_RSP_SUC);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.rp_scanning_state);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.scanned_items_limit);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.scan.timeout);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_ad_type_check(uint8_t *ad_type_filter, uint8_t ad_count)
{
    uint8_t i = 0;

    for (i = 0; i < ad_count; ++i) {
        if (ad_type_filter[i] == BT_DATA_NAME_SHORTENED || ad_type_filter[i] == BT_DATA_UUID16_SOME ||
            ad_type_filter[i] == BT_DATA_UUID32_SOME || ad_type_filter[i] == BT_DATA_UUID128_SOME) {
            BT_ERR("rmt prov extend scan ad type invalid");
            return -EINVAL;
        }

        for (int j = 0; j < i; j++) {
            if (ad_type_filter[i] == ad_type_filter[j]) {
                BT_ERR("rmt prov extend scan ad type same");
                return -EINVAL;
            }
        }
    }

    return 0;
}

static uint8_t bt_mesh_rmt_prov_ad_type_get(uint8_t *ad_filter, uint8_t ad_count, uint8_t ad)
{
    int i;

    for (i = 0; i < ad_count; ++i) {
        if (ad == ad_filter[i]) {
            return ad_filter[i];
        }
        if (ad == BT_DATA_NAME_SHORTENED && ad_filter[i] == BT_DATA_NAME_COMPLETE) {
            return ad;
        }
    }

    return SUCCESS;
}

static uint8_t bt_mesh_rmt_prov_ad_type_del(uint8_t *ad_filter, uint8_t ad_count, uint8_t ad)
{
    int i;

    for (i = 0; i < ad_count; ++i) {
        if (ad == ad_filter[i]) {
            ad_filter[i] = 0;
        }
        if (ad == BT_DATA_NAME_SHORTENED && ad_filter[i] == BT_DATA_NAME_COMPLETE) {
            ad_filter[i] = 0;
        }
    }

    return SUCCESS;
}

static int bt_mesh_rmt_prov_extend_scan_srv_rsp(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint8_t uri_hash, uint8_t status)
{
    struct os_mbuf *rsp = NULL;
    const struct bt_mesh_prov *prov = NULL;
    struct bt_mesh_msg_ctx ex_ctx = {0};

    // 获取server信息
    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_EXTEND_SCAN_RSP);
    net_buf_simple_add_u8(rsp, status);
    if (status == RMT_PROV_SRV_RSP_SUC) {
        prov = bt_mesh_prov_get();
        if (prov == NULL) {
            BT_ERR("remote prov ptr get failed!");
            os_mbuf_free_chain(rsp);
            return -OP_FAIL;
        }
        net_buf_simple_add_mem(rsp, prov->uuid, BLE_MESH_UUID_LEN);
        net_buf_simple_add_le16(rsp, prov->oob_info);
        if (prov->uri && uri_hash) {
            uint8_t uri_len = strlen(prov->uri);
            net_buf_simple_add_u8(rsp, uri_len + 1);
            net_buf_simple_add_u8(rsp, BT_DATA_URI);
            net_buf_simple_add_mem(rsp, prov->uri, uri_len);
            BT_WARN("rmt prov ex scan rsp add uri:%s", prov->uri);
        }
    }

    bt_mesh_rmt_prov_ctx_set(&ex_ctx, ctx, false);
    bt_mesh_msg_send(&ex_ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_ex_scan_dev_reset(bt_mesh_rmt_prov_ex_scan_dev_t *dev)
{
    if (dev->ad_data) {
        os_mbuf_free_chain(dev->ad_data);
    }
    memset_s(dev, sizeof(bt_mesh_rmt_prov_ex_scan_dev_t), 0, sizeof(bt_mesh_rmt_prov_ex_scan_dev_t));

    return SUCCESS;
}

static int bt_mesh_rmt_prov_extend_scan_unprov_rsp(bt_mesh_rmt_prov_ex_scan_dev_t *dev)
{
    struct os_mbuf *rsp = NULL;
    struct bt_mesh_msg_ctx ex_ctx = {0};

    if (!dev->en) {
        return -INVALID_PARAM;
    }
    dev->en = false;
    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_EXTEND_SCAN_RSP);
    net_buf_simple_add_u8(rsp, RMT_PROV_SRV_RSP_SUC);
    net_buf_simple_add_mem(rsp, dev->uuid, BLE_MESH_UUID_LEN);
    if (dev->oob_en) {
        net_buf_simple_add_le16(rsp, dev->oob_info);
    }
    if (dev->ad_data && dev->ad_data->om_len >= BT_MESH_RMT_DEV_AD_LEN) {  // 3:ad data 最小长度
        net_buf_simple_add_mem(rsp, dev->ad_data->om_data, dev->ad_data->om_len);
    }

    bt_mesh_rmt_prov_ctx_set(&ex_ctx, &g_rmt_prov_sta.ctx, false);
    bt_mesh_msg_send(&ex_ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, NULL, NULL);

    bt_mesh_rmt_prov_ex_scan_dev_reset(dev);
    os_mbuf_free_chain(rsp);
    return SUCCESS;
}

static bt_mesh_rmt_prov_ex_scan_dev_t *bt_mesh_rmt_prov_ex_scan_get_by_uuid(const uint8_t uuid[BLE_MESH_UUID_LEN])
{
    int i;

    for (i = 0; i < BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM; ++i) {
        if (uuid) {
            if (g_rmt_prov_sta.ex_scan.devs[i].en &&
                memcmp(g_rmt_prov_sta.ex_scan.devs[i].uuid, uuid, BLE_MESH_UUID_LEN) == 0) {
                return &g_rmt_prov_sta.ex_scan.devs[i];
            }
        } else {
            if (!g_rmt_prov_sta.ex_scan.devs[i].en) {
                return &g_rmt_prov_sta.ex_scan.devs[i];
            }
        }
    }

    return NULL;
}

static uint8_t bt_mesh_rmt_prov_ex_scan_param_stash(
    struct bt_mesh_msg_ctx *ctx, uint8_t *uuid, uint8_t *ad_type_filter, uint8_t ad_count, uint8_t timeout)
{
    bt_mesh_rmt_prov_ex_scan_dev_t *dev;

    dev = bt_mesh_rmt_prov_ex_scan_get_by_uuid(uuid);
    if (!dev) {
        if (g_rmt_prov_sta.ex_scan.state) {
            return RMT_PROV_SRV_LIMITED_RESOURCES;
        } else {
            dev = bt_mesh_rmt_prov_ex_scan_get_by_uuid(NULL);
            if (!dev) {
                BT_WARN("Extended scan fail: No memory");
                return RMT_PROV_SRV_LIMITED_RESOURCES;
            }
        }
    }

    bt_mesh_rmt_prov_ex_scan_dev_reset(dev);
    dev->ad_data = NET_BUF_SIMPLE(BT_MESH_RMT_DEV_AD_LEN);
    if (!dev->ad_data) {
        return RMT_PROV_SRV_LIMITED_RESOURCES;
    }
    if (memcpy_s(dev->ad_type_filter, ad_count, ad_type_filter, ad_count) != 0) {
        BT_ERR("memcpy failed");
        os_mbuf_free_chain(dev->ad_data);
        return RMT_PROV_SRV_LIMITED_RESOURCES;
    }
    memcpy_s(&g_rmt_prov_sta.ctx, sizeof(struct bt_mesh_msg_ctx), ctx, sizeof(struct bt_mesh_msg_ctx));
    memcpy_s(dev->uuid, BLE_MESH_UUID_LEN, uuid, BLE_MESH_UUID_LEN);
    dev->ad_count = ad_count;
    g_rmt_prov_sta.ex_scan.timeout = timeout;
    dev->en = true;
    g_rmt_prov_sta.ex_scan.state = RMT_PROV_SRV_SCAN_MULTI_NODES;

    return RMT_PROV_SRV_RSP_SUC;
}

static int bt_mesh_rmt_prov_op_extend_scan_start(
    struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_rmt_prov_srv_t *cfg = model->user_data;
    uint8_t status = RMT_PROV_SRV_RSP_SUC;
    uint8_t ad_count;
    uint8_t *ad_type_filter = NULL;
    uint8_t timeout;
    uint8_t *uuid = NULL;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));
    ad_count = net_buf_simple_pull_u8(buf);
    if (buf->om_len < ad_count || ad_count == 0 || ad_count > BT_MESH_RMT_PROV_AD_TYPE_LEN) {
        BT_ERR("rmt prov ex scan ad count invalid");
        return -INVALID_PARAM;
    }

    ad_type_filter = net_buf_simple_pull_mem(buf, ad_count);
    if (bt_mesh_rmt_prov_ad_type_check(ad_type_filter, ad_count) != 0) {
        return -INVALID_PARAM;
    }
    if (buf->om_len == BT_MESH_RMT_PROV_DATA_LEN_TIMEOUT) {
        // 指定设备扫描
        uuid = net_buf_simple_pull_mem(buf, BLE_MESH_UUID_LEN);
        timeout = net_buf_simple_pull_u8(buf);
        if (!timeout || timeout > BT_MESH_RMT_EX_SCAN_TIMEOUT_MAX) {
            BT_ERR("rmt prov ex scan timeout invalid");
            return -INVALID_PARAM;
        }

        if (g_rmt_prov_sta.ex_scan.state && bt_mesh_rmt_prov_ctx_check_invalid(ctx)) {
            status = RMT_PROV_SRV_LIMITED_RESOURCES;
            goto done;
        }
        status = bt_mesh_rmt_prov_ex_scan_param_stash(ctx, uuid, ad_type_filter, ad_count, timeout);
        if (status) {
            goto done;
        }

        k_work_reschedule(&cfg->ex_scan_time_transition.timer, K_SECONDS(timeout));
        return SUCCESS;
    }

done:
    bt_mesh_rmt_prov_extend_scan_srv_rsp(
        model, ctx, bt_mesh_rmt_prov_ad_type_get(ad_type_filter, ad_count, BT_DATA_URI), status);
    return SUCCESS;
}

/*
 * link 相关
 */

static void bt_mesh_rmt_prov_link_status(struct bt_mesh_msg_ctx *ctx, struct bt_mesh_model *model, uint8_t status)
{
    struct os_mbuf *rsp = NULL;

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_LINK_STA);
    net_buf_simple_add_u8(rsp, status);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.link.state);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return;
}

static void bt_mesh_rmt_prov_link_report(uint8_t reason)
{
    struct os_mbuf *rsp = NULL;

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_LINK_RSP);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.link.status);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.link.state);
    if (g_rmt_prov_sta.link.status == RMT_PROV_SRV_LINK_CLOSE_BY_DEV ||
        g_rmt_prov_sta.link.status == RMT_PROV_SRV_LINK_CLOSE_BY_SRV) {
        net_buf_simple_add_u8(rsp, reason);
    }
    bt_mesh_msg_send(&g_rmt_prov_sta.ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, NULL, NULL);
    os_mbuf_free_chain(rsp);
    return;
}

static void bt_mesh_rmt_prov_link_close(uint8_t status, uint8_t reason)
{
    g_rmt_prov_sta.link.status = status;
    if (g_rmt_prov_sta.link.nppi.refreshing) {
        bt_mesh_rmt_prov_link_closed(NULL, NULL, reason);
        if (g_rmt_prov_sta.link.rmt_prov_bearer_cb && g_rmt_prov_sta.link.rmt_prov_bearer_cb->link_closed) {
            g_rmt_prov_sta.link.rmt_prov_bearer_cb->link_closed(
                &rmt_prov_srv_bearer, g_rmt_prov_sta.link.rmt_prov_bearer_cb_data, reason);
        }
    } else {
        pb_adv.link_close(reason);
    }

    return;
}

static void bt_mesh_rmt_prov_link_opened(const struct prov_bearer *bearer, void *cb_data)
{
    BT_INFO("remote prov link opened")
    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_ACTIVE;
    g_rmt_prov_sta.pdu_sta.inbound = 0;
    g_rmt_prov_sta.pdu_sta.outbound = 0;
    bt_mesh_rmt_prov_link_report(RMT_PROV_SRV_LINK_CLOSE_RFU);

    return;
}

static void bt_mesh_rmt_prov_link_closed(const struct prov_bearer *bearer, void *cb_data, uint8_t reason)
{
    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_IDLE) {
        return;
    }
    BT_INFO("remote prov link closed reason:%d state:%d status:%d",
        reason,
        g_rmt_prov_sta.link.state,
        g_rmt_prov_sta.link.status);

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_OPEN) {
        g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_OPEN_FAILED;
    } else if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_TRANS) {
        g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND;
    }

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_OPEN) {
        g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_OPEN_FAILED;
    } else if (reason == PROV_BEARER_LINK_STATUS_TIMEOUT) {
        if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_TRANS) {
            g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND;
        } else {
            g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_CLOSE_BY_SRV;
        }
    } else if (reason == PROV_BEARER_LINK_STATUS_FAIL && g_rmt_prov_sta.link.status != RMT_PROV_SRV_LINK_CLOSE_BY_CLI &&
               g_rmt_prov_sta.link.status != RMT_PROV_SRV_LINK_CLOSE_BY_SRV) {
        g_rmt_prov_sta.link.status = RMT_PROV_SRV_LINK_CLOSE_BY_DEV;
    }

    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_IDLE;
    bt_mesh_rmt_prov_link_report(reason);
    g_rmt_prov_sta.link.nppi.refreshing = false;
    g_rmt_prov_sta.link.status = RMT_PROV_SRV_RSP_SUC;
    return;
}

static void bt_mesh_rmt_prov_link_err(const struct prov_bearer *bearer, void *cb_data, uint8_t err)
{
    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_IDLE) {
        return;
    }

    BT_ERR("remote prov link err")
    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_IDLE;
    bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_RCV, err);
}

static void bt_mesh_rmt_prov_pdu_recv(const struct prov_bearer *bearer, void *cb_data, struct os_mbuf *buf)
{
    int ret = 0;
    BT_DBG("remote prov pdu recv");

    if (g_rmt_prov_sta.link.state != RMT_PROV_SRV_LINK_ACTIVE && g_rmt_prov_sta.link.state != RMT_PROV_SRV_LINK_TRANS) {
        return;
    }
    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_ACTIVE;

    ret = bt_mesh_rmt_prov_pdu_report_send(false, ++g_rmt_prov_sta.pdu_sta.inbound, buf->om_data, buf->om_len);
    if (ret != 0) {
        bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND, RMT_PROV_SRV_LINK_CLOSE_FAIL);
    }
    return;
}

static const struct prov_bearer_cb rmt_prov_bearer_cb = {
    .link_opened = bt_mesh_rmt_prov_link_opened,
    .link_closed = bt_mesh_rmt_prov_link_closed,
    .error = bt_mesh_rmt_prov_link_err,
    .recv = bt_mesh_rmt_prov_pdu_recv,
};

static int bt_mesh_rmt_prov_refresh_link_accept(const struct prov_bearer_cb *cb, void *cb_data)
{
    g_rmt_prov_sta.link.rmt_prov_bearer_cb = (struct prov_bearer_cb *)cb;
    g_rmt_prov_sta.link.rmt_prov_bearer_cb_data = cb_data;

    return 0;
}

static void bt_mesh_rmt_prov_refresh_pdu_send_complete(int err, void *cb_data)
{
    if (err != 0) {
        BT_ERR("remote prov refresh pdu send fail: %d", err);
        bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_RCV, RMT_PROV_SRV_LINK_CLOSE_FAIL);
        return;
    }

    if (g_rmt_prov_sta.link.send_complate_cb) {
        g_rmt_prov_sta.link.send_complate_cb(err, g_rmt_prov_sta.link.send_complate_cb_data);
    }
    return;
}

static int bt_mesh_rmt_prov_refresh_pdu_send(struct os_mbuf *buf, prov_bearer_send_complete_t cb, void *cb_data)
{
    int ret;
    struct os_mbuf *rsp = NULL;
    struct bt_mesh_msg_ctx ctx = {0};

    static const struct bt_mesh_send_cb send_cb = {
        .end = bt_mesh_rmt_prov_refresh_pdu_send_complete,
    };

    if (!g_rmt_prov_sta.link.nppi.refreshing) {
        return -EINVAL;
    }

    g_rmt_prov_sta.link.send_complate_cb = cb;
    g_rmt_prov_sta.link.send_complate_cb_data = cb_data;

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -OP_FAIL;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_PDU_RSP);
    net_buf_simple_add_u8(rsp, ++g_rmt_prov_sta.pdu_sta.inbound);
    net_buf_simple_add_mem(rsp, buf->om_data, buf->om_len);

    bt_mesh_rmt_prov_ctx_set(&ctx, &g_rmt_prov_sta.ctx, true);
    ret = bt_mesh_msg_send(&ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, &send_cb, NULL);
    if (ret != 0) {
        BT_ERR("remote prov pdu send fail: %d", ret);
        bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND, RMT_PROV_SRV_LINK_CLOSE_FAIL);
    }

    os_mbuf_free_chain(rsp);
    return ret;
}

static void bt_mesh_rmt_prov_clear_tx(void)
{
    // 暂无处理
    return;
}

const struct prov_bearer rmt_prov_srv_bearer = {
    .type = BT_MESH_RMT_PROV,
    .link_accept = bt_mesh_rmt_prov_refresh_link_accept,
    .send = bt_mesh_rmt_prov_refresh_pdu_send,
    .clear_tx = bt_mesh_rmt_prov_clear_tx,
};

static int bt_mesh_rmt_prov_op_link_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));
    bt_mesh_rmt_prov_link_status(ctx, model, RMT_PROV_SRV_RSP_SUC);
    return SUCCESS;
}

static uint8_t bt_mesh_rmt_prov_refresh_link_open(struct os_mbuf *buf)
{
    uint8_t nppi = 0;

    nppi = net_buf_simple_pull_u8(buf);
    if (nppi > RMT_PROV_SRV_NODE_COM_REF) {
        BT_ERR("ivalid param nppi: %u", nppi);
        return RMT_PROV_SRV_INVALID_STATE;
    }

    if (nppi == RMT_PROV_SRV_NODE_COM_REF && !atomic_test_bit(bt_mesh.flags, BT_MESH_COMP_DIRTY)) {
        BT_ERR("Composition data page 128 is equal to page 0");
        bt_mesh_rmt_prov_link_status(&g_rmt_prov_sta.ctx, g_rmt_prov_sta.model, RMT_PROV_SRV_LINK_CANNOT_OPEN);
        return RMT_PROV_SRV_LINK_CANNOT_OPEN;
    }

    bt_mesh_prov_enable(BT_MESH_RMT_PROV);
    g_rmt_prov_sta.link.nppi.nppi_procedure = nppi;
    g_rmt_prov_sta.link.nppi.refreshing = true;
    g_rmt_prov_sta.pdu_sta.inbound = 0;
    g_rmt_prov_sta.pdu_sta.outbound = 0;
    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_ACTIVE;
    bt_mesh_rmt_prov_link_status(&g_rmt_prov_sta.ctx, g_rmt_prov_sta.model, RMT_PROV_SRV_RSP_SUC);
    if (g_rmt_prov_sta.link.rmt_prov_bearer_cb && g_rmt_prov_sta.link.rmt_prov_bearer_cb->link_opened) {
        g_rmt_prov_sta.link.rmt_prov_bearer_cb->link_opened(
            &rmt_prov_srv_bearer, g_rmt_prov_sta.link.rmt_prov_bearer_cb_data);
    }

    bt_mesh_rmt_prov_link_report(RMT_PROV_SRV_LINK_CLOSE_RFU);

    return RMT_PROV_SRV_RSP_SUC;
}

static uint8_t bt_mesh_rmt_prov_unprov_link_open(struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t timeout = 0;

    memcpy_s(g_rmt_prov_sta.link.uuid,
        BLE_MESH_UUID_LEN,
        net_buf_simple_pull_mem(buf, BLE_MESH_UUID_LEN),
        BLE_MESH_UUID_LEN);
    if (buf->om_len == 1) {
        timeout = net_buf_simple_pull_u8(buf);
        if (!timeout || timeout > BT_MESH_RMT_PROV_LINK_TIME_OUT) {
            BT_ERR("invalid timeout: %d", timeout);
            return RMT_PROV_SRV_INVALID_STATE;
        }
    }

    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_OPEN;
    g_rmt_prov_sta.link.timeout = timeout;
    g_rmt_prov_sta.pdu_sta.inbound = 0;
    g_rmt_prov_sta.pdu_sta.outbound = 0;
    bt_mesh_rmt_prov_link_status(&g_rmt_prov_sta.ctx, g_rmt_prov_sta.model, RMT_PROV_SRV_RSP_SUC);
    pb_adv.link_open(g_rmt_prov_sta.link.uuid, g_rmt_prov_sta.link.timeout, &rmt_prov_bearer_cb, &g_rmt_prov_sta);

    return RMT_PROV_SRV_RSP_SUC;
}

static int bt_mesh_rmt_prov_op_link_open(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    bool nppi_flag = (buf->om_len == 1);
    uint8_t nppi;
    uint8_t status = RMT_PROV_SRV_RSP_SUC;
    uint8_t *uuid = NULL;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    // 解析数据
    if (buf->om_len != BT_MESH_RMT_PROV_DATA_LEN_NPPI && buf->om_len != BT_MESH_RMT_PROV_DATA_LEN_UUID &&
        buf->om_len != BT_MESH_RMT_PROV_DATA_LEN_TIMEOUT) {
        BT_ERR("link open data len is invalid");
        return -INVALID_PARAM;
    }

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_CLOSE || g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_TRANS) {
        status = RMT_PROV_SRV_INVALID_STATE;
        goto done;
    }

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_OPEN || g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_ACTIVE) {
        if (bt_mesh_rmt_prov_ctx_check_invalid(ctx)) {
            status = RMT_PROV_SRV_LINK_CANNOT_OPEN;
            goto done;
        }

        if (nppi_flag) {
            nppi = net_buf_simple_pull_u8(buf);
            if (!g_rmt_prov_sta.link.nppi.refreshing || nppi != g_rmt_prov_sta.link.nppi.nppi_procedure) {
                status = RMT_PROV_SRV_LINK_CANNOT_OPEN;
            }
            goto done;
        }

        uuid = net_buf_simple_pull_mem(buf, BLE_MESH_UUID_LEN);
        if (g_rmt_prov_sta.link.nppi.refreshing || memcmp(uuid, g_rmt_prov_sta.link.uuid, BLE_MESH_UUID_LEN)) {
            status = RMT_PROV_SRV_LINK_CANNOT_OPEN;
        }
        goto done;
    }

    // link idle
    memcpy_s(&g_rmt_prov_sta.ctx, sizeof(struct bt_mesh_msg_ctx), ctx, sizeof(struct bt_mesh_msg_ctx));
    if (nppi_flag) {
        return bt_mesh_rmt_prov_refresh_link_open(buf);
    } else {
        return bt_mesh_rmt_prov_unprov_link_open(ctx, buf);
    }
done:
    bt_mesh_rmt_prov_link_status(ctx, model, status);
    return SUCCESS;
}

static int bt_mesh_rmt_prov_op_link_close(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t reason;
    uint8_t status = 0;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    // 参数解析
    reason = net_buf_simple_pull_u8(buf);
    if (reason != RMT_PROV_SRV_LINK_CLOSE_SUC && reason != RMT_PROV_SRV_LINK_CLOSE_FAIL) {
        return -INVALID_PARAM;
    }

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_IDLE || g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_CLOSE) {
        status = RMT_PROV_SRV_RSP_SUC;
        goto done;
    }

    if (bt_mesh_rmt_prov_ctx_check_invalid(ctx)) {
        BT_ERR("remote prov recv unknow client message");
        status = RMT_PROV_SRV_INVALID_STATE;
        goto done;
    }
    g_rmt_prov_sta.link.close_reason = reason;
    g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_CLOSE;
    bt_mesh_rmt_prov_link_status(ctx, model, status);
    bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CLI, reason);
    return SUCCESS;
done:
    bt_mesh_rmt_prov_link_status(ctx, model, status);

    return SUCCESS;
}

static void bt_mesh_rmt_prov_pdu_outbound_rsp(void)
{
    struct os_mbuf *rsp = NULL;

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return;
    }
    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_PDU_OUNBOUND_RSP);
    net_buf_simple_add_u8(rsp, g_rmt_prov_sta.pdu_sta.outbound);

    bt_mesh_msg_send(&g_rmt_prov_sta.ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, NULL, NULL);

    os_mbuf_free_chain(rsp);
    return;
}

static void bt_mesh_rmt_prov_pdu_send_complete(int err, void *cb_data)
{
    if (err != 0) {
        BT_ERR("remote prov pdu send failed:%d", err);
        bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND, RMT_PROV_SRV_LINK_CLOSE_FAIL);
        return;
    }

    if (g_rmt_prov_sta.link.state == RMT_PROV_SRV_LINK_TRANS) {
        g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_ACTIVE;
        g_rmt_prov_sta.pdu_sta.outbound++;
        bt_mesh_rmt_prov_pdu_outbound_rsp();
    }

    return;
}

STATIC int bt_mesh_rmt_prov_op_pdu_send(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    int ret;
    uint8_t outbound;

    BT_DBG("remote prov rcv data len %d, hex %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    if (bt_mesh_rmt_prov_ctx_check_invalid(ctx)) {
        BT_ERR("remote prov recv unknow client message");
        return -INVALID_PARAM;
    }
    if (g_rmt_pb_adv_recv != NULL) {
        // If a user defines a callback and the processing is successful, the message will no longer be processed.
        if (!g_rmt_pb_adv_recv(ctx->addr, ctx->recv_rssi, 0, buf)) {
            return 0;
        }
    }
    if (g_rmt_prov_sta.link.state != RMT_PROV_SRV_LINK_ACTIVE) {
        BT_ERR("remote prov link state not in active:%d", g_rmt_prov_sta.link.state);
        return -INVALID_PARAM;
    }

    outbound = net_buf_simple_pull_u8(buf);
    if (outbound != g_rmt_prov_sta.pdu_sta.outbound + 1) {
        BT_ERR("Invalid pdu outbound: %u, expected %u", outbound, g_rmt_prov_sta.pdu_sta.outbound + 1);
        bt_mesh_rmt_prov_pdu_outbound_rsp();
        return -INVALID_PARAM;
    }

    if (g_rmt_prov_sta.link.nppi.refreshing) {
        g_rmt_prov_sta.pdu_sta.outbound++;
        bt_mesh_rmt_prov_pdu_outbound_rsp();
        g_rmt_prov_sta.link.rmt_prov_bearer_cb->recv(
            &rmt_prov_srv_bearer, g_rmt_prov_sta.link.rmt_prov_bearer_cb_data, buf);
    } else {
        g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_TRANS;
        ret = pb_adv.send(buf, bt_mesh_rmt_prov_pdu_send_complete, &g_rmt_prov_sta);
        if (ret != 0) {
            bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_CANNOT_SEND, RMT_PROV_SRV_LINK_CLOSE_FAIL);
        }
    }

    return SUCCESS;
}

const struct bt_mesh_model_op bt_mesh_rmt_prov_srv_op[] = {
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_CAPABILITIES_GET, 0, bt_mesh_rmt_prov_op_scan_capabilities_get},
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_GET, 0, bt_mesh_rmt_prov_op_scan_get},
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_START, 2, bt_mesh_rmt_prov_op_scan_start},
    {BLE_MESH_MODEL_OP_RMT_PROV_SCAN_STOP, 0, bt_mesh_rmt_prov_op_scan_stop},
    {BLE_MESH_MODEL_OP_RMT_PROV_EXTEND_SCAN_START, 2, bt_mesh_rmt_prov_op_extend_scan_start},
    {BLE_MESH_MODEL_OP_RMT_PROV_LINK_GET, 0, bt_mesh_rmt_prov_op_link_get},
    {BLE_MESH_MODEL_OP_RMT_PROV_LINK_OPEN, 1, bt_mesh_rmt_prov_op_link_open},
    {BLE_MESH_MODEL_OP_RMT_PROV_LINK_CLOSE, 1, bt_mesh_rmt_prov_op_link_close},
    {BLE_MESH_MODEL_OP_RMT_PROV_PDU_SEND, 2, bt_mesh_rmt_prov_op_pdu_send},
    BT_MESH_MODEL_OP_END,
};

static void bt_mesh_remote_scan_timer_handle(struct ble_npl_event *work)
{
    // 停止扫描
    g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_IDLE;
    g_rmt_prov_sta.scan.timeout = 0;
    bt_mesh_rmt_prov_dev_list_reset();
    BT_WARN("remote prov scan scan timeout");
}

static void bt_mesh_remote_ex_scan_timer_handle(struct ble_npl_event *work)
{
    uint32_t i;
    g_rmt_prov_sta.ex_scan.state = RMT_PROV_SRV_SCAN_IDLE;
    BT_WARN("rmt prov ex scan timer handle in");
    // 遍历上报
    for (i = 0; i < BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM; ++i) {
        bt_mesh_rmt_prov_extend_scan_unprov_rsp(&g_rmt_prov_sta.ex_scan.devs[i]);
    }

    return;
}

static void subnet_evt(struct bt_mesh_subnet *subnet, enum bt_mesh_key_evt evt)
{
    if (!g_rmt_prov_sta.model || evt != BT_MESH_KEY_DELETED) {
        return;
    }
    int i;
    BT_INFO("Remote prov netkey deleted");
    if (g_rmt_prov_sta.link.state != RMT_PROV_SRV_LINK_IDLE && subnet->net_idx == g_rmt_prov_sta.ctx.net_idx) {
        bt_mesh_rmt_prov_link_close(RMT_PROV_SRV_LINK_CLOSE_BY_SRV, RMT_PROV_SRV_LINK_CLOSE_FAIL);
        /* Skip the link closing stage, as specified in the Bluetooth
         * MshPRTv1.1: 4.4.5.4.
         */
        g_rmt_prov_sta.link.state = RMT_PROV_SRV_LINK_IDLE;
    } else if (g_rmt_prov_sta.scan.rp_scanning_state && subnet->net_idx == g_rmt_prov_sta.ctx.net_idx) {
        g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_IDLE;
        g_rmt_prov_sta.scan.timeout = 0;
        bt_mesh_rmt_prov_dev_list_reset();
        g_rmt_prov_sta.ex_scan.state = RMT_PROV_SRV_SCAN_IDLE;
        for (i = 0; i < BT_MESH_RMT_PROV_EX_SCAN_DEV_NUM; ++i) {
            if (g_rmt_prov_sta.ex_scan.devs[i].en) {
                bt_mesh_rmt_prov_ex_scan_dev_reset(&g_rmt_prov_sta.ex_scan.devs[i]);
            }
        }
    }
}

static int bt_mesh_rmt_prov_srv_init(struct bt_mesh_model *model)
{
    struct bt_mesh_rmt_prov_srv_t *cfg = model->user_data;

    if (!cfg) {
        BT_ERR("No remote prov Server");
        return -EINVAL;
    }

    if (!bt_mesh_subnet_cb_list[4]) {            // 4:dev key del回调数组序号
        bt_mesh_subnet_cb_list[4] = subnet_evt;  // 4:dev key del回调数组序号
    }

    cfg->model = model;
    g_rmt_prov_sta.model = model;

    model->keys[0] = BT_MESH_KEY_DEV;

    k_work_init_delayable(&cfg->scan_time_transition.timer, bt_mesh_remote_scan_timer_handle);
    k_work_add_arg_delayable(&cfg->scan_time_transition.timer, cfg);

    k_work_init_delayable(&cfg->ex_scan_time_transition.timer, bt_mesh_remote_ex_scan_timer_handle);
    k_work_add_arg_delayable(&cfg->ex_scan_time_transition.timer, cfg);

    g_rmt_prov_sta.scan.scanned_items_limit = MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM;

    OSAL_INIT_LIST_HEAD(&g_rmt_prov_sta.dev_list);

    return 0;
}

static int bt_mesh_rmt_prov_srv_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_rmt_prov_srv_t *cfg = model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cfg) {
        BT_ERR("No remote prov Server");
        return -EINVAL;
    }

    k_work_cancel_delayable_delete(&cfg->scan_time_transition.timer);

    k_work_cancel_delayable_delete(&cfg->ex_scan_time_transition.timer);

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_rmt_prov_srv_cb = {
    .init = bt_mesh_rmt_prov_srv_init,
    .deinit = bt_mesh_rmt_prov_srv_deinit,
};

int bt_mesh_rmt_prov_scan_start(struct bt_mesh_msg_ctx *ctx, bt_mesh_rmt_prov_scan_start_t *scan_start)
{
    if (g_rmt_prov_sta.model == NULL) {
        return -ENOTSUP;
    }

    struct bt_mesh_rmt_prov_srv_t *cfg = (struct bt_mesh_rmt_prov_srv_t *)g_rmt_prov_sta.model->user_data;

    if (scan_start->scan_items_limit == 0xff) {
        BT_ERR("remote prov scan start invalid limit param");
        return -EINVAL;
    }

    if (scan_start->uuid_len != 0 && scan_start->uuid_len != BLE_MESH_UUID_LEN) {
        BT_ERR("remote prov scan start invalid uuid len");
        return -EINVAL;
    }

    if (scan_start->timeout == 0) {
        BT_ERR("remote prov scan start invalid timeout param");
        return -EINVAL;
    }

    if (scan_start->scan_items_limit > MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM) {
        BT_ERR("remote prov scan limit param too big");
        return -EINVAL;
    }

    if (g_rmt_prov_sta.scan.rp_scanning_state != RMT_PROV_SRV_SCAN_IDLE && bt_mesh_rmt_prov_ctx_check_invalid(&ctx)) {
        BT_ERR("remote prov recv unknow client message");
        return -EINVAL;
    }

    // 更新参数
    memcpy_s(&g_rmt_prov_sta.ctx, sizeof(struct bt_mesh_msg_ctx), ctx, sizeof(struct bt_mesh_msg_ctx));
    g_rmt_prov_sta.scan.status = RMT_PROV_SRV_RSP_SUC;
    g_rmt_prov_sta.scan.scanned_items_limit =
        scan_start->scan_items_limit == 0 ? MYNEWT_VAL_BLE_MESH_RMT_PROV_MAX_SCAN_ITEM : scan_start->scan_items_limit;
    g_rmt_prov_sta.scan.timeout = scan_start->timeout;
    // reset dev_list
    bt_mesh_rmt_prov_dev_list_reset();
    // start scan
    if (scan_start->uuid_len == BLE_MESH_UUID_LEN) {
        memcpy_s(g_rmt_prov_sta.scan.uuid,
            BLE_MESH_UUID_LEN,
            scan_start->uuid,
            BLE_MESH_UUID_LEN);
        g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_SINGLE_NODE;
    } else {
        g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_MULTI_NODES;
    }
    // 启动定时器
    k_work_schedule(&cfg->scan_time_transition.timer, K_SECONDS(g_rmt_prov_sta.scan.timeout));
    return 0;
}

void bt_mesh_rmt_prov_adv_recv_cb_set(bt_mesh_rmt_prov_adv_recv_callback recv_func)
{
    g_rmt_pb_adv_recv = recv_func;
}

static void bt_mesh_rmt_prov_pdu_report_send_complete(int err, void *cb_data)
{
    BT_INFO("send_end")
}

int bt_mesh_rmt_prov_pdu_report_send(bool send_rel, uint8_t inbound, const uint8_t *data, uint16_t len)
{
    int ret = 0;
    struct bt_mesh_msg_ctx ctx = {0};
    struct os_mbuf *rsp = NULL;
    
    static const struct bt_mesh_send_cb send_cb = {
        .start = NULL,
        .end = bt_mesh_rmt_prov_pdu_report_send_complete,
    };

    if (g_rmt_prov_sta.model == NULL) {
        return -ENOTSUP;
    }

    rsp = NET_BUF_SIMPLE(MODEL_OPCODE_LEN);
    if (!rsp) {
        BT_ERR("net buf simple failed");
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(rsp, BLE_MESH_MODEL_OP_RMT_PROV_PDU_RSP);
    net_buf_simple_add_u8(rsp, inbound);
    net_buf_simple_add_mem(rsp, data, len);

    bt_mesh_rmt_prov_ctx_set(&ctx, &g_rmt_prov_sta.ctx, send_rel);
    if (send_rel) {
        ret = bt_mesh_msg_send(&ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, &send_cb, NULL);
    } else {
        ret = bt_mesh_msg_send(&ctx, rsp, bt_mesh_model_elem(g_rmt_prov_sta.model)->addr, NULL, NULL);
    }
    
    if (ret != 0) {
        BT_ERR("remote prov pdu send fail: %d", ret);
    }
    os_mbuf_free_chain(rsp);
    return ret;
}

int bt_mesh_stop_rmt_scan(void)
{
    if (g_rmt_prov_sta.model == NULL || g_rmt_prov_sta.model->user_data == NULL) {
        return -ENOTSUP;
    }
    struct bt_mesh_rmt_prov_srv_t *cfg = (struct bt_mesh_rmt_prov_srv_t *)g_rmt_prov_sta.model->user_data;
    g_rmt_prov_sta.scan.rp_scanning_state = RMT_PROV_SRV_SCAN_IDLE;
    g_rmt_prov_sta.scan.timeout = 0;
    bt_mesh_rmt_prov_dev_list_reset();
    k_work_cancel_delayable(&cfg->scan_time_transition.timer);

    return 0;
}