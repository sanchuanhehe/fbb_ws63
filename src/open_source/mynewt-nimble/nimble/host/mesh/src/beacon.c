/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
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
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
#include "remote_prov_server.h"
#endif
static struct k_work_delayable beacon_timer;
static struct bt_mesh_subnet *beacon_send_sub_curr = NULL;
static beacon_recv g_beacon_recv = NULL;

static int cache_check(struct bt_mesh_subnet *sub, void *beacon_data)
{
    return !memcmp(sub->secure_beacon.cache, beacon_data, SEC_BEACON_MSG_LEN);
}

static void cache_add(const uint8_t auth[SEC_BEACON_MSG_LEN], struct bt_mesh_beacon *beacon)
{
    memcpy(beacon->cache, auth, SEC_BEACON_MSG_LEN);
}

static void beacon_complete(int err, void *user_data)
{
	struct bt_mesh_subnet *sub = user_data;

	BT_DBG("err %d", err);

	sub->secure_beacon.sent = k_uptime_get_32();
}

static int secure_beacon_create(struct bt_mesh_subnet *sub, 
			   struct os_mbuf *buf)
{
	uint8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;

	net_buf_simple_add_u8(buf, BEACON_TYPE_SECURE);

	keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];

	net_buf_simple_add_u8(buf, flags);

	/* Network ID */
	net_buf_simple_add_mem(buf, keys->net_id, 8);

	/* IV Index */
	net_buf_simple_add_be32(buf, bt_mesh.iv_index);

	net_buf_simple_add_mem(buf, sub->secure_beacon.auth, 8);

	BT_DBG("net_idx 0x%04x flags 0x%02x NetID %s", sub->net_idx,
	       flags, bt_hex(keys->net_id, 8));
	BT_DBG("IV Index 0x%08x Auth %s", (unsigned) bt_mesh.iv_index,
	       bt_hex(sub->secure_beacon.auth, 8));

    return 0;
}

int bt_mesh_beacon_create(struct bt_mesh_subnet *sub, struct os_mbuf *buf, bool priv)
{
	// bug_fix: add null pointer check
	if (sub == NULL || buf == NULL) {
		BT_ERR("sub(%p) or buff(%p) is null", sub, buf);
		return -1;
	}

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    if (priv) {
        private_beacon_create(sub, buf);
        return 0;
    }
#endif
    secure_beacon_create(sub, buf);
    return 0;
}

static int secure_beacon_send(struct bt_mesh_subnet *sub, void *cb_data)
{
	static const struct bt_mesh_send_cb send_cb = {
		.end = beacon_complete,
	};
	// bug_fix: add null pointer check
	if (sub == NULL) {
		BT_ERR("sub is null");
		return -ENOMEM;
	}

    struct bt_mesh_beacon *beacon;
	uint32_t now = k_uptime_get_32();
	struct os_mbuf *buf = NULL;
	int32_t time_diff;
    int err = 0;

    beacon = &sub->secure_beacon;
    time_diff = now - beacon->sent;
    if (time_diff < MAX_BEACON_TIME &&
        time_diff < BEACON_THRESHOLD(beacon)) {
        return -ENOMSG;
	}

	buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, PROV_XMIT, K_NO_WAIT);
	if (!buf) {
		BT_ERR("Unable to allocate beacon buffer");
		return -ENOMEM;
	}

    secure_beacon_create(sub, buf);
	BT_DBG("len %u: %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

	bt_mesh_adv_send(buf, &send_cb, sub);
	net_buf_unref(buf);

    return err;
}

#if CONFIG_BLE_MESH_PTS_MODE
int bt_mesh_cmd_secure_beacon_send(uint8_t key_refresh)
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
    bt_mesh_beacon_update(&sub);

    return secure_beacon_send(&sub, NULL);
}
#endif

static int unprovisioned_beacon_send(void)
{
	const struct bt_mesh_prov *prov;
	uint8_t uri_hash[16] = { 0 };
	struct os_mbuf *buf;
	uint16_t oob_info;

	BT_DBG("unprovisioned_beacon_send");

	buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, UNPROV_XMIT, K_NO_WAIT);
	if (!buf) {
		BT_ERR("Unable to allocate beacon buffer");
		return -ENOBUFS;
	}

	prov = bt_mesh_prov_get();

	net_buf_add_u8(buf, BEACON_TYPE_UNPROVISIONED);
	net_buf_add_mem(buf, prov->uuid, 16);

	if (prov->uri && bt_mesh_s1(prov->uri, uri_hash) == 0) {
		oob_info = prov->oob_info | BT_MESH_PROV_OOB_URI;
	} else {
		oob_info = prov->oob_info;
	}

	net_buf_add_be16(buf, oob_info);
	net_buf_add_mem(buf, uri_hash, 4);

	bt_mesh_adv_send(buf, NULL, NULL);
	net_buf_unref(buf);

	return 0;
}

static void unprovisioned_beacon_recv(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf)
{
	const struct bt_mesh_prov *prov;
	uint8_t *uuid;
	uint16_t oob_info;
	uint32_t uri_hash_val;
	uint32_t *uri_hash = NULL;

	if (buf->om_len != 18 && buf->om_len != 22) {
		BT_ERR("Invalid unprovisioned beacon length (%u)", buf->om_len);
		return;
	}

	uuid = net_buf_simple_pull_mem(buf, 16);
	oob_info = net_buf_simple_pull_be16(buf);

	if (buf->om_len == 4) {
		uri_hash_val = net_buf_simple_pull_be32(buf);
		uri_hash = &uri_hash_val;
	}

	BT_DBG("uuid %s", bt_hex(uuid, 16));

	prov = bt_mesh_prov_get();

	if (prov->unprovisioned_beacon) {
		prov->unprovisioned_beacon(uuid,
					   (bt_mesh_prov_oob_info_t)oob_info,
					   uri_hash);
	}
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
	bt_mesh_rmt_prov_unprovisioned_beacon(addr, rssi, uuid, (bt_mesh_prov_oob_info_t)oob_info, uri_hash);
#endif
}

static void sub_update_beacon_observation(struct bt_mesh_subnet *sub)
{
    sub->secure_beacon.last = sub->secure_beacon.cur;
    sub->secure_beacon.cur = 0U;

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    sub->priv_beacon_date.last = sub->priv_beacon_date.cur;
    sub->priv_beacon_date.cur = 0U;
#endif
}

static void update_beacon_observation(void)
{
	static bool first_half;

	/* Observation period is 20 seconds, whereas the beacon timer
	 * runs every 10 seconds. We process what's happened during the
	 * window only after the seconnd half.
	 */
	first_half = !first_half;
	if (first_half) {
		return;
	}

	bt_mesh_subnet_foreach(sub_update_beacon_observation);
}

static bool beacons_send_next(void)
{
    int err = 0;
    struct bt_mesh_subnet *sub_first = bt_mesh_subnet_next(NULL);
    struct bt_mesh_subnet *sub_next;

    do {
        sub_next = bt_mesh_subnet_next(beacon_send_sub_curr);
        if (sub_next == sub_first && beacon_send_sub_curr != NULL) {
            beacon_send_sub_curr = NULL;
            return false;
        }

        beacon_send_sub_curr = sub_next;
		if (beacon_send_sub_curr == NULL) {
			BT_ERR("beacon_send_sub_curr is null");
			return false;
		}

        BT_DBG("sub cur net_id:%d kr_phase:%d", beacon_send_sub_curr->net_idx, beacon_send_sub_curr->kr_phase);
        if (bt_mesh_beacon_enabled() || atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
            err = secure_beacon_send(beacon_send_sub_curr, NULL);
        }

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
        if (bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED) {
            err |= priv_beacon_send(beacon_send_sub_curr, NULL);
        }
#endif
    } while (err);

    return true;
}

static void beacon_send(struct ble_npl_event *work)
{
	BT_DBG("is provisioned:%d:%d:%d\n", bt_mesh_is_provisioned(), bt_mesh_beacon_enabled(), atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR));
	if (bt_mesh_is_provisioned()) {
        if (!bt_mesh_beacon_enabled() && !atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR) &&
            (bt_mesh_priv_beacon_get() != BT_MESH_FEATURE_ENABLED)){
			return;
		}

        if (!beacon_send_sub_curr) {
            update_beacon_observation();
        }

		beacons_send_next();

		k_work_schedule(&beacon_timer, PROVISIONED_INTERVAL);

		return;
	}

	if (IS_ENABLED(BLE_MESH_PB_ADV)) {
		/* Don't send anything if we have an active provisioning link */
		if (!bt_mesh_prov_active() && atomic_test_bit(bt_mesh.flags, BT_MESH_UNPROV_BEACON_SEND)) {
			unprovisioned_beacon_send();
		}

		k_work_schedule(&beacon_timer, K_SECONDS(MYNEWT_VAL(BLE_MESH_UNPROV_BEACON_INT)));
	}
}

static bool auth_match(struct bt_mesh_subnet_keys *keys,
		       const struct beacon_params *params)
{
	uint8_t net_auth[8];

	if (memcmp(params->net_id, keys->net_id, 8)) {
		return false;
	}

	bt_mesh_beacon_auth(keys->beacon, params->flags, keys->net_id,
			    params->iv_index, net_auth);

	if (memcmp(params->auth, net_auth, 8)) {
		BT_WARN("Authentication Value %s != %s",
			bt_hex(params->auth, 8), bt_hex(net_auth, 8));
		return false;
	}

	return true;
}

static int subnet_by_id(struct bt_mesh_subnet *sub, void *cb_data)
{
	struct beacon_params *params = cb_data;

	for (int i = 0; i < ARRAY_SIZE(sub->keys); i++) {
		if (sub->keys[i].valid && auth_match(&sub->keys[i], params)) {
			params->new_key = (i > 0);
			return true;
		}
	}

	return false;
}

static void secure_beacon_recv(struct os_mbuf *buf)
{
	struct beacon_params params;
	struct bt_mesh_subnet *sub;
    struct bt_mesh_beacon *beacon;
	uint8_t *data;

    if (buf->om_len < 21) {
		BT_ERR("Too short secure beacon (len %u)", buf->om_len);
		return;
	}

	sub = bt_mesh_subnet_find(cache_check, buf->om_data);
	if (sub) {
        beacon = &sub->secure_beacon;
        net_beacon_register(beacon);
        return;
	}

	/* So we can add to the cache if auth matches */
	data = buf->om_data;

	params.flags = net_buf_simple_pull_u8(buf);
    params.net_id = net_buf_simple_pull_mem(buf, 8);
	params.iv_index = net_buf_simple_pull_be32(buf);
	params.auth = buf->om_data;

	BT_DBG("flags 0x%02x id %s iv_index 0x%08x",
	       params.flags, bt_hex(params.net_id, 8), params.iv_index);

	sub = bt_mesh_subnet_find(subnet_by_id, &params);
	if (!sub) {
		BT_DBG("No subnet that matched beacon");
		return;
	}

	if (sub->kr_phase == BT_MESH_KR_PHASE_2 && !params.new_key) {
		BT_WARN("Ignoring Phase 2 KR Update secured using old key");
		return;
	}

    beacon = &sub->secure_beacon;
    cache_add(data, beacon);

	bt_mesh_kr_update(sub, BT_MESH_KEY_REFRESH(params.flags),
			  params.new_key);

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

void bt_mesh_beacon_recv(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf)
{
	uint8_t type;
	// bug_fix: add null pointer check
	if (buf == NULL || buf->om_len < 1) {
		BT_ERR("Too short beacon");
		return;
	}
	BT_DBG("%u bytes: %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));

	type = net_buf_simple_pull_u8(buf);
	switch (type) {
	case BEACON_TYPE_UNPROVISIONED:
		if (IS_ENABLED(BLE_MESH_PB_ADV)) {
			unprovisioned_beacon_recv(addr, rssi, buf);
		}
		break;
	case BEACON_TYPE_SECURE:
		secure_beacon_recv(buf);
		break;
    case BEACON_TYPE_PRIVATE:
        private_beacon_recv(buf);
        break;
	default:
		if (g_beacon_recv != NULL) {
			g_beacon_recv(addr, rssi, buf);
		} else {
			BT_WARN("Unknown beacon type 0x%02x", type);
		}
		break;
	}
}

void bt_mesh_beacon_update(struct bt_mesh_subnet *sub)
{
	// bug_fix: add null pointer check
	if (sub == NULL) {
		BT_ERR("sub is null");
		return;
	}
	uint8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;
	int err;

	keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];

	BT_DBG("NetIndex 0x%03x Using %s key", sub->net_idx,
	       SUBNET_KEY_TX_IDX(sub) ? "new" : "current");
	BT_DBG("flags 0x%02x, IVI 0x%08x", flags, bt_mesh.iv_index);

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    sub->priv_beacon_ctx.idx = priv_random.idx - 1;
    priv_random.timestamp = 0;
#endif

    err = bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id,
				   bt_mesh.iv_index, sub->secure_beacon.auth);
	if (err) {
		BT_ERR("Failed updating net beacon for 0x%03x", sub->net_idx);
	}
}

static void subnet_evt(struct bt_mesh_subnet *sub, enum bt_mesh_key_evt evt)
{
	if (evt != BT_MESH_KEY_DELETED) {
		bt_mesh_beacon_update(sub);
	}
}

void bt_mesh_beacon_init(void)
{
	if (!bt_mesh_subnet_cb_list[1]) {
		bt_mesh_subnet_cb_list[1] = subnet_evt;
	}

	k_work_init_delayable(&beacon_timer, beacon_send);

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    private_random_update();
#endif
}

void bt_mesh_beacon_deinit(void)
{
    if (bt_mesh_subnet_cb_list[1] == subnet_evt) {
        bt_mesh_subnet_cb_list[1] = NULL;
    }
    k_work_cancel_delayable_delete(&beacon_timer);
}

void bt_mesh_beacon_ivu_initiator(bool enable)
{
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_INITIATOR, enable);

	/* Fire the beacon handler straight away if it's not already pending -
	 * in which case we'll fire according to the ongoing periodic sending.
	 * If beacons are disabled, the handler will exit early.
	 *
	 * An alternative solution would be to check whether beacons are enabled
	 * here, and cancel if not. As the cancel operation may fail, we would
	 * still have to implement an early exit mechanism, so we might as well
	 * just use this every time.
	 */
	k_work_schedule(&beacon_timer, K_NO_WAIT);
}

static void subnet_beacon_enable(struct bt_mesh_subnet *sub)
{
    sub->secure_beacon.last = 0U;
    sub->secure_beacon.cur = 0U;

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
    sub->priv_beacon_date.last = 0U;
    sub->priv_beacon_date.cur = 0U;
#endif

	bt_mesh_beacon_update(sub);
}

void bt_mesh_beacon_enable(void)
{
	if (bt_mesh_is_provisioned()) {
		bt_mesh_subnet_foreach(subnet_beacon_enable);
	}

	k_work_reschedule(&beacon_timer, K_NO_WAIT);
}

void bt_mesh_beacon_disable(void)
{
	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
		/* If this fails, we'll do an early exit in the work handler. */
		(void)k_work_cancel_delayable(&beacon_timer);
	}
}

void bt_mesh_beacon_register_cbks(beacon_recv func)
{
	g_beacon_recv = func;
}

void bt_mesh_beacon_set_unprov_beacon_flag(bool flag)
{
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_UNPROV_BEACON_SEND, flag);
	BT_INFO("set unprov beacon flag: %d provisioned:%d",
			 atomic_test_bit(bt_mesh.flags, BT_MESH_UNPROV_BEACON_SEND),
			 bt_mesh_is_provisioned());
	if (!flag && !bt_mesh_is_provisioned() && IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
		bt_le_adv_stop();
	}
}