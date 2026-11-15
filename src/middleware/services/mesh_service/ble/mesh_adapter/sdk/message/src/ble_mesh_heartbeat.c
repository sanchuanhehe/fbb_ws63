/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh model.
 *
 * History:
 * 2025-4-15, Create file.
 */
 
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "mesh/glue.h"
#include "net.h"
#include "heartbeat.h"

#include "bts_def.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_heartbeat.h"

#define MAX_HEARTBEAT_PUB_COUNT 0x11
#define MAX_HEARTBEAT_PUB_PERIOD 0x11

static ble_mesh_heartbeat_callbacks_t g_heartbeat_callback = {0};

static void ble_mesh_beacon_recv_cb(const struct bt_mesh_hb_sub *sub, uint8_t hops, uint16_t feature)
{
    ble_mesh_heartbeat_sub_t heartbeat_sub_t = {0};
    if (g_heartbeat_callback.heartbeat_recv_cb != NULL) {
        heartbeat_sub_t.count = sub->count;
        heartbeat_sub_t.period = sub->period;
        heartbeat_sub_t.remaining = sub->remaining;
        heartbeat_sub_t.src = sub->src;
        heartbeat_sub_t.dst = sub->dst;
        heartbeat_sub_t.max_hops = sub->max_hops;
        heartbeat_sub_t.min_hops = sub->min_hops;

        g_heartbeat_callback.heartbeat_recv_cb(&heartbeat_sub_t, hops, feature);
    }
}

errcode_t ble_mesh_heartbeat_callbacks(const ble_mesh_heartbeat_callbacks_t *func)
{
    if (func == NULL) {
        BT_ERR("[ERR] input param null!");
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    
    g_heartbeat_callback.heartbeat_recv_cb = func->heartbeat_recv_cb;

    if (g_heartbeat_callback.heartbeat_recv_cb != NULL) {
        hb_cb.recv = ble_mesh_beacon_recv_cb;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_heartbeat_pub(const ble_mesh_heartbeat_pub_t *pub)
{
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (pub == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    struct bt_mesh_hb_pub hb_pub = {
        .count = bt_mesh_hb_pwr2(pub->count),
        .dst = pub->dst,
        .feat = pub->feature,
        .net_idx = pub->net_idx,
        .period = bt_mesh_hb_pwr2(pub->period),
        .ttl = pub->ttl,
    };
    if (BT_MESH_ADDR_IS_VIRTUAL(hb_pub.dst)) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    if (pub->count > MAX_HEARTBEAT_PUB_COUNT && pub->count != 0xff) {
        BT_ERR("invalid count_log value 0x%02x", pub->count);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    if (pub->period > MAX_HEARTBEAT_PUB_PERIOD) {
        BT_ERR("invalid period_log value 0x%02x", pub->period);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    if (pub->ttl > BT_MESH_TTL_MAX && pub->ttl != BT_MESH_TTL_DEFAULT) {
        BT_ERR("invalid TTL value 0x%02x", pub->ttl);
        return ERRCODE_BLE_MESH_TTL_NOT_INVALID;
    }

    if (hb_pub.net_idx > 0xfff) {
        BT_ERR("invalid net key idx 0x%04x", hb_pub.net_idx);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    int ret = bt_mesh_hb_pub_set(&hb_pub);
    return ble_mesh_errcode_convert(ret);
}

errcode_t ble_mesh_get_heartbeat_pub(ble_mesh_heartbeat_pub_t *pub)
{
    struct bt_mesh_hb_pub val = {0};
    if (pub == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    bt_mesh_hb_pub_get(&val);
    pub->dst = val.dst;
    pub->count = hb_pub_count_log(val.count);
    pub->period = bt_mesh_hb_log(val.period);
    pub->ttl = val.ttl;
    pub->feature = val.feat;
    pub->net_idx = val.net_idx;
    return ERRCODE_BLE_MESH_SUCCESS;
}

uint16_t ble_mesh_get_heartbeat_pub_addr(void)
{
    struct bt_mesh_hb_pub pub = {0};
    bt_mesh_hb_pub_get(&pub);
    return pub.dst;
}

errcode_t ble_mesh_start_heartbeat(void)
{
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    return ble_mesh_errcode_convert(bt_mesh_hb_send());
}