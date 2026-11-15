/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "mesh/slist.h"
#include "model/dfu.h"
#include "dfu_bank.h"

#define IS_ARRAY_ELEMENT(array, ptr)                                       \
    ((ptr) && (uintptr_t)(array) <= (uintptr_t)(ptr) &&                    \
        (uintptr_t)(ptr) < (uintptr_t)(&(array)[ARRAY_SIZE(array)]) &&     \
        ((uintptr_t)(ptr) - (uintptr_t)(array)) % sizeof((array)[0]) == 0)

#define ARRAY_INDEX(array, ptr)                        \
    ({                                                 \
        __ASSERT_NO_MSG(IS_ARRAY_ELEMENT(array, ptr)); \
        (__typeof__((array)[0]) *)(ptr) - (array);     \
    })

#define SLOT_ENTRY_BUFLEN 25
#define DFU_SLOT_SETTINGS_PATH "bt/mesh-dfu/bank"

static sys_slist_t list;

static struct bank_t {
    uint32_t idx;
    struct bt_mesh_dfu_bank_t bank;
    sys_snode_t n;
} banks[MYNEWT_VAL_BT_MESH_DFU_BANK_CNT];

static uint32_t bank_index;

static bool bank_eq(const struct bt_mesh_dfu_bank_t *bank, const uint8_t *fwid, uint32_t fwid_len)
{
    return (bank->fwid_len == fwid_len) && !memcmp(fwid, bank->fwid, fwid_len);
}

static bool is_bank_committed(struct bank_t *bank_to_check)
{
    struct bank_t *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        if (s == bank_to_check) {
            return true;
        }
    }

    return false;
}

static int bank_store(const struct bank_t *bank_to_store)
{
    int err = 0;

    return err;
}

static void bank_erase(struct bank_t *bank_to_erase)
{
}

static void bank_index_defrag(void)
{
    bank_index = 0;
    struct bank_t *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        s->idx = ++bank_index;
        bank_store(s);
    }
}

int bt_mesh_dfu_bank_count(void)
{
    int cnt = 0;
    sys_snode_t *n;

    SYS_SLIST_FOR_EACH_NODE(&list, n)
    {
        cnt++;
    }

    return cnt;
}

struct bt_mesh_dfu_bank_t *bt_mesh_dfu_bank_reserve(void)
{
    struct bank_t *bank = NULL;

    for (uint32_t i = 0; i < ARRAY_SIZE(banks); ++i) {
        if (banks[i].idx == 0) {
            bank = &banks[i];
            break;
        }
    }

    if (!bank) {
        BT_WARN("No space");
        return NULL;
    }

    if (bank_index == UINT32_MAX) {
        bank_index_defrag();
    }

    bank->bank.fwid_len = 0;
    bank->bank.metadata_len = 0;
    bank->bank.size = 0;
    bank->idx = ++bank_index;

    BT_INFO("Reserved bank #%u", bank - &banks[0]);

    return &bank->bank;
}

int bt_mesh_dfu_bank_info_set(
    struct bt_mesh_dfu_bank_t *dfu_bank, uint32_t size, const uint8_t *metadata, uint32_t metadata_len)
{
    struct bank_t *bank = CONTAINER_OF(dfu_bank, struct bank_t, bank);

    if (metadata_len > MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN) {
        return -EFBIG;
    }

    if (bank->idx == 0 || is_bank_committed(bank)) {
        return -EINVAL;
    }

    bank->bank.size = size;
    bank->bank.metadata_len = metadata_len;
    memcpy_s(bank->bank.metadata, metadata_len, metadata, metadata_len);
    return 0;
}

int bt_mesh_dfu_bank_fwid_set(struct bt_mesh_dfu_bank_t *dfu_bank, const uint8_t *fwid, uint32_t fwid_len)
{
    struct bank_t *bank = CONTAINER_OF(dfu_bank, struct bank_t, bank);

    if (fwid_len > MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN) {
        return -EFBIG;
    }

    if (bank->idx == 0 || is_bank_committed(bank)) {
        return -EINVAL;
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(banks); i++) {
        if (banks[i].idx != 0 && bank_eq(&banks[i].bank, fwid, fwid_len)) {
            return is_bank_committed(&banks[i]) ? -EEXIST : -EALREADY;
        }
    }

    bank->bank.fwid_len = fwid_len;
    memcpy_s(bank->bank.fwid, fwid_len, fwid, fwid_len);
    return 0;
}

int bt_mesh_dfu_bank_commit(struct bt_mesh_dfu_bank_t *dfu_bank)
{
    int err;
    struct bank_t *bank = CONTAINER_OF(dfu_bank, struct bank_t, bank);

    if (bank->idx == 0 || bank->bank.fwid_len == 0 || bank->bank.size == 0 || is_bank_committed(bank)) {
        return -EINVAL;
    }

    err = bank_store(bank);
    if (err) {
        BT_WARN("Store failed (err: %d)", err);
        return err;
    }

    sys_slist_append(&list, &bank->n);

    BT_INFO("Stored bank #%u: %s", ARRAY_INDEX(banks, bank), bt_hex(bank->bank.fwid, bank->bank.fwid_len));
    return 0;
}

void bt_mesh_dfu_bank_release(const struct bt_mesh_dfu_bank_t *dfu_bank)
{
    struct bank_t *bank = CONTAINER_OF(dfu_bank, struct bank_t, bank);

    if (is_bank_committed(bank)) {
        return;
    }

    bank->idx = 0;
}

int bt_mesh_dfu_bank_del(const struct bt_mesh_dfu_bank_t *dfu_bank)
{
    struct bank_t *bank = CONTAINER_OF(dfu_bank, struct bank_t, bank);

    if (!sys_slist_find_and_remove(&list, &bank->n)) {
        return -EINVAL;
    }

    int idx = ARRAY_INDEX(banks, bank);

    BT_INFO("%u", idx);

    bank_erase(bank);
    bank->idx = 0;

    return 0;
}

void bt_mesh_dfu_bank_del_all(void)
{
    struct bank_t *s;
    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        bank_erase(s);
        s->idx = 0;
    }

    sys_slist_init(&list);
}

const struct bt_mesh_dfu_bank_t *bt_mesh_dfu_bank_get_by_idx(uint16_t img_idx)
{
    struct bank_t *s;
    uint16_t remaining_img_idx = img_idx;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        if (!remaining_img_idx--) {
            return &s->bank;
        }
    }

    return NULL;
}

int bt_mesh_dfu_bank_get(const uint8_t *fwid, uint32_t fwid_len, struct bt_mesh_dfu_bank_t **bank)
{
    struct bank_t *s;
    int idx = 0;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        if (bank_eq(&s->bank, fwid, fwid_len)) {
            if (bank) {
                *bank = &s->bank;
            }
            return idx;
        }
        idx++;
    }

    return -ENOENT;
}

int bt_mesh_dfu_bank_img_idx_get(const struct bt_mesh_dfu_bank_t *dfu_bank)
{
    struct bank_t *s;
    int idx = 0;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        if (&s->bank == dfu_bank) {
            return idx;
        }
        idx++;
    }

    return -ENOENT;
}

uint32_t bt_mesh_dfu_bank_foreach(bt_mesh_dfu_bank_cb_t cb, void *user_data)
{
    enum bt_mesh_dfu_iter_t iter;
    uint32_t cnt = 0;
    struct bank_t *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n)
    {
        cnt++;

        if (!cb) {
            continue;
        }

        iter = cb(&s->bank, user_data);
        if (iter != BT_MESH_DFU_ITER_CONTINUE) {
            break;
        }
    }

    return cnt;
}