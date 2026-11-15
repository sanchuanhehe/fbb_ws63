 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFU_SERVER_H__
#define _DFU_SERVER_H__

#include "dfu.h"
#include "blob_server.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfu_srv_t;

#define BT_MESH_DFU_DATE_SIZE_0          0
#define BT_MESH_DFU_ENTRY_HEAD_LEN       2
#define BT_MESH_DFU_UPDATE_METADATA_LEN  2
#define BT_MESH_DFU_STATUS_MASK          3
#define BT_MESH_DFU_UPDATE_STATUS_LEN    14
#define BT_MESH_DFU_UPDATE_PHASE         5
#define DFU_SRV_UINT64_ADD_OFFSET        32

#define BT_MESH_DFU_SRV_INIT(_handlers, _imgs, _img_count)               \
    {                                                                    \
        .blob = { .cb = &_bt_mesh_dfu_srv_blob_cb }, .cb = (_handlers),  \
        .imgs = (_imgs), .img_count = (_img_count),                      \
    }

#define BT_MESH_MODEL_DFU_SRV()                                       \
    BT_MESH_MODEL_BLOB_SRV(&g_dfu_srv.blob),                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFU_SRV, _bt_mesh_dfu_srv_op, NULL, \
             (&g_dfu_srv), &_bt_mesh_dfu_srv_cb)

struct bt_mesh_dfu_srv_cb {
    int (*check)(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img,
                 struct os_mbuf *metadata, enum bt_mesh_dfu_effect_t *effect);
    int (*start)(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img,
                 struct os_mbuf *metadata, const struct bt_mesh_blob_stream_t **stream);
    void (*end)(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img, bool success);
    int (*recover)(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img,
                   const struct bt_mesh_blob_stream_t **stream);
    int (*apply)(struct bt_mesh_dfu_srv_t *srv,  const struct bt_mesh_dfu_img_t *img);
};

struct bt_mesh_dfu_srv_t {
    struct bt_mesh_blob_server_t blob;
    const struct bt_mesh_dfu_srv_cb *cb;
    const struct bt_mesh_dfu_img_t *imgs;
    uint32_t img_count;
    struct bt_mesh_model *mod;
    struct {
        uint8_t effect;
        uint8_t phase;
        uint8_t ttl;
        uint8_t idx;
        uint16_t timeout_base;
        uint16_t meta;
    } update;
};

struct bt_mesh_dfu_handle_param_t {
    uint8_t ttl;
    uint8_t idx;
    uint16_t timeout_base;
    uint16_t meta_checksum;
    uint64_t blob_id;
};

void bt_mesh_dfu_srv_verified(struct bt_mesh_dfu_srv_t *srv);
void bt_mesh_dfu_srv_rejected(struct bt_mesh_dfu_srv_t *srv);
void bt_mesh_dfu_srv_cancel(struct bt_mesh_dfu_srv_t *srv);
void bt_mesh_dfu_srv_applied(struct bt_mesh_dfu_srv_t *srv);
bool bt_mesh_dfu_srv_is_busy(const struct bt_mesh_dfu_srv_t *srv);
uint8_t bt_mesh_dfu_srv_progress(const struct bt_mesh_dfu_srv_t *srv);
int bt_mesh_set_target_fwid(const uint8_t *fwid, uint8_t fwid_len);
uint8_t bt_mesh_get_dfu_status(void);

extern const struct bt_mesh_model_op _bt_mesh_dfu_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_dfu_srv_cb;
extern const struct bt_mesh_blob_server_cb_t _bt_mesh_dfu_srv_blob_cb;
extern struct bt_mesh_dfu_srv_t g_dfu_srv;

#ifdef CONFIG_BLE_MESH_PTS_MODE
void target_dfu_verify_fail_set();
void target_dfu_apply_stop_set();
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLUETOOTH_MESH_DFU_SRV_H__ */