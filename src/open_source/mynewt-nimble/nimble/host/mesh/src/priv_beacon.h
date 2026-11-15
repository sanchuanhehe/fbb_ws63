/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: BLE Mesh Priv Beacon.
 */

#ifndef __PRIV_BEACON_H__
#define __PRIV_BEACON_H__

#include "os/os_mbuf.h"

#define PROVISIONED_INTERVAL      (K_SECONDS(10))
#define RESCHEDULE_TIME_20        (K_MSEC(20))
#define MAX_BEACON_TIME           (K_SECONDS(600))
#define MIN_BEACON_TIME           (K_SECONDS(10))
#define MIN_BEACON_TIME_NUM       (10)

#define BEACON_TYPE_UNPROVISIONED  0x00
#define BEACON_TYPE_SECURE         0x01
#define BEACON_TYPE_PRIVATE        0x02

#define SEC_BEACON_MSG_LEN      21
#define PRI_BEACON_MSG_LEN      26
#define PRI_BEACON_KEY_LEN      16
#define PRI_BEACON_RANDOM_LEN   13
#define PRI_BEACON_DATE_LEN     5
#define PRI_BEACON_TAG_LEN      8
#define PRI_BEACON_OUT_DATE_LEN 5

/* 3 transmissions, 20ms interval */
#define UNPROV_XMIT                BT_MESH_TRANSMIT(2, 20)
/* 1 transmission, 20ms interval */
#define PROV_XMIT                  BT_MESH_TRANSMIT(0, 20)

/* If the interval has passed or is within 5 seconds from now send a beacon */
#define BEACON_THRESHOLD(beacon) \
    (K_SECONDS(10 * ((beacon)->last + 1)) - K_SECONDS(5))

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static struct {
    uint16_t idx;
    uint8_t val[13];
    uint64_t timestamp;
} priv_random;
#endif

struct beacon_params {
    union {
        const uint8_t *net_id;
        struct {
            const uint8_t *data;
            const uint8_t *random;
        };
    };
    const uint8_t *auth;
    uint32_t iv_index;
    uint8_t flags;
    bool new_key;
};

void private_beacon_recv(struct os_mbuf *buf);

void bt_mesh_beacon_cache_clear(struct bt_mesh_subnet *sub);

void bt_mesh_beacon_cache_clear(struct bt_mesh_subnet *sub);

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
void private_beacon_create(struct bt_mesh_subnet *sub, struct os_mbuf *buf);
int private_beacon_update(struct bt_mesh_subnet *sub);
int private_random_update(void);
int priv_beacon_send(struct bt_mesh_subnet *sub, void *cb_data);
#if CONFIG_BLE_MESH_PTS_MODE
int bt_mesh_cmd_priv_beacon_send(uint8_t key_refresh);
#endif
#endif

void private_beacon_recv(struct os_mbuf *buf);
void net_beacon_register(struct bt_mesh_beacon *beacon);
void bt_mesh_priv_beacon_srv_store_schedule(void);
#endif
