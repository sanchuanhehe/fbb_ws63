/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides sfc driver patch header. \n
 *
 * History: \n
 * 2023-11-06, Create file. \n
 */

#ifndef SFC_PATCH_H
#define SFC_PATCH_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// hal
extern errcode_t hal_sfc_write_enable(void);
extern errcode_t hal_sfc_regs_wait_ready(uint8_t wip_bit);

// driver
extern errcode_t check_opt_param(uint32_t addr, uint32_t size);

errcode_t uapi_sfc_get_flash_id(uint32_t *flash_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif