/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:
 *
 * History:
 * 2025-6-04, Create file.
 */

#ifndef NV_OPERATOR_H
#define NV_OPERATOR_H
#include "errcode.h"
#include "key_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BTH_BLE_MESH_RPL_NV_ID          NV_ID_BTH_BLE_MESH_RPL              /* ble mesh replay list */
#define BTH_BLE_MESH_NET_KEY_NV_ID      NV_ID_BTH_BLE_MESH_NET_KEY          /* ble mesh net key */
#define BTH_BLE_MESH_APP_KEY_NV_ID      NV_ID_BTH_BLE_MESH_APP_KEY          /* ble mesh app key */
#define BTH_BLE_MESH_NET_ADDR_NV_ID     NV_ID_BTH_BLE_MESH_NET_ADDR         /* ble mesh net addr */
#define BTH_BLE_MESH_IV_NV_ID           NV_ID_BTH_BLE_MESH_IV               /* ble mesh iv */
#define BTH_BLE_MESH_SEQ_NV_ID          NV_ID_BTH_BLE_MESH_SEQ              /* ble mesh sequence */
#define BTH_BLE_MESH_HB_PUB_NV_ID       NV_ID_BTH_BLE_MESH_HB_PUB           /* ble mesh heart beat */
#define BTH_BLE_MESH_CFG_NV_ID          NV_ID_BTH_BLE_MESH_CFG              /* ble mesh cfg */
#define BTH_BLE_MESH_VA_NV_ID           NV_ID_BTH_BLE_MESH_VA               /* ble mesh virtual addr */
#define BTH_BLE_MESH_MOD_NV_ID          NV_ID_BTH_BLE_MESH_MOD              /* ble mesh model nv id */
#define BTH_BLE_MESH_TUPLE_NV_ID        NV_ID_BTH_BLE_MESH_TUPLE            /* ble mesh model tuple id */
#define BTH_BLE_MESH_COMP_DATA_128      NV_ID_COMPOSITION_DATA_PAGE_128     /* comp data page 128 */
#define BTH_BLE_MESH_OTA_FLAG_NV_ID     NV_ID_BTH_BLE_MESH_OTA_FLAG         /* ble mesh ota flag */
/**
 * @brief  写入持久化数据。
 * @param  [in]  key  NV Key。
 * @param  [in]  data 待写入数据，内存申请释放有调用者负责。
 * @param  [in]  len  待写入数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 */
errcode_t ble_mesh_data_write(uint16_t key, const uint8_t *data, uint16_t len);

/**
 * @brief  读取持久化数据。
 * @param  [in]  key  NV Key。
 * @param  [in]  len  待读取数据长度。
 * @param  [in]  len  实际读取数据长度。
 * @param  [in]  data 读出的数据，内存申请释放有调用者负责。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 */
errcode_t ble_mesh_data_read(uint16_t key, uint16_t len, uint16_t *real_len, uint8_t *data);

/**
 * @brief  写入加密持久化数据。
 * @param  [in]  key  NV Key。
 * @param  [in]  data 待写入数据，内存申请释放有调用者负责。
 * @param  [in]  len  待写入数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 */
errcode_t ble_mesh_data_write_encrypt(uint16_t key, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif