/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon.
 */

#include "syscfg/syscfg.h"

#define BLE_NPL_LOG_MODULE BLE_MESH_BEACON_LOG
#include <nimble/nimble_npl_log.h>

#include <errno.h>
#include <assert.h>
#include "os/os_mbuf.h"
#include "mesh/mesh.h"
#include "adv.h"
#include "mesh_priv.h"
#include "net.h"
#include "prov.h"
#include "crypto.h"
#include "beacon.h"
#include "priv_beacon.h"
#include "foundation.h"
#include "atomic_adapter.h"

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static int priv_cache_check(struct bt_mesh_subnet *sub, void *beacon_data)
{
    return !memcmp(sub->priv_beacon_date.cache, beacon_data, PRI_BEACON_MSG_LEN);
}

static void priv_cache_add(const uint8_t auth[PRI_BEACON_MSG_LEN], struct bt_mesh_beacon *beacon)
{
    memcpy(beacon->cache, auth, CONFIG_BT_MESH_PRIV_BEACONS);
}
#endif

void bt_mesh_beacon_cache_clear(struct bt_mesh_subnet *sub)
{
    (void)memset(sub->secure_beacon.cache, 0, sizeof(sub->secure_beacon.cache));
#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    (void)memset(sub->priv_beacon_date.cache, 0, sizeof(sub->priv_beacon_date.cache));
#endif
}


#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static void priv_beacon_complete(int err, void *user_data)
{
    struct bt_mesh_subnet *sub = user_data;

    BT_DBG("err %d", err);
    sub->priv_beacon_date.sent = k_uptime_get_32();
}
#endif

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
int private_random_update(void)
{
    uint8_t interval = bt_mesh_priv_beacon_update_interval_get();
    uint64_t uptime = k_uptime_get();
    int err;

    if (bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED && interval &&
        uptime - priv_random.timestamp < (MIN_BEACON_TIME_NUM * interval * MSEC_PER_SEC) &&
        priv_random.timestamp != 0) {
        return 0;
    }

    err = bt_rand(priv_random.val, sizeof(priv_random.val));
    if (err) {
        return err;
    }

    priv_random.idx++;
    priv_random.timestamp = uptime;

    return 0;
}

int private_beacon_update(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_subnet_keys *keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];
    uint8_t flags = bt_mesh_net_flags(sub);
    int err;

    err = bt_mesh_beacon_encrypt(keys->priv_beacon, flags, bt_mesh.iv_index,
                                 priv_random.val, sub->priv_beacon_ctx.data,
                                 sub->priv_beacon_date.auth);
    if (err) {
        BT_ERR("Can't encrypt private beacon");
        return err;
    }

    sub->priv_beacon_ctx.idx = priv_random.idx;
    return 0;
}

void private_beacon_create(struct bt_mesh_subnet *sub, struct os_mbuf *buf)
{
    int err;

    err = private_random_update();
    if (err) {
        return;
    }

    if (sub->priv_beacon_ctx.idx != priv_random.idx) {
        err = private_beacon_update(sub);
        if (err) {
            return;
        }
    }

    net_buf_simple_add_u8(buf, BEACON_TYPE_PRIVATE);
    net_buf_simple_add_mem(buf, (char*)priv_random.val, PRI_BEACON_RANDOM_LEN);
    net_buf_simple_add_mem(buf, (char*)sub->priv_beacon_ctx.data, PRI_BEACON_DATE_LEN);
    net_buf_simple_add_mem(buf, (char*)sub->priv_beacon_date.auth, PRI_BEACON_TAG_LEN);

    BT_DBG("0x%03x", sub->net_idx);
    return;
}

int priv_beacon_send(struct bt_mesh_subnet *sub, void *cb_data)
{
    static const struct bt_mesh_send_cb send_cb = {
        .end = priv_beacon_complete,
    };
    struct bt_mesh_beacon *beacon;
    uint32_t now = k_uptime_get_32();
    struct os_mbuf *buf;
    int32_t time_diff;
    int err = 0;

    beacon = &sub->priv_beacon_date;
    time_diff = now - beacon->sent;
    if (time_diff < MAX_BEACON_TIME && time_diff < BEACON_THRESHOLD(beacon)) {
        return -ENOMSG;
    }

    buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, PROV_XMIT, K_NO_WAIT);
    if (!buf) {
        BT_ERR("Unable to allocate beacon buffer");
        return -ENOMEM;
    }

    private_beacon_create(sub, buf);
    BT_DBG("len %u: %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

    bt_mesh_adv_send(buf, &send_cb, sub);
    net_buf_unref(buf);
    return err;
}

#if CONFIG_BLE_MESH_PTS_MODE
int bt_mesh_cmd_priv_beacon_send(uint8_t key_refresh)
{
    struct bt_mesh_subnet sub = {0};

    memcpy_s(&sub, sizeof(struct bt_mesh_subnet), bt_mesh_subnet_next(NULL), sizeof(struct bt_mesh_subnet));
    if (key_refresh) {
        sub.kr_phase = BT_MESH_KR_PHASE_2;
    } else {
        sub.kr_phase = BT_MESH_KR_NORMAL;
        memcpy_s(&sub.keys[0], sizeof(sub.keys[0]), &sub.keys[1], sizeof(sub.keys[1]));
        sub.keys[1].valid = 0U;
    }
    sub.secure_beacon.sent = 0;
    return priv_beacon_send(&sub, NULL);
}
#endif
#endif

static int priv_beacon_decrypt(struct bt_mesh_subnet *sub, void *cb_data)
{
    struct beacon_params *params = cb_data;
    uint8_t out[PRI_BEACON_OUT_DATE_LEN];
    int err;

    for (unsigned int i = 0; i < ARRAY_SIZE(sub->keys); i++) {
        if (!sub->keys[i].valid) {
            continue;
        }

        err = bt_mesh_beacon_decrypt(sub->keys[i].priv_beacon, params->random, params->data, params->auth, out);
        if (!err) {
            params->new_key = (i > 0);
            params->flags = out[0];
            params->iv_index = sys_get_be32(&out[1]);

            return err;
        }
    }

    return 0;
}

void net_beacon_register(struct bt_mesh_beacon *beacon)
{
    if (((bt_mesh_priv_beacon_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED) ||
         bt_mesh_beacon_enabled()) && beacon->cur < 0xff) {
        beacon->cur++;
    }
}

void private_beacon_recv(struct os_mbuf *buf)
{
    struct beacon_params params;
    struct bt_mesh_subnet *sub;
    struct bt_mesh_beacon *beacon;
    uint8_t *data;

    if (buf->om_len < PRI_BEACON_MSG_LEN) {
        BT_ERR("Too short private beacon (len %u)", buf->om_len);
        return;
    }

    sub = bt_mesh_subnet_find(priv_cache_check, buf->om_data);
    if (sub) {
        beacon = &sub->priv_beacon_date;
        net_beacon_register(beacon);
        return;
    }
    data = buf->om_data;

    params.random = net_buf_simple_pull_mem(buf, PRI_BEACON_RANDOM_LEN);
    params.data = net_buf_simple_pull_mem(buf, PRI_BEACON_DATE_LEN);
    params.auth = buf->om_data;

    sub = bt_mesh_subnet_find(priv_beacon_decrypt, &params);
    if (!sub) {
        BT_DBG("No subnet that matched beacon");
        return;
    }

    if (sub->kr_phase == BT_MESH_KR_PHASE_2 && !params.new_key) {
        BT_WARN("Ignoring Phase 2 KR Update secured using old key");
        return;
    }

    beacon = &sub->priv_beacon_date;
    priv_cache_add(data, beacon);

    bt_mesh_kr_update(sub, BT_MESH_KEY_REFRESH(params.flags), params.new_key);

    /* If we have NetKey0 accept initiation only from it */
    if (bt_mesh_subnet_get(BT_MESH_KEY_PRIMARY) &&
        sub->net_idx != BT_MESH_KEY_PRIMARY) {
        BT_WARN("Ignoring secure beacon on non-primary subnet");
        net_beacon_register(beacon);
        return;
    }

    BT_DBG("net_idx 0x%04x iv_index 0x%08x, current iv_index 0x%08x",
           sub->net_idx, params.iv_index, bt_mesh.iv_index);

    if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR) &&
        (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS) ==
         BT_MESH_IV_UPDATE(params.flags))) {
        bt_mesh_beacon_ivu_initiator(false);
    }

    bt_mesh_net_iv_update(params.iv_index, BT_MESH_IV_UPDATE(params.flags));
    net_beacon_register(beacon);
}