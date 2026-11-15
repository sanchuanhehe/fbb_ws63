/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: ble mesh config source.
 *
 * History:
 * 2025-12-6, Create file.
 */
#include <stdbool.h>
#include <stdint.h>

#include "mesh/glue.h"
#include "mesh/access.h"
#include "mesh/cfg.h"
#include "access.h"

#include "bts_def.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_model_inner.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_errcode_inner.h"
#include "ble_mesh_config.h"

#define MAX_TRANSMIT_COUNT 7

typedef struct ble_mesh_mod_id {
    uint16_t mod_id;
    uint16_t company;
    bool is_vnd;
} ble_mesh_mod_id_t;

typedef struct ble_mesh_mod_sub_list_get_para {
    ble_mesh_mod_id_t mod_id;
    bool is_match;
    uint16_t count;
    uint16_t index;
    uint16_t *sub_list;
} ble_mesh_mod_sub_list_get_para_t;

typedef enum {
    ADD_SUB = 0x00,
    DEL_SUB = 0x01,
    OVERWRITE_SUB = 0x02,
} ble_mesh_model_sub_op_t;

typedef struct ble_mesh_model_sub_op_by_elem_para {
    uint16_t elem_addr;
    uint16_t sub_addr;
    ble_mesh_model_sub_op_t op_code;
    int  ret_code;
    bool is_model_match;
} ble_mesh_model_sub_op_by_elem_para_t;

typedef struct ble_mesh_model_sub_op_by_id_para {
    ble_mesh_mod_id_t mod_id;
    uint16_t sub_addr;
    ble_mesh_model_sub_op_t op_code;
    int  ret_code;
    bool is_model_match;
} ble_mesh_model_sub_op_by_id_para_t;

static void ble_mesh_vnd_mod_parse(uint32_t mod_id, ble_mesh_mod_id_t *mod)
{
    /*
        The Vendor Model ID is composed of two fields: a 16-bit Company Identifier [4] assigned by the Bluetooth SIG
        and a 16-bit Vendor Model Identifier assigned by the vendor
     */
    if (mod_id > 0xFFFF) {
        mod->mod_id = mod_id & 0xFFFF;
        mod->company = (mod_id >> 16) & 0xFFFF;  // 16 : 16-bit Vendor Model Identifier assigned by the vendor
        mod->is_vnd = true;
    } else {
        mod->mod_id = mod_id & 0xFFFF;
        mod->is_vnd = false;
    }
    return;
}

static bool ble_mesh_mod_is_exist(struct bt_mesh_model *mod, bool vnd, ble_mesh_mod_id_t *mod_id)
{
    if (mod_id->is_vnd && vnd) {  // vnd mod
        if (mod->vnd.id == mod_id->mod_id && mod->vnd.company == mod_id->company) {
            return true;
        }
        return false;
    }

    if (!mod_id->is_vnd && !vnd) {
        if (mod->id == mod_id->mod_id) {
            return true;
        }
    }

    return false;
}

static void ble_mesh_get_mod_sub_count_iter_cb(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                                               bool vnd, bool primary, void *user_data)
{
    ble_mesh_mod_sub_list_get_para_t *mod_sub_list_para = (ble_mesh_mod_sub_list_get_para_t *)user_data;

    if (ble_mesh_mod_is_exist(mod, vnd, &mod_sub_list_para->mod_id)) {
        goto mod_match;
    }
    return;
mod_match:
    mod_sub_list_para->is_match = true;
    for (uint16_t i = 0; i < ARRAY_SIZE(mod->groups); i++) {
        if (mod->groups[i] == BT_MESH_ADDR_UNASSIGNED) {
            continue;
        }
        mod_sub_list_para->count++;
    }
    return;
}

static void ble_mesh_get_mod_sub_list_iter_cb(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                                              bool vnd, bool primary, void *user_data)
{
    ble_mesh_mod_sub_list_get_para_t *mod_sub_list_para = (ble_mesh_mod_sub_list_get_para_t *)user_data;

    if (ble_mesh_mod_is_exist(mod, vnd, &mod_sub_list_para->mod_id)) {
        goto mod_match;
    }
    return;
mod_match:
    mod_sub_list_para->is_match = true;
    for (uint16_t i = 0, j = mod_sub_list_para->index; i < ARRAY_SIZE(mod->groups) && j < mod_sub_list_para->count; i++) {
        if (mod->groups[i] == BT_MESH_ADDR_UNASSIGNED) {
            continue;
        }
        mod_sub_list_para->index++;
        mod_sub_list_para->sub_list[j++] = mod->groups[i];
    }
    return;
}

static void ble_mesh_mod_sub_operator_by_id_iter_cb(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                                                    bool vnd, bool primary, void *user_data)
{
    ble_mesh_model_sub_op_by_id_para_t *mod_sub_op = (ble_mesh_model_sub_op_by_id_para_t *)user_data;
    if (mod_sub_op->ret_code != 0) {
        return; // 已经发生错误，不再继续执行
    }

    if (ble_mesh_mod_is_exist(mod, vnd, &mod_sub_op->mod_id)) {
        mod_sub_op->is_model_match = true;
        goto mod_match;
    }
    return;

mod_match:
    switch (mod_sub_op->op_code) {
        case ADD_SUB:
            mod_sub_op->ret_code = bt_mesh_add_mod_sub(mod, mod_sub_op->sub_addr);
            break;
        case DEL_SUB:
            mod_sub_op->ret_code = bt_mesh_del_mod_sub(mod, mod_sub_op->sub_addr);
            break;
        case OVERWRITE_SUB:
            mod_sub_op->ret_code = bt_mesh_overwrite_mod_sub(mod, mod_sub_op->sub_addr);
            break;
        default:
            break;
    }
    return;
}

static void ble_mesh_mod_sub_operator_by_elem_iter_cb(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                                                      bool vnd, bool primary, void *user_data)
{
    if (mod->id == BT_MESH_MODEL_ID_CFG_SRV || mod->id == BT_MESH_MODEL_ID_CFG_CLI) {
        return;
    }

    ble_mesh_model_sub_op_by_elem_para_t *mod_elem_sub_op = (ble_mesh_model_sub_op_by_elem_para_t *)user_data;
    if (mod_elem_sub_op->ret_code != 0) {
        return; // 已经发生错误，不再继续执行
    }

    if (!BT_MESH_ADDR_IS_UNICAST(mod_elem_sub_op->elem_addr) || elem->addr == mod_elem_sub_op->elem_addr) {
        mod_elem_sub_op->is_model_match = true;
        goto mod_match;
    }
    return;

mod_match:
    switch (mod_elem_sub_op->op_code) {
        case ADD_SUB:
            mod_elem_sub_op->ret_code = bt_mesh_add_mod_sub(mod, mod_elem_sub_op->sub_addr);
            break;
        case DEL_SUB:
            mod_elem_sub_op->ret_code = bt_mesh_del_mod_sub(mod, mod_elem_sub_op->sub_addr);
            break;
        case OVERWRITE_SUB:
            mod_elem_sub_op->ret_code = bt_mesh_overwrite_mod_sub(mod, mod_elem_sub_op->sub_addr);
            break;
        default:
            break;
    }
    return;
}

errcode_t ble_mesh_set_default_ttl(uint8_t default_ttl)
{
    int err = 0;

    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    err = bt_mesh_default_ttl_set(default_ttl);
    if (err != ERRCODE_BLE_MESH_SUCCESS) {
        BT_ERR("default ttl %u set error", default_ttl);
        return ble_mesh_errcode_convert(err);
    }
    
    return ble_mesh_errcode_convert(err);
}

uint8_t ble_mesh_get_default_ttl()
{
    return bt_mesh_default_ttl_get();
}

errcode_t ble_mesh_set_net_transmit(uint8_t count, uint16_t interval)
{
    uint8_t xmit = 0;

    if (!ble_mesh_is_enable()) {
        BT_ERR("mesh stack not enable");
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (count > MAX_TRANSMIT_COUNT) {
        BT_ERR("transmit count %u error", count);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    xmit = BT_MESH_TRANSMIT(count, interval);
    bt_mesh_net_transmit_set(xmit);

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_get_net_transmit(uint8_t *count, uint16_t *interval)
{
    uint8_t xmit = 0;

    if (count == NULL || interval == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    xmit = bt_mesh_net_transmit_get();
    *count = BT_MESH_TRANSMIT_COUNT(xmit);
    *interval = BT_MESH_TRANSMIT_INT(xmit);

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_set_relay(ble_mesh_feat_state_t state, uint8_t count, uint16_t interval)
{
    uint8_t xmit = 0;
    if (!CONFIG_BT_MESH_RELAY) {
        return ERRCODE_BLE_MESH_UNSUPPORTED;
    }
    
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (count > MAX_TRANSMIT_COUNT) {
        BT_ERR("transmit count %u error", count);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    xmit = BT_MESH_TRANSMIT(count, interval);

    int ret = bt_mesh_relay_set(state, xmit);
    return ble_mesh_errcode_convert(ret);
}

errcode_t ble_mesh_set_relay_state(ble_mesh_feat_state_t state)
{
    if (!CONFIG_BT_MESH_RELAY) {
        return ERRCODE_BLE_MESH_UNSUPPORTED;
    }
    uint8_t xmit = bt_mesh_relay_retransmit_get();
    int ret = bt_mesh_relay_set(state, xmit);
    return ble_mesh_errcode_convert(ret);
}

ble_mesh_feat_state_t ble_mesh_get_relay_state(void)
{
    enum bt_mesh_feat_state relay = bt_mesh_relay_get();
    switch (relay) {
        case BT_MESH_FEATURE_DISABLED:
            return BLE_MESH_FEATURE_DISABLED;
        case BT_MESH_FEATURE_ENABLED:
            return BLE_MESH_FEATURE_ENABLED;
        default:
            break;
    }
    return BLE_MESH_FEATURE_UNSUPPORT;
}

errcode_t ble_mesh_get_relay_transmit(uint8_t *count, uint16_t *interval)
{
    if (!CONFIG_BT_MESH_RELAY) {
        return ERRCODE_BLE_MESH_UNSUPPORTED;
    }

    if (count == NULL || interval == NULL) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    uint8_t xmit = bt_mesh_relay_retransmit_get();
    *count = BT_MESH_TRANSMIT_COUNT(xmit);
    *interval = BT_MESH_TRANSMIT_INT(xmit);
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_get_mod_sub_list_count(uint32_t mod_id, uint16_t *count)
{
    ble_mesh_mod_sub_list_get_para_t list_get_para = {0};

    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!count) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    
    ble_mesh_vnd_mod_parse(mod_id, &list_get_para.mod_id);
    
    bt_mesh_model_foreach(ble_mesh_get_mod_sub_count_iter_cb, &list_get_para);
    
    if (!list_get_para.is_match) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    *count = list_get_para.count;

    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_get_mod_sub_list(uint32_t mod_id, uint16_t *list, uint16_t *count)
{
    ble_mesh_mod_sub_list_get_para_t list_get_para = {0};
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }
    
    if (!list || !count) {
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    ble_mesh_vnd_mod_parse(mod_id, &list_get_para.mod_id);

    list_get_para.sub_list = list;
    list_get_para.count = *count;

    bt_mesh_model_foreach(ble_mesh_get_mod_sub_list_iter_cb, &list_get_para);
    
    if (!list_get_para.is_match) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    *count = list_get_para.index;
    return ERRCODE_BLE_MESH_SUCCESS;
}

errcode_t ble_mesh_add_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id)
{
    ble_mesh_model_sub_op_by_id_para_t mod_sub_op = {0};

    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    mod_sub_op.sub_addr = sub_addr;
    mod_sub_op.op_code = ADD_SUB;
 
    ble_mesh_vnd_mod_parse(mod_id, &mod_sub_op.mod_id);

    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_id_iter_cb, &mod_sub_op);

    if (!mod_sub_op.is_model_match) {
        BT_ERR("Invalid model:0x%08x", mod_id);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    return ble_mesh_errcode_convert(mod_sub_op.ret_code);
}

errcode_t ble_mesh_del_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id)
{
    ble_mesh_model_sub_op_by_id_para_t mod_sub_op = {0};

    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    mod_sub_op.sub_addr = sub_addr;
    mod_sub_op.op_code = DEL_SUB;
    ble_mesh_vnd_mod_parse(mod_id, &mod_sub_op.mod_id);

    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_id_iter_cb, &mod_sub_op);

    if (!mod_sub_op.is_model_match) {
        BT_ERR("Invalid model:0x%08x", mod_id);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }
    return ble_mesh_errcode_convert(mod_sub_op.ret_code);
}

errcode_t ble_mesh_overwrite_mod_sub_by_model_id(uint16_t sub_addr, uint32_t mod_id)
{
    ble_mesh_model_sub_op_by_id_para_t mod_sub_op = {0};

    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    mod_sub_op.sub_addr = sub_addr;
    mod_sub_op.op_code = OVERWRITE_SUB;
    ble_mesh_vnd_mod_parse(mod_id, &mod_sub_op.mod_id);

    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_id_iter_cb, &mod_sub_op);

    if (!mod_sub_op.is_model_match) {
        BT_ERR("Invalid model:0x%08x", mod_id);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }
    return ble_mesh_errcode_convert(mod_sub_op.ret_code);
}

errcode_t ble_mesh_add_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr)
{
    ble_mesh_model_sub_op_by_elem_para_t mod_sub_para = {0};
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    mod_sub_para.sub_addr = sub_addr;
    mod_sub_para.elem_addr = elem_addr;
    mod_sub_para.op_code = ADD_SUB;
 
    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_elem_iter_cb, &mod_sub_para);
 
    if (!mod_sub_para.is_model_match) {
        BT_ERR("Invalid elem addr:0x%04x", elem_addr);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }
    return ble_mesh_errcode_convert(mod_sub_para.ret_code);
}

errcode_t ble_mesh_del_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr)
{
    ble_mesh_model_sub_op_by_elem_para_t mod_sub_para = {0};
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    mod_sub_para.sub_addr = sub_addr;
    mod_sub_para.elem_addr = elem_addr;
    mod_sub_para.op_code = DEL_SUB;

    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_elem_iter_cb, &mod_sub_para);
    
    if (!mod_sub_para.is_model_match) {
        BT_ERR("Invalid elem addr:0x%04x", elem_addr);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }
    return ble_mesh_errcode_convert(mod_sub_para.ret_code);
}

errcode_t ble_mesh_overwrite_mod_sub_by_elem(uint16_t sub_addr, uint16_t elem_addr)
{
    ble_mesh_model_sub_op_by_elem_para_t mod_sub_para = {0};
    if (!ble_mesh_is_enable()) {
        return ERRCODE_BLE_MESH_NOT_READY;
    }

    if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
        BT_ERR("Invalid sub addr(0x%04x)", sub_addr);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    mod_sub_para.sub_addr = sub_addr;
    mod_sub_para.elem_addr = elem_addr;
    mod_sub_para.op_code = OVERWRITE_SUB;
 
    bt_mesh_model_foreach(ble_mesh_mod_sub_operator_by_elem_iter_cb, &mod_sub_para);
 
    if (!mod_sub_para.is_model_match) {
        BT_ERR("Invalid elem addr:0x%04x", elem_addr);
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }
    return ble_mesh_errcode_convert(mod_sub_para.ret_code);
}