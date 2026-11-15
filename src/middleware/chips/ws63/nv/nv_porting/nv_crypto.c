/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: NV crypto
 * Author: 
 * History:
 * 2025-09-23, Create file.
 */
#include "nv_porting.h"
#include "security_sha256.h"

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
#include "efuse.h"
#include "cipher.h"
#include "km.h"
#include "trng.h"

#define INVALID_HANDLE              (0xFFFFFFFF)
#define NV_SECTION_KEY_LEN          16
#define NV_TAG_LENGTH               16
#define NV_IV_LENGTH                12
#define NV_DIE_ID_LENGTH_BYTES      16
#define NV_DIE_ID_LEN_WORDS         4
#define NV_DIE_ID_LOW_32            0
#define NV_DIE_ID_HIGH_32           3
#define NV_SALT_LEN                 28
#define NV_UUID_LEN                 16
#define NV_SECTION_KEY_OFFSET_LEN   (NV_SALT_LEN - NV_UUID_LEN)
#define NV_SECTION_KEY_OFFSET       (NV_SECTION_KEY_LEN - NV_SECTION_KEY_OFFSET_LEN)

static uint32_t g_keyslot = INVALID_HANDLE;
static uint8_t g_nv_encrypt_tag[NV_TAG_LENGTH] = {0};

typedef struct {
    uint8_t type;
    uint8_t upgrade;
    uint16_t key_id;
    uint16_t enc_key;
    uint16_t version;
    uint32_t rnd;
    uint32_t die_id;
} kv_crypto_section_key_info_t;

typedef struct {
    uint16_t enc_key;
    uint16_t version;
    uint32_t rnd;
    uint32_t die_id;
} kv_crypto_iv_info_t;

// section_key: (type || upgrade || key_id || enc_key || version || rnd || DIE_ID), use high 32 bits of DIE_ID
static void nv_crypto_make_section_key(uint8_t *section_key, const kv_key_header_t *header, uint32_t die_id_high32)
{
    kv_crypto_section_key_info_t *kv_crypto_key_info = (kv_crypto_section_key_info_t *)(uintptr_t)section_key;
    kv_crypto_key_info->type = header->type;
    kv_crypto_key_info->upgrade = header->upgrade;
    kv_crypto_key_info->key_id = header->key_id;
    kv_crypto_key_info->enc_key = header->enc_key;
    kv_crypto_key_info->version = header->version;
    kv_crypto_key_info->rnd = header->rnd;
    kv_crypto_key_info->die_id = die_id_high32;
}

// iv: (enc_key || version || rnd || DIE_ID), use low 32 bits of DIE_ID
static void nv_crypto_make_iv(uint8_t *iv, const kv_key_header_t *header, uint32_t die_id_low32)
{
    kv_crypto_iv_info_t *kv_crypto_iv_info = (kv_crypto_iv_info_t *)(uintptr_t)iv;
    kv_crypto_iv_info->enc_key = header->enc_key;
    kv_crypto_iv_info->version = header->version;
    kv_crypto_iv_info->rnd = header->rnd;
    kv_crypto_iv_info->die_id = die_id_low32;
}

// salt: (section_key || uuid), skip the low 4 bytes of section_key
static errcode_t nv_crypto_make_salt(uint8_t *salt, uint32_t salt_len, uint32_t die_id_high32,
                                     const kv_key_header_t *header)
{
    errno_t ret;
    uint8_t section_key[NV_SECTION_KEY_LEN] = {0};
    const uint8_t uuid[NV_UUID_LEN] = {
        0xCA, 0x2B, 0xF7, 0x6A, 0xFF, 0x79, 0x4E, 0xAC,
        0xA6, 0x3D, 0x4E, 0x0A, 0xAA, 0xEE, 0x6C, 0x77
    };

    nv_crypto_make_section_key(section_key, header, die_id_high32);

    ret = memcpy_s(salt, NV_SECTION_KEY_OFFSET_LEN, section_key + NV_SECTION_KEY_OFFSET, NV_SECTION_KEY_OFFSET_LEN);
    nv_chk_return(ret != EOK, ERRCODE_FAIL, "[NV] section_key memcpy_s failed!\r\n");

    ret = memcpy_s(salt + NV_SECTION_KEY_OFFSET_LEN, salt_len - NV_SECTION_KEY_OFFSET_LEN, uuid, sizeof(uuid));
    nv_chk_return(ret != EOK, ERRCODE_FAIL, "[NV] uuid memcpy_s failed!\r\n");

    return ERRCODE_SUCC;
}

static errcode_t nv_crypto_cipher_create(uint32_t *symc_handle, uint32_t *keyslot)
{
    errcode_t ret;
    uapi_drv_cipher_symc_attr_t symc_attr = {
        .symc_alg = UAPI_DRV_CIPHER_SYMC_ALG_AES,
        .work_mode = UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM,
        .symc_type = UAPI_DRV_CIPHER_SYMC_TYPE_NORMAL,
        .is_long_term = true
    };

    ret = uapi_drv_cipher_symc_create(symc_handle, &symc_attr);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] symc_handle create failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_keyslot_create(keyslot, UAPI_DRV_KEYSLOT_TYPE_MCIPHER);
    nv_chk_goto(ret != ERRCODE_SUCC, free_symc, "[NV] keyslot create failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_cipher_symc_attach(*symc_handle, *keyslot);
    nv_chk_goto(ret != ERRCODE_SUCC, free_keyslot, "[NV] symc attach failed! ret = 0x%x\r\n", ret);

    return ERRCODE_SUCC;

free_keyslot:
    (void)uapi_drv_keyslot_destroy(*keyslot);
free_symc:
    (void)uapi_drv_cipher_symc_destroy(*symc_handle);
    return ret;
}

static errcode_t nv_crypto_set_hard_key(uint32_t keyslot, const kv_key_header_t *header, uint32_t die_id_high32)
{
    uint32_t klad;
    uint8_t salt[NV_SALT_LEN] = {0};
    errcode_t ret;

    ret = nv_crypto_make_salt(salt, sizeof(salt), die_id_high32, header);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] make salt failed!\r\n");

    uapi_drv_klad_attr_t klad_attr = {
        .klad_cfg.rootkey_type = UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_REE,
        .key_cfg.engine = UAPI_DRV_KLAD_ENGINE_AES,
        .key_cfg.decrypt_support = true,
        .key_cfg.encrypt_support = true,
        .key_sec_cfg.dest_buf_sec_support = true,
        .key_sec_cfg.dest_buf_non_sec_support = true,
        .key_sec_cfg.src_buf_sec_support = true,
        .key_sec_cfg.src_buf_non_sec_support = true,
    };

    uapi_drv_klad_effective_key_t effective_key = {
        .kdf_hard_alg = UAPI_DRV_KDF_HARD_ALG_SHA256,
        .key_parity = false,
        .key_size = UAPI_DRV_KLAD_KEY_SIZE_128BIT,
        .salt = salt,
        .salt_length = sizeof(salt),
    };

    ret = uapi_drv_klad_create(&klad);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] klad create failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_klad_attach(klad, UAPI_DRV_KLAD_DEST_MCIPHER, keyslot);
    nv_chk_goto(ret != ERRCODE_SUCC, cleanup, "[NV] klad attach failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_klad_set_attr(klad, &klad_attr);
    nv_chk_goto(ret != ERRCODE_SUCC, cleanup, "[NV] klad set attr failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_klad_set_effective_key(klad, &effective_key);
    nv_chk_goto(ret != ERRCODE_SUCC, cleanup, "[NV] klad set effective key failed! ret = 0x%x\r\n", ret);

    ret = uapi_drv_klad_detach(klad, UAPI_DRV_KLAD_DEST_MCIPHER, keyslot);
    nv_chk_goto(ret != ERRCODE_SUCC, cleanup, "[NV] klad detach failed! ret = 0x%x\r\n", ret);

cleanup:
    (void)uapi_drv_klad_destroy(klad);
    return ret;
}

static void nv_crypto_cipher_destroy(uint32_t symc_handle, uint32_t keyslot)
{
    (void)uapi_drv_cipher_symc_detach(symc_handle, keyslot);
    (void)uapi_drv_keyslot_destroy(keyslot);
    (void)uapi_drv_cipher_symc_destroy(symc_handle);
}

/* 创建加解密通道 */
errcode_t nv_crypto_claim_aes(uint32_t *crypto_handle, const kv_key_header_t *header)
{
    uint32_t symc_handle = INVALID_HANDLE;
    uint32_t die_id[NV_DIE_ID_LEN_WORDS] = {0};
    uint8_t iv[NV_IV_LENGTH] = {0};
    errcode_t ret;

    ret = uapi_efuse_get_die_id((uint8_t *)(uintptr_t)die_id, NV_DIE_ID_LENGTH_BYTES);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] get die_id failed! ret = 0x%x\r\n", ret);

    nv_crypto_make_iv(iv, header, die_id[NV_DIE_ID_LOW_32]);

    uapi_drv_cipher_symc_config_aes_ccm_gcm_t gcm_cfg = {
        .aad_buf.phys_addr = (uintptr_t)header,
        .aad_buf.virt_addr = (void *)header,
        .aad_len = sizeof(kv_key_header_t),
    };

    uapi_drv_cipher_symc_ctrl_t symc_ctrl = {
        .symc_alg = UAPI_DRV_CIPHER_SYMC_ALG_AES,
        .work_mode = UAPI_DRV_CIPHER_SYMC_WORK_MODE_GCM,
        .symc_key_length = UAPI_DRV_CIPHER_SYMC_KEY_128BIT,
        .symc_bit_width = UAPI_DRV_CIPHER_SYMC_BIT_WIDTH_128BIT,
        .iv_change_flag = UAPI_DRV_CIPHER_SYMC_GCM_IV_CHANGE_START,
        .iv_length = sizeof(iv),
        .param = &gcm_cfg
    };

    errno_t res = memcpy_s(symc_ctrl.iv, sizeof(symc_ctrl.iv), iv, sizeof(iv));
    nv_chk_return(res != EOK, ERRCODE_FAIL, "[NV] iv memcpy_s failed!\r\n");
    
    ret = nv_crypto_cipher_create(&symc_handle, &g_keyslot);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] symc and keyslot create failed!\r\n");

    ret = nv_crypto_set_hard_key(g_keyslot, header, die_id[NV_DIE_ID_HIGH_32]);
    nv_chk_goto(ret != ERRCODE_SUCC, err, "[NV] set hard key failed!\r\n");

    ret = uapi_drv_cipher_symc_set_config(symc_handle, &symc_ctrl);
    nv_chk_goto(ret != ERRCODE_SUCC, err, "[NV] symc set config failed! ret = 0x%x\r\n", ret);

    *crypto_handle = symc_handle;
    nv_log_info("[NV] nv_crypto_claim_aes succ!\r\n");
    return ERRCODE_SUCC;
err:
    nv_crypto_cipher_destroy(symc_handle, g_keyslot);
    g_keyslot = INVALID_HANDLE;
    return ret;
}

/* 释放加解密通道 */
void nv_crypto_release_aes(uint32_t crypto_handle)
{
    if (crypto_handle != INVALID_HANDLE) {
        nv_crypto_cipher_destroy(crypto_handle, g_keyslot);
        g_keyslot = INVALID_HANDLE;
        nv_log_info("[NV] nv_crypto_release_aes succ!\r\n");
    }
}

/* NV数据加密 */
errcode_t nv_crypto_encode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length)
{
    if (crypto_handle == INVALID_HANDLE || src == 0 || dest == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    errcode_t ret;
    uapi_drv_cipher_buf_attr_t src_buf = { .phys_addr = src };
    uapi_drv_cipher_buf_attr_t dst_buf = { .phys_addr = dest };

    ret = uapi_drv_cipher_symc_encrypt(crypto_handle, &src_buf, &dst_buf, length);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] encrypt failed! ret = 0x%x\r\n", ret);

    nv_log_info("[NV] encode succ!\r\n");
    return ret;
}

/* NV数据解密 */
errcode_t nv_crypto_decode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length)
{
    if (crypto_handle == INVALID_HANDLE || src == 0 || dest == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    errcode_t ret;
    uapi_drv_cipher_buf_attr_t src_buf = { .phys_addr = src };
    uapi_drv_cipher_buf_attr_t dst_buf = { .phys_addr = dest };

    ret = uapi_drv_cipher_symc_decrypt(crypto_handle, &src_buf, &dst_buf, length);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] decrypt failed! ret = 0x%x\r\n", ret);

    nv_log_info("[NV] decode succ!\r\n");
    return ret;
}

/* 获取加密Tag */
errcode_t nv_crypto_get_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t *tag_len)
{
    if (crypto_handle == INVALID_HANDLE || tag == NULL || tag_len == NULL || *tag_len < NV_TAG_LENGTH) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    errcode_t ret = uapi_drv_cipher_symc_get_tag(crypto_handle, tag, NV_TAG_LENGTH);
    if (ret != ERRCODE_SUCC) {
        *tag_len = 0;
        nv_log_err("[NV] get tag failed! ret = 0x%x\r\n", ret);
        return ret;
    }
    *tag_len = NV_TAG_LENGTH;
    return ret;
}

/* 设置当前NV的Tag */
errcode_t nv_crypto_set_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t tag_len)
{
    if (crypto_handle == INVALID_HANDLE || tag == NULL || tag_len < NV_TAG_LENGTH) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    errno_t ret =  memcpy_s(g_nv_encrypt_tag, NV_TAG_LENGTH, tag, NV_TAG_LENGTH);
    nv_chk_return(ret != EOK, ERRCODE_FAIL, "[NV] set tag failed!\r\n");

    return ERRCODE_SUCC;
}

/* 校验Tag */
errcode_t nv_crypto_validate_tag(uint32_t crypto_handle)
{
    uint8_t decrypt_tag[NV_TAG_LENGTH] = {0};
    errcode_t ret;
    
    ret = uapi_drv_cipher_symc_get_tag(crypto_handle, decrypt_tag, NV_TAG_LENGTH);
    nv_chk_return(ret != ERRCODE_SUCC, ret, "[NV] get tag failed! ret = 0x%x\r\n", ret);

    int32_t res = memcmp(g_nv_encrypt_tag, decrypt_tag, NV_TAG_LENGTH);
    nv_chk_return(res != 0, ERRCODE_FAIL, "[NV] validate tag failed!\r\n");

    return ERRCODE_SUCC;
}

/* 获取随机数 */
void nv_crypto_generate_random(uint32_t *rnd)
{
    (void)uapi_drv_cipher_trng_get_random(rnd);
}
#endif /* #if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) */

/* 以下函数预留，可不实现 */
errcode_t nv_crypto_start_hash(uint32_t *handle)
{
    return uapi_drv_cipher_sha256_start(handle);
}

errcode_t nv_crypto_update_hash(uint32_t handle, const uint8_t *src, uint32_t length)
{
    return uapi_drv_cipher_sha256_update(handle, src, length);
}

errcode_t nv_crypto_complete_hash(uint32_t handle, uint8_t *hash, uint32_t *hash_len)
{
    return uapi_drv_cipher_sha256_finish(handle, hash, hash_len);
}
