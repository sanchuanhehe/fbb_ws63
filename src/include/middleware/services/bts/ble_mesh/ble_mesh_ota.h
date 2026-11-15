/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: ble_mesh_ota.h ble mesh OTA 接口描述
 */

/**
 * @defgroup bluetooth_bts_mesh_ota BLE MESH OTA API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BLE_MESH_OTA_H
#define BLE_MESH_OTA_H

#include <stdint.h>

#include "ble_mesh_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Length of firmware id.
 * @else
 * @brief  固件ID长度。
 * @endif
 */
#define BLE_MESH_OTA_FWID_MAX_LEN 16

/**
 * @if Eng
 * @brief  Length of firmware metadata.
 * @else
 * @brief  固件metadata长度。
 * @endif
 */
#define BLE_MESH_OTA_METADATA_MAX_LEN 32

/**
 * @if Eng
 * @brief Enum of ota firmware update status.
 * @else
 * @brief OTA固件升级状态。
 * @endif
 */
typedef enum {
    BLE_MESH_FIRMWARE_UPDATE_IDLE              = 0x00,       /*!< @if Eng Idle
                                                                  @else   空闲状态 @endif */
    BLE_MESH_FIRMWARE_UPDATE_TRANSFER_ERR      = 0x01,       /*!< @if Eng Transfer BLOB procedure failed
                                                                  @else  BLOB传输失败 @endif */
    BLE_MESH_FIRMWARE_UPDATE_TRANSFER_ACTIVE   = 0x02,       /*!< @if Eng Receive Firmware procedure is being executed
                                                                  @else  BLOB传输中 @endif */
    BLE_MESH_FIRMWARE_UPDATE_VERIFY            = 0x03,      /*!< @if Eng Verify Firmware procedure is being executed
                                                                  @else  升级校验中 @endif */
    BLE_MESH_FIRMWARE_UPDATE_VERIFY_OK         = 0x04,      /*!< @if Eng Verify Firmware procedure completed
                                                                  @else  升级校验完成 @endif */
    BLE_MESH_FIRMWARE_UPDATE_VERIFY_FAIL       = 0x05,      /*!< @if Eng Verify Firmware procedure failed
                                                                  @else  升级校验失败 @endif */
    BLE_MESH_FIRMWARE_UPDATE_APPLYING          = 0x06,      /*!< @if Eng Apply New Firmware procedure is being executed
                                                                  @else  固件生效中 @endif */
    BLE_MESH_FIRMWARE_UPDATE_RFU               = 0x07,      /*!< @if Eng Reserved for Future Use.
                                                                  @else  预留 @endif */
} ble_mesh_firmware_update_phase_t;

/**
 * @if Eng
 * @brief Enum of ota upgrade status.
 * @else
 * @brief OTA升级状态。
 * @endif
 */
typedef enum {
    OTA_UPGRADE_START = 0x01,  /*!< @if Eng OTA upgrade Start
                                    @else  OTA升级开始 @endif */
    TRANSFER_START = 0x02,     /*!< @if Eng Block Transfer Start
                                    @else  BLOCK传输开始 @endif */
    TRANSFER_END = 0x03,       /*!< @if Eng Block Transfer End
                                    @else  BLOCK传输结束 @endif */
    OTA_UPGRADE_END = 0x04,    /*!< @if Eng OTA upgrade End
                                    @else  OTA升级结束 @endif */
} ble_mesh_ota_upgrade_status_t;

/**
 * @if Eng
 * @brief  Call this funtion to obtain the OTA upgrade flag.
 * @par  Call this funtion to obtain the OTA upgrade flag.
 * @param  [in] upg_flag OTA update flag: 0 indicates no OTA update has been performed,
                         1 indicates an OTA update has been performed.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  调用此接口获取OTA升级标记。
 * @par  调用此接口获取OTA升级标记。
 * @param  [in] upg_flag OTA升级标记，0未进行过OTA升级，1进行过OTA升级
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_ota_get_upgrade_flag(uint8_t *upg_flag);

/**
 * @if Eng
 * @brief  Call this funtion to clear the OTA upgrade flag.
 * @par  Call this funtion to clear the OTA upgrade flag.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @else
 * @brief  调用此接口清除OTA升级标记。
 * @par  调用此接口清除OTA升级标记。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @endif
 */
errcode_t ble_mesh_ota_clear_upgrade_flag(void);

/**
 * @if Eng
 * @brief  Call this interface to determine whether it is allowed to restart the device.
 * @par  Call this interface to determine whether it is allowed to restart the device.
 * @retval true: reboot system.
 * @retval false: do not reboot system.
 * @else
 * @brief  调用此接口判断是否允许重启设备让新固件生效。
 * @par  调用此接口判断是否允许重启设备让新固件生效。
 * @retval true： 重启设备。
 * @retval false：不重启设备。
 * @endif
 */
typedef bool (*ble_mesh_ota_upgrade_check_callback)(void);

/**
 * @if Eng
 * @brief  Call this interface to check metadata whether the firmware image can be updated.
 * @par  Call this interface to check metadata whether the firmware image can be updated.
 * @param  [in]  metadata  metadata.
 * @param  [in]  len  metadata length.
 * @retval true: matadata check valid.
 * @retval false: matadata check invalid.
 * @else
 * @brief  调用此接口检查metadata是否允许升级。
 * @par  调用此接口检查metadata是否允许升级。
 * @param  [in]  metadata  metadata.
 * @param  [in]  len  metadata长度.
 * @retval true： metadata检查合法。
 * @retval false：metadata检查不合法。
 * @endif
 */
typedef bool (*ble_mesh_ota_metadata_check_callback)(const uint8_t *metadata, uint8_t len);

/**
 * @if Eng
 * @brief  Call this interface to obtain the ota upgrade status.
 * @par  Call this interface to obtain the ota upgrade status.
 * @param  [in]  upgrade_status  OTA upgrade status.
 * @retval None。
 * @else
 * @brief  调用此接口获取OTA升级状态。
 * @par  调用此接口获取OTA升级状态。
 * @param  [in]  upgrade_status  OTA升级状态.
 * @retval 无。
 * @endif
 */
typedef void (*ble_mesh_ota_upgrade_status_callback)(ble_mesh_ota_upgrade_status_t upgrade_status);

/**
 * @if Eng
 * @brief Struct of ota callback function.
 * @else
 * @brief OTA相关回调接口注册。
 * @endif
 */
typedef struct ble_mesh_ota_register_callbacks {
    ble_mesh_ota_upgrade_check_callback  upgrade_check_cb;            /*!< @if Eng Upgrade check callback.
                                                                           @else   升级检查。 @endif */
    ble_mesh_ota_metadata_check_callback metadata_check_cb;           /*!< @if Eng metadata check callback.
                                                                           @else   metadata检查。 @endif */
    ble_mesh_ota_upgrade_status_callback ota_upgrade_status_cb;       /*!< @if Eng ota upgrade status report callback.
                                                                           @else   OTA升级状态上报。 @endif */
} ble_mesh_ota_register_callbacks_t;

/**
 * @if Eng
 * @brief OTA firmware info.
 * @else
 * @brief ota固件信息
 * @endif
 */
typedef struct ble_mesh_dfu_firmware_info {
    uint32_t size;                                                  /*!< @if Eng firmware size.
                                                                         @else   固件大小。 @endif */
    uint32_t fwid_len;                                              /*!< @if Eng firmware id length.
                                                                         @else   固件id长度。 @endif */
    uint32_t metadata_len;                                          /*!< @if Eng firmware metadata length.
                                                                         @else   固件metadata长度。 @endif */
    uint8_t fwid[BLE_MESH_OTA_FWID_MAX_LEN];                        /*!< @if Eng firmware id.
                                                                         @else   固件id。 @endif */
    uint8_t metadata[BLE_MESH_OTA_METADATA_MAX_LEN];                /*!< @if Eng firmware metadata.
                                                                         @else   固件metadata。 @endif */
} ble_mesh_dfu_firmware_info_t;

/**
 * @if Eng
 * @brief This API is used to add OTA firmware information.
 * @par This API is used to add OTA firmware information.
 * @attention Only one firmware information can be added.
 * @param  [in]  fw_info  firmware info.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @li
 * @see
 * @else
 * @brief  调用此接口添加OTA固件信息。
 * @par    调用此接口添加OTA固件信息。
 * @attention 只能添加一个固件信息。
 * @param  [in]  fw_info  固件信息。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_ota_add_firmware(const ble_mesh_dfu_firmware_info_t *fw_info);

/**
 * @if Eng
 * @brief This API is used to delete the information of the added OTA firmware.
 * @par This API is used to delete the information of the added OTA firmware.
 * @attention
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口删除已添加OTA固件信息。
 * @par    调用此接口删除已添加OTA固件信息。
 * @attention
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_ota_del_all_firmware(void);

/**
 * @if Eng
 * @brief This API is used to set the firmware ID of the target node.
 * @par This API is used to set the firmware ID of the target node.
 * @attention
 * @param  [in]  fwid      firmware id。
 * @param  [in]  fwid_len  firmware id lengeh, Currently the maximum supported length for fwid is 4 bytes.
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口设置目标节点的固件ID。
 * @par    调用此接口设置目标节点的固件ID。
 * @attention
 * @param  [in]  fwid      固件ID。
 * @param  [in]  fwid_len  固件ID长度, 目前最大长度支持4字节的fwid。
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_ota_set_target_fwid(const uint8_t *fwid, uint8_t fwid_len);

/**
 * @if Eng
 * @brief This API is used to get the status of the target node firmware upgrade.
 * @par This API is used to get the status of the target node firmware upgrade.
 * @attention
 * @retval Firmware upgrade status. see @ref ble_mesh_firmware_update_phase_t
 * @par Dependency:
 * @else
 * @brief  获取目标节点固件升级的状态。
 * @par    获取目标节点固件升级的状态。
 * @attention
 * @retval 固件升级状态。 see @ref ble_mesh_firmware_update_phase_t
 * @par 依赖:
 * @endif
 */
ble_mesh_firmware_update_phase_t ble_mesh_ota_get_firmware_update_status(void);

/**
 * @if Eng
 * @brief This API is used to register the callbacks of the OTA.
 * @par This API is used to register the callbacks of the OTA.
 * @attention
 * @retval ERRCODE_BLE_MESH_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_ble_mesh_t
 * @par Dependency:
 * @else
 * @brief  调用此接口注册OTA回调接口。
 * @par    调用此接口注册OTA回调接口。
 * @attention
 * @retval ERRCODE_BLE_MESH_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_ble_mesh_t
 * @par 依赖:
 * @endif
 */
errcode_t ble_mesh_ota_register_callbacks(const ble_mesh_ota_register_callbacks_t *func);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */