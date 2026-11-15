/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh message.
 *
 * History:
 * 2025-4-15, Create file.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "securec.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "mesh/msg.h"
#include "transport.h"
#include "adv.h"
#include "prov.h"
#include "beacon.h"
#include "access.h"

#include "bts_def.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_message.h"

#define BLE_MESH_MAX_TRANSMIT_COUNT 7
#define BLE_MESH_DEFAULT_TRANS MYNEWT_VAL_BLE_MESH_NETWORK_TRANSMIT_COUNT
#define BLE_MESH_DEFAULT_INTERVAL MYNEWT_VAL_BLE_MESH_NETWORK_TRANSMIT_INTERVAL

ble_mesh_msg_callbacks_t g_ble_mesh_msg_callbacks = { 0 };

errcode_t ble_mesh_msg_send(const ble_mesh_msg_header_t *ctx, const uint8_t *data, uint16_t len)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (data == NULL || len == 0) {
        BT_ERR("msg_send para invalid:%p len:%d", data, len);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    struct bt_mesh_msg_ctx bt_ctx = {
        .send_ttl = ctx->send_ttl,
        .net_idx = ctx->net_idx,
        .addr = ctx->dest_addr,
        .app_idx = ctx->app_idx,
        .send_rel = ctx->send_rel,
    };

    struct bt_mesh_net_tx tx = {
        .ctx = &bt_ctx,
        .src = ctx->src_addr,
    };
    int err = 0;
    struct os_mbuf *sdu = NET_BUF_SIMPLE(BT_MESH_NET_MIN_PDU_LEN + len);
    if (sdu == NULL) {
        BT_ERR("buf alloc failed, len:%d", len);
        return ERRCODE_BLE_MESH_MALLOC_FAILED;
    }

    net_buf_simple_init(sdu, 0);

    net_buf_simple_add_mem(sdu, data, len);

    err = bt_mesh_trans_send(&tx, sdu, NULL, NULL);
    os_mbuf_free_chain(sdu);

    return ble_mesh_errcode_convert(err);
}

static void ble_mesh_msg_access_recv_cbk(uint32_t opcode, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    ble_mesh_msg_header_t recv_ctx = {
        .app_idx = ctx->app_idx,
        .net_idx = ctx->net_idx,
        .src_addr = ctx->addr,
        .dest_addr = ctx->recv_dst,
        .recv_rssi = ctx->recv_rssi,
        .recv_ttl = ctx->recv_ttl,
        .recv_op = opcode,
    };
    if (g_ble_mesh_msg_callbacks.access_cb != NULL) {
        g_ble_mesh_msg_callbacks.access_cb(&recv_ctx, buf->om_data, buf->om_len);
    }
}

static void ble_mesh_beacon_recv_cbk(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf)
{
    if (g_ble_mesh_msg_callbacks.beacon_recv_cb != NULL) {
        bd_addr_t bd_addr = {0};
        if (addr != NULL) {
            bd_addr.type = addr->type;
            (void)memcpy_s(bd_addr.addr, sizeof(bd_addr.addr), addr->val, sizeof(addr->val));
        }
        
        g_ble_mesh_msg_callbacks.beacon_recv_cb(&bd_addr, rssi, buf->om_data, buf->om_len);
    }
}

static bool ble_mesh_prov_data_recv_cbk(const bt_addr_le_t *addr, int8_t rssi, uint32_t link_id, struct os_mbuf *buf)
{
    if (g_ble_mesh_msg_callbacks.pb_adv_recv_cb != NULL) {
        bd_addr_t bd_addr = {0};
        if (addr != NULL) {
            bd_addr.type = addr->type;
            (void)memcpy_s(bd_addr.addr, sizeof(bd_addr.addr), addr->val, sizeof(addr->val));
        }
        
        return g_ble_mesh_msg_callbacks.pb_adv_recv_cb(&bd_addr, rssi, link_id, buf->om_data, buf->om_len);
    }
    return true;
}

static bool ble_mesh_rmt_prov_data_recv_cbk(uint16_t src_addr, int8_t rssi,
                                            uint32_t link_id, struct os_mbuf *buf)
{
    if (g_ble_mesh_msg_callbacks.rmt_pb_adv_recv_cb != NULL) {
        return g_ble_mesh_msg_callbacks.rmt_pb_adv_recv_cb(src_addr, rssi, link_id, buf->om_data, buf->om_len);
    }
    return true;
}

errcode_t ble_mesh_msg_register_callbacks(const ble_mesh_msg_callbacks_t *func)
{
    if (func == NULL) {
        BT_ERR("[ERR] input param null!");
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    if (memcpy_s(&g_ble_mesh_msg_callbacks, sizeof(g_ble_mesh_msg_callbacks),
        func, sizeof(ble_mesh_msg_callbacks_t)) != EOK) {
        BT_ERR("[ERR] msg register callback memcpy fail!");
        return ERRCODE_BLE_MESH_FAIL;
    }
    if (func->access_cb != NULL) {
        bt_mesh_msg_cb_set(ble_mesh_msg_access_recv_cbk);
    }

    if (func->beacon_recv_cb != NULL) {
        bt_mesh_beacon_register_cbks(ble_mesh_beacon_recv_cbk);
    }

    if (func->pb_adv_recv_cb != NULL) {
        bt_mesh_prov_adv_recv_cb_set(ble_mesh_prov_data_recv_cbk);
    }

    if (func->rmt_pb_adv_recv_cb != NULL) {
        bt_mesh_rmt_prov_adv_recv_cb_set(ble_mesh_rmt_prov_data_recv_cbk);
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_beacon_send(uint8_t trans_count, uint8_t interval, const uint8_t *data, uint8_t len)
{
    uint8_t remaining_interval = interval;
    uint8_t remaining_trans_count = trans_count;
    struct os_mbuf *buf = NULL;
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (data == NULL || len == 0) {
        BT_ERR("beacon_send para invalid:%p len:%d", data, len);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    BT_DBG("beacon_send");
    uint8_t xmit = 0;
    if (remaining_trans_count == 0 || remaining_trans_count > BLE_MESH_MAX_TRANSMIT_COUNT) {
        remaining_trans_count = BLE_MESH_DEFAULT_TRANS;
    }
    
    if (remaining_interval == 0) {
        remaining_interval = BLE_MESH_DEFAULT_INTERVAL;
    }
    
    xmit = BT_MESH_TRANSMIT(remaining_trans_count, remaining_interval);
    
    buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, xmit, K_NO_WAIT);
    if (!buf) {
        BT_ERR("Unable to allocate beacon buffer");
        return ERRCODE_BLE_MESH_ALLOC_BUFF_FAILED;
    }
    net_buf_add_mem(buf, data, len);
    bt_mesh_adv_send(buf, NULL, NULL);
    net_buf_unref(buf);

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_clear_msg_buff(void)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    bt_mesh_clear_adv_buf();
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_get_msg_buff_count(uint8_t *count)
{
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (count == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    *count = bt_mesh_get_adv_buf_num();
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_prov_adv_send(const uint8_t *data, uint16_t len)
{
    int ret = 0;
    
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    
    if (data == NULL || len == 0) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    struct os_mbuf *buf = PROV_BUF(len);
    if (buf == NULL) {
        BT_ERR("Unable to allocate beacon buffer");
        return ERRCODE_BLE_MESH_ALLOC_BUFF_FAILED;
    }

    net_buf_reserve(buf, PROV_BEARER_BUF_HEADROOM);

    net_buf_simple_add_mem(buf, data, len);
    ret = bt_mesh_prov_send_adv(buf);
    if (ret != 0) {
        BT_ERR("Failed to send prov data, ret:%d", ret);
        os_mbuf_free_chain(buf);
        return ble_mesh_errcode_convert(ret);
    }

    os_mbuf_free_chain(buf);
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_rmt_prov_pdu_report(bool send_rel, const uint8_t *data, uint16_t len)
{
    int ret = 0;
    
    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    
    if (data == NULL || len == 0) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    ret = bt_mesh_rmt_prov_pdu_report_send(send_rel, 0, data, len);
    if (ret != 0) {
        return ble_mesh_errcode_convert(ret);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_extend_segment_adv(bool flag)
{
    bt_mesh_set_en_adv(flag);
    return ERRCODE_BLE_MESH_SUCCESS;
}