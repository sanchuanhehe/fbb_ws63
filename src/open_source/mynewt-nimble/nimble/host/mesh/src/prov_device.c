/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2020 Lingao Meng
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BLE_NPL_LOG_MODULE BLE_MESH_PROV_LOG
#include <nimble/nimble_npl_log.h>

#include "testing.h"
#include "crypto.h"
#include "adv.h"
#include "mesh/mesh.h"
#include "net.h"
#include "rpl.h"
#include "beacon.h"
#include "priv_beacon.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "prov.h"
#include "atomic_adapter.h"
#include "settings.h"
#include "pb_gatt_srv.h"
#include "remote_prov_server.h"

static void send_pub_key(void);
static void reprovision_fail(void);

static int reset_state(void)
{
    return bt_mesh_prov_reset_state();
}

static void prov_send_fail_msg(uint8_t err)
{
	struct os_mbuf *buf = PROV_BUF(PDU_LEN_FAILED);

	BT_DBG("%u", err);

	bt_mesh_prov_link.expect = PROV_NO_PDU;

	bt_mesh_prov_buf_init(buf, PROV_FAILED);
	net_buf_simple_add_u8(buf, err);

	if (bt_mesh_prov_send(buf, NULL)) {
		BT_ERR("Failed to send Provisioning Failed message");
	}
	os_mbuf_free_chain(buf);
}

// add for provisioner resend prov data
static int prov_send_complete_msg(void)
{
	struct os_mbuf *msg = PROV_BUF(PDU_LEN_COMPLETE);;
	if (msg == NULL) {
		BT_ERR("Failed to alloc msg, complete send failed.");
		return -EINVAL;
	}
	bt_mesh_prov_buf_init(msg, PROV_COMPLETE);
	if (bt_mesh_prov_send(msg, NULL)) {
		BT_ERR("Failed to send Provisioning Complete");
		os_mbuf_free_chain(msg);
		return -EINVAL;
	}
	os_mbuf_free_chain(msg);
	return 0;
}

static void prov_fail(uint8_t reason)
{
    if (bt_mesh_prov_link.cur_pdu_type == PROV_DATA) {
        /* feature: for provisioner resend prov_data
         * check if device have provisioned, expect pdu is 0xFF
         * ignore error PROV_ERR_UNEXP_PDU, send complete message to provisioner
         */
        if (reason == PROV_ERR_UNEXP_PDU && bt_mesh_is_provisioned() && bt_mesh_prov_active()) {
            BT_WARN("recv repeat PROV_DATA, device have provisioned");
            prov_send_complete_msg();
            return;
        }
    }

    BT_ERR("reason: 0x%02x", reason);
    /* According to Bluetooth Mesh Specification v1.0.1, Section 5.4.4, the
     * provisioner just closes the link when something fails, while the
     * provisionee sends the fail message, and waits for the provisioner to
     * close the link.
     */
    prov_send_fail_msg(reason);

    if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags, REPROVISION)) {
        reprovision_fail();
    }
}

static void prov_invite(const uint8_t *data)
{
	struct os_mbuf *buf = PROV_BUF(PDU_LEN_CAPABILITIES);

	BT_DBG("Attention Duration: %u seconds", data[0]);

	if (data[0]) {
		bt_mesh_attention(NULL, data[0]);
	}

	memcpy(bt_mesh_prov_link.conf_inputs.invite, data, PDU_LEN_INVITE);

	bt_mesh_prov_buf_init(buf, PROV_CAPABILITIES);

	/* Number of Elements supported */
	net_buf_simple_add_u8(buf, bt_mesh_elem_count());

	/* Supported algorithms - FIPS P-256 Eliptic Curve */
	uint16_t auth_support = BIT(BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM);
	if (MYNEWT_VAL(BT_MESH_PROV_AUTH_CMAC_AES_256)) {
		auth_support |= BIT(BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM);
	}
    net_buf_simple_add_be16(buf, auth_support);

	/* Public Key Type */
	net_buf_simple_add_u8(buf,
			      bt_mesh_prov->public_key_be == NULL ? PUB_KEY_NO_OOB : PUB_KEY_OOB);

	/* Static OOB Type */
	net_buf_simple_add_u8(buf, bt_mesh_prov->static_val ? BIT(0) : 0x00);

	/* Output OOB Size */
	net_buf_simple_add_u8(buf, bt_mesh_prov->output_size);

	/* Output OOB Action */
	net_buf_simple_add_be16(buf, bt_mesh_prov->output_actions);

	/* Input OOB Size */
	net_buf_simple_add_u8(buf, bt_mesh_prov->input_size);

	/* Input OOB Action */
	net_buf_simple_add_be16(buf, bt_mesh_prov->input_actions);

	memcpy(bt_mesh_prov_link.conf_inputs.capabilities, &buf->om_data[1], PDU_LEN_CAPABILITIES);

	if (bt_mesh_prov_send(buf, NULL)) {
		BT_ERR("Failed to send capabilities");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_START;
done:
	os_mbuf_free_chain(buf);
}

static void prov_start(const uint8_t *data)
{
	BT_DBG("Algorithm:   0x%02x", data[0]);
	BT_DBG("Public Key:  0x%02x", data[1]);
	BT_DBG("Auth Method: 0x%02x", data[2]);
	BT_DBG("Auth Action: 0x%02x", data[3]);
	BT_DBG("Auth Size:   0x%02x", data[4]);

    if (data[0] == BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM || data[0] == BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM) {
        bt_mesh_prov_link.algorithm = data[0];
    } else {
        BT_ERR("Unknown algorithm 0x%02x", data[0]);
        prov_fail(PROV_ERR_NVAL_FMT);
        return;
    }

    uint8_t auth_size = bt_mesh_prov_auth_size_get();

	if (data[1] > PUB_KEY_OOB ||
	    (data[1] == PUB_KEY_OOB &&
	    (!MYNEWT_VAL(BLE_MESH_PROV_OOB_PUBLIC_KEY) || !bt_mesh_prov->public_key_be))) {
		BT_ERR("Invalid public key type: 0x%02x", data[1]);
		prov_fail(PROV_ERR_NVAL_FMT);
		return;
	}

	atomic_set_bit_to(bt_mesh_prov_link.flags, OOB_PUB_KEY, data[1] == PUB_KEY_OOB);

	memcpy(bt_mesh_prov_link.conf_inputs.start, data, PDU_LEN_START);

	bt_mesh_prov_link.expect = PROV_PUB_KEY;
	bt_mesh_prov_link.oob_method = data[2];
	bt_mesh_prov_link.oob_action = data[3];
	bt_mesh_prov_link.oob_size = data[4];

	if (bt_mesh_prov_auth(false, data[2], data[3], data[4]) < 0) {
		BT_ERR("Invalid authentication method: 0x%02x; "
		       "action: 0x%02x; size: 0x%02x", data[2], data[3],
		       data[4]);
		prov_fail(PROV_ERR_NVAL_FMT);
	}

	if (atomic_test_bit(bt_mesh_prov_link.flags, OOB_STATIC_KEY)) {
		/* Trim the Auth if it is longer than required length */
        memcpy(bt_mesh_prov_link.auth, bt_mesh_prov->static_val,
		       bt_mesh_prov->static_val_len > auth_size ? auth_size
								: bt_mesh_prov->static_val_len);
 
		/* Pad with zeros if the Auth is shorter the required length */
        if (bt_mesh_prov->static_val_len < auth_size) {
            memset(bt_mesh_prov_link.auth + bt_mesh_prov->static_val_len, 0,
			       auth_size - bt_mesh_prov->static_val_len);
        }
        BT_DBG("AuthValue  %s", bt_hex(bt_mesh_prov_link.auth, PROV_AUTH_MAX_LEN));
    }
}

static void send_confirm(void)
{
	struct os_mbuf *cfm = PROV_BUF(PDU_LEN_CONFIRM);
	uint8_t auth_size = bt_mesh_prov_auth_size_get();
	uint8_t *inputs = (uint8_t *)&bt_mesh_prov_link.conf_inputs;
	uint8_t conf_key_input[64] = {0};
 
	BT_DBG("ConfInputs[0]   %s", bt_hex(inputs, 32));
	BT_DBG("ConfInputs[32]  %s", bt_hex(&inputs[32], 32));
	BT_DBG("ConfInputs[64]  %s", bt_hex(&inputs[64], 32));
	BT_DBG("ConfInputs[96]  %s", bt_hex(&inputs[96], 32));
	BT_DBG("ConfInputs[128] %s", bt_hex(&inputs[128], 17));
 
	if (bt_mesh_prov_conf_salt(bt_mesh_prov_link.algorithm, inputs, 
			bt_mesh_prov_link.conf_salt)) {
		BT_ERR("Unable to generate confirmation salt");
		prov_fail(PROV_ERR_UNEXP_ERR);
		goto done;
	}
 
	BT_DBG("ConfirmationSalt: %s", bt_hex(bt_mesh_prov_link.conf_salt, auth_size));
 
	memcpy(conf_key_input, bt_mesh_prov_link.dhkey, BT_DH_KEY_LEN);
 
	if (bt_mesh_prov_link.algorithm == BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM) {
		memcpy(&conf_key_input[BT_DH_KEY_LEN], bt_mesh_prov_link.auth, PROV_AUTH_MAX_LEN);
		BT_DBG("AuthValue  %s", bt_hex(bt_mesh_prov_link.auth, PROV_AUTH_MAX_LEN));
	}
 
	if (bt_mesh_prov_conf_key(bt_mesh_prov_link.algorithm, conf_key_input,
			bt_mesh_prov_link.conf_salt, bt_mesh_prov_link.conf_key)) {
		BT_ERR("Unable to generate confirmation key");
		prov_fail(PROV_ERR_UNEXP_ERR);
		goto done;
	}
 
	BT_DBG("ConfirmationKey: %s", bt_hex(bt_mesh_prov_link.conf_key, auth_size));
 
	if (bt_rand(bt_mesh_prov_link.rand, auth_size)) {
		BT_ERR("Unable to generate random number");
		prov_fail(PROV_ERR_UNEXP_ERR);
		goto done;
	}
 
	BT_DBG("LocalRandom: %s", bt_hex(bt_mesh_prov_link.rand, auth_size));
 
	bt_mesh_prov_buf_init(cfm, PROV_CONFIRM);
 
	if (bt_mesh_prov_conf(bt_mesh_prov_link.algorithm, bt_mesh_prov_link.conf_key, 
	bt_mesh_prov_link.rand, bt_mesh_prov_link.auth, net_buf_simple_add(cfm, auth_size))) {
		BT_ERR("Unable to generate confirmation value");
		prov_fail(PROV_ERR_UNEXP_ERR);
		goto done;
	}
 
	if (bt_mesh_prov_send(cfm, NULL)) {
		BT_ERR("Failed to send Provisioning Confirm");
		goto done;
	}
 
	bt_mesh_prov_link.expect = PROV_RANDOM;
 
done:
	os_mbuf_free_chain(cfm);
}

static void send_input_complete(void)
{
	struct os_mbuf *buf = PROV_BUF(PDU_LEN_INPUT_COMPLETE);

	bt_mesh_prov_buf_init(buf, PROV_INPUT_COMPLETE);
	if (bt_mesh_prov_send(buf, NULL)) {
		BT_ERR("Failed to send Provisioning Input Complete");
	}
	bt_mesh_prov_link.expect = PROV_CONFIRM;
}

static void public_key_sent(int err, void *cb_data)
{
	atomic_set_bit(bt_mesh_prov_link.flags, PUB_KEY_SENT);

	if (atomic_test_bit(bt_mesh_prov_link.flags, INPUT_COMPLETE)) {
		send_input_complete();
		return;
	}
}

static void start_auth(void)
{
	if (atomic_test_bit(bt_mesh_prov_link.flags, WAIT_NUMBER) ||
	atomic_test_bit(bt_mesh_prov_link.flags, WAIT_STRING)) {
		bt_mesh_prov_link.expect = PROV_NO_PDU; /* Wait for input */
	} else {
		bt_mesh_prov_link.expect = PROV_CONFIRM;
	}
}

static void send_pub_key(void)
{
	struct os_mbuf *buf = PROV_BUF(PDU_LEN_PUB_KEY);
	const uint8_t *key;
 
	key = bt_pub_key_get();
	if (!key) {
		BT_ERR("No public key available");
		prov_fail(PROV_ERR_UNEXP_ERR);
		goto done;
	}
 
	bt_mesh_prov_buf_init(buf, PROV_PUB_KEY);
 
	/* Swap X and Y halves independently to big-endian */
	net_buf_simple_add_mem(buf, key, PUB_KEY_SIZE);

	BT_DBG("Local Public Key: %s", bt_hex(buf->om_data + 1, PUB_KEY_SIZE));
 
	/* PublicKeyDevice */
	memcpy(bt_mesh_prov_link.conf_inputs.pub_key_device, &buf->om_data[1], PDU_LEN_PUB_KEY);
 
	if (bt_mesh_prov_send(buf, public_key_sent)) {
		BT_ERR("Failed to send Public Key");
		goto done;
	}
 
	start_auth();
done:
	os_mbuf_free_chain(buf);
}

static void prov_dh_key_gen(void)
{
	const uint8_t *remote_pub_key;
	const uint8_t *remote_priv_key;
 
	remote_pub_key = bt_mesh_prov_link.conf_inputs.pub_key_provisioner;
 
	if (MYNEWT_VAL(BLE_MESH_PROV_OOB_PUBLIC_KEY) &&
	    atomic_test_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY)) {
		remote_priv_key = bt_mesh_prov->private_key_be;
	} else {
		remote_priv_key = NULL;
	}
 
	if (bt_dh_key_gen(remote_pub_key, remote_priv_key, bt_mesh_prov_link.dhkey)) {
		BT_ERR("Failed to generate DHKey");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}
 
	BT_DBG("DHkey: %s", bt_hex(bt_mesh_prov_link.dhkey, DH_KEY_SIZE));
 
	if (atomic_test_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY)) {
		start_auth();
	} else {
		send_pub_key();
	}
}

static void prov_dh_key_gen_handler(struct ble_npl_callout *work)
{
	prov_dh_key_gen();
}

static struct ble_npl_callout dh_gen_work;

static void prov_pub_key(const uint8_t *data)
{
	BT_DBG("Remote Public Key: %s", bt_hex(data, BT_PUB_KEY_LEN));
 
	/* PublicKeyProvisioner */
	memcpy(bt_mesh_prov_link.conf_inputs.pub_key_provisioner, data, PDU_LEN_PUB_KEY);
 
	if (MYNEWT_VAL(BLE_MESH_PROV_OOB_PUBLIC_KEY) &&
	    atomic_test_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY)) {
		if (!bt_mesh_prov->public_key_be || !bt_mesh_prov->private_key_be) {
			BT_ERR("Public or private key is not ready");
			prov_fail(PROV_ERR_UNEXP_ERR);
			return;
		}
 
		if (!memcmp(bt_mesh_prov->public_key_be,
			    bt_mesh_prov_link.conf_inputs.pub_key_provisioner, PDU_LEN_PUB_KEY)) {
			BT_ERR("Public keys are identical");
			prov_fail(PROV_ERR_NVAL_FMT);
			return;
		}
 
		/* No swap needed since user provides public key in big-endian */
		memcpy(bt_mesh_prov_link.conf_inputs.pub_key_device, bt_mesh_prov->public_key_be,
		       PDU_LEN_PUB_KEY);
	}

    prov_dh_key_gen();
}

static void notify_input_complete(void)
{
	if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags,
				      NOTIFY_INPUT_COMPLETE) &&
	    bt_mesh_prov->input_complete) {
		bt_mesh_prov->input_complete();
	}
}

static void send_random(void)
{
	struct os_mbuf *rnd = PROV_BUF(PDU_LEN_RANDOM);

	bt_mesh_prov_buf_init(rnd, PROV_RANDOM);
    net_buf_simple_add_mem(rnd, bt_mesh_prov_link.rand, bt_mesh_prov_auth_size_get());

	if (bt_mesh_prov_send(rnd, NULL)) {
		BT_ERR("Failed to send Provisioning Random");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_DATA;
done:
	os_mbuf_free_chain(rnd);
}

static void prov_random(const uint8_t *data)
{
	uint8_t rand_size = bt_mesh_prov_auth_size_get();
	uint8_t conf_verify[PROV_AUTH_MAX_LEN];
 
	BT_DBG("Remote Random: %s", bt_hex(data, rand_size));
	if (!memcmp(data, bt_mesh_prov_link.rand, rand_size)) {
		BT_ERR("Random value is identical to ours, rejecting.");
		prov_fail(PROV_ERR_CFM_FAILED);
		return;
	}
 
	if (bt_mesh_prov_conf(bt_mesh_prov_link.algorithm, bt_mesh_prov_link.conf_key, 
	data, bt_mesh_prov_link.auth, conf_verify)) {
		BT_ERR("Unable to calculate confirmation verification");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}
 
	if (memcmp(conf_verify, bt_mesh_prov_link.conf, rand_size)) {
		BT_ERR("Invalid confirmation value");
		BT_DBG("Received:   %s", bt_hex(bt_mesh_prov_link.conf, rand_size));
		BT_DBG("Calculated: %s",  bt_hex(conf_verify, rand_size));
		prov_fail(PROV_ERR_CFM_FAILED);
		return;
	}
	BT_DBG("Received:   %s", bt_hex(bt_mesh_prov_link.conf, rand_size));
	BT_DBG("Calculated: %s",  bt_hex(conf_verify, rand_size));
	if (bt_mesh_prov_salt(bt_mesh_prov_link.algorithm, bt_mesh_prov_link.conf_salt,
		data, bt_mesh_prov_link.rand, bt_mesh_prov_link.prov_salt)) {
		BT_ERR("Failed to generate provisioning salt");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}
 
	BT_DBG("ProvisioningSalt: %s", bt_hex(bt_mesh_prov_link.prov_salt, 16));
 
	send_random();
}

static void prov_confirm(const uint8_t *data)
{
	uint8_t conf_size = bt_mesh_prov_auth_size_get();
 
	BT_DBG("Remote Confirm: %s", bt_hex(data, conf_size));
 
	memcpy(bt_mesh_prov_link.conf, data, conf_size);
 
	notify_input_complete();
 
	send_confirm();
 
}

static inline bool is_pb_gatt(void)
{
	return bt_mesh_prov_link.bearer &&
	       bt_mesh_prov_link.bearer->type == BT_MESH_PROV_GATT;
}

static bool refresh_is_valid(const uint8_t *netkey, uint16_t net_idx, uint32_t iv_index)
{
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
    uint8_t nppi = bt_mesh_node_refresh_get();
    struct bt_mesh_subnet *sub = bt_mesh_subnet_get(net_idx);
    uint16_t old_addr = bt_mesh_primary_addr();
    bool valid_addr = false;

    if (iv_index != bt_mesh.iv_index) {
        BT_ERR("Invalid IV index");
        return false;
    }

    if (!sub || memcmp(netkey, sub->keys[SUBNET_KEY_TX_IDX(sub)].net, 16)) {
        BT_ERR("Invalid netkey");
        return false;
    }

    if (nppi == RMT_PROV_SRV_NODE_ADDR_REF) {
        valid_addr =
            bt_mesh_prov_link.addr < old_addr || bt_mesh_prov_link.addr >= old_addr + bt_mesh_comp_get()->elem_count;
    } else {
        valid_addr = bt_mesh_prov_link.addr == bt_mesh_primary_addr();
    }

    if (!valid_addr) {
        BT_ERR("Invalid address");
    }
	return valid_addr;
#else
	return false;
#endif
}

static void prov_data(const uint8_t *data)
{
	uint8_t session_key[16];
	uint8_t nonce[13];
	uint8_t dev_key[16];
	uint8_t pdu[25];
	uint8_t flags;
	uint32_t iv_index;
	uint16_t addr;
	uint16_t net_idx;
	int err;
	bool identity_enable;

	err = bt_mesh_session_key(bt_mesh_prov_link.dhkey,
				  bt_mesh_prov_link.prov_salt, session_key);
	if (err) {
		BT_ERR("Unable to generate session key");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

	err = bt_mesh_prov_nonce(bt_mesh_prov_link.dhkey,
				 bt_mesh_prov_link.prov_salt, nonce);
	if (err) {
		BT_ERR("Unable to generate session nonce");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	BT_DBG("Nonce: %s", bt_hex(nonce, 13));

	err = bt_mesh_prov_decrypt(session_key, nonce, data, pdu);
	if (err) {
		BT_ERR("Unable to decrypt provisioning data");
		prov_fail(PROV_ERR_DECRYPT);
		return;
	}

	err = bt_mesh_dev_key(bt_mesh_prov_link.dhkey,
			      bt_mesh_prov_link.prov_salt, dev_key);
	if (err) {
		BT_ERR("Unable to generate device key");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	BT_DBG("DevKey: %s", bt_hex(dev_key, 16));

	net_idx = sys_get_be16(&pdu[16]);
	flags = pdu[18];
	iv_index = sys_get_be32(&pdu[19]);
	addr = sys_get_be16(&pdu[23]);
	bt_mesh_prov_link.addr = addr;

	if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION) &&
	    !refresh_is_valid(pdu, net_idx, iv_index)) {
		prov_send_fail_msg(PROV_ERR_INVALID_DATA);
        BT_ERR("prov_send_fail_msg(PROV_ERR_INVALID_DATA)");
		return;
	}

	BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x",
	       net_idx, iv_index, addr);

	if (prov_send_complete_msg() != 0) {
		BT_ERR("prov_send_complete_msg failed");
		return;
	}

	/* Ignore any further PDUs on this link */
	bt_mesh_prov_link.expect = PROV_NO_PDU;
	atomic_set_bit(bt_mesh_prov_link.flags, COMPLETE);

	/* 远程配网服务器 nppi refresh*/
    if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION)) {
        bt_mesh_dev_key_cand(dev_key, sizeof(dev_key));
        return;
    }

	/* Store info, since bt_mesh_provision() will end up clearing it */
	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
		identity_enable = is_pb_gatt();
	} else {
		identity_enable = false;
	}

	err = bt_mesh_provision(pdu, net_idx, flags, iv_index, addr, dev_key);
	if (err) {
		BT_ERR("Failed to provision (err %d)", err);
		return;
	}

	/* After PB-GATT provisioning we should start advertising
	 * using Node Identity.
	 */
	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) && identity_enable) {
		bt_mesh_proxy_identity_enable();
	}
}

static void local_input_complete(void)
{
	if (atomic_test_bit(bt_mesh_prov_link.flags, PUB_KEY_SENT) ||
	    atomic_test_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY)) {
		send_input_complete();
	} else {
		atomic_set_bit(bt_mesh_prov_link.flags, INPUT_COMPLETE);
	}
}

static void reprovision_complete(void)
{
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
    bt_mesh_reprovision(bt_mesh_prov_link.addr);

    /* When performing the refresh composition procedure,
     * the device key will be activated after the first
     * successful decryption with the new key.
     */
    if (bt_mesh_node_refresh_get() == RMT_PROV_SRV_NODE_ADDR_REF) {
        bt_mesh_dev_key_cand_activate();
    }

    if (bt_mesh_prov->reprovisioned) {
        bt_mesh_prov->reprovisioned(bt_mesh_primary_addr());
    }
#endif
}

static void reprovision_fail(void)
{
    bt_mesh_dev_key_cand_remove();
}

static void prov_link_closed(void)
{
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
    if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION)) {
        if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags, COMPLETE) &&
            bt_mesh_link_close_reason_get() == RMT_PROV_SRV_LINK_CLOSE_SUC) {
            reprovision_complete();
        } else {
            reprovision_fail();
        }
    }
#endif
	reset_state();
}

static void prov_link_opened(void)
{
	bt_mesh_prov_link.expect = PROV_INVITE;
	bt_mesh_prov_link.cur_pdu_type = PROV_NO_PDU;
    if (bt_mesh_is_provisioned()) {
        atomic_set_bit(bt_mesh_prov_link.flags, REPROVISION);
    } else {
        atomic_clear_bit(bt_mesh_prov_link.flags, REPROVISION);
    }
}

static const struct bt_mesh_prov_role role_device = {
	.input_complete = local_input_complete,
	.link_opened = prov_link_opened,
	.link_closed = prov_link_closed,
	.error = prov_fail,
	.op = {
		[PROV_INVITE] = prov_invite,
		[PROV_START] = prov_start,
		[PROV_PUB_KEY] = prov_pub_key,
		[PROV_CONFIRM] = prov_confirm,
		[PROV_RANDOM] = prov_random,
		[PROV_DATA] = prov_data,
	},
};

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
	BT_DBG("bt_mesh_prov_enable");
#ifdef CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER
	if ((!IS_ENABLED(BLE_MESH_REMOTE_PROVISION)) &&
		(bearers & BT_MESH_RMT_PROV)) {
		rmt_prov_srv_bearer.link_accept(bt_mesh_prov_bearer_cb_get(), NULL);
	}
#endif
	
	if (bt_mesh_is_provisioned()) {
		return -EALREADY;
	}
	
	// defalut enable unprov beacon send
	bt_mesh_beacon_set_unprov_beacon_flag(true);

	if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
	    (bearers & BT_MESH_PROV_ADV)) {
		pb_adv.link_accept(bt_mesh_prov_bearer_cb_get(), NULL);
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
	    (bearers & BT_MESH_PROV_GATT)) {
		pb_gatt.link_accept(bt_mesh_prov_bearer_cb_get(), NULL);
	}

	BT_DBG("bt_mesh_prov_link.role = &role_device");
	bt_mesh_prov_link.role = &role_device;

	return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
	if (bt_mesh_is_provisioned()) {
		return -EALREADY;
	}

	if (bt_mesh_prov_active()) {
		return -EBUSY;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
	    (bearers & BT_MESH_PROV_ADV)) {
		bt_mesh_beacon_disable();
		bt_mesh_scan_disable();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
	    (bearers & BT_MESH_PROV_GATT)) {
		(void)bt_mesh_pb_gatt_disable();
	}

	return 0;
}
