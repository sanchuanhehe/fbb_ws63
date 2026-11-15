/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PROV_H__
#define __PROV_H__

#include "prov_bearer.h"
#include "os/os_mbuf.h"
#include "mesh/mesh.h"
#include "../src/ble_hs_conn_priv.h"

#define PROV_ERR_NONE          0x00
#define PROV_ERR_NVAL_PDU      0x01
#define PROV_ERR_NVAL_FMT      0x02
#define PROV_ERR_UNEXP_PDU     0x03
#define PROV_ERR_CFM_FAILED    0x04
#define PROV_ERR_RESOURCES     0x05
#define PROV_ERR_DECRYPT       0x06
#define PROV_ERR_UNEXP_ERR     0x07
#define PROV_ERR_ADDR          0x08
#define PROV_ERR_INVALID_DATA  0x09

#define AUTH_METHOD_NO_OOB     0x00
#define AUTH_METHOD_STATIC     0x01
#define AUTH_METHOD_OUTPUT     0x02
#define AUTH_METHOD_INPUT      0x03

#define OUTPUT_OOB_BLINK       0x00
#define OUTPUT_OOB_BEEP        0x01
#define OUTPUT_OOB_VIBRATE     0x02
#define OUTPUT_OOB_NUMBER      0x03
#define OUTPUT_OOB_STRING      0x04

#define INPUT_OOB_PUSH         0x00
#define INPUT_OOB_TWIST        0x01
#define INPUT_OOB_NUMBER       0x02
#define INPUT_OOB_STRING       0x03

#define PUB_KEY_NO_OOB         0x00
#define PUB_KEY_OOB            0x01

#define PROV_INVITE            0x00
#define PROV_CAPABILITIES      0x01
#define PROV_START             0x02
#define PROV_PUB_KEY           0x03
#define PROV_INPUT_COMPLETE    0x04
#define PROV_CONFIRM           0x05
#define PROV_RANDOM            0x06
#define PROV_DATA              0x07
#define PROV_COMPLETE          0x08
#define PROV_FAILED            0x09

#define PROV_NO_PDU            0xff

#define DEVICE_KEY_LEN         16

#define PDU_LEN_INVITE         1
#define PDU_LEN_CAPABILITIES   11
#define PDU_LEN_START          5
#define PDU_LEN_PUB_KEY        64
#define PDU_LEN_INPUT_COMPLETE 0
#define PDU_LEN_CONFIRM        32 /* Max size */
#define PDU_LEN_RANDOM         32 /* Max size */
#define PDU_LEN_DATA           33
#define PDU_LEN_COMPLETE       0
#define PDU_LEN_FAILED         1

#define PDU_OP_LEN             1

#define PROV_ALG_P256          0x00

#define PROV_IO_OOB_SIZE_MAX   8 /* in bytes */

#define PRIV_KEY_SIZE 32
#define PUB_KEY_SIZE  PDU_LEN_PUB_KEY
#define DH_KEY_SIZE   32
 
#define PROV_AUTH_MAX_LEN   32
#define STATIC_AUTH_LEN 16
#define PROV_BUF(len) \
	NET_BUF_SIMPLE(PROV_BEARER_BUF_HEADROOM + PDU_OP_LEN + len)

enum {
	WAIT_PUB_KEY,           /* Waiting for local PubKey to be generated */
	LINK_ACTIVE,            /* Link has been opened */
	WAIT_NUMBER,            /* Waiting for number input from user */
	WAIT_STRING,            /* Waiting for string input from user */
	NOTIFY_INPUT_COMPLETE,  /* Notify that input has been completed. */
	PROVISIONER,            /* The link was opened as provisioner */
	OOB_PUB_KEY,            /* OOB Public key used */
	PUB_KEY_SENT,           /* Public key has been sent */
	REMOTE_PUB_KEY,         /* Remote key has been received */
	INPUT_COMPLETE,         /* Device input completed */
	WAIT_CONFIRM,           /* Wait for send confirm */
	WAIT_AUTH,              /* Wait for auth response */
	OOB_STATIC_KEY,         /* OOB Static Authentication */
	WAIT_DH_KEY, /* Wait for DH Key */

	REPROVISION,            /* The link was opened as a reprovision target */
	COMPLETE,               /* The provisioning process completed. */
	PROV_SIG_MESH,          /* The prov is open as sig mesh prov type */
	NUM_FLAGS,
};

/** Provisioning role */
struct bt_mesh_prov_role {
	void (*link_opened)(void);

	void (*link_closed)(void);

	void (*error)(uint8_t reason);

	void (*input_complete)(void);

	void (*op[10])(const uint8_t *data);
};

struct bt_mesh_prov_link {
	ATOMIC_DEFINE(flags, NUM_FLAGS);

	const struct prov_bearer *bearer;
	const struct bt_mesh_prov_role *role;

	uint16_t addr;                  /* Assigned address */
	uint8_t algorithm;              /* Authen algorithm */
	uint8_t oob_method;             /* Authen method */
	uint8_t oob_action;             /* Authen action */
	uint8_t oob_size;               /* Authen size */
	uint8_t auth[PROV_AUTH_MAX_LEN];               /* Authen value */
	uint8_t dhkey[BT_DH_KEY_LEN];   /* Calculated DHKey */
	uint8_t expect;                 /* Next expected PDU */
	uint8_t cur_pdu_type;           /* current PDU */
	uint8_t conf[PROV_AUTH_MAX_LEN];               /* Local/Remote Confirmation */
	uint8_t rand[PROV_AUTH_MAX_LEN];               /* Local Random */
 
	uint8_t conf_salt[PROV_AUTH_MAX_LEN];          /* ConfirmationSalt */
	uint8_t conf_key[PROV_AUTH_MAX_LEN];           /* ConfirmationKey */

	/* ConfirmationInput fields: */
	struct {
		uint8_t invite[PDU_LEN_INVITE];
		uint8_t capabilities[PDU_LEN_CAPABILITIES];
		uint8_t start[PDU_LEN_START];
		uint8_t pub_key_provisioner[PDU_LEN_PUB_KEY]; /* big-endian */
		uint8_t pub_key_device[PDU_LEN_PUB_KEY]; /* big-endian */
	} conf_inputs;
	uint8_t prov_salt[16];          /* Provisioning Salt */
};

extern struct bt_mesh_prov_link bt_mesh_prov_link;
extern const struct bt_mesh_prov *bt_mesh_prov;

static inline int bt_mesh_prov_send(struct os_mbuf *buf,
				    prov_bearer_send_complete_t cb)
{
	if (bt_mesh_prov_link.bearer == NULL) {
		return -EINVAL;
	}
	return bt_mesh_prov_link.bearer->send(buf, cb, NULL);
}

static inline void bt_mesh_prov_buf_init(struct os_mbuf *buf, uint8_t type)
{
	net_buf_reserve(buf, PROV_BEARER_BUF_HEADROOM);
	net_buf_simple_add_u8(buf, type);
}

static inline uint8_t bt_mesh_prov_auth_size_get(void)
{
	return bt_mesh_prov_link.algorithm == BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM ? 16 : 32;
}
 
int bt_mesh_prov_reset_state(void);

bool bt_mesh_prov_active(void);

int bt_mesh_prov_auth(bool is_provisioner, uint8_t method, uint8_t action, uint8_t size);

int bt_mesh_pb_gatt_open(uint16_t conn_handle);
int bt_mesh_pb_gatt_close(uint16_t conn_handle);
int bt_mesh_pb_gatt_recv(uint16_t conn_handle, struct os_mbuf *buf);

const struct bt_mesh_prov *bt_mesh_prov_get(void);

void bt_mesh_prov_reset_link(void);

void bt_mesh_prov_complete(uint16_t net_idx, uint16_t addr);
void bt_mesh_prov_reset(void);

const struct prov_bearer_cb *bt_mesh_prov_bearer_cb_get(void);

void bt_mesh_pb_adv_recv(const bt_addr_le_t *addr, int8_t rssi, struct os_mbuf *buf);
int bt_mesh_prov_adv_link_id_set(uint32_t link_id);
int bt_mesh_prov_adv_link_set(uint32_t link_id, bool is_filter);
bool bt_mesh_pb_is_sig_mesh_prov(void);
void bt_mesh_pb_set_sig_mesh_prov(bool flag);
typedef bool (* pb_adv_recv)(const bt_addr_le_t *addr, int8_t rssi, uint32_t link_id, struct os_mbuf *buf);
void bt_mesh_prov_adv_recv_cb_set(pb_adv_recv func);

int bt_mesh_prov_init(const struct bt_mesh_prov *prov);

int bt_mesh_prov_deinit(void);
#endif
