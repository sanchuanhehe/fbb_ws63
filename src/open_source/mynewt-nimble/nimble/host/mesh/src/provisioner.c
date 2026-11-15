/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2020 Lingao Meng
 * Copyright (c) 2021 Manulytica Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BLE_NPL_LOG_MODULE BLE_MESH_PROV_LOG
#include <nimble/nimble_npl_log.h>

#include "testing.h"

#include "mesh/glue.h"
#include "atomic_adapter.h"
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
#include "settings.h"
#include "remote_prov_client.h"

static struct {
	struct bt_mesh_cdb_node *node;
	uint16_t addr;
	uint16_t net_idx;
	uint8_t attention_duration;
	uint8_t uuid[16];
	uint8_t new_device_key[DEVICE_KEY_LEN];
	uint16_t rmt_addr;
	struct k_work_delayable complete_timer;
} prov_device;

static void send_pub_key(void);
static void prov_dh_key_gen(void);

static int reset_state(void)
{
#if MYNEWT_VAL(BLE_MESH_CDB)
	if (prov_device.node != NULL) {
        if (!atomic_test_and_clear_bit(bt_mesh_prov_link.flags, REPROVISION) ||
            prov_device.addr != prov_device.rmt_addr) {
            bt_mesh_cdb_node_del(prov_device.node, false);
        }
	}
#endif
	return bt_mesh_prov_reset_state();
}

static void prov_link_close(enum prov_bearer_link_status status)
{
	BT_DBG("%u", status);
	bt_mesh_prov_link.expect = PROV_NO_PDU;
	if (bt_mesh_prov_link.bearer != NULL) {
		bt_mesh_prov_link.bearer->link_close(status);
	}
}

static void prov_fail(uint8_t reason)
{
	BT_DBG("0x%0x", reason);
	/* According to Bluetooth Mesh Specification v1.0.1, Section 5.4.4, the
	 * provisioner just closes the link when something fails, while the
	 * provisionee sends the fail message, and waits for the provisioner to
	 * close the link.
	 */
	prov_link_close(PROV_BEARER_LINK_STATUS_FAIL);
}

static void send_invite(void)
{
	struct os_mbuf *inv = PROV_BUF(PDU_LEN_INVITE);

	BT_DBG("");

	bt_mesh_prov_buf_init(inv, PROV_INVITE);
	net_buf_simple_add_u8(inv, prov_device.attention_duration);

	memcpy(bt_mesh_prov_link.conf_inputs.invite, &prov_device.attention_duration,
	       PDU_LEN_INVITE);

	if (bt_mesh_prov_send(inv, NULL)) {
		BT_ERR("Failed to send invite");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_CAPABILITIES;
done:
	os_mbuf_free_chain(inv);
}

static void start_sent(int err, void *cb_data)
{
	if (!bt_pub_key_get()) {
		atomic_set_bit(bt_mesh_prov_link.flags, WAIT_PUB_KEY);
		BT_WARN("Waiting for local public key");
	} else {
		send_pub_key();
	}
}

static void send_start(void)
{
	BT_DBG("");
	struct os_mbuf *start = PROV_BUF(PDU_LEN_START);

	bool oob_pub_key = bt_mesh_prov_link.conf_inputs.capabilities[3] == PUB_KEY_OOB;

	bt_mesh_prov_buf_init(start, PROV_START);
    net_buf_simple_add_u8(start, bt_mesh_prov_link.algorithm);

	if (atomic_test_bit(bt_mesh_prov_link.flags, REMOTE_PUB_KEY) && oob_pub_key) {
		net_buf_simple_add_u8(start, PUB_KEY_OOB);
		atomic_set_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY);
	} else {
		net_buf_simple_add_u8(start, PUB_KEY_NO_OOB);
	}

	net_buf_simple_add_u8(start, bt_mesh_prov_link.oob_method);

	net_buf_simple_add_u8(start, bt_mesh_prov_link.oob_action);

	net_buf_simple_add_u8(start, bt_mesh_prov_link.oob_size);

    memcpy(bt_mesh_prov_link.conf_inputs.start, &start->om_data[1], PDU_LEN_START);

	if (bt_mesh_prov_auth(true, bt_mesh_prov_link.oob_method,
			      bt_mesh_prov_link.oob_action, bt_mesh_prov_link.oob_size) < 0) {
		BT_ERR("Invalid authentication method: 0x%02x; "
		       "action: 0x%02x; size: 0x%02x", bt_mesh_prov_link.oob_method,
		       bt_mesh_prov_link.oob_action, bt_mesh_prov_link.oob_size);
		goto done;
	}

	if (bt_mesh_prov_send(start, start_sent)) {
		BT_ERR("Failed to send Provisioning Start");
		goto done;
	}
done:
	os_mbuf_free_chain(start);
}

static bool prov_check_method(struct bt_mesh_dev_capabilities *caps)
{
	if (bt_mesh_prov_link.oob_method == AUTH_METHOD_STATIC) {
		if (!caps->static_oob) {
			BT_WARN("Device not support OOB static authentication provisioning");
			return false;
		}
	} else if (bt_mesh_prov_link.oob_method == AUTH_METHOD_INPUT) {
		if (bt_mesh_prov_link.oob_size > caps->input_size) {
			BT_WARN("The required input length (0x%02x) "
				"exceeds the device capacity (0x%02x)",
				bt_mesh_prov_link.oob_size, caps->input_size);
			return false;
		}

		if (!(BIT(bt_mesh_prov_link.oob_action) & caps->input_actions)) {
			BT_WARN("The required input action (0x%04x) "
				"not supported by the device (0x%02x)",
				(uint16_t)BIT(bt_mesh_prov_link.oob_action), caps->input_actions);
			return false;
		}

		if (bt_mesh_prov_link.oob_action == INPUT_OOB_STRING) {
			if (!bt_mesh_prov->output_string) {
				BT_WARN("Not support output string");
				return false;
			}
		} else {
			if (!bt_mesh_prov->output_number) {
				BT_WARN("Not support output number");
				return false;
			}
		}
	} else if (bt_mesh_prov_link.oob_method == AUTH_METHOD_OUTPUT) {
		if (bt_mesh_prov_link.oob_size > caps->output_size) {
			BT_WARN("The required output length (0x%02x) "
				"exceeds the device capacity (0x%02x)",
				bt_mesh_prov_link.oob_size, caps->output_size);
			return false;
		}

		if (!(BIT(bt_mesh_prov_link.oob_action) & caps->output_actions)) {
			BT_WARN("The required output action (0x%04x) "
				"not supported by the device (0x%02x)",
				(uint16_t)BIT(bt_mesh_prov_link.oob_action), caps->output_actions);
			return false;
		}

		if (!bt_mesh_prov->input) {
			BT_WARN("Not support input");
			return false;
		}
	}

	return true;
}

static void prov_capabilities(const uint8_t *data)
{
	struct bt_mesh_dev_capabilities caps;

	BT_DBG("Elements:          %u", data[0]);

	caps.algorithms = sys_get_be16(&data[1]);
	BT_DBG("Algorithms:        0x%02x", caps.algorithms);

    bool is_aes128 = false;
    bool is_sha256 = false;
 
    if ((caps.algorithms & BIT(BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM))) {
        is_aes128 = true;
    }

    if ((caps.algorithms & BIT(BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM))) {
        is_sha256 = true;
    }

    if (!(is_sha256 || is_aes128)) {
        BT_ERR("Invalid encryption algorithm");
        prov_fail(PROV_ERR_NVAL_FMT);
        return;
    }

	caps.pub_key_type = data[3];
	caps.static_oob = data[4];
	caps.output_size = data[5];
	BT_DBG("Public Key Type:   0x%02x", caps.pub_key_type);
	BT_DBG("Static OOB Type:   0x%02x", caps.static_oob);
	BT_DBG("Output OOB Size:   %u", caps.output_size);

	caps.output_actions = (bt_mesh_output_action_t)
		(sys_get_be16(&data[6]));
	caps.input_size = data[8];
	caps.input_actions = (bt_mesh_input_action_t)
		(sys_get_be16(&data[9]));
	BT_DBG("Output OOB Action: 0x%04x", caps.output_actions);
	BT_DBG("Input OOB Size:    %u", caps.input_size);
	BT_DBG("Input OOB Action:  0x%04x", caps.input_actions);

    if (data[0] == 0) {
		BT_ERR("Invalid number of elements");
		prov_fail(PROV_ERR_NVAL_FMT);
		return;
	}

    if (caps.static_oob & BT_MESH_OOB_AUTH_REQUIRED) {
        bool oob_availability = caps.output_size > 0 || caps.input_size > 0 ||
            (caps.static_oob & BT_MESH_STATIC_OOB_AVAILABLE);

        if (!oob_availability && !is_sha256) {
            BT_ERR("Invalid capabilities for OOB authentication");
            prov_fail(PROV_ERR_NVAL_FMT);
            return;
        }
    }

    bt_mesh_prov_link.algorithm = is_sha256 ? BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM :
            BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM;

#if MYNEWT_VAL(BLE_MESH_CDB)
    if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION) && prov_device.rmt_addr == prov_device.addr) {
        prov_device.node = bt_mesh_cdb_node_get(prov_device.addr);
        if (prov_device.node == NULL) {
            BT_ERR("Failed get reprovision node 0x%04x", prov_device.addr);
            prov_fail(PROV_ERR_RESOURCES);
            return;
        }
    } else {
        prov_device.node = bt_mesh_cdb_node_alloc(prov_device.uuid, prov_device.addr, data[0], prov_device.net_idx);
        if (prov_device.node == NULL) {
            BT_ERR("Failed allocating node 0x%04x", prov_device.addr);
            prov_fail(PROV_ERR_RESOURCES);
            return;
        }
    }
#endif
	memcpy(bt_mesh_prov_link.conf_inputs.capabilities, data, PDU_LEN_CAPABILITIES);

	if (bt_mesh_prov->capabilities) {
		bt_mesh_prov->capabilities(&caps);
	}

	if (!prov_check_method(&caps)) {
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	send_start();
}

static void send_confirm(void)
{
    uint8_t auth_size = bt_mesh_prov_auth_size_get();
    uint8_t *inputs = (uint8_t *)&bt_mesh_prov_link.conf_inputs;
    uint8_t conf_key_input[64];
 
    BT_DBG("ConfInputs[0]   %s", bt_hex(inputs, 32));
    BT_DBG("ConfInputs[32]  %s", bt_hex(&inputs[32], 32));
    BT_DBG("ConfInputs[64]  %s", bt_hex(&inputs[64], 32));
    BT_DBG("ConfInputs[96]  %s", bt_hex(&inputs[96], 32));
    BT_DBG("ConfInputs[128] %s", bt_hex(&inputs[128], 17));
 
    if (bt_mesh_prov_conf_salt(bt_mesh_prov_link.algorithm, 
                        inputs, 
                        bt_mesh_prov_link.conf_salt)) {
        BT_ERR("Unable to generate confirmation salt");
        prov_fail(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(bt_mesh_prov_link.conf_salt, auth_size));
    memcpy(conf_key_input, bt_mesh_prov_link.dhkey, 32);

    if (bt_mesh_prov_link.algorithm == BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM) {
        memcpy(&conf_key_input[32], bt_mesh_prov_link.auth, 32);
        BT_DBG("AuthValue  %s", bt_hex(bt_mesh_prov_link.auth, 32));
    }

    if (bt_mesh_prov_conf_key(bt_mesh_prov_link.algorithm, conf_key_input,
                    bt_mesh_prov_link.conf_salt, bt_mesh_prov_link.conf_key)) {
        BT_ERR("Unable to generate confirmation key");
        prov_fail(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(bt_mesh_prov_link.conf_key, auth_size));
 
    if (bt_rand(bt_mesh_prov_link.rand, auth_size)) {
        BT_ERR("Unable to generate random number");
        prov_fail(PROV_ERR_UNEXP_ERR);
        return;
    }
    BT_DBG("LocalRandom: %s", bt_hex(bt_mesh_prov_link.rand, auth_size));

    struct os_mbuf *cfm = PROV_BUF(PDU_LEN_CONFIRM);
    bt_mesh_prov_buf_init(cfm, PROV_CONFIRM);
 
    if (bt_mesh_prov_conf(bt_mesh_prov_link.algorithm, bt_mesh_prov_link.conf_key,
                bt_mesh_prov_link.rand, bt_mesh_prov_link.auth,
                bt_mesh_prov_link.conf)) {
        BT_ERR("Unable to generate confirmation value");
        goto done;
    }

    net_buf_simple_add_mem(cfm, bt_mesh_prov_link.conf, auth_size);

    if (bt_mesh_prov_send(cfm, NULL)) {
        BT_ERR("Failed to send Provisioning Confirm");
        goto done;
    }

    bt_mesh_prov_link.expect = PROV_CONFIRM;
done:
    os_mbuf_free_chain(cfm);
}

static void public_key_sent(int err, void *cb_data)
{
	unused(err);
	unused(cb_data);
	atomic_set_bit(bt_mesh_prov_link.flags, PUB_KEY_SENT);

	if (atomic_test_bit(bt_mesh_prov_link.flags, OOB_PUB_KEY) &&
	    atomic_test_bit(bt_mesh_prov_link.flags, REMOTE_PUB_KEY)) {
		prov_dh_key_gen();
		return;
	}
}

static void send_pub_key(void)
{
	const uint8_t *key;

	key = bt_pub_key_get();
	if (!key) {
		BT_ERR("No public key available");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	struct os_mbuf *buf = PROV_BUF(PDU_LEN_PUB_KEY);
	bt_mesh_prov_buf_init(buf, PROV_PUB_KEY);

    net_buf_simple_add_mem(buf, key, PUB_KEY_SIZE);
    BT_DBG("Local Public Key: %s", bt_hex(buf->om_data + 1, PUB_KEY_SIZE));

	/* PublicKeyProvisioner */
	memcpy(bt_mesh_prov_link.conf_inputs.pub_key_provisioner, &buf->om_data[1], PDU_LEN_PUB_KEY);

	if (bt_mesh_prov_send(buf, public_key_sent)) {
		BT_ERR("Failed to send Public Key");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_PUB_KEY;
done:
	os_mbuf_free_chain(buf);
}

static void prov_dh_key_cb(const uint8_t dhkey[BT_DH_KEY_LEN])
{
	BT_DBG("%p", dhkey);

	if (!dhkey) {
		BT_ERR("DHKey generation failed");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	sys_memcpy_swap(bt_mesh_prov_link.dhkey, dhkey, BT_DH_KEY_LEN);

	BT_DBG("DHkey: %s", bt_hex(bt_mesh_prov_link.dhkey, BT_DH_KEY_LEN));

	if (atomic_test_bit(bt_mesh_prov_link.flags, WAIT_STRING) ||
	    atomic_test_bit(bt_mesh_prov_link.flags, WAIT_NUMBER) ||
	    atomic_test_bit(bt_mesh_prov_link.flags, NOTIFY_INPUT_COMPLETE)) {
		atomic_set_bit(bt_mesh_prov_link.flags, WAIT_CONFIRM);
		return;
	}

	send_confirm();
}

static void prov_dh_key_gen(void)
{
	uint8_t remote_pk_le[BT_PUB_KEY_LEN];
	const uint8_t *remote_pk;
	const uint8_t *local_pk;

	local_pk = bt_mesh_prov_link.conf_inputs.pub_key_provisioner;
	remote_pk = bt_mesh_prov_link.conf_inputs.pub_key_device;

	if (!memcmp(local_pk, remote_pk, PUB_KEY_SIZE)) {
		BT_ERR("Public keys are identical");
		prov_fail(PROV_ERR_NVAL_FMT);
		return;
	}

	if (bt_dh_key_gen(remote_pk, NULL, bt_mesh_prov_link.dhkey)) {
		BT_ERR("Failed to generate DHKey");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}
    BT_DBG("DHkey: %s", bt_hex(bt_mesh_prov_link.dhkey, DH_KEY_SIZE));

	if (atomic_test_bit(bt_mesh_prov_link.flags, NOTIFY_INPUT_COMPLETE)) {
		bt_mesh_prov_link.expect = PROV_INPUT_COMPLETE;
	}

    if (atomic_test_bit(bt_mesh_prov_link.flags, WAIT_STRING) ||
        atomic_test_bit(bt_mesh_prov_link.flags, WAIT_NUMBER) ||
        atomic_test_bit(bt_mesh_prov_link.flags, NOTIFY_INPUT_COMPLETE)) {
        atomic_set_bit(bt_mesh_prov_link.flags, WAIT_CONFIRM);
        return;
    }

    send_confirm();
}

static struct ble_npl_callout dh_gen_work;

static void prov_pub_key(const uint8_t *data)
{
	BT_DBG("Remote Public Key: %s", bt_hex(data, PUB_KEY_SIZE));

	atomic_set_bit(bt_mesh_prov_link.flags, REMOTE_PUB_KEY);

	/* PublicKeyDevice */
	memcpy(bt_mesh_prov_link.conf_inputs.pub_key_device, data, PUB_KEY_SIZE);
	if (bt_mesh_prov_link.bearer != NULL) {
		bt_mesh_prov_link.bearer->clear_tx();
	}

	prov_dh_key_gen();
}

static void pub_key_ready(const uint8_t *pkey)
{
	if (!pkey) {
		BT_WARN("Public key not available");
		return;
	}

	BT_DBG("Local public key ready");

	if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags, WAIT_PUB_KEY)) {
		send_pub_key();
	}
}

static void notify_input_complete(void)
{
	if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags,
				      NOTIFY_INPUT_COMPLETE) &&
	    bt_mesh_prov->input_complete) {
		bt_mesh_prov->input_complete();
	}
}

static void prov_input_complete(const uint8_t *data)
{
	unused(data);
	BT_DBG("prov_input_complete");

	notify_input_complete();

	if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags, WAIT_CONFIRM)) {
		send_confirm();
	}
}

static void send_prov_data(void)
{
	struct bt_mesh_subnet *sub;
	uint8_t session_key[16];
	uint8_t nonce[13];
	int err;

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
	err = bt_mesh_dev_key(bt_mesh_prov_link.dhkey,
			      bt_mesh_prov_link.prov_salt, prov_device.new_device_key);
	if (err) {
		BT_ERR("Unable to generate device key");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

#if MYNEWT_VAL(BLE_MESH_CDB)
	sub = bt_mesh_subnet_get(prov_device.node->net_idx);
#else
	sub = bt_mesh_subnet_get(prov_device.net_idx);
#endif
	if (sub == NULL) {
		BT_ERR("No subnet with net_idx %u",
		       prov_device.node->net_idx);
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	struct os_mbuf *pdu = PROV_BUF(PDU_LEN_DATA + 1);
	bt_mesh_prov_buf_init(pdu, PROV_DATA);
#if MYNEWT_VAL(BLE_MESH_CDB)
	net_buf_simple_add_mem(pdu, sub->keys[SUBNET_KEY_TX_IDX(sub)].net, 16);
	net_buf_simple_add_be16(pdu, prov_device.node->net_idx);
	net_buf_simple_add_u8(pdu, bt_mesh_cdb_subnet_flags(sub));
	net_buf_simple_add_be32(pdu, bt_mesh_cdb.iv_index);

	net_buf_simple_add_be16(pdu, prov_device.node->addr);

#else
	net_buf_simple_add_mem(pdu, sub->keys[SUBNET_KEY_TX_IDX(sub)].net, 16);
	net_buf_simple_add_be16(pdu, prov_device.net_idx);
	net_buf_simple_add_u8(pdu, *(uint8_t*)bt_mesh.flags);
	net_buf_simple_add_be32(pdu, bt_mesh.iv_index);
	net_buf_simple_add_be16(pdu, prov_device.addr);
#endif
    BT_DBG("net_idx %u, iv_index 0x%08x, addr 0x%04x",
            prov_device.net_idx, bt_mesh.iv_index,
            prov_device.addr);

	net_buf_simple_add(pdu, 8); /* For MIC */

	err = bt_mesh_prov_encrypt(session_key, nonce, &pdu->om_data[1],
				   &pdu->om_data[1]);
	if (err) {
		BT_ERR("Unable to encrypt provisioning data");
		prov_fail(PROV_ERR_DECRYPT);
		goto done;
	}

	if (bt_mesh_prov_send(pdu, NULL)) {
		BT_ERR("Failed to send Provisioning Data");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_COMPLETE;
done:
	os_mbuf_free_chain(pdu);
}

static void prov_complete(const uint8_t *data)
{
	unused(data);
	struct bt_mesh_cdb_node *node = prov_device.node;
	uint8_t *dev_key;
#if MYNEWT_VAL(BLE_MESH_CDB)
	dev_key = prov_device.node->dev_key;
#else
	dev_key = bt_mesh.dev_key;
#endif

	(void)memcpy_s(dev_key, DEVICE_KEY_LEN,prov_device.new_device_key, DEVICE_KEY_LEN);

	BT_DBG("DevKey: %s", bt_hex(dev_key, DEVICE_KEY_LEN));
#if MYNEWT_VAL(BLE_MESH_CDB)
	BT_DBG("key %s, net_idx %u, num_elem %u, addr 0x%04x",
	       bt_hex(node->dev_key, 16), node->net_idx, node->num_elem,
	       node->addr);
#else
    BT_DBG("net_idx %u, addr 0x%04x", prov_device.net_idx, prov_device.addr);
#endif

#if MYNEWT_VAL(BLE_MESH_CDB)
	if (IS_ENABLED(CONFIG_BT_SETTINGS) || IS_ENABLED(CONFIG_BT_SETTINGS_EXT)) {
		bt_mesh_cdb_node_store(node);
	}

    if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION) && prov_device.addr != prov_device.rmt_addr) {
        struct bt_mesh_cdb_node *rmt_node = bt_mesh_cdb_node_get(prov_device.addr);
        if (rmt_node != NULL) {
            bt_mesh_cdb_node_del(rmt_node, false);
            prov_device.rmt_addr = 0;
        }
    }
#endif

	prov_device.node = NULL;
	prov_link_close(PROV_BEARER_LINK_STATUS_SUCCESS);

    k_work_schedule(&prov_device.complete_timer, 0);
	if (MYNEWT_VAL(BLE_MESH_CDB) && bt_mesh_prov->node_added) {
		bt_mesh_prov->node_added(node->net_idx, node->uuid, node->addr,
					 node->num_elem);
	}
}

static void send_random(void)
{
	struct os_mbuf *rnd = PROV_BUF(PDU_LEN_RANDOM);
    uint8_t rand_size = bt_mesh_prov_auth_size_get();

	bt_mesh_prov_buf_init(rnd, PROV_RANDOM);
	net_buf_simple_add_mem(rnd, bt_mesh_prov_link.rand, rand_size);
	BT_DBG("send_random: %s",  bt_hex(bt_mesh_prov_link.rand, rand_size));
	if (bt_mesh_prov_send(rnd, NULL)) {
		BT_ERR("Failed to send Provisioning Random");
		goto done;
	}

	bt_mesh_prov_link.expect = PROV_RANDOM;
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

    BT_DBG("algorithm 0x%02x", bt_mesh_prov_link.algorithm);
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
	BT_DBG("Received:   %s", bt_hex(bt_mesh_prov_link.conf, 16));
	BT_DBG("Calculated: %s",  bt_hex(conf_verify, 16));

    if (bt_mesh_prov_salt(bt_mesh_prov_link.algorithm, bt_mesh_prov_link.conf_salt,
			      bt_mesh_prov_link.rand, data, bt_mesh_prov_link.prov_salt)) {
		BT_ERR("Failed to generate provisioning salt");
		prov_fail(PROV_ERR_UNEXP_ERR);
		return;
	}

	BT_DBG("ProvisioningSalt: %s", bt_hex(bt_mesh_prov_link.prov_salt, 16));

	send_prov_data();
}

static void prov_confirm(const uint8_t *data)
{
    uint8_t conf_size = bt_mesh_prov_auth_size_get();

	BT_DBG("Remote Confirm: %s", bt_hex(data, conf_size));

	if (!memcmp(data, bt_mesh_prov_link.conf, conf_size)) {
		BT_ERR("Confirm value is identical to ours, rejecting.");
		prov_fail(PROV_ERR_CFM_FAILED);
		return;
	}

	memcpy(bt_mesh_prov_link.conf, data, conf_size);

	send_random();
}

static void prov_failed(const uint8_t *data)
{
	BT_WARN("Error: 0x%02x", data[0]);
	reset_state();
}

static void local_input_complete(void)
{
	if (atomic_test_and_clear_bit(bt_mesh_prov_link.flags, WAIT_CONFIRM)) {
		send_confirm();
	}
}

static void prov_link_closed(void)
{
	k_work_cancel_delayable(&prov_device.complete_timer);
	reset_state();
}

static void prov_link_opened(void)
{
	send_invite();
}

static const struct bt_mesh_prov_role role_provisioner = {
	.input_complete = local_input_complete,
	.link_opened = prov_link_opened,
	.link_closed = prov_link_closed,
	.error = prov_fail,
	.op = {
		[PROV_CAPABILITIES] = prov_capabilities,
		[PROV_PUB_KEY] = prov_pub_key,
		[PROV_INPUT_COMPLETE] = prov_input_complete,
		[PROV_CONFIRM] = prov_confirm,
		[PROV_RANDOM] = prov_random,
		[PROV_COMPLETE] = prov_complete,
		[PROV_FAILED] = prov_failed,
	},
};

static void prov_set_method(uint8_t method, uint8_t action, uint8_t size)
{
	bt_mesh_prov_link.oob_method = method;
	bt_mesh_prov_link.oob_action = action;
	bt_mesh_prov_link.oob_size = size;
}

int bt_mesh_auth_method_set_input(bt_mesh_input_action_t action, uint8_t size)
{
	if (!action || !size || size > PROV_IO_OOB_SIZE_MAX) {
		return -EINVAL;
	}

	prov_set_method(AUTH_METHOD_INPUT, find_msb_set(action) - 1, size);
	return 0;
}

int bt_mesh_auth_method_set_output(bt_mesh_output_action_t action, uint8_t size)
{
	if (!action || !size || size > PROV_IO_OOB_SIZE_MAX) {
		return -EINVAL;
	}

	prov_set_method(AUTH_METHOD_OUTPUT, find_msb_set(action) - 1, size);
	return 0;
}

int bt_mesh_auth_method_set_static(const uint8_t *static_val, uint8_t size)
{
    if (!size || !static_val) {
        return -EINVAL;
    }

    prov_set_method(AUTH_METHOD_STATIC, 0, 0);

    /* Trim the Auth if it is longer than required length */
    memcpy(bt_mesh_prov_link.auth, static_val,
            size > PROV_AUTH_MAX_LEN ? PROV_AUTH_MAX_LEN : size);
 
    /* Pad with zeros if the Auth is shorter the required length */
    if (size < PROV_AUTH_MAX_LEN) {
        memset(bt_mesh_prov_link.auth + size, 0, PROV_AUTH_MAX_LEN - size);
    }

    return 0;
}

int bt_mesh_auth_method_set_none(void)
{
	prov_set_method(AUTH_METHOD_NO_OOB, 0, 0);
	return 0;
}

int bt_mesh_prov_remote_pub_key_set(const uint8_t public_key[BT_PUB_KEY_LEN])
{
	if (public_key == NULL) {
		return -EINVAL;
	}

	if (atomic_test_and_set_bit(bt_mesh_prov_link.flags, REMOTE_PUB_KEY)) {
		return -EALREADY;
	}

	/* Swap X and Y halves independently to big-endian */
	memcpy(bt_mesh_prov_link.conf_inputs.pub_key_device, public_key, PDU_LEN_PUB_KEY);

	return 0;
}

#if defined(CONFIG_BT_MESH_PB_ADV)
int bt_mesh_pb_adv_open(const uint8_t uuid[16], uint16_t net_idx, uint16_t addr,
			uint8_t attention_duration)
{
	int err;

	// if (atomic_test_and_set_bit(bt_mesh_prov_link.flags, LINK_ACTIVE)) {
	// 	return -EBUSY;
	// }

	atomic_set_bit(bt_mesh_prov_link.flags, PROVISIONER);
	memcpy(prov_device.uuid, uuid, 16);
	prov_device.addr = addr;
	prov_device.net_idx = net_idx;
	prov_device.attention_duration = attention_duration;
	bt_mesh_prov_link.bearer = &pb_adv;
	bt_mesh_prov_link.role = &role_provisioner;

	err = bt_mesh_prov_link.bearer->link_open(prov_device.uuid, PROTOCOL_TIMEOUT,
						  bt_mesh_prov_bearer_cb_get(), NULL);
	// if (err) {
	// 	atomic_clear_bit(bt_mesh_prov_link.flags, LINK_ACTIVE);
	// }

	return err;
}
#endif
static void provisioner_complete(struct ble_npl_event *ev)
{
	atomic_clear_bit(bt_mesh_prov_link.flags, REPROVISION);
	unused(ev);
	BT_ERR("provisioner_complete enter bt_mesh_prov->complete 0x%x \r\n", bt_mesh_prov->complete);
    if (bt_mesh_prov->complete) {
        bt_mesh_prov->complete(prov_device.net_idx, prov_device.addr);
    }
}

int bt_mesh_provisioner_deinit(void)
{
    k_work_cancel_delayable_delete(&prov_device.complete_timer);
    return 0;
}

int bt_mesh_provisioner_enable(uint16_t addr, uint16_t net_idx, uint8_t flags, uint32_t iv_index)
{
    int err;
    uint8_t net_key[16];

    if (atomic_test_and_set_bit(bt_mesh.flags, BT_MESH_VALID)) {
        return -EALREADY;
    }

    bt_rand(net_key, 16);

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);

    bt_mesh.seq = 0;

    bt_mesh_comp_provision(addr);

    bt_mesh_scan_enable();
	BT_DBG("provisioner_complete before");
    k_work_init_delayable(&prov_device.complete_timer, provisioner_complete);
    return err;
}

int bt_mesh_pb_rmt_open(const uint8_t *uuid, uint16_t prov_addr, uint8_t attention_duration,
    const struct prov_bearer *bearer, void *bearer_cb_data)
{
    int err;
    struct bt_mesh_rmt_prov_node_t *ctx = bearer_cb_data;

    if (uuid != NULL) {
        memcpy(prov_device.uuid, uuid, 16);
    } else {
        // nppi流程
        if (ctx->nppi == BT_MESH_RMT_PROV_NODE_REFRESH_DEVKEY) {
            prov_addr = ctx->srv_node->addr;
        }
        prov_device.rmt_addr = ctx->srv_node->addr;
        atomic_set_bit(bt_mesh_prov_link.flags, REPROVISION);
    }

    atomic_set_bit(bt_mesh_prov_link.flags, PROVISIONER);
    prov_device.addr = prov_addr;
    prov_device.net_idx = ctx->srv_node->net_idx;
    prov_device.attention_duration = attention_duration;
    bt_mesh_prov_link.bearer = bearer;
    bt_mesh_prov_link.role = &role_provisioner;
    err = bt_mesh_prov_link.bearer->link_open(
        uuid, LINK_TIMEOUT_SECONDS_DEFAULT, bt_mesh_prov_bearer_cb_get(), bearer_cb_data);

    return err;
}
