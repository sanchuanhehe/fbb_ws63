/*
 * * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "malloc.h"
#include "hal_hota_board.h"
#include "upg_debug.h"
#include "upg.h"

#define PUBKEY_LENGTH 270

// 该数组用于保存OH升级校验公钥，仅用于XTS测试示例。产品开发中，由产品业务生成和保存公钥值，通过HotaHalGetPubKey()接口正确返回。
static const unsigned char g_pub_key_buf[PUBKEY_LENGTH] = {0};

typedef enum {
    PARTITION_OTA = 0,
    PARTITION_KERNEL_A = 1,
    PARTITION_MAX
} hota_partition_t;

static const ComponentTableInfo g_component_table[] = {
    { PARTITION_OTA, "ota_file", "", 0},
    { PARTITION_KERNEL_A, "kernel_A", "/sdcard/update/kernel.bin", 0},
    { PARTITION_MAX, NULL, NULL, 0}
};

typedef enum {
    UPG_STATUS_UNINIT = 0,
    UPG_STATUS_INIT = 1,
    UPG_STATUS_START = 2,
    UPG_STATUS_FINISH = 3,
    UPG_STATUS_MAX
} upg_proc_status_t;
static uint8_t g_upg_status = UPG_STATUS_UNINIT;

errcode_t upg_init(void)
{
    errcode_t ret;
    upg_func_t upg_func = {0};
    upg_func.malloc = malloc;
    upg_func.free = free;
    upg_func.serial_putc = NULL;
    ret = uapi_upg_init(&upg_func);
    if (ret != ERRCODE_SUCC && ret != ERRCODE_UPG_ALREADY_INIT) {
        upg_log_err("[UPG] upgrade init failed!\r\n");
        return ret;
    }
    upg_log_info("[UPG] upgrade init OK!\r\n");
    return ERRCODE_SUCC;
}

int HotaHalInit(void)
{
    errcode_t res = upg_init();
    if (res != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }
    g_upg_status = UPG_STATUS_INIT;
    upg_prepare_info_t info = {0};
    uapi_upg_prepare(&info);
    return OHOS_SUCCESS;
}

int HotaHalGetUpdateIndex(unsigned int *index)
{
#ifndef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
    *index = PARTITION_OTA;
    return OHOS_SUCCESS;
#endif
}

int HotaHalDeInit(void)
{
    if (g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRead(int partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer)
{
    if (g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    if (uapi_upg_read_package(offset, buffer, bufLen) != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int HotaHalWrite(int partition, unsigned char *buffer, unsigned int offset, unsigned int buffLen)
{
    if (partition != PARTITION_OTA || g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    unsigned int result = uapi_upg_write_package_sync(offset, buffer, buffLen);
    if (result != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRestart(void)
{
    upg_reboot();
    return OHOS_SUCCESS;
}

int HotaHalSetBootSettings(void)
{
    return OHOS_SUCCESS;
}
/* no support */
int HotaHalRollback(void)
{
    return OHOS_SUCCESS;
}

const ComponentTableInfo *HotaHalGetPartitionInfo(void)
{
    return g_component_table;
}

unsigned char *HotaHalGetPubKey(unsigned int *length)
{
    if (length == NULL) {
        return NULL;
    }

    *length = sizeof(g_pub_key_buf);
    return (unsigned char *)g_pub_key_buf;
}


int HotaHalGetUpdateAbility(void)
{
    return 0;
}

/* no support */
int HotaHalGetOtaPkgPath(char *path, int len)
{
    (void)path;
    (void)len;
    return OHOS_SUCCESS;
}

/* support reboot when update locally */
int HotaHalIsDeviceCanReboot(void)
{
    return 1;
}

/* no support */
int HotaHalGetMetaData(UpdateMetaData *metaData)
{
    (void)metaData;
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalSetMetaData(UpdateMetaData *metaData)
{
    (void)metaData;
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalRebootAndCleanUserData(void)
{
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalRebootAndCleanCache(void)
{
    return OHOS_SUCCESS;
}

int HotaHalCheckVersionValid(const char *currentVersion, const char *pkgVersion, unsigned int pkgVersionLength)
{
    return (strncmp(currentVersion, pkgVersion, pkgVersionLength) == 0) ? 1 : 0;
}
