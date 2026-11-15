/*  Bluetooth Mesh */

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "syscfg/syscfg.h"
#define BLE_NPL_LOG_MODULE BLE_MESH_ADV_LOG
#include <nimble/nimble_npl_log.h>

#include "atomic_adapter.h"
#include "mesh/mesh.h"
#include "ble_hs_adv.h"
#include "bts_def.h"
#include "ble_gap.h"
#include "mesh/porting.h"

#include "osal_addr.h"
#include "adv.h"
#include "net.h"
#include "foundation.h"
#include "beacon.h"
#include "priv_beacon.h"
#include "prov.h"
#include "proxy.h"

/* Window and Interval are equal for continuous scanning */
#define MESH_SCAN_INTERVAL    BT_MESH_ADV_SCAN_UNIT(BT_MESH_SCAN_INTERVAL_MS)
#define MESH_SCAN_WINDOW      BT_MESH_ADV_SCAN_UNIT(BT_MESH_SCAN_WINDOW_MS)

const uint8_t bt_mesh_adv_type[BT_MESH_ADV_TYPES] = {
	[BT_MESH_ADV_PROV]   = BT_DATA_MESH_PROV,
	[BT_MESH_ADV_DATA]   = BT_DATA_MESH_MESSAGE,
	[BT_MESH_ADV_BEACON] = BT_DATA_MESH_BEACON,
	[BT_MESH_ADV_URI]    = BT_DATA_URI,
};

extern uint8_t g_mesh_addr_type;

struct os_mbuf_pool adv_os_mbuf_pool;
struct ble_npl_eventq bt_mesh_adv_queue;
#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
uint8_t g_print_scan_addr_count = 0;
#endif

os_membuf_t adv_buf_mem[OS_MEMPOOL_SIZE(
        MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT),
        BT_MESH_ADV_DATA_SIZE + BT_MESH_MBUF_HEADER_SIZE)];
struct os_mempool adv_buf_mempool;

static struct bt_mesh_adv adv_pool[CONFIG_BT_MESH_ADV_BUF_COUNT];

static struct bt_mesh_adv *adv_alloc(int id)
{
	if(id >= CONFIG_BT_MESH_ADV_BUF_COUNT){
		BT_ERR("invalid id: %d", id);
		return NULL;
	}
	return &adv_pool[id];
}

struct os_mbuf *bt_mesh_adv_create_from_pool(struct os_mbuf_pool *pool,
					     bt_mesh_adv_alloc_t get_id,
					     enum bt_mesh_adv_type type,
					     uint8_t xmit, int32_t timeout)
{
	struct bt_mesh_adv *adv;
	struct os_mbuf *buf;

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
		BT_WARN("Refusing to allocate buffer while suspended");
		return NULL;
	}

	buf = os_mbuf_get_pkthdr(pool, BT_MESH_ADV_USER_DATA_SIZE);
	if (!buf) {
		return NULL;
	}

	adv = get_id(net_buf_id(buf));
	if(!adv) {
		os_mbuf_free_chain(buf);
		return NULL;
	}
	BT_MESH_ADV(buf) = adv;

	memset(adv, 0, sizeof(*adv));

	adv->type         = type;
	adv->xmit         = xmit;

	adv->ref_cnt = 1;
	ble_npl_event_set_arg(&adv->ev, buf);

	return buf;
}

struct os_mbuf *bt_mesh_adv_create(enum bt_mesh_adv_type type, uint8_t xmit,
				   int32_t timeout)
{
	return bt_mesh_adv_create_from_pool(&adv_os_mbuf_pool, adv_alloc, type,
					    xmit, timeout);
}

void bt_mesh_adv_send(struct os_mbuf *buf, const struct bt_mesh_send_cb *cb,
		      void *cb_data)
{
	BT_DBG("buf %p, type 0x%02x len %u: %s", buf, BT_MESH_ADV(buf)->type, buf->om_len,
	       bt_hex(buf->om_data, buf->om_len));

	BT_MESH_ADV(buf)->cb = cb;
	BT_MESH_ADV(buf)->cb_data = cb_data;
	BT_MESH_ADV(buf)->busy = 1;

	net_buf_put(&bt_mesh_adv_queue, net_buf_ref(buf));
	bt_mesh_adv_buf_ready();
}

static void bt_mesh_scan_cb(const bt_addr_le_t *addr, int8_t rssi,
			    uint8_t adv_type, struct os_mbuf *buf)
{
	if (adv_type != BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
		return;
	}

#if BT_MESH_EXTENDED_DEBUG
	BT_DBG("len %u: %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));
#endif

	while (buf->om_len > 1) {
		struct net_buf_simple_state state;
		uint8_t len, type;

		len = net_buf_simple_pull_u8(buf);
		/* Check for early termination */
		if (len == 0) {
			return;
		}

		if (len > buf->om_len) {
			BT_DBG("AD malformed");
			return;
		}

		net_buf_simple_save(buf, &state);

		type = net_buf_simple_pull_u8(buf);

		switch (type) {
		case BLE_HS_ADV_TYPE_MESH_MESSAGE:
			bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
			break;
#if MYNEWT_VAL(BLE_MESH_PB_ADV)
		case BLE_HS_ADV_TYPE_MESH_PROV:
			bt_mesh_pb_adv_recv(addr, rssi, buf);
			break;
#endif
		case BLE_HS_ADV_TYPE_MESH_BEACON:
			bt_mesh_beacon_recv(addr, rssi, buf);
			break;
		default:
			break;
		}

		net_buf_simple_restore(buf, &state);
		net_buf_simple_pull_mem(buf, len);
	}
}

int
ble_adv_gap_mesh_cb(struct ble_gap_event *event, void *arg)
{
#if MYNEWT_VAL(BLE_EXT_ADV)
	struct ble_gap_ext_disc_desc *ext_desc;
#endif
	struct ble_gap_disc_desc *desc;
	struct os_mbuf *buf = NULL;
	uint8_t msg_type = 0;
	bd_addr_t net_addr = { 0 };
	switch (event->type) {
#if MYNEWT_VAL(BLE_EXT_ADV)
	case BLE_GAP_EVENT_EXT_DISC:
		ext_desc = &event->ext_disc;
		buf = os_mbuf_get_pkthdr(&adv_os_mbuf_pool, 0);
		if (!buf || os_mbuf_append(buf, ext_desc->data, ext_desc->length_data)) {
			BT_ERR("Could not append data");
			os_mbuf_free_chain(buf);
			break;
		}
		bt_mesh_scan_cb(&ext_desc->addr, ext_desc->rssi,
				ext_desc->legacy_event_type, buf);
		os_mbuf_free_chain(buf);
		break;
#endif
	case BLE_GAP_EVENT_DISC:
        desc = &event->disc;
		if (desc->length_data < 3) {
			break;
		}
		msg_type = desc->data[1];
		switch(msg_type) {
            case BLE_HS_ADV_TYPE_MESH_MESSAGE:
                break;
            case BLE_HS_ADV_TYPE_MESH_BEACON:
                // 给指定地址配网
                if (0 != desc->data[2]) {   // 0为BEACON_TYPE_UNPROVISIONED
                    break;
				}
#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
                ble_mesh_get_prov_addr(&net_addr);  // 获取配网的指定地址
                if (memcmp(desc->addr.val, net_addr.addr, 6)) {
					g_print_scan_addr_count++;
					if (g_print_scan_addr_count % 50 == 0) {
						BT_ERR("Not as expected, scan addr: %02x:%02x:%02x:%02x:%02x:%02x,\
							net_addr: %02x:%02x:%02x:%02x:%02x:%02x",
						desc->addr.val[0], desc->addr.val[1], desc->addr.val[2], desc->addr.val[3],
						desc->addr.val[4], desc->addr.val[5],
						net_addr.addr[0], net_addr.addr[1], net_addr.addr[2], net_addr.addr[3],
						net_addr.addr[4], net_addr.addr[5]);
                    }
                    return -EINVAL;
                }
#endif
                // 启停配网
			case BLE_HS_ADV_TYPE_MESH_PROV:
				break;
			default:
				return -EINVAL;
		}

        buf = (struct os_mbuf *)osal_kmalloc(sizeof(struct os_mbuf) + desc->length_data, 0);
        if (!buf) {
            BT_ERR("Could not allocate mbuf");
			return -EINVAL;
        }

		memset_s(buf, sizeof(struct os_mbuf), 0, sizeof(struct os_mbuf));
		buf->om_data = buf->om_databuf;
        // 复制数据到缓冲区
        if(EOK != memcpy_s(buf->om_data, desc->length_data, desc->data, desc->length_data)) {
			osal_kfree(buf);
			return -EINVAL;
		}
        buf->om_len = desc->length_data;
		buf->om_omp = NULL;
        buf->om_pkthdr_len = 0;

        bt_mesh_scan_cb(&desc->addr, desc->rssi, desc->event_type, buf);
		osal_kfree(buf);
		break;
	default:
		break;
	}
	
	return 0;
}

int bt_mesh_scan_enable(void)
{
	int err;

#if MYNEWT_VAL(BLE_EXT_ADV)
	struct ble_gap_ext_disc_params uncoded_params =
		{ .itvl = MESH_SCAN_INTERVAL, .window = MESH_SCAN_WINDOW,
		.passive = 1 };

	BT_DBG("");

	err =  ble_gap_ext_disc(g_mesh_addr_type, 0, 0, 0, 0, 0,
				&uncoded_params, NULL, NULL, NULL);
#else
	struct ble_gap_disc_params scan_param =
		{ .passive = 1, .filter_duplicates = 0, .itvl =
		  MESH_SCAN_INTERVAL, .window = MESH_SCAN_WINDOW };

	BT_DBG("");

	err =  ble_gap_disc(g_mesh_addr_type, BLE_HS_FOREVER, &scan_param,
			    NULL, NULL);
#endif
	if (err && err != BLE_HS_EALREADY) {
		BT_ERR("starting scan failed (err %d)", err);
		return err;
	}

	return 0;
}

int bt_mesh_scan_disable(void)
{
	int err;

	BT_DBG("");

	err = ble_gap_disc_cancel();
	if (err && err != BLE_HS_EALREADY) {
		BT_ERR("stopping scan failed (err %d)", err);
		return err;
	}

	return 0;
}
