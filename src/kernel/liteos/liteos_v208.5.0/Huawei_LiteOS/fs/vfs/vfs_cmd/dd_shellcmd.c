/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: implementation for dd commands.
 * Author: Huawei LiteOS Team
 * Create: 2021-03-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifdef LOSCFG_SHELL
#include "unistd.h"
#include "inode/inode.h"
#include "los_typedef.h"
#include "shell.h"
#include "shcmd.h"

#define LOSCFG_DD_DEFAULT_BS 1024
#define LOSCFG_DD_DEFAULT_CNT 1
#define LOSCFG_DD_TASK_PRIORITY 10

typedef struct {
    CHAR filepath[PATH_MAX];
    size_t mode;
    UINT32 blockSize;
    UINT32 blockCount;
} DdCmdAttr;

enum DdMode {
    MODE_READ = 1,
    MODE_WRITE,
    MODE_MAX,
};

typedef struct {
    const CHAR *translate;
    INT32 oFlag;
    INT32 sFlag;
} ModeTable;

#define DD_USAGE() do { \
    PRINTK("dd file=[FILEPATH] mode=[1:READ/2:WRITE] bs=[SIZE] count=[N]\r\n"); \
} while (0)

typedef ssize_t (*READ_WRITE_FUNC)(INT32 fd, FAR VOID *buf, size_t nbytes);

STATIC ModeTable g_modeTbl[] = {
    [MODE_READ]     = {"read", O_RDONLY, S_IRUSR | S_IWUSR},
    [MODE_WRITE]    = {"write", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR},
    [MODE_MAX]      = {"not support", 0, 0},
};

STATIC INLINE UINT64 OsDdGetCurrTime(VOID)
{
    return HalClockGetCycles();
}

STATIC INLINE VOID OsDdPrintRate(UINT64 startTime, const CHAR *opt, UINT64 size)
{
    DOUBLE time = (OsDdGetCurrTime() - startTime) * 1.0 / OS_SYS_CLOCK;
    DOUBLE mb = size * 1.0 / (1024 * 1024);
    DOUBLE gb = mb / 1024;
    DOUBLE speed = mb / time;
    PRINTK(" total %llu bytes (%0.2lf GB, %0.2lfMB) %s, time used: %.6f(s), %0.2lfMB/S\r\n",
        size, gb, mb, opt, time, speed);
}

STATIC VOID OsParamsDump(const DdCmdAttr *attr)
{
    PRINTK("---------------------------\r\n");
    PRINTK("filepath   : %s\r\n", attr->filepath);
    PRINTK("mode       : %s\r\n", g_modeTbl[attr->mode].translate);
    PRINTK("blockSize  : %u\r\n", attr->blockSize);
    PRINTK("blockCount : %u\r\n", attr->blockCount);
    PRINTK("---------------------------\r\n");
}

STATIC BOOL OsParamsValid(DdCmdAttr *attr)
{
    if ((attr->filepath == NULL) || (*attr->filepath == '\0')) {
        PRINT_ERR("filename must not empty.\n");
        return FALSE;
    }

    if ((attr->mode >= MODE_MAX) || (attr->mode < MODE_READ)) {
        PRINT_ERR("mode must be 1(read) or 2(write).\n");
        return FALSE;
    }

    if (attr->blockSize == 0) {
        attr->blockSize = LOSCFG_DD_DEFAULT_BS;
        PRINT_INFO("bs not configured, used default %d.\n", LOSCFG_DD_DEFAULT_BS);
    }

    if (attr->blockCount == 0) {
        attr->blockCount = LOSCFG_DD_DEFAULT_CNT;
        PRINT_INFO("count not configured, used default %d.\n", LOSCFG_DD_DEFAULT_CNT);
    }

    return TRUE;
}

STATIC UINT32 OsParseOpt(INT32 argc, const CHAR **argv, DdCmdAttr *attr)
{
#define OPTION_MATCH(d, s) strncmp((d), (s), strlen(s))

    INT32 i;
    CHAR *endPtr = NULL;
    for (i = 0; i < argc; i++) {
        CHAR const *name = argv[i];
        CHAR const *value = strchr(name, '=');
        if (value == NULL) {
            return LOS_NOK;
        }

        value++;
        PRINT_DEBUG("[%d]:[%s][%s]\n", i, name, value);

        if (OPTION_MATCH(name, "file") == 0) {
            if (snprintf_s(attr->filepath, PATH_MAX, PATH_MAX - 1, "%s", value) < 0) {
                PRINT_ERR("filepath error\n");
                return LOS_NOK;
            }
        } else if (OPTION_MATCH(name, "mode") == 0) {
            attr->mode = strtoul(value, &endPtr, 0);
        } else if (OPTION_MATCH(name, "bs") == 0) {
            attr->blockSize = strtoul(value, &endPtr, 0);
        } else if (OPTION_MATCH(name, "count") == 0) {
            attr->blockCount = strtoul(value, &endPtr, 0);
        } else {
            PRINT_ERR("invalid opt %d!\n", i);
            return LOS_NOK;
        }
    }

    if (!OsParamsValid(attr)) {
        return LOS_NOK;
    }

    OsParamsDump(attr);
    return LOS_OK;
}

STATIC INLINE ssize_t OsRead(INT32 fd, FAR VOID *buf, size_t nbytes)
{
    return read(fd, buf, nbytes);
}

STATIC INLINE ssize_t OsWrite(INT32 fd, FAR VOID *buf, size_t nbytes)
{
    return write(fd, buf, nbytes);
}

STATIC ssize_t OsReadWriteFile(INT32 fd, UINT32 bs, UINT32 c, BOOL r)
{
    ssize_t nBytes;
    UINT32 blockSize = bs;
    UINT32 count = c;
    ssize_t totalSize = 0;
    READ_WRITE_FUNC callback = NULL;

    CHAR *buf = malloc(blockSize);
    if (buf == NULL) {
        PRINT_ERR("memory not enough for buffer size 0x%x\n", blockSize);
        return 0;
    }

    (VOID)memset_s(buf, blockSize, 0, blockSize);

    if (r) {
        callback = OsRead;
    } else {
        callback = OsWrite;
    }

    do {
        nBytes = callback(fd, buf, blockSize);
        if (nBytes < 0) {
            PRINT_ERR("fail to operate: %s, bytes = %d\n", strerror(errno), nBytes);
            goto END;
        } else if ((UINT32)nBytes != blockSize) {
            totalSize += nBytes;
            goto END;
        }
        totalSize += nBytes;
    } while (count-- > 1);
END:
    free(buf);
    return totalSize;
}

VOID OsDdProcess(const VOID *arg)
{
    INT32 ret;
    CHAR *fullPath = NULL;
    INT32 fd = -1;
    UINT64 totalSize = 0;
    UINT64 startTime = 0;
    DdCmdAttr *attr = (DdCmdAttr *)arg;

    CHAR *shellWorkingDirt = OsShellGetWorkingDirectory();
    if (shellWorkingDirt == NULL) {
        PRINT_ERR("dd process failed, get work dir is null.\n");
        goto RELEASE_ATTR;
    }

    PRINT_DEBUG("#tid = %u\n", LOS_CurTaskIDGet());

    ret = vfs_normalize_path(shellWorkingDirt, attr->filepath, &fullPath);
    if (ret < 0) {
        PRINT_ERR("dd process path failed %d.\n", -ret);
        goto RELEASE_ATTR;
    }

    fd = open(fullPath, g_modeTbl[attr->mode].oFlag, g_modeTbl[attr->mode].sFlag);
    if (fd < 0) {
        PRINT_ERR("dd process open %s failed, %d, %s.\n", fullPath, errno, strerror(errno));
        goto RELEASE_PATH;
    }

    startTime = OsDdGetCurrTime();

    if (attr->mode == MODE_READ) {
        totalSize += OsReadWriteFile(fd, attr->blockSize, attr->blockCount, TRUE);
    } else if (attr->mode == MODE_WRITE) {
        totalSize += OsReadWriteFile(fd, attr->blockSize, attr->blockCount, FALSE);
    }

    ret = close(fd);
    if (ret < 0) {
        PRINT_ERR("file close failed, %d, %s.\n", errno, strerror(errno));
    }

    OsDdPrintRate(startTime, g_modeTbl[attr->mode].translate, totalSize);
RELEASE_PATH:
    free(fullPath);
RELEASE_ATTR:
    free(attr);
}

STATIC UINT32 OsCreateDdTask(const DdCmdAttr *attr)
{
    UINT32 ret;
    UINT32 tid;
    TSK_INIT_PARAM_S taskInitParam;

    (VOID)memset_s((VOID *)(&taskInitParam), sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsDdProcess;
    LOS_TASK_PARAM_INIT_ARG(taskInitParam, attr);
    taskInitParam.usTaskPrio = LOSCFG_DD_TASK_PRIORITY;
    taskInitParam.uwResved = LOS_TASK_STATUS_DETACHED;
    taskInitParam.pcName = "DdAgent";
    taskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
#ifdef LOSCFG_KERNEL_SMP
    taskInitParam.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif
    ret = LOS_TaskCreate(&tid, &taskInitParam);
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsShellCmdDd(INT32 argc, const CHAR **argv)
{
    UINT32 ret;
    DdCmdAttr *ddCmd = NULL;
    if ((argc < 2) || (argc > 4)) {
        PRINTK("dd need at least 2 params to set filename and operate mode to run, and less than 4 params.\n");
        DD_USAGE();
        return LOS_NOK;
    }

    ddCmd = malloc(sizeof(DdCmdAttr));
    if (ddCmd == NULL) {
        PRINTK("no memory to used for dd\n");
        return LOS_NOK;
    }

    (VOID)memset_s(ddCmd, sizeof(DdCmdAttr), 0, sizeof(DdCmdAttr));
    ret = OsParseOpt(argc, argv, ddCmd);
    if (ret != LOS_OK) {
        PRINTK("opt parse error!\n");
        goto ERROR_PARSE;
    }

    ret = OsCreateDdTask(ddCmd);
    if (ret != LOS_OK) {
        PRINT_ERR("dd task create failed 0x%x\n", ret);
        goto ERROR_RELEASE_ATTR;
    }
    return LOS_OK;

ERROR_PARSE:
    DD_USAGE();
ERROR_RELEASE_ATTR:
    free(ddCmd);
    return LOS_NOK;
}

SHELLCMD_ENTRY(dd_shellcmd, CMD_TYPE_EX, "dd", XARGS, (CmdCallBackFunc)OsShellCmdDd);

#endif
